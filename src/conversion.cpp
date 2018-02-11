#include "conversion.h"
#include "encryption.h"

namespace lua_to_roblox
{
	VOID convert_proto(int rl, Proto* p, r_proto* rp, int* rpnups) { // same order as readProto
		/* main conversion of vanilla proto to roblox proto */

		R_PROTO_OBFUSCATE(&rp->source, RLUAS_NEW(rl, getstr(p->source)));

		rp->sizep = p->sizep;
		auto** rpp_mem = (r_proto**)RLUAM_MALLOC(rl, sizeof(r_proto**) * p->sizep);
		R_PROTO_OBFUSCATE(&rp->p, rpp_mem);

		for (int k = 0; k < p->sizep; k++) {
			rpp_mem[k] = rluaF_newproto(rl);
			convert_proto(rl, p->p[k], rpp_mem[k], rpnups);
		}

		rp->sizek = p->sizek;
		auto* rp_k_mem = (r_tvalue*)RLUAM_MALLOC(rl, sizeof(r_tvalue) * p->sizek);
		R_PROTO_OBFUSCATE(&rp->k, rp_k_mem);

		for (int k = 0; k < p->sizek; k++) {
			TValue* o = &p->k[k];
			r_tvalue* ro = &rp_k_mem[k];
			switch (o->tt) {
			case LUA_TNIL:
				ro->tt = RLUA_TNIL;
				ro->value.n = 0;
				break;
			case LUA_TBOOLEAN:
				ro->tt = RLUA_TBOOLEAN;
				ro->value.b = o->value.b;
				break;
			case LUA_TNUMBER:
				ro->tt = RLUA_TNUMBER;
				ro->value.n = r_xor_number(&o->value.n);
				break;
			case LUA_TSTRING:
				ro->tt = RLUA_TSTRING;
				ro->value.gc = RLUAS_NEW(rl, getstr((TString*)o->value.gc));
				break;
			default:
				break;
			}
		}

		rp->sizecode = p->sizecode;
		auto* rp_code_mem = (int*)RLUAM_MALLOC(rl, sizeof(int*) * p->sizecode);
		R_PROTO_OBFUSCATE(&rp->code, rp_code_mem);

		for (int k = 0; k < p->sizecode; k++) {
			/* instruction conversion */

			Instruction inst = p->code[k]; //vanilla instruction
			int r_inst = 0;
			OpCode op = GET_OPCODE(inst);

			RSET_OPCODE(r_inst, get_roblox_opcode[op]);

			switch (getOpMode(op)) {
			case iABC:
				RSETARG_A(r_inst, GETARG_A(inst));
				RSETARG_B(r_inst, GETARG_B(inst));
				RSETARG_C(r_inst, GETARG_C(inst));
				break;
			case iABx:
				RSETARG_A(r_inst, GETARG_A(inst));
				RSETARG_Bx(r_inst, GETARG_Bx(inst));
				break;
			case iAsBx:
				RSETARG_A(r_inst, GETARG_A(inst));
				RSETARG_sBx(r_inst, GETARG_sBx(inst));
				break;
			}

			/* lol im not doing that hench enc for you do it yourself */
			switch (op) {
			case OP_JMP:
				//you have a dilemma!!!
				break;
			case OP_CALL:
				//you have a dilemma!!!
				break;
			case OP_TAILCALL:
			case OP_RETURN:
				r_inst = dax_encode_op(r_inst, k, 1470882913, k - 1577854801, 641680189) & 0x3FFFFFF; //you can do return atleast!!!
				break;
			case OP_CLOSURE:
				//you have a dilemma!!!
				break;
			case OP_SETUPVAL:
				//you have a dilemma!!!
			case OP_MOVE:
				r_inst = r_inst & 0xFFFC21FF | 0x2000;
				break;
			}

			rp_code_mem[k] = r_inst * encode_key;
		}

		// sizelocvars set
		rp->sizelineinfo = p->sizelineinfo;
		auto* rp_lineinfo_mem = (int*)RLUAM_MALLOC(rl, sizeof(int*) * p->sizelineinfo);
		R_PROTO_OBFUSCATE(&rp->lineinfo, rp_lineinfo_mem);

		for (int k = 0; k < p->sizelineinfo; k++) {
			rp_lineinfo_mem[k] = p->lineinfo[k] ^ (k << 8);
		}

		// sizeupvalues set
		rp->maxstacksize = p->maxstacksize;
		rp->is_vararg = p->is_vararg;
		*rpnups += p->nups; rp->nups = p->nups;
		rp->numparams = p->numparams;
	}

	VOID set_l_closure(int rl, LClosure* lcl) {
		Proto* p = lcl->p;
		r_proto* rp = rluaF_newproto(rl);
		int rpnups = 0;

		convert_proto(rl, p, rp, &rpnups);
		rl_closure* rlcl = rluaF_newLclosure(rl, rpnups, *(int*)(rl + RL_L_GT));
		RL_CLOSURE_OBFUSCATE(&rlcl->p, rp);

		for (int k = 0; k < rpnups; k++) {
			*(int*)((int)&rlcl->upvals[k]) = rluaF_newupval(rl);
		}

		rlua_pushlclosure(rl, rlcl);
	}

	VOID call_l_closure(int rl, LClosure* lcl) {
		set_l_closure(rl, lcl);
		r_spawn(rl);
	}

	VOID execute_script(int rl, lua_State* l, std::string source) {
		int r_thread = r_lua_newthread(rl);
		set_encode_key(r_thread, &encode_key); 

		int unk[] = { NULL, NULL };
		r_sandbox_thread(r_thread, 6, (int)unk);

		if (luaL_loadstring(l, source.c_str())) {
			printf("Error: %s\n", lua_tostring(l, -1));
			lua_pop(l, 1);
		}

		else {
			pause();
			TValue* o = (l->top) - 1;
			LClosure* lcl = &clvalue(o)->l;
			call_l_closure(r_thread, lcl);
			lua_pop(l, 1);
			resume();
		}

		printf("\nlstack: %d\n", lua_gettop(l));
		printf("rstack: %d\n", RLUA_GETTOP(rl));
	}
}

void execute_script(int rl, lua_State* l, std::string source) {
	lua_to_roblox::execute_script(rl, l, source);
}