0x00446520: 55                       push ebp
0x00446521: 8bec                     mov ebp, esp
0x00446523: 81ecd0010000             sub esp, 0x1d0
0x00446529: c745ac00000000           mov dword ptr [ebp - 0x54], 0
0x00446530: eb09                     jmp 0x44653b
0x00446532: 8b45ac                   mov eax, dword ptr [ebp - 0x54]
0x00446535: 83c001                   add eax, 1
0x00446538: 8945ac                   mov dword ptr [ebp - 0x54], eax
0x0044653b: 837dac04                 cmp dword ptr [ebp - 0x54], 4
0x0044653f: 7d10                     jge 0x446551
0x00446541: 6a00                     push 0
0x00446543: 8b4dac                   mov ecx, dword ptr [ebp - 0x54]
0x00446546: 51                       push ecx
0x00446547: e8148afdff               call 0x41ef60
0x0044654c: 83c408                   add esp, 8
0x0044654f: ebe1                     jmp 0x446532
0x00446551: 8b5508                   mov edx, dword ptr [ebp + 8]
0x00446554: a144338000               mov eax, dword ptr [0x803344]
0x00446559: 894224                   mov dword ptr [edx + 0x24], eax
0x0044655c: e89fc0ffff               call 0x442600
0x00446561: e8ea7dfcff               call 0x40e350
0x00446566: 898534ffffff             mov dword ptr [ebp - 0xcc], eax
0x0044656c: 833dd00f7f0005           cmp dword ptr [0x7f0fd0], 5
0x00446573: 0f8569030000             jne 0x4468e2
0x00446579: e812f3fbff               call 0x405890
0x0044657e: 85c0                     test eax, eax
0x00446580: 0f845c030000             je 0x4468e2
0x00446586: 6a00                     push 0
0x00446588: e87310fcff               call 0x407600
0x0044658d: 83c404                   add esp, 4
0x00446590: 8b08                     mov ecx, dword ptr [eax]
0x00446592: 898dd8feffff             mov dword ptr [ebp - 0x128], ecx
0x00446598: 8b5004                   mov edx, dword ptr [eax + 4]
0x0044659b: 8995dcfeffff             mov dword ptr [ebp - 0x124], edx
0x004465a1: 8b4008                   mov eax, dword ptr [eax + 8]
0x004465a4: 8985e0feffff             mov dword ptr [ebp - 0x120], eax
0x004465aa: 6a00                     push 0
0x004465ac: e8ef0b0200               call 0x4671a0
0x004465b1: 83c404                   add esp, 4
0x004465b4: 8985f0feffff             mov dword ptr [ebp - 0x110], eax
0x004465ba: 6a00                     push 0
0x004465bc: e84f0c0200               call 0x467210
0x004465c1: 83c404                   add esp, 4
0x004465c4: 898564feffff             mov dword ptr [ebp - 0x19c], eax
0x004465ca: c7855cfeffff00000000     mov dword ptr [ebp - 0x1a4], 0
0x004465d4: c78560feffff0000a040     mov dword ptr [ebp - 0x1a0], 0x40a00000
0x004465de: 8b0df0466100             mov ecx, dword ptr [0x6146f0]
0x004465e4: 898d8cfeffff             mov dword ptr [ebp - 0x174], ecx
0x004465ea: 8b15f4466100             mov edx, dword ptr [0x6146f4]
0x004465f0: 899590feffff             mov dword ptr [ebp - 0x170], edx
0x004465f6: a1f8466100               mov eax, dword ptr [0x6146f8]
0x004465fb: 898594feffff             mov dword ptr [ebp - 0x16c], eax
0x00446601: c785e4feffff00000000     mov dword ptr [ebp - 0x11c], 0
0x0044660b: db05f00f7f00             fild dword ptr [0x7f0ff0]
0x00446611: d80d3cd05c00             fmul dword ptr [0x5cd03c]
0x00446617: e894d10500               call 0x4a37b0
0x0044661c: dc0de0e15c00             fmul qword ptr [0x5ce1e0]
0x00446622: dc05d8e15c00             fadd qword ptr [0x5ce1d8]
0x00446628: d99de8feffff             fstp dword ptr [ebp - 0x118]
0x0044662e: c785ecfeffff00000000     mov dword ptr [ebp - 0x114], 0
0x00446638: 8d8de4feffff             lea ecx, [ebp - 0x11c]
0x0044663e: 51                       push ecx
0x0044663f: 8d95e4feffff             lea edx, [ebp - 0x11c]
0x00446645: 52                       push edx
0x00446646: e865d30700               call 0x4c39b0
0x0044664b: ddd8                     fstp st(0)
0x0044664d: 83c408                   add esp, 8
0x00446650: 8b85d8feffff             mov eax, dword ptr [ebp - 0x128]
0x00446656: 898574feffff             mov dword ptr [ebp - 0x18c], eax
0x0044665c: 8b8ddcfeffff             mov ecx, dword ptr [ebp - 0x124]
0x00446662: 898d78feffff             mov dword ptr [ebp - 0x188], ecx
0x00446668: 8b95e0feffff             mov edx, dword ptr [ebp - 0x120]
0x0044666e: 89957cfeffff             mov dword ptr [ebp - 0x184], edx
0x00446674: 8b85d8feffff             mov eax, dword ptr [ebp - 0x128]
0x0044667a: 898580feffff             mov dword ptr [ebp - 0x180], eax
0x00446680: 8b8ddcfeffff             mov ecx, dword ptr [ebp - 0x124]
0x00446686: 898d84feffff             mov dword ptr [ebp - 0x17c], ecx
0x0044668c: 8b95e0feffff             mov edx, dword ptr [ebp - 0x120]
0x00446692: 899588feffff             mov dword ptr [ebp - 0x178], edx
0x00446698: d985e4feffff             fld dword ptr [ebp - 0x11c]
0x0044669e: d88d60feffff             fmul dword ptr [ebp - 0x1a0]
0x004466a4: d88580feffff             fadd dword ptr [ebp - 0x180]
0x004466aa: d99d80feffff             fstp dword ptr [ebp - 0x180]
0x004466b0: d985e8feffff             fld dword ptr [ebp - 0x118]
0x004466b6: d88d60feffff             fmul dword ptr [ebp - 0x1a0]
0x004466bc: d88584feffff             fadd dword ptr [ebp - 0x17c]
0x004466c2: d99d84feffff             fstp dword ptr [ebp - 0x17c]
0x004466c8: d985ecfeffff             fld dword ptr [ebp - 0x114]
0x004466ce: d88d60feffff             fmul dword ptr [ebp - 0x1a0]
0x004466d4: d88588feffff             fadd dword ptr [ebp - 0x178]
0x004466da: d99d88feffff             fstp dword ptr [ebp - 0x178]
0x004466e0: 33c0                     xor eax, eax
0x004466e2: 85c0                     test eax, eax
0x004466e4: 75b2                     jne 0x446698
0x004466e6: 8b8d80feffff             mov ecx, dword ptr [ebp - 0x180]
0x004466ec: 898d68feffff             mov dword ptr [ebp - 0x198], ecx
0x004466f2: 8b9584feffff             mov edx, dword ptr [ebp - 0x17c]
0x004466f8: 89956cfeffff             mov dword ptr [ebp - 0x194], edx
0x004466fe: 8b8588feffff             mov eax, dword ptr [ebp - 0x178]
0x00446704: 898570feffff             mov dword ptr [ebp - 0x190], eax
0x0044670a: 8d8d98feffff             lea ecx, [ebp - 0x168]
0x00446710: 51                       push ecx
0x00446711: 8d9574feffff             lea edx, [ebp - 0x18c]
0x00446717: 52                       push edx
0x00446718: e8c3580100               call 0x45bfe0
0x0044671d: 50                       push eax
0x0044671e: e8ade50600               call 0x4b4cd0
0x00446723: 83c40c                   add esp, 0xc
0x00446726: 89855cfeffff             mov dword ptr [ebp - 0x1a4], eax
0x0044672c: 83bd5cfeffff00           cmp dword ptr [ebp - 0x1a4], 0
0x00446733: 7452                     je 0x446787
0x00446735: c78558feffff00000000     mov dword ptr [ebp - 0x1a8], 0
0x0044673f: 8d85d8feffff             lea eax, [ebp - 0x128]
0x00446745: 50                       push eax
0x00446746: 8d8d98feffff             lea ecx, [ebp - 0x168]
0x0044674c: 51                       push ecx
0x0044674d: e8fe5b0100               call 0x45c350
0x00446752: 83c408                   add esp, 8
0x00446755: 83f801                   cmp eax, 1
0x00446758: 750a                     jne 0x446764
0x0044675a: c7855cfeffff00000000     mov dword ptr [ebp - 0x1a4], 0
0x00446764: d985d0feffff             fld dword ptr [ebp - 0x130]
0x0044676a: d88d60feffff             fmul dword ptr [ebp - 0x1a0]
0x00446770: d99558feffff             fst dword ptr [ebp - 0x1a8]
0x00446776: d8256cc55c00             fsub dword ptr [0x5cc56c]
0x0044677c: 8b5508                   mov edx, dword ptr [ebp + 8]
0x0044677f: d99aa4090000             fstp dword ptr [edx + 0x9a4]
0x00446785: eb62                     jmp 0x4467e9
0x00446787: 8b4508                   mov eax, dword ptr [ebp + 8]
0x0044678a: d980a4090000             fld dword ptr [eax + 0x9a4]
0x00446790: d81d88d05c00             fcomp dword ptr [0x5cd088]
0x00446796: dfe0                     fnstsw ax
0x00446798: f6c441                   test ah, 0x41
0x0044679b: 7a0d                     jp 0x4467aa
0x0044679d: 8b4d08                   mov ecx, dword ptr [ebp + 8]
0x004467a0: c781a409000000002040     mov dword ptr [ecx + 0x9a4], 0x40200000
0x004467aa: 8b5508                   mov edx, dword ptr [ebp + 8]
0x004467ad: d982a4090000             fld dword ptr [edx + 0x9a4]
0x004467b3: d89d60feffff             fcomp dword ptr [ebp - 0x1a0]
0x004467b9: dfe0                     fnstsw ax
0x004467bb: f6c405                   test ah, 5
0x004467be: 7a1a                     jp 0x4467da
0x004467c0: 8b4508                   mov eax, dword ptr [ebp + 8]
0x004467c3: d980a4090000             fld dword ptr [eax + 0x9a4]
0x004467c9: d80dd4e15c00             fmul dword ptr [0x5ce1d4]
0x004467cf: 8b4d08                   mov ecx, dword ptr [ebp + 8]
0x004467d2: d999a4090000             fstp dword ptr [ecx + 0x9a4]
0x004467d8: eb0f                     jmp 0x4467e9
0x004467da: 8b5508                   mov edx, dword ptr [ebp + 8]
0x004467dd: 8b8560feffff             mov eax, dword ptr [ebp - 0x1a0]
0x004467e3: 8982a4090000             mov dword ptr [edx + 0x9a4], eax
0x004467e9: 8b8dd8feffff             mov ecx, dword ptr [ebp - 0x128]
0x004467ef: 898d68feffff             mov dword ptr [ebp - 0x198], ecx
0x004467f5: 8b95dcfeffff             mov edx, dword ptr [ebp - 0x124]
0x004467fb: 89956cfeffff             mov dword ptr [ebp - 0x194], edx
0x00446801: 8b85e0feffff             mov eax, dword ptr [ebp - 0x120]
0x00446807: 898570feffff             mov dword ptr [ebp - 0x190], eax
0x0044680d: 8b4d08                   mov ecx, dword ptr [ebp + 8]
0x00446810: d981a4090000             fld dword ptr [ecx + 0x9a4]
0x00446816: d88de4feffff             fmul dword ptr [ebp - 0x11c]
0x0044681c: d88568feffff             fadd dword ptr [ebp - 0x198]
0x00446822: d99d68feffff             fstp dword ptr [ebp - 0x198]
0x00446828: 8b5508                   mov edx, dword ptr [ebp + 8]
0x0044682b: d982a4090000             fld dword ptr [edx + 0x9a4]
0x00446831: d88de8feffff             fmul dword ptr [ebp - 0x118]
0x00446837: d8856cfeffff             fadd dword ptr [ebp - 0x194]
0x0044683d: d99d6cfeffff             fstp dword ptr [ebp - 0x194]
0x00446843: 8b4508                   mov eax, dword ptr [ebp + 8]
0x00446846: d980a4090000             fld dword ptr [eax + 0x9a4]
0x0044684c: d88decfeffff             fmul dword ptr [ebp - 0x114]
0x00446852: d88570feffff             fadd dword ptr [ebp - 0x190]
0x00446858: d99d70feffff             fstp dword ptr [ebp - 0x190]
0x0044685e: 33c9                     xor ecx, ecx
0x00446860: 85c9                     test ecx, ecx
0x00446862: 75a9                     jne 0x44680d
0x00446864: 6a00                     push 0
0x00446866: 8d9568feffff             lea edx, [ebp - 0x198]
0x0044686c: 52                       push edx
0x0044686d: 8b8564feffff             mov eax, dword ptr [ebp - 0x19c]
0x00446873: 50                       push eax
0x00446874: e827e90700               call 0x4c51a0
0x00446879: 83c40c                   add esp, 0xc
0x0044687c: db05f00f7f00             fild dword ptr [0x7f0ff0]
0x00446882: d80d3cd05c00             fmul dword ptr [0x5cd03c]
0x00446888: e873ce0500               call 0x4a3700
0x0044688d: d99d8cfeffff             fstp dword ptr [ebp - 0x174]
0x00446893: db05f00f7f00             fild dword ptr [0x7f0ff0]
0x00446899: d80d3cd05c00             fmul dword ptr [0x5cd03c]
0x0044689f: e80ccf0500               call 0x4a37b0
0x004468a4: d99d94feffff             fstp dword ptr [ebp - 0x16c]
0x004468aa: 8d8d8cfeffff             lea ecx, [ebp - 0x174]
0x004468b0: 51                       push ecx
0x004468b1: 8d95d8feffff             lea edx, [ebp - 0x128]
0x004468b7: 52                       push edx
0x004468b8: 8b8564feffff             mov eax, dword ptr [ebp - 0x19c]
0x004468be: 50                       push eax
0x004468bf: e86cdb0600               call 0x4b4430
0x004468c4: 83c40c                   add esp, 0xc
0x004468c7: 8b4d08                   mov ecx, dword ptr [ebp + 8]
0x004468ca: c74158cdcc4c3f           mov dword ptr [ecx + 0x58], 0x3f4ccccd
0x004468d1: 8b5508                   mov edx, dword ptr [ebp + 8]
0x004468d4: 52                       push edx
0x004468d5: e826aeffff               call 0x441700
0x004468da: 83c404                   add esp, 4
0x004468dd: e92e190000               jmp 0x448210
0x004468e2: 83bd34ffffff06           cmp dword ptr [ebp - 0xcc], 6
0x004468e9: 750a                     jne 0x4468f5
0x004468eb: e8b08dfeff               call 0x42f6a0
0x004468f0: 83f80b                   cmp eax, 0xb
0x004468f3: 7526                     jne 0x44691b
0x004468f5: 8b4508                   mov eax, dword ptr [ebp + 8]
0x004468f8: 50                       push eax
0x004468f9: e8c2fbffff               call 0x4464c0
0x004468fe: 83c404                   add esp, 4
0x00446901: 8b4d08                   mov ecx, dword ptr [ebp + 8]
0x00446904: 83791c00                 cmp dword ptr [ecx + 0x1c], 0
0x00446908: 7411                     je 0x44691b
0x0044690a: 8b5508                   mov edx, dword ptr [ebp + 8]
0x0044690d: 52                       push edx
0x0044690e: e8adbeffff               call 0x4427c0
0x00446913: 83c404                   add esp, 4
0x00446916: e9f5180000               jmp 0x448210
0x0044691b: c745ac00000000           mov dword ptr [ebp - 0x54], 0
0x00446922: eb09                     jmp 0x44692d
0x00446924: 8b45ac                   mov eax, dword ptr [ebp - 0x54]
0x00446927: 83c001                   add eax, 1
0x0044692a: 8945ac                   mov dword ptr [ebp - 0x54], eax
0x0044692d: 837dac04                 cmp dword ptr [ebp - 0x54], 4
0x00446931: 7d52                     jge 0x446985
0x00446933: c78554feffff00000000     mov dword ptr [ebp - 0x1ac], 0
0x0044693d: 8d8d54feffff             lea ecx, [ebp - 0x1ac]
0x00446943: 51                       push ecx
0x00446944: 6a00                     push 0
0x00446946: 8b55ac                   mov edx, dword ptr [ebp - 0x54]
0x00446949: 52                       push edx
0x0044694a: e8d187fdff               call 0x41f120
0x0044694f: 83c40c                   add esp, 0xc
0x00446952: d98554feffff             fld dword ptr [ebp - 0x1ac]
0x00446958: d81dd0e15c00             fcomp dword ptr [0x5ce1d0]
0x0044695e: dfe0                     fnstsw ax
0x00446960: f6c405                   test ah, 5
0x00446963: 7a10                     jp 0x446975
0x00446965: 6a01                     push 1
0x00446967: 8b45ac                   mov eax, dword ptr [ebp - 0x54]
0x0044696a: 50                       push eax
0x0044696b: e8f085fdff               call 0x41ef60
0x00446970: 83c408                   add esp, 8
0x00446973: eb0e                     jmp 0x446983
0x00446975: 6a02                     push 2
0x00446977: 8b4dac                   mov ecx, dword ptr [ebp - 0x54]
0x0044697a: 51                       push ecx
0x0044697b: e8e085fdff               call 0x41ef60
0x00446980: 83c408                   add esp, 8
0x00446983: eb9f                     jmp 0x446924
0x00446985: e8168dfeff               call 0x42f6a0
0x0044698a: 83f802                   cmp eax, 2
0x0044698d: 741b                     je 0x4469aa
0x0044698f: 833dd00f7f0005           cmp dword ptr [0x7f0fd0], 5
0x00446996: 7412                     je 0x4469aa
0x00446998: 833dd00f7f000a           cmp dword ptr [0x7f0fd0], 0xa
0x0044699f: 7409                     je 0x4469aa
0x004469a1: 833dd00f7f0009           cmp dword ptr [0x7f0fd0], 9
0x004469a8: 750c                     jne 0x4469b6
0x004469aa: 6a00                     push 0
0x004469ac: 6a00                     push 0
0x004469ae: e8ad85fdff               call 0x41ef60
0x004469b3: 83c408                   add esp, 8
0x004469b6: e8a5c0ffff               call 0x442a60
0x004469bb: 8d559c                   lea edx, [ebp - 0x64]
0x004469be: 52                       push edx
0x004469bf: 8d45d8                   lea eax, [ebp - 0x28]
0x004469c2: 50                       push eax
0x004469c3: e8b877fcff               call 0x40e180
0x004469c8: 83c408                   add esp, 8
0x004469cb: 833dd00f7f0007           cmp dword ptr [0x7f0fd0], 7
0x004469d2: 0f852e010000             jne 0x446b06
0x004469d8: c7854cfeffff00000000     mov dword ptr [ebp - 0x1b4], 0
0x004469e2: c78540feffff00000000     mov dword ptr [ebp - 0x1c0], 0
0x004469ec: c78550feffff000080bf     mov dword ptr [ebp - 0x1b0], 0xbf800000
0x004469f6: c78548feffff000080bf     mov dword ptr [ebp - 0x1b8], 0xbf800000
0x00446a00: c78544feffff8fc2f53c     mov dword ptr [ebp - 0x1bc], 0x3cf5c28f
0x00446a0a: c745ac00000000           mov dword ptr [ebp - 0x54], 0
0x00446a11: 837dac04                 cmp dword ptr [ebp - 0x54], 4
0x00446a15: 7d3f                     jge 0x446a56
0x00446a17: 8b4dac                   mov ecx, dword ptr [ebp - 0x54]
0x00446a1a: 51                       push ecx
0x00446a1b: e8100dfdff               call 0x417730
0x00446a20: 83c404                   add esp, 4
0x00446a23: d9953cfeffff             fst dword ptr [ebp - 0x1c4]
0x00446a29: d89d50feffff             fcomp dword ptr [ebp - 0x1b0]
0x00446a2f: dfe0                     fnstsw ax
0x00446a31: f6c441                   test ah, 0x41
0x00446a34: 7515                     jne 0x446a4b
0x00446a36: 8b55ac                   mov edx, dword ptr [ebp - 0x54]
0x00446a39: 89954cfeffff             mov dword ptr [ebp - 0x1b4], edx
0x00446a3f: 8b853cfeffff             mov eax, dword ptr [ebp - 0x1c4]
0x00446a45: 898550feffff             mov dword ptr [ebp - 0x1b0], eax
0x00446a4b: 8b4dac                   mov ecx, dword ptr [ebp - 0x54]
0x00446a4e: 83c101                   add ecx, 1
0x00446a51: 894dac                   mov dword ptr [ebp - 0x54], ecx
0x00446a54: ebbb                     jmp 0x446a11
0x00446a56: 8b954cfeffff             mov edx, dword ptr [ebp - 0x1b4]
0x00446a5c: 899540feffff             mov dword ptr [ebp - 0x1c0], edx
0x00446a62: c745ac00000000           mov dword ptr [ebp - 0x54], 0
0x00446a69: 837dac04                 cmp dword ptr [ebp - 0x54], 4
0x00446a6d: 0f8d81000000             jge 0x446af4
0x00446a73: 8b45ac                   mov eax, dword ptr [ebp - 0x54]
0x00446a76: 3b854cfeffff             cmp eax, dword ptr [ebp - 0x1b4]
0x00446a7c: 7468                     je 0x446ae6
0x00446a7e: 6a00                     push 0
0x00446a80: e82b5d0200               call 0x46c7b0
0x00446a85: 83c404                   add esp, 4
0x00446a88: 83f801                   cmp eax, 1
0x00446a8b: 7559                     jne 0x446ae6
0x00446a8d: 8b4dac                   mov ecx, dword ptr [ebp - 0x54]
0x00446a90: 51                       push ecx
0x00446a91: e89a0cfdff               call 0x417730
0x00446a96: 83c404                   add esp, 4
0x00446a99: d99d34feffff             fstp dword ptr [ebp - 0x1cc]
0x00446a9f: d98550feffff             fld dword ptr [ebp - 0x1b0]
0x00446aa5: d8a534feffff             fsub dword ptr [ebp - 0x1cc]
0x00446aab: d99538feffff             fst dword ptr [ebp - 0x1c8]
0x00446ab1: d89d44feffff             fcomp dword ptr [ebp - 0x1bc]
0x00446ab7: dfe0                     fnstsw ax
0x00446ab9: f6c405                   test ah, 5
0x00446abc: 7a28                     jp 0x446ae6
0x00446abe: d98538feffff             fld dword ptr [ebp - 0x1c8]
0x00446ac4: d89d48feffff             fcomp dword ptr [ebp - 0x1b8]
0x00446aca: dfe0                     fnstsw ax
0x00446acc: f6c441                   test ah, 0x41
0x00446acf: 7515                     jne 0x446ae6
0x00446ad1: 8b9538feffff             mov edx, dword ptr [ebp - 0x1c8]
0x00446ad7: 899548feffff             mov dword ptr [ebp - 0x1b8], edx
0x00446add: 8b45ac                   mov eax, dword ptr [ebp - 0x54]
0x00446ae0: 898540feffff             mov dword ptr [ebp - 0x1c0], eax
0x00446ae6: 8b4dac                   mov ecx, dword ptr [ebp - 0x54]
0x00446ae9: 83c101                   add ecx, 1
0x00446aec: 894dac                   mov dword ptr [ebp - 0x54], ecx
0x00446aef: e975ffffff               jmp 0x446a69
0x00446af4: 8b954cfeffff             mov edx, dword ptr [ebp - 0x1b4]
0x00446afa: 8955d8                   mov dword ptr [ebp - 0x28], edx
0x00446afd: 8b8540feffff             mov eax, dword ptr [ebp - 0x1c0]
0x00446b03: 89459c                   mov dword ptr [ebp - 0x64], eax
0x00446b06: 833dd00f7f0004           cmp dword ptr [0x7f0fd0], 4
0x00446b0d: 750d                     jne 0x446b1c
0x00446b0f: c7459c00000000           mov dword ptr [ebp - 0x64], 0
0x00446b16: 8b4d9c                   mov ecx, dword ptr [ebp - 0x64]
0x00446b19: 894dd8                   mov dword ptr [ebp - 0x28], ecx
0x00446b1c: 833dd00f7f0009           cmp dword ptr [0x7f0fd0], 9
0x00446b23: 750d                     jne 0x446b32
0x00446b25: c7459c00000000           mov dword ptr [ebp - 0x64], 0
0x00446b2c: 8b559c                   mov edx, dword ptr [ebp - 0x64]
0x00446b2f: 8955d8                   mov dword ptr [ebp - 0x28], edx
0x00446b32: 833dd00f7f0008           cmp dword ptr [0x7f0fd0], 8
0x00446b39: 750d                     jne 0x446b48
0x00446b3b: c7459c00000000           mov dword ptr [ebp - 0x64], 0
0x00446b42: 8b459c                   mov eax, dword ptr [ebp - 0x64]
0x00446b45: 8945d8                   mov dword ptr [ebp - 0x28], eax
0x00446b48: 8b4dd8                   mov ecx, dword ptr [ebp - 0x28]
0x00446b4b: 51                       push ecx
0x00446b4c: 8d55a0                   lea edx, [ebp - 0x60]
0x00446b4f: 52                       push edx
0x00446b50: e84b690200               call 0x46d4a0
0x00446b55: 83c408                   add esp, 8
0x00446b58: 8b45a0                   mov eax, dword ptr [ebp - 0x60]
0x00446b5b: 8b4830                   mov ecx, dword ptr [eax + 0x30]
0x00446b5e: 894dc4                   mov dword ptr [ebp - 0x3c], ecx
0x00446b61: 8b55a0                   mov edx, dword ptr [ebp - 0x60]
0x00446b64: 8b4234                   mov eax, dword ptr [edx + 0x34]
0x00446b67: 8945c8                   mov dword ptr [ebp - 0x38], eax
0x00446b6a: 8b4da0                   mov ecx, dword ptr [ebp - 0x60]
0x00446b6d: 8b5138                   mov edx, dword ptr [ecx + 0x38]
0x00446b70: 899544ffffff             mov dword ptr [ebp - 0xbc], edx
0x00446b76: 8b45d8                   mov eax, dword ptr [ebp - 0x28]
0x00446b79: 50                       push eax
0x00446b7a: 8d4de0                   lea ecx, [ebp - 0x20]
0x00446b7d: 51                       push ecx
0x00446b7e: e8ad5f0200               call 0x46cb30
0x00446b83: 83c408                   add esp, 8
0x00446b86: d945e0                   fld dword ptr [ebp - 0x20]
0x00446b89: d80d18cd5c00             fmul dword ptr [0x5ccd18]
0x00446b8f: d95de0                   fstp dword ptr [ebp - 0x20]
0x00446b92: d945e4                   fld dword ptr [ebp - 0x1c]
0x00446b95: d80d18cd5c00             fmul dword ptr [0x5ccd18]
0x00446b9b: d95de4                   fstp dword ptr [ebp - 0x1c]
0x00446b9e: d945e8                   fld dword ptr [ebp - 0x18]
0x00446ba1: d80d18cd5c00             fmul dword ptr [0x5ccd18]
0x00446ba7: d95de8                   fstp dword ptr [ebp - 0x18]
0x00446baa: 33d2                     xor edx, edx
0x00446bac: 85d2                     test edx, edx
0x00446bae: 75d6                     jne 0x446b86
0x00446bb0: d945c4                   fld dword ptr [ebp - 0x3c]
0x00446bb3: d845e0                   fadd dword ptr [ebp - 0x20]
0x00446bb6: d95dc4                   fstp dword ptr [ebp - 0x3c]
0x00446bb9: d98544ffffff             fld dword ptr [ebp - 0xbc]
0x00446bbf: d845e8                   fadd dword ptr [ebp - 0x18]
0x00446bc2: d99d44ffffff             fstp dword ptr [ebp - 0xbc]
0x00446bc8: 8b459c                   mov eax, dword ptr [ebp - 0x64]
0x00446bcb: 50                       push eax
0x00446bcc: 8d4da0                   lea ecx, [ebp - 0x60]
0x00446bcf: 51                       push ecx
0x00446bd0: e8cb680200               call 0x46d4a0
0x00446bd5: 83c408                   add esp, 8
0x00446bd8: 8b55a0                   mov edx, dword ptr [ebp - 0x60]
0x00446bdb: d945c4                   fld dword ptr [ebp - 0x3c]
0x00446bde: d86230                   fsub dword ptr [edx + 0x30]
0x00446be1: d95de0                   fstp dword ptr [ebp - 0x20]
0x00446be4: 8b45a0                   mov eax, dword ptr [ebp - 0x60]
0x00446be7: d945c8                   fld dword ptr [ebp - 0x38]
0x00446bea: d86034                   fsub dword ptr [eax + 0x34]
0x00446bed: d95de4                   fstp dword ptr [ebp - 0x1c]
0x00446bf0: 8b4da0                   mov ecx, dword ptr [ebp - 0x60]
0x00446bf3: d98544ffffff             fld dword ptr [ebp - 0xbc]
0x00446bf9: d86138                   fsub dword ptr [ecx + 0x38]
0x00446bfc: d95de8                   fstp dword ptr [ebp - 0x18]
0x00446bff: 8b55d8                   mov edx, dword ptr [ebp - 0x28]
0x00446c02: 52                       push edx
0x00446c03: 8d45b0                   lea eax, [ebp - 0x50]
0x00446c06: 50                       push eax
0x00446c07: e8245f0200               call 0x46cb30
0x00446c0c: 83c408                   add esp, 8
0x00446c0f: d945b0                   fld dword ptr [ebp - 0x50]
0x00446c12: d80d18cd5c00             fmul dword ptr [0x5ccd18]
0x00446c18: d95db0                   fstp dword ptr [ebp - 0x50]
0x00446c1b: d945b4                   fld dword ptr [ebp - 0x4c]
0x00446c1e: d80d18cd5c00             fmul dword ptr [0x5ccd18]
0x00446c24: d95db4                   fstp dword ptr [ebp - 0x4c]
0x00446c27: d945b8                   fld dword ptr [ebp - 0x48]
0x00446c2a: d80d18cd5c00             fmul dword ptr [0x5ccd18]
0x00446c30: d95db8                   fstp dword ptr [ebp - 0x48]
0x00446c33: 33c9                     xor ecx, ecx
0x00446c35: 85c9                     test ecx, ecx
0x00446c37: 75d6                     jne 0x446c0f
0x00446c39: d945e0                   fld dword ptr [ebp - 0x20]
0x00446c3c: d865b0                   fsub dword ptr [ebp - 0x50]
0x00446c3f: d95de0                   fstp dword ptr [ebp - 0x20]
0x00446c42: d945e8                   fld dword ptr [ebp - 0x18]
0x00446c45: d865b8                   fsub dword ptr [ebp - 0x48]
0x00446c48: d95de8                   fstp dword ptr [ebp - 0x18]
0x00446c4b: 8b55e0                   mov edx, dword ptr [ebp - 0x20]
0x00446c4e: 899538ffffff             mov dword ptr [ebp - 0xc8], edx
0x00446c54: c7853cffffff00000000     mov dword ptr [ebp - 0xc4], 0
0x00446c5e: 8b45e8                   mov eax, dword ptr [ebp - 0x18]
0x00446c61: 898540ffffff             mov dword ptr [ebp - 0xc0], eax
0x00446c67: 8d8d38ffffff             lea ecx, [ebp - 0xc8]
0x00446c6d: 51                       push ecx
0x00446c6e: e84dce0700               call 0x4c3ac0
0x00446c73: 83c404                   add esp, 4
0x00446c76: d80dbcc95c00             fmul dword ptr [0x5cc9bc]
0x00446c7c: d99d2cffffff             fstp dword ptr [ebp - 0xd4]
0x00446c82: 8b952cffffff             mov edx, dword ptr [ebp - 0xd4]
0x00446c88: 8955d4                   mov dword ptr [ebp - 0x2c], edx
0x00446c8b: 8b45d8                   mov eax, dword ptr [ebp - 0x28]
0x00446c8e: 3b459c                   cmp eax, dword ptr [ebp - 0x64]
0x00446c91: 7546                     jne 0x446cd9
0x00446c93: d9852cffffff             fld dword ptr [ebp - 0xd4]
0x00446c99: d81d20c35c00             fcomp dword ptr [0x5cc320]
0x00446c9f: dfe0                     fnstsw ax
0x00446ca1: f6c405                   test ah, 5
0x00446ca4: 7a33                     jp 0x446cd9
0x00446ca6: 8d4da8                   lea ecx, [ebp - 0x58]
0x00446ca9: 51                       push ecx
0x00446caa: 8d9524ffffff             lea edx, [ebp - 0xdc]
0x00446cb0: 52                       push edx
0x00446cb1: 8b45d8                   mov eax, dword ptr [ebp - 0x28]
0x00446cb4: 50                       push eax
0x00446cb5: e8f65e0200               call 0x46cbb0
0x00446cba: 83c40c                   add esp, 0xc
0x00446cbd: 83bd24ffffff00           cmp dword ptr [ebp - 0xdc], 0
0x00446cc4: 7413                     je 0x446cd9
0x00446cc6: c7852cffffff00000041     mov dword ptr [ebp - 0xd4], 0x41000000
0x00446cd0: 8b8d2cffffff             mov ecx, dword ptr [ebp - 0xd4]
0x00446cd6: 894dd4                   mov dword ptr [ebp - 0x2c], ecx
0x00446cd9: d945e0                   fld dword ptr [ebp - 0x20]
0x00446cdc: d83574c55c00             fdiv dword ptr [0x5cc574]
0x00446ce2: d86dc4                   fsubr dword ptr [ebp - 0x3c]
0x00446ce5: d95dc4                   fstp dword ptr [ebp - 0x3c]
0x00446ce8: 8b55c4                   mov edx, dword ptr [ebp - 0x3c]
0x00446ceb: 8955bc                   mov dword ptr [ebp - 0x44], edx
0x00446cee: d945e4                   fld dword ptr [ebp - 0x1c]
0x00446cf1: d83574c55c00             fdiv dword ptr [0x5cc574]
0x00446cf7: d86dc8                   fsubr dword ptr [ebp - 0x38]
0x00446cfa: d95dc8                   fstp dword ptr [ebp - 0x38]
0x00446cfd: 8b45c8                   mov eax, dword ptr [ebp - 0x38]
0x00446d00: 898508ffffff             mov dword ptr [ebp - 0xf8], eax
0x00446d06: d945e8                   fld dword ptr [ebp - 0x18]
0x00446d09: d83574c55c00             fdiv dword ptr [0x5cc574]
0x00446d0f: d8ad44ffffff             fsubr dword ptr [ebp - 0xbc]
0x00446d15: d99d44ffffff             fstp dword ptr [ebp - 0xbc]
0x00446d1b: 8b8d44ffffff             mov ecx, dword ptr [ebp - 0xbc]
0x00446d21: 898d20ffffff             mov dword ptr [ebp - 0xe0], ecx
0x00446d27: c78530ffffff00000000     mov dword ptr [ebp - 0xd0], 0
0x00446d31: 8b5508                   mov edx, dword ptr [ebp + 8]
0x00446d34: 8b45d8                   mov eax, dword ptr [ebp - 0x28]
0x00446d37: 3b8294090000             cmp eax, dword ptr [edx + 0x994]
0x00446d3d: 7518                     jne 0x446d57
0x00446d3f: 8b4d08                   mov ecx, dword ptr [ebp + 8]
0x00446d42: 8b559c                   mov edx, dword ptr [ebp - 0x64]
0x00446d45: 3b9198090000             cmp edx, dword ptr [ecx + 0x998]
0x00446d4b: 750a                     jne 0x446d57
0x00446d4d: c78530ffffff01000000     mov dword ptr [ebp - 0xd0], 1
0x00446d57: 8b4508                   mov eax, dword ptr [ebp + 8]
0x00446d5a: 8b4dd8                   mov ecx, dword ptr [ebp - 0x28]
0x00446d5d: 3b8898090000             cmp ecx, dword ptr [eax + 0x998]
0x00446d63: 7518                     jne 0x446d7d
0x00446d65: 8b5508                   mov edx, dword ptr [ebp + 8]
0x00446d68: 8b459c                   mov eax, dword ptr [ebp - 0x64]
0x00446d6b: 3b8294090000             cmp eax, dword ptr [edx + 0x994]
0x00446d71: 750a                     jne 0x446d7d
0x00446d73: c78530ffffff01000000     mov dword ptr [ebp - 0xd0], 1
0x00446d7d: 837d0c00                 cmp dword ptr [ebp + 0xc], 0
0x00446d81: 740a                     je 0x446d8d
0x00446d83: c78530ffffff00000000     mov dword ptr [ebp - 0xd0], 0
0x00446d8d: 83bd30ffffff00           cmp dword ptr [ebp - 0xd0], 0
0x00446d94: 0f85ea010000             jne 0x446f84
0x00446d9a: d9050c107f00             fld dword ptr [0x7f100c]
0x00446da0: dcc0                     fadd st(0), st(0)
0x00446da2: 8b4d08                   mov ecx, dword ptr [ebp + 8]
0x00446da5: d8819c090000             fadd dword ptr [ecx + 0x99c]
0x00446dab: 8b5508                   mov edx, dword ptr [ebp + 8]
0x00446dae: d99a9c090000             fstp dword ptr [edx + 0x99c]
0x00446db4: 8b4508                   mov eax, dword ptr [ebp + 8]
0x00446db7: d9809c090000             fld dword ptr [eax + 0x99c]
0x00446dbd: d81d20c35c00             fcomp dword ptr [0x5cc320]
0x00446dc3: dfe0                     fnstsw ax
0x00446dc5: f6c401                   test ah, 1
0x00446dc8: 751d                     jne 0x446de7
0x00446dca: 8b4d08                   mov ecx, dword ptr [ebp + 8]
0x00446dcd: 8b55d8                   mov edx, dword ptr [ebp - 0x28]
0x00446dd0: 899194090000             mov dword ptr [ecx + 0x994], edx
0x00446dd6: 8b4508                   mov eax, dword ptr [ebp + 8]
0x00446dd9: 8b4d9c                   mov ecx, dword ptr [ebp - 0x64]
0x00446ddc: 898898090000             mov dword ptr [eax + 0x998], ecx
0x00446de2: e99b010000               jmp 0x446f82
0x00446de7: 8b5508                   mov edx, dword ptr [ebp + 8]
0x00446dea: 8b8294090000             mov eax, dword ptr [edx + 0x994]
0x00446df0: 50                       push eax
0x00446df1: 8d4da0                   lea ecx, [ebp - 0x60]
0x00446df4: 51                       push ecx
0x00446df5: e8a6660200               call 0x46d4a0
0x00446dfa: 83c408                   add esp, 8
0x00446dfd: 8b55a0                   mov edx, dword ptr [ebp - 0x60]
0x00446e00: 8b4230                   mov eax, dword ptr [edx + 0x30]
0x00446e03: 8945cc                   mov dword ptr [ebp - 0x34], eax
0x00446e06: 8b4da0                   mov ecx, dword ptr [ebp - 0x60]
0x00446e09: 8b5134                   mov edx, dword ptr [ecx + 0x34]
0x00446e0c: 8955d0                   mov dword ptr [ebp - 0x30], edx
0x00446e0f: 8b45a0                   mov eax, dword ptr [ebp - 0x60]
0x00446e12: 8b4838                   mov ecx, dword ptr [eax + 0x38]
0x00446e15: 898d54ffffff             mov dword ptr [ebp - 0xac], ecx
0x00446e1b: 8b5508                   mov edx, dword ptr [ebp + 8]
0x00446e1e: 8b8298090000             mov eax, dword ptr [edx + 0x998]
0x00446e24: 50                       push eax
0x00446e25: 8d4da0                   lea ecx, [ebp - 0x60]
0x00446e28: 51                       push ecx
0x00446e29: e872660200               call 0x46d4a0
0x00446e2e: 83c408                   add esp, 8
0x00446e31: 8b55a0                   mov edx, dword ptr [ebp - 0x60]
0x00446e34: d945cc                   fld dword ptr [ebp - 0x34]
0x00446e37: d86230                   fsub dword ptr [edx + 0x30]
0x00446e3a: d95de0                   fstp dword ptr [ebp - 0x20]
0x00446e3d: 8b45a0                   mov eax, dword ptr [ebp - 0x60]
0x00446e40: d945d0                   fld dword ptr [ebp - 0x30]
0x00446e43: d86034                   fsub dword ptr [eax + 0x34]
0x00446e46: d95de4                   fstp dword ptr [ebp - 0x1c]
0x00446e49: 8b4da0                   mov ecx, dword ptr [ebp - 0x60]
0x00446e4c: d98554ffffff             fld dword ptr [ebp - 0xac]
0x00446e52: d86138                   fsub dword ptr [ecx + 0x38]
0x00446e55: d95de8                   fstp dword ptr [ebp - 0x18]
0x00446e58: 8b55e0                   mov edx, dword ptr [ebp - 0x20]
0x00446e5b: 899538ffffff             mov dword ptr [ebp - 0xc8], edx
0x00446e61: c7853cffffff00000000     mov dword ptr [ebp - 0xc4], 0
0x00446e6b: 8b45e8                   mov eax, dword ptr [ebp - 0x18]
0x00446e6e: 898540ffffff             mov dword ptr [ebp - 0xc0], eax
0x00446e74: 8d8d38ffffff             lea ecx, [ebp - 0xc8]
0x00446e7a: 51                       push ecx
0x00446e7b: e840cc0700               call 0x4c3ac0
0x00446e80: 83c404                   add esp, 4
0x00446e83: d80dbcc95c00             fmul dword ptr [0x5cc9bc]
0x00446e89: d99d1cffffff             fstp dword ptr [ebp - 0xe4]
0x00446e8f: d945e0                   fld dword ptr [ebp - 0x20]
0x00446e92: d83574c55c00             fdiv dword ptr [0x5cc574]
0x00446e98: d86dcc                   fsubr dword ptr [ebp - 0x34]
0x00446e9b: d95dcc                   fstp dword ptr [ebp - 0x34]
0x00446e9e: d945e4                   fld dword ptr [ebp - 0x1c]
0x00446ea1: d83574c55c00             fdiv dword ptr [0x5cc574]
0x00446ea7: d86dd0                   fsubr dword ptr [ebp - 0x30]
0x00446eaa: d95dd0                   fstp dword ptr [ebp - 0x30]
0x00446ead: d945e8                   fld dword ptr [ebp - 0x18]
0x00446eb0: d83574c55c00             fdiv dword ptr [0x5cc574]
0x00446eb6: d8ad54ffffff             fsubr dword ptr [ebp - 0xac]
0x00446ebc: d99d54ffffff             fstp dword ptr [ebp - 0xac]
0x00446ec2: 8b5508                   mov edx, dword ptr [ebp + 8]
0x00446ec5: 8b829c090000             mov eax, dword ptr [edx + 0x99c]
0x00446ecb: 89854cffffff             mov dword ptr [ebp - 0xb4], eax
0x00446ed1: d9854cffffff             fld dword ptr [ebp - 0xb4]
0x00446ed7: dcc0                     fadd st(0), st(0)
0x00446ed9: d9954cffffff             fst dword ptr [ebp - 0xb4]
0x00446edf: d82520c35c00             fsub dword ptr [0x5cc320]
0x00446ee5: d9954cffffff             fst dword ptr [ebp - 0xb4]
0x00446eeb: d81d7c755d00             fcomp dword ptr [0x5d757c]
0x00446ef1: dfe0                     fnstsw ax
0x00446ef3: f6c405                   test ah, 5
0x00446ef6: 7a0a                     jp 0x446f02
0x00446ef8: c7854cffffff00000000     mov dword ptr [ebp - 0xb4], 0
0x00446f02: d90520c35c00             fld dword ptr [0x5cc320]
0x00446f08: d8a54cffffff             fsub dword ptr [ebp - 0xb4]
0x00446f0e: d99d28ffffff             fstp dword ptr [ebp - 0xd8]
0x00446f14: d945c4                   fld dword ptr [ebp - 0x3c]
0x00446f17: d88d4cffffff             fmul dword ptr [ebp - 0xb4]
0x00446f1d: d945cc                   fld dword ptr [ebp - 0x34]
0x00446f20: d88d28ffffff             fmul dword ptr [ebp - 0xd8]
0x00446f26: dec1                     faddp st(1)
0x00446f28: d95dc4                   fstp dword ptr [ebp - 0x3c]
0x00446f2b: d945c8                   fld dword ptr [ebp - 0x38]
0x00446f2e: d88d4cffffff             fmul dword ptr [ebp - 0xb4]
0x00446f34: d945d0                   fld dword ptr [ebp - 0x30]
0x00446f37: d88d28ffffff             fmul dword ptr [ebp - 0xd8]
0x00446f3d: dec1                     faddp st(1)
0x00446f3f: d95dc8                   fstp dword ptr [ebp - 0x38]
0x00446f42: d98544ffffff             fld dword ptr [ebp - 0xbc]
0x00446f48: d88d4cffffff             fmul dword ptr [ebp - 0xb4]
0x00446f4e: d98554ffffff             fld dword ptr [ebp - 0xac]
0x00446f54: d88d28ffffff             fmul dword ptr [ebp - 0xd8]
0x00446f5a: dec1                     faddp st(1)
0x00446f5c: d99d44ffffff             fstp dword ptr [ebp - 0xbc]
0x00446f62: d9852cffffff             fld dword ptr [ebp - 0xd4]
0x00446f68: d88d4cffffff             fmul dword ptr [ebp - 0xb4]
0x00446f6e: d9851cffffff             fld dword ptr [ebp - 0xe4]
0x00446f74: d88d28ffffff             fmul dword ptr [ebp - 0xd8]
0x00446f7a: dec1                     faddp st(1)
0x00446f7c: d99d2cffffff             fstp dword ptr [ebp - 0xd4]
0x00446f82: eb0d                     jmp 0x446f91
0x00446f84: 8b4d08                   mov ecx, dword ptr [ebp + 8]
0x00446f87: c7819c09000000000000     mov dword ptr [ecx + 0x99c], 0
0x00446f91: 8b55d8                   mov edx, dword ptr [ebp - 0x28]
0x00446f94: 52                       push edx
0x00446f95: e8b61afcff               call 0x408a50
0x00446f9a: 83c404                   add esp, 4
0x00446f9d: d99550ffffff             fst dword ptr [ebp - 0xb0]
0x00446fa3: e8a0bc0500               call 0x4a2c48
0x00446fa8: 8945ac                   mov dword ptr [ebp - 0x54], eax
0x00446fab: 8d45ec                   lea eax, [ebp - 0x14]
0x00446fae: 50                       push eax
0x00446faf: 8d4de0                   lea ecx, [ebp - 0x20]
0x00446fb2: 51                       push ecx
0x00446fb3: 8b55ac                   mov edx, dword ptr [ebp - 0x54]
0x00446fb6: 52                       push edx
0x00446fb7: e864a8ffff               call 0x441820
0x00446fbc: 83c40c                   add esp, 0xc
0x00446fbf: 8b45ac                   mov eax, dword ptr [ebp - 0x54]
0x00446fc2: 83c001                   add eax, 1
0x00446fc5: 8985f4feffff             mov dword ptr [ebp - 0x10c], eax
0x00446fcb: 6a00                     push 0
0x00446fcd: e8defbfdff               call 0x426bb0
0x00446fd2: 83c404                   add esp, 4
0x00446fd5: 3985f4feffff             cmp dword ptr [ebp - 0x10c], eax
0x00446fdb: 7c0a                     jl 0x446fe7
0x00446fdd: c785f4feffff00000000     mov dword ptr [ebp - 0x10c], 0
0x00446fe7: 8d4da4                   lea ecx, [ebp - 0x5c]
0x00446fea: 51                       push ecx
0x00446feb: 8d55b0                   lea edx, [ebp - 0x50]
0x00446fee: 52                       push edx
0x00446fef: 8b85f4feffff             mov eax, dword ptr [ebp - 0x10c]
0x00446ff5: 50                       push eax
0x00446ff6: e825a8ffff               call 0x441820
0x00446ffb: 83c40c                   add esp, 0xc
0x00446ffe: db45ac                   fild dword ptr [ebp - 0x54]
0x00447001: d8ad50ffffff             fsubr dword ptr [ebp - 0xb0]
0x00447007: d99d50ffffff             fstp dword ptr [ebp - 0xb0]
0x0044700d: d90520c35c00             fld dword ptr [0x5cc320]
0x00447013: d8a550ffffff             fsub dword ptr [ebp - 0xb0]
0x00447019: d99d4cffffff             fstp dword ptr [ebp - 0xb4]
0x0044701f: d945e0                   fld dword ptr [ebp - 0x20]
0x00447022: d88d4cffffff             fmul dword ptr [ebp - 0xb4]
0x00447028: d945b0                   fld dword ptr [ebp - 0x50]
0x0044702b: d88d50ffffff             fmul dword ptr [ebp - 0xb0]
0x00447031: dec1                     faddp st(1)
0x00447033: d99df8feffff             fstp dword ptr [ebp - 0x108]
0x00447039: d945e4                   fld dword ptr [ebp - 0x1c]
0x0044703c: d88d4cffffff             fmul dword ptr [ebp - 0xb4]
0x00447042: d945b4                   fld dword ptr [ebp - 0x4c]
0x00447045: d88d50ffffff             fmul dword ptr [ebp - 0xb0]
0x0044704b: dec1                     faddp st(1)
0x0044704d: d99dfcfeffff             fstp dword ptr [ebp - 0x104]
0x00447053: d945e8                   fld dword ptr [ebp - 0x18]
0x00447056: d88d4cffffff             fmul dword ptr [ebp - 0xb4]
0x0044705c: d945b8                   fld dword ptr [ebp - 0x48]
0x0044705f: d88d50ffffff             fmul dword ptr [ebp - 0xb0]
0x00447065: dec1                     faddp st(1)
0x00447067: d99d00ffffff             fstp dword ptr [ebp - 0x100]
0x0044706d: d945ec                   fld dword ptr [ebp - 0x14]
0x00447070: d88d4cffffff             fmul dword ptr [ebp - 0xb4]
0x00447076: d945a4                   fld dword ptr [ebp - 0x5c]
0x00447079: d88d50ffffff             fmul dword ptr [ebp - 0xb0]
0x0044707f: dec1                     faddp st(1)
0x00447081: d99d48ffffff             fstp dword ptr [ebp - 0xb8]
0x00447087: 8b4d9c                   mov ecx, dword ptr [ebp - 0x64]
0x0044708a: 51                       push ecx
0x0044708b: e8c019fcff               call 0x408a50
0x00447090: 83c404                   add esp, 4
0x00447093: d99550ffffff             fst dword ptr [ebp - 0xb0]
0x00447099: e8aabb0500               call 0x4a2c48
0x0044709e: 8945ac                   mov dword ptr [ebp - 0x54], eax
0x004470a1: 8d55ec                   lea edx, [ebp - 0x14]
0x004470a4: 52                       push edx
0x004470a5: 8d45e0                   lea eax, [ebp - 0x20]
0x004470a8: 50                       push eax
0x004470a9: 8b4dac                   mov ecx, dword ptr [ebp - 0x54]
0x004470ac: 51                       push ecx
0x004470ad: e86ea7ffff               call 0x441820
0x004470b2: 83c40c                   add esp, 0xc
0x004470b5: 8b55ac                   mov edx, dword ptr [ebp - 0x54]
0x004470b8: 83c201                   add edx, 1
0x004470bb: 8995f4feffff             mov dword ptr [ebp - 0x10c], edx
0x004470c1: 6a00                     push 0
0x004470c3: e8e8fafdff               call 0x426bb0
0x004470c8: 83c404                   add esp, 4
0x004470cb: 3985f4feffff             cmp dword ptr [ebp - 0x10c], eax
0x004470d1: 7c0a                     jl 0x4470dd
0x004470d3: c785f4feffff00000000     mov dword ptr [ebp - 0x10c], 0
0x004470dd: 8d45a4                   lea eax, [ebp - 0x5c]
0x004470e0: 50                       push eax
0x004470e1: 8d4db0                   lea ecx, [ebp - 0x50]
0x004470e4: 51                       push ecx
0x004470e5: 8b95f4feffff             mov edx, dword ptr [ebp - 0x10c]
0x004470eb: 52                       push edx
0x004470ec: e82fa7ffff               call 0x441820
0x004470f1: 83c40c                   add esp, 0xc
0x004470f4: db45ac                   fild dword ptr [ebp - 0x54]
0x004470f7: d8ad50ffffff             fsubr dword ptr [ebp - 0xb0]
0x004470fd: d99d50ffffff             fstp dword ptr [ebp - 0xb0]
0x00447103: d90520c35c00             fld dword ptr [0x5cc320]
0x00447109: d8a550ffffff             fsub dword ptr [ebp - 0xb0]
0x0044710f: d99d4cffffff             fstp dword ptr [ebp - 0xb4]
0x00447115: d945e0                   fld dword ptr [ebp - 0x20]
0x00447118: d88d4cffffff             fmul dword ptr [ebp - 0xb4]
0x0044711e: d945b0                   fld dword ptr [ebp - 0x50]
0x00447121: d88d50ffffff             fmul dword ptr [ebp - 0xb0]
0x00447127: dec1                     faddp st(1)
0x00447129: d99d38ffffff             fstp dword ptr [ebp - 0xc8]
0x0044712f: d945e4                   fld dword ptr [ebp - 0x1c]
0x00447132: d88d4cffffff             fmul dword ptr [ebp - 0xb4]
0x00447138: d945b4                   fld dword ptr [ebp - 0x4c]
0x0044713b: d88d50ffffff             fmul dword ptr [ebp - 0xb0]
0x00447141: dec1                     faddp st(1)
0x00447143: d99d3cffffff             fstp dword ptr [ebp - 0xc4]
0x00447149: d945e8                   fld dword ptr [ebp - 0x18]
0x0044714c: d88d4cffffff             fmul dword ptr [ebp - 0xb4]
0x00447152: d945b8                   fld dword ptr [ebp - 0x48]
0x00447155: d88d50ffffff             fmul dword ptr [ebp - 0xb0]
0x0044715b: dec1                     faddp st(1)
0x0044715d: d99d40ffffff             fstp dword ptr [ebp - 0xc0]
0x00447163: d945ec                   fld dword ptr [ebp - 0x14]
0x00447166: d88d4cffffff             fmul dword ptr [ebp - 0xb4]
0x0044716c: d945a4                   fld dword ptr [ebp - 0x5c]
0x0044716f: d88d50ffffff             fmul dword ptr [ebp - 0xb0]
0x00447175: dec1                     faddp st(1)
0x00447177: d99d04ffffff             fstp dword ptr [ebp - 0xfc]
0x0044717d: d98548ffffff             fld dword ptr [ebp - 0xb8]
0x00447183: d88504ffffff             fadd dword ptr [ebp - 0xfc]
0x00447189: d99548ffffff             fst dword ptr [ebp - 0xb8]
0x0044718f: d80d2cc35c00             fmul dword ptr [0x5cc32c]
0x00447195: d99d48ffffff             fstp dword ptr [ebp - 0xb8]
0x0044719b: d985f8feffff             fld dword ptr [ebp - 0x108]
0x004471a1: d88538ffffff             fadd dword ptr [ebp - 0xc8]
0x004471a7: d95de0                   fstp dword ptr [ebp - 0x20]
0x004471aa: d985fcfeffff             fld dword ptr [ebp - 0x104]
0x004471b0: d8853cffffff             fadd dword ptr [ebp - 0xc4]
0x004471b6: d95de4                   fstp dword ptr [ebp - 0x1c]
0x004471b9: d98500ffffff             fld dword ptr [ebp - 0x100]
0x004471bf: d88540ffffff             fadd dword ptr [ebp - 0xc0]
0x004471c5: d95de8                   fstp dword ptr [ebp - 0x18]
0x004471c8: 33c0                     xor eax, eax
0x004471ca: 85c0                     test eax, eax
0x004471cc: 75cd                     jne 0x44719b
0x004471ce: 8d4de0                   lea ecx, [ebp - 0x20]
0x004471d1: 51                       push ecx
0x004471d2: 8d55e0                   lea edx, [ebp - 0x20]
0x004471d5: 52                       push edx
0x004471d6: e8d5c70700               call 0x4c39b0
0x004471db: ddd8                     fstp st(0)
0x004471dd: 83c408                   add esp, 8
0x004471e0: c745c000000000           mov dword ptr [ebp - 0x40], 0
0x004471e7: c78518ffffff00000000     mov dword ptr [ebp - 0xe8], 0
0x004471f1: c745fc00000000           mov dword ptr [ebp - 4], 0
0x004471f8: eb09                     jmp 0x447203
0x004471fa: 8b45fc                   mov eax, dword ptr [ebp - 4]
0x004471fd: 83c001                   add eax, 1
0x00447200: 8945fc                   mov dword ptr [ebp - 4], eax
0x00447203: 837dfc04                 cmp dword ptr [ebp - 4], 4
0x00447207: 7d5b                     jge 0x447264
0x00447209: 8b4dfc                   mov ecx, dword ptr [ebp - 4]
0x0044720c: 51                       push ecx
0x0044720d: e85e71fcff               call 0x40e370
0x00447212: 83c404                   add esp, 4
0x00447215: 85c0                     test eax, eax
0x00447217: 7449                     je 0x447262
0x00447219: 8b55fc                   mov edx, dword ptr [ebp - 4]
0x0044721c: 52                       push edx
0x0044721d: e88e550200               call 0x46c7b0
0x00447222: 83c404                   add esp, 4
0x00447225: 83f801                   cmp eax, 1
0x00447228: 7538                     jne 0x447262
0x0044722a: 8b45c0                   mov eax, dword ptr [ebp - 0x40]
0x0044722d: 83c001                   add eax, 1
0x00447230: 8945c0                   mov dword ptr [ebp - 0x40], eax
0x00447233: 8d4da8                   lea ecx, [ebp - 0x58]
0x00447236: 51                       push ecx
0x00447237: 8d9524ffffff             lea edx, [ebp - 0xdc]
0x0044723d: 52                       push edx
0x0044723e: 8b45fc                   mov eax, dword ptr [ebp - 4]
0x00447241: 50                       push eax
0x00447242: e869590200               call 0x46cbb0
0x00447247: 83c40c                   add esp, 0xc
0x0044724a: 83bd24ffffff00           cmp dword ptr [ebp - 0xdc], 0
0x00447251: 740f                     je 0x447262
0x00447253: 8b8d18ffffff             mov ecx, dword ptr [ebp - 0xe8]
0x00447259: 83c101                   add ecx, 1
0x0044725c: 898d18ffffff             mov dword ptr [ebp - 0xe8], ecx
0x00447262: eb96                     jmp 0x4471fa
0x00447264: 837dc001                 cmp dword ptr [ebp - 0x40], 1
0x00447268: 7547                     jne 0x4472b1
0x0044726a: 83bd18ffffff01           cmp dword ptr [ebp - 0xe8], 1
0x00447271: 753e                     jne 0x4472b1
0x00447273: db45a8                   fild dword ptr [ebp - 0x58]
0x00447276: d835fcc95c00             fdiv dword ptr [0x5cc9fc]
0x0044727c: d9954cffffff             fst dword ptr [ebp - 0xb4]
0x00447282: d88548ffffff             fadd dword ptr [ebp - 0xb8]
0x00447288: d9954cffffff             fst dword ptr [ebp - 0xb4]
0x0044728e: d81d5cc55c00             fcomp dword ptr [0x5cc55c]
0x00447294: dfe0                     fnstsw ax
0x00447296: f6c441                   test ah, 0x41
0x00447299: 750a                     jne 0x4472a5
0x0044729b: c7854cffffff00002041     mov dword ptr [ebp - 0xb4], 0x41200000
0x004472a5: 8b954cffffff             mov edx, dword ptr [ebp - 0xb4]
0x004472ab: 899548ffffff             mov dword ptr [ebp - 0xb8], edx
0x004472b1: d98548ffffff             fld dword ptr [ebp - 0xb8]
0x004472b7: d80dbcca5c00             fmul dword ptr [0x5ccabc]
0x004472bd: d99d48ffffff             fstp dword ptr [ebp - 0xb8]
0x004472c3: 833dd00f7f0008           cmp dword ptr [0x7f0fd0], 8
0x004472ca: 750a                     jne 0x4472d6
0x004472cc: c78548ffffff00008040     mov dword ptr [ebp - 0xb8], 0x40800000
0x004472d6: 833dd00f7f0004           cmp dword ptr [0x7f0fd0], 4
0x004472dd: 750a                     jne 0x4472e9
0x004472df: c78548ffffff00008040     mov dword ptr [ebp - 0xb8], 0x40800000
0x004472e9: 8b8548ffffff             mov eax, dword ptr [ebp - 0xb8]
0x004472ef: 89854cffffff             mov dword ptr [ebp - 0xb4], eax
0x004472f5: d98548ffffff             fld dword ptr [ebp - 0xb8]
0x004472fb: d89d2cffffff             fcomp dword ptr [ebp - 0xd4]
0x00447301: dfe0                     fnstsw ax
0x00447303: f6c405                   test ah, 5
0x00447306: 7a0c                     jp 0x447314
0x00447308: 8b8d2cffffff             mov ecx, dword ptr [ebp - 0xd4]
0x0044730e: 898d48ffffff             mov dword ptr [ebp - 0xb8], ecx
0x00447314: d98548ffffff             fld dword ptr [ebp - 0xb8]
0x0044731a: d81d5cc55c00             fcomp dword ptr [0x5cc55c]
0x00447320: dfe0                     fnstsw ax
0x00447322: f6c441                   test ah, 0x41
0x00447325: 750a                     jne 0x447331
0x00447327: c78548ffffff00002041     mov dword ptr [ebp - 0xb8], 0x41200000
0x00447331: 833d380f7f0000           cmp dword ptr [0x7f0f38], 0
0x00447338: 740a                     je 0x447344
0x0044733a: c78548ffffff00002041     mov dword ptr [ebp - 0xb8], 0x41200000
0x00447344: d9854cffffff             fld dword ptr [ebp - 0xb4]
0x0044734a: d85dd4                   fcomp dword ptr [ebp - 0x2c]
0x0044734d: dfe0                     fnstsw ax
0x0044734f: f6c405                   test ah, 5
0x00447352: 7a09                     jp 0x44735d
0x00447354: 8b55d4                   mov edx, dword ptr [ebp - 0x2c]
0x00447357: 89954cffffff             mov dword ptr [ebp - 0xb4], edx
0x0044735d: d9854cffffff             fld dword ptr [ebp - 0xb4]
0x00447363: d81d5cc55c00             fcomp dword ptr [0x5cc55c]
0x00447369: dfe0                     fnstsw ax
0x0044736b: f6c441                   test ah, 0x41
0x0044736e: 750a                     jne 0x44737a
0x00447370: c7854cffffff00002041     mov dword ptr [ebp - 0xb4], 0x41200000
0x0044737a: 8b4508                   mov eax, dword ptr [ebp + 8]
0x0044737d: 8b8d4cffffff             mov ecx, dword ptr [ebp - 0xb4]
0x00447383: 8988a0090000             mov dword ptr [eax + 0x9a0], ecx
0x00447389: d905fc466100             fld dword ptr [0x6146fc]
0x0044738f: d84de0                   fmul dword ptr [ebp - 0x20]
0x00447392: d90500476100             fld dword ptr [0x614700]
0x00447398: d84de4                   fmul dword ptr [ebp - 0x1c]
0x0044739b: dec1                     faddp st(1)
0x0044739d: d90504476100             fld dword ptr [0x614704]
0x004473a3: d84de8                   fmul dword ptr [ebp - 0x18]
0x004473a6: dec1                     faddp st(1)
0x004473a8: d955dc                   fst dword ptr [ebp - 0x24]
0x004473ab: d81d3cc35c00             fcomp dword ptr [0x5cc33c]
0x004473b1: dfe0                     fnstsw ax
0x004473b3: f6c405                   test ah, 5
0x004473b6: 7a07                     jp 0x4473bf
0x004473b8: c745dc000080bf           mov dword ptr [ebp - 0x24], 0xbf800000
0x004473bf: d945dc                   fld dword ptr [ebp - 0x24]
0x004473c2: d81d20c35c00             fcomp dword ptr [0x5cc320]
0x004473c8: dfe0                     fnstsw ax
0x004473ca: f6c441                   test ah, 0x41
0x004473cd: 7507                     jne 0x4473d6
0x004473cf: c745dc0000803f           mov dword ptr [ebp - 0x24], 0x3f800000
0x004473d6: d945dc                   fld dword ptr [ebp - 0x24]
0x004473d9: 83ec08                   sub esp, 8
0x004473dc: dd1c24                   fstp qword ptr [esp]
0x004473df: e8a0bf0500               call 0x4a3384
0x004473e4: 83c408                   add esp, 8
0x004473e7: dc0de0ca5c00             fmul qword ptr [0x5ccae0]
0x004473ed: d99530feffff             fst dword ptr [ebp - 0x1d0]
0x004473f3: d82d9cd05c00             fsubr dword ptr [0x5cd09c]
0x004473f9: d82dd0ca5c00             fsubr dword ptr [0x5ccad0]
0x004473ff: d95ddc                   fstp dword ptr [ebp - 0x24]
0x00447402: d98548ffffff             fld dword ptr [ebp - 0xb8]
0x00447408: dc0dc8e15c00             fmul qword ptr [0x5ce1c8]
0x0044740e: dc3530d05c00             fdiv qword ptr [0x5cd030]
0x00447414: dc2dc0e15c00             fsubr qword ptr [0x5ce1c0]
0x0044741a: d99d4cffffff             fstp dword ptr [ebp - 0xb4]
0x00447420: d945e0                   fld dword ptr [ebp - 0x20]
0x00447423: d88d4cffffff             fmul dword ptr [ebp - 0xb4]
0x00447429: d95de0                   fstp dword ptr [ebp - 0x20]
0x0044742c: d945e4                   fld dword ptr [ebp - 0x1c]
0x0044742f: d88d4cffffff             fmul dword ptr [ebp - 0xb4]
0x00447435: d95de4                   fstp dword ptr [ebp - 0x1c]
0x00447438: d945e8                   fld dword ptr [ebp - 0x18]
0x0044743b: d88d4cffffff             fmul dword ptr [ebp - 0xb4]
0x00447441: d95de8                   fstp dword ptr [ebp - 0x18]
0x00447444: 33d2                     xor edx, edx
0x00447446: 85d2                     test edx, edx
0x00447448: 75d6                     jne 0x447420
0x0044744a: 8d45e0                   lea eax, [ebp - 0x20]
0x0044744d: 50                       push eax
0x0044744e: 8d4db0                   lea ecx, [ebp - 0x50]
0x00447451: 51                       push ecx
0x00447452: e859c50700               call 0x4c39b0
0x00447457: ddd8                     fstp st(0)
0x00447459: 83c408                   add esp, 8
0x0044745c: d945b4                   fld dword ptr [ebp - 0x4c]
0x0044745f: d80d04476100             fmul dword ptr [0x614704]
0x00447465: d945b8                   fld dword ptr [ebp - 0x48]
0x00447468: d80d00476100             fmul dword ptr [0x614700]
0x0044746e: dee9                     fsubp st(1)
0x00447470: d99df8feffff             fstp dword ptr [ebp - 0x108]
0x00447476: d945b8                   fld dword ptr [ebp - 0x48]
0x00447479: d80dfc466100             fmul dword ptr [0x6146fc]
0x0044747f: d945b0                   fld dword ptr [ebp - 0x50]
0x00447482: d80d04476100             fmul dword ptr [0x614704]
0x00447488: dee9                     fsubp st(1)
0x0044748a: d99dfcfeffff             fstp dword ptr [ebp - 0x104]
0x00447490: d945b0                   fld dword ptr [ebp - 0x50]
0x00447493: d80d00476100             fmul dword ptr [0x614700]
0x00447499: d945b4                   fld dword ptr [ebp - 0x4c]
0x0044749c: d80dfc466100             fmul dword ptr [0x6146fc]
0x004474a2: dee9                     fsubp st(1)
0x004474a4: d99d00ffffff             fstp dword ptr [ebp - 0x100]
0x004474aa: 33d2                     xor edx, edx
0x004474ac: 85d2                     test edx, edx
0x004474ae: 75ac                     jne 0x44745c
0x004474b0: d90520d15c00             fld dword ptr [0x5cd120]
0x004474b6: d88d48ffffff             fmul dword ptr [ebp - 0xb8]
0x004474bc: d8355cc55c00             fdiv dword ptr [0x5cc55c]
0x004474c2: d99d4cffffff             fstp dword ptr [ebp - 0xb4]
0x004474c8: e833f7fdff               call 0x426c00
0x004474cd: 83f81a                   cmp eax, 0x1a
0x004474d0: 752a                     jne 0x4474fc
0x004474d2: d905b8e15c00             fld dword ptr [0x5ce1b8]
0x004474d8: d88d48ffffff             fmul dword ptr [ebp - 0xb8]
0x004474de: d8355cc55c00             fdiv dword ptr [0x5cc55c]
0x004474e4: d99d4cffffff             fstp dword ptr [ebp - 0xb4]
0x004474ea: d98548ffffff             fld dword ptr [ebp - 0xb8]
0x004474f0: d80d74d05c00             fmul dword ptr [0x5cd074]
0x004474f6: d99d48ffffff             fstp dword ptr [ebp - 0xb8]
0x004474fc: d9854cffffff             fld dword ptr [ebp - 0xb4]
0x00447502: d82558c35c00             fsub dword ptr [0x5cc358]
0x00447508: d9954cffffff             fst dword ptr [ebp - 0xb4]
0x0044750e: d845dc                   fadd dword ptr [ebp - 0x24]
0x00447511: d81db4e15c00             fcomp dword ptr [0x5ce1b4]
0x00447517: dfe0                     fnstsw ax
0x00447519: f6c401                   test ah, 1
0x0044751c: 750f                     jne 0x44752d
0x0044751e: d905b4e15c00             fld dword ptr [0x5ce1b4]
0x00447524: d865dc                   fsub dword ptr [ebp - 0x24]
0x00447527: d99d4cffffff             fstp dword ptr [ebp - 0xb4]
0x0044752d: 833d380f7f0000           cmp dword ptr [0x7f0f38], 0
0x00447534: 740a                     je 0x447540
0x00447536: c7854cffffff0000b442     mov dword ptr [ebp - 0xb4], 0x42b40000
0x00447540: 6a00                     push 0
0x00447542: 8b854cffffff             mov eax, dword ptr [ebp - 0xb4]
0x00447548: 50                       push eax
0x00447549: 8d8df8feffff             lea ecx, [ebp - 0x108]
0x0044754f: 51                       push ecx
0x00447550: 8d9558ffffff             lea edx, [ebp - 0xa8]
0x00447556: 52                       push edx
0x00447557: e8c4d70700               call 0x4c4d20
0x0044755c: 83c410                   add esp, 0x10
0x0044755f: 8d8558ffffff             lea eax, [ebp - 0xa8]
0x00447565: 50                       push eax
0x00447566: 6a01                     push 1
0x00447568: 8d4de0                   lea ecx, [ebp - 0x20]
0x0044756b: 51                       push ecx
0x0044756c: 8d55e0                   lea edx, [ebp - 0x20]
0x0044756f: 52                       push edx
0x00447570: e87bc80700               call 0x4c3df0
0x00447575: 83c410                   add esp, 0x10
0x00447578: d98548ffffff             fld dword ptr [ebp - 0xb8]
0x0044757e: d835bcc95c00             fdiv dword ptr [0x5cc9bc]
0x00447584: d83558c35c00             fdiv dword ptr [0x5cc358]
0x0044758a: d80520c35c00             fadd dword ptr [0x5cc320]
0x00447590: d99d4cffffff             fstp dword ptr [ebp - 0xb4]
0x00447596: d945e0                   fld dword ptr [ebp - 0x20]
0x00447599: d88d4cffffff             fmul dword ptr [ebp - 0xb4]
0x0044759f: d95de0                   fstp dword ptr [ebp - 0x20]
0x004475a2: d945e4                   fld dword ptr [ebp - 0x1c]
0x004475a5: d88d4cffffff             fmul dword ptr [ebp - 0xb4]
0x004475ab: d95de4                   fstp dword ptr [ebp - 0x1c]
0x004475ae: d945e8                   fld dword ptr [ebp - 0x18]
0x004475b1: d88d4cffffff             fmul dword ptr [ebp - 0xb4]
0x004475b7: d95de8                   fstp dword ptr [ebp - 0x18]
0x004475ba: 33c0                     xor eax, eax
0x004475bc: 85c0                     test eax, eax
0x004475be: 75d6                     jne 0x447596
0x004475c0: d945e0                   fld dword ptr [ebp - 0x20]
0x004475c3: d845c4                   fadd dword ptr [ebp - 0x3c]
0x004475c6: d99d0cffffff             fstp dword ptr [ebp - 0xf4]
0x004475cc: d945e4                   fld dword ptr [ebp - 0x1c]
0x004475cf: d845c8                   fadd dword ptr [ebp - 0x38]
0x004475d2: d99d10ffffff             fstp dword ptr [ebp - 0xf0]
0x004475d8: d945e8                   fld dword ptr [ebp - 0x18]
0x004475db: d88544ffffff             fadd dword ptr [ebp - 0xbc]
0x004475e1: d99d14ffffff             fstp dword ptr [ebp - 0xec]
0x004475e7: 833d380f7f0000           cmp dword ptr [0x7f0f38], 0
0x004475ee: 7424                     je 0x447614
0x004475f0: 8b4dc4                   mov ecx, dword ptr [ebp - 0x3c]
0x004475f3: 898d0cffffff             mov dword ptr [ebp - 0xf4], ecx
0x004475f9: d945c8                   fld dword ptr [ebp - 0x38]
0x004475fc: d805b0c95c00             fadd dword ptr [0x5cc9b0]
0x00447602: d99d10ffffff             fstp dword ptr [ebp - 0xf0]
0x00447608: 8b9544ffffff             mov edx, dword ptr [ebp - 0xbc]
0x0044760e: 899514ffffff             mov dword ptr [ebp - 0xec], edx
0x00447614: c745e400000000           mov dword ptr [ebp - 0x1c], 0
0x0044761b: 8d45e0                   lea eax, [ebp - 0x20]
0x0044761e: 50                       push eax
0x0044761f: 8d4de0                   lea ecx, [ebp - 0x20]
0x00447622: 51                       push ecx
0x00447623: e888c30700               call 0x4c39b0
0x00447628: ddd8                     fstp st(0)
0x0044762a: 83c408                   add esp, 8
0x0044762d: d9055cc55c00             fld dword ptr [0x5cc55c]
0x00447633: d8a548ffffff             fsub dword ptr [ebp - 0xb8]
0x00447639: d80db0e15c00             fmul dword ptr [0x5ce1b0]
0x0044763f: d8355cc55c00             fdiv dword ptr [0x5cc55c]
0x00447645: d99d4cffffff             fstp dword ptr [ebp - 0xb4]
0x0044764b: d90588d05c00             fld dword ptr [0x5cd088]
0x00447651: d88d48ffffff             fmul dword ptr [ebp - 0xb8]
0x00447657: d8355cc55c00             fdiv dword ptr [0x5cc55c]
0x0044765d: d8854cffffff             fadd dword ptr [ebp - 0xb4]
0x00447663: d99d4cffffff             fstp dword ptr [ebp - 0xb4]
0x00447669: e892f5fdff               call 0x426c00
0x0044766e: 83f81a                   cmp eax, 0x1a
0x00447671: 751e                     jne 0x447691
0x00447673: d905ace15c00             fld dword ptr [0x5ce1ac]
0x00447679: d88d48ffffff             fmul dword ptr [ebp - 0xb8]
0x0044767f: d8355cc55c00             fdiv dword ptr [0x5cc55c]
0x00447685: d8ad4cffffff             fsubr dword ptr [ebp - 0xb4]
0x0044768b: d99d4cffffff             fstp dword ptr [ebp - 0xb4]
0x00447691: d945e0                   fld dword ptr [ebp - 0x20]
0x00447694: d88d4cffffff             fmul dword ptr [ebp - 0xb4]
0x0044769a: d95de0                   fstp dword ptr [ebp - 0x20]
0x0044769d: d945e4                   fld dword ptr [ebp - 0x1c]
0x004476a0: d88d4cffffff             fmul dword ptr [ebp - 0xb4]
0x004476a6: d95de4                   fstp dword ptr [ebp - 0x1c]
0x004476a9: d945e8                   fld dword ptr [ebp - 0x18]
0x004476ac: d88d4cffffff             fmul dword ptr [ebp - 0xb4]
0x004476b2: d95de8                   fstp dword ptr [ebp - 0x18]
0x004476b5: 33d2                     xor edx, edx
0x004476b7: 85d2                     test edx, edx
0x004476b9: 75d6                     jne 0x447691
0x004476bb: 833d380f7f0000           cmp dword ptr [0x7f0f38], 0
0x004476c2: 754e                     jne 0x447712
0x004476c4: d945c4                   fld dword ptr [ebp - 0x3c]
0x004476c7: d845e0                   fadd dword ptr [ebp - 0x20]
0x004476ca: d95dc4                   fstp dword ptr [ebp - 0x3c]
0x004476cd: d945c8                   fld dword ptr [ebp - 0x38]
0x004476d0: d845e4                   fadd dword ptr [ebp - 0x1c]
0x004476d3: d95dc8                   fstp dword ptr [ebp - 0x38]
0x004476d6: d98544ffffff             fld dword ptr [ebp - 0xbc]
0x004476dc: d845e8                   fadd dword ptr [ebp - 0x18]
0x004476df: d99d44ffffff             fstp dword ptr [ebp - 0xbc]
0x004476e5: d9850cffffff             fld dword ptr [ebp - 0xf4]
0x004476eb: d845e0                   fadd dword ptr [ebp - 0x20]
0x004476ee: d99d0cffffff             fstp dword ptr [ebp - 0xf4]
0x004476f4: d98510ffffff             fld dword ptr [ebp - 0xf0]
0x004476fa: d845e4                   fadd dword ptr [ebp - 0x1c]
0x004476fd: d99d10ffffff             fstp dword ptr [ebp - 0xf0]
0x00447703: d98514ffffff             fld dword ptr [ebp - 0xec]
0x00447709: d845e8                   fadd dword ptr [ebp - 0x18]
0x0044770c: d99d14ffffff             fstp dword ptr [ebp - 0xec]
0x00447712: c78530ffffff00000000     mov dword ptr [ebp - 0xd0], 0
0x0044771c: 8b4508                   mov eax, dword ptr [ebp + 8]
0x0044771f: d98064090000             fld dword ptr [eax + 0x964]
0x00447725: d8ad0cffffff             fsubr dword ptr [ebp - 0xf4]
0x0044772b: d95de0                   fstp dword ptr [ebp - 0x20]
0x0044772e: 8b4d08                   mov ecx, dword ptr [ebp + 8]
0x00447731: d98168090000             fld dword ptr [ecx + 0x968]
0x00447737: d8ad10ffffff             fsubr dword ptr [ebp - 0xf0]
0x0044773d: d95de4                   fstp dword ptr [ebp - 0x1c]
0x00447740: 8b5508                   mov edx, dword ptr [ebp + 8]
0x00447743: d9826c090000             fld dword ptr [edx + 0x96c]
0x00447749: d8ad14ffffff             fsubr dword ptr [ebp - 0xec]
0x0044774f: d95de8                   fstp dword ptr [ebp - 0x18]
0x00447752: 33c0                     xor eax, eax
0x00447754: 85c0                     test eax, eax
0x00447756: 75c4                     jne 0x44771c
0x00447758: 8d4de0                   lea ecx, [ebp - 0x20]
0x0044775b: 51                       push ecx
0x0044775c: e85fc30700               call 0x4c3ac0
0x00447761: 83c404                   add esp, 4
0x00447764: d9954cffffff             fst dword ptr [ebp - 0xb4]
0x0044776a: d81d58c35c00             fcomp dword ptr [0x5cc358]
0x00447770: dfe0                     fnstsw ax
0x00447772: f6c441                   test ah, 0x41
0x00447775: 750a                     jne 0x447781
0x00447777: c78530ffffff01000000     mov dword ptr [ebp - 0xd0], 1
0x00447781: 837d0c00                 cmp dword ptr [ebp + 0xc], 0
0x00447785: 750d                     jne 0x447794
0x00447787: 83bd30ffffff00           cmp dword ptr [ebp - 0xd0], 0
0x0044778e: 0f84db000000             je 0x44786f
0x00447794: 8b5508                   mov edx, dword ptr [ebp + 8]
0x00447797: 81c264090000             add edx, 0x964
0x0044779d: 8b850cffffff             mov eax, dword ptr [ebp - 0xf4]
0x004477a3: 8902                     mov dword ptr [edx], eax
0x004477a5: 8b8d10ffffff             mov ecx, dword ptr [ebp - 0xf0]
0x004477ab: 894a04                   mov dword ptr [edx + 4], ecx
0x004477ae: 8b8514ffffff             mov eax, dword ptr [ebp - 0xec]
0x004477b4: 894208                   mov dword ptr [edx + 8], eax
0x004477b7: 8b4d08                   mov ecx, dword ptr [ebp + 8]
0x004477ba: c7817809000000000000     mov dword ptr [ecx + 0x978], 0
0x004477c4: 8b5508                   mov edx, dword ptr [ebp + 8]
0x004477c7: c7827409000000000000     mov dword ptr [edx + 0x974], 0
0x004477d1: 8b4508                   mov eax, dword ptr [ebp + 8]
0x004477d4: c7807009000000000000     mov dword ptr [eax + 0x970], 0
0x004477de: 8b4dc4                   mov ecx, dword ptr [ebp - 0x3c]
0x004477e1: 894dbc                   mov dword ptr [ebp - 0x44], ecx
0x004477e4: 8b5508                   mov edx, dword ptr [ebp + 8]
0x004477e7: 8b45bc                   mov eax, dword ptr [ebp - 0x44]
0x004477ea: 89827c090000             mov dword ptr [edx + 0x97c], eax
0x004477f0: 8b4dc8                   mov ecx, dword ptr [ebp - 0x38]
0x004477f3: 898d08ffffff             mov dword ptr [ebp - 0xf8], ecx
0x004477f9: 8b5508                   mov edx, dword ptr [ebp + 8]
0x004477fc: 8b8508ffffff             mov eax, dword ptr [ebp - 0xf8]
0x00447802: 898280090000             mov dword ptr [edx + 0x980], eax
0x00447808: 8b8d44ffffff             mov ecx, dword ptr [ebp - 0xbc]
0x0044780e: 898d20ffffff             mov dword ptr [ebp - 0xe0], ecx
0x00447814: 8b5508                   mov edx, dword ptr [ebp + 8]
0x00447817: 8b8520ffffff             mov eax, dword ptr [ebp - 0xe0]
0x0044781d: 898284090000             mov dword ptr [edx + 0x984], eax
0x00447823: 8b4d08                   mov ecx, dword ptr [ebp + 8]
0x00447826: c7819009000000000000     mov dword ptr [ecx + 0x990], 0
0x00447830: 8b5508                   mov edx, dword ptr [ebp + 8]
0x00447833: c7828c09000000000000     mov dword ptr [edx + 0x98c], 0
0x0044783d: 8b4508                   mov eax, dword ptr [ebp + 8]
0x00447840: c7808809000000000000     mov dword ptr [eax + 0x988], 0
0x0044784a: 8b4d08                   mov ecx, dword ptr [ebp + 8]
0x0044784d: 8b55d8                   mov edx, dword ptr [ebp - 0x28]
0x00447850: 899194090000             mov dword ptr [ecx + 0x994], edx
0x00447856: 8b4508                   mov eax, dword ptr [ebp + 8]
0x00447859: 8b4d9c                   mov ecx, dword ptr [ebp - 0x64]
0x0044785c: 898898090000             mov dword ptr [eax + 0x998], ecx
0x00447862: 8b5508                   mov edx, dword ptr [ebp + 8]
0x00447865: c7829c09000000000000     mov dword ptr [edx + 0x99c], 0
0x0044786f: 8b4508                   mov eax, dword ptr [ebp + 8]
0x00447872: d98064090000             fld dword ptr [eax + 0x964]
0x00447878: d8ad0cffffff             fsubr dword ptr [ebp - 0xf4]
0x0044787e: d95de0                   fstp dword ptr [ebp - 0x20]
0x00447881: 8b4d08                   mov ecx, dword ptr [ebp + 8]
0x00447884: d98168090000             fld dword ptr [ecx + 0x968]
0x0044788a: d8ad10ffffff             fsubr dword ptr [ebp - 0xf0]
0x00447890: d95de4                   fstp dword ptr [ebp - 0x1c]
0x00447893: 8b5508                   mov edx, dword ptr [ebp + 8]
0x00447896: d9826c090000             fld dword ptr [edx + 0x96c]
0x0044789c: d8ad14ffffff             fsubr dword ptr [ebp - 0xec]
0x004478a2: d95de8                   fstp dword ptr [ebp - 0x18]
0x004478a5: 33c0                     xor eax, eax
0x004478a7: 85c0                     test eax, eax
0x004478a9: 75c4                     jne 0x44786f
0x004478ab: d945e0                   fld dword ptr [ebp - 0x20]
0x004478ae: d80da0c95c00             fmul dword ptr [0x5cc9a0]
0x004478b4: d95de0                   fstp dword ptr [ebp - 0x20]
0x004478b7: d945e4                   fld dword ptr [ebp - 0x1c]
0x004478ba: d80da0c95c00             fmul dword ptr [0x5cc9a0]
0x004478c0: d95de4                   fstp dword ptr [ebp - 0x1c]
0x004478c3: d945e8                   fld dword ptr [ebp - 0x18]
0x004478c6: d80da0c95c00             fmul dword ptr [0x5cc9a0]
0x004478cc: d95de8                   fstp dword ptr [ebp - 0x18]
0x004478cf: 33c9                     xor ecx, ecx
0x004478d1: 85c9                     test ecx, ecx
0x004478d3: 75d6                     jne 0x4478ab
0x004478d5: 8b5508                   mov edx, dword ptr [ebp + 8]
0x004478d8: d98270090000             fld dword ptr [edx + 0x970]
0x004478de: d845e0                   fadd dword ptr [ebp - 0x20]
0x004478e1: 8b4508                   mov eax, dword ptr [ebp + 8]
0x004478e4: d99870090000             fstp dword ptr [eax + 0x970]
0x004478ea: 8b4d08                   mov ecx, dword ptr [ebp + 8]
0x004478ed: d98174090000             fld dword ptr [ecx + 0x974]
0x004478f3: d845e4                   fadd dword ptr [ebp - 0x1c]
0x004478f6: 8b5508                   mov edx, dword ptr [ebp + 8]
0x004478f9: d99a74090000             fstp dword ptr [edx + 0x974]
0x004478ff: 8b4508                   mov eax, dword ptr [ebp + 8]
0x00447902: d98078090000             fld dword ptr [eax + 0x978]
0x00447908: d845e8                   fadd dword ptr [ebp - 0x18]
0x0044790b: 8b4d08                   mov ecx, dword ptr [ebp + 8]
0x0044790e: d99978090000             fstp dword ptr [ecx + 0x978]
0x00447914: 33d2                     xor edx, edx
0x00447916: 85d2                     test edx, edx
0x00447918: 75bb                     jne 0x4478d5
0x0044791a: 8b4508                   mov eax, dword ptr [ebp + 8]
0x0044791d: d98070090000             fld dword ptr [eax + 0x970]
0x00447923: d80dbcc95c00             fmul dword ptr [0x5cc9bc]
0x00447929: 8b4d08                   mov ecx, dword ptr [ebp + 8]
0x0044792c: d99970090000             fstp dword ptr [ecx + 0x970]
0x00447932: 8b5508                   mov edx, dword ptr [ebp + 8]
0x00447935: d98274090000             fld dword ptr [edx + 0x974]
0x0044793b: d80dbcc95c00             fmul dword ptr [0x5cc9bc]
0x00447941: 8b4508                   mov eax, dword ptr [ebp + 8]
0x00447944: d99874090000             fstp dword ptr [eax + 0x974]
0x0044794a: 8b4d08                   mov ecx, dword ptr [ebp + 8]
0x0044794d: d98178090000             fld dword ptr [ecx + 0x978]
0x00447953: d80dbcc95c00             fmul dword ptr [0x5cc9bc]
0x00447959: 8b5508                   mov edx, dword ptr [ebp + 8]
0x0044795c: d99a78090000             fstp dword ptr [edx + 0x978]
0x00447962: 33c0                     xor eax, eax
0x00447964: 85c0                     test eax, eax
0x00447966: 75b2                     jne 0x44791a
0x00447968: 8b4d08                   mov ecx, dword ptr [ebp + 8]
0x0044796b: d98164090000             fld dword ptr [ecx + 0x964]
0x00447971: 8b5508                   mov edx, dword ptr [ebp + 8]
0x00447974: d88270090000             fadd dword ptr [edx + 0x970]
0x0044797a: d95de0                   fstp dword ptr [ebp - 0x20]
0x0044797d: 8b4508                   mov eax, dword ptr [ebp + 8]
0x00447980: d98068090000             fld dword ptr [eax + 0x968]
0x00447986: 8b4d08                   mov ecx, dword ptr [ebp + 8]
0x00447989: d88174090000             fadd dword ptr [ecx + 0x974]
0x0044798f: d95de4                   fstp dword ptr [ebp - 0x1c]
0x00447992: 8b5508                   mov edx, dword ptr [ebp + 8]
0x00447995: d9826c090000             fld dword ptr [edx + 0x96c]
0x0044799b: 8b4508                   mov eax, dword ptr [ebp + 8]
0x0044799e: d88078090000             fadd dword ptr [eax + 0x978]
0x004479a4: d95de8                   fstp dword ptr [ebp - 0x18]
0x004479a7: 33c9                     xor ecx, ecx
0x004479a9: 85c9                     test ecx, ecx
0x004479ab: 75bb                     jne 0x447968
0x004479ad: d9850cffffff             fld dword ptr [ebp - 0xf4]
0x004479b3: d865e0                   fsub dword ptr [ebp - 0x20]
0x004479b6: d95db0                   fstp dword ptr [ebp - 0x50]
0x004479b9: d98510ffffff             fld dword ptr [ebp - 0xf0]
0x004479bf: d865e4                   fsub dword ptr [ebp - 0x1c]
0x004479c2: d95db4                   fstp dword ptr [ebp - 0x4c]
0x004479c5: d98514ffffff             fld dword ptr [ebp - 0xec]
0x004479cb: d865e8                   fsub dword ptr [ebp - 0x18]
0x004479ce: d95db8                   fstp dword ptr [ebp - 0x48]
0x004479d1: 33d2                     xor edx, edx
0x004479d3: 85d2                     test edx, edx
0x004479d5: 75d6                     jne 0x4479ad
0x004479d7: d9055cc55c00             fld dword ptr [0x5cc55c]
0x004479dd: d8a548ffffff             fsub dword ptr [ebp - 0xb8]
0x004479e3: d8355cc55c00             fdiv dword ptr [0x5cc55c]
0x004479e9: d84db0                   fmul dword ptr [ebp - 0x50]
0x004479ec: d95db0                   fstp dword ptr [ebp - 0x50]
0x004479ef: d9055cc55c00             fld dword ptr [0x5cc55c]
0x004479f5: d8a548ffffff             fsub dword ptr [ebp - 0xb8]
0x004479fb: d8355cc55c00             fdiv dword ptr [0x5cc55c]
0x00447a01: d84db4                   fmul dword ptr [ebp - 0x4c]
0x00447a04: d95db4                   fstp dword ptr [ebp - 0x4c]
0x00447a07: d9055cc55c00             fld dword ptr [0x5cc55c]
0x00447a0d: d8a548ffffff             fsub dword ptr [ebp - 0xb8]
0x00447a13: d8355cc55c00             fdiv dword ptr [0x5cc55c]
0x00447a19: d84db8                   fmul dword ptr [ebp - 0x48]
0x00447a1c: d95db8                   fstp dword ptr [ebp - 0x48]
0x00447a1f: 33c0                     xor eax, eax
0x00447a21: 85c0                     test eax, eax
0x00447a23: 75b2                     jne 0x4479d7
0x00447a25: d9850cffffff             fld dword ptr [ebp - 0xf4]
0x00447a2b: d865b0                   fsub dword ptr [ebp - 0x50]
0x00447a2e: d99d0cffffff             fstp dword ptr [ebp - 0xf4]
0x00447a34: d98510ffffff             fld dword ptr [ebp - 0xf0]
0x00447a3a: d865b4                   fsub dword ptr [ebp - 0x4c]
0x00447a3d: d99d10ffffff             fstp dword ptr [ebp - 0xf0]
0x00447a43: d98514ffffff             fld dword ptr [ebp - 0xec]
0x00447a49: d865b8                   fsub dword ptr [ebp - 0x48]
0x00447a4c: d99d14ffffff             fstp dword ptr [ebp - 0xec]
0x00447a52: 8b4dc4                   mov ecx, dword ptr [ebp - 0x3c]
0x00447a55: 894df0                   mov dword ptr [ebp - 0x10], ecx
0x00447a58: 8b55c8                   mov edx, dword ptr [ebp - 0x38]
0x00447a5b: 8955f4                   mov dword ptr [ebp - 0xc], edx
0x00447a5e: 8b8544ffffff             mov eax, dword ptr [ebp - 0xbc]
0x00447a64: 8945f8                   mov dword ptr [ebp - 8], eax
0x00447a67: 8b4d08                   mov ecx, dword ptr [ebp + 8]
0x00447a6a: d9817c090000             fld dword ptr [ecx + 0x97c]
0x00447a70: d86df0                   fsubr dword ptr [ebp - 0x10]
0x00447a73: d95de0                   fstp dword ptr [ebp - 0x20]
0x00447a76: 8b5508                   mov edx, dword ptr [ebp + 8]
0x00447a79: d98280090000             fld dword ptr [edx + 0x980]
0x00447a7f: d86df4                   fsubr dword ptr [ebp - 0xc]
0x00447a82: d95de4                   fstp dword ptr [ebp - 0x1c]
0x00447a85: 8b4508                   mov eax, dword ptr [ebp + 8]
0x00447a88: d98084090000             fld dword ptr [eax + 0x984]
0x00447a8e: d86df8                   fsubr dword ptr [ebp - 8]
0x00447a91: d95de8                   fstp dword ptr [ebp - 0x18]
0x00447a94: 33c9                     xor ecx, ecx
0x00447a96: 85c9                     test ecx, ecx
0x00447a98: 75cd                     jne 0x447a67
0x00447a9a: d945e0                   fld dword ptr [ebp - 0x20]
0x00447a9d: d80da0c95c00             fmul dword ptr [0x5cc9a0]
0x00447aa3: d95de0                   fstp dword ptr [ebp - 0x20]
0x00447aa6: d945e4                   fld dword ptr [ebp - 0x1c]
0x00447aa9: d80da0c95c00             fmul dword ptr [0x5cc9a0]
0x00447aaf: d95de4                   fstp dword ptr [ebp - 0x1c]
0x00447ab2: d945e8                   fld dword ptr [ebp - 0x18]
0x00447ab5: d80da0c95c00             fmul dword ptr [0x5cc9a0]
0x00447abb: d95de8                   fstp dword ptr [ebp - 0x18]
0x00447abe: 33d2                     xor edx, edx
0x00447ac0: 85d2                     test edx, edx
0x00447ac2: 75d6                     jne 0x447a9a
0x00447ac4: 8b4508                   mov eax, dword ptr [ebp + 8]
0x00447ac7: d98088090000             fld dword ptr [eax + 0x988]
0x00447acd: d845e0                   fadd dword ptr [ebp - 0x20]
0x00447ad0: 8b4d08                   mov ecx, dword ptr [ebp + 8]
0x00447ad3: d99988090000             fstp dword ptr [ecx + 0x988]
0x00447ad9: 8b5508                   mov edx, dword ptr [ebp + 8]
0x00447adc: d9828c090000             fld dword ptr [edx + 0x98c]
0x00447ae2: d845e4                   fadd dword ptr [ebp - 0x1c]
0x00447ae5: 8b4508                   mov eax, dword ptr [ebp + 8]
0x00447ae8: d9988c090000             fstp dword ptr [eax + 0x98c]
0x00447aee: 8b4d08                   mov ecx, dword ptr [ebp + 8]
0x00447af1: d98190090000             fld dword ptr [ecx + 0x990]
0x00447af7: d845e8                   fadd dword ptr [ebp - 0x18]
0x00447afa: 8b5508                   mov edx, dword ptr [ebp + 8]
0x00447afd: d99a90090000             fstp dword ptr [edx + 0x990]
0x00447b03: 33c0                     xor eax, eax
0x00447b05: 85c0                     test eax, eax
0x00447b07: 75bb                     jne 0x447ac4
0x00447b09: 8b4d08                   mov ecx, dword ptr [ebp + 8]
0x00447b0c: d98188090000             fld dword ptr [ecx + 0x988]
0x00447b12: d80dbcc95c00             fmul dword ptr [0x5cc9bc]
0x00447b18: 8b5508                   mov edx, dword ptr [ebp + 8]
0x00447b1b: d99a88090000             fstp dword ptr [edx + 0x988]
0x00447b21: 8b4508                   mov eax, dword ptr [ebp + 8]
0x00447b24: d9808c090000             fld dword ptr [eax + 0x98c]
0x00447b2a: d80dbcc95c00             fmul dword ptr [0x5cc9bc]
0x00447b30: 8b4d08                   mov ecx, dword ptr [ebp + 8]
0x00447b33: d9998c090000             fstp dword ptr [ecx + 0x98c]
0x00447b39: 8b5508                   mov edx, dword ptr [ebp + 8]
0x00447b3c: d98290090000             fld dword ptr [edx + 0x990]
0x00447b42: d80dbcc95c00             fmul dword ptr [0x5cc9bc]
0x00447b48: 8b4508                   mov eax, dword ptr [ebp + 8]
0x00447b4b: d99890090000             fstp dword ptr [eax + 0x990]
0x00447b51: 33c9                     xor ecx, ecx
0x00447b53: 85c9                     test ecx, ecx
0x00447b55: 75b2                     jne 0x447b09
0x00447b57: 8b5508                   mov edx, dword ptr [ebp + 8]
0x00447b5a: d9827c090000             fld dword ptr [edx + 0x97c]
0x00447b60: 8b4508                   mov eax, dword ptr [ebp + 8]
0x00447b63: d88088090000             fadd dword ptr [eax + 0x988]
0x00447b69: d95de0                   fstp dword ptr [ebp - 0x20]
0x00447b6c: 8b4d08                   mov ecx, dword ptr [ebp + 8]
0x00447b6f: d98180090000             fld dword ptr [ecx + 0x980]
0x00447b75: 8b5508                   mov edx, dword ptr [ebp + 8]
0x00447b78: d8828c090000             fadd dword ptr [edx + 0x98c]
0x00447b7e: d95de4                   fstp dword ptr [ebp - 0x1c]
0x00447b81: 8b4508                   mov eax, dword ptr [ebp + 8]
0x00447b84: d98084090000             fld dword ptr [eax + 0x984]
0x00447b8a: 8b4d08                   mov ecx, dword ptr [ebp + 8]
0x00447b8d: d88190090000             fadd dword ptr [ecx + 0x990]
0x00447b93: d95de8                   fstp dword ptr [ebp - 0x18]
0x00447b96: 33d2                     xor edx, edx
0x00447b98: 85d2                     test edx, edx
0x00447b9a: 75bb                     jne 0x447b57
0x00447b9c: d945f0                   fld dword ptr [ebp - 0x10]
0x00447b9f: d865e0                   fsub dword ptr [ebp - 0x20]
0x00447ba2: d95db0                   fstp dword ptr [ebp - 0x50]
0x00447ba5: d945f4                   fld dword ptr [ebp - 0xc]
0x00447ba8: d865e4                   fsub dword ptr [ebp - 0x1c]
0x00447bab: d95db4                   fstp dword ptr [ebp - 0x4c]
0x00447bae: d945f8                   fld dword ptr [ebp - 8]
0x00447bb1: d865e8                   fsub dword ptr [ebp - 0x18]
0x00447bb4: d95db8                   fstp dword ptr [ebp - 0x48]
0x00447bb7: 33c0                     xor eax, eax
0x00447bb9: 85c0                     test eax, eax
0x00447bbb: 75df                     jne 0x447b9c
0x00447bbd: d9055cc55c00             fld dword ptr [0x5cc55c]
0x00447bc3: d8a548ffffff             fsub dword ptr [ebp - 0xb8]
0x00447bc9: d8355cc55c00             fdiv dword ptr [0x5cc55c]
0x00447bcf: d84db0                   fmul dword ptr [ebp - 0x50]
0x00447bd2: d95db0                   fstp dword ptr [ebp - 0x50]
0x00447bd5: d9055cc55c00             fld dword ptr [0x5cc55c]
0x00447bdb: d8a548ffffff             fsub dword ptr [ebp - 0xb8]
0x00447be1: d8355cc55c00             fdiv dword ptr [0x5cc55c]
0x00447be7: d84db4                   fmul dword ptr [ebp - 0x4c]
0x00447bea: d95db4                   fstp dword ptr [ebp - 0x4c]
0x00447bed: d9055cc55c00             fld dword ptr [0x5cc55c]
0x00447bf3: d8a548ffffff             fsub dword ptr [ebp - 0xb8]
0x00447bf9: d8355cc55c00             fdiv dword ptr [0x5cc55c]
0x00447bff: d84db8                   fmul dword ptr [ebp - 0x48]
0x00447c02: d95db8                   fstp dword ptr [ebp - 0x48]
0x00447c05: 33c9                     xor ecx, ecx
0x00447c07: 85c9                     test ecx, ecx
0x00447c09: 75b2                     jne 0x447bbd
0x00447c0b: d945f0                   fld dword ptr [ebp - 0x10]
0x00447c0e: d865b0                   fsub dword ptr [ebp - 0x50]
0x00447c11: d95df0                   fstp dword ptr [ebp - 0x10]
0x00447c14: d945f4                   fld dword ptr [ebp - 0xc]
0x00447c17: d865b4                   fsub dword ptr [ebp - 0x4c]
0x00447c1a: d95df4                   fstp dword ptr [ebp - 0xc]
0x00447c1d: d945f8                   fld dword ptr [ebp - 8]
0x00447c20: d865b8                   fsub dword ptr [ebp - 0x48]
0x00447c23: d95df8                   fstp dword ptr [ebp - 8]
0x00447c26: 833d380f7f0000           cmp dword ptr [0x7f0f38], 0
0x00447c2d: 0f85b7020000             jne 0x447eea
0x00447c33: a130107f00               mov eax, dword ptr [0x7f1030]
0x00447c38: 33d2                     xor edx, edx
0x00447c3a: b960ea0000               mov ecx, 0xea60
0x00447c3f: f7f1                     div ecx
0x00447c41: 8945ac                   mov dword ptr [ebp - 0x54], eax
0x00447c44: d98548ffffff             fld dword ptr [ebp - 0xb8]
0x00447c4a: d805c0c95c00             fadd dword ptr [0x5cc9c0]
0x00447c50: d83558c35c00             fdiv dword ptr [0x5cc358]
0x00447c56: d99d28ffffff             fstp dword ptr [ebp - 0xd8]
0x00447c5c: db45ac                   fild dword ptr [ebp - 0x54]
0x00447c5f: d9954cffffff             fst dword ptr [ebp - 0xb4]
0x00447c65: d83540e05c00             fdiv dword ptr [0x5ce040]
0x00447c6b: d9954cffffff             fst dword ptr [ebp - 0xb4]
0x00447c71: e88aba0500               call 0x4a3700
0x00447c76: d9954cffffff             fst dword ptr [ebp - 0xb4]
0x00447c7c: d8355cc55c00             fdiv dword ptr [0x5cc55c]
0x00447c82: d9954cffffff             fst dword ptr [ebp - 0xb4]
0x00447c88: d88d28ffffff             fmul dword ptr [ebp - 0xd8]
0x00447c8e: d99d4cffffff             fstp dword ptr [ebp - 0xb4]
0x00447c94: d945f0                   fld dword ptr [ebp - 0x10]
0x00447c97: d8854cffffff             fadd dword ptr [ebp - 0xb4]
0x00447c9d: d95df0                   fstp dword ptr [ebp - 0x10]
0x00447ca0: db45ac                   fild dword ptr [ebp - 0x54]
0x00447ca3: d9954cffffff             fst dword ptr [ebp - 0xb4]
0x00447ca9: d835a8e15c00             fdiv dword ptr [0x5ce1a8]
0x00447caf: d9954cffffff             fst dword ptr [ebp - 0xb4]
0x00447cb5: e846ba0500               call 0x4a3700
0x00447cba: d9954cffffff             fst dword ptr [ebp - 0xb4]
0x00447cc0: d8355cc55c00             fdiv dword ptr [0x5cc55c]
0x00447cc6: d9954cffffff             fst dword ptr [ebp - 0xb4]
0x00447ccc: d88d28ffffff             fmul dword ptr [ebp - 0xd8]
0x00447cd2: d99d4cffffff             fstp dword ptr [ebp - 0xb4]
0x00447cd8: d945f8                   fld dword ptr [ebp - 8]
0x00447cdb: d8854cffffff             fadd dword ptr [ebp - 0xb4]
0x00447ce1: d95df8                   fstp dword ptr [ebp - 8]
0x00447ce4: db45ac                   fild dword ptr [ebp - 0x54]
0x00447ce7: d9954cffffff             fst dword ptr [ebp - 0xb4]
0x00447ced: d83544e05c00             fdiv dword ptr [0x5ce044]
0x00447cf3: d9954cffffff             fst dword ptr [ebp - 0xb4]
0x00447cf9: e802ba0500               call 0x4a3700
0x00447cfe: d9954cffffff             fst dword ptr [ebp - 0xb4]
0x00447d04: d83558c35c00             fdiv dword ptr [0x5cc358]
0x00447d0a: d9954cffffff             fst dword ptr [ebp - 0xb4]
0x00447d10: d88d28ffffff             fmul dword ptr [ebp - 0xd8]
0x00447d16: d99d4cffffff             fstp dword ptr [ebp - 0xb4]
0x00447d1c: d9850cffffff             fld dword ptr [ebp - 0xf4]
0x00447d22: d8854cffffff             fadd dword ptr [ebp - 0xb4]
0x00447d28: d99d0cffffff             fstp dword ptr [ebp - 0xf4]
0x00447d2e: db45ac                   fild dword ptr [ebp - 0x54]
0x00447d31: d9954cffffff             fst dword ptr [ebp - 0xb4]
0x00447d37: d835e8d95c00             fdiv dword ptr [0x5cd9e8]
0x00447d3d: d9954cffffff             fst dword ptr [ebp - 0xb4]
0x00447d43: e8b8b90500               call 0x4a3700
0x00447d48: d9954cffffff             fst dword ptr [ebp - 0xb4]
0x00447d4e: d83558c35c00             fdiv dword ptr [0x5cc358]
0x00447d54: d9954cffffff             fst dword ptr [ebp - 0xb4]
0x00447d5a: d88d28ffffff             fmul dword ptr [ebp - 0xd8]
0x00447d60: d99d4cffffff             fstp dword ptr [ebp - 0xb4]
0x00447d66: d98510ffffff             fld dword ptr [ebp - 0xf0]
0x00447d6c: d8854cffffff             fadd dword ptr [ebp - 0xb4]
0x00447d72: d99d10ffffff             fstp dword ptr [ebp - 0xf0]
0x00447d78: db45ac                   fild dword ptr [ebp - 0x54]
0x00447d7b: d9954cffffff             fst dword ptr [ebp - 0xb4]
0x00447d81: d835a4e15c00             fdiv dword ptr [0x5ce1a4]
0x00447d87: d9954cffffff             fst dword ptr [ebp - 0xb4]
0x00447d8d: e86eb90500               call 0x4a3700
0x00447d92: d9954cffffff             fst dword ptr [ebp - 0xb4]
0x00447d98: d83558c35c00             fdiv dword ptr [0x5cc358]
0x00447d9e: d9954cffffff             fst dword ptr [ebp - 0xb4]
0x00447da4: d88d28ffffff             fmul dword ptr [ebp - 0xd8]
0x00447daa: d99d4cffffff             fstp dword ptr [ebp - 0xb4]
0x00447db0: d98514ffffff             fld dword ptr [ebp - 0xec]
0x00447db6: d8854cffffff             fadd dword ptr [ebp - 0xb4]
0x00447dbc: d99d14ffffff             fstp dword ptr [ebp - 0xec]
0x00447dc2: db45ac                   fild dword ptr [ebp - 0x54]
0x00447dc5: d9954cffffff             fst dword ptr [ebp - 0xb4]
0x00447dcb: d835a0e15c00             fdiv dword ptr [0x5ce1a0]
0x00447dd1: d9954cffffff             fst dword ptr [ebp - 0xb4]
0x00447dd7: e824b90500               call 0x4a3700
0x00447ddc: d9954cffffff             fst dword ptr [ebp - 0xb4]
0x00447de2: d8355cc55c00             fdiv dword ptr [0x5cc55c]
0x00447de8: d9954cffffff             fst dword ptr [ebp - 0xb4]
0x00447dee: d88d28ffffff             fmul dword ptr [ebp - 0xd8]
0x00447df4: d99d4cffffff             fstp dword ptr [ebp - 0xb4]
0x00447dfa: d9850cffffff             fld dword ptr [ebp - 0xf4]
0x00447e00: d8854cffffff             fadd dword ptr [ebp - 0xb4]
0x00447e06: d99d0cffffff             fstp dword ptr [ebp - 0xf4]
0x00447e0c: db45ac                   fild dword ptr [ebp - 0x54]
0x00447e0f: d9954cffffff             fst dword ptr [ebp - 0xb4]
0x00447e15: d8359ce15c00             fdiv dword ptr [0x5ce19c]
0x00447e1b: d9954cffffff             fst dword ptr [ebp - 0xb4]
0x00447e21: e8dab80500               call 0x4a3700
0x00447e26: d9954cffffff             fst dword ptr [ebp - 0xb4]
0x00447e2c: d8355cc55c00             fdiv dword ptr [0x5cc55c]
0x00447e32: d9954cffffff             fst dword ptr [ebp - 0xb4]
0x00447e38: d88d28ffffff             fmul dword ptr [ebp - 0xd8]
0x00447e3e: d99d4cffffff             fstp dword ptr [ebp - 0xb4]
0x00447e44: d98514ffffff             fld dword ptr [ebp - 0xec]
0x00447e4a: d8854cffffff             fadd dword ptr [ebp - 0xb4]
0x00447e50: d99d14ffffff             fstp dword ptr [ebp - 0xec]
0x00447e56: db45ac                   fild dword ptr [ebp - 0x54]
0x00447e59: d9954cffffff             fst dword ptr [ebp - 0xb4]
0x00447e5f: d83598e15c00             fdiv dword ptr [0x5ce198]
0x00447e65: d9954cffffff             fst dword ptr [ebp - 0xb4]
0x00447e6b: e890b80500               call 0x4a3700
0x00447e70: d9954cffffff             fst dword ptr [ebp - 0xb4]
0x00447e76: d83594e15c00             fdiv dword ptr [0x5ce194]
0x00447e7c: d9954cffffff             fst dword ptr [ebp - 0xb4]
0x00447e82: d88d28ffffff             fmul dword ptr [ebp - 0xd8]
0x00447e88: d99d4cffffff             fstp dword ptr [ebp - 0xb4]
0x00447e8e: d9850cffffff             fld dword ptr [ebp - 0xf4]
0x00447e94: d8854cffffff             fadd dword ptr [ebp - 0xb4]
0x00447e9a: d99d0cffffff             fstp dword ptr [ebp - 0xf4]
0x00447ea0: db45ac                   fild dword ptr [ebp - 0x54]
0x00447ea3: d9954cffffff             fst dword ptr [ebp - 0xb4]
0x00447ea9: d83590e15c00             fdiv dword ptr [0x5ce190]
0x00447eaf: d9954cffffff             fst dword ptr [ebp - 0xb4]
0x00447eb5: e846b80500               call 0x4a3700
0x00447eba: d9954cffffff             fst dword ptr [ebp - 0xb4]
0x00447ec0: d83594e15c00             fdiv dword ptr [0x5ce194]
0x00447ec6: d9954cffffff             fst dword ptr [ebp - 0xb4]
0x00447ecc: d88d28ffffff             fmul dword ptr [ebp - 0xd8]
0x00447ed2: d99d4cffffff             fstp dword ptr [ebp - 0xb4]
0x00447ed8: d98514ffffff             fld dword ptr [ebp - 0xec]
0x00447ede: d8854cffffff             fadd dword ptr [ebp - 0xb4]
0x00447ee4: d99d14ffffff             fstp dword ptr [ebp - 0xec]
0x00447eea: 8b15c80f7f00             mov edx, dword ptr [0x7f0fc8]
0x00447ef0: 52                       push edx
0x00447ef1: d905c80f7f00             fld dword ptr [0x7f0fc8]
0x00447ef7: d9e0                     fchs 
0x00447ef9: 51                       push ecx
0x00447efa: d91c24                   fstp dword ptr [esp]
0x00447efd: e84ea70200               call 0x472650
0x00447f02: 83c408                   add esp, 8
0x00447f05: d845f0                   fadd dword ptr [ebp - 0x10]
0x00447f08: d95df0                   fstp dword ptr [ebp - 0x10]
0x00447f0b: a1c80f7f00               mov eax, dword ptr [0x7f0fc8]
0x00447f10: 50                       push eax
0x00447f11: d905c80f7f00             fld dword ptr [0x7f0fc8]
0x00447f17: d9e0                     fchs 
0x00447f19: 51                       push ecx
0x00447f1a: d91c24                   fstp dword ptr [esp]
0x00447f1d: e82ea70200               call 0x472650
0x00447f22: 83c408                   add esp, 8
0x00447f25: d845f4                   fadd dword ptr [ebp - 0xc]
0x00447f28: d95df4                   fstp dword ptr [ebp - 0xc]
0x00447f2b: 8b0dc80f7f00             mov ecx, dword ptr [0x7f0fc8]
0x00447f31: 51                       push ecx
0x00447f32: d905c80f7f00             fld dword ptr [0x7f0fc8]
0x00447f38: d9e0                     fchs 
0x00447f3a: 51                       push ecx
0x00447f3b: d91c24                   fstp dword ptr [esp]
0x00447f3e: e80da70200               call 0x472650
0x00447f43: 83c408                   add esp, 8
0x00447f46: d845f8                   fadd dword ptr [ebp - 8]
0x00447f49: d95df8                   fstp dword ptr [ebp - 8]
0x00447f4c: 8b55f0                   mov edx, dword ptr [ebp - 0x10]
0x00447f4f: 8955c4                   mov dword ptr [ebp - 0x3c], edx
0x00447f52: 8b45f4                   mov eax, dword ptr [ebp - 0xc]
0x00447f55: 8945c8                   mov dword ptr [ebp - 0x38], eax
0x00447f58: 8b4df8                   mov ecx, dword ptr [ebp - 8]
0x00447f5b: 898d44ffffff             mov dword ptr [ebp - 0xbc], ecx
0x00447f61: 8b5508                   mov edx, dword ptr [ebp + 8]
0x00447f64: 8b850cffffff             mov eax, dword ptr [ebp - 0xf4]
0x00447f6a: 894240                   mov dword ptr [edx + 0x40], eax
0x00447f6d: 8b4d08                   mov ecx, dword ptr [ebp + 8]
0x00447f70: 8b9510ffffff             mov edx, dword ptr [ebp - 0xf0]
0x00447f76: 895144                   mov dword ptr [ecx + 0x44], edx
0x00447f79: 8b4508                   mov eax, dword ptr [ebp + 8]
0x00447f7c: 8b8d14ffffff             mov ecx, dword ptr [ebp - 0xec]
0x00447f82: 894848                   mov dword ptr [eax + 0x48], ecx
0x00447f85: 8b5508                   mov edx, dword ptr [ebp + 8]
0x00447f88: 81c264090000             add edx, 0x964
0x00447f8e: 8b850cffffff             mov eax, dword ptr [ebp - 0xf4]
0x00447f94: 8902                     mov dword ptr [edx], eax
0x00447f96: 8b8d10ffffff             mov ecx, dword ptr [ebp - 0xf0]
0x00447f9c: 894a04                   mov dword ptr [edx + 4], ecx
0x00447f9f: 8b8514ffffff             mov eax, dword ptr [ebp - 0xec]
0x00447fa5: 894208                   mov dword ptr [edx + 8], eax
0x00447fa8: 8b4d08                   mov ecx, dword ptr [ebp + 8]
0x00447fab: 81c17c090000             add ecx, 0x97c
0x00447fb1: 8b55f0                   mov edx, dword ptr [ebp - 0x10]
0x00447fb4: 8911                     mov dword ptr [ecx], edx
0x00447fb6: 8b45f4                   mov eax, dword ptr [ebp - 0xc]
0x00447fb9: 894104                   mov dword ptr [ecx + 4], eax
0x00447fbc: 8b55f8                   mov edx, dword ptr [ebp - 8]
0x00447fbf: 895108                   mov dword ptr [ecx + 8], edx
0x00447fc2: 8b4508                   mov eax, dword ptr [ebp + 8]
0x00447fc5: d945c4                   fld dword ptr [ebp - 0x3c]
0x00447fc8: d86040                   fsub dword ptr [eax + 0x40]
0x00447fcb: d95dc4                   fstp dword ptr [ebp - 0x3c]
0x00447fce: 8b4d08                   mov ecx, dword ptr [ebp + 8]
0x00447fd1: d945c8                   fld dword ptr [ebp - 0x38]
0x00447fd4: d86144                   fsub dword ptr [ecx + 0x44]
0x00447fd7: d95dc8                   fstp dword ptr [ebp - 0x38]
0x00447fda: 8b5508                   mov edx, dword ptr [ebp + 8]
0x00447fdd: d98544ffffff             fld dword ptr [ebp - 0xbc]
0x00447fe3: d86248                   fsub dword ptr [edx + 0x48]
0x00447fe6: d99d44ffffff             fstp dword ptr [ebp - 0xbc]
0x00447fec: 8b4508                   mov eax, dword ptr [ebp + 8]
0x00447fef: 8b4dc4                   mov ecx, dword ptr [ebp - 0x3c]
0x00447ff2: 89484c                   mov dword ptr [eax + 0x4c], ecx
0x00447ff5: 8b5508                   mov edx, dword ptr [ebp + 8]
0x00447ff8: 8b45c8                   mov eax, dword ptr [ebp - 0x38]
0x00447ffb: 894250                   mov dword ptr [edx + 0x50], eax
0x00447ffe: 8b4d08                   mov ecx, dword ptr [ebp + 8]
0x00448001: 8b9544ffffff             mov edx, dword ptr [ebp - 0xbc]
0x00448007: 895154                   mov dword ptr [ecx + 0x54], edx
0x0044800a: 8b4508                   mov eax, dword ptr [ebp + 8]
0x0044800d: c740589a99193f           mov dword ptr [eax + 0x58], 0x3f19999a
0x00448014: d98544ffffff             fld dword ptr [ebp - 0xbc]
0x0044801a: d81d7c755d00             fcomp dword ptr [0x5d757c]
0x00448020: dfe0                     fnstsw ax
0x00448022: f6c444                   test ah, 0x44
0x00448025: 7b5b                     jnp 0x448082
0x00448027: d945c4                   fld dword ptr [ebp - 0x3c]
0x0044802a: d8b544ffffff             fdiv dword ptr [ebp - 0xbc]
0x00448030: d99550ffffff             fst dword ptr [ebp - 0xb0]
0x00448036: e8e5b50500               call 0x4a3620
0x0044803b: dc0de0ca5c00             fmul qword ptr [0x5ccae0]
0x00448041: d9e0                     fchs 
0x00448043: d99d50ffffff             fstp dword ptr [ebp - 0xb0]
0x00448049: d9059cd05c00             fld dword ptr [0x5cd09c]
0x0044804f: d8a550ffffff             fsub dword ptr [ebp - 0xb0]
0x00448055: d99d50ffffff             fstp dword ptr [ebp - 0xb0]
0x0044805b: d98544ffffff             fld dword ptr [ebp - 0xbc]
0x00448061: d81d7c755d00             fcomp dword ptr [0x5d757c]
0x00448067: dfe0                     fnstsw ax
0x00448069: f6c441                   test ah, 0x41
0x0044806c: 7512                     jne 0x448080
0x0044806e: d9059cd05c00             fld dword ptr [0x5cd09c]
0x00448074: d88550ffffff             fadd dword ptr [ebp - 0xb0]
0x0044807a: d99d50ffffff             fstp dword ptr [ebp - 0xb0]
0x00448080: eb26                     jmp 0x4480a8
0x00448082: d945c4                   fld dword ptr [ebp - 0x3c]
0x00448085: d81d7c755d00             fcomp dword ptr [0x5d757c]
0x0044808b: dfe0                     fnstsw ax
0x0044808d: f6c405                   test ah, 5
0x00448090: 7a0c                     jp 0x44809e
0x00448092: c78550ffffff00008743     mov dword ptr [ebp - 0xb0], 0x43870000
0x0044809c: eb0a                     jmp 0x4480a8
0x0044809e: c78550ffffff0000b442     mov dword ptr [ebp - 0xb0], 0x42b40000
0x004480a8: 8b4d08                   mov ecx, dword ptr [ebp + 8]
0x004480ab: 8b9550ffffff             mov edx, dword ptr [ebp - 0xb0]
0x004480b1: 895138                   mov dword ptr [ecx + 0x38], edx
0x004480b4: d945c4                   fld dword ptr [ebp - 0x3c]
0x004480b7: d84dc4                   fmul dword ptr [ebp - 0x3c]
0x004480ba: d95dc4                   fstp dword ptr [ebp - 0x3c]
0x004480bd: d98544ffffff             fld dword ptr [ebp - 0xbc]
0x004480c3: d88d44ffffff             fmul dword ptr [ebp - 0xbc]
0x004480c9: d99544ffffff             fst dword ptr [ebp - 0xbc]
0x004480cf: d845c4                   fadd dword ptr [ebp - 0x3c]
0x004480d2: d99544ffffff             fst dword ptr [ebp - 0xbc]
0x004480d8: d81d7c755d00             fcomp dword ptr [0x5d757c]
0x004480de: dfe0                     fnstsw ax
0x004480e0: f6c444                   test ah, 0x44
0x004480e3: 7b15                     jnp 0x4480fa
0x004480e5: 8b8544ffffff             mov eax, dword ptr [ebp - 0xbc]
0x004480eb: 50                       push eax
0x004480ec: e83fba0700               call 0x4c3b30
0x004480f1: 83c404                   add esp, 4
0x004480f4: d99d44ffffff             fstp dword ptr [ebp - 0xbc]
0x004480fa: d98544ffffff             fld dword ptr [ebp - 0xbc]
0x00448100: d81d7c755d00             fcomp dword ptr [0x5d757c]
0x00448106: dfe0                     fnstsw ax
0x00448108: f6c444                   test ah, 0x44
0x0044810b: 7b5b                     jnp 0x448168
0x0044810d: d945c8                   fld dword ptr [ebp - 0x38]
0x00448110: d8b544ffffff             fdiv dword ptr [ebp - 0xbc]
0x00448116: d99550ffffff             fst dword ptr [ebp - 0xb0]
0x0044811c: e8ffb40500               call 0x4a3620
0x00448121: dc0de0ca5c00             fmul qword ptr [0x5ccae0]
0x00448127: d9e0                     fchs 
0x00448129: d99d50ffffff             fstp dword ptr [ebp - 0xb0]
0x0044812f: d9059cd05c00             fld dword ptr [0x5cd09c]
0x00448135: d8a550ffffff             fsub dword ptr [ebp - 0xb0]
0x0044813b: d99d50ffffff             fstp dword ptr [ebp - 0xb0]
0x00448141: d98544ffffff             fld dword ptr [ebp - 0xbc]
0x00448147: d81d7c755d00             fcomp dword ptr [0x5d757c]
0x0044814d: dfe0                     fnstsw ax
0x0044814f: f6c441                   test ah, 0x41
0x00448152: 7512                     jne 0x448166
0x00448154: d9059cd05c00             fld dword ptr [0x5cd09c]
0x0044815a: d88550ffffff             fadd dword ptr [ebp - 0xb0]
0x00448160: d99d50ffffff             fstp dword ptr [ebp - 0xb0]
0x00448166: eb26                     jmp 0x44818e
0x00448168: d945c8                   fld dword ptr [ebp - 0x38]
0x0044816b: d81d7c755d00             fcomp dword ptr [0x5d757c]
0x00448171: dfe0                     fnstsw ax
0x00448173: f6c405                   test ah, 5
0x00448176: 7a0c                     jp 0x448184
0x00448178: c78550ffffff00008743     mov dword ptr [ebp - 0xb0], 0x43870000
0x00448182: eb0a                     jmp 0x44818e
0x00448184: c78550ffffff0000b442     mov dword ptr [ebp - 0xb0], 0x42b40000
0x0044818e: d905c4ca5c00             fld dword ptr [0x5ccac4]
0x00448194: d8a550ffffff             fsub dword ptr [ebp - 0xb0]
0x0044819a: 8b4d08                   mov ecx, dword ptr [ebp + 8]
0x0044819d: d95934                   fstp dword ptr [ecx + 0x34]
0x004481a0: 8b5508                   mov edx, dword ptr [ebp + 8]
0x004481a3: c7423c00000000           mov dword ptr [edx + 0x3c], 0
0x004481aa: 833d380f7f0000           cmp dword ptr [0x7f0f38], 0
0x004481b1: 740a                     je 0x4481bd
0x004481b3: 8b4508                   mov eax, dword ptr [ebp + 8]
0x004481b6: c7403800000000           mov dword ptr [eax + 0x38], 0
0x004481bd: 8b4d08                   mov ecx, dword ptr [ebp + 8]
0x004481c0: 51                       push ecx
0x004481c1: e89a95ffff               call 0x441760
0x004481c6: 83c404                   add esp, 4
0x004481c9: 8b5508                   mov edx, dword ptr [ebp + 8]
0x004481cc: 83c240                   add edx, 0x40
0x004481cf: 8b4508                   mov eax, dword ptr [ebp + 8]
0x004481d2: 83c004                   add eax, 4
0x004481d5: 8b0a                     mov ecx, dword ptr [edx]
0x004481d7: 8908                     mov dword ptr [eax], ecx
0x004481d9: 8b4a04                   mov ecx, dword ptr [edx + 4]
0x004481dc: 894804                   mov dword ptr [eax + 4], ecx
0x004481df: 8b5208                   mov edx, dword ptr [edx + 8]
0x004481e2: 895008                   mov dword ptr [eax + 8], edx
0x004481e5: 8b4508                   mov eax, dword ptr [ebp + 8]
0x004481e8: 83c010                   add eax, 0x10
0x004481eb: 8b4df0                   mov ecx, dword ptr [ebp - 0x10]
0x004481ee: 8908                     mov dword ptr [eax], ecx
0x004481f0: 8b55f4                   mov edx, dword ptr [ebp - 0xc]
0x004481f3: 895004                   mov dword ptr [eax + 4], edx
0x004481f6: 8b4df8                   mov ecx, dword ptr [ebp - 8]
0x004481f9: 894808                   mov dword ptr [eax + 8], ecx
0x004481fc: 8b5508                   mov edx, dword ptr [ebp + 8]
0x004481ff: 833a00                   cmp dword ptr [edx], 0
0x00448202: 740c                     je 0x448210
0x00448204: 8b4508                   mov eax, dword ptr [ebp + 8]
0x00448207: 50                       push eax
0x00448208: e813a8ffff               call 0x442a20
0x0044820d: 83c404                   add esp, 4
0x00448210: 8be5                     mov esp, ebp
0x00448212: 5d                       pop ebp
