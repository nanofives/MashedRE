0x00554940: 81ecc8000000             sub esp, 0xc8
0x00554946: a1e02b9100               mov eax, dword ptr [0x912be0]
0x0055494b: 53                       push ebx
0x0055494c: 55                       push ebp
0x0055494d: 8bac24d4000000           mov ebp, dword ptr [esp + 0xd4]
0x00554954: 33db                     xor ebx, ebx
0x00554956: 89442428                 mov dword ptr [esp + 0x28], eax
0x0055495a: 3beb                     cmp ebp, ebx
0x0055495c: 0f845e080000             je 0x5551c0
0x00554962: 8b8d34010000             mov ecx, dword ptr [ebp + 0x134]
0x00554968: 51                       push ecx
0x00554969: e8c2030700               call 0x5c4d30
0x0055496e: 898424cc000000           mov dword ptr [esp + 0xcc], eax
0x00554975: 8a4510                   mov al, byte ptr [ebp + 0x10]
0x00554978: 83c404                   add esp, 4
0x0055497b: a802                     test al, 2
0x0055497d: 741a                     je 0x554999
0x0055497f: c784248000000002000000   mov dword ptr [esp + 0x80], 2
0x0055498a: c6442430ff               mov byte ptr [esp + 0x30], 0xff
0x0055498f: c744245001000000         mov dword ptr [esp + 0x50], 1
0x00554997: eb14                     jmp 0x5549ad
0x00554999: c784248000000001000000   mov dword ptr [esp + 0x80], 1
0x005549a4: c644243000               mov byte ptr [esp + 0x30], 0
0x005549a9: 895c2450                 mov dword ptr [esp + 0x50], ebx
0x005549ad: 56                       push esi
0x005549ae: 8bb424dc000000           mov esi, dword ptr [esp + 0xdc]
0x005549b5: 57                       push edi
0x005549b6: 8974243c                 mov dword ptr [esp + 0x3c], esi
0x005549ba: e851e3ffff               call 0x552d10
0x005549bf: 8b8424e8000000           mov eax, dword ptr [esp + 0xe8]
0x005549c6: 8b5004                   mov edx, dword ptr [eax + 4]
0x005549c9: 8b00                     mov eax, dword ptr [eax]
0x005549cb: 52                       push edx
0x005549cc: 50                       push eax
0x005549cd: e81ee4ffff               call 0x552df0
0x005549d2: 8b8424ec000000           mov eax, dword ptr [esp + 0xec]
0x005549d9: 50                       push eax
0x005549da: 50                       push eax
0x005549db: e8c0e3ffff               call 0x552da0
0x005549e0: d94508                   fld dword ptr [ebp + 8]
0x005549e3: 83c40c                   add esp, 0xc
0x005549e6: d9e0                     fchs
0x005549e8: d91c24                   fstp dword ptr [esp]
0x005549eb: 53                       push ebx
0x005549ec: e8ffe3ffff               call 0x552df0
0x005549f1: e84ae4ffff               call 0x552e40
0x005549f6: 680000803f               push 0x3f800000
0x005549fb: 56                       push esi
0x005549fc: 55                       push ebp
0x005549fd: 898424a0000000           mov dword ptr [esp + 0xa0], eax
0x00554a04: e8c70a0000               call 0x5554d0
0x00554a09: d83d20c35c00             fdivr dword ptr [0x5cc320]
0x00554a0f: 8b54246c                 mov edx, dword ptr [esp + 0x6c]
0x00554a13: 8b7c244c                 mov edi, dword ptr [esp + 0x4c]
0x00554a17: 33c0                     xor eax, eax
0x00554a19: 8b0d042a9100             mov ecx, dword ptr [0x912a04]
0x00554a1f: 8a0432                   mov al, byte ptr [edx + esi]
0x00554a22: 81e7ff000000             and edi, 0xff
0x00554a28: 23c7                     and eax, edi
0x00554a2a: 33d2                     xor edx, edx
0x00554a2c: 8a16                     mov dl, byte ptr [esi]
0x00554a2e: 83c414                   add esp, 0x14
0x00554a31: c1e008                   shl eax, 8
0x00554a34: 0bc2                     or eax, edx
0x00554a36: c744242c00000000         mov dword ptr [esp + 0x2c], 0
0x00554a3e: c744242800000000         mov dword ptr [esp + 0x28], 0
0x00554a46: 895c2450                 mov dword ptr [esp + 0x50], ebx
0x00554a4a: 89bc24cc000000           mov dword ptr [esp + 0xcc], edi
0x00554a51: d95c2434                 fstp dword ptr [esp + 0x34]
0x00554a55: 0f8e48070000             jle 0x5551a3
0x00554a5b: 8bb424ec000000           mov esi, dword ptr [esp + 0xec]
0x00554a62: 3d80000000               cmp eax, 0x80
0x00554a67: 7c21                     jl 0x554a8a
0x00554a69: 2b8524010000             sub eax, dword ptr [ebp + 0x124]
0x00554a6f: 7814                     js 0x554a85
0x00554a71: 3b8528010000             cmp eax, dword ptr [ebp + 0x128]
0x00554a77: 7d0c                     jge 0x554a85
0x00554a79: 8b952c010000             mov edx, dword ptr [ebp + 0x12c]
0x00554a7f: 0fbf3c42                 movsx edi, word ptr [edx + eax*2]
0x00554a83: eb0a                     jmp 0x554a8f
0x00554a85: 83cfff                   or edi, 0xffffffff
0x00554a88: eb05                     jmp 0x554a8f
0x00554a8a: 0fbf7c4524               movsx edi, word ptr [ebp + eax*2 + 0x24]
0x00554a8f: 8b842488000000           mov eax, dword ptr [esp + 0x88]
0x00554a96: 8b54243c                 mov edx, dword ptr [esp + 0x3c]
0x00554a9a: 03d0                     add edx, eax
0x00554a9c: 85ff                     test edi, edi
0x00554a9e: 8954243c                 mov dword ptr [esp + 0x3c], edx
0x00554aa2: 0f8c88060000             jl 0x555130
0x00554aa8: 8b9424d0000000           mov edx, dword ptr [esp + 0xd0]
0x00554aaf: 8b442450                 mov eax, dword ptr [esp + 0x50]
0x00554ab3: c1e705                   shl edi, 5
0x00554ab6: 03fa                     add edi, edx
0x00554ab8: 8b5704                   mov edx, dword ptr [edi + 4]
0x00554abb: 3bd0                     cmp edx, eax
0x00554abd: 750c                     jne 0x554acb
0x00554abf: 81fb7a010000             cmp ebx, 0x17a
0x00554ac5: 0f8ea5000000             jle 0x554b70
0x00554acb: 85db                     test ebx, ebx
0x00554acd: 744c                     je 0x554b1b
0x00554acf: 8b84248c000000           mov eax, dword ptr [esp + 0x8c]
0x00554ad6: 8b15042a9100             mov edx, dword ptr [0x912a04]
0x00554adc: 6a05                     push 5
0x00554ade: 50                       push eax
0x00554adf: 2bca                     sub ecx, edx
0x00554ae1: b8398ee338               mov eax, 0x38e38e39
0x00554ae6: f7e9                     imul ecx
0x00554ae8: c1fa03                   sar edx, 3
0x00554aeb: 8bca                     mov ecx, edx
0x00554aed: c1e91f                   shr ecx, 0x1f
0x00554af0: 03d1                     add edx, ecx
0x00554af2: 52                       push edx
0x00554af3: 8b15042a9100             mov edx, dword ptr [0x912a04]
0x00554af9: 52                       push edx
0x00554afa: e87185f7ff               call 0x4cd070
0x00554aff: 83c410                   add esp, 0x10
0x00554b02: 85c0                     test eax, eax
0x00554b04: 7415                     je 0x554b1b
0x00554b06: 53                       push ebx
0x00554b07: 6800279100               push 0x912700
0x00554b0c: 6a03                     push 3
0x00554b0e: e85d86f7ff               call 0x4cd170
0x00554b13: 83c40c                   add esp, 0xc
0x00554b16: e82586f7ff               call 0x4cd140
0x00554b1b: 8b4704                   mov eax, dword ptr [edi + 4]
0x00554b1e: 8b4c2450                 mov ecx, dword ptr [esp + 0x50]
0x00554b22: 3bc1                     cmp eax, ecx
0x00554b24: 742f                     je 0x554b55
0x00554b26: 8b4050                   mov eax, dword ptr [eax + 0x50]
0x00554b29: 8b0df83f7d00             mov ecx, dword ptr [0x7d3ff8]
0x00554b2f: 25ff000000               and eax, 0xff
0x00554b34: 50                       push eax
0x00554b35: 6a09                     push 9
0x00554b37: ff5120                   call dword ptr [ecx + 0x20]
0x00554b3a: 8b5704                   mov edx, dword ptr [edi + 4]
0x00554b3d: 8b0df83f7d00             mov ecx, dword ptr [0x7d3ff8]
0x00554b43: 8b02                     mov eax, dword ptr [edx]
0x00554b45: 50                       push eax
0x00554b46: 6a01                     push 1
0x00554b48: ff5120                   call dword ptr [ecx + 0x20]
0x00554b4b: 8b5704                   mov edx, dword ptr [edi + 4]
0x00554b4e: 83c410                   add esp, 0x10
0x00554b51: 89542450                 mov dword ptr [esp + 0x50], edx
0x00554b55: 8b4668                   mov eax, dword ptr [esi + 0x68]
0x00554b58: 8b0d042a9100             mov ecx, dword ptr [0x912a04]
0x00554b5e: 33db                     xor ebx, ebx
0x00554b60: 85c0                     test eax, eax
0x00554b62: 740c                     je 0x554b70
0x00554b64: 8b4664                   mov eax, dword ptr [esi + 0x64]
0x00554b67: a802                     test al, 2
0x00554b69: 7405                     je 0x554b70
0x00554b6b: 24fd                     and al, 0xfd
0x00554b6d: 894664                   mov dword ptr [esi + 0x64], eax
0x00554b70: 8b442428                 mov eax, dword ptr [esp + 0x28]
0x00554b74: 8b542430                 mov edx, dword ptr [esp + 0x30]
0x00554b78: 8901                     mov dword ptr [ecx], eax
0x00554b7a: c7410400000000           mov dword ptr [ecx + 4], 0
0x00554b81: 895108                   mov dword ptr [ecx + 8], edx
0x00554b84: d94708                   fld dword ptr [edi + 8]
0x00554b87: d9591c                   fstp dword ptr [ecx + 0x1c]
0x00554b8a: d94714                   fld dword ptr [edi + 0x14]
0x00554b8d: d95920                   fstp dword ptr [ecx + 0x20]
0x00554b90: f6466401                 test byte ptr [esi + 0x64], 1
0x00554b94: 0f8417010000             je 0x554cb1
0x00554b9a: d9442434                 fld dword ptr [esp + 0x34]
0x00554b9e: d84c242c                 fmul dword ptr [esp + 0x2c]
0x00554ba2: d94648                   fld dword ptr [esi + 0x48]
0x00554ba5: d8c9                     fmul st(1)
0x00554ba7: d9464c                   fld dword ptr [esi + 0x4c]
0x00554baa: d8ca                     fmul st(2)
0x00554bac: d95c2414                 fstp dword ptr [esp + 0x14]
0x00554bb0: d94650                   fld dword ptr [esi + 0x50]
0x00554bb3: d8ca                     fmul st(2)
0x00554bb5: d95c2418                 fstp dword ptr [esp + 0x18]
0x00554bb9: d94654                   fld dword ptr [esi + 0x54]
0x00554bbc: d8ca                     fmul st(2)
0x00554bbe: d95c241c                 fstp dword ptr [esp + 0x1c]
0x00554bc2: d94630                   fld dword ptr [esi + 0x30]
0x00554bc5: d8c1                     fadd st(1)
0x00554bc7: d95c2410                 fstp dword ptr [esp + 0x10]
0x00554bcb: ddd8                     fstp st(0)
0x00554bcd: ddd8                     fstp st(0)
0x00554bcf: d94634                   fld dword ptr [esi + 0x34]
0x00554bd2: d8442414                 fadd dword ptr [esp + 0x14]
0x00554bd6: d95c2414                 fstp dword ptr [esp + 0x14]
0x00554bda: d94638                   fld dword ptr [esi + 0x38]
0x00554bdd: d8442418                 fadd dword ptr [esp + 0x18]
0x00554be1: d95c2418                 fstp dword ptr [esp + 0x18]
0x00554be5: d9463c                   fld dword ptr [esi + 0x3c]
0x00554be8: d844241c                 fadd dword ptr [esp + 0x1c]
0x00554bec: d95c241c                 fstp dword ptr [esp + 0x1c]
0x00554bf0: d97c2466                 fnstcw word ptr [esp + 0x66]
0x00554bf4: d9442410                 fld dword ptr [esp + 0x10]
0x00554bf8: 668b442466               mov ax, word ptr [esp + 0x66]
0x00554bfd: 80cc0c                   or ah, 0xc
0x00554c00: 668944245e               mov word ptr [esp + 0x5e], ax
0x00554c05: d96c245e                 fldcw word ptr [esp + 0x5e]
0x00554c09: db9c2498000000           fistp dword ptr [esp + 0x98]
0x00554c10: d96c2466                 fldcw word ptr [esp + 0x66]
0x00554c14: d97c2442                 fnstcw word ptr [esp + 0x42]
0x00554c18: d9442414                 fld dword ptr [esp + 0x14]
0x00554c1c: 668b442442               mov ax, word ptr [esp + 0x42]
0x00554c21: 80cc0c                   or ah, 0xc
0x00554c24: 668944246a               mov word ptr [esp + 0x6a], ax
0x00554c29: d96c246a                 fldcw word ptr [esp + 0x6a]
0x00554c2d: db9c24b8000000           fistp dword ptr [esp + 0xb8]
0x00554c34: d96c2442                 fldcw word ptr [esp + 0x42]
0x00554c38: d97c2448                 fnstcw word ptr [esp + 0x48]
0x00554c3c: d9442418                 fld dword ptr [esp + 0x18]
0x00554c40: 668b442448               mov ax, word ptr [esp + 0x48]
0x00554c45: 80cc0c                   or ah, 0xc
0x00554c48: 6689442444               mov word ptr [esp + 0x44], ax
0x00554c4d: d96c2444                 fldcw word ptr [esp + 0x44]
0x00554c51: db9c24a0000000           fistp dword ptr [esp + 0xa0]
0x00554c58: d96c2448                 fldcw word ptr [esp + 0x48]
0x00554c5c: d97c2460                 fnstcw word ptr [esp + 0x60]
0x00554c60: d944241c                 fld dword ptr [esp + 0x1c]
0x00554c64: 668b442460               mov ax, word ptr [esp + 0x60]
0x00554c69: 80cc0c                   or ah, 0xc
0x00554c6c: 668944244c               mov word ptr [esp + 0x4c], ax
0x00554c71: d96c244c                 fldcw word ptr [esp + 0x4c]
0x00554c75: db9c24d4000000           fistp dword ptr [esp + 0xd4]
0x00554c7c: d96c2460                 fldcw word ptr [esp + 0x60]
0x00554c80: 8b9424b8000000           mov edx, dword ptr [esp + 0xb8]
0x00554c87: 33c0                     xor eax, eax
0x00554c89: 8aa424d4000000           mov ah, byte ptr [esp + 0xd4]
0x00554c90: 81e2ff000000             and edx, 0xff
0x00554c96: 8a842498000000           mov al, byte ptr [esp + 0x98]
0x00554c9d: c1e008                   shl eax, 8
0x00554ca0: 0bc2                     or eax, edx
0x00554ca2: 8b9424a0000000           mov edx, dword ptr [esp + 0xa0]
0x00554ca9: 81e2ff000000             and edx, 0xff
0x00554caf: eb17                     jmp 0x554cc8
0x00554cb1: 33c0                     xor eax, eax
0x00554cb3: 33d2                     xor edx, edx
0x00554cb5: 8a6663                   mov ah, byte ptr [esi + 0x63]
0x00554cb8: 8a5661                   mov dl, byte ptr [esi + 0x61]
0x00554cbb: 8a4660                   mov al, byte ptr [esi + 0x60]
0x00554cbe: c1e008                   shl eax, 8
0x00554cc1: 0bc2                     or eax, edx
0x00554cc3: 33d2                     xor edx, edx
0x00554cc5: 8a5662                   mov dl, byte ptr [esi + 0x62]
0x00554cc8: c1e008                   shl eax, 8
0x00554ccb: 0bc2                     or eax, edx
0x00554ccd: 8b542430                 mov edx, dword ptr [esp + 0x30]
0x00554cd1: 894118                   mov dword ptr [ecx + 0x18], eax
0x00554cd4: 8b442428                 mov eax, dword ptr [esp + 0x28]
0x00554cd8: 83c124                   add ecx, 0x24
0x00554cdb: 8901                     mov dword ptr [ecx], eax
0x00554cdd: c741040000803f           mov dword ptr [ecx + 4], 0x3f800000
0x00554ce4: 895108                   mov dword ptr [ecx + 8], edx
0x00554ce7: d94708                   fld dword ptr [edi + 8]
0x00554cea: d9591c                   fstp dword ptr [ecx + 0x1c]
0x00554ced: d9470c                   fld dword ptr [edi + 0xc]
0x00554cf0: d95920                   fstp dword ptr [ecx + 0x20]
0x00554cf3: f6466401                 test byte ptr [esi + 0x64], 1
0x00554cf7: 0f8412010000             je 0x554e0f
0x00554cfd: d9442434                 fld dword ptr [esp + 0x34]
0x00554d01: d84c242c                 fmul dword ptr [esp + 0x2c]
0x00554d05: d94618                   fld dword ptr [esi + 0x18]
0x00554d08: d8c9                     fmul st(1)
0x00554d0a: d9461c                   fld dword ptr [esi + 0x1c]
0x00554d0d: d8ca                     fmul st(2)
0x00554d0f: d95c2414                 fstp dword ptr [esp + 0x14]
0x00554d13: d94620                   fld dword ptr [esi + 0x20]
0x00554d16: d8ca                     fmul st(2)
0x00554d18: d95c2418                 fstp dword ptr [esp + 0x18]
0x00554d1c: d94624                   fld dword ptr [esi + 0x24]
0x00554d1f: d8ca                     fmul st(2)
0x00554d21: d95c241c                 fstp dword ptr [esp + 0x1c]
0x00554d25: d806                     fadd dword ptr [esi]
0x00554d27: d95c2410                 fstp dword ptr [esp + 0x10]
0x00554d2b: ddd8                     fstp st(0)
0x00554d2d: d94604                   fld dword ptr [esi + 4]
0x00554d30: d8442414                 fadd dword ptr [esp + 0x14]
0x00554d34: d95c2414                 fstp dword ptr [esp + 0x14]
0x00554d38: d94608                   fld dword ptr [esi + 8]
0x00554d3b: d8442418                 fadd dword ptr [esp + 0x18]
0x00554d3f: d95c2418                 fstp dword ptr [esp + 0x18]
0x00554d43: d9460c                   fld dword ptr [esi + 0xc]
0x00554d46: d844241c                 fadd dword ptr [esp + 0x1c]
0x00554d4a: d95c241c                 fstp dword ptr [esp + 0x1c]
0x00554d4e: d97c2446                 fnstcw word ptr [esp + 0x46]
0x00554d52: d9442410                 fld dword ptr [esp + 0x10]
0x00554d56: 668b442446               mov ax, word ptr [esp + 0x46]
0x00554d5b: 80cc0c                   or ah, 0xc
0x00554d5e: 668944246e               mov word ptr [esp + 0x6e], ax
0x00554d63: d96c246e                 fldcw word ptr [esp + 0x6e]
0x00554d67: db9c24a8000000           fistp dword ptr [esp + 0xa8]
0x00554d6e: d96c2446                 fldcw word ptr [esp + 0x46]
0x00554d72: d97c244a                 fnstcw word ptr [esp + 0x4a]
0x00554d76: d9442414                 fld dword ptr [esp + 0x14]
0x00554d7a: 668b44244a               mov ax, word ptr [esp + 0x4a]
0x00554d7f: 80cc0c                   or ah, 0xc
0x00554d82: 6689442472               mov word ptr [esp + 0x72], ax
0x00554d87: d96c2472                 fldcw word ptr [esp + 0x72]
0x00554d8b: db9c24c0000000           fistp dword ptr [esp + 0xc0]
0x00554d92: d96c244a                 fldcw word ptr [esp + 0x4a]
0x00554d96: d97c244e                 fnstcw word ptr [esp + 0x4e]
0x00554d9a: d9442418                 fld dword ptr [esp + 0x18]
0x00554d9e: 668b44244e               mov ax, word ptr [esp + 0x4e]
0x00554da3: 80cc0c                   or ah, 0xc
0x00554da6: 6689442476               mov word ptr [esp + 0x76], ax
0x00554dab: d96c2476                 fldcw word ptr [esp + 0x76]
0x00554daf: db9c24b0000000           fistp dword ptr [esp + 0xb0]
0x00554db6: d96c244e                 fldcw word ptr [esp + 0x4e]
0x00554dba: d97c2456                 fnstcw word ptr [esp + 0x56]
0x00554dbe: d944241c                 fld dword ptr [esp + 0x1c]
0x00554dc2: 668b442456               mov ax, word ptr [esp + 0x56]
0x00554dc7: 80cc0c                   or ah, 0xc
0x00554dca: 668944247a               mov word ptr [esp + 0x7a], ax
0x00554dcf: d96c247a                 fldcw word ptr [esp + 0x7a]
0x00554dd3: db9c24c8000000           fistp dword ptr [esp + 0xc8]
0x00554dda: d96c2456                 fldcw word ptr [esp + 0x56]
0x00554dde: 8b9424c0000000           mov edx, dword ptr [esp + 0xc0]
0x00554de5: 33c0                     xor eax, eax
0x00554de7: 8aa424c8000000           mov ah, byte ptr [esp + 0xc8]
0x00554dee: 81e2ff000000             and edx, 0xff
0x00554df4: 8a8424a8000000           mov al, byte ptr [esp + 0xa8]
0x00554dfb: c1e008                   shl eax, 8
0x00554dfe: 0bc2                     or eax, edx
0x00554e00: 8b9424b0000000           mov edx, dword ptr [esp + 0xb0]
0x00554e07: 81e2ff000000             and edx, 0xff
0x00554e0d: eb17                     jmp 0x554e26
0x00554e0f: 33c0                     xor eax, eax
0x00554e11: 33d2                     xor edx, edx
0x00554e13: 8a6663                   mov ah, byte ptr [esi + 0x63]
0x00554e16: 8a5661                   mov dl, byte ptr [esi + 0x61]
0x00554e19: 8a4660                   mov al, byte ptr [esi + 0x60]
0x00554e1c: c1e008                   shl eax, 8
0x00554e1f: 0bc2                     or eax, edx
0x00554e21: 33d2                     xor edx, edx
0x00554e23: 8a5662                   mov dl, byte ptr [esi + 0x62]
0x00554e26: d9442428                 fld dword ptr [esp + 0x28]
0x00554e2a: c1e008                   shl eax, 8
0x00554e2d: 0bc2                     or eax, edx
0x00554e2f: 894118                   mov dword ptr [ecx + 0x18], eax
0x00554e32: 8b442430                 mov eax, dword ptr [esp + 0x30]
0x00554e36: d807                     fadd dword ptr [edi]
0x00554e38: 83c124                   add ecx, 0x24
0x00554e3b: c7410400000000           mov dword ptr [ecx + 4], 0
0x00554e42: 894108                   mov dword ptr [ecx + 8], eax
0x00554e45: d919                     fstp dword ptr [ecx]
0x00554e47: d94710                   fld dword ptr [edi + 0x10]
0x00554e4a: d9591c                   fstp dword ptr [ecx + 0x1c]
0x00554e4d: d94714                   fld dword ptr [edi + 0x14]
0x00554e50: d95920                   fstp dword ptr [ecx + 0x20]
0x00554e53: f6466401                 test byte ptr [esi + 0x64], 1
0x00554e57: 0f842c010000             je 0x554f89
0x00554e5d: d944242c                 fld dword ptr [esp + 0x2c]
0x00554e61: d807                     fadd dword ptr [edi]
0x00554e63: d84c2434                 fmul dword ptr [esp + 0x34]
0x00554e67: d94648                   fld dword ptr [esi + 0x48]
0x00554e6a: d8c9                     fmul st(1)
0x00554e6c: d9464c                   fld dword ptr [esi + 0x4c]
0x00554e6f: d8ca                     fmul st(2)
0x00554e71: d95c2414                 fstp dword ptr [esp + 0x14]
0x00554e75: d94650                   fld dword ptr [esi + 0x50]
0x00554e78: d8ca                     fmul st(2)
0x00554e7a: d95c2418                 fstp dword ptr [esp + 0x18]
0x00554e7e: d94654                   fld dword ptr [esi + 0x54]
0x00554e81: d8ca                     fmul st(2)
0x00554e83: d95c241c                 fstp dword ptr [esp + 0x1c]
0x00554e87: d94630                   fld dword ptr [esi + 0x30]
0x00554e8a: d8c1                     fadd st(1)
0x00554e8c: d95c2410                 fstp dword ptr [esp + 0x10]
0x00554e90: ddd8                     fstp st(0)
0x00554e92: ddd8                     fstp st(0)
0x00554e94: d94634                   fld dword ptr [esi + 0x34]
0x00554e97: d8442414                 fadd dword ptr [esp + 0x14]
0x00554e9b: d95c2414                 fstp dword ptr [esp + 0x14]
0x00554e9f: d94638                   fld dword ptr [esi + 0x38]
0x00554ea2: d8442418                 fadd dword ptr [esp + 0x18]
0x00554ea6: d95c2418                 fstp dword ptr [esp + 0x18]
0x00554eaa: d9463c                   fld dword ptr [esi + 0x3c]
0x00554ead: d844241c                 fadd dword ptr [esp + 0x1c]
0x00554eb1: d95c241c                 fstp dword ptr [esp + 0x1c]
0x00554eb5: d97c245c                 fnstcw word ptr [esp + 0x5c]
0x00554eb9: d9442410                 fld dword ptr [esp + 0x10]
0x00554ebd: 668b44245c               mov ax, word ptr [esp + 0x5c]
0x00554ec2: 80cc0c                   or ah, 0xc
0x00554ec5: 668944247e               mov word ptr [esp + 0x7e], ax
0x00554eca: d96c247e                 fldcw word ptr [esp + 0x7e]
0x00554ece: db9c2494000000           fistp dword ptr [esp + 0x94]
0x00554ed5: d96c245c                 fldcw word ptr [esp + 0x5c]
0x00554ed9: d9bc2486000000           fnstcw word ptr [esp + 0x86]
0x00554ee0: d9442414                 fld dword ptr [esp + 0x14]
0x00554ee4: 668b842486000000         mov ax, word ptr [esp + 0x86]
0x00554eec: 80cc0c                   or ah, 0xc
0x00554eef: 6689842482000000         mov word ptr [esp + 0x82], ax
0x00554ef7: d9ac2482000000           fldcw word ptr [esp + 0x82]
0x00554efe: db9c249c000000           fistp dword ptr [esp + 0x9c]
0x00554f05: d9ac2486000000           fldcw word ptr [esp + 0x86]
0x00554f0c: d97c2464                 fnstcw word ptr [esp + 0x64]
0x00554f10: d9442418                 fld dword ptr [esp + 0x18]
0x00554f14: 668b442464               mov ax, word ptr [esp + 0x64]
0x00554f19: 80cc0c                   or ah, 0xc
0x00554f1c: 6689442474               mov word ptr [esp + 0x74], ax
0x00554f21: d96c2474                 fldcw word ptr [esp + 0x74]
0x00554f25: db9c24a4000000           fistp dword ptr [esp + 0xa4]
0x00554f2c: d96c2464                 fldcw word ptr [esp + 0x64]
0x00554f30: d97c2468                 fnstcw word ptr [esp + 0x68]
0x00554f34: d944241c                 fld dword ptr [esp + 0x1c]
0x00554f38: 668b442468               mov ax, word ptr [esp + 0x68]
0x00554f3d: 80cc0c                   or ah, 0xc
0x00554f40: 6689842484000000         mov word ptr [esp + 0x84], ax
0x00554f48: d9ac2484000000           fldcw word ptr [esp + 0x84]
0x00554f4f: db9c2490000000           fistp dword ptr [esp + 0x90]
0x00554f56: d96c2468                 fldcw word ptr [esp + 0x68]
0x00554f5a: 8b84249c000000           mov eax, dword ptr [esp + 0x9c]
0x00554f61: 33d2                     xor edx, edx
0x00554f63: 8ab42490000000           mov dh, byte ptr [esp + 0x90]
0x00554f6a: 25ff000000               and eax, 0xff
0x00554f6f: 8a942494000000           mov dl, byte ptr [esp + 0x94]
0x00554f76: c1e208                   shl edx, 8
0x00554f79: 0bd0                     or edx, eax
0x00554f7b: 8b8424a4000000           mov eax, dword ptr [esp + 0xa4]
0x00554f82: 25ff000000               and eax, 0xff
0x00554f87: eb17                     jmp 0x554fa0
0x00554f89: 33d2                     xor edx, edx
0x00554f8b: 33c0                     xor eax, eax
0x00554f8d: 8a7663                   mov dh, byte ptr [esi + 0x63]
0x00554f90: 8a4661                   mov al, byte ptr [esi + 0x61]
0x00554f93: 8a5660                   mov dl, byte ptr [esi + 0x60]
0x00554f96: c1e208                   shl edx, 8
0x00554f99: 0bd0                     or edx, eax
0x00554f9b: 33c0                     xor eax, eax
0x00554f9d: 8a4662                   mov al, byte ptr [esi + 0x62]
0x00554fa0: d9442428                 fld dword ptr [esp + 0x28]
0x00554fa4: c1e208                   shl edx, 8
0x00554fa7: 0bd0                     or edx, eax
0x00554fa9: 895118                   mov dword ptr [ecx + 0x18], edx
0x00554fac: 8b542430                 mov edx, dword ptr [esp + 0x30]
0x00554fb0: d807                     fadd dword ptr [edi]
0x00554fb2: 83c124                   add ecx, 0x24
0x00554fb5: c741040000803f           mov dword ptr [ecx + 4], 0x3f800000
0x00554fbc: 895108                   mov dword ptr [ecx + 8], edx
0x00554fbf: d919                     fstp dword ptr [ecx]
0x00554fc1: d94710                   fld dword ptr [edi + 0x10]
0x00554fc4: d9591c                   fstp dword ptr [ecx + 0x1c]
0x00554fc7: d9470c                   fld dword ptr [edi + 0xc]
0x00554fca: d95920                   fstp dword ptr [ecx + 0x20]
0x00554fcd: f6466401                 test byte ptr [esi + 0x64], 1
0x00554fd1: 0f841a010000             je 0x5550f1
0x00554fd7: d944242c                 fld dword ptr [esp + 0x2c]
0x00554fdb: d807                     fadd dword ptr [edi]
0x00554fdd: d84c2434                 fmul dword ptr [esp + 0x34]
0x00554fe1: d94618                   fld dword ptr [esi + 0x18]
0x00554fe4: d8c9                     fmul st(1)
0x00554fe6: d9461c                   fld dword ptr [esi + 0x1c]
0x00554fe9: d8ca                     fmul st(2)
0x00554feb: d95c2414                 fstp dword ptr [esp + 0x14]
0x00554fef: d94620                   fld dword ptr [esi + 0x20]
0x00554ff2: d8ca                     fmul st(2)
0x00554ff4: d95c2418                 fstp dword ptr [esp + 0x18]
0x00554ff8: d94624                   fld dword ptr [esi + 0x24]
0x00554ffb: d8ca                     fmul st(2)
0x00554ffd: d95c241c                 fstp dword ptr [esp + 0x1c]
0x00555001: d806                     fadd dword ptr [esi]
0x00555003: d95c2410                 fstp dword ptr [esp + 0x10]
0x00555007: ddd8                     fstp st(0)
0x00555009: d94604                   fld dword ptr [esi + 4]
0x0055500c: d8442414                 fadd dword ptr [esp + 0x14]
0x00555010: d95c2414                 fstp dword ptr [esp + 0x14]
0x00555014: d94608                   fld dword ptr [esi + 8]
0x00555017: d8442418                 fadd dword ptr [esp + 0x18]
0x0055501b: d95c2418                 fstp dword ptr [esp + 0x18]
0x0055501f: d9460c                   fld dword ptr [esi + 0xc]
0x00555022: d844241c                 fadd dword ptr [esp + 0x1c]
0x00555026: d95c241c                 fstp dword ptr [esp + 0x1c]
0x0055502a: d97c246c                 fnstcw word ptr [esp + 0x6c]
0x0055502e: d9442410                 fld dword ptr [esp + 0x10]
0x00555032: 668b44246c               mov ax, word ptr [esp + 0x6c]
0x00555037: 80cc0c                   or ah, 0xc
0x0055503a: 6689442478               mov word ptr [esp + 0x78], ax
0x0055503f: d96c2478                 fldcw word ptr [esp + 0x78]
0x00555043: db9c24b4000000           fistp dword ptr [esp + 0xb4]
0x0055504a: d96c246c                 fldcw word ptr [esp + 0x6c]
0x0055504e: d97c2470                 fnstcw word ptr [esp + 0x70]
0x00555052: d9442414                 fld dword ptr [esp + 0x14]
0x00555056: 668b442470               mov ax, word ptr [esp + 0x70]
0x0055505b: 80cc0c                   or ah, 0xc
0x0055505e: 6689842480000000         mov word ptr [esp + 0x80], ax
0x00555066: d9ac2480000000           fldcw word ptr [esp + 0x80]
0x0055506d: db9c24bc000000           fistp dword ptr [esp + 0xbc]
0x00555074: d96c2470                 fldcw word ptr [esp + 0x70]
0x00555078: d97c2440                 fnstcw word ptr [esp + 0x40]
0x0055507c: d9442418                 fld dword ptr [esp + 0x18]
0x00555080: 668b442440               mov ax, word ptr [esp + 0x40]
0x00555085: 80cc0c                   or ah, 0xc
0x00555088: 668944247c               mov word ptr [esp + 0x7c], ax
0x0055508d: d96c247c                 fldcw word ptr [esp + 0x7c]
0x00555091: db9c24c4000000           fistp dword ptr [esp + 0xc4]
0x00555098: d96c2440                 fldcw word ptr [esp + 0x40]
0x0055509c: d97c2438                 fnstcw word ptr [esp + 0x38]
0x005550a0: d944241c                 fld dword ptr [esp + 0x1c]
0x005550a4: 668b442438               mov ax, word ptr [esp + 0x38]
0x005550a9: 80cc0c                   or ah, 0xc
0x005550ac: 6689442462               mov word ptr [esp + 0x62], ax
0x005550b1: d96c2462                 fldcw word ptr [esp + 0x62]
0x005550b5: db9c24ac000000           fistp dword ptr [esp + 0xac]
0x005550bc: d96c2438                 fldcw word ptr [esp + 0x38]
0x005550c0: 8b9424bc000000           mov edx, dword ptr [esp + 0xbc]
0x005550c7: 33c0                     xor eax, eax
0x005550c9: 8aa424ac000000           mov ah, byte ptr [esp + 0xac]
0x005550d0: 81e2ff000000             and edx, 0xff
0x005550d6: 8a8424b4000000           mov al, byte ptr [esp + 0xb4]
0x005550dd: c1e008                   shl eax, 8
0x005550e0: 0bc2                     or eax, edx
0x005550e2: 8b9424c4000000           mov edx, dword ptr [esp + 0xc4]
0x005550e9: 81e2ff000000             and edx, 0xff
0x005550ef: eb17                     jmp 0x555108
0x005550f1: 33c0                     xor eax, eax
0x005550f3: 33d2                     xor edx, edx
0x005550f5: 8a6663                   mov ah, byte ptr [esi + 0x63]
0x005550f8: 8a5661                   mov dl, byte ptr [esi + 0x61]
0x005550fb: 8a4660                   mov al, byte ptr [esi + 0x60]
0x005550fe: c1e008                   shl eax, 8
0x00555101: 0bc2                     or eax, edx
0x00555103: 33d2                     xor edx, edx
0x00555105: 8a5662                   mov dl, byte ptr [esi + 0x62]
0x00555108: d9442428                 fld dword ptr [esp + 0x28]
0x0055510c: c1e008                   shl eax, 8
0x0055510f: 0bc2                     or eax, edx
0x00555111: 894118                   mov dword ptr [ecx + 0x18], eax
0x00555114: 83c124                   add ecx, 0x24
0x00555117: d8450c                   fadd dword ptr [ebp + 0xc]
0x0055511a: 83c306                   add ebx, 6
0x0055511d: d807                     fadd dword ptr [edi]
0x0055511f: d95c2428                 fstp dword ptr [esp + 0x28]
0x00555123: d944242c                 fld dword ptr [esp + 0x2c]
0x00555127: d8450c                   fadd dword ptr [ebp + 0xc]
0x0055512a: d807                     fadd dword ptr [edi]
0x0055512c: d95c242c                 fstp dword ptr [esp + 0x2c]
0x00555130: 8b542458                 mov edx, dword ptr [esp + 0x58]
0x00555134: 8b7c243c                 mov edi, dword ptr [esp + 0x3c]
0x00555138: 33c0                     xor eax, eax
0x0055513a: 8a0417                   mov al, byte ptr [edi + edx]
0x0055513d: 8bbc24cc000000           mov edi, dword ptr [esp + 0xcc]
0x00555144: 23c7                     and eax, edi
0x00555146: 8b7c243c                 mov edi, dword ptr [esp + 0x3c]
0x0055514a: 33d2                     xor edx, edx
0x0055514c: 8a17                     mov dl, byte ptr [edi]
0x0055514e: c1e008                   shl eax, 8
0x00555151: 0bc2                     or eax, edx
0x00555153: 0f8f09f9ffff             jg 0x554a62
0x00555159: 85db                     test ebx, ebx
0x0055515b: 7446                     je 0x5551a3
0x0055515d: 8b84248c000000           mov eax, dword ptr [esp + 0x8c]
0x00555164: 8b35042a9100             mov esi, dword ptr [0x912a04]
0x0055516a: 6a05                     push 5
0x0055516c: 50                       push eax
0x0055516d: 2bce                     sub ecx, esi
0x0055516f: b8398ee338               mov eax, 0x38e38e39
0x00555174: f7e9                     imul ecx
0x00555176: c1fa03                   sar edx, 3
0x00555179: 8bca                     mov ecx, edx
0x0055517b: c1e91f                   shr ecx, 0x1f
0x0055517e: 03d1                     add edx, ecx
0x00555180: 52                       push edx
0x00555181: 56                       push esi
0x00555182: e8e97ef7ff               call 0x4cd070
0x00555187: 83c410                   add esp, 0x10
0x0055518a: 85c0                     test eax, eax
0x0055518c: 7415                     je 0x5551a3
0x0055518e: 53                       push ebx
0x0055518f: 6800279100               push 0x912700
0x00555194: 6a03                     push 3
0x00555196: e8d57ff7ff               call 0x4cd170
0x0055519b: 83c40c                   add esp, 0xc
0x0055519e: e89d7ff7ff               call 0x4cd140
0x005551a3: d9442428                 fld dword ptr [esp + 0x28]
0x005551a7: d88c24e4000000           fmul dword ptr [esp + 0xe4]
0x005551ae: 8b8424e8000000           mov eax, dword ptr [esp + 0xe8]
0x005551b5: d800                     fadd dword ptr [eax]
0x005551b7: d918                     fstp dword ptr [eax]
0x005551b9: e8b2dbffff               call 0x552d70
0x005551be: 5f                       pop edi
0x005551bf: 5e                       pop esi
0x005551c0: 8bc5                     mov eax, ebp
0x005551c2: 5d                       pop ebp
0x005551c3: 5b                       pop ebx
0x005551c4: 81c4c8000000             add esp, 0xc8
0x005551ca: c3                       ret
0x005551cb: 90                       nop
0x005551cc: 90                       nop
0x005551cd: 90                       nop
0x005551ce: 90                       nop
0x005551cf: 90                       nop
0x005551d0: a1082a9100               mov eax, dword ptr [0x912a08]
0x005551d5: 8b0df83f7d00             mov ecx, dword ptr [0x7d3ff8]
0x005551db: 56                       push esi
0x005551dc: 57                       push edi
0x005551dd: 6899010300               push 0x30199
0x005551e2: 50                       push eax
0x005551e3: ff9118010000             call dword ptr [ecx + 0x118]
0x005551e9: 8bf0                     mov esi, eax
0x005551eb: 33ff                     xor edi, edi
0x005551ed: 83c408                   add esp, 8
0x005551f0: 3bf7                     cmp esi, edi
0x005551f2: 7505                     jne 0x5551f9
0x005551f4: 5f                       pop edi
0x005551f5: 33c0                     xor eax, eax
0x005551f7: 5e                       pop esi
0x005551f8: c3                       ret
0x005551f9: 33d2                     xor edx, edx
0x005551fb: c70601000000             mov dword ptr [esi], 1
0x00555201: c746040000803f           mov dword ptr [esi + 4], 0x3f800000
0x00555208: 897e08                   mov dword ptr [esi + 8], edi
0x0055520b: 897e0c                   mov dword ptr [esi + 0xc], edi
0x0055520e: 897e10                   mov dword ptr [esi + 0x10], edi
0x00555211: 895614                   mov dword ptr [esi + 0x14], edx
0x00555214: 6890010300               push 0x30190
0x00555219: 895618                   mov dword ptr [esi + 0x18], edx
0x0055521c: 6a20                     push 0x20
0x0055521e: 89561c                   mov dword ptr [esi + 0x1c], edx
0x00555221: 895620                   mov dword ptr [esi + 0x20], edx
0x00555224: 89be28010000             mov dword ptr [esi + 0x128], edi
0x0055522a: 89be30010000             mov dword ptr [esi + 0x130], edi
0x00555230: 89be24010000             mov dword ptr [esi + 0x124], edi
0x00555236: 89be2c010000             mov dword ptr [esi + 0x12c], edi
0x0055523c: e88ff80600               call 0x5c4ad0
0x00555241: 898634010000             mov dword ptr [esi + 0x134], eax
0x00555247: 89be3c010000             mov dword ptr [esi + 0x13c], edi
0x0055524d: 89be40010000             mov dword ptr [esi + 0x140], edi
0x00555253: 8d7e24                   lea edi, [esi + 0x24]
0x00555256: b940000000               mov ecx, 0x40
0x0055525b: 83c8ff                   or eax, 0xffffffff
0x0055525e: 83c408                   add esp, 8
0x00555261: c7863801000040495500     mov dword ptr [esi + 0x138], 0x554940
0x0055526b: f3ab                     rep stosd dword ptr es:[edi], eax
0x0055526d: 8bc6                     mov eax, esi
0x0055526f: 5f                       pop edi
0x00555270: 5e                       pop esi
0x00555271: c3                       ret
0x00555272: 90                       nop
0x00555273: 90                       nop
0x00555274: 90                       nop
0x00555275: 90                       nop
0x00555276: 90                       nop
0x00555277: 90                       nop
0x00555278: 90                       nop
0x00555279: 90                       nop
0x0055527a: 90                       nop
0x0055527b: 90                       nop
0x0055527c: 90                       nop
0x0055527d: 90                       nop
0x0055527e: 90                       nop
0x0055527f: 90                       nop
0x00555280: 53                       push ebx
0x00555281: 56                       push esi
0x00555282: 8b35182a9100             mov esi, dword ptr [0x912a18]
0x00555288: 33db                     xor ebx, ebx
0x0055528a: 3bf3                     cmp esi, ebx
0x0055528c: 7439                     je 0x5552c7
0x0055528e: 57                       push edi
0x0055528f: 8b06                     mov eax, dword ptr [esi]
0x00555291: 8b0df83f7d00             mov ecx, dword ptr [0x7d3ff8]
0x00555297: 50                       push eax
0x00555298: ff910c010000             call dword ptr [ecx + 0x10c]
