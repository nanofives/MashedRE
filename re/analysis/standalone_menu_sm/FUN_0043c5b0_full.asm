0x0043c5b0: 83ec48                   sub esp, 0x48
0x0043c5b3: a1a4ec6700               mov eax, dword ptr [0x67eca4]
0x0043c5b8: 85c0                     test eax, eax
0x0043c5ba: c744241800000000         mov dword ptr [esp + 0x18], 0
0x0043c5c2: c744241c00000000         mov dword ptr [esp + 0x1c], 0
0x0043c5ca: c744242000c01f44         mov dword ptr [esp + 0x20], 0x441fc000
0x0043c5d2: c74424240080ef43         mov dword ptr [esp + 0x24], 0x43ef8000
0x0043c5da: c6442400ff               mov byte ptr [esp], 0xff
0x0043c5df: c6442401ff               mov byte ptr [esp + 1], 0xff
0x0043c5e4: c6442402ff               mov byte ptr [esp + 2], 0xff
0x0043c5e9: c6442403ff               mov byte ptr [esp + 3], 0xff
0x0043c5ee: c644240800               mov byte ptr [esp + 8], 0
0x0043c5f3: c644240900               mov byte ptr [esp + 9], 0
0x0043c5f8: c644240a00               mov byte ptr [esp + 0xa], 0
0x0043c5fd: c644240bff               mov byte ptr [esp + 0xb], 0xff
0x0043c602: c6442404e8               mov byte ptr [esp + 4], 0xe8
0x0043c607: c6442405d0               mov byte ptr [esp + 5], 0xd0
0x0043c60c: c6442406f8               mov byte ptr [esp + 6], 0xf8
0x0043c611: c644240740               mov byte ptr [esp + 7], 0x40
0x0043c616: 0f84760c0000             je 0x43d292
0x0043c61c: 83f801                   cmp eax, 1
0x0043c61f: 7526                     jne 0x43c647
0x0043c621: 50                       push eax
0x0043c622: e8b9e4feff               call 0x42aae0
0x0043c627: a1f83f7d00               mov eax, dword ptr [0x7d3ff8]
0x0043c62c: 6a01                     push 1
0x0043c62e: 6a06                     push 6
0x0043c630: ff5020                   call dword ptr [eax + 0x20]
0x0043c633: 8b0df83f7d00             mov ecx, dword ptr [0x7d3ff8]
0x0043c639: 6a01                     push 1
0x0043c63b: 6a08                     push 8
0x0043c63d: ff5120                   call dword ptr [ecx + 0x20]
0x0043c640: 83c414                   add esp, 0x14
0x0043c643: 83c448                   add esp, 0x48
0x0043c646: c3                       ret 
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
0x0043c77e: d80520c35c00             fadd dword ptr [0x5cc320]
0x0043c784: 8b8038ed6700             mov eax, dword ptr [eax + 0x67ed38]
0x0043c78a: 47                       inc edi
0x0043c78b: 3da0725f00               cmp eax, 0x5f72a0
0x0043c790: d9542444                 fst dword ptr [esp + 0x44]
0x0043c794: 897c241c                 mov dword ptr [esp + 0x1c], edi
0x0043c798: d95c242c                 fstp dword ptr [esp + 0x2c]
0x0043c79c: c6442413ff               mov byte ptr [esp + 0x13], 0xff
0x0043c7a1: c744242800007042         mov dword ptr [esp + 0x28], 0x42700000
0x0043c7a9: 7438                     je 0x43c7e3
0x0043c7ab: 3d70735f00               cmp eax, 0x5f7370
0x0043c7b0: 7431                     je 0x43c7e3
0x0043c7b2: 3da06d5f00               cmp eax, 0x5f6da0
0x0043c7b7: 742a                     je 0x43c7e3
0x0043c7b9: 3df86d5f00               cmp eax, 0x5f6df8
0x0043c7be: 7509                     jne 0x43c7c9
0x0043c7c0: a144e86700               mov eax, dword ptr [0x67e844]
0x0043c7c5: 85c0                     test eax, eax
0x0043c7c7: 751a                     jne 0x43c7e3
0x0043c7c9: c744243000005243         mov dword ptr [esp + 0x30], 0x43520000
0x0043c7d1: c744244000008743         mov dword ptr [esp + 0x40], 0x43870000
0x0043c7d9: c74424380000c842         mov dword ptr [esp + 0x38], 0x42c80000
0x0043c7e1: eb18                     jmp 0x43c7fb
0x0043c7e3: c744243000002643         mov dword ptr [esp + 0x30], 0x43260000
0x0043c7eb: c74424380000a842         mov dword ptr [esp + 0x38], 0x42a80000
0x0043c7f3: c744244000006243         mov dword ptr [esp + 0x40], 0x43620000
0x0043c7fb: 8b4ee4                   mov ecx, dword ptr [esi - 0x1c]
0x0043c7fe: 81f9ff010000             cmp ecx, 0x1ff
0x0043c804: c744243c0000d041         mov dword ptr [esp + 0x3c], 0x41d00000
0x0043c80c: c74424340000d041         mov dword ptr [esp + 0x34], 0x41d00000
0x0043c814: 7d35                     jge 0x43c84b
0x0043c816: b867666666               mov eax, 0x66666667
0x0043c81b: f7e9                     imul ecx
0x0043c81d: c1fa02                   sar edx, 2
0x0043c820: 8bca                     mov ecx, edx
0x0043c822: c1e91f                   shr ecx, 0x1f
0x0043c825: 03d1                     add edx, ecx
0x0043c827: 83fa0d                   cmp edx, 0xd
0x0043c82a: 89542424                 mov dword ptr [esp + 0x24], edx
0x0043c82e: 7d1b                     jge 0x43c84b
0x0043c830: db442424                 fild dword ptr [esp + 0x24]
0x0043c834: d946f0                   fld dword ptr [esi - 0x10]
0x0043c837: d8e1                     fsub st(1)
0x0043c839: d9542444                 fst dword ptr [esp + 0x44]
0x0043c83d: d95c242c                 fstp dword ptr [esp + 0x2c]
0x0043c841: dcc0                     fadd st(0), st(0)
0x0043c843: d954243c                 fst dword ptr [esp + 0x3c]
0x0043c847: d95c2434                 fstp dword ptr [esp + 0x34]
0x0043c84b: a114e96700               mov eax, dword ptr [0x67e914]
0x0043c850: 85c0                     test eax, eax
0x0043c852: 7407                     je 0x43c85b
0x0043c854: ba01000000               mov edx, 1
0x0043c859: eb11                     jmp 0x43c86c
0x0043c85b: 8b3d44e86700             mov edi, dword ptr [0x67e844]
0x0043c861: 33d2                     xor edx, edx
0x0043c863: 85ff                     test edi, edi
0x0043c865: 0f95c2                   setne dl
0x0043c868: 4a                       dec edx
0x0043c869: 83e202                   and edx, 2
0x0043c86c: 8b46d8                   mov eax, dword ptr [esi - 0x28]
0x0043c86f: 85c0                     test eax, eax
0x0043c871: 8b5c2440                 mov ebx, dword ptr [esp + 0x40]
0x0043c875: 7409                     je 0x43c880
0x0043c877: 83f802                   cmp eax, 2
0x0043c87a: 0f85f8030000             jne 0x43cc78
0x0043c880: 8b3e                     mov edi, dword ptr [esi]
0x0043c882: 83ffff                   cmp edi, -1
0x0043c885: 0f84d2090000             je 0x43d25d
0x0043c88b: 8bcd                     mov ecx, ebp
0x0043c88d: 2bca                     sub ecx, edx
0x0043c88f: 8b54241c                 mov edx, dword ptr [esp + 0x1c]
0x0043c893: 8bc1                     mov eax, ecx
0x0043c895: c1e004                   shl eax, 4
0x0043c898: 03c2                     add eax, edx
0x0043c89a: 8b2c8580ed6700           mov ebp, dword ptr [eax*4 + 0x67ed80]
0x0043c8a1: 85ed                     test ebp, ebp
0x0043c8a3: 7509                     jne 0x43c8ae
0x0043c8a5: 8d442410                 lea eax, [esp + 0x10]
0x0043c8a9: e822e2feff               call 0x42aad0
0x0043c8ae: 8b6c2430                 mov ebp, dword ptr [esp + 0x30]
0x0043c8b2: c1e106                   shl ecx, 6
0x0043c8b5: 8b8180ed6700             mov eax, dword ptr [ecx + 0x67ed80]
0x0043c8bb: 4a                       dec edx
0x0043c8bc: 3bd0                     cmp edx, eax
0x0043c8be: 0f8443010000             je 0x43ca07
0x0043c8c4: 8b4c2414                 mov ecx, dword ptr [esp + 0x14]
0x0043c8c8: 8b542434                 mov edx, dword ptr [esp + 0x34]
0x0043c8cc: 8b44242c                 mov eax, dword ptr [esp + 0x2c]
0x0043c8d0: 8b7c2428                 mov edi, dword ptr [esp + 0x28]
0x0043c8d4: 51                       push ecx
0x0043c8d5: 52                       push edx
0x0043c8d6: 55                       push ebp
0x0043c8d7: 50                       push eax
0x0043c8d8: 57                       push edi
0x0043c8d9: e882630300               call 0x472c60
0x0043c8de: 8b4c2428                 mov ecx, dword ptr [esp + 0x28]
0x0043c8e2: 8b542450                 mov edx, dword ptr [esp + 0x50]
0x0043c8e6: 8b44244c                 mov eax, dword ptr [esp + 0x4c]
0x0043c8ea: 51                       push ecx
0x0043c8eb: 8b4c245c                 mov ecx, dword ptr [esp + 0x5c]
0x0043c8ef: 52                       push edx
0x0043c8f0: 50                       push eax
0x0043c8f1: 51                       push ecx
0x0043c8f2: 53                       push ebx
0x0043c8f3: e8486c0300               call 0x473540
0x0043c8f8: d944245c                 fld dword ptr [esp + 0x5c]
0x0043c8fc: d81d74c55c00             fcomp dword ptr [0x5cc574]
0x0043c902: 83c428                   add esp, 0x28
0x0043c905: c644241013               mov byte ptr [esp + 0x10], 0x13
0x0043c90a: c644241115               mov byte ptr [esp + 0x11], 0x15
0x0043c90f: c644241250               mov byte ptr [esp + 0x12], 0x50
0x0043c914: dfe0                     fnstsw ax
0x0043c916: f6c441                   test ah, 0x41
0x0043c919: 0f85b5020000             jne 0x43cbd4
0x0043c91f: d944243c                 fld dword ptr [esp + 0x3c]
0x0043c923: e820630600               call 0x4a2c48
0x0043c928: 8b542410                 mov edx, dword ptr [esp + 0x10]
0x0043c92c: 8b4c242c                 mov ecx, dword ptr [esp + 0x2c]
0x0043c930: 52                       push edx
0x0043c931: 89442428                 mov dword ptr [esp + 0x28], eax
0x0043c935: c744243800000040         mov dword ptr [esp + 0x38], 0x40000000
0x0043c93d: 8b442438                 mov eax, dword ptr [esp + 0x38]
0x0043c941: 50                       push eax
0x0043c942: 55                       push ebp
0x0043c943: 51                       push ecx
0x0043c944: 57                       push edi
0x0043c945: c744245000000040         mov dword ptr [esp + 0x50], 0x40000000
0x0043c94d: e80e630300               call 0x472c60
0x0043c952: 8b542424                 mov edx, dword ptr [esp + 0x24]
0x0043c956: 8b442450                 mov eax, dword ptr [esp + 0x50]
0x0043c95a: 8b4c244c                 mov ecx, dword ptr [esp + 0x4c]
0x0043c95e: 52                       push edx
0x0043c95f: 8b54245c                 mov edx, dword ptr [esp + 0x5c]
0x0043c963: 50                       push eax
0x0043c964: 51                       push ecx
0x0043c965: 52                       push edx
0x0043c966: 53                       push ebx
0x0043c967: e8d46b0300               call 0x473540
0x0043c96c: 8b44244c                 mov eax, dword ptr [esp + 0x4c]
0x0043c970: 8b4c2438                 mov ecx, dword ptr [esp + 0x38]
0x0043c974: 8b54245c                 mov edx, dword ptr [esp + 0x5c]
0x0043c978: 83c0fe                   add eax, -2
0x0043c97b: 89442448                 mov dword ptr [esp + 0x48], eax
0x0043c97f: db442448                 fild dword ptr [esp + 0x48]
0x0043c983: 51                       push ecx
0x0043c984: 52                       push edx
0x0043c985: 55                       push ebp
0x0043c986: d9542454                 fst dword ptr [esp + 0x54]
0x0043c98a: d8442478                 fadd dword ptr [esp + 0x78]
0x0043c98e: d9542460                 fst dword ptr [esp + 0x60]
0x0043c992: 8b442460                 mov eax, dword ptr [esp + 0x60]
0x0043c996: 50                       push eax
0x0043c997: d95c247c                 fstp dword ptr [esp + 0x7c]
0x0043c99b: 57                       push edi
0x0043c99c: e8bf620300               call 0x472c60
0x0043c9a1: 8b7c244c                 mov edi, dword ptr [esp + 0x4c]
0x0043c9a5: 8b4c2478                 mov ecx, dword ptr [esp + 0x78]
0x0043c9a9: 8b542474                 mov edx, dword ptr [esp + 0x74]
0x0043c9ad: 8b842480000000           mov eax, dword ptr [esp + 0x80]
0x0043c9b4: 57                       push edi
0x0043c9b5: 51                       push ecx
0x0043c9b6: 52                       push edx
0x0043c9b7: 50                       push eax
0x0043c9b8: 53                       push ebx
0x0043c9b9: e8826b0300               call 0x473540
0x0043c9be: d944247c                 fld dword ptr [esp + 0x7c]
0x0043c9c2: d8642470                 fsub dword ptr [esp + 0x70]
0x0043c9c6: 83c450                   add esp, 0x50
0x0043c9c9: 57                       push edi
0x0043c9ca: c744243400000040         mov dword ptr [esp + 0x34], 0x40000000
0x0043c9d2: 8b6c2434                 mov ebp, dword ptr [esp + 0x34]
0x0043c9d6: d95c2430                 fstp dword ptr [esp + 0x30]
0x0043c9da: db442428                 fild dword ptr [esp + 0x28]
0x0043c9de: 8b542430                 mov edx, dword ptr [esp + 0x30]
0x0043c9e2: c744242c00006842         mov dword ptr [esp + 0x2c], 0x42680000
0x0043c9ea: 8b7c242c                 mov edi, dword ptr [esp + 0x2c]
0x0043c9ee: d95c2438                 fstp dword ptr [esp + 0x38]
0x0043c9f2: 8b4c2438                 mov ecx, dword ptr [esp + 0x38]
0x0043c9f6: 51                       push ecx
0x0043c9f7: 55                       push ebp
0x0043c9f8: 52                       push edx
0x0043c9f9: 57                       push edi
0x0043c9fa: e861620300               call 0x472c60
0x0043c9ff: 83c414                   add esp, 0x14
0x0043ca02: e9cd010000               jmp 0x43cbd4
0x0043ca07: 81ff24020000             cmp edi, 0x224
0x0043ca0d: 8b7c2428                 mov edi, dword ptr [esp + 0x28]
0x0043ca11: 0f84b8010000             je 0x43cbcf
0x0043ca17: 8b4c2434                 mov ecx, dword ptr [esp + 0x34]
0x0043ca1b: 8b54242c                 mov edx, dword ptr [esp + 0x2c]
0x0043ca1f: c6442410f0               mov byte ptr [esp + 0x10], 0xf0
0x0043ca24: c64424116e               mov byte ptr [esp + 0x11], 0x6e
0x0043ca29: c644241214               mov byte ptr [esp + 0x12], 0x14
0x0043ca2e: c6442413a0               mov byte ptr [esp + 0x13], 0xa0
0x0043ca33: 8b442410                 mov eax, dword ptr [esp + 0x10]
0x0043ca37: 50                       push eax
0x0043ca38: 51                       push ecx
0x0043ca39: 55                       push ebp
0x0043ca3a: 52                       push edx
0x0043ca3b: 57                       push edi
0x0043ca3c: e81f620300               call 0x472c60
0x0043ca41: 8b442424                 mov eax, dword ptr [esp + 0x24]
0x0043ca45: 8b4c2450                 mov ecx, dword ptr [esp + 0x50]
0x0043ca49: 8b54244c                 mov edx, dword ptr [esp + 0x4c]
0x0043ca4d: 50                       push eax
0x0043ca4e: 8b44245c                 mov eax, dword ptr [esp + 0x5c]
0x0043ca52: 51                       push ecx
0x0043ca53: 52                       push edx
0x0043ca54: 50                       push eax
0x0043ca55: 53                       push ebx
0x0043ca56: e8e56a0300               call 0x473540
0x0043ca5b: d944245c                 fld dword ptr [esp + 0x5c]
0x0043ca5f: d81d74c55c00             fcomp dword ptr [0x5cc574]
0x0043ca65: 83c428                   add esp, 0x28
0x0043ca68: c6442410b4               mov byte ptr [esp + 0x10], 0xb4
0x0043ca6d: c644241150               mov byte ptr [esp + 0x11], 0x50
0x0043ca72: c644241210               mov byte ptr [esp + 0x12], 0x10
0x0043ca77: dfe0                     fnstsw ax
0x0043ca79: c6442413ff               mov byte ptr [esp + 0x13], 0xff
0x0043ca7e: f6c441                   test ah, 0x41
0x0043ca81: 0f85e3000000             jne 0x43cb6a
0x0043ca87: d944243c                 fld dword ptr [esp + 0x3c]
0x0043ca8b: e8b8610600               call 0x4a2c48
0x0043ca90: 8b4c2410                 mov ecx, dword ptr [esp + 0x10]
0x0043ca94: 51                       push ecx
0x0043ca95: c744243800000040         mov dword ptr [esp + 0x38], 0x40000000
0x0043ca9d: 8b542438                 mov edx, dword ptr [esp + 0x38]
0x0043caa1: 52                       push edx
0x0043caa2: 89442428                 mov dword ptr [esp + 0x28], eax
0x0043caa6: 8b442434                 mov eax, dword ptr [esp + 0x34]
0x0043caaa: 55                       push ebp
0x0043caab: 50                       push eax
0x0043caac: 57                       push edi
0x0043caad: c744245000000040         mov dword ptr [esp + 0x50], 0x40000000
0x0043cab5: e8a6610300               call 0x472c60
0x0043caba: 8b4c2424                 mov ecx, dword ptr [esp + 0x24]
0x0043cabe: 8b542450                 mov edx, dword ptr [esp + 0x50]
0x0043cac2: 8b44244c                 mov eax, dword ptr [esp + 0x4c]
0x0043cac6: 51                       push ecx
0x0043cac7: 8b4c245c                 mov ecx, dword ptr [esp + 0x5c]
0x0043cacb: 52                       push edx
0x0043cacc: 50                       push eax
0x0043cacd: 51                       push ecx
0x0043cace: 53                       push ebx
0x0043cacf: e86c6a0300               call 0x473540
0x0043cad4: 8b542448                 mov edx, dword ptr [esp + 0x48]
0x0043cad8: 8b442438                 mov eax, dword ptr [esp + 0x38]
0x0043cadc: 8b4c245c                 mov ecx, dword ptr [esp + 0x5c]
0x0043cae0: 83c2fe                   add edx, -2
0x0043cae3: 8954244c                 mov dword ptr [esp + 0x4c], edx
0x0043cae7: db44244c                 fild dword ptr [esp + 0x4c]
0x0043caeb: 50                       push eax
0x0043caec: 51                       push ecx
0x0043caed: 55                       push ebp
0x0043caee: d9542458                 fst dword ptr [esp + 0x58]
0x0043caf2: d8442478                 fadd dword ptr [esp + 0x78]
0x0043caf6: d9542460                 fst dword ptr [esp + 0x60]
0x0043cafa: 8b542460                 mov edx, dword ptr [esp + 0x60]
0x0043cafe: 52                       push edx
0x0043caff: d95c247c                 fstp dword ptr [esp + 0x7c]
0x0043cb03: 57                       push edi
0x0043cb04: e857610300               call 0x472c60
0x0043cb09: 8b7c244c                 mov edi, dword ptr [esp + 0x4c]
0x0043cb0d: 8b442478                 mov eax, dword ptr [esp + 0x78]
0x0043cb11: 8b4c2474                 mov ecx, dword ptr [esp + 0x74]
0x0043cb15: 8b942480000000           mov edx, dword ptr [esp + 0x80]
0x0043cb1c: 57                       push edi
0x0043cb1d: 50                       push eax
0x0043cb1e: 51                       push ecx
0x0043cb1f: 52                       push edx
0x0043cb20: 53                       push ebx
0x0043cb21: e81a6a0300               call 0x473540
0x0043cb26: d944247c                 fld dword ptr [esp + 0x7c]
0x0043cb2a: d8642474                 fsub dword ptr [esp + 0x74]
0x0043cb2e: 83c450                   add esp, 0x50
0x0043cb31: 57                       push edi
0x0043cb32: c744243400000040         mov dword ptr [esp + 0x34], 0x40000000
0x0043cb3a: 8b6c2434                 mov ebp, dword ptr [esp + 0x34]
0x0043cb3e: d95c2430                 fstp dword ptr [esp + 0x30]
0x0043cb42: db442424                 fild dword ptr [esp + 0x24]
0x0043cb46: 8b4c2430                 mov ecx, dword ptr [esp + 0x30]
0x0043cb4a: c744242c00006842         mov dword ptr [esp + 0x2c], 0x42680000
0x0043cb52: 8b7c242c                 mov edi, dword ptr [esp + 0x2c]
0x0043cb56: d95c2438                 fstp dword ptr [esp + 0x38]
0x0043cb5a: 8b442438                 mov eax, dword ptr [esp + 0x38]
0x0043cb5e: 50                       push eax
0x0043cb5f: 55                       push ebp
0x0043cb60: 51                       push ecx
0x0043cb61: 57                       push edi
0x0043cb62: e8f9600300               call 0x472c60
0x0043cb67: 83c414                   add esp, 0x14
0x0043cb6a: 8b54242c                 mov edx, dword ptr [esp + 0x2c]
0x0043cb6e: d9442428                 fld dword ptr [esp + 0x28]
0x0043cb72: 8b442434                 mov eax, dword ptr [esp + 0x34]
0x0043cb76: d825d8d85c00             fsub dword ptr [0x5cd8d8]
0x0043cb7c: 8b4c2410                 mov ecx, dword ptr [esp + 0x10]
0x0043cb80: 6a00                     push 0
0x0043cb82: 6a01                     push 1
0x0043cb84: d95c2458                 fstp dword ptr [esp + 0x58]
0x0043cb88: 680000803f               push 0x3f800000
0x0043cb8d: 6a00                     push 0
0x0043cb8f: 680000803f               push 0x3f800000
0x0043cb94: 6a00                     push 0
0x0043cb96: 51                       push ecx
0x0043cb97: 89542470                 mov dword ptr [esp + 0x70], edx
0x0043cb9b: 8b4c2470                 mov ecx, dword ptr [esp + 0x70]
0x0043cb9f: 8bd0                     mov edx, eax
0x0043cba1: 52                       push edx
0x0043cba2: 8b542470                 mov edx, dword ptr [esp + 0x70]
0x0043cba6: 8944246c                 mov dword ptr [esp + 0x6c], eax
0x0043cbaa: c744246800005041         mov dword ptr [esp + 0x68], 0x41500000
0x0043cbb2: 8b442468                 mov eax, dword ptr [esp + 0x68]
0x0043cbb6: 50                       push eax
0x0043cbb7: 51                       push ecx
0x0043cbb8: 52                       push edx
0x0043cbb9: 687cda5c00               push 0x5cda7c
0x0043cbbe: e88deffcff               call 0x40bb50
0x0043cbc3: 83c404                   add esp, 4
0x0043cbc6: 50                       push eax
0x0043cbc7: e8246e0300               call 0x4739f0
0x0043cbcc: 83c430                   add esp, 0x30
0x0043cbcf: c6442413ff               mov byte ptr [esp + 0x13], 0xff
0x0043cbd4: a1e4908900               mov eax, dword ptr [0x8990e4]
0x0043cbd9: 85c0                     test eax, eax
0x0043cbdb: c644241000               mov byte ptr [esp + 0x10], 0
0x0043cbe0: c644241100               mov byte ptr [esp + 0x11], 0
0x0043cbe5: c644241200               mov byte ptr [esp + 0x12], 0
0x0043cbea: 743b                     je 0x43cc27
0x0043cbec: 8b46f8                   mov eax, dword ptr [esi - 8]
0x0043cbef: 8b4ee8                   mov ecx, dword ptr [esi - 0x18]
0x0043cbf2: 6880000000               push 0x80
0x0043cbf7: 50                       push eax
0x0043cbf8: 8b46f0                   mov eax, dword ptr [esi - 0x10]
0x0043cbfb: 51                       push ecx
0x0043cbfc: 8b4eec                   mov ecx, dword ptr [esi - 0x14]
0x0043cbff: c644241e00               mov byte ptr [esp + 0x1e], 0
0x0043cc04: c644241d00               mov byte ptr [esp + 0x1d], 0
0x0043cc09: c644241c00               mov byte ptr [esp + 0x1c], 0
0x0043cc0e: c644241f80               mov byte ptr [esp + 0x1f], 0x80
0x0043cc13: 8b54241c                 mov edx, dword ptr [esp + 0x1c]
0x0043cc17: 52                       push edx
0x0043cc18: 8b16                     mov edx, dword ptr [esi]
0x0043cc1a: 50                       push eax
0x0043cc1b: 51                       push ecx
0x0043cc1c: 52                       push edx
0x0043cc1d: e81eb5feff               call 0x428140
0x0043cc22: 83c41c                   add esp, 0x1c
0x0043cc25: eb59                     jmp 0x43cc80
0x0043cc27: 8b06                     mov eax, dword ptr [esi]
0x0043cc29: 3d24020000               cmp eax, 0x224
0x0043cc2e: 7524                     jne 0x43cc54
0x0043cc30: 8b4ee4                   mov ecx, dword ptr [esi - 0x1c]
0x0043cc33: 8b542410                 mov edx, dword ptr [esp + 0x10]
0x0043cc37: 51                       push ecx
0x0043cc38: 8b4ef0                   mov ecx, dword ptr [esi - 0x10]
0x0043cc3b: 6a02                     push 2
0x0043cc3d: 683333333f               push 0x3f333333
0x0043cc42: 52                       push edx
0x0043cc43: 51                       push ecx
0x0043cc44: 680000a043               push 0x43a00000
0x0043cc49: 50                       push eax
0x0043cc4a: e8f1b4feff               call 0x428140
0x0043cc4f: 83c41c                   add esp, 0x1c
0x0043cc52: eb2c                     jmp 0x43cc80
0x0043cc54: 8b56e4                   mov edx, dword ptr [esi - 0x1c]
0x0043cc57: 8b4ef8                   mov ecx, dword ptr [esi - 8]
0x0043cc5a: 52                       push edx
0x0043cc5b: 8b56e8                   mov edx, dword ptr [esi - 0x18]
0x0043cc5e: 51                       push ecx
0x0043cc5f: 8b4c2418                 mov ecx, dword ptr [esp + 0x18]
0x0043cc63: 52                       push edx
0x0043cc64: 8b56f0                   mov edx, dword ptr [esi - 0x10]
0x0043cc67: 51                       push ecx
0x0043cc68: 8b4eec                   mov ecx, dword ptr [esi - 0x14]
0x0043cc6b: 52                       push edx
0x0043cc6c: 51                       push ecx
0x0043cc6d: 50                       push eax
0x0043cc6e: e8cdb4feff               call 0x428140
0x0043cc73: 83c41c                   add esp, 0x1c
0x0043cc76: eb08                     jmp 0x43cc80
0x0043cc78: 8b7c2428                 mov edi, dword ptr [esp + 0x28]
0x0043cc7c: 8b6c2430                 mov ebp, dword ptr [esp + 0x30]
0x0043cc80: 8b46d8                   mov eax, dword ptr [esi - 0x28]
0x0043cc83: 85c0                     test eax, eax
0x0043cc85: 0f84d2050000             je 0x43d25d
0x0043cc8b: 83f802                   cmp eax, 2
0x0043cc8e: 0f84c9050000             je 0x43d25d
0x0043cc94: 8b56fc                   mov edx, dword ptr [esi - 4]
0x0043cc97: 83faff                   cmp edx, -1
0x0043cc9a: 0f84bd050000             je 0x43d25d
0x0043cca0: 8b0df8e96700             mov ecx, dword ptr [0x67e9f8]
0x0043cca6: 8bc1                     mov eax, ecx
0x0043cca8: c1e004                   shl eax, 4
0x0043ccab: 0344241c                 add eax, dword ptr [esp + 0x1c]
0x0043ccaf: 833c8540ed670000         cmp dword ptr [eax*4 + 0x67ed40], 0
0x0043ccb7: 7509                     jne 0x43ccc2
0x0043ccb9: 8d442410                 lea eax, [esp + 0x10]
0x0043ccbd: e80edefeff               call 0x42aad0
0x0043ccc2: 8b44241c                 mov eax, dword ptr [esp + 0x1c]
0x0043ccc6: c1e106                   shl ecx, 6
0x0043ccc9: 48                       dec eax
0x0043ccca: 3b8140ed6700             cmp eax, dword ptr [ecx + 0x67ed40]
0x0043ccd0: 0f8450010000             je 0x43ce26
0x0043ccd6: 8b4c2414                 mov ecx, dword ptr [esp + 0x14]
0x0043ccda: 8b542434                 mov edx, dword ptr [esp + 0x34]
0x0043ccde: 8b44242c                 mov eax, dword ptr [esp + 0x2c]
0x0043cce2: 51                       push ecx
0x0043cce3: 52                       push edx
0x0043cce4: 55                       push ebp
0x0043cce5: 50                       push eax
0x0043cce6: 57                       push edi
0x0043cce7: e8745f0300               call 0x472c60
0x0043ccec: 8b4c2428                 mov ecx, dword ptr [esp + 0x28]
0x0043ccf0: 8b542450                 mov edx, dword ptr [esp + 0x50]
0x0043ccf4: 8b44244c                 mov eax, dword ptr [esp + 0x4c]
0x0043ccf8: 51                       push ecx
0x0043ccf9: 8b4c245c                 mov ecx, dword ptr [esp + 0x5c]
0x0043ccfd: 52                       push edx
0x0043ccfe: 50                       push eax
0x0043ccff: 51                       push ecx
0x0043cd00: 53                       push ebx
0x0043cd01: e83a680300               call 0x473540
0x0043cd06: d944245c                 fld dword ptr [esp + 0x5c]
0x0043cd0a: d81d74c55c00             fcomp dword ptr [0x5cc574]
0x0043cd10: 83c428                   add esp, 0x28
0x0043cd13: c644241013               mov byte ptr [esp + 0x10], 0x13
0x0043cd18: c644241115               mov byte ptr [esp + 0x11], 0x15
0x0043cd1d: c644241250               mov byte ptr [esp + 0x12], 0x50
0x0043cd22: dfe0                     fnstsw ax
0x0043cd24: f6c441                   test ah, 0x41
0x0043cd27: 0f8590030000             jne 0x43d0bd
0x0043cd2d: d944243c                 fld dword ptr [esp + 0x3c]
0x0043cd31: e8125f0600               call 0x4a2c48
0x0043cd36: 8b542410                 mov edx, dword ptr [esp + 0x10]
0x0043cd3a: 8b4c242c                 mov ecx, dword ptr [esp + 0x2c]
0x0043cd3e: 52                       push edx
0x0043cd3f: 89442428                 mov dword ptr [esp + 0x28], eax
0x0043cd43: c744243800000040         mov dword ptr [esp + 0x38], 0x40000000
0x0043cd4b: 8b442438                 mov eax, dword ptr [esp + 0x38]
0x0043cd4f: 50                       push eax
0x0043cd50: 55                       push ebp
0x0043cd51: 51                       push ecx
0x0043cd52: 57                       push edi
0x0043cd53: c744245000000040         mov dword ptr [esp + 0x50], 0x40000000
0x0043cd5b: e8005f0300               call 0x472c60
0x0043cd60: 8b542424                 mov edx, dword ptr [esp + 0x24]
0x0043cd64: 8b442450                 mov eax, dword ptr [esp + 0x50]
0x0043cd68: 8b4c244c                 mov ecx, dword ptr [esp + 0x4c]
0x0043cd6c: 52                       push edx
0x0043cd6d: 8b54245c                 mov edx, dword ptr [esp + 0x5c]
0x0043cd71: 50                       push eax
0x0043cd72: 51                       push ecx
0x0043cd73: 52                       push edx
0x0043cd74: 53                       push ebx
0x0043cd75: e8c6670300               call 0x473540
0x0043cd7a: 8b44244c                 mov eax, dword ptr [esp + 0x4c]
0x0043cd7e: 8b4c2438                 mov ecx, dword ptr [esp + 0x38]
0x0043cd82: 8b54245c                 mov edx, dword ptr [esp + 0x5c]
0x0043cd86: 83c0fe                   add eax, -2
0x0043cd89: 89442448                 mov dword ptr [esp + 0x48], eax
0x0043cd8d: db442448                 fild dword ptr [esp + 0x48]
0x0043cd91: 51                       push ecx
0x0043cd92: 52                       push edx
0x0043cd93: 55                       push ebp
0x0043cd94: d9542454                 fst dword ptr [esp + 0x54]
0x0043cd98: d8442460                 fadd dword ptr [esp + 0x60]
0x0043cd9c: d95c2460                 fstp dword ptr [esp + 0x60]
0x0043cda0: 8b442460                 mov eax, dword ptr [esp + 0x60]
0x0043cda4: d9442454                 fld dword ptr [esp + 0x54]
0x0043cda8: 50                       push eax
0x0043cda9: d844247c                 fadd dword ptr [esp + 0x7c]
0x0043cdad: 57                       push edi
0x0043cdae: d99c2480000000           fstp dword ptr [esp + 0x80]
0x0043cdb5: e8a65e0300               call 0x472c60
0x0043cdba: 8b7c244c                 mov edi, dword ptr [esp + 0x4c]
0x0043cdbe: 8b4c2478                 mov ecx, dword ptr [esp + 0x78]
0x0043cdc2: 8b542474                 mov edx, dword ptr [esp + 0x74]
0x0043cdc6: 8b842480000000           mov eax, dword ptr [esp + 0x80]
0x0043cdcd: 57                       push edi
0x0043cdce: 51                       push ecx
0x0043cdcf: 52                       push edx
0x0043cdd0: 50                       push eax
0x0043cdd1: 53                       push ebx
0x0043cdd2: e869670300               call 0x473540
0x0043cdd7: d944247c                 fld dword ptr [esp + 0x7c]
0x0043cddb: d8642470                 fsub dword ptr [esp + 0x70]
0x0043cddf: 83c450                   add esp, 0x50
0x0043cde2: 57                       push edi
0x0043cde3: c744243400000040         mov dword ptr [esp + 0x34], 0x40000000
0x0043cdeb: 8b542434                 mov edx, dword ptr [esp + 0x34]
0x0043cdef: d95c2430                 fstp dword ptr [esp + 0x30]
0x0043cdf3: d944242c                 fld dword ptr [esp + 0x2c]
0x0043cdf7: 8b442430                 mov eax, dword ptr [esp + 0x30]
0x0043cdfb: d82574c55c00             fsub dword ptr [0x5cc574]
0x0043ce01: d95c242c                 fstp dword ptr [esp + 0x2c]
0x0043ce05: db442428                 fild dword ptr [esp + 0x28]
0x0043ce09: d95c2438                 fstp dword ptr [esp + 0x38]
0x0043ce0d: 8b4c2438                 mov ecx, dword ptr [esp + 0x38]
0x0043ce11: 51                       push ecx
0x0043ce12: 8b4c2430                 mov ecx, dword ptr [esp + 0x30]
0x0043ce16: 52                       push edx
0x0043ce17: 50                       push eax
0x0043ce18: 51                       push ecx
0x0043ce19: e8425e0300               call 0x472c60
0x0043ce1e: 83c414                   add esp, 0x14
0x0043ce21: e997020000               jmp 0x43d0bd
0x0043ce26: 81fa24020000             cmp edx, 0x224
0x0043ce2c: 0f85ae000000             jne 0x43cee0
0x0043ce32: c6442410f0               mov byte ptr [esp + 0x10], 0xf0
0x0043ce37: c64424116e               mov byte ptr [esp + 0x11], 0x6e
0x0043ce3c: c644241214               mov byte ptr [esp + 0x12], 0x14
0x0043ce41: c6442413a0               mov byte ptr [esp + 0x13], 0xa0
0x0043ce46: 8b542410                 mov edx, dword ptr [esp + 0x10]
0x0043ce4a: 52                       push edx
0x0043ce4b: 6800002042               push 0x42200000
0x0043ce50: 680000a043               push 0x43a00000
0x0043ce55: 6a00                     push 0
0x0043ce57: 680000b443               push 0x43b40000
0x0043ce5c: 6a00                     push 0
0x0043ce5e: 6800008c43               push 0x438c0000
0x0043ce63: e8585f0300               call 0x472dc0
0x0043ce68: 8b44242c                 mov eax, dword ptr [esp + 0x2c]
0x0043ce6c: 50                       push eax
0x0043ce6d: 680000dc43               push 0x43dc0000
0x0043ce72: 680000a043               push 0x43a00000
0x0043ce77: 680000f043               push 0x43f00000
0x0043ce7c: 680000b443               push 0x43b40000
0x0043ce81: 680000f043               push 0x43f00000
0x0043ce86: 6800008c43               push 0x438c0000
0x0043ce8b: e8305f0300               call 0x472dc0
0x0043ce90: 8b4c2448                 mov ecx, dword ptr [esp + 0x48]
0x0043ce94: 51                       push ecx
0x0043ce95: 6800007043               push 0x43700000
0x0043ce9a: 6800002042               push 0x42200000
0x0043ce9f: 6800008c43               push 0x438c0000
0x0043cea4: 6a00                     push 0
0x0043cea6: 6800004843               push 0x43480000
0x0043ceab: 6a00                     push 0
0x0043cead: e80e5f0300               call 0x472dc0
0x0043ceb2: 8b542464                 mov edx, dword ptr [esp + 0x64]
0x0043ceb6: 83c454                   add esp, 0x54
0x0043ceb9: 52                       push edx
0x0043ceba: 6800007043               push 0x43700000
0x0043cebf: 6800001644               push 0x44160000
0x0043cec4: 6800008c43               push 0x438c0000
0x0043cec9: 6800002044               push 0x44200000
0x0043cece: 6800004843               push 0x43480000
0x0043ced3: 6800002044               push 0x44200000
0x0043ced8: e8e35e0300               call 0x472dc0
0x0043cedd: 83c41c                   add esp, 0x1c
0x0043cee0: 817efc24020000           cmp dword ptr [esi - 4], 0x224
0x0043cee7: 0f84cb010000             je 0x43d0b8
0x0043ceed: 8b4c2434                 mov ecx, dword ptr [esp + 0x34]
0x0043cef1: 8b54242c                 mov edx, dword ptr [esp + 0x2c]
0x0043cef5: c6442410f0               mov byte ptr [esp + 0x10], 0xf0
0x0043cefa: c64424116e               mov byte ptr [esp + 0x11], 0x6e
0x0043ceff: c644241214               mov byte ptr [esp + 0x12], 0x14
0x0043cf04: c6442413a0               mov byte ptr [esp + 0x13], 0xa0
0x0043cf09: 8b442410                 mov eax, dword ptr [esp + 0x10]
0x0043cf0d: 50                       push eax
0x0043cf0e: 51                       push ecx
0x0043cf0f: 55                       push ebp
0x0043cf10: 52                       push edx
0x0043cf11: 57                       push edi
0x0043cf12: e8495d0300               call 0x472c60
0x0043cf17: 8b442424                 mov eax, dword ptr [esp + 0x24]
0x0043cf1b: 8b4c2450                 mov ecx, dword ptr [esp + 0x50]
0x0043cf1f: 8b54244c                 mov edx, dword ptr [esp + 0x4c]
0x0043cf23: 50                       push eax
0x0043cf24: 8b44245c                 mov eax, dword ptr [esp + 0x5c]
0x0043cf28: 51                       push ecx
0x0043cf29: 52                       push edx
0x0043cf2a: 50                       push eax
0x0043cf2b: 53                       push ebx
0x0043cf2c: e80f660300               call 0x473540
0x0043cf31: d944245c                 fld dword ptr [esp + 0x5c]
0x0043cf35: d81d74c55c00             fcomp dword ptr [0x5cc574]
0x0043cf3b: 83c428                   add esp, 0x28
0x0043cf3e: c6442410b4               mov byte ptr [esp + 0x10], 0xb4
0x0043cf43: c644241150               mov byte ptr [esp + 0x11], 0x50
0x0043cf48: c644241210               mov byte ptr [esp + 0x12], 0x10
0x0043cf4d: dfe0                     fnstsw ax
0x0043cf4f: c6442413ff               mov byte ptr [esp + 0x13], 0xff
0x0043cf54: f6c441                   test ah, 0x41
0x0043cf57: 0f85f6000000             jne 0x43d053
0x0043cf5d: d944243c                 fld dword ptr [esp + 0x3c]
0x0043cf61: e8e25c0600               call 0x4a2c48
0x0043cf66: 8b4c2410                 mov ecx, dword ptr [esp + 0x10]
0x0043cf6a: 51                       push ecx
0x0043cf6b: c744243800000040         mov dword ptr [esp + 0x38], 0x40000000
0x0043cf73: 8b542438                 mov edx, dword ptr [esp + 0x38]
0x0043cf77: 52                       push edx
0x0043cf78: 8944242c                 mov dword ptr [esp + 0x2c], eax
0x0043cf7c: 8b442434                 mov eax, dword ptr [esp + 0x34]
0x0043cf80: 55                       push ebp
0x0043cf81: 50                       push eax
0x0043cf82: 57                       push edi
0x0043cf83: c744245000000040         mov dword ptr [esp + 0x50], 0x40000000
0x0043cf8b: e8d05c0300               call 0x472c60
0x0043cf90: 8b4c2424                 mov ecx, dword ptr [esp + 0x24]
0x0043cf94: 8b542450                 mov edx, dword ptr [esp + 0x50]
0x0043cf98: 8b44244c                 mov eax, dword ptr [esp + 0x4c]
0x0043cf9c: 51                       push ecx
0x0043cf9d: 8b4c245c                 mov ecx, dword ptr [esp + 0x5c]
0x0043cfa1: 52                       push edx
0x0043cfa2: 50                       push eax
0x0043cfa3: 51                       push ecx
0x0043cfa4: 53                       push ebx
0x0043cfa5: e896650300               call 0x473540
0x0043cfaa: 8b54244c                 mov edx, dword ptr [esp + 0x4c]
0x0043cfae: 8b442438                 mov eax, dword ptr [esp + 0x38]
0x0043cfb2: 8b4c245c                 mov ecx, dword ptr [esp + 0x5c]
0x0043cfb6: 83c2fe                   add edx, -2
0x0043cfb9: 89542448                 mov dword ptr [esp + 0x48], edx
0x0043cfbd: db442448                 fild dword ptr [esp + 0x48]
0x0043cfc1: 50                       push eax
0x0043cfc2: 51                       push ecx
0x0043cfc3: 55                       push ebp
0x0043cfc4: d9542454                 fst dword ptr [esp + 0x54]
0x0043cfc8: d8442460                 fadd dword ptr [esp + 0x60]
0x0043cfcc: d95c2460                 fstp dword ptr [esp + 0x60]
0x0043cfd0: 8b542460                 mov edx, dword ptr [esp + 0x60]
0x0043cfd4: d9442454                 fld dword ptr [esp + 0x54]
0x0043cfd8: 52                       push edx
0x0043cfd9: d844247c                 fadd dword ptr [esp + 0x7c]
0x0043cfdd: 57                       push edi
0x0043cfde: d99c2480000000           fstp dword ptr [esp + 0x80]
0x0043cfe5: e8765c0300               call 0x472c60
0x0043cfea: 8b7c244c                 mov edi, dword ptr [esp + 0x4c]
0x0043cfee: 8b442478                 mov eax, dword ptr [esp + 0x78]
0x0043cff2: 8b4c2474                 mov ecx, dword ptr [esp + 0x74]
0x0043cff6: 8b942480000000           mov edx, dword ptr [esp + 0x80]
0x0043cffd: 57                       push edi
0x0043cffe: 50                       push eax
0x0043cfff: 51                       push ecx
0x0043d000: 52                       push edx
0x0043d001: 53                       push ebx
0x0043d002: e839650300               call 0x473540
0x0043d007: d944247c                 fld dword ptr [esp + 0x7c]
0x0043d00b: d8642470                 fsub dword ptr [esp + 0x70]
0x0043d00f: 83c450                   add esp, 0x50
0x0043d012: 57                       push edi
0x0043d013: c744243400000040         mov dword ptr [esp + 0x34], 0x40000000
0x0043d01b: 8b4c2434                 mov ecx, dword ptr [esp + 0x34]
0x0043d01f: d95c2430                 fstp dword ptr [esp + 0x30]
0x0043d023: d944242c                 fld dword ptr [esp + 0x2c]
0x0043d027: 8b542430                 mov edx, dword ptr [esp + 0x30]
0x0043d02b: d82574c55c00             fsub dword ptr [0x5cc574]
0x0043d031: d95c242c                 fstp dword ptr [esp + 0x2c]
0x0043d035: db442428                 fild dword ptr [esp + 0x28]
0x0043d039: d95c2438                 fstp dword ptr [esp + 0x38]
0x0043d03d: 8b442438                 mov eax, dword ptr [esp + 0x38]
0x0043d041: 50                       push eax
0x0043d042: 8b442430                 mov eax, dword ptr [esp + 0x30]
0x0043d046: 51                       push ecx
0x0043d047: 52                       push edx
0x0043d048: 50                       push eax
0x0043d049: e8125c0300               call 0x472c60
0x0043d04e: 83c414                   add esp, 0x14
0x0043d051: eb04                     jmp 0x43d057
0x0043d053: 8b7c2410                 mov edi, dword ptr [esp + 0x10]
0x0043d057: 8b542434                 mov edx, dword ptr [esp + 0x34]
0x0043d05b: d9442428                 fld dword ptr [esp + 0x28]
0x0043d05f: 8b4c242c                 mov ecx, dword ptr [esp + 0x2c]
0x0043d063: d825d8d85c00             fsub dword ptr [0x5cd8d8]
0x0043d069: 6a00                     push 0
0x0043d06b: 6a01                     push 1
0x0043d06d: 680000803f               push 0x3f800000
0x0043d072: d95c245c                 fstp dword ptr [esp + 0x5c]
0x0043d076: 6a00                     push 0
0x0043d078: 680000803f               push 0x3f800000
0x0043d07d: 6a00                     push 0
0x0043d07f: 57                       push edi
0x0043d080: 8bc2                     mov eax, edx
0x0043d082: 50                       push eax
0x0043d083: 8b442470                 mov eax, dword ptr [esp + 0x70]
0x0043d087: 894c2474                 mov dword ptr [esp + 0x74], ecx
0x0043d08b: c744246800005041         mov dword ptr [esp + 0x68], 0x41500000
0x0043d093: 8b4c2468                 mov ecx, dword ptr [esp + 0x68]
0x0043d097: 51                       push ecx
0x0043d098: 89542470                 mov dword ptr [esp + 0x70], edx
0x0043d09c: 8b542478                 mov edx, dword ptr [esp + 0x78]
0x0043d0a0: 52                       push edx
0x0043d0a1: 50                       push eax
0x0043d0a2: 687cda5c00               push 0x5cda7c
0x0043d0a7: e8a4eafcff               call 0x40bb50
0x0043d0ac: 83c404                   add esp, 4
0x0043d0af: 50                       push eax
0x0043d0b0: e83b690300               call 0x4739f0
0x0043d0b5: 83c430                   add esp, 0x30
0x0043d0b8: c6442413ff               mov byte ptr [esp + 0x13], 0xff
0x0043d0bd: a1e4908900               mov eax, dword ptr [0x8990e4]
0x0043d0c2: 85c0                     test eax, eax
0x0043d0c4: c644241000               mov byte ptr [esp + 0x10], 0
0x0043d0c9: c644241100               mov byte ptr [esp + 0x11], 0
0x0043d0ce: c644241200               mov byte ptr [esp + 0x12], 0
0x0043d0d3: 743f                     je 0x43d114
0x0043d0d5: 8b4ef8                   mov ecx, dword ptr [esi - 8]
0x0043d0d8: 8b56e8                   mov edx, dword ptr [esi - 0x18]
0x0043d0db: 6880000000               push 0x80
0x0043d0e0: 51                       push ecx
0x0043d0e1: 8b4ef0                   mov ecx, dword ptr [esi - 0x10]
0x0043d0e4: 52                       push edx
0x0043d0e5: 8b56ec                   mov edx, dword ptr [esi - 0x14]
0x0043d0e8: c644241e00               mov byte ptr [esp + 0x1e], 0
0x0043d0ed: c644241d00               mov byte ptr [esp + 0x1d], 0
0x0043d0f2: c644241c00               mov byte ptr [esp + 0x1c], 0
0x0043d0f7: c644241f80               mov byte ptr [esp + 0x1f], 0x80
0x0043d0fc: 8b44241c                 mov eax, dword ptr [esp + 0x1c]
0x0043d100: 50                       push eax
0x0043d101: 8b46fc                   mov eax, dword ptr [esi - 4]
0x0043d104: 51                       push ecx
0x0043d105: 52                       push edx
0x0043d106: 50                       push eax
0x0043d107: e834b0feff               call 0x428140
0x0043d10c: 83c41c                   add esp, 0x1c
0x0043d10f: e949010000               jmp 0x43d25d
0x0043d114: 8b46fc                   mov eax, dword ptr [esi - 4]
0x0043d117: 3d24020000               cmp eax, 0x224
0x0043d11c: 7527                     jne 0x43d145
0x0043d11e: 8b4ee4                   mov ecx, dword ptr [esi - 0x1c]
0x0043d121: 8b542410                 mov edx, dword ptr [esp + 0x10]
0x0043d125: 51                       push ecx
0x0043d126: 8b4ef0                   mov ecx, dword ptr [esi - 0x10]
0x0043d129: 6a02                     push 2
0x0043d12b: 683333333f               push 0x3f333333
0x0043d130: 52                       push edx
0x0043d131: 51                       push ecx
0x0043d132: 680000a043               push 0x43a00000
0x0043d137: 50                       push eax
0x0043d138: e803b0feff               call 0x428140
0x0043d13d: 83c41c                   add esp, 0x1c
0x0043d140: e918010000               jmp 0x43d25d
0x0043d145: 8b56e4                   mov edx, dword ptr [esi - 0x1c]
0x0043d148: 8b4ef8                   mov ecx, dword ptr [esi - 8]
0x0043d14b: 52                       push edx
0x0043d14c: 8b56e8                   mov edx, dword ptr [esi - 0x18]
0x0043d14f: 51                       push ecx
0x0043d150: 8b4c2418                 mov ecx, dword ptr [esp + 0x18]
0x0043d154: 52                       push edx
0x0043d155: 8b56f0                   mov edx, dword ptr [esi - 0x10]
0x0043d158: 51                       push ecx
0x0043d159: 8b4eec                   mov ecx, dword ptr [esi - 0x14]
0x0043d15c: 52                       push edx
0x0043d15d: 51                       push ecx
0x0043d15e: 50                       push eax
0x0043d15f: e8dcaffeff               call 0x428140
0x0043d164: 83c41c                   add esp, 0x1c
0x0043d167: e9f1000000               jmp 0x43d25d
0x0043d16c: 3d000012ff               cmp eax, 0xff120000
0x0043d171: 7412                     je 0x43d185
0x0043d173: 3d000013ff               cmp eax, 0xff130000
0x0043d178: 740b                     je 0x43d185
0x0043d17a: 3d000023ff               cmp eax, 0xff230000
0x0043d17f: 0f85d8000000             jne 0x43d25d
0x0043d185: 8b46d8                   mov eax, dword ptr [esi - 0x28]
0x0043d188: 85c0                     test eax, eax
0x0043d18a: 7405                     je 0x43d191
0x0043d18c: 83f802                   cmp eax, 2
0x0043d18f: 7561                     jne 0x43d1f2
0x0043d191: 8b06                     mov eax, dword ptr [esi]
0x0043d193: 83f8ff                   cmp eax, -1
0x0043d196: 0f84c1000000             je 0x43d25d
0x0043d19c: 8b56e4                   mov edx, dword ptr [esi - 0x1c]
0x0043d19f: d946f0                   fld dword ptr [esi - 0x10]
0x0043d1a2: 8b4ef8                   mov ecx, dword ptr [esi - 8]
0x0043d1a5: d8051cc35c00             fadd dword ptr [0x5cc31c]
0x0043d1ab: 52                       push edx
0x0043d1ac: 8b56e8                   mov edx, dword ptr [esi - 0x18]
0x0043d1af: 51                       push ecx
0x0043d1b0: 8b4c2420                 mov ecx, dword ptr [esp + 0x20]
0x0043d1b4: 52                       push edx
0x0043d1b5: 51                       push ecx
0x0043d1b6: 83ec08                   sub esp, 8
0x0043d1b9: d95c2404                 fstp dword ptr [esp + 4]
0x0043d1bd: d946ec                   fld dword ptr [esi - 0x14]
0x0043d1c0: d8051cc35c00             fadd dword ptr [0x5cc31c]
0x0043d1c6: d91c24                   fstp dword ptr [esp]
0x0043d1c9: 50                       push eax
0x0043d1ca: e871affeff               call 0x428140
0x0043d1cf: 8b56e4                   mov edx, dword ptr [esi - 0x1c]
0x0043d1d2: 8b46f8                   mov eax, dword ptr [esi - 8]
0x0043d1d5: 8b4ee8                   mov ecx, dword ptr [esi - 0x18]
0x0043d1d8: 52                       push edx
0x0043d1d9: 8b56e0                   mov edx, dword ptr [esi - 0x20]
0x0043d1dc: 50                       push eax
0x0043d1dd: 8b46f0                   mov eax, dword ptr [esi - 0x10]
0x0043d1e0: 51                       push ecx
0x0043d1e1: 8b4eec                   mov ecx, dword ptr [esi - 0x14]
0x0043d1e4: 52                       push edx
0x0043d1e5: 8b16                     mov edx, dword ptr [esi]
0x0043d1e7: 50                       push eax
0x0043d1e8: 51                       push ecx
0x0043d1e9: 52                       push edx
0x0043d1ea: e851affeff               call 0x428140
0x0043d1ef: 83c438                   add esp, 0x38
0x0043d1f2: 8b46d8                   mov eax, dword ptr [esi - 0x28]
0x0043d1f5: 85c0                     test eax, eax
0x0043d1f7: 7464                     je 0x43d25d
0x0043d1f9: 83f802                   cmp eax, 2
0x0043d1fc: 745f                     je 0x43d25d
0x0043d1fe: 8b46fc                   mov eax, dword ptr [esi - 4]
0x0043d201: 83f8ff                   cmp eax, -1
0x0043d204: 7457                     je 0x43d25d
0x0043d206: 8b4ee4                   mov ecx, dword ptr [esi - 0x1c]
0x0043d209: d946f0                   fld dword ptr [esi - 0x10]
0x0043d20c: 8b56f8                   mov edx, dword ptr [esi - 8]
0x0043d20f: d8051cc35c00             fadd dword ptr [0x5cc31c]
0x0043d215: 51                       push ecx
0x0043d216: 8b4ee8                   mov ecx, dword ptr [esi - 0x18]
0x0043d219: 52                       push edx
0x0043d21a: 8b542420                 mov edx, dword ptr [esp + 0x20]
0x0043d21e: 51                       push ecx
0x0043d21f: 52                       push edx
0x0043d220: 83ec08                   sub esp, 8
0x0043d223: d95c2404                 fstp dword ptr [esp + 4]
0x0043d227: d946ec                   fld dword ptr [esi - 0x14]
0x0043d22a: d8051cc35c00             fadd dword ptr [0x5cc31c]
0x0043d230: d91c24                   fstp dword ptr [esp]
0x0043d233: 50                       push eax
0x0043d234: e807affeff               call 0x428140
0x0043d239: 8b46e4                   mov eax, dword ptr [esi - 0x1c]
0x0043d23c: 8b4ef8                   mov ecx, dword ptr [esi - 8]
0x0043d23f: 8b56e8                   mov edx, dword ptr [esi - 0x18]
0x0043d242: 50                       push eax
0x0043d243: 8b46e0                   mov eax, dword ptr [esi - 0x20]
0x0043d246: 51                       push ecx
0x0043d247: 8b4ef0                   mov ecx, dword ptr [esi - 0x10]
0x0043d24a: 52                       push edx
0x0043d24b: 8b56ec                   mov edx, dword ptr [esi - 0x14]
0x0043d24e: 50                       push eax
0x0043d24f: 8b46fc                   mov eax, dword ptr [esi - 4]
0x0043d252: 51                       push ecx
0x0043d253: 52                       push edx
0x0043d254: 50                       push eax
0x0043d255: e8e6aefeff               call 0x428140
0x0043d25a: 83c438                   add esp, 0x38
0x0043d25d: 83c634                   add esi, 0x34
0x0043d260: 81fe04918900             cmp esi, 0x899104
0x0043d266: 0f8cc4f4ffff             jl 0x43c730
0x0043d26c: e8bfecffff               call 0x43bf30
0x0043d271: 5f                       pop edi
0x0043d272: 5e                       pop esi
0x0043d273: 5d                       pop ebp
0x0043d274: 5b                       pop ebx
0x0043d275: 8b0df83f7d00             mov ecx, dword ptr [0x7d3ff8]
0x0043d27b: 6a01                     push 1
0x0043d27d: 6a06                     push 6
0x0043d27f: ff5120                   call dword ptr [ecx + 0x20]
0x0043d282: 8b15f83f7d00             mov edx, dword ptr [0x7d3ff8]
0x0043d288: 6a01                     push 1
0x0043d28a: 6a08                     push 8
0x0043d28c: ff5220                   call dword ptr [edx + 0x20]
0x0043d28f: 83c410                   add esp, 0x10
0x0043d292: 83c448                   add esp, 0x48
