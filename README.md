# ProtoConversion
proto conversion is a widely used way of unrestricted Lua script execution in the roblox exploiting community
idea of proto conversion was discovered by Brandon (Chirality)

# How does it work?
When script execution occuts in Roblox, the client (you) recieves bytecode of Lua scripts from the server,
Roblox then does a thing called *deserializing* to this bytecode, in basic terms, this means the bytecode
is decompressed and converted into a proto struct.

So, with logic, the way we can accomplish script execution is creating a **vanilla** Lua function and convert it
to roblox's format, therefore we are liable to feed it to Roblox's Lua VM and get it ran.

Similar to bytecode conversion, the thing pushed onto the stack is a proto that is put into a function

# Is this Script-Ware?
Yes, this is essientally Script-Ware without the excess code and user interface and all of the other stuff.
This is plain out Script-Wares core.

# Am I allowed to use this?
More than welcome to take this and use it for your own private/commerical use, I do not care as long as valid credits
are stated.

# Does this fully work?
No, due to Roblox's addition of opcode encryption (which is not included in here) you need to solve the encryption and
place the solved encryption in lua_to_roblox::convert_proto(), I put a comment of "Instruction Conversion", theres no
way you can miss it.

# Credits
AzuLX & Quad Core - Writing this code
Brandon (Chirality) - Coming up with the idea and also his retcheck bypass
Louka - I believe begin_console() is Louka's code?
