#pragma once
#include "retcheck.h"
#include "utilities.h"

extern "C" {
	// lua core
#include "Lua/lua.h"
#include "Lua/lauxlib.h"
#include "Lua/lualib.h"
	// lua internals
#include "Lua/lobject.h"
#include "Lua/lstate.h"
#include "Lua/lfunc.h"
#include "Lua/lopcodes.h"
	// lparser.c modified for roblox's op_closure change
}

static int rl; //global state
static int script_context;
static DWORD encode_key;

#define LOCALSCRIPTGLOBALSTATE	164

/* rebasing addresses for memory arrangement randomization (aslr) */
#define IDABASE 0x400000
#define ROBLOXBASE (int)GetModuleHandle(NULL)
#define REBASE_ADDRESS(x) (ROBLOXBASE + x - IDABASE)

namespace addresses
{
	static int scriptcontext = REBASE_ADDRESS(0x1008714);
	static int xorconst = REBASE_ADDRESS(0x147E6F0);
	static int luaF_newproto = REBASE_ADDRESS(0x71B140);
	static int luaM_realloc_ = REBASE_ADDRESS(0x71CC20);
	static int luaS_newlstr = REBASE_ADDRESS(0x71D2B0);
	static int luaF_newLclosure = REBASE_ADDRESS(0x71B0C0);
	static int luaF_newupval = REBASE_ADDRESS(0x71B210);
	static int sandbox_thread = REBASE_ADDRESS(0x6AD1E0);
	static int index2adr = REBASE_ADDRESS(0x719080);
	static int lua_settop = REBASE_ADDRESS(0x723320);
	static int spawn = REBASE_ADDRESS(0x6AD3A0);
	static int lua_newthread = REBASE_ADDRESS(0x721CF0);
}

/* roblox lua structs */

#define RL_TOP     32
#define RL_BASE     20
#define RL_L_GT     104
#define RL_L_G     12
#define RL_L_G_CKEY    56

union r_value {
	PVOID gc;
	PVOID p;
	double n;
	int b;
};

struct r_tvalue {
	r_value value;
	int tt;
};

struct r_proto {
	CommonHeader;    // +00
	unsigned char Unk0;   // +07
	int sizek;     // +08
	int* source;    // +12
	r_proto* p;     // +16
	int* locvars;    // +20
	r_tvalue* k;     // +24
	int* upvalues;    // +28
	Instruction* code;   // +32
	int sizeupvalues;   // +36
	int* lineinfo;    // +40
	unsigned char Unk1[4];  // +44
	int sizep;     // +48
	int lastlinedefined;  // +52
	int sizelocvars;   // +56
	int linedefined;   // +60
	int sizecode;    // +64
	unsigned char Unk2[4];  // +68
	int sizelineinfo;   // +72
	byte is_vararg;    // +76
	byte nups;     // +77
	byte maxstacksize;   // +78
	byte numparams;    // +79
};

struct rl_closure {
	unsigned char unk[15];		// +00
	r_proto* p;					// +16
	int upvals[1];				// +20
};

/* memory call information */
typedef r_proto*(__cdecl* rluaF_newproto_def)(int);
typedef VOID*(__cdecl* rluaM_realloc__def)(int, int, int, int, int);
typedef int*(__cdecl* rluaS_newlstr_def)(int, const char*, int);
typedef rl_closure*(__cdecl* rluaF_newLclosure_def)(int, int, int);
typedef int(__cdecl* rluaF_newupval_def)(int);
typedef int(__cdecl* rSandboxThread_def)(int, int, int);
typedef DWORD(__cdecl* r_index2adr_def)(DWORD, DWORD);
typedef void(__cdecl* r_lua_settop_def)(int, int);
typedef void(__cdecl* r_spawn_def)(int);
typedef int(__cdecl* r_lua_newthread_def)(int);

static r_spawn_def r_spawn = (r_spawn_def)addresses::spawn;
static r_lua_settop_def r_lua_settop = (r_lua_settop_def)Retcheck::unprotect((byte*)(addresses::lua_settop));
static r_index2adr_def r_lua_index2adr = (r_index2adr_def)Retcheck::unprotect((byte*)(addresses::index2adr));
static rluaF_newproto_def rluaF_newproto = (rluaF_newproto_def)addresses::luaF_newproto;
static rluaM_realloc__def rluaM_realloc_ = (rluaM_realloc__def)addresses::luaM_realloc_;
static rluaS_newlstr_def rluaS_newlstr = (rluaS_newlstr_def)addresses::luaS_newlstr;
static rluaF_newLclosure_def rluaF_newLclosure = (rluaF_newLclosure_def)addresses::luaF_newLclosure;
static rluaF_newupval_def rluaF_newupval = (rluaF_newupval_def)addresses::luaF_newupval;
static rSandboxThread_def r_sandbox_thread = (rSandboxThread_def)addresses::sandbox_thread;
static r_lua_newthread_def r_lua_newthread = (r_lua_newthread_def)Retcheck::unprotect((byte*)(addresses::lua_newthread));

/* roblox lua macros */
#define RLUAM_MALLOC(rl, t) rluaM_realloc_(rl, 0, 0, (t), 4)
#define RLUAS_NEW(rl, str) rluaS_newlstr(rl, str, strlen(str))
#define R_API_INCR_TOP(rl) *(int*)(rl + RL_TOP) += sizeof(r_tvalue)
#define R_LUA_POP(rl, n) r_lua_settop(rl, -(n)-1)
#define R_CLEAR_STACK(rl) r_lua_settop(rl, 0)
#define RLUA_GETTOP(rl) (*(int*)(rl + RL_TOP) - *(int*)(rl + RL_BASE)) >> 4

#define RLUA_TNONE    -1
#define RLUA_TNIL    0
#define RLUA_TLIGHTUSERDATA  1
#define RLUA_TNUMBER   2
#define RLUA_TBOOLEAN   3
#define RLUA_TSTRING   4
#define RLUA_TTHREAD   5
#define RLUA_TFUNCTION   6
#define RLUA_TTABLE    7
#define RLUA_TUSERDATA   8
#define RLUA_TPROTO    9
#define RLUA_TUPVAL    10

static VOID rlua_pushlclosure(int rl, rl_closure* rlcl);
static double r_xor_number(double* number);
VOID set_encode_key(int rl, DWORD* key);