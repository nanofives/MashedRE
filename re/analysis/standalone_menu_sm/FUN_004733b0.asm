0x004733b0: 51                       push ecx
0x004733b1: 53                       push ebx
0x004733b2: 56                       push esi
0x004733b3: 57                       push edi
0x004733b4: e8f784fbff               call 0x42b8b0
0x004733b9: 0fbfc0                   movsx eax, ax
0x004733bc: 8944240c                 mov dword ptr [esp + 0xc], eax
0x004733c0: db44240c                 fild dword ptr [esp + 0xc]
0x004733c4: d84c2414                 fmul dword ptr [esp + 0x14]
0x004733c8: d80da8d55c00             fmul dword ptr [0x5cd5a8]
0x004733ce: d95c2414                 fstp dword ptr [esp + 0x14]
0x004733d2: e8e984fbff               call 0x42b8c0
0x004733d7: 0fbfc8                   movsx ecx, ax
0x004733da: 894c240c                 mov dword ptr [esp + 0xc], ecx
0x004733de: db44240c                 fild dword ptr [esp + 0xc]
0x004733e2: d84c2418                 fmul dword ptr [esp + 0x18]
0x004733e6: d80d60c55c00             fmul dword ptr [0x5cc560]
0x004733ec: d95c2418                 fstp dword ptr [esp + 0x18]
0x004733f0: e8bb84fbff               call 0x42b8b0
0x004733f5: 0fbfd0                   movsx edx, ax
0x004733f8: 8954240c                 mov dword ptr [esp + 0xc], edx
0x004733fc: db44240c                 fild dword ptr [esp + 0xc]
0x00473400: d84c241c                 fmul dword ptr [esp + 0x1c]
0x00473404: d80da8d55c00             fmul dword ptr [0x5cd5a8]
0x0047340a: d95c241c                 fstp dword ptr [esp + 0x1c]
0x0047340e: e8ad84fbff               call 0x42b8c0
0x00473413: 8b542418                 mov edx, dword ptr [esp + 0x18]
0x00473417: 8b4c2414                 mov ecx, dword ptr [esp + 0x14]
0x0047341b: 0fbfc0                   movsx eax, ax
0x0047341e: 8944240c                 mov dword ptr [esp + 0xc], eax
0x00473422: 8bc2                     mov eax, edx
0x00473424: 890d208a8900             mov dword ptr [0x898a20], ecx
0x0047342a: db44240c                 fild dword ptr [esp + 0xc]
0x0047342e: 890d588a8900             mov dword ptr [0x898a58], ecx
0x00473434: a3408a8900               mov dword ptr [0x898a40], eax
0x00473439: 668b442426               mov ax, word ptr [esp + 0x26]
0x0047343e: d84c2420                 fmul dword ptr [esp + 0x20]
0x00473442: 8915248a8900             mov dword ptr [0x898a24], edx
0x00473448: 8b542424                 mov edx, dword ptr [esp + 0x24]
0x0047344c: 0fb6cc                   movzx ecx, ah
0x0047344f: d80d60c55c00             fmul dword ptr [0x5cc560]
0x00473455: 0fb6f2                   movzx esi, dl
0x00473458: d95c2420                 fstp dword ptr [esp + 0x20]
0x0047345c: d944241c                 fld dword ptr [esp + 0x1c]
0x00473460: d8442414                 fadd dword ptr [esp + 0x14]
0x00473464: c1e108                   shl ecx, 8
0x00473467: 0bce                     or ecx, esi
0x00473469: 0fb6d6                   movzx edx, dh
0x0047346c: d9153c8a8900             fst dword ptr [0x898a3c]
0x00473472: d9442420                 fld dword ptr [esp + 0x20]
0x00473476: d8442418                 fadd dword ptr [esp + 0x18]
0x0047347a: c1e108                   shl ecx, 8
0x0047347d: 0bca                     or ecx, edx
0x0047347f: 8b15f83f7d00             mov edx, dword ptr [0x7d3ff8]
0x00473485: d9155c8a8900             fst dword ptr [0x898a5c]
0x0047348b: 0fb6c0                   movzx eax, al
0x0047348e: d9c9                     fxch st(1)
0x00473490: d91d748a8900             fstp dword ptr [0x898a74]
0x00473496: c1e108                   shl ecx, 8
0x00473499: 0bc8                     or ecx, eax
0x0047349b: d91d788a8900             fstp dword ptr [0x898a78]
0x004734a1: b8288a8900               mov eax, 0x898a28
0x004734a6: be0000803f               mov esi, 0x3f800000
0x004734ab: eb03                     jmp 0x4734b0
0x004734ad: 8d4900                   lea ecx, [ecx]
0x004734b0: 894808                   mov dword ptr [eax + 8], ecx
0x004734b3: 8b7a18                   mov edi, dword ptr [edx + 0x18]
0x004734b6: 8938                     mov dword ptr [eax], edi
0x004734b8: 897004                   mov dword ptr [eax + 4], esi
0x004734bb: 83c01c                   add eax, 0x1c
