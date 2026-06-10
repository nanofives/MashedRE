0x00410d10: 83ec28                   sub esp, 0x28
0x00410d13: e868230300               call 0x443080
0x00410d18: 83f801                   cmp eax, 1
0x00410d1b: 7506                     jne 0x410d23
0x00410d1d: 33c0                     xor eax, eax
0x00410d1f: 83c428                   add esp, 0x28
0x00410d22: c3                       ret 
0x00410d23: a1d00f7f00               mov eax, dword ptr [0x7f0fd0]
0x00410d28: 53                       push ebx
0x00410d29: 55                       push ebp
0x00410d2a: 83c0fc                   add eax, -4
0x00410d2d: 83f806                   cmp eax, 6
0x00410d30: 56                       push esi
0x00410d31: 57                       push edi
0x00410d32: 0f87ab010000             ja 0x410ee3
0x00410d38: ff24854c114100           jmp dword ptr [eax*4 + 0x41114c]
0x00410d3f: 6a00                     push 0
0x00410d41: e86aba0500               call 0x46c7b0
0x00410d46: 83c404                   add esp, 4
0x00410d49: 85c0                     test eax, eax
0x00410d4b: 7427                     je 0x410d74
0x00410d4d: 5f                       pop edi
0x00410d4e: 5e                       pop esi
0x00410d4f: 5d                       pop ebp
0x00410d50: 5b                       pop ebx
0x00410d51: 83c428                   add esp, 0x28
0x00410d54: e9374bffff               jmp 0x405890
0x00410d59: 6a00                     push 0
0x00410d5b: e8d0690000               call 0x417730
0x00410d60: d81d1cc35c00             fcomp dword ptr [0x5cc31c]
0x00410d66: dfe0                     fnstsw ax
0x00410d68: 83c404                   add esp, 4
0x00410d6b: f6c401                   test ah, 1
0x00410d6e: 0f856f010000             jne 0x410ee3
0x00410d74: 5f                       pop edi
0x00410d75: 5e                       pop esi
0x00410d76: 5d                       pop ebp
0x00410d77: b801000000               mov eax, 1
0x00410d7c: 5b                       pop ebx
0x00410d7d: 83c428                   add esp, 0x28
0x00410d80: c3                       ret 
0x00410d81: 6a00                     push 0
0x00410d83: e8a8690000               call 0x417730
0x00410d88: d81d1cc35c00             fcomp dword ptr [0x5cc31c]
0x00410d8e: 83c404                   add esp, 4
0x00410d91: dfe0                     fnstsw ax
0x00410d93: f6c401                   test ah, 1
0x00410d96: 74dc                     je 0x410d74
0x00410d98: 6a01                     push 1
0x00410d9a: e891690000               call 0x417730
0x00410d9f: d81d1cc35c00             fcomp dword ptr [0x5cc31c]
0x00410da5: 83c404                   add esp, 4
0x00410da8: dfe0                     fnstsw ax
0x00410daa: f6c401                   test ah, 1
0x00410dad: 74c5                     je 0x410d74
0x00410daf: 8d442420                 lea eax, [esp + 0x20]
0x00410db3: 50                       push eax
0x00410db4: 8d4c241c                 lea ecx, [esp + 0x1c]
0x00410db8: 51                       push ecx
0x00410db9: 6a00                     push 0
0x00410dbb: e8f0bd0500               call 0x46cbb0
0x00410dc0: 8b442424                 mov eax, dword ptr [esp + 0x24]
0x00410dc4: 83c40c                   add esp, 0xc
0x00410dc7: 85c0                     test eax, eax
0x00410dc9: 75a9                     jne 0x410d74
0x00410dcb: 6a01                     push 1
0x00410dcd: e84e2d0100               call 0x423b20
0x00410dd2: 83c404                   add esp, 4
0x00410dd5: 85c0                     test eax, eax
0x00410dd7: 0f8406010000             je 0x410ee3
0x00410ddd: 5f                       pop edi
0x00410dde: 5e                       pop esi
0x00410ddf: 5d                       pop ebp
0x00410de0: b801000000               mov eax, 1
0x00410de5: 5b                       pop ebx
0x00410de6: 83c428                   add esp, 0x28
0x00410de9: c3                       ret 
0x00410dea: 6a00                     push 0
0x00410dec: e83f690000               call 0x417730
0x00410df1: d81d1cc35c00             fcomp dword ptr [0x5cc31c]
0x00410df7: 83c404                   add esp, 4
0x00410dfa: dfe0                     fnstsw ax
0x00410dfc: f6c401                   test ah, 1
0x00410dff: 0f846fffffff             je 0x410d74
0x00410e05: 8d542420                 lea edx, [esp + 0x20]
0x00410e09: 52                       push edx
0x00410e0a: 8d44241c                 lea eax, [esp + 0x1c]
0x00410e0e: 50                       push eax
0x00410e0f: 6a00                     push 0
0x00410e11: e89abd0500               call 0x46cbb0
0x00410e16: 8b442424                 mov eax, dword ptr [esp + 0x24]
0x00410e1a: 83c40c                   add esp, 0xc
0x00410e1d: 85c0                     test eax, eax
0x00410e1f: 0f84be000000             je 0x410ee3
0x00410e25: 5f                       pop edi
0x00410e26: 5e                       pop esi
0x00410e27: 5d                       pop ebp
0x00410e28: b801000000               mov eax, 1
0x00410e2d: 5b                       pop ebx
0x00410e2e: 83c428                   add esp, 0x28
0x00410e31: c3                       ret 
0x00410e32: 6a00                     push 0
0x00410e34: e877b90500               call 0x46c7b0
0x00410e39: 83c404                   add esp, 4
0x00410e3c: 85c0                     test eax, eax
0x00410e3e: 0f8430ffffff             je 0x410d74
0x00410e44: 33f6                     xor esi, esi
0x00410e46: 33ff                     xor edi, edi
0x00410e48: e8f3d4ffff               call 0x40e340
0x00410e4d: 85c0                     test eax, eax
0x00410e4f: 7e32                     jle 0x410e83
0x00410e51: 56                       push esi
0x00410e52: e859b90500               call 0x46c7b0
0x00410e57: 83c404                   add esp, 4
0x00410e5a: 85c0                     test eax, eax
0x00410e5c: 7501                     jne 0x410e5f
0x00410e5e: 47                       inc edi
0x00410e5f: 56                       push esi
0x00410e60: e8cb680000               call 0x417730
0x00410e65: d81d1cc35c00             fcomp dword ptr [0x5cc31c]
0x00410e6b: 83c404                   add esp, 4
0x00410e6e: dfe0                     fnstsw ax
0x00410e70: f6c441                   test ah, 0x41
0x00410e73: 0f84fbfeffff             je 0x410d74
0x00410e79: 46                       inc esi
0x00410e7a: e8c1d4ffff               call 0x40e340
0x00410e7f: 3bf0                     cmp esi, eax
0x00410e81: 7cce                     jl 0x410e51
0x00410e83: e8b8d4ffff               call 0x40e340
0x00410e88: 48                       dec eax
0x00410e89: 33c9                     xor ecx, ecx
0x00410e8b: 3bf8                     cmp edi, eax
0x00410e8d: 0f9dc1                   setge cl
0x00410e90: 5f                       pop edi
0x00410e91: 5e                       pop esi
0x00410e92: 5d                       pop ebp
0x00410e93: 5b                       pop ebx
0x00410e94: 8bc1                     mov eax, ecx
0x00410e96: 83c428                   add esp, 0x28
0x00410e99: c3                       ret 
0x00410e9a: 8d542420                 lea edx, [esp + 0x20]
0x00410e9e: 52                       push edx
0x00410e9f: 8d44241c                 lea eax, [esp + 0x1c]
0x00410ea3: 50                       push eax
0x00410ea4: 6a00                     push 0
0x00410ea6: e805bd0500               call 0x46cbb0
0x00410eab: 8b442424                 mov eax, dword ptr [esp + 0x24]
0x00410eaf: 83c40c                   add esp, 0xc
0x00410eb2: 85c0                     test eax, eax
0x00410eb4: 0f85bafeffff             jne 0x410d74
0x00410eba: d905e40f7f00             fld dword ptr [0x7f0fe4]
0x00410ec0: d81d7c755d00             fcomp dword ptr [0x5d757c]
0x00410ec6: dfe0                     fnstsw ax
0x00410ec8: f6c441                   test ah, 0x41
0x00410ecb: 0f8ba3feffff             jnp 0x410d74
0x00410ed1: 6a00                     push 0
0x00410ed3: e858680000               call 0x417730
0x00410ed8: d81d74c55c00             fcomp dword ptr [0x5cc574]
0x00410ede: e983feffff               jmp 0x410d66
0x00410ee3: c744241c00000000         mov dword ptr [esp + 0x1c], 0
0x00410eeb: e8001f0300               call 0x442df0
0x00410ef0: d81d5cc55c00             fcomp dword ptr [0x5cc55c]
0x00410ef6: dfe0                     fnstsw ax
0x00410ef8: f6c444                   test ah, 0x44
0x00410efb: 0f8a3d010000             jp 0x41103e
0x00410f01: 8d4c242c                 lea ecx, [esp + 0x2c]
0x00410f05: 51                       push ecx
0x00410f06: 8d542418                 lea edx, [esp + 0x18]
0x00410f0a: 52                       push edx
0x00410f0b: e870d2ffff               call 0x40e180
0x00410f10: 8b74241c                 mov esi, dword ptr [esp + 0x1c]
0x00410f14: 56                       push esi
0x00410f15: e8b67bffff               call 0x408ad0
0x00410f1a: d95c241c                 fstp dword ptr [esp + 0x1c]
0x00410f1e: 8b7c2438                 mov edi, dword ptr [esp + 0x38]
0x00410f22: 57                       push edi
0x00410f23: e8a87bffff               call 0x408ad0
0x00410f28: d95c2424                 fstp dword ptr [esp + 0x24]
0x00410f2c: d9442420                 fld dword ptr [esp + 0x20]
0x00410f30: 83c410                   add esp, 0x10
0x00410f33: d81d30c75c00             fcomp dword ptr [0x5cc730]
0x00410f39: dfe0                     fnstsw ax
0x00410f3b: f6c441                   test ah, 0x41
0x00410f3e: 7521                     jne 0x410f61
0x00410f40: d9442414                 fld dword ptr [esp + 0x14]
0x00410f44: d81d6ccd5c00             fcomp dword ptr [0x5ccd6c]
0x00410f4a: dfe0                     fnstsw ax
0x00410f4c: f6c405                   test ah, 5
0x00410f4f: 7a10                     jp 0x410f61
0x00410f51: d9442410                 fld dword ptr [esp + 0x10]
0x00410f55: d82568c55c00             fsub dword ptr [0x5cc568]
0x00410f5b: d95c2410                 fstp dword ptr [esp + 0x10]
0x00410f5f: eb30                     jmp 0x410f91
0x00410f61: d9442414                 fld dword ptr [esp + 0x14]
0x00410f65: d81d30c75c00             fcomp dword ptr [0x5cc730]
0x00410f6b: dfe0                     fnstsw ax
0x00410f6d: f6c441                   test ah, 0x41
0x00410f70: 751f                     jne 0x410f91
0x00410f72: d9442410                 fld dword ptr [esp + 0x10]
0x00410f76: d81d6ccd5c00             fcomp dword ptr [0x5ccd6c]
0x00410f7c: dfe0                     fnstsw ax
0x00410f7e: f6c405                   test ah, 5
0x00410f81: 7a0e                     jp 0x410f91
0x00410f83: d9442414                 fld dword ptr [esp + 0x14]
0x00410f87: d82568c55c00             fsub dword ptr [0x5cc568]
0x00410f8d: d95c2414                 fstp dword ptr [esp + 0x14]
0x00410f91: 833dd00f7f0001           cmp dword ptr [0x7f0fd0], 1
0x00410f98: 751d                     jne 0x410fb7
0x00410f9a: a1d40f7f00               mov eax, dword ptr [0x7f0fd4]
0x00410f9f: 3bf0                     cmp esi, eax
0x00410fa1: 7508                     jne 0x410fab
0x00410fa3: c74424100000c842         mov dword ptr [esp + 0x10], 0x42c80000
0x00410fab: 3bf8                     cmp edi, eax
0x00410fad: 7508                     jne 0x410fb7
0x00410faf: c74424140000c842         mov dword ptr [esp + 0x14], 0x42c80000
0x00410fb7: 8d442430                 lea eax, [esp + 0x30]
0x00410fbb: 50                       push eax
0x00410fbc: 8d4c242c                 lea ecx, [esp + 0x2c]
0x00410fc0: 51                       push ecx
0x00410fc1: 56                       push esi
0x00410fc2: e8e9bb0500               call 0x46cbb0
0x00410fc7: 8d542440                 lea edx, [esp + 0x40]
0x00410fcb: 52                       push edx
0x00410fcc: 8d442434                 lea eax, [esp + 0x34]
0x00410fd0: 50                       push eax
0x00410fd1: 57                       push edi
0x00410fd2: e8d9bb0500               call 0x46cbb0
0x00410fd7: d9442428                 fld dword ptr [esp + 0x28]
0x00410fdb: d85c242c                 fcomp dword ptr [esp + 0x2c]
0x00410fdf: 83c418                   add esp, 0x18
0x00410fe2: dfe0                     fnstsw ax
0x00410fe4: f6c441                   test ah, 0x41
0x00410fe7: 751a                     jne 0x411003
0x00410fe9: 8b442428                 mov eax, dword ptr [esp + 0x28]
0x00410fed: 85c0                     test eax, eax
0x00410fef: 7422                     je 0x411013
0x00410ff1: 8b442424                 mov eax, dword ptr [esp + 0x24]
0x00410ff5: 85c0                     test eax, eax
0x00410ff7: 741c                     je 0x411015
0x00410ff9: 5f                       pop edi
0x00410ffa: 5e                       pop esi
0x00410ffb: 5d                       pop ebp
0x00410ffc: 33c0                     xor eax, eax
0x00410ffe: 5b                       pop ebx
0x00410fff: 83c428                   add esp, 0x28
0x00411002: c3                       ret 
0x00411003: 8b442424                 mov eax, dword ptr [esp + 0x24]
0x00411007: 85c0                     test eax, eax
0x00411009: 740a                     je 0x411015
0x0041100b: 8b442428                 mov eax, dword ptr [esp + 0x28]
0x0041100f: 85c0                     test eax, eax
0x00411011: 75e6                     jne 0x410ff9
0x00411013: 8bf7                     mov esi, edi
0x00411015: 6880000000               push 0x80
0x0041101a: 6a0a                     push 0xa
0x0041101c: 6a03                     push 3
0x0041101e: 56                       push esi
0x0041101f: e8bc120800               call 0x4922e0
0x00411024: 56                       push esi
0x00411025: e8a61f0100               call 0x422fd0
0x0041102a: 6a01                     push 1
0x0041102c: 56                       push esi
0x0041102d: e8aedeffff               call 0x40eee0
0x00411032: 83c41c                   add esp, 0x1c
0x00411035: 83f801                   cmp eax, 1
0x00411038: 0f8436fdffff             je 0x410d74
0x0041103e: 33ed                     xor ebp, ebp
0x00411040: 33db                     xor ebx, ebx
0x00411042: 33ff                     xor edi, edi
0x00411044: be34000000               mov esi, 0x34
0x00411049: 8da42400000000           lea esp, [esp]
0x00411050: 8b0d70275f00             mov ecx, dword ptr [0x5f2770]
0x00411056: 833c0e00                 cmp dword ptr [esi + ecx], 0
0x0041105a: 7410                     je 0x41106c
0x0041105c: 57                       push edi
0x0041105d: 45                       inc ebp
0x0041105e: e84db70500               call 0x46c7b0
0x00411063: 83c404                   add esp, 4
0x00411066: 83f801                   cmp eax, 1
0x00411069: 7501                     jne 0x41106c
0x0041106b: 43                       inc ebx
0x0041106c: 83c604                   add esi, 4
0x0041106f: 47                       inc edi
0x00411070: 83fe44                   cmp esi, 0x44
0x00411073: 7cdb                     jl 0x411050
0x00411075: 83fd01                   cmp ebp, 1
0x00411078: 891d20338000             mov dword ptr [0x803320], ebx
0x0041107e: 7475                     je 0x4110f5
0x00411080: 83fb01                   cmp ebx, 1
0x00411083: 0f84ebfcffff             je 0x410d74
0x00411089: 85db                     test ebx, ebx
0x0041108b: 0f8568ffffff             jne 0x410ff9
0x00411091: 33ff                     xor edi, edi
0x00411093: be34000000               mov esi, 0x34
0x00411098: eb06                     jmp 0x4110a0
0x0041109a: 8d9b00000000             lea ebx, [ebx]
0x004110a0: 8b1570275f00             mov edx, dword ptr [0x5f2770]
0x004110a6: 833c1600                 cmp dword ptr [esi + edx], 0
0x004110aa: 740d                     je 0x4110b9
0x004110ac: 57                       push edi
0x004110ad: e89e79ffff               call 0x408a50
0x004110b2: d95c2420                 fstp dword ptr [esp + 0x20]
0x004110b6: 83c404                   add esp, 4
0x004110b9: 83c604                   add esi, 4
0x004110bc: 47                       inc edi
0x004110bd: 83fe44                   cmp esi, 0x44
0x004110c0: 7cde                     jl 0x4110a0
0x004110c2: 33ff                     xor edi, edi
0x004110c4: be34000000               mov esi, 0x34
0x004110c9: 8da42400000000           lea esp, [esp]
0x004110d0: a170275f00               mov eax, dword ptr [0x5f2770]
0x004110d5: 833c0600                 cmp dword ptr [esi + eax], 0
0x004110d9: 7431                     je 0x41110c
0x004110db: 57                       push edi
0x004110dc: e86f79ffff               call 0x408a50
0x004110e1: d8542420                 fcom dword ptr [esp + 0x20]
0x004110e5: 83c404                   add esp, 4
0x004110e8: dfe0                     fnstsw ax
0x004110ea: f6c441                   test ah, 0x41
0x004110ed: 751b                     jne 0x41110a
0x004110ef: d95c241c                 fstp dword ptr [esp + 0x1c]
0x004110f3: eb17                     jmp 0x41110c
0x004110f5: 85db                     test ebx, ebx
0x004110f7: 0f85fcfeffff             jne 0x410ff9
0x004110fd: 5f                       pop edi
0x004110fe: 5e                       pop esi
0x004110ff: 5d                       pop ebp
0x00411100: b801000000               mov eax, 1
0x00411105: 5b                       pop ebx
0x00411106: 83c428                   add esp, 0x28
0x00411109: c3                       ret 
0x0041110a: ddd8                     fstp st(0)
0x0041110c: 83c604                   add esi, 4
0x0041110f: 47                       inc edi
0x00411110: 83fe44                   cmp esi, 0x44
0x00411113: 7cbb                     jl 0x4110d0
0x00411115: 8b74241c                 mov esi, dword ptr [esp + 0x1c]
0x00411119: 56                       push esi
0x0041111a: 6a00                     push 0
0x0041111c: e84f79ffff               call 0x408a70
0x00411121: 56                       push esi
0x00411122: 6a01                     push 1
0x00411124: e84779ffff               call 0x408a70
0x00411129: 56                       push esi
0x0041112a: 6a02                     push 2
0x0041112c: e83f79ffff               call 0x408a70
0x00411131: 56                       push esi
0x00411132: 6a03                     push 3
0x00411134: e83779ffff               call 0x408a70
0x00411139: 83c420                   add esp, 0x20
0x0041113c: 5f                       pop edi
0x0041113d: 5e                       pop esi
0x0041113e: 5d                       pop ebp
0x0041113f: b801000000               mov eax, 1
0x00411144: 5b                       pop ebx
0x00411145: 83c428                   add esp, 0x28
