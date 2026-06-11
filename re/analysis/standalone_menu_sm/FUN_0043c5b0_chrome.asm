0x0043c647: 8b15f83f7d00             mov edx, dword ptr [0x7d3ff8]
0x0043c64d: 6a00                     push 0
0x0043c64f: 6a06                     push 6
0x0043c651: ff5220                   call dword ptr [edx + 0x20]
0x0043c654: a1f83f7d00               mov eax, dword ptr [0x7d3ff8]
0x0043c659: 6a01                     push 1
0x0043c65b: 6a08                     push 8
0x0043c65d: ff5020                   call dword ptr [eax + 0x20]
0x0043c660: a1a4ec6700               mov eax, dword ptr [0x67eca4]
0x0043c665: 83c410                   add esp, 0x10
0x0043c668: 83f805                   cmp eax, 5
0x0043c66b: 0f8d8a000000             jge 0x43c6fb
0x0043c671: 6a00                     push 0
0x0043c673: e8381fffff               call 0x42e5b0
0x0043c678: 83c404                   add esp, 4
0x0043c67b: e8201dffff               call 0x42e3a0
0x0043c680: e8abf2feff               call 0x42b930
0x0043c685: 83f821                   cmp eax, 0x21
0x0043c688: 7471                     je 0x43c6fb
0x0043c68a: 8b4c2408                 mov ecx, dword ptr [esp + 8]
0x0043c68e: 6a01                     push 1
0x0043c690: 68cdcc4c3f               push 0x3f4ccccd
0x0043c695: 51                       push ecx
0x0043c696: 6800005042               push 0x42500000
0x0043c69b: 6800001644               push 0x44160000
0x0043c6a0: 6a41                     push 0x41
0x0043c6a2: c744243000003843         mov dword ptr [esp + 0x30], 0x43380000
0x0043c6aa: c744243800000043         mov dword ptr [esp + 0x38], 0x43000000
0x0043c6b2: c744243400000000         mov dword ptr [esp + 0x34], 0
0x0043c6ba: c744243c00009842         mov dword ptr [esp + 0x3c], 0x42980000
0x0043c6c2: e839b7feff               call 0x427e00
0x0043c6c7: 6a01                     push 1
0x0043c6c9: 68cdcc4c3f               push 0x3f4ccccd
0x0043c6ce: c6442420ff               mov byte ptr [esp + 0x20], 0xff
0x0043c6d3: c6442421ff               mov byte ptr [esp + 0x21], 0xff
0x0043c6d8: c6442422ff               mov byte ptr [esp + 0x22], 0xff
0x0043c6dd: c6442423ff               mov byte ptr [esp + 0x23], 0xff
0x0043c6e2: 8b542420                 mov edx, dword ptr [esp + 0x20]
0x0043c6e6: 52                       push edx
0x0043c6e7: 6800004042               push 0x42400000
0x0043c6ec: 6800001544               push 0x44150000
0x0043c6f1: 6a41                     push 0x41
0x0043c6f3: e808b7feff               call 0x427e00
0x0043c6f8: 83c430                   add esp, 0x30
0x0043c6fb: 6a01                     push 1
0x0043c6fd: e8dee3feff               call 0x42aae0
0x0043c702: a1a4ec6700               mov eax, dword ptr [0x67eca4]
0x0043c707: 83c404                   add esp, 4
0x0043c70a: 83f804                   cmp eax, 4
0x0043c70d: 0f8d620b0000             jge 0x43d275
0x0043c713: 53                       push ebx
0x0043c714: 55                       push ebp
0x0043c715: 56                       push esi
0x0043c716: 57                       push edi
0x0043c717: c744241c00000000         mov dword ptr [esp + 0x1c], 0
0x0043c71f: beec8a8900               mov esi, 0x898aec
0x0043c724: eb0a                     jmp 0x43c730
0x0043c726: 8da42400000000           lea esp, [esp]
0x0043c72d: 8d4900                   lea ecx, [ecx]
0x0043c730: 8b46d4                   mov eax, dword ptr [esi - 0x2c]
0x0043c733: 3d000011ff               cmp eax, 0xff110000
0x0043c738: 0f8f2e0a0000             jg 0x43d16c
0x0043c73e: 0f84410a0000             je 0x43d185
0x0043c744: 3d000000ff               cmp eax, 0xff000000
0x0043c749: 0f84360a0000             je 0x43d185
0x0043c74f: 3d000004ff               cmp eax, 0xff040000
0x0043c754: 7410                     je 0x43c766
0x0043c756: 3d000010ff               cmp eax, 0xff100000
0x0043c75b: 0f84240a0000             je 0x43d185
0x0043c761: e9f70a0000               jmp 0x43d25d
0x0043c766: d946f0                   fld dword ptr [esi - 0x10]
0x0043c769: 8b2df8e96700             mov ebp, dword ptr [0x67e9f8]
0x0043c76f: d825d8d85c00             fsub dword ptr [0x5cd8d8]
0x0043c775: 8b7c241c                 mov edi, dword ptr [esp + 0x1c]
0x0043c779: 8bc5                     mov eax, ebp
0x0043c77b: c1e006                   shl eax, 6
