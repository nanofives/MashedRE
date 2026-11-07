0x004b6480  89  100.0  100.0  cxx:ok  -  inlined memset + mask loop with separate shift counter
0x004b64e0  58  100.0  100.0  cxx:ok  -  inlined memset, int fill, unsigned count > 0
0x004b6540  5  100.0  100.0  cxx:ok  -  jmp thunk to 0x004b6640
0x004b6550  5  100.0  100.0  cxx:ok  -  jmp thunk to 0x004b6700
0x004b6560  5  100.0  100.0  cxx:ok  -  jmp thunk to 0x004b6610
0x004b6590  5  100.0  100.0  cxx:ok  -  jmp thunk to 0x004b67a0
0x004b65a0  5  100.0  100.0  cxx:ok  -  jmp thunk to 0x004b68e0
0x004b65b0  5  100.0  100.0  cxx:ok  -  jmp thunk to 0x004b68f0
0x004b65c0  25  100.0  100.0  cxx:ok  -  struct copy from DAT_008ab7e0
0x004b6640  180  60.5  88.4  cxx:ok  b  p in EDX then copied to ESI for the last call, 9176 puts it in ESI from the start
0x004b6710  87  100.0  100.0  cxx:ok  -  static callee (name in EAX) with a stand-in caller
0x004b6770  48  100.0  100.0  cxx:ok  -  fclose by address else CloseHandle
0x004b67e0  241  100.0  100.0  cxx:ok  -  memset OVERLAPPED then Offset store, for(;;) poll loop
0x004b68f0  66  100.0  100.0  cxx:ok  -  e = &table[i] in the loop body
0x004b6b00  3  100.0  100.0  cxx:ok  -  static callee (ECX dst, EAX value) with a stand-in caller
0x004b6b90  7  100.0  100.0  cxx:ok  -  static callee (EAX, ECX) with a stand-in caller
0x004b6b20  110  71.4  91.4  cxx:ok  b  static callee (EAX/EBX/ECX) with stand-in caller, byte-count register choice after the read
0x004b71c0  52  54.2  66.7  cxx:ok  a  repne scasb strlen, not emitted by 9176 (Lua TU, other compiler)
0x004b55a0  421  83.7  92.7  cxx:ok  b  RwGlobals pointer in EAX vs ECX through the colour block
0x004b6940  448  100.0  100.0  cxx:ok  -  piz open, holds static 0x004b6710

## Final list (h4)
0x004b6480  89  100.0  100.0  cxx:ok  -  inlined memset + mask loop with a separate shift counter
0x004b64e0  58  100.0  100.0  cxx:ok  -  inlined memset, int fill, unsigned count > 0
0x004b6540  5  100.0  100.0  cxx:ok  -  jmp thunk to 0x004b6640
0x004b6550  5  100.0  100.0  cxx:ok  -  jmp thunk to 0x004b6700
0x004b6560  5  100.0  100.0  cxx:ok  -  jmp thunk to 0x004b6610
0x004b6590  5  100.0  100.0  cxx:ok  -  jmp thunk to 0x004b67a0
0x004b65a0  5  100.0  100.0  cxx:ok  -  jmp thunk to 0x004b68e0
0x004b65b0  5  100.0  100.0  cxx:ok  -  jmp thunk to 0x004b68f0
0x004b65c0  25  100.0  100.0  cxx:ok  -  struct copy from DAT_008ab7e0
0x004b6640  180  60.5  88.4  cxx:ok  b  p in EDX then copied to ESI for the last call, 9176 keeps it in ESI from the start
0x004b6710  87  100.0  100.0  cxx:ok  -  static callee (name in EAX) with a stand-in caller
0x004b6770  48  100.0  100.0  cxx:ok  -  fclose by address else CloseHandle
0x004b67e0  241  100.0  100.0  cxx:ok  -  memset OVERLAPPED then Offset store, for(;;) poll loop
0x004b68f0  66  100.0  100.0  cxx:ok  -  e = &table[i] in the loop body
0x004b6b00  3  100.0  100.0  cxx:ok  -  static callee (ECX dst, EAX value) with a stand-in caller
0x004b6b20  110  71.4  91.4  cxx:ok  b  static callee (EAX/EBX/ECX) with a stand-in caller, byte-count register after the read
0x004b6b90  7  100.0  100.0  cxx:ok  -  static callee (EAX, ECX) with a stand-in caller
0x004b71c0  52  54.2  66.7  cxx:ok  a  repne scasb strlen, 9176 never emits it (Lua TU, other compiler), flags /O2 /Ob1 /G5
0x004b55a0  421  83.7  92.7  cxx:ok  b  RwGlobals pointer in EAX instead of ECX through the colour block
0x004b6940  448  100.0  100.0  cxx:ok  -  piz open
0x004b6940 holds 0x004b6710
