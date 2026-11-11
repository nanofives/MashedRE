# Tables from measure_v4.tsv and measure_v4_relax.tsv

**Verdicts, all REFUSED rows with a source**

| | rows |
|---|---|
| NOT-PROVEN | 912 |
| EQUIV | 107 |
| EQUIV-IF-CALLERS-IGNORE | 61 |
| EQUIV (byte-identical today) | 26 |
| total | 1106 |

**Verdicts, rows at 95% or more**

| | rows |
|---|---|
| NOT-PROVEN | 74 |
| EQUIV | 34 |
| EQUIV (byte-identical today) | 19 |
| total | 127 |

**Verdicts with --relax-icall (A4': calls through a pointer read no register)**

| | rows |
|---|---|
| NOT-PROVEN | 900 |
| EQUIV | 105 |
| EQUIV-IF-CALLERS-IGNORE | 76 |
| EQUIV (byte-identical today) | 25 |
| total | 1106 |

**Class sets of the proven rows (EQUIV and EQUIV-IF-CALLERS-IGNORE)**

| | rows |
|---|---|
| EQUIV [none] | 26 |
| EQUIV [LAYOUT] | 15 |
| EQUIV [VALUE-FLOW] | 14 |
| EQUIV-IF-CALLERS-IGNORE [VALUE-FLOW] | 14 |
| EQUIV [ORDER,FRAME] | 10 |
| EQUIV [FRAME,VALUE-FLOW] | 10 |
| EQUIV-IF-CALLERS-IGNORE [LAYOUT,VALUE-FLOW] | 9 |
| EQUIV [LAYOUT,VALUE-FLOW] | 8 |
| EQUIV [ORDER] | 5 |
| EQUIV-IF-CALLERS-IGNORE [LAYOUT,FRAME,VALUE-FLOW] | 5 |
| EQUIV [ENC,LAYOUT] | 4 |
| EQUIV-IF-CALLERS-IGNORE [REG,LAYOUT,VALUE-FLOW] | 4 |
| EQUIV [LAYOUT,FRAME,VALUE-FLOW] | 4 |
| EQUIV [REG,VALUE-FLOW] | 4 |
| EQUIV [ENC,LAYOUT,VALUE-FLOW] | 3 |
| EQUIV [ORDER,FRAME,VALUE-FLOW] | 3 |
| EQUIV [SIB] | 3 |
| EQUIV-IF-CALLERS-IGNORE [REG,LAYOUT,FRAME,VALUE-FLOW] | 3 |
| EQUIV-IF-CALLERS-IGNORE [LAYOUT] | 3 |
| EQUIV-IF-CALLERS-IGNORE [FRAME,VALUE-FLOW] | 3 |
| EQUIV-IF-CALLERS-IGNORE [REG,VALUE-FLOW] | 3 |
| EQUIV [ORDER,LAYOUT,FRAME,VALUE-FLOW] | 3 |
| EQUIV-IF-CALLERS-IGNORE [ORDER,LAYOUT,FRAME,VALUE-FLOW] | 3 |
| EQUIV-IF-CALLERS-IGNORE [ORDER,LAYOUT,VALUE-FLOW] | 2 |
| EQUIV [ENC,VALUE-FLOW] | 2 |
| EQUIV [ENC] | 2 |
| EQUIV-IF-CALLERS-IGNORE [REG,FRAME,VALUE-FLOW] | 2 |
| EQUIV [REG] | 2 |
| EQUIV [ORDER,LAYOUT] | 2 |
| EQUIV [ORDER,LAYOUT,FRAME] | 2 |
| EQUIV [ENC,SIB,ORDER,VALUE-FLOW] | 1 |
| EQUIV [ENC,ORDER,LAYOUT,FRAME,VALUE-FLOW] | 1 |
| EQUIV-IF-CALLERS-IGNORE [ENC,FRAME,VALUE-FLOW] | 1 |
| EQUIV [LAYOUT,FRAME] | 1 |
| EQUIV [SIB,VALUE-FLOW] | 1 |
| EQUIV [ENC,LAYOUT,FRAME,VALUE-FLOW] | 1 |
| EQUIV [SIB,ORDER] | 1 |
| EQUIV-IF-CALLERS-IGNORE [ORDER,VALUE-FLOW] | 1 |
| EQUIV-IF-CALLERS-IGNORE [ORDER,FRAME,VALUE-FLOW] | 1 |
| EQUIV-IF-CALLERS-IGNORE [ENC,LAYOUT,VALUE-FLOW] | 1 |
| EQUIV [ENC,ORDER,FRAME,VALUE-FLOW] | 1 |
| EQUIV-IF-CALLERS-IGNORE [SIB,REG,LAYOUT,FRAME,VALUE-FLOW] | 1 |
| EQUIV-IF-CALLERS-IGNORE [ORDER,LAYOUT] | 1 |
| EQUIV [REG,FRAME,VALUE-FLOW] | 1 |
| EQUIV [ORDER,LAYOUT,VALUE-FLOW] | 1 |
| EQUIV [REG,LAYOUT,VALUE-FLOW] | 1 |
| EQUIV-IF-CALLERS-IGNORE [LAYOUT,FRAME] | 1 |
| EQUIV-IF-CALLERS-IGNORE [ENC,REG,FRAME,VALUE-FLOW] | 1 |
| EQUIV [SIB,REG,LAYOUT] | 1 |
| EQUIV-IF-CALLERS-IGNORE [REG,ORDER,LAYOUT,VALUE-FLOW] | 1 |
| EQUIV-IF-CALLERS-IGNORE [ENC,LAYOUT,FRAME,VALUE-FLOW] | 1 |

**Proven rows that use each class (a row counts once per class)**

| | rows |
|---|---|
| VALUE-FLOW | 115 |
| LAYOUT | 82 |
| FRAME | 59 |
| ORDER | 39 |
| (byte-identical) | 26 |
| REG | 24 |
| ENC | 19 |
| SIB | 8 |

**EQUIV-IF-CALLERS-IGNORE by register set and caller scan**

| | rows |
|---|---|
| edx,ecx / callers all ignore: False | 12 |
| ecx / callers all ignore: False | 10 |
| eax / callers all ignore: False | 10 |
| eax / callers all ignore: True | 8 |
| eax,ecx / callers all ignore: False | 6 |
| ecx / callers all ignore: True | 5 |
| edx,ecx / callers all ignore: True | 3 |
| edx / callers all ignore: False | 2 |
| edx / callers all ignore: True | 1 |
| eax,edx / callers all ignore: False | 1 |
| eax,edx,ecx / callers all ignore: True | 1 |
| eax,edx,ecx / callers all ignore: False | 1 |
| eax,ecx / callers all ignore: True | 1 |

**NOT-PROVEN by first unexplained difference**

| | rows |
|---|---|
| x87-region | 238 |
| branch-condition | 164 |
| call-args | 113 |
| call-regargs | 103 |
| call-memory | 61 |
| unsupported | 56 |
| call-sequence | 32 |
| esp-unknown | 25 |
| call-target | 20 |
| switch-table | 17 |
| ret-memory | 17 |
| indirect-call-esp-reset | 11 |
| unresolved-callee | 10 |
| ret-esp | 8 |
| switch-index | 6 |
| timeout | 5 |
| no-annotation | 5 |
| callee-pop-unknown | 4 |
| call-x87 | 3 |
| loop-without-branch | 3 |
| indirect-jump | 3 |
| no-size | 3 |
| cfg-mismatch | 3 |
| ret-address | 2 |

**NOT-PROVEN by first unexplained difference, rows at 95% or more**

| | rows |
|---|---|
| x87-region | 36 |
| call-memory | 8 |
| unsupported | 8 |
| branch-condition | 3 |
| switch-index | 3 |
| ret-esp | 3 |
| switch-table | 2 |
| call-regargs | 2 |
| unresolved-callee | 1 |
| call-sequence | 1 |
| loop-without-branch | 1 |
| timeout | 1 |
| indirect-call-esp-reset | 1 |
| callee-pop-unknown | 1 |
| esp-unknown | 1 |
| ret-memory | 1 |
| call-target | 1 |

**unsupported, by construct**

| | rows |
|---|---|
| orig conditional jump out of the function at | 13 |
| rep movsd with a variable count | 13 |
| rep stosd with a variable count | 9 |
| instruction pushf | 4 |
| instruction repne scasb al, byte ptr es:[edi] | 4 |
| rep stosd count 2048 | 2 |
| instruction xorps xmm0, xmm0 | 2 |
| rep movsd count 1376 | 1 |
| rep stosd count 56553 | 1 |
| rep movsd count 32768 | 1 |
| instruction stmxcsr dword ptr [esp + 4] | 1 |
| instruction repe cmpsb byte ptr [esi], byte ptr es:[edi] | 1 |
| instruction movaps xmm3, xmmword ptr [ebx] | 1 |
| instruction movss xmm0, dword ptr [esi] | 1 |
| rep stosw with a variable count | 1 |
| instruction movss xmm1, dword ptr [ecx] | 1 |

## Top-down path: the 326 C0..C3 game functions in reach (blocking_c0c3.tsv: origtop_size.py --order today, class game-c0c3)

| | functions |
|---|---|
| NOT-PROVEN | 254 |
| no match_ledger row (no source) | 33 |
| EQUIV | 32 |
| EQUIV-IF-CALLERS-IGNORE | 7 |
| total | 326 |

## BINDINGS.tsv stand-in and under-stand-in rows (91)

| kind | verdict | rows |
|---|---|---|
| stand-in | EQUIV-IF-CALLERS-IGNORE | 1 |
| stand-in | NOT-PROVEN | 6 |
| stand-in | ledger PASS | 51 |
| stand-in | no match_ledger row | 1 |
| under-stand-in | NOT-PROVEN | 4 |
| under-stand-in | ledger PASS | 27 |
| under-stand-in | no match_ledger row | 1 |

## Released functions on the top-down path

| RVA | name | verdict | classes | regs / callers ignore |
|---|---|---|---|---|
| `0x00407be0` | FUN_00407be0 | EQUIV | ENC,LAYOUT,VALUE-FLOW |   |
| `0x00408b00` | FUN_00408b00 | EQUIV | ENC,SIB,ORDER,VALUE-FLOW |   |
| `0x0040b290` | FUN_0040b290 | EQUIV | VALUE-FLOW |   |
| `0x00417180` | FUN_00417180 | EQUIV | ORDER,FRAME,VALUE-FLOW |   |
| `0x00417450` | FUN_00417450 | EQUIV | ENC,LAYOUT,VALUE-FLOW |   |
| `0x0041f770` | FUN_0041f770 | EQUIV | ENC,LAYOUT |   |
| `0x00429310` | TimeTrial::Tick | EQUIV | VALUE-FLOW |   |
| `0x00429bd0` | FUN_00429bd0 | EQUIV | ORDER,FRAME |   |
| `0x0042aa00` | FUN_0042aa00 | EQUIV | ENC,ORDER,LAYOUT,FRAME,VALUE-FLOW |   |
| `0x00430670` | MenuOrdinalToPlayerSlot | EQUIV-IF-CALLERS-IGNORE | REG,LAYOUT,VALUE-FLOW | ecx False |
| `0x004332a0` | FUN_004332a0 | EQUIV | LAYOUT,VALUE-FLOW |   |
| `0x00448220` | Frontend::PostRaceResultCamera | EQUIV | LAYOUT,VALUE-FLOW |   |
| `0x0045c480` | FUN_0045c480 | EQUIV | FRAME,VALUE-FLOW |   |
| `0x00475a00` | FUN_00475a00 | EQUIV | SIB |   |
| `0x0047cea0` | FUN_0047cea0 | EQUIV | SIB,VALUE-FLOW |   |
| `0x00484de0` | FUN_00484de0 | EQUIV | ORDER |   |
| `0x00485b30` | PoolRemove485b30 | EQUIV | ENC,LAYOUT,FRAME,VALUE-FLOW |   |
| `0x0048a4e0` | FUN_0048a4e0 | EQUIV | VALUE-FLOW |   |
| `0x00490020` | FUN_00490020 | EQUIV-IF-CALLERS-IGNORE | ORDER,LAYOUT,VALUE-FLOW | edx,ecx False |
| `0x004938e0` | FUN_004938e0 | EQUIV | ENC,VALUE-FLOW |   |
| `0x00498ea0` | FUN_00498ea0 | EQUIV | SIB,ORDER |   |
| `0x004b61c0` | FUN_004b61c0 | EQUIV | ENC |   |
| `0x004c18c0` | FUN_004c18c0 | EQUIV | LAYOUT |   |
| `0x004d4f00` | FUN_004d4f00 | EQUIV | ORDER |   |
| `0x004d8480` | RwErrSlotWrite | EQUIV-IF-CALLERS-IGNORE | ORDER,FRAME,VALUE-FLOW | edx,ecx False |
| `0x00555360` | FUN_00555360 | EQUIV-IF-CALLERS-IGNORE | ORDER,LAYOUT,VALUE-FLOW | eax True |
| `0x005a6b70` | FUN_005a6b70 | EQUIV | ORDER,LAYOUT,VALUE-FLOW |   |
| `0x005a8890` | FUN_005a8890 | EQUIV | REG,LAYOUT,VALUE-FLOW |   |
| `0x005aa7a0` | FUN_005aa7a0 | EQUIV | LAYOUT |   |
| `0x005aa7e0` | FUN_005aa7e0 | EQUIV-IF-CALLERS-IGNORE | VALUE-FLOW | edx,ecx False |
| `0x005aaac0` | FUN_005aaac0 | EQUIV | ORDER,LAYOUT,FRAME,VALUE-FLOW |   |
| `0x005aab00` | FUN_005aab00 | EQUIV | VALUE-FLOW |   |
| `0x005ab410` | AudioRwsChunkTypeSeek | EQUIV | LAYOUT,VALUE-FLOW |   |
| `0x005acd10` | AudioFmtTableSearch | EQUIV-IF-CALLERS-IGNORE | VALUE-FLOW | eax,ecx False |
| `0x005ace70` | FUN_005ace70 | EQUIV | LAYOUT |   |
| `0x005ad320` | FUN_005ad320 | EQUIV | FRAME,VALUE-FLOW |   |
| `0x005ae3a0` | FUN_005ae3a0 | EQUIV | VALUE-FLOW |   |
| `0x005ae920` | AudioPoolFree | EQUIV | SIB |   |
| `0x005b0cf0` | FUN_005b0cf0 | EQUIV-IF-CALLERS-IGNORE | REG,LAYOUT,FRAME,VALUE-FLOW | edx,ecx False |

## Released BINDINGS stand-in rows

| RVA | name | kind | verdict | classes | regs / callers ignore |
|---|---|---|---|---|---|
| `0x004d8480` | RwErrSlotWrite | stand-in | EQUIV-IF-CALLERS-IGNORE | ORDER,FRAME,VALUE-FLOW | edx,ecx False |
