0x00442a60: 83ec3c                   sub esp, 0x3c
0x00442a63: 33c0                     xor eax, eax
0x00442a65: a3b0898900               mov dword ptr [0x8989b0], eax
0x00442a6a: 8d4c2404                 lea ecx, [esp + 4]
0x00442a6e: a3b4898900               mov dword ptr [0x8989b4], eax
0x00442a73: 51                       push ecx
0x00442a74: 8d542404                 lea edx, [esp + 4]
0x00442a78: a3b8898900               mov dword ptr [0x8989b8], eax
0x00442a7d: 52                       push edx
0x00442a7e: a3bc898900               mov dword ptr [0x8989bc], eax
0x00442a83: c744240804000000         mov dword ptr [esp + 8], 4
0x00442a8b: e8f0b6fcff               call 0x40e180
0x00442a90: 8b442408                 mov eax, dword ptr [esp + 8]
0x00442a94: 50                       push eax
0x00442a95: e83660fcff               call 0x408ad0
0x00442a9a: d95c2414                 fstp dword ptr [esp + 0x14]
0x00442a9e: 8b4c2410                 mov ecx, dword ptr [esp + 0x10]
0x00442aa2: 51                       push ecx
0x00442aa3: e82860fcff               call 0x408ad0
0x00442aa8: d95c241c                 fstp dword ptr [esp + 0x1c]
0x00442aac: d9442418                 fld dword ptr [esp + 0x18]
0x00442ab0: 83c410                   add esp, 0x10
0x00442ab3: d81d30c75c00             fcomp dword ptr [0x5cc730]
0x00442ab9: dfe0                     fnstsw ax
0x00442abb: f6c441                   test ah, 0x41
0x00442abe: 7521                     jne 0x442ae1
0x00442ac0: d944240c                 fld dword ptr [esp + 0xc]
0x00442ac4: d81d6ccd5c00             fcomp dword ptr [0x5ccd6c]
0x00442aca: dfe0                     fnstsw ax
0x00442acc: f6c405                   test ah, 5
0x00442acf: 7a10                     jp 0x442ae1
0x00442ad1: d9442408                 fld dword ptr [esp + 8]
0x00442ad5: d82568c55c00             fsub dword ptr [0x5cc568]
0x00442adb: d95c2408                 fstp dword ptr [esp + 8]
0x00442adf: eb30                     jmp 0x442b11
0x00442ae1: d944240c                 fld dword ptr [esp + 0xc]
0x00442ae5: d81d30c75c00             fcomp dword ptr [0x5cc730]
0x00442aeb: dfe0                     fnstsw ax
0x00442aed: f6c441                   test ah, 0x41
0x00442af0: 751f                     jne 0x442b11
0x00442af2: d9442408                 fld dword ptr [esp + 8]
0x00442af6: d81d6ccd5c00             fcomp dword ptr [0x5ccd6c]
0x00442afc: dfe0                     fnstsw ax
0x00442afe: f6c405                   test ah, 5
0x00442b01: 7a0e                     jp 0x442b11
0x00442b03: d944240c                 fld dword ptr [esp + 0xc]
0x00442b07: d82568c55c00             fsub dword ptr [0x5cc568]
0x00442b0d: d95c240c                 fstp dword ptr [esp + 0xc]
0x00442b11: 8b4c2400                 mov ecx, dword ptr [esp]
0x00442b15: 56                       push esi
0x00442b16: 8d542420                 lea edx, [esp + 0x20]
0x00442b1a: 52                       push edx
0x00442b1b: 8d442420                 lea eax, [esp + 0x20]
0x00442b1f: 50                       push eax
0x00442b20: 51                       push ecx
0x00442b21: e88aa00200               call 0x46cbb0
0x00442b26: 8b4c2414                 mov ecx, dword ptr [esp + 0x14]
0x00442b2a: 8d542430                 lea edx, [esp + 0x30]
0x00442b2e: 52                       push edx
0x00442b2f: 8d442428                 lea eax, [esp + 0x28]
0x00442b33: 50                       push eax
0x00442b34: 51                       push ecx
0x00442b35: e876a00200               call 0x46cbb0
0x00442b3a: d9442424                 fld dword ptr [esp + 0x24]
0x00442b3e: d85c2428                 fcomp dword ptr [esp + 0x28]
0x00442b42: 83c418                   add esp, 0x18
0x00442b45: dfe0                     fnstsw ax
0x00442b47: f6c441                   test ah, 0x41
0x00442b4a: 751e                     jne 0x442b6a
0x00442b4c: 8b44241c                 mov eax, dword ptr [esp + 0x1c]
0x00442b50: 85c0                     test eax, eax
0x00442b52: 7434                     je 0x442b88
0x00442b54: 8b442418                 mov eax, dword ptr [esp + 0x18]
0x00442b58: 85c0                     test eax, eax
0x00442b5a: 0f850e010000             jne 0x442c6e
0x00442b60: 8b742408                 mov esi, dword ptr [esp + 8]
0x00442b64: 8b442404                 mov eax, dword ptr [esp + 4]
0x00442b68: eb26                     jmp 0x442b90
0x00442b6a: 8b442418                 mov eax, dword ptr [esp + 0x18]
0x00442b6e: 85c0                     test eax, eax
0x00442b70: 750a                     jne 0x442b7c
0x00442b72: 8b742408                 mov esi, dword ptr [esp + 8]
0x00442b76: 8b442404                 mov eax, dword ptr [esp + 4]
0x00442b7a: eb14                     jmp 0x442b90
0x00442b7c: 8b44241c                 mov eax, dword ptr [esp + 0x1c]
0x00442b80: 85c0                     test eax, eax
0x00442b82: 0f85e6000000             jne 0x442c6e
0x00442b88: 8b742404                 mov esi, dword ptr [esp + 4]
0x00442b8c: 8b442408                 mov eax, dword ptr [esp + 8]
0x00442b90: 56                       push esi
0x00442b91: 8935a8898900             mov dword ptr [0x8989a8], esi
0x00442b97: a3c8898900               mov dword ptr [0x8989c8], eax
0x00442b9c: e8cfb7fcff               call 0x40e370
0x00442ba1: 83c404                   add esp, 4
0x00442ba4: 85c0                     test eax, eax
0x00442ba6: 0f84c2000000             je 0x442c6e
0x00442bac: 56                       push esi
0x00442bad: e8fe9b0200               call 0x46c7b0
0x00442bb2: 83c404                   add esp, 4
0x00442bb5: 83f801                   cmp eax, 1
0x00442bb8: 0f85b0000000             jne 0x442c6e
0x00442bbe: 8d542414                 lea edx, [esp + 0x14]
0x00442bc2: 56                       push esi
0x00442bc3: 52                       push edx
0x00442bc4: e8d7a80200               call 0x46d4a0
0x00442bc9: 8b44241c                 mov eax, dword ptr [esp + 0x1c]
0x00442bcd: 8b4830                   mov ecx, dword ptr [eax + 0x30]
0x00442bd0: 8b5038                   mov edx, dword ptr [eax + 0x38]
0x00442bd3: 83c408                   add esp, 8
0x00442bd6: 33c0                     xor eax, eax
0x00442bd8: 894c2428                 mov dword ptr [esp + 0x28], ecx
0x00442bdc: 89542430                 mov dword ptr [esp + 0x30], edx
0x00442be0: 89442404                 mov dword ptr [esp + 4], eax
0x00442be4: 50                       push eax
0x00442be5: e886b7fcff               call 0x40e370
0x00442bea: 83c404                   add esp, 4
0x00442bed: 85c0                     test eax, eax
0x00442bef: 746b                     je 0x442c5c
0x00442bf1: 8b442404                 mov eax, dword ptr [esp + 4]
0x00442bf5: 50                       push eax
0x00442bf6: e8b59b0200               call 0x46c7b0
0x00442bfb: 83c404                   add esp, 4
0x00442bfe: 83f801                   cmp eax, 1
0x00442c01: 7559                     jne 0x442c5c
0x00442c03: 8b4c2404                 mov ecx, dword ptr [esp + 4]
0x00442c07: 51                       push ecx
0x00442c08: 8d542418                 lea edx, [esp + 0x18]
0x00442c0c: 52                       push edx
0x00442c0d: e88ea80200               call 0x46d4a0
0x00442c12: 8b44241c                 mov eax, dword ptr [esp + 0x1c]
0x00442c16: d94030                   fld dword ptr [eax + 0x30]
0x00442c19: d94038                   fld dword ptr [eax + 0x38]
0x00442c1c: 8d44243c                 lea eax, [esp + 0x3c]
0x00442c20: d9442430                 fld dword ptr [esp + 0x30]
0x00442c24: 50                       push eax
0x00442c25: d8e2                     fsub st(2)
0x00442c27: c744244400000000         mov dword ptr [esp + 0x44], 0
0x00442c2f: d95c2440                 fstp dword ptr [esp + 0x40]
0x00442c33: d944243c                 fld dword ptr [esp + 0x3c]
0x00442c37: d8e1                     fsub st(1)
0x00442c39: d95c2448                 fstp dword ptr [esp + 0x48]
0x00442c3d: ddd8                     fstp st(0)
0x00442c3f: ddd8                     fstp st(0)
0x00442c41: e87a0e0800               call 0x4c3ac0
0x00442c46: 8b442410                 mov eax, dword ptr [esp + 0x10]
0x00442c4a: d80dbcc95c00             fmul dword ptr [0x5cc9bc]
0x00442c50: 83c40c                   add esp, 0xc
0x00442c53: d91c85b0898900           fstp dword ptr [eax*4 + 0x8989b0]
0x00442c5a: eb04                     jmp 0x442c60
0x00442c5c: 8b442404                 mov eax, dword ptr [esp + 4]
0x00442c60: 40                       inc eax
0x00442c61: 83f804                   cmp eax, 4
0x00442c64: 89442404                 mov dword ptr [esp + 4], eax
0x00442c68: 0f8c76ffffff             jl 0x442be4
0x00442c6e: 5e                       pop esi
0x00442c6f: 83c43c                   add esp, 0x3c
