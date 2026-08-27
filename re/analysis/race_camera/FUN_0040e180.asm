0x0040e180: 83ec3c                   sub esp, 0x3c
0x0040e183: 53                       push ebx
0x0040e184: 55                       push ebp
0x0040e185: 56                       push esi
0x0040e186: 57                       push edi
0x0040e187: 83cfff                   or edi, 0xffffffff
0x0040e18a: be34000000               mov esi, 0x34
0x0040e18f: 83cdff                   or ebp, 0xffffffff
0x0040e192: c744241c00000000         mov dword ptr [esp + 0x1c], 0
0x0040e19a: 897c2410                 mov dword ptr [esp + 0x10], edi
0x0040e19e: 33db                     xor ebx, ebx
0x0040e1a0: 89742420                 mov dword ptr [esp + 0x20], esi
0x0040e1a4: eb0a                     jmp 0x40e1b0
0x0040e1a6: 8da42400000000           lea esp, [esp]
0x0040e1ad: 8d4900                   lea ecx, [ecx]
0x0040e1b0: a170275f00               mov eax, dword ptr [0x5f2770]
0x0040e1b5: 833c0600                 cmp dword ptr [esi + eax], 0
0x0040e1b9: 0f8419010000             je 0x40e2d8
0x0040e1bf: 53                       push ebx
0x0040e1c0: e8ebe50500               call 0x46c7b0
0x0040e1c5: 83c404                   add esp, 4
0x0040e1c8: 83f801                   cmp eax, 1
0x0040e1cb: 0f8507010000             jne 0x40e2d8
0x0040e1d1: 8d4c2424                 lea ecx, [esp + 0x24]
0x0040e1d5: 51                       push ecx
0x0040e1d6: 8d542418                 lea edx, [esp + 0x18]
0x0040e1da: 52                       push edx
0x0040e1db: 53                       push ebx
0x0040e1dc: e8cfe90500               call 0x46cbb0
0x0040e1e1: 8b442420                 mov eax, dword ptr [esp + 0x20]
0x0040e1e5: 83c40c                   add esp, 0xc
0x0040e1e8: 85c0                     test eax, eax
0x0040e1ea: 0f85e8000000             jne 0x40e2d8
0x0040e1f0: 8d442418                 lea eax, [esp + 0x18]
0x0040e1f4: 53                       push ebx
0x0040e1f5: 50                       push eax
0x0040e1f6: e8a5f20500               call 0x46d4a0
0x0040e1fb: 8b442420                 mov eax, dword ptr [esp + 0x20]
0x0040e1ff: 8b4830                   mov ecx, dword ptr [eax + 0x30]
0x0040e202: 8b5034                   mov edx, dword ptr [eax + 0x34]
0x0040e205: 8b4038                   mov eax, dword ptr [eax + 0x38]
0x0040e208: 83c408                   add esp, 8
0x0040e20b: 894c2428                 mov dword ptr [esp + 0x28], ecx
0x0040e20f: 8954242c                 mov dword ptr [esp + 0x2c], edx
0x0040e213: 89442430                 mov dword ptr [esp + 0x30], eax
0x0040e217: 33f6                     xor esi, esi
0x0040e219: bf34000000               mov edi, 0x34
0x0040e21e: 8bff                     mov edi, edi
0x0040e220: 8b0d70275f00             mov ecx, dword ptr [0x5f2770]
0x0040e226: 833c0f00                 cmp dword ptr [edi + ecx], 0
0x0040e22a: 0f8493000000             je 0x40e2c3
0x0040e230: 56                       push esi
0x0040e231: e87ae50500               call 0x46c7b0
0x0040e236: 83c404                   add esp, 4
0x0040e239: 83f801                   cmp eax, 1
0x0040e23c: 0f8581000000             jne 0x40e2c3
0x0040e242: 8d542424                 lea edx, [esp + 0x24]
0x0040e246: 52                       push edx
0x0040e247: 8d442418                 lea eax, [esp + 0x18]
0x0040e24b: 50                       push eax
0x0040e24c: 56                       push esi
0x0040e24d: e85ee90500               call 0x46cbb0
0x0040e252: 8b442420                 mov eax, dword ptr [esp + 0x20]
0x0040e256: 83c40c                   add esp, 0xc
0x0040e259: 85c0                     test eax, eax
0x0040e25b: 7566                     jne 0x40e2c3
0x0040e25d: 8d4c2418                 lea ecx, [esp + 0x18]
0x0040e261: 56                       push esi
0x0040e262: 51                       push ecx
0x0040e263: e838f20500               call 0x46d4a0
0x0040e268: 8b442420                 mov eax, dword ptr [esp + 0x20]
0x0040e26c: d94030                   fld dword ptr [eax + 0x30]
0x0040e26f: 8b5038                   mov edx, dword ptr [eax + 0x38]
0x0040e272: d94034                   fld dword ptr [eax + 0x34]
0x0040e275: 89542450                 mov dword ptr [esp + 0x50], edx
0x0040e279: d9442430                 fld dword ptr [esp + 0x30]
0x0040e27d: 8d44243c                 lea eax, [esp + 0x3c]
0x0040e281: d8e2                     fsub st(2)
0x0040e283: 50                       push eax
0x0040e284: d95c2440                 fstp dword ptr [esp + 0x40]
0x0040e288: d9442438                 fld dword ptr [esp + 0x38]
0x0040e28c: d8e1                     fsub st(1)
0x0040e28e: d95c2444                 fstp dword ptr [esp + 0x44]
0x0040e292: ddd8                     fstp st(0)
0x0040e294: ddd8                     fstp st(0)
0x0040e296: d944243c                 fld dword ptr [esp + 0x3c]
0x0040e29a: d8642454                 fsub dword ptr [esp + 0x54]
0x0040e29e: d95c2448                 fstp dword ptr [esp + 0x48]
0x0040e2a2: e819580b00               call 0x4c3ac0
0x0040e2a7: d8542428                 fcom dword ptr [esp + 0x28]
0x0040e2ab: 83c40c                   add esp, 0xc
0x0040e2ae: dfe0                     fnstsw ax
0x0040e2b0: f6c401                   test ah, 1
0x0040e2b3: 750c                     jne 0x40e2c1
0x0040e2b5: d95c241c                 fstp dword ptr [esp + 0x1c]
0x0040e2b9: 8bee                     mov ebp, esi
0x0040e2bb: 895c2410                 mov dword ptr [esp + 0x10], ebx
0x0040e2bf: eb02                     jmp 0x40e2c3
0x0040e2c1: ddd8                     fstp st(0)
0x0040e2c3: 83c704                   add edi, 4
0x0040e2c6: 46                       inc esi
0x0040e2c7: 83ff44                   cmp edi, 0x44
0x0040e2ca: 0f8c50ffffff             jl 0x40e220
0x0040e2d0: 8b7c2410                 mov edi, dword ptr [esp + 0x10]
0x0040e2d4: 8b742420                 mov esi, dword ptr [esp + 0x20]
0x0040e2d8: 83c604                   add esi, 4
0x0040e2db: 43                       inc ebx
0x0040e2dc: 83fe44                   cmp esi, 0x44
0x0040e2df: 89742420                 mov dword ptr [esp + 0x20], esi
0x0040e2e3: 0f8cc7feffff             jl 0x40e1b0
0x0040e2e9: 83fdff                   cmp ebp, -1
0x0040e2ec: 7502                     jne 0x40e2f0
0x0040e2ee: 8bef                     mov ebp, edi
0x0040e2f0: 83ffff                   cmp edi, -1
0x0040e2f3: 7506                     jne 0x40e2fb
0x0040e2f5: 896c2410                 mov dword ptr [esp + 0x10], ebp
0x0040e2f9: 8bfd                     mov edi, ebp
0x0040e2fb: 83fdff                   cmp ebp, -1
0x0040e2fe: 7502                     jne 0x40e302
0x0040e300: 33ed                     xor ebp, ebp
0x0040e302: 83ffff                   cmp edi, -1
0x0040e305: 7516                     jne 0x40e31d
0x0040e307: 8b4c2450                 mov ecx, dword ptr [esp + 0x50]
0x0040e30b: 8b542454                 mov edx, dword ptr [esp + 0x54]
0x0040e30f: 5f                       pop edi
0x0040e310: 5e                       pop esi
0x0040e311: 8929                     mov dword ptr [ecx], ebp
0x0040e313: 33c0                     xor eax, eax
0x0040e315: 5d                       pop ebp
0x0040e316: 8902                     mov dword ptr [edx], eax
0x0040e318: 5b                       pop ebx
0x0040e319: 83c43c                   add esp, 0x3c
0x0040e31c: c3                       ret
0x0040e31d: 8b442450                 mov eax, dword ptr [esp + 0x50]
0x0040e321: 8b4c2454                 mov ecx, dword ptr [esp + 0x54]
0x0040e325: 8928                     mov dword ptr [eax], ebp
0x0040e327: 8939                     mov dword ptr [ecx], edi
0x0040e329: 5f                       pop edi
0x0040e32a: 5e                       pop esi
0x0040e32b: 5d                       pop ebp
0x0040e32c: 5b                       pop ebx
0x0040e32d: 83c43c                   add esp, 0x3c
0x0040e330: c3                       ret
0x0040e331: 90                       nop
0x0040e332: 90                       nop
0x0040e333: 90                       nop
0x0040e334: 90                       nop
0x0040e335: 90                       nop
0x0040e336: 90                       nop
0x0040e337: 90                       nop
0x0040e338: 90                       nop
0x0040e339: 90                       nop
0x0040e33a: 90                       nop
0x0040e33b: 90                       nop
0x0040e33c: 90                       nop
0x0040e33d: 90                       nop
0x0040e33e: 90                       nop
0x0040e33f: 90                       nop
0x0040e340: a1d0948a00               mov eax, dword ptr [0x8a94d0]
0x0040e345: c3                       ret
0x0040e346: 90                       nop
0x0040e347: 90                       nop
0x0040e348: 90                       nop
0x0040e349: 90                       nop
0x0040e34a: 90                       nop
0x0040e34b: 90                       nop
0x0040e34c: 90                       nop
0x0040e34d: 90                       nop
0x0040e34e: 90                       nop
0x0040e34f: 90                       nop
0x0040e350: a18cba6300               mov eax, dword ptr [0x63ba8c]
0x0040e355: c3                       ret
0x0040e356: 90                       nop
0x0040e357: 90                       nop
0x0040e358: 90                       nop
0x0040e359: 90                       nop
0x0040e35a: 90                       nop
0x0040e35b: 90                       nop
0x0040e35c: 90                       nop
0x0040e35d: 90                       nop
0x0040e35e: 90                       nop
0x0040e35f: 90                       nop
0x0040e360: 8b442404                 mov eax, dword ptr [esp + 4]
0x0040e364: a38cba6300               mov dword ptr [0x63ba8c], eax
0x0040e369: c3                       ret
0x0040e36a: 90                       nop
0x0040e36b: 90                       nop
0x0040e36c: 90                       nop
0x0040e36d: 90                       nop
0x0040e36e: 90                       nop
0x0040e36f: 90                       nop
0x0040e370: 8b442404                 mov eax, dword ptr [esp + 4]
0x0040e374: 83f804                   cmp eax, 4
0x0040e377: 7c03                     jl 0x40e37c
0x0040e379: 33c0                     xor eax, eax
0x0040e37b: c3                       ret
0x0040e37c: 8b1570275f00             mov edx, dword ptr [0x5f2770]
0x0040e382: 56                       push esi
0x0040e383: 8b748234                 mov esi, dword ptr [edx + eax*4 + 0x34]
0x0040e387: 33c9                     xor ecx, ecx
0x0040e389: 85f6                     test esi, esi
0x0040e38b: 0f95c1                   setne cl
0x0040e38e: 5e                       pop esi
0x0040e38f: 8bc1                     mov eax, ecx
0x0040e391: c3                       ret
0x0040e392: 90                       nop
0x0040e393: 90                       nop
0x0040e394: 90                       nop
0x0040e395: 90                       nop
0x0040e396: 90                       nop
0x0040e397: 90                       nop
0x0040e398: 90                       nop
0x0040e399: 90                       nop
0x0040e39a: 90                       nop
0x0040e39b: 90                       nop
0x0040e39c: 90                       nop
0x0040e39d: 90                       nop
0x0040e39e: 90                       nop
0x0040e39f: 90                       nop
0x0040e3a0: 8b442404                 mov eax, dword ptr [esp + 4]
0x0040e3a4: 83f805                   cmp eax, 5
0x0040e3a7: 777e                     ja 0x40e427
0x0040e3a9: ff248530e44000           jmp dword ptr [eax*4 + 0x40e430]
0x0040e3b0: 8b442408                 mov eax, dword ptr [esp + 8]
0x0040e3b4: c60098                   mov byte ptr [eax], 0x98
0x0040e3b7: c640013a                 mov byte ptr [eax + 1], 0x3a
0x0040e3bb: c640023d                 mov byte ptr [eax + 2], 0x3d
0x0040e3bf: c64003ff                 mov byte ptr [eax + 3], 0xff
0x0040e3c3: c3                       ret
0x0040e3c4: 8b442408                 mov eax, dword ptr [esp + 8]
0x0040e3c8: c6004e                   mov byte ptr [eax], 0x4e
0x0040e3cb: c6400189                 mov byte ptr [eax + 1], 0x89
0x0040e3cf: c64002ae                 mov byte ptr [eax + 2], 0xae
0x0040e3d3: c64003ff                 mov byte ptr [eax + 3], 0xff
0x0040e3d7: c3                       ret
0x0040e3d8: 8b442408                 mov eax, dword ptr [esp + 8]
0x0040e3dc: c60061                   mov byte ptr [eax], 0x61
0x0040e3df: c6400176                 mov byte ptr [eax + 1], 0x76
0x0040e3e3: c6400256                 mov byte ptr [eax + 2], 0x56
0x0040e3e7: c64003ff                 mov byte ptr [eax + 3], 0xff
0x0040e3eb: c3                       ret
0x0040e3ec: 8b442408                 mov eax, dword ptr [esp + 8]
0x0040e3f0: c600db                   mov byte ptr [eax], 0xdb
0x0040e3f3: c64001c3                 mov byte ptr [eax + 1], 0xc3
0x0040e3f7: c6400262                 mov byte ptr [eax + 2], 0x62
0x0040e3fb: c64003ff                 mov byte ptr [eax + 3], 0xff
0x0040e3ff: c3                       ret
0x0040e400: 8b442408                 mov eax, dword ptr [esp + 8]
0x0040e404: b1a7                     mov cl, 0xa7
0x0040e406: c600eb                   mov byte ptr [eax], 0xeb
0x0040e409: 884801                   mov byte ptr [eax + 1], cl
0x0040e40c: 884802                   mov byte ptr [eax + 2], cl
0x0040e40f: c64003ff                 mov byte ptr [eax + 3], 0xff
0x0040e413: c3                       ret
0x0040e414: 8b442408                 mov eax, dword ptr [esp + 8]
0x0040e418: 32c9                     xor cl, cl
0x0040e41a: 8808                     mov byte ptr [eax], cl
0x0040e41c: 884801                   mov byte ptr [eax + 1], cl
0x0040e41f: 884802                   mov byte ptr [eax + 2], cl
0x0040e422: c64003ff                 mov byte ptr [eax + 3], 0xff
0x0040e426: c3                       ret
0x0040e427: 6af2                     push -0xe
0x0040e429: e8fd4e0900               call 0x4a332b
0x0040e42e: 8bff                     mov edi, edi
0x0040e430: b0e3                     mov al, 0xe3
0x0040e432: 40                       inc eax
0x0040e433: 00c4                     add ah, al
0x0040e435: e340                     jecxz 0x40e477
0x0040e437: 00d8                     add al, bl
0x0040e439: e340                     jecxz 0x40e47b
