0x00473ee0: 83ec3c                   sub esp, 0x3c
0x00473ee3: 53                       push ebx
0x00473ee4: 55                       push ebp
0x00473ee5: 56                       push esi
0x00473ee6: 57                       push edi
0x00473ee7: c644241800               mov byte ptr [esp + 0x18], 0
0x00473eec: c644241900               mov byte ptr [esp + 0x19], 0
0x00473ef1: c644241a00               mov byte ptr [esp + 0x1a], 0
0x00473ef6: c644241b00               mov byte ptr [esp + 0x1b], 0
0x00473efb: e8b079fbff               call 0x42b8b0
0x00473f00: e8bb79fbff               call 0x42b8c0
0x00473f05: e8a679fbff               call 0x42b8b0
0x00473f0a: e8b179fbff               call 0x42b8c0
0x00473f0f: d9442458                 fld dword ptr [esp + 0x58]
0x00473f13: 8b742418                 mov esi, dword ptr [esp + 0x18]
0x00473f17: d825d0c95c00             fsub dword ptr [0x5cc9d0]
0x00473f1d: 56                       push esi
0x00473f1e: c644241600               mov byte ptr [esp + 0x16], 0
0x00473f23: c644241500               mov byte ptr [esp + 0x15], 0
0x00473f28: d95c2428                 fstp dword ptr [esp + 0x28]
0x00473f2c: 8b4c2428                 mov ecx, dword ptr [esp + 0x28]
0x00473f30: c644241400               mov byte ptr [esp + 0x14], 0
0x00473f35: c6442417ff               mov byte ptr [esp + 0x17], 0xff
0x00473f3a: 8b7c2414                 mov edi, dword ptr [esp + 0x14]
0x00473f3e: 57                       push edi
0x00473f3f: c744243800008042         mov dword ptr [esp + 0x38], 0x42800000
0x00473f47: 8b5c2438                 mov ebx, dword ptr [esp + 0x38]
0x00473f4b: 53                       push ebx
0x00473f4c: c744243800000043         mov dword ptr [esp + 0x38], 0x43000000
0x00473f54: 8b6c2438                 mov ebp, dword ptr [esp + 0x38]
0x00473f58: c744243400000000         mov dword ptr [esp + 0x34], 0
0x00473f60: 8b442434                 mov eax, dword ptr [esp + 0x34]
0x00473f64: 55                       push ebp
0x00473f65: 50                       push eax
0x00473f66: 51                       push ecx
0x00473f67: e844f4ffff               call 0x4733b0
0x00473f6c: 8b44243c                 mov eax, dword ptr [esp + 0x3c]
0x00473f70: 56                       push esi
0x00473f71: 57                       push edi
0x00473f72: 53                       push ebx
0x00473f73: c744244c0000d043         mov dword ptr [esp + 0x4c], 0x43d00000
0x00473f7b: 8b54244c                 mov edx, dword ptr [esp + 0x4c]
0x00473f7f: 55                       push ebp
0x00473f80: 52                       push edx
0x00473f81: 50                       push eax
0x00473f82: e829f4ffff               call 0x4733b0
0x00473f87: a1c8ec8600               mov eax, dword ptr [0x86ecc8]
0x00473f8c: 83c430                   add esp, 0x30
0x00473f8f: 3dff000000               cmp eax, 0xff
0x00473f94: 7e0a                     jle 0x473fa0
0x00473f96: b8ff000000               mov eax, 0xff
0x00473f9b: a3c8ec8600               mov dword ptr [0x86ecc8], eax
0x00473fa0: 8b0dccec8600             mov ecx, dword ptr [0x86eccc]
0x00473fa6: ba02000000               mov edx, 2
0x00473fab: eb03                     jmp 0x473fb0
0x00473fad: 8d4900                   lea ecx, [ecx]
0x00473fb0: 3bc1                     cmp eax, ecx
0x00473fb2: 7e03                     jle 0x473fb7
0x00473fb4: 41                       inc ecx
0x00473fb5: 3bc1                     cmp eax, ecx
0x00473fb7: 7d01                     jge 0x473fba
0x00473fb9: 49                       dec ecx
0x00473fba: 4a                       dec edx
0x00473fbb: 75f3                     jne 0x473fb0
0x00473fbd: bf5a000000               mov edi, 0x5a
0x00473fc2: 890dccec8600             mov dword ptr [0x86eccc], ecx
0x00473fc8: c744243089880842         mov dword ptr [esp + 0x30], 0x42088889
0x00473fd0: c6442412ff               mov byte ptr [esp + 0x12], 0xff
0x00473fd5: c6442411ff               mov byte ptr [esp + 0x11], 0xff
0x00473fda: c6442410ff               mov byte ptr [esp + 0x10], 0xff
0x00473fdf: c744241c00000000         mov dword ptr [esp + 0x1c], 0
0x00473fe7: 897c2414                 mov dword ptr [esp + 0x14], edi
0x00473feb: eb04                     jmp 0x473ff1
0x00473fed: 8b742418                 mov esi, dword ptr [esp + 0x18]
0x00473ff1: db442414                 fild dword ptr [esp + 0x14]
0x00473ff5: 51                       push ecx
0x00473ff6: d9c0                     fld st(0)
0x00473ff8: d8c9                     fmul st(1)
0x00473ffa: d82dd0ea5c00             fsubr dword ptr [0x5cead0]
0x00474000: d91c24                   fstp dword ptr [esp]
0x00474003: ddd8                     fstp st(0)
0x00474005: e826fb0400               call 0x4c3b30
0x0047400a: 8b0dccec8600             mov ecx, dword ptr [0x86eccc]
0x00474010: 8d0c49                   lea ecx, [ecx + ecx*2]
0x00474013: c1e105                   shl ecx, 5
0x00474016: b881808080               mov eax, 0x80808081
0x0047401b: f7e9                     imul ecx
0x0047401d: 03d1                     add edx, ecx
0x0047401f: c1fa07                   sar edx, 7
0x00474022: d86c245c                 fsubr dword ptr [esp + 0x5c]
0x00474026: 8bca                     mov ecx, edx
0x00474028: c1e91f                   shr ecx, 0x1f
0x0047402b: d825d0c95c00             fsub dword ptr [0x5cc9d0]
0x00474031: 8bda                     mov ebx, edx
0x00474033: 8b542434                 mov edx, dword ptr [esp + 0x34]
0x00474037: 56                       push esi
0x00474038: d95c242c                 fstp dword ptr [esp + 0x2c]
0x0047403c: 03d9                     add ebx, ecx
0x0047403e: db442424                 fild dword ptr [esp + 0x24]
0x00474042: 8b4c242c                 mov ecx, dword ptr [esp + 0x2c]
0x00474046: 885c241b                 mov byte ptr [esp + 0x1b], bl
0x0047404a: 8b742418                 mov esi, dword ptr [esp + 0x18]
0x0047404e: d80dccea5c00             fmul dword ptr [0x5ceacc]
0x00474054: 56                       push esi
0x00474055: 52                       push edx
0x00474056: c744243c00000043         mov dword ptr [esp + 0x3c], 0x43000000
0x0047405e: 8b44243c                 mov eax, dword ptr [esp + 0x3c]
0x00474062: d95c2438                 fstp dword ptr [esp + 0x38]
0x00474066: 8b6c2438                 mov ebp, dword ptr [esp + 0x38]
0x0047406a: 50                       push eax
0x0047406b: 55                       push ebp
0x0047406c: 51                       push ecx
0x0047406d: e8aef1ffff               call 0x473220
0x00474072: 0fb6d3                   movzx edx, bl
0x00474075: 8954243c                 mov dword ptr [esp + 0x3c], edx
0x00474079: 89742430                 mov dword ptr [esp + 0x30], esi
0x0047407d: db44243c                 fild dword ptr [esp + 0x3c]
0x00474081: d80d74d05c00             fmul dword ptr [0x5cd074]
0x00474087: e8bceb0200               call 0x4a2c48
0x0047408c: 8b54244c                 mov edx, dword ptr [esp + 0x4c]
0x00474090: 56                       push esi
0x00474091: 88442437                 mov byte ptr [esp + 0x37], al
0x00474095: 8b4c2434                 mov ecx, dword ptr [esp + 0x34]
0x00474099: 8b442444                 mov eax, dword ptr [esp + 0x44]
0x0047409d: 51                       push ecx
0x0047409e: 52                       push edx
0x0047409f: 50                       push eax
0x004740a0: c744245000000000         mov dword ptr [esp + 0x50], 0
0x004740a8: 8b4c2450                 mov ecx, dword ptr [esp + 0x50]
0x004740ac: 55                       push ebp
0x004740ad: 51                       push ecx
0x004740ae: 89442460                 mov dword ptr [esp + 0x60], eax
0x004740b2: e869f1ffff               call 0x473220
0x004740b7: 8b542450                 mov edx, dword ptr [esp + 0x50]
0x004740bb: 83ef0c                   sub edi, 0xc
0x004740be: 83c20c                   add edx, 0xc
0x004740c1: 83c434                   add esp, 0x34
0x004740c4: 83ffa6                   cmp edi, -0x5a
0x004740c7: 8954241c                 mov dword ptr [esp + 0x1c], edx
0x004740cb: 897c2414                 mov dword ptr [esp + 0x14], edi
0x004740cf: 0f8f18ffffff             jg 0x473fed
0x004740d5: b080                     mov al, 0x80
0x004740d7: 88442412                 mov byte ptr [esp + 0x12], al
0x004740db: 88442411                 mov byte ptr [esp + 0x11], al
0x004740df: 88442410                 mov byte ptr [esp + 0x10], al
0x004740e3: 33c0                     xor eax, eax
0x004740e5: c6442413ff               mov byte ptr [esp + 0x13], 0xff
0x004740ea: 8944241c                 mov dword ptr [esp + 0x1c], eax
0x004740ee: eb04                     jmp 0x4740f4
0x004740f0: 8b44241c                 mov eax, dword ptr [esp + 0x1c]
0x004740f4: 33ff                     xor edi, edi
0x004740f6: 8bef                     mov ebp, edi
0x004740f8: 83e501                   and ebp, 1
0x004740fb: 8bdd                     mov ebx, ebp
0x004740fd: 8bd7                     mov edx, edi
0x004740ff: 6bdb15                   imul ebx, ebx, 0x15
0x00474102: 33f6                     xor esi, esi
0x00474104: 6bd215                   imul edx, edx, 0x15
0x00474107: 89542458                 mov dword ptr [esp + 0x58], edx
0x0047410b: db442458                 fild dword ptr [esp + 0x58]
0x0047410f: d95c2420                 fstp dword ptr [esp + 0x20]
0x00474113: 85c0                     test eax, eax
0x00474115: 8d4cb62d                 lea ecx, [esi + esi*4 + 0x2d]
0x00474119: 8d14cb                   lea edx, [ebx + ecx*8]
0x0047411c: 89542458                 mov dword ptr [esp + 0x58], edx
0x00474120: db442458                 fild dword ptr [esp + 0x58]
0x00474124: d95c2424                 fstp dword ptr [esp + 0x24]
0x00474128: d9442420                 fld dword ptr [esp + 0x20]
0x0047412c: 7406                     je 0x474134
0x0047412e: d805c8ea5c00             fadd dword ptr [0x5ceac8]
0x00474134: d9442424                 fld dword ptr [esp + 0x24]
0x00474138: 8d0c37                   lea ecx, [edi + esi]
0x0047413b: d805e0e95c00             fadd dword ptr [0x5ce9e0]
0x00474141: 894c2414                 mov dword ptr [esp + 0x14], ecx
0x00474145: 8d543701                 lea edx, [edi + esi + 1]
0x00474149: 8d4c3702                 lea ecx, [edi + esi + 2]
0x0047414d: d9c0                     fld st(0)
0x0047414f: d9c2                     fld st(2)
0x00474151: d805e0e95c00             fadd dword ptr [0x5ce9e0]
0x00474157: d9542430                 fst dword ptr [esp + 0x30]
0x0047415b: d9ca                     fxch st(2)
0x0047415d: d95c2434                 fstp dword ptr [esp + 0x34]
0x00474161: d9c9                     fxch st(1)
0x00474163: d95c2438                 fstp dword ptr [esp + 0x38]
0x00474167: d90510107f00             fld dword ptr [0x7f1010]
0x0047416d: dcc0                     fadd st(0), st(0)
0x0047416f: d95c2458                 fstp dword ptr [esp + 0x58]
0x00474173: db442414                 fild dword ptr [esp + 0x14]
0x00474177: 89542414                 mov dword ptr [esp + 0x14], edx
0x0047417b: 8d147e                   lea edx, [esi + edi*2]
0x0047417e: d8442458                 fadd dword ptr [esp + 0x58]
0x00474182: d9fe                     fsin 
0x00474184: dc0df8e15c00             fmul qword ptr [0x5ce1f8]
0x0047418a: d8442424                 fadd dword ptr [esp + 0x24]
0x0047418e: d95c243c                 fstp dword ptr [esp + 0x3c]
0x00474192: db442414                 fild dword ptr [esp + 0x14]
0x00474196: d8442458                 fadd dword ptr [esp + 0x58]
0x0047419a: d9fe                     fsin 
0x0047419c: dc0df8e15c00             fmul qword ptr [0x5ce1f8]
0x004741a2: d9542414                 fst dword ptr [esp + 0x14]
0x004741a6: d8c1                     fadd st(1)
0x004741a8: d95c2444                 fstp dword ptr [esp + 0x44]
0x004741ac: ddd8                     fstp st(0)
0x004741ae: d9442414                 fld dword ptr [esp + 0x14]
0x004741b2: 894c2414                 mov dword ptr [esp + 0x14], ecx
0x004741b6: d8442424                 fadd dword ptr [esp + 0x24]
0x004741ba: 8d4c7e01                 lea ecx, [esi + edi*2 + 1]
0x004741be: d95c242c                 fstp dword ptr [esp + 0x2c]
0x004741c2: db442414                 fild dword ptr [esp + 0x14]
0x004741c6: 89542414                 mov dword ptr [esp + 0x14], edx
0x004741ca: 8d547e02                 lea edx, [esi + edi*2 + 2]
0x004741ce: d8442458                 fadd dword ptr [esp + 0x58]
0x004741d2: d9fe                     fsin 
0x004741d4: dc0df8e15c00             fmul qword ptr [0x5ce1f8]
0x004741da: d8442434                 fadd dword ptr [esp + 0x34]
0x004741de: d95c2434                 fstp dword ptr [esp + 0x34]
0x004741e2: db442414                 fild dword ptr [esp + 0x14]
0x004741e6: 894c2414                 mov dword ptr [esp + 0x14], ecx
0x004741ea: 8d4c7e03                 lea ecx, [esi + edi*2 + 3]
0x004741ee: d8442458                 fadd dword ptr [esp + 0x58]
0x004741f2: d9fe                     fsin 
0x004741f4: dcc0                     fadd st(0), st(0)
0x004741f6: d8c1                     fadd st(1)
0x004741f8: d95c2440                 fstp dword ptr [esp + 0x40]
0x004741fc: db442414                 fild dword ptr [esp + 0x14]
0x00474200: 89542414                 mov dword ptr [esp + 0x14], edx
0x00474204: d8442458                 fadd dword ptr [esp + 0x58]
0x00474208: d9fe                     fsin 
0x0047420a: dcc0                     fadd st(0), st(0)
0x0047420c: d8c1                     fadd st(1)
0x0047420e: d95c2448                 fstp dword ptr [esp + 0x48]
0x00474212: ddd8                     fstp st(0)
0x00474214: db442414                 fild dword ptr [esp + 0x14]
0x00474218: 894c2414                 mov dword ptr [esp + 0x14], ecx
0x0047421c: d8442458                 fadd dword ptr [esp + 0x58]
0x00474220: d9fe                     fsin 
0x00474222: dcc0                     fadd st(0), st(0)
0x00474224: d8442430                 fadd dword ptr [esp + 0x30]
0x00474228: d95c2430                 fstp dword ptr [esp + 0x30]
0x0047422c: db442414                 fild dword ptr [esp + 0x14]
0x00474230: d8442458                 fadd dword ptr [esp + 0x58]
0x00474234: d9fe                     fsin 
0x00474236: dcc0                     fadd st(0), st(0)
0x00474238: d8442438                 fadd dword ptr [esp + 0x38]
0x0047423c: d95c2438                 fstp dword ptr [esp + 0x38]
0x00474240: 85c0                     test eax, eax
0x00474242: 7517                     jne 0x47425b
0x00474244: 83ff02                   cmp edi, 2
0x00474247: 7526                     jne 0x47426f
0x00474249: c744243800008042         mov dword ptr [esp + 0x38], 0x42800000
0x00474251: c744243000008042         mov dword ptr [esp + 0x30], 0x42800000
0x00474259: eb14                     jmp 0x47426f
0x0047425b: 85ff                     test edi, edi
0x0047425d: 7510                     jne 0x47426f
0x0047425f: c74424480000d043         mov dword ptr [esp + 0x48], 0x43d00000
0x00474267: c74424400000d043         mov dword ptr [esp + 0x40], 0x43d00000
0x0047426f: 85f6                     test esi, esi
0x00474271: 8b542410                 mov edx, dword ptr [esp + 0x10]
0x00474275: 89542414                 mov dword ptr [esp + 0x14], edx
0x00474279: 750c                     jne 0x474287
0x0047427b: 85ed                     test ebp, ebp
0x0047427d: 7508                     jne 0x474287
0x0047427f: 8b442418                 mov eax, dword ptr [esp + 0x18]
0x00474283: 89442414                 mov dword ptr [esp + 0x14], eax
0x00474287: e82476fbff               call 0x42b8b0
0x0047428c: 0fbfc8                   movsx ecx, ax
0x0047428f: 894c2458                 mov dword ptr [esp + 0x58], ecx
0x00474293: db442458                 fild dword ptr [esp + 0x58]
0x00474297: d84c243c                 fmul dword ptr [esp + 0x3c]
0x0047429b: d80da8d55c00             fmul dword ptr [0x5cd5a8]
0x004742a1: d95c243c                 fstp dword ptr [esp + 0x3c]
0x004742a5: e81676fbff               call 0x42b8c0
0x004742aa: 0fbfd0                   movsx edx, ax
0x004742ad: 89542458                 mov dword ptr [esp + 0x58], edx
0x004742b1: db442458                 fild dword ptr [esp + 0x58]
0x004742b5: d84c2440                 fmul dword ptr [esp + 0x40]
0x004742b9: d80d60c55c00             fmul dword ptr [0x5cc560]
0x004742bf: d95c2440                 fstp dword ptr [esp + 0x40]
0x004742c3: e8e875fbff               call 0x42b8b0
0x004742c8: 0fbfc0                   movsx eax, ax
0x004742cb: 89442458                 mov dword ptr [esp + 0x58], eax
0x004742cf: db442458                 fild dword ptr [esp + 0x58]
0x004742d3: d84c2444                 fmul dword ptr [esp + 0x44]
0x004742d7: d80da8d55c00             fmul dword ptr [0x5cd5a8]
0x004742dd: d95c2444                 fstp dword ptr [esp + 0x44]
0x004742e1: e8da75fbff               call 0x42b8c0
0x004742e6: 0fbfc8                   movsx ecx, ax
0x004742e9: 894c2458                 mov dword ptr [esp + 0x58], ecx
0x004742ed: db442458                 fild dword ptr [esp + 0x58]
0x004742f1: d84c2448                 fmul dword ptr [esp + 0x48]
0x004742f5: d80d60c55c00             fmul dword ptr [0x5cc560]
0x004742fb: d95c2448                 fstp dword ptr [esp + 0x48]
0x004742ff: e8ac75fbff               call 0x42b8b0
0x00474304: 0fbfd0                   movsx edx, ax
0x00474307: 89542458                 mov dword ptr [esp + 0x58], edx
0x0047430b: db442458                 fild dword ptr [esp + 0x58]
0x0047430f: d84c242c                 fmul dword ptr [esp + 0x2c]
0x00474313: d80da8d55c00             fmul dword ptr [0x5cd5a8]
0x00474319: d95c242c                 fstp dword ptr [esp + 0x2c]
0x0047431d: e89e75fbff               call 0x42b8c0
0x00474322: 0fbfc0                   movsx eax, ax
0x00474325: 89442458                 mov dword ptr [esp + 0x58], eax
0x00474329: db442458                 fild dword ptr [esp + 0x58]
0x0047432d: d84c2430                 fmul dword ptr [esp + 0x30]
0x00474331: d80d60c55c00             fmul dword ptr [0x5cc560]
0x00474337: d95c2430                 fstp dword ptr [esp + 0x30]
0x0047433b: e87075fbff               call 0x42b8b0
0x00474340: 0fbfc8                   movsx ecx, ax
0x00474343: 894c2458                 mov dword ptr [esp + 0x58], ecx
0x00474347: db442458                 fild dword ptr [esp + 0x58]
0x0047434b: d84c2434                 fmul dword ptr [esp + 0x34]
0x0047434f: d80da8d55c00             fmul dword ptr [0x5cd5a8]
0x00474355: d95c2434                 fstp dword ptr [esp + 0x34]
0x00474359: e86275fbff               call 0x42b8c0
0x0047435e: 8b4c2440                 mov ecx, dword ptr [esp + 0x40]
0x00474362: 0fbfd0                   movsx edx, ax
0x00474365: 8b44243c                 mov eax, dword ptr [esp + 0x3c]
0x00474369: a3208a8900               mov dword ptr [0x898a20], eax
0x0047436e: 8b442448                 mov eax, dword ptr [esp + 0x48]
0x00474372: a3408a8900               mov dword ptr [0x898a40], eax
0x00474377: 8b442434                 mov eax, dword ptr [esp + 0x34]
0x0047437b: a3748a8900               mov dword ptr [0x898a74], eax
0x00474380: 0fb6442417               movzx eax, byte ptr [esp + 0x17]
0x00474385: 89542458                 mov dword ptr [esp + 0x58], edx
0x00474389: 8b542444                 mov edx, dword ptr [esp + 0x44]
0x0047438d: db442458                 fild dword ptr [esp + 0x58]
0x00474391: 890d248a8900             mov dword ptr [0x898a24], ecx
0x00474397: 8b4c242c                 mov ecx, dword ptr [esp + 0x2c]
0x0047439b: 890d588a8900             mov dword ptr [0x898a58], ecx
0x004743a1: 0fb64c2414               movzx ecx, byte ptr [esp + 0x14]
0x004743a6: d84c2438                 fmul dword ptr [esp + 0x38]
0x004743aa: d80d60c55c00             fmul dword ptr [0x5cc560]
0x004743b0: c1e008                   shl eax, 8
0x004743b3: 89153c8a8900             mov dword ptr [0x898a3c], edx
0x004743b9: 8b542430                 mov edx, dword ptr [esp + 0x30]
0x004743bd: 0bc1                     or eax, ecx
0x004743bf: d91d788a8900             fstp dword ptr [0x898a78]
0x004743c5: 0fb64c2416               movzx ecx, byte ptr [esp + 0x16]
0x004743ca: 89155c8a8900             mov dword ptr [0x898a5c], edx
0x004743d0: 0fb6542415               movzx edx, byte ptr [esp + 0x15]
0x004743d5: c1e008                   shl eax, 8
0x004743d8: 0bc2                     or eax, edx
0x004743da: c1e008                   shl eax, 8
0x004743dd: 0bc1                     or eax, ecx
0x004743df: a3308a8900               mov dword ptr [0x898a30], eax
0x004743e4: b9808080ff               mov ecx, 0xff808080
0x004743e9: 890d4c8a8900             mov dword ptr [0x898a4c], ecx
0x004743ef: a3688a8900               mov dword ptr [0x898a68], eax
0x004743f4: 890d848a8900             mov dword ptr [0x898a84], ecx
0x004743fa: 8b0df83f7d00             mov ecx, dword ptr [0x7d3ff8]
0x00474400: b82c8a8900               mov eax, 0x898a2c
0x00474405: eb09                     jmp 0x474410
0x00474407: 8da42400000000           lea esp, [esp]
0x0047440e: 8bff                     mov edi, edi
0x00474410: 8b5118                   mov edx, dword ptr [ecx + 0x18]
0x00474413: 8950fc                   mov dword ptr [eax - 4], edx
0x00474416: c7000000803f             mov dword ptr [eax], 0x3f800000
0x0047441c: 83c01c                   add eax, 0x1c
0x0047441f: 3d9c8a8900               cmp eax, 0x898a9c
0x00474424: 7cea                     jl 0x474410
0x00474426: 6a00                     push 0
0x00474428: 6a01                     push 1
0x0047442a: ff5120                   call dword ptr [ecx + 0x20]
0x0047442d: a1f83f7d00               mov eax, dword ptr [0x7d3ff8]
0x00474432: 6a01                     push 1
0x00474434: 6a0c                     push 0xc
0x00474436: ff5020                   call dword ptr [eax + 0x20]
0x00474439: 8b0df83f7d00             mov ecx, dword ptr [0x7d3ff8]
0x0047443f: 6a05                     push 5
0x00474441: 6a0a                     push 0xa
0x00474443: ff5120                   call dword ptr [ecx + 0x20]
0x00474446: 8b15f83f7d00             mov edx, dword ptr [0x7d3ff8]
0x0047444c: 6a06                     push 6
0x0047444e: 6a0b                     push 0xb
0x00474450: ff5220                   call dword ptr [edx + 0x20]
0x00474453: a1f83f7d00               mov eax, dword ptr [0x7d3ff8]
0x00474458: 6a04                     push 4
0x0047445a: 68208a8900               push 0x898a20
0x0047445f: 6a04                     push 4
0x00474461: ff5030                   call dword ptr [eax + 0x30]
0x00474464: 8b442448                 mov eax, dword ptr [esp + 0x48]
0x00474468: 83c42c                   add esp, 0x2c
0x0047446b: 46                       inc esi
0x0047446c: 83fe07                   cmp esi, 7
0x0047446f: 0f8c9efcffff             jl 0x474113
0x00474475: 47                       inc edi
0x00474476: 83ff03                   cmp edi, 3
0x00474479: 0f8c77fcffff             jl 0x4740f6
0x0047447f: 40                       inc eax
0x00474480: 83f802                   cmp eax, 2
0x00474483: 8944241c                 mov dword ptr [esp + 0x1c], eax
0x00474487: 0f8c63fcffff             jl 0x4740f0
0x0047448d: 5f                       pop edi
0x0047448e: 5e                       pop esi
0x0047448f: 5d                       pop ebp
0x00474490: 5b                       pop ebx
0x00474491: 83c43c                   add esp, 0x3c
0x00474494: c3                       ret 
0x00474495: 90                       nop 
0x00474496: 90                       nop 
0x00474497: 90                       nop 
0x00474498: 90                       nop 
0x00474499: 90                       nop 
0x0047449a: 90                       nop 
0x0047449b: 90                       nop 
0x0047449c: 90                       nop 
0x0047449d: 90                       nop 
0x0047449e: 90                       nop 
0x0047449f: 90                       nop 
0x004744a0: 83ec44                   sub esp, 0x44
0x004744a3: 53                       push ebx
0x004744a4: b080                     mov al, 0x80
0x004744a6: 55                       push ebp
0x004744a7: 88442412                 mov byte ptr [esp + 0x12], al
0x004744ab: 88442411                 mov byte ptr [esp + 0x11], al
0x004744af: 88442410                 mov byte ptr [esp + 0x10], al
0x004744b3: 56                       push esi
0x004744b4: 33c0                     xor eax, eax
0x004744b6: 57                       push edi
0x004744b7: c644241c00               mov byte ptr [esp + 0x1c], 0
0x004744bc: c644241d00               mov byte ptr [esp + 0x1d], 0
0x004744c1: c644241e00               mov byte ptr [esp + 0x1e], 0
0x004744c6: c644241f00               mov byte ptr [esp + 0x1f], 0
0x004744cb: c644241bff               mov byte ptr [esp + 0x1b], 0xff
0x004744d0: 89442414                 mov dword ptr [esp + 0x14], eax
0x004744d4: eb0a                     jmp 0x4744e0
0x004744d6: 8b442414                 mov eax, dword ptr [esp + 0x14]
0x004744da: 8d9b00000000             lea ebx, [ebx]
0x004744e0: 33ff                     xor edi, edi
0x004744e2: 8bcf                     mov ecx, edi
0x004744e4: 83e101                   and ecx, 1
0x004744e7: 8be9                     mov ebp, ecx
0x004744e9: 8bd7                     mov edx, edi
0x004744eb: 6bed15                   imul ebp, ebp, 0x15
0x004744ee: 33f6                     xor esi, esi
0x004744f0: 6bd215                   imul edx, edx, 0x15
0x004744f3: 89542420                 mov dword ptr [esp + 0x20], edx
0x004744f7: 894c2424                 mov dword ptr [esp + 0x24], ecx
0x004744fb: db442420                 fild dword ptr [esp + 0x20]
0x004744ff: d95c2428                 fstp dword ptr [esp + 0x28]
0x00474503: eb0b                     jmp 0x474510
0x00474505: 8b4c2424                 mov ecx, dword ptr [esp + 0x24]
0x00474509: 8da42400000000           lea esp, [esp]
0x00474510: 85c0                     test eax, eax
0x00474512: 8d54b62d                 lea edx, [esi + esi*4 + 0x2d]
0x00474516: 8d54d500                 lea edx, [ebp + edx*8]
0x0047451a: 89542420                 mov dword ptr [esp + 0x20], edx
0x0047451e: db442420                 fild dword ptr [esp + 0x20]
0x00474522: d95c244c                 fstp dword ptr [esp + 0x4c]
0x00474526: d9442428                 fld dword ptr [esp + 0x28]
0x0047452a: 7406                     je 0x474532
0x0047452c: d805c8ea5c00             fadd dword ptr [0x5ceac8]
0x00474532: d944244c                 fld dword ptr [esp + 0x4c]
0x00474536: 8d143e                   lea edx, [esi + edi]
0x00474539: d805e0e95c00             fadd dword ptr [0x5ce9e0]
0x0047453f: 89542420                 mov dword ptr [esp + 0x20], edx
0x00474543: 8d543701                 lea edx, [edi + esi + 1]
0x00474547: d9c0                     fld st(0)
0x00474549: d9c2                     fld st(2)
0x0047454b: d805e0e95c00             fadd dword ptr [0x5ce9e0]
0x00474551: d9542430                 fst dword ptr [esp + 0x30]
0x00474555: d9ca                     fxch st(2)
0x00474557: d95c2434                 fstp dword ptr [esp + 0x34]
0x0047455b: d9c9                     fxch st(1)
0x0047455d: d95c2438                 fstp dword ptr [esp + 0x38]
0x00474561: d90510107f00             fld dword ptr [0x7f1010]
0x00474567: dcc0                     fadd st(0), st(0)
0x00474569: d95c2410                 fstp dword ptr [esp + 0x10]
0x0047456d: db442420                 fild dword ptr [esp + 0x20]
0x00474571: 89542420                 mov dword ptr [esp + 0x20], edx
0x00474575: 8d543702                 lea edx, [edi + esi + 2]
0x00474579: d8442410                 fadd dword ptr [esp + 0x10]
0x0047457d: d9fe                     fsin 
0x0047457f: dc0df8e15c00             fmul qword ptr [0x5ce1f8]
0x00474585: d844244c                 fadd dword ptr [esp + 0x4c]
0x00474589: d95c243c                 fstp dword ptr [esp + 0x3c]
0x0047458d: db442420                 fild dword ptr [esp + 0x20]
0x00474591: d8442410                 fadd dword ptr [esp + 0x10]
0x00474595: d9fe                     fsin 
0x00474597: dc0df8e15c00             fmul qword ptr [0x5ce1f8]
0x0047459d: d9542420                 fst dword ptr [esp + 0x20]
0x004745a1: d8c1                     fadd st(1)
0x004745a3: d95c2444                 fstp dword ptr [esp + 0x44]
0x004745a7: ddd8                     fstp st(0)
0x004745a9: d9442420                 fld dword ptr [esp + 0x20]
0x004745ad: 89542420                 mov dword ptr [esp + 0x20], edx
0x004745b1: d844244c                 fadd dword ptr [esp + 0x4c]
0x004745b5: 8d147e                   lea edx, [esi + edi*2]
0x004745b8: d95c242c                 fstp dword ptr [esp + 0x2c]
0x004745bc: db442420                 fild dword ptr [esp + 0x20]
0x004745c0: 89542420                 mov dword ptr [esp + 0x20], edx
0x004745c4: 8d547e01                 lea edx, [esi + edi*2 + 1]
0x004745c8: d8442410                 fadd dword ptr [esp + 0x10]
0x004745cc: d9fe                     fsin 
0x004745ce: dc0df8e15c00             fmul qword ptr [0x5ce1f8]
0x004745d4: d8442434                 fadd dword ptr [esp + 0x34]
0x004745d8: d95c2434                 fstp dword ptr [esp + 0x34]
0x004745dc: db442420                 fild dword ptr [esp + 0x20]
0x004745e0: 89542420                 mov dword ptr [esp + 0x20], edx
0x004745e4: 8d547e02                 lea edx, [esi + edi*2 + 2]
0x004745e8: d8442410                 fadd dword ptr [esp + 0x10]
0x004745ec: d9fe                     fsin 
0x004745ee: dcc0                     fadd st(0), st(0)
0x004745f0: d8c1                     fadd st(1)
0x004745f2: d95c2440                 fstp dword ptr [esp + 0x40]
0x004745f6: db442420                 fild dword ptr [esp + 0x20]
0x004745fa: 89542420                 mov dword ptr [esp + 0x20], edx
0x004745fe: 8d547e03                 lea edx, [esi + edi*2 + 3]
0x00474602: d8442410                 fadd dword ptr [esp + 0x10]
0x00474606: d9fe                     fsin 
0x00474608: dcc0                     fadd st(0), st(0)
0x0047460a: d8c1                     fadd st(1)
0x0047460c: d95c2448                 fstp dword ptr [esp + 0x48]
0x00474610: ddd8                     fstp st(0)
0x00474612: db442420                 fild dword ptr [esp + 0x20]
0x00474616: 89542420                 mov dword ptr [esp + 0x20], edx
0x0047461a: d8442410                 fadd dword ptr [esp + 0x10]
0x0047461e: d9fe                     fsin 
0x00474620: dcc0                     fadd st(0), st(0)
0x00474622: d8442430                 fadd dword ptr [esp + 0x30]
0x00474626: d95c2430                 fstp dword ptr [esp + 0x30]
0x0047462a: db442420                 fild dword ptr [esp + 0x20]
0x0047462e: d8442410                 fadd dword ptr [esp + 0x10]
0x00474632: d9fe                     fsin 
0x00474634: dcc0                     fadd st(0), st(0)
0x00474636: d8442438                 fadd dword ptr [esp + 0x38]
0x0047463a: d95c2438                 fstp dword ptr [esp + 0x38]
0x0047463e: 85c0                     test eax, eax
0x00474640: 7517                     jne 0x474659
0x00474642: 83ff02                   cmp edi, 2
0x00474645: 7526                     jne 0x47466d
0x00474647: c744243800008042         mov dword ptr [esp + 0x38], 0x42800000
0x0047464f: c744243000008042         mov dword ptr [esp + 0x30], 0x42800000
0x00474657: eb14                     jmp 0x47466d
0x00474659: 85ff                     test edi, edi
0x0047465b: 7510                     jne 0x47466d
0x0047465d: c74424480000d043         mov dword ptr [esp + 0x48], 0x43d00000
0x00474665: c74424400000d043         mov dword ptr [esp + 0x40], 0x43d00000
0x0047466d: 85f6                     test esi, esi
0x0047466f: 8b5c2418                 mov ebx, dword ptr [esp + 0x18]
0x00474673: 895c2420                 mov dword ptr [esp + 0x20], ebx
0x00474677: 750c                     jne 0x474685
0x00474679: 85c9                     test ecx, ecx
0x0047467b: 7508                     jne 0x474685
0x0047467d: 8b5c241c                 mov ebx, dword ptr [esp + 0x1c]
0x00474681: 895c2420                 mov dword ptr [esp + 0x20], ebx
0x00474685: e82672fbff               call 0x42b8b0
0x0047468a: 0fbfc0                   movsx eax, ax
0x0047468d: 89442410                 mov dword ptr [esp + 0x10], eax
0x00474691: db442410                 fild dword ptr [esp + 0x10]
0x00474695: d84c243c                 fmul dword ptr [esp + 0x3c]
0x00474699: d80da8d55c00             fmul dword ptr [0x5cd5a8]
0x0047469f: d95c243c                 fstp dword ptr [esp + 0x3c]
0x004746a3: e81872fbff               call 0x42b8c0
0x004746a8: 0fbfc8                   movsx ecx, ax
0x004746ab: 894c2410                 mov dword ptr [esp + 0x10], ecx
0x004746af: db442410                 fild dword ptr [esp + 0x10]
0x004746b3: d84c2440                 fmul dword ptr [esp + 0x40]
0x004746b7: d80d60c55c00             fmul dword ptr [0x5cc560]
0x004746bd: d95c2440                 fstp dword ptr [esp + 0x40]
0x004746c1: e8ea71fbff               call 0x42b8b0
0x004746c6: 0fbfd0                   movsx edx, ax
0x004746c9: 89542410                 mov dword ptr [esp + 0x10], edx
0x004746cd: db442410                 fild dword ptr [esp + 0x10]
0x004746d1: d84c2444                 fmul dword ptr [esp + 0x44]
0x004746d5: d80da8d55c00             fmul dword ptr [0x5cd5a8]
0x004746db: d95c2444                 fstp dword ptr [esp + 0x44]
0x004746df: e8dc71fbff               call 0x42b8c0
0x004746e4: 0fbfc0                   movsx eax, ax
0x004746e7: 89442410                 mov dword ptr [esp + 0x10], eax
0x004746eb: db442410                 fild dword ptr [esp + 0x10]
0x004746ef: d84c2448                 fmul dword ptr [esp + 0x48]
0x004746f3: d80d60c55c00             fmul dword ptr [0x5cc560]
0x004746f9: d95c2448                 fstp dword ptr [esp + 0x48]
0x004746fd: e8ae71fbff               call 0x42b8b0
0x00474702: 0fbfc8                   movsx ecx, ax
0x00474705: 894c2410                 mov dword ptr [esp + 0x10], ecx
0x00474709: db442410                 fild dword ptr [esp + 0x10]
0x0047470d: d84c242c                 fmul dword ptr [esp + 0x2c]
0x00474711: d80da8d55c00             fmul dword ptr [0x5cd5a8]
0x00474717: d95c242c                 fstp dword ptr [esp + 0x2c]
0x0047471b: e8a071fbff               call 0x42b8c0
0x00474720: 0fbfd0                   movsx edx, ax
0x00474723: 89542410                 mov dword ptr [esp + 0x10], edx
0x00474727: db442410                 fild dword ptr [esp + 0x10]
0x0047472b: d84c2430                 fmul dword ptr [esp + 0x30]
0x0047472f: d80d60c55c00             fmul dword ptr [0x5cc560]
0x00474735: d95c2430                 fstp dword ptr [esp + 0x30]
0x00474739: e87271fbff               call 0x42b8b0
0x0047473e: 0fbfc0                   movsx eax, ax
0x00474741: 89442410                 mov dword ptr [esp + 0x10], eax
0x00474745: db442410                 fild dword ptr [esp + 0x10]
0x00474749: d84c2434                 fmul dword ptr [esp + 0x34]
0x0047474d: d80da8d55c00             fmul dword ptr [0x5cd5a8]
0x00474753: d95c2434                 fstp dword ptr [esp + 0x34]
0x00474757: e86471fbff               call 0x42b8c0
0x0047475c: 8b54243c                 mov edx, dword ptr [esp + 0x3c]
0x00474760: 0fbfc8                   movsx ecx, ax
0x00474763: 8b442440                 mov eax, dword ptr [esp + 0x40]
0x00474767: 894c2410                 mov dword ptr [esp + 0x10], ecx
0x0047476b: 8b4c2444                 mov ecx, dword ptr [esp + 0x44]
0x0047476f: a3248a8900               mov dword ptr [0x898a24], eax
0x00474774: 8b44242c                 mov eax, dword ptr [esp + 0x2c]
0x00474778: db442410                 fild dword ptr [esp + 0x10]
0x0047477c: 8915208a8900             mov dword ptr [0x898a20], edx
0x00474782: 8b542448                 mov edx, dword ptr [esp + 0x48]
0x00474786: d84c2438                 fmul dword ptr [esp + 0x38]
0x0047478a: 890d3c8a8900             mov dword ptr [0x898a3c], ecx
0x00474790: 8b4c2430                 mov ecx, dword ptr [esp + 0x30]
0x00474794: a3588a8900               mov dword ptr [0x898a58], eax
0x00474799: d80d60c55c00             fmul dword ptr [0x5cc560]
0x0047479f: 8915408a8900             mov dword ptr [0x898a40], edx
0x004747a5: 8b542434                 mov edx, dword ptr [esp + 0x34]
0x004747a9: 890d5c8a8900             mov dword ptr [0x898a5c], ecx
0x004747af: 668b4c2422               mov cx, word ptr [esp + 0x22]
0x004747b4: d91d788a8900             fstp dword ptr [0x898a78]
0x004747ba: 0fb6c5                   movzx eax, ch
0x004747bd: 8915748a8900             mov dword ptr [0x898a74], edx
0x004747c3: c1e008                   shl eax, 8
0x004747c6: 0fb6d3                   movzx edx, bl
0x004747c9: 0bc2                     or eax, edx
0x004747cb: c1e008                   shl eax, 8
0x004747ce: 0fb6d7                   movzx edx, bh
0x004747d1: 0bc2                     or eax, edx
0x004747d3: 0fb6c9                   movzx ecx, cl
0x004747d6: c1e008                   shl eax, 8
0x004747d9: 0bc1                     or eax, ecx
0x004747db: b9808080ff               mov ecx, 0xff808080
0x004747e0: a3308a8900               mov dword ptr [0x898a30], eax
0x004747e5: 890d4c8a8900             mov dword ptr [0x898a4c], ecx
0x004747eb: a3688a8900               mov dword ptr [0x898a68], eax
0x004747f0: 890d848a8900             mov dword ptr [0x898a84], ecx
0x004747f6: 8b0df83f7d00             mov ecx, dword ptr [0x7d3ff8]
0x004747fc: b82c8a8900               mov eax, 0x898a2c
0x00474801: 8b5118                   mov edx, dword ptr [ecx + 0x18]
0x00474804: 8950fc                   mov dword ptr [eax - 4], edx
0x00474807: c7000000803f             mov dword ptr [eax], 0x3f800000
0x0047480d: 83c01c                   add eax, 0x1c
0x00474810: 3d9c8a8900               cmp eax, 0x898a9c
0x00474815: 7cea                     jl 0x474801
0x00474817: 6a00                     push 0
0x00474819: 6a01                     push 1
0x0047481b: ff5120                   call dword ptr [ecx + 0x20]
0x0047481e: a1f83f7d00               mov eax, dword ptr [0x7d3ff8]
0x00474823: 6a01                     push 1
0x00474825: 6a0c                     push 0xc
0x00474827: ff5020                   call dword ptr [eax + 0x20]
0x0047482a: 8b0df83f7d00             mov ecx, dword ptr [0x7d3ff8]
0x00474830: 6a05                     push 5
0x00474832: 6a0a                     push 0xa
0x00474834: ff5120                   call dword ptr [ecx + 0x20]
0x00474837: 8b15f83f7d00             mov edx, dword ptr [0x7d3ff8]
0x0047483d: 6a06                     push 6
0x0047483f: 6a0b                     push 0xb
0x00474841: ff5220                   call dword ptr [edx + 0x20]
0x00474844: a1f83f7d00               mov eax, dword ptr [0x7d3ff8]
0x00474849: 6a04                     push 4
0x0047484b: 68208a8900               push 0x898a20
0x00474850: 6a04                     push 4
0x00474852: ff5030                   call dword ptr [eax + 0x30]
0x00474855: 8b442440                 mov eax, dword ptr [esp + 0x40]
0x00474859: 83c42c                   add esp, 0x2c
0x0047485c: 46                       inc esi
0x0047485d: 83fe07                   cmp esi, 7
0x00474860: 0f8c9ffcffff             jl 0x474505
0x00474866: 47                       inc edi
0x00474867: 83ff03                   cmp edi, 3
0x0047486a: 0f8c72fcffff             jl 0x4744e2
0x00474870: 40                       inc eax
0x00474871: 83f802                   cmp eax, 2
0x00474874: 89442414                 mov dword ptr [esp + 0x14], eax
0x00474878: 0f8c58fcffff             jl 0x4744d6
0x0047487e: 5f                       pop edi
0x0047487f: 5e                       pop esi
0x00474880: 5d                       pop ebp
0x00474881: 5b                       pop ebx
0x00474882: 83c444                   add esp, 0x44
0x00474885: c3                       ret 
0x00474886: 90                       nop 
0x00474887: 90                       nop 
0x00474888: 90                       nop 
0x00474889: 90                       nop 
0x0047488a: 90                       nop 
0x0047488b: 90                       nop 
0x0047488c: 90                       nop 
0x0047488d: 90                       nop 
0x0047488e: 90                       nop 
0x0047488f: 90                       nop 
