0x00432b30: 53                       push ebx
0x00432b31: 55                       push ebp
0x00432b32: 57                       push edi
0x00432b33: 33ed                     xor ebp, ebp
0x00432b35: 33ff                     xor edi, edi
0x00432b37: 3bc5                     cmp eax, ebp
0x00432b39: 7435                     je 0x432b70
0x00432b3b: 83f802                   cmp eax, 2
0x00432b3e: bb000008ff               mov ebx, 0xff080000
0x00432b43: 750d                     jne 0x432b52
0x00432b45: 896c2410                 mov dword ptr [esp + 0x10], ebp
0x00432b49: e88282ffff               call 0x42add0
0x00432b4e: 8bf8                     mov edi, eax
0x00432b50: eb39                     jmp 0x432b8b
0x00432b52: ff05f8e96700             inc dword ptr [0x67e9f8]
0x00432b58: e87382ffff               call 0x42add0
0x00432b5d: 8bf8                     mov edi, eax
0x00432b5f: 83cbff                   or ebx, 0xffffffff
0x00432b62: 3bfb                     cmp edi, ebx
0x00432b64: 7502                     jne 0x432b68
0x00432b66: 33ff                     xor edi, edi
0x00432b68: ff0df8e96700             dec dword ptr [0x67e9f8]
0x00432b6e: eb1e                     jmp 0x432b8e
0x00432b70: 392df8e96700             cmp dword ptr [0x67e9f8], ebp
0x00432b76: 7413                     je 0x432b8b
0x00432b78: bb000008ff               mov ebx, 0xff080000
0x00432b7d: e84e82ffff               call 0x42add0
0x00432b82: 8bf8                     mov edi, eax
0x00432b84: 83ffff                   cmp edi, -1
0x00432b87: 7502                     jne 0x432b8b
0x00432b89: 33ff                     xor edi, edi
0x00432b8b: 83cbff                   or ebx, 0xffffffff
0x00432b8e: 8b442410                 mov eax, dword ptr [esp + 0x10]
0x00432b92: 48                       dec eax
0x00432b93: 83f809                   cmp eax, 9
0x00432b96: 0f87bf040000             ja 0x43305b
0x00432b9c: ff248560304300           jmp dword ptr [eax*4 + 0x433060]
0x00432ba3: 55                       push ebp
0x00432ba4: 68ac010000               push 0x1ac
0x00432ba9: 6a40                     push 0x40
0x00432bab: 8bc6                     mov eax, esi
0x00432bad: ba000010ff               mov edx, 0xff100000
0x00432bb2: e85981ffff               call 0x42ad10
0x00432bb7: 8b06                     mov eax, dword ptr [esi]
0x00432bb9: 6bc034                   imul eax, eax, 0x34
0x00432bbc: 83c40c                   add esp, 0xc
0x00432bbf: c780e88a890042000000     mov dword ptr [eax + 0x898ae8], 0x42
0x00432bc9: e8528dffff               call 0x42b920
0x00432bce: 39442414                 cmp dword ptr [esp + 0x14], eax
0x00432bd2: 750b                     jne 0x432bdf
0x00432bd4: 8b0e                     mov ecx, dword ptr [esi]
0x00432bd6: 6bc934                   imul ecx, ecx, 0x34
0x00432bd9: 8999e88a8900             mov dword ptr [ecx + 0x898ae8], ebx
0x00432bdf: 4f                       dec edi
0x00432be0: 83ff09                   cmp edi, 9
0x00432be3: 0f873f030000             ja 0x432f28
0x00432be9: ff24bd88304300           jmp dword ptr [edi*4 + 0x433088]
0x00432bf0: 8b16                     mov edx, dword ptr [esi]
0x00432bf2: 6bd234                   imul edx, edx, 0x34
0x00432bf5: 899aec8a8900             mov dword ptr [edx + 0x898aec], ebx
0x00432bfb: 8b06                     mov eax, dword ptr [esi]
0x00432bfd: 6bc034                   imul eax, eax, 0x34
0x00432c00: c780c48a890000100000     mov dword ptr [eax + 0x898ac4], 0x1000
0x00432c0a: 8b0e                     mov ecx, dword ptr [esi]
0x00432c0c: 6bc934                   imul ecx, ecx, 0x34
0x00432c0f: c781d08a8900ff010000     mov dword ptr [ecx + 0x898ad0], 0x1ff
0x00432c19: 8b06                     mov eax, dword ptr [esi]
0x00432c1b: 5f                       pop edi
0x00432c1c: 40                       inc eax
0x00432c1d: 5d                       pop ebp
0x00432c1e: 8906                     mov dword ptr [esi], eax
0x00432c20: 5b                       pop ebx
0x00432c21: c3                       ret 
0x00432c22: 8b16                     mov edx, dword ptr [esi]
0x00432c24: 6bd234                   imul edx, edx, 0x34
0x00432c27: c782ec8a890043000000     mov dword ptr [edx + 0x898aec], 0x43
0x00432c31: e99d020000               jmp 0x432ed3
0x00432c36: 8b16                     mov edx, dword ptr [esi]
0x00432c38: 6bd234                   imul edx, edx, 0x34
0x00432c3b: c782ec8a890025020000     mov dword ptr [edx + 0x898aec], 0x225
0x00432c45: e989020000               jmp 0x432ed3
0x00432c4a: 8b16                     mov edx, dword ptr [esi]
0x00432c4c: 6bd234                   imul edx, edx, 0x34
0x00432c4f: c782ec8a890048000000     mov dword ptr [edx + 0x898aec], 0x48
0x00432c59: 8b06                     mov eax, dword ptr [esi]
0x00432c5b: 6bc034                   imul eax, eax, 0x34
0x00432c5e: c780ec8a890013000000     mov dword ptr [eax + 0x898aec], 0x13
0x00432c68: e998020000               jmp 0x432f05
0x00432c6d: 8b06                     mov eax, dword ptr [esi]
0x00432c6f: 6bc034                   imul eax, eax, 0x34
0x00432c72: c780ec8a890058000000     mov dword ptr [eax + 0x898aec], 0x58
0x00432c7c: e984020000               jmp 0x432f05
0x00432c81: 55                       push ebp
0x00432c82: 68ac010000               push 0x1ac
0x00432c87: 6a40                     push 0x40
0x00432c89: 8bc6                     mov eax, esi
0x00432c8b: ba000010ff               mov edx, 0xff100000
0x00432c90: e87b80ffff               call 0x42ad10
0x00432c95: 8b06                     mov eax, dword ptr [esi]
0x00432c97: 6bc034                   imul eax, eax, 0x34
0x00432c9a: 83c40c                   add esp, 0xc
0x00432c9d: 4f                       dec edi
0x00432c9e: 83ff09                   cmp edi, 9
0x00432ca1: c780e88a890043000000     mov dword ptr [eax + 0x898ae8], 0x43
0x00432cab: 0f8783030000             ja 0x433034
0x00432cb1: ff24bdb0304300           jmp dword ptr [edi*4 + 0x4330b0]
0x00432cb8: 8b0e                     mov ecx, dword ptr [esi]
0x00432cba: 6bc934                   imul ecx, ecx, 0x34
0x00432cbd: c781ec8a890042000000     mov dword ptr [ecx + 0x898aec], 0x42
0x00432cc7: e973010000               jmp 0x432e3f
0x00432ccc: 8b16                     mov edx, dword ptr [esi]
0x00432cce: 6bd234                   imul edx, edx, 0x34
0x00432cd1: c782ec8a890033010000     mov dword ptr [edx + 0x898aec], 0x133
0x00432cdb: e9f3010000               jmp 0x432ed3
0x00432ce0: 55                       push ebp
0x00432ce1: 68ac010000               push 0x1ac
0x00432ce6: 6a40                     push 0x40
0x00432ce8: 8bc6                     mov eax, esi
0x00432cea: ba000010ff               mov edx, 0xff100000
0x00432cef: e81c80ffff               call 0x42ad10
0x00432cf4: 8b16                     mov edx, dword ptr [esi]
0x00432cf6: 6bd234                   imul edx, edx, 0x34
0x00432cf9: 83c40c                   add esp, 0xc
0x00432cfc: 4f                       dec edi
0x00432cfd: 83ff09                   cmp edi, 9
0x00432d00: c782e88a890025020000     mov dword ptr [edx + 0x898ae8], 0x225
0x00432d0a: 0f8724030000             ja 0x433034
0x00432d10: ff24bdd8304300           jmp dword ptr [edi*4 + 0x4330d8]
0x00432d17: 8b06                     mov eax, dword ptr [esi]
0x00432d19: 6bc034                   imul eax, eax, 0x34
0x00432d1c: 8998ec8a8900             mov dword ptr [eax + 0x898aec], ebx
0x00432d22: 8b0e                     mov ecx, dword ptr [esi]
0x00432d24: 6bc934                   imul ecx, ecx, 0x34
0x00432d27: c781c48a890000100000     mov dword ptr [ecx + 0x898ac4], 0x1000
0x00432d31: 8b16                     mov edx, dword ptr [esi]
0x00432d33: 6bd234                   imul edx, edx, 0x34
0x00432d36: c782d08a8900ff010000     mov dword ptr [edx + 0x898ad0], 0x1ff
0x00432d40: 8b06                     mov eax, dword ptr [esi]
0x00432d42: 5f                       pop edi
0x00432d43: 40                       inc eax
0x00432d44: 5d                       pop ebp
0x00432d45: 8906                     mov dword ptr [esi], eax
0x00432d47: 5b                       pop ebx
0x00432d48: c3                       ret 
0x00432d49: 8b06                     mov eax, dword ptr [esi]
0x00432d4b: 6bc034                   imul eax, eax, 0x34
0x00432d4e: c780ec8a890043000000     mov dword ptr [eax + 0x898aec], 0x43
0x00432d58: e9a8010000               jmp 0x432f05
0x00432d5d: 8b06                     mov eax, dword ptr [esi]
0x00432d5f: 6bc034                   imul eax, eax, 0x34
0x00432d62: c780ec8a890042000000     mov dword ptr [eax + 0x898aec], 0x42
0x00432d6c: e994010000               jmp 0x432f05
0x00432d71: 55                       push ebp
0x00432d72: 68ac010000               push 0x1ac
0x00432d77: 6a40                     push 0x40
0x00432d79: 8bc6                     mov eax, esi
0x00432d7b: ba000023ff               mov edx, 0xff230000
0x00432d80: e88b7fffff               call 0x42ad10
0x00432d85: 83c40c                   add esp, 0xc
0x00432d88: e8337cffff               call 0x42a9c0
0x00432d8d: 8b16                     mov edx, dword ptr [esi]
0x00432d8f: 6bd234                   imul edx, edx, 0x34
0x00432d92: 4f                       dec edi
0x00432d93: 83ff09                   cmp edi, 9
0x00432d96: 8982e88a8900             mov dword ptr [edx + 0x898ae8], eax
0x00432d9c: 0f87c0000000             ja 0x432e62
0x00432da2: ff24bd00314300           jmp dword ptr [edi*4 + 0x433100]
0x00432da9: 8b06                     mov eax, dword ptr [esi]
0x00432dab: 6bc034                   imul eax, eax, 0x34
0x00432dae: c780ec8a890025020000     mov dword ptr [eax + 0x898aec], 0x225
0x00432db8: e948010000               jmp 0x432f05
0x00432dbd: 8b06                     mov eax, dword ptr [esi]
0x00432dbf: 6bc034                   imul eax, eax, 0x34
0x00432dc2: c780ec8a890048000000     mov dword ptr [eax + 0x898aec], 0x48
0x00432dcc: 8b0e                     mov ecx, dword ptr [esi]
0x00432dce: 6bc934                   imul ecx, ecx, 0x34
0x00432dd1: c781ec8a890013000000     mov dword ptr [ecx + 0x898aec], 0x13
0x00432ddb: eb62                     jmp 0x432e3f
0x00432ddd: 8b0e                     mov ecx, dword ptr [esi]
0x00432ddf: 6bc934                   imul ecx, ecx, 0x34
0x00432de2: c781ec8a890058000000     mov dword ptr [ecx + 0x898aec], 0x58
0x00432dec: eb51                     jmp 0x432e3f
0x00432dee: 55                       push ebp
0x00432def: 68ac010000               push 0x1ac
0x00432df4: 6a40                     push 0x40
0x00432df6: 8bc6                     mov eax, esi
0x00432df8: ba000010ff               mov edx, 0xff100000
0x00432dfd: e80e7fffff               call 0x42ad10
0x00432e02: 8b0e                     mov ecx, dword ptr [esi]
0x00432e04: 6bc934                   imul ecx, ecx, 0x34
0x00432e07: c781e88a890048000000     mov dword ptr [ecx + 0x898ae8], 0x48
0x00432e11: 8b16                     mov edx, dword ptr [esi]
0x00432e13: 6bd234                   imul edx, edx, 0x34
0x00432e16: 83c40c                   add esp, 0xc
0x00432e19: 4f                       dec edi
0x00432e1a: 83ff09                   cmp edi, 9
0x00432e1d: c782e88a890013000000     mov dword ptr [edx + 0x898ae8], 0x13
0x00432e27: 7739                     ja 0x432e62
0x00432e29: ff24bd28314300           jmp dword ptr [edi*4 + 0x433128]
0x00432e30: 8b0e                     mov ecx, dword ptr [esi]
0x00432e32: 6bc934                   imul ecx, ecx, 0x34
0x00432e35: c781ec8a890033010000     mov dword ptr [ecx + 0x898aec], 0x133
0x00432e3f: 8b16                     mov edx, dword ptr [esi]
0x00432e41: 6bd234                   imul edx, edx, 0x34
0x00432e44: 89aac48a8900             mov dword ptr [edx + 0x898ac4], ebp
0x00432e4a: 8b06                     mov eax, dword ptr [esi]
0x00432e4c: 6bc034                   imul eax, eax, 0x34
0x00432e4f: c780d08a8900ff010000     mov dword ptr [eax + 0x898ad0], 0x1ff
0x00432e59: 8b06                     mov eax, dword ptr [esi]
0x00432e5b: 5f                       pop edi
0x00432e5c: 40                       inc eax
0x00432e5d: 5d                       pop ebp
0x00432e5e: 8906                     mov dword ptr [esi], eax
0x00432e60: 5b                       pop ebx
0x00432e61: c3                       ret 
0x00432e62: 8b0e                     mov ecx, dword ptr [esi]
0x00432e64: 6bc934                   imul ecx, ecx, 0x34
0x00432e67: 8999ec8a8900             mov dword ptr [ecx + 0x898aec], ebx
0x00432e6d: 8b16                     mov edx, dword ptr [esi]
0x00432e6f: 6bd234                   imul edx, edx, 0x34
0x00432e72: c782c48a890001000000     mov dword ptr [edx + 0x898ac4], 1
0x00432e7c: 8b06                     mov eax, dword ptr [esi]
0x00432e7e: 6bc034                   imul eax, eax, 0x34
0x00432e81: 89a8d08a8900             mov dword ptr [eax + 0x898ad0], ebp
0x00432e87: 8b06                     mov eax, dword ptr [esi]
0x00432e89: 5f                       pop edi
0x00432e8a: 40                       inc eax
0x00432e8b: 5d                       pop ebp
0x00432e8c: 8906                     mov dword ptr [esi], eax
0x00432e8e: 5b                       pop ebx
0x00432e8f: c3                       ret 
0x00432e90: 55                       push ebp
0x00432e91: 68ac010000               push 0x1ac
0x00432e96: 6a40                     push 0x40
0x00432e98: 8bc6                     mov eax, esi
0x00432e9a: ba000011ff               mov edx, 0xff110000
0x00432e9f: e86c7effff               call 0x42ad10
0x00432ea4: 8b0e                     mov ecx, dword ptr [esi]
0x00432ea6: 6bc934                   imul ecx, ecx, 0x34
0x00432ea9: 83c40c                   add esp, 0xc
0x00432eac: 4f                       dec edi
0x00432ead: 83ff09                   cmp edi, 9
0x00432eb0: c781e88a890058000000     mov dword ptr [ecx + 0x898ae8], 0x58
0x00432eba: 776c                     ja 0x432f28
0x00432ebc: ff24bd50314300           jmp dword ptr [edi*4 + 0x433150]
0x00432ec3: e8f87affff               call 0x42a9c0
0x00432ec8: 8b16                     mov edx, dword ptr [esi]
0x00432eca: 6bd234                   imul edx, edx, 0x34
0x00432ecd: 8982ec8a8900             mov dword ptr [edx + 0x898aec], eax
0x00432ed3: 8b06                     mov eax, dword ptr [esi]
0x00432ed5: 6bc034                   imul eax, eax, 0x34
0x00432ed8: 89a8c48a8900             mov dword ptr [eax + 0x898ac4], ebp
0x00432ede: 8b0e                     mov ecx, dword ptr [esi]
0x00432ee0: 6bc934                   imul ecx, ecx, 0x34
0x00432ee3: c781d08a8900ff010000     mov dword ptr [ecx + 0x898ad0], 0x1ff
0x00432eed: 8b06                     mov eax, dword ptr [esi]
0x00432eef: 5f                       pop edi
0x00432ef0: 40                       inc eax
0x00432ef1: 5d                       pop ebp
0x00432ef2: 8906                     mov dword ptr [esi], eax
0x00432ef4: 5b                       pop ebx
0x00432ef5: c3                       ret 
0x00432ef6: 8b06                     mov eax, dword ptr [esi]
0x00432ef8: 6bc034                   imul eax, eax, 0x34
0x00432efb: c780ec8a890033010000     mov dword ptr [eax + 0x898aec], 0x133
0x00432f05: 8b0e                     mov ecx, dword ptr [esi]
0x00432f07: 6bc934                   imul ecx, ecx, 0x34
0x00432f0a: 89a9c48a8900             mov dword ptr [ecx + 0x898ac4], ebp
0x00432f10: 8b16                     mov edx, dword ptr [esi]
0x00432f12: 6bd234                   imul edx, edx, 0x34
0x00432f15: c782d08a8900ff010000     mov dword ptr [edx + 0x898ad0], 0x1ff
0x00432f1f: 8b06                     mov eax, dword ptr [esi]
0x00432f21: 5f                       pop edi
0x00432f22: 40                       inc eax
0x00432f23: 5d                       pop ebp
0x00432f24: 8906                     mov dword ptr [esi], eax
0x00432f26: 5b                       pop ebx
0x00432f27: c3                       ret 
0x00432f28: 8b06                     mov eax, dword ptr [esi]
0x00432f2a: 6bc034                   imul eax, eax, 0x34
0x00432f2d: 8998ec8a8900             mov dword ptr [eax + 0x898aec], ebx
0x00432f33: 8b0e                     mov ecx, dword ptr [esi]
0x00432f35: 6bc934                   imul ecx, ecx, 0x34
0x00432f38: c781c48a890001000000     mov dword ptr [ecx + 0x898ac4], 1
0x00432f42: 8b16                     mov edx, dword ptr [esi]
0x00432f44: 6bd234                   imul edx, edx, 0x34
0x00432f47: 89aad08a8900             mov dword ptr [edx + 0x898ad0], ebp
0x00432f4d: 8b06                     mov eax, dword ptr [esi]
0x00432f4f: 5f                       pop edi
0x00432f50: 40                       inc eax
0x00432f51: 5d                       pop ebp
0x00432f52: 8906                     mov dword ptr [esi], eax
0x00432f54: 5b                       pop ebx
0x00432f55: c3                       ret 
0x00432f56: 55                       push ebp
0x00432f57: 68ac010000               push 0x1ac
0x00432f5c: 6a40                     push 0x40
0x00432f5e: 8bc6                     mov eax, esi
0x00432f60: ba000010ff               mov edx, 0xff100000
0x00432f65: e8a67dffff               call 0x42ad10
0x00432f6a: 8b06                     mov eax, dword ptr [esi]
0x00432f6c: 6bc034                   imul eax, eax, 0x34
0x00432f6f: 83c40c                   add esp, 0xc
0x00432f72: 4f                       dec edi
0x00432f73: 83ff09                   cmp edi, 9
0x00432f76: c780e88a890033010000     mov dword ptr [eax + 0x898ae8], 0x133
0x00432f80: 0f87ae000000             ja 0x433034
0x00432f86: ff24bd78314300           jmp dword ptr [edi*4 + 0x433178]
0x00432f8d: 8b0e                     mov ecx, dword ptr [esi]
0x00432f8f: 6bc934                   imul ecx, ecx, 0x34
0x00432f92: 8999ec8a8900             mov dword ptr [ecx + 0x898aec], ebx
0x00432f98: 8b16                     mov edx, dword ptr [esi]
0x00432f9a: 6bd234                   imul edx, edx, 0x34
0x00432f9d: c782c48a890000100000     mov dword ptr [edx + 0x898ac4], 0x1000
0x00432fa7: e99efeffff               jmp 0x432e4a
0x00432fac: 8b0e                     mov ecx, dword ptr [esi]
0x00432fae: 6bc934                   imul ecx, ecx, 0x34
0x00432fb1: c781ec8a890043000000     mov dword ptr [ecx + 0x898aec], 0x43
0x00432fbb: e97ffeffff               jmp 0x432e3f
0x00432fc0: 8b0e                     mov ecx, dword ptr [esi]
0x00432fc2: 6bc934                   imul ecx, ecx, 0x34
0x00432fc5: c781ec8a890025020000     mov dword ptr [ecx + 0x898aec], 0x225
0x00432fcf: e96bfeffff               jmp 0x432e3f
0x00432fd4: e8e779ffff               call 0x42a9c0
0x00432fd9: 8b0e                     mov ecx, dword ptr [esi]
0x00432fdb: 6bc934                   imul ecx, ecx, 0x34
0x00432fde: 8981ec8a8900             mov dword ptr [ecx + 0x898aec], eax
0x00432fe4: e956feffff               jmp 0x432e3f
0x00432fe9: 8b0e                     mov ecx, dword ptr [esi]
0x00432feb: 6bc934                   imul ecx, ecx, 0x34
0x00432fee: c781ec8a890048000000     mov dword ptr [ecx + 0x898aec], 0x48
0x00432ff8: 8b16                     mov edx, dword ptr [esi]
0x00432ffa: 6bd234                   imul edx, edx, 0x34
0x00432ffd: c782ec8a890013000000     mov dword ptr [edx + 0x898aec], 0x13
0x00433007: e9c7feffff               jmp 0x432ed3
0x0043300c: 8b16                     mov edx, dword ptr [esi]
0x0043300e: 6bd234                   imul edx, edx, 0x34
0x00433011: c782ec8a890058000000     mov dword ptr [edx + 0x898aec], 0x58
0x0043301b: e9b3feffff               jmp 0x432ed3
0x00433020: 8b16                     mov edx, dword ptr [esi]
0x00433022: 6bd234                   imul edx, edx, 0x34
0x00433025: c782ec8a890042000000     mov dword ptr [edx + 0x898aec], 0x42
0x0043302f: e99ffeffff               jmp 0x432ed3
0x00433034: 8b16                     mov edx, dword ptr [esi]
0x00433036: 6bd234                   imul edx, edx, 0x34
0x00433039: 899aec8a8900             mov dword ptr [edx + 0x898aec], ebx
0x0043303f: 8b06                     mov eax, dword ptr [esi]
0x00433041: 6bc034                   imul eax, eax, 0x34
0x00433044: c780c48a890001000000     mov dword ptr [eax + 0x898ac4], 1
0x0043304e: 8b0e                     mov ecx, dword ptr [esi]
0x00433050: 6bc934                   imul ecx, ecx, 0x34
0x00433053: 89a9d08a8900             mov dword ptr [ecx + 0x898ad0], ebp
0x00433059: ff06                     inc dword ptr [esi]
0x0043305b: 5f                       pop edi
0x0043305c: 5d                       pop ebp
0x0043305d: 5b                       pop ebx
