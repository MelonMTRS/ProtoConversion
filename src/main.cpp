/* ProtoConversion
 * idea of way created by Brandon (Chirality)
 * when script execution happens via roblox, the client (you) recieves the bytecode
 * from the server, roblox will then deserialize it, this means it will convert the bytecode into a proto struct
 * in order to accomplish script execution what you can do is create a vanilla lua function and convert it to roblox's format and therefore
 * feed it to the lua vm and run it
 * just like bytecode conversion, the thing pushed onto the stack is a proto that is put into a function

 * written by azulx and quad core 2018
 * this is the final roblox related thing that we both are releasing, this uses the code that Script-Ware uses
 */

#include "conversion.h"
#include "memscan.h"
#include <iostream>

void init()
{
	begin_console("ProtoConversion!");

	l = lua_open(); //initialize the vanilla lua state

	int vf_table = addresses::scriptcontext;
	script_context = memory::scan(PAGE_READWRITE, (char*)&vf_table, (char*)"xxxx");

	rl = (script_context + LOCALSCRIPTGLOBALSTATE) - *(int*)(script_context + LOCALSCRIPTGLOBALSTATE); //declare the globalstate

	while (true)
	{
		std::cout << ">";

		std::string input;
		std::getline(std::cin, input);
		std::vector<std::string> arguments = split_string(input, ' ');

		if (arguments.at(0) == "credits")
		{
			std::cout << "script execution shit, made by azulx and quad core |||| using elysian enc, we dont make new things! non-innovation is key here at roblox shitsploiting!\n";
		}

		else if (!arguments.at(0).empty())
		{
			execute_script(rl, l, input);
		}
	}
}

BOOL WINAPI DllMain(HMODULE dll, DWORD reason, PVOID reserved) {
	if (reason == DLL_PROCESS_ATTACH) {
		DisableThreadLibraryCalls(dll);
		CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)init, NULL, NULL, NULL);
	}
	return TRUE;
}