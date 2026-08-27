0x00441820: 83ec64                   sub esp, 0x64
0x00441823: 53                       push ebx
0x00441824: 8b5c2474                 mov ebx, dword ptr [esp + 0x74]
0x00441828: 56                       push esi
0x00441829: 57                       push edi
0x0044182a: c744241800000000         mov dword ptr [esp + 0x18], 0
0x00441832: c744241c000080bf         mov dword ptr [esp + 0x1c], 0xbf800000
0x0044183a: c744242000000000         mov dword ptr [esp + 0x20], 0
0x00441842: c70300000000             mov dword ptr [ebx], 0
0x00441848: e8437ffcff               call 0x409790
0x0044184d: e84e80fcff               call 0x4098a0
0x00441852: 85c0                     test eax, eax
0x00441854: 8b7c2474                 mov edi, dword ptr [esp + 0x74]
0x00441858: 0f848b000000             je 0x4418e9
0x0044185e: 8d0c7f                   lea ecx, [edi + edi*2]
0x00441861: 8d3488                   lea esi, [eax + ecx*4]
0x00441864: 8b4608                   mov eax, dword ptr [esi + 8]
0x00441867: 3d000080bf               cmp eax, 0xbf800000
0x0044186c: 7404                     je 0x441872
0x0044186e: 8bd0                     mov edx, eax
0x00441870: 8913                     mov dword ptr [ebx], edx
0x00441872: 813e000080bf             cmp dword ptr [esi], 0xbf800000
0x00441878: 746f                     je 0x4418e9
0x0044187a: d905d0ca5c00             fld dword ptr [0x5ccad0]
0x00441880: 6a00                     push 0
0x00441882: d826                     fsub dword ptr [esi]
0x00441884: 51                       push ecx
0x00441885: 8d442438                 lea eax, [esp + 0x38]
0x00441889: d91c24                   fstp dword ptr [esp]
0x0044188c: 68f0466100               push 0x6146f0
0x00441891: 50                       push eax
0x00441892: e889340800               call 0x4c4d20
0x00441897: 8bbc2488000000           mov edi, dword ptr [esp + 0x88]
0x0044189e: 8d4c2440                 lea ecx, [esp + 0x40]
0x004418a2: 51                       push ecx
0x004418a3: 6a01                     push 1
0x004418a5: 8d542430                 lea edx, [esp + 0x30]
0x004418a9: 52                       push edx
0x004418aa: 57                       push edi
0x004418ab: e840250800               call 0x4c3df0
0x004418b0: d94604                   fld dword ptr [esi + 4]
0x004418b3: d8059cd05c00             fadd dword ptr [0x5cd09c]
0x004418b9: 83c420                   add esp, 0x20
0x004418bc: 6a00                     push 0
0x004418be: 51                       push ecx
0x004418bf: d91c24                   fstp dword ptr [esp]
0x004418c2: 8d442438                 lea eax, [esp + 0x38]
0x004418c6: 68fc466100               push 0x6146fc
0x004418cb: 50                       push eax
0x004418cc: e84f340800               call 0x4c4d20
0x004418d1: 8d4c2440                 lea ecx, [esp + 0x40]
0x004418d5: 51                       push ecx
0x004418d6: 6a01                     push 1
0x004418d8: 57                       push edi
0x004418d9: 57                       push edi
0x004418da: e811250800               call 0x4c3df0
0x004418df: 83c420                   add esp, 0x20
0x004418e2: 5f                       pop edi
0x004418e3: 5e                       pop esi
0x004418e4: 5b                       pop ebx
0x004418e5: 83c464                   add esp, 0x64
0x004418e8: c3                       ret
0x004418e9: 57                       push edi
0x004418ea: e8d153feff               call 0x426cc0
0x004418ef: 8b10                     mov edx, dword ptr [eax]
0x004418f1: 8b74247c                 mov esi, dword ptr [esp + 0x7c]
0x004418f5: 8916                     mov dword ptr [esi], edx
0x004418f7: 8b4804                   mov ecx, dword ptr [eax + 4]
0x004418fa: 894e04                   mov dword ptr [esi + 4], ecx
0x004418fd: 8b5008                   mov edx, dword ptr [eax + 8]
0x00441900: 6a00                     push 0
0x00441902: 57                       push edi
0x00441903: 895608                   mov dword ptr [esi + 8], edx
0x00441906: e8f553feff               call 0x426d00
0x0044190b: 8b08                     mov ecx, dword ptr [eax]
0x0044190d: 894c2418                 mov dword ptr [esp + 0x18], ecx
0x00441911: 8b5004                   mov edx, dword ptr [eax + 4]
0x00441914: 8954241c                 mov dword ptr [esp + 0x1c], edx
0x00441918: 8b4008                   mov eax, dword ptr [eax + 8]
0x0044191b: 6a03                     push 3
0x0044191d: 57                       push edi
0x0044191e: 89442428                 mov dword ptr [esp + 0x28], eax
0x00441922: e8d953feff               call 0x426d00
0x00441927: d900                     fld dword ptr [eax]
0x00441929: d94004                   fld dword ptr [eax + 4]
0x0044192c: 8b4808                   mov ecx, dword ptr [eax + 8]
0x0044192f: d9442420                 fld dword ptr [esp + 0x20]
0x00441933: 6a00                     push 0
0x00441935: d8e2                     fsub st(2)
0x00441937: 894c2444                 mov dword ptr [esp + 0x44], ecx
0x0044193b: 680000c8c1               push 0xc1c80000
0x00441940: 8d542428                 lea edx, [esp + 0x28]
0x00441944: d95c2428                 fstp dword ptr [esp + 0x28]
0x00441948: 52                       push edx
0x00441949: d9442430                 fld dword ptr [esp + 0x30]
0x0044194d: 8d442450                 lea eax, [esp + 0x50]
0x00441951: d8e1                     fsub st(1)
0x00441953: 50                       push eax
0x00441954: d95c2434                 fstp dword ptr [esp + 0x34]
0x00441958: ddd8                     fstp st(0)
0x0044195a: ddd8                     fstp st(0)
0x0044195c: d9442438                 fld dword ptr [esp + 0x38]
0x00441960: d8642450                 fsub dword ptr [esp + 0x50]
0x00441964: d95c2438                 fstp dword ptr [esp + 0x38]
0x00441968: e8b3330800               call 0x4c4d20
0x0044196d: 8d4c2454                 lea ecx, [esp + 0x54]
0x00441971: 51                       push ecx
0x00441972: 6a01                     push 1
0x00441974: 56                       push esi
0x00441975: 56                       push esi
0x00441976: e875240800               call 0x4c3df0
0x0044197b: 83c434                   add esp, 0x34
0x0044197e: 5f                       pop edi
0x0044197f: 5e                       pop esi
0x00441980: 5b                       pop ebx
0x00441981: 83c464                   add esp, 0x64
0x00441984: c3                       ret
0x00441985: 90                       nop
0x00441986: 90                       nop
0x00441987: 90                       nop
0x00441988: 90                       nop
0x00441989: 90                       nop
0x0044198a: 90                       nop
0x0044198b: 90                       nop
0x0044198c: 90                       nop
0x0044198d: 90                       nop
0x0044198e: 90                       nop
0x0044198f: 90                       nop
0x00441990: 83ec14                   sub esp, 0x14
0x00441993: d905581a7f00             fld dword ptr [0x7f1a58]
0x00441999: 56                       push esi
0x0044199a: 8b74241c                 mov esi, dword ptr [esp + 0x1c]
0x0044199e: d9542408                 fst dword ptr [esp + 8]
0x004419a2: d9055c1a7f00             fld dword ptr [0x7f1a5c]
0x004419a8: 8b8684000000             mov eax, dword ptr [esi + 0x84]
0x004419ae: d9542404                 fst dword ptr [esp + 4]
0x004419b2: 6a02                     push 2
0x004419b4: d9c9                     fxch st(1)
0x004419b6: 50                       push eax
0x004419b7: d95e40                   fstp dword ptr [esi + 0x40]
0x004419ba: c7464400007041           mov dword ptr [esi + 0x44], 0x41700000
0x004419c1: d95e48                   fstp dword ptr [esi + 0x48]
0x004419c4: e847020800               call 0x4c1c10
0x004419c9: d9442410                 fld dword ptr [esp + 0x10]
0x004419cd: 8d542414                 lea edx, [esp + 0x14]
0x004419d1: d86640                   fsub dword ptr [esi + 0x40]
0x004419d4: 52                       push edx
0x004419d5: c7465800008040           mov dword ptr [esi + 0x58], 0x40800000
0x004419dc: d9542414                 fst dword ptr [esp + 0x14]
0x004419e0: d94644                   fld dword ptr [esi + 0x44]
0x004419e3: d9e0                     fchs
0x004419e5: d9542428                 fst dword ptr [esp + 0x28]
0x004419e9: d9442410                 fld dword ptr [esp + 0x10]
0x004419ed: d86648                   fsub dword ptr [esi + 0x48]
0x004419f0: d95c2410                 fstp dword ptr [esp + 0x10]
0x004419f4: 8b4c2410                 mov ecx, dword ptr [esp + 0x10]
0x004419f8: d9c9                     fxch st(1)
0x004419fa: d95c2418                 fstp dword ptr [esp + 0x18]
0x004419fe: 894c2420                 mov dword ptr [esp + 0x20], ecx
0x00441a02: d95c241c                 fstp dword ptr [esp + 0x1c]
0x00441a06: e8b5200800               call 0x4c3ac0
0x00441a0b: ddd8                     fstp st(0)
0x00441a0d: 83c40c                   add esp, 0xc
0x00441a10: d9442404                 fld dword ptr [esp + 4]
0x00441a14: d81d7c755d00             fcomp dword ptr [0x5d757c]
0x00441a1a: d9442408                 fld dword ptr [esp + 8]
0x00441a1e: dfe0                     fnstsw ax
0x00441a20: f6c444                   test ah, 0x44
0x00441a23: 7b2f                     jnp 0x441a54
0x00441a25: d8742404                 fdiv dword ptr [esp + 4]
0x00441a29: d9e8                     fld1
0x00441a2b: d9f3                     fpatan
0x00441a2d: dc0de0ca5c00             fmul qword ptr [0x5ccae0]
0x00441a33: d9e0                     fchs
0x00441a35: d82d9cd05c00             fsubr dword ptr [0x5cd09c]
0x00441a3b: d9442404                 fld dword ptr [esp + 4]
0x00441a3f: d81d7c755d00             fcomp dword ptr [0x5d757c]
0x00441a45: dfe0                     fnstsw ax
0x00441a47: f6c441                   test ah, 0x41
0x00441a4a: 7523                     jne 0x441a6f
0x00441a4c: d8059cd05c00             fadd dword ptr [0x5cd09c]
0x00441a52: eb1b                     jmp 0x441a6f
0x00441a54: d81d7c755d00             fcomp dword ptr [0x5d757c]
0x00441a5a: dfe0                     fnstsw ax
0x00441a5c: f6c405                   test ah, 5
0x00441a5f: 7a08                     jp 0x441a69
0x00441a61: d90524d35c00             fld dword ptr [0x5cd324]
0x00441a67: eb06                     jmp 0x441a6f
0x00441a69: d905d0ca5c00             fld dword ptr [0x5ccad0]
0x00441a6f: d95e38                   fstp dword ptr [esi + 0x38]
0x00441a72: d9442404                 fld dword ptr [esp + 4]
0x00441a76: d84c2404                 fmul dword ptr [esp + 4]
0x00441a7a: d9442408                 fld dword ptr [esp + 8]
0x00441a7e: d84c2408                 fmul dword ptr [esp + 8]
0x00441a82: dec1                     faddp st(1)
0x00441a84: d9542404                 fst dword ptr [esp + 4]
0x00441a88: d81d7c755d00             fcomp dword ptr [0x5d757c]
0x00441a8e: dfe0                     fnstsw ax
0x00441a90: f6c444                   test ah, 0x44
0x00441a93: 7b51                     jnp 0x441ae6
0x00441a95: 8b442404                 mov eax, dword ptr [esp + 4]
0x00441a99: 50                       push eax
0x00441a9a: e891200800               call 0x4c3b30
0x00441a9f: d9542408                 fst dword ptr [esp + 8]
0x00441aa3: d81d7c755d00             fcomp dword ptr [0x5d757c]
0x00441aa9: 83c404                   add esp, 4
0x00441aac: dfe0                     fnstsw ax
0x00441aae: f6c444                   test ah, 0x44
0x00441ab1: 7b33                     jnp 0x441ae6
0x00441ab3: d944241c                 fld dword ptr [esp + 0x1c]
0x00441ab7: d8742404                 fdiv dword ptr [esp + 4]
0x00441abb: d9e8                     fld1
0x00441abd: d9f3                     fpatan
0x00441abf: dc0de0ca5c00             fmul qword ptr [0x5ccae0]
0x00441ac5: d9e0                     fchs
0x00441ac7: d82d9cd05c00             fsubr dword ptr [0x5cd09c]
0x00441acd: d9442404                 fld dword ptr [esp + 4]
0x00441ad1: d81d7c755d00             fcomp dword ptr [0x5d757c]
0x00441ad7: dfe0                     fnstsw ax
0x00441ad9: f6c441                   test ah, 0x41
0x00441adc: 7527                     jne 0x441b05
0x00441ade: d8059cd05c00             fadd dword ptr [0x5cd09c]
0x00441ae4: eb1f                     jmp 0x441b05
0x00441ae6: d944241c                 fld dword ptr [esp + 0x1c]
0x00441aea: d81d7c755d00             fcomp dword ptr [0x5d757c]
0x00441af0: dfe0                     fnstsw ax
0x00441af2: f6c405                   test ah, 5
0x00441af5: 7a08                     jp 0x441aff
0x00441af7: d90524d35c00             fld dword ptr [0x5cd324]
0x00441afd: eb06                     jmp 0x441b05
0x00441aff: d905d0ca5c00             fld dword ptr [0x5ccad0]
0x00441b05: d905c4ca5c00             fld dword ptr [0x5ccac4]
0x00441b0b: 56                       push esi
0x00441b0c: d8e1                     fsub st(1)
0x00441b0e: c7463c00000000           mov dword ptr [esi + 0x3c], 0
0x00441b15: d95e34                   fstp dword ptr [esi + 0x34]
0x00441b18: ddd8                     fstp st(0)
0x00441b1a: e841fcffff               call 0x441760
0x00441b1f: 83c404                   add esp, 4
0x00441b22: 5e                       pop esi
0x00441b23: 83c414                   add esp, 0x14
0x00441b26: c3                       ret
0x00441b27: 90                       nop
0x00441b28: 90                       nop
0x00441b29: 90                       nop
0x00441b2a: 90                       nop
0x00441b2b: 90                       nop
0x00441b2c: 90                       nop
0x00441b2d: 90                       nop
0x00441b2e: 90                       nop
0x00441b2f: 90                       nop
0x00441b30: 83ec20                   sub esp, 0x20
0x00441b33: 53                       push ebx
0x00441b34: 55                       push ebp
0x00441b35: 56                       push esi
0x00441b36: 8b742434                 mov esi, dword ptr [esp + 0x34]
0x00441b3a: 8b06                     mov eax, dword ptr [esi]
0x00441b3c: d9461c                   fld dword ptr [esi + 0x1c]
0x00441b3f: 8b4e04                   mov ecx, dword ptr [esi + 4]
0x00441b42: 8b5608                   mov edx, dword ptr [esi + 8]
0x00441b45: 89442420                 mov dword ptr [esp + 0x20], eax
0x00441b49: 8b460c                   mov eax, dword ptr [esi + 0xc]
0x00441b4c: 894c2424                 mov dword ptr [esp + 0x24], ecx
0x00441b50: 8b4e10                   mov ecx, dword ptr [esi + 0x10]
0x00441b53: 89542428                 mov dword ptr [esp + 0x28], edx
0x00441b57: 8b5614                   mov edx, dword ptr [esi + 0x14]
0x00441b5a: 89442414                 mov dword ptr [esp + 0x14], eax
0x00441b5e: 8b4618                   mov eax, dword ptr [esi + 0x18]
0x00441b61: 57                       push edi
0x00441b62: 894c241c                 mov dword ptr [esp + 0x1c], ecx
0x00441b66: 89542420                 mov dword ptr [esp + 0x20], edx
0x00441b6a: 89442438                 mov dword ptr [esp + 0x38], eax
0x00441b6e: e8d5100600               call 0x4a2c48
0x00441b73: d94620                   fld dword ptr [esi + 0x20]
0x00441b76: 8bf8                     mov edi, eax
0x00441b78: e8cb100600               call 0x4a2c48
0x00441b7d: d94624                   fld dword ptr [esi + 0x24]
0x00441b80: 8bd8                     mov ebx, eax
0x00441b82: e8c1100600               call 0x4a2c48
0x00441b87: 8b4e28                   mov ecx, dword ptr [esi + 0x28]
0x00441b8a: 8b562c                   mov edx, dword ptr [esi + 0x2c]
0x00441b8d: 8b742434                 mov esi, dword ptr [esp + 0x34]
0x00441b91: 897e04                   mov dword ptr [esi + 4], edi
0x00441b94: 33ff                     xor edi, edi
0x00441b96: 894c2410                 mov dword ptr [esp + 0x10], ecx
0x00441b9a: 89542414                 mov dword ptr [esp + 0x14], edx
0x00441b9e: 8b5618                   mov edx, dword ptr [esi + 0x18]
0x00441ba1: 57                       push edi
