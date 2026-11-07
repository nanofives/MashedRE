| rva | size | best % | regs % | class | residual |
|---|---|---|---|---|---|
| 0x005bbfc0 | 88 | 94.1 | 94.1 | a | lea edx,[esp] scheduled before mov [esp],0 in original; pp temp/order/permuter no change |
| 0x005bbf60 | 96 | 92.3 | 94.9 | a | load order of the two setne args; locals and permuter no gain |
| 0x005bc920 | 387 | 91.9 | 91.9 | b | permuter 85.4->91.9 (ops 98.4%) |
| 0x005b4150 | 145 | 90.0 | 90.0 | b | fidiv/fimul dword on zero-extended temp vs fild qword+fdivp; FTOL_INLINE asm macro used |
| 0x005b4060 | 226 | 88.4 | 88.4 | b | FTOI __asm macro for inline fistp; original does fidiv/fimul dword via zero-high temp for the uint divisor/multiplier, compiler gives fild q |
| 0x005b7110 | 279 | 88.1 | 94.0 | b | missing mov ecx,ebx copy of incremented index before the stride compare (83 vs 84 ins); compare/row-address forms + permuter no change |
| 0x005b6710 | 79 | 87.5 | 87.5 | a | byte zero-extend via stack slot instead of mov eax,ecx; permuter no gain |
| 0x005b66c0 | 73 | 87.1 | 87.1 | b | original hoists xor edx,edx and returns it; if/switch/r=0/q-q forms keep xor eax,eax at the return |
| 0x005b3a00 | 291 | 84.7 | 86.4 | a | S7 struct copies for rep movsd; original pushes ebp/edi before first call |
| 0x005baa60 | 404 | 83.7 | 85.3 | a | x87 operand order in the float*const products and path-3 store schedule; 99.2% ops |
| 0x005b2080 | 266 | 82.6 | 82.6 | a | volatile fields (58->82.6); short compare operand order and push edi placement remain |
| 0x005baf00 | 62 | 82.6 | 82.6 | b | float copies compile as int moves, permuter swap won 66.7->82.6 |
| 0x005b73b0 | 38 | 82.4 | 100.0 | a | param copy lands in eax not ecx; 6 loop spellings plus permuter unchanged |
| 0x005b3620 | 77 | 82.1 | 89.7 | a | original loads param_1 first, lea ebx,[eax+4]; typed struct/do/for/while/permuter no change |
| 0x005bbed0 | 89 | 80.5 | 80.5 | a | original keeps first return 0 epilogue unmerged; compiler tail-merges; flags/permuter no change |
| 0x005b2de0 | 142 | 80.4 | 93.5 | b | original tests flags from memory and reloads for the or (no CSE); volatile/int cast//=/permuter no change; 5b2fd0 really 5 args, 5ab1c0 4 ar |
| 0x005b3670 | 50 | 75.0 | 79.2 | a | head ptr materialised before first load; permuter no gain |
| 0x005bcda0 | 126 | 73.7 | 73.7 | a | original keeps duplicated release tails (57 vs 48 ins); single-exit form is best |
| 0x005b29e0 | 277 | 73.3 | 90.0 | b | compile fix + permuter; range-compare order and loop temp registers |
| 0x005bc890 | 102 | 72.0 | 78.0 | a | callee-saved pushes before first call (no shrink-wrap) in original |
| 0x005bce80 | 95 | 69.6 | 87.0 | b | 0x7b8 kept in eax across two [+0x40] stores not reproduced (local, int* index, permuter) |
| 0x005bba60 | 87 | 64.9 | 100.0 | a | original pushes d before computing [esi+0x4c]/lea; arg schedule + reg choice not reproduced |
| 0x005bbac0 | 86 | 64.9 | 97.3 | a | arg loads hoisted above the branch in original; 3 shapes plus permuter |
| 0x005bcd80 | 28 | 64.7 | 64.7 | a | top-tested loop (jmp back) always rotated by the compiler; while/for/goto/continue, /O1 /Oy- /Os /Ox, permuter tried |
| 0x005b1260 | 663 | 63.0 | 69.2 | b | compile fix (stray s_rws;1 line) + permuter 61.1->63.0 |
| 0x005b7980 | 264 | 62.5 | 65.9 | b | switch + q=p+0x5c base found (14.8->62.5); case-2 block register/branch layout not found |
| 0x005bcbe0 | 144 | 62.3 | 66.0 | b | permuter 56.6->62.3 |
| 0x005b2190 | 139 | 61.0 | 63.4 | b | raw kept (permuter no gain) |
| 0x005b35a0 | 51 | 60.0 | 90.0 | a | eax/edx roles of param_1 and param_1+4 swapped; permuter 55->60 |
| 0x005bac00 | 294 | 59.0 | 67.0 | b | rewritten (12-dword block, 3 vtable calls); compiler hoists self+8 into esi/edi |
| 0x005bcab0 | 198 | 58.7 | 66.7 | b | permuter 49.2->58.7 (ops 90.5%) |
| 0x005b3760 | 668 | 57.1 | 64.1 | b | permuter 49.4->57.1 |
| 0x005b6910 | 296 | 54.4 | 68.9 | b | permuter 50.5->54.4 |
| 0x005b3300 | 354 | 53.8 | 53.8 | b | LPLONG compile fix + permuter 41.7->53.8 |
| 0x005b3020 | 177 | 53.5 | 69.0 | b | permuter 35.2->53.5 |
| 0x005b36b0 | 57 | 53.1 | 68.8 | b | permuter 41.4->53.1; operand/register order not found |
| 0x005b51d0 | 476 | 50.3 | 55.4 | b | permuter 47.8->50.3; large, structure not found |
| 0x005b1180 | 67 | 46.4 | 46.4 | b | byte-pair regs plus inner push ebx; /G5 spills, flags /O2 /Ob1 best |
| 0x005bbd50 | 96 | 45.7 | 48.6 | b | permuter 27.8->45.7; /GS; vtable call with out-param |
| 0x005bbc10 | 306 | 44.4 | 53.0 | b | Blk12 offsets, 6 params; store interleave of zero-init and field stores plus epilogue layout not reproduced |
| 0x005bc690 | 186 | 43.8 | 67.4 | b | original hoists param loads and puts the error return inline after jge; layout/push scheduling not reproduced |
| 0x005b3e60 | 500 | 43.6 | 50.9 | b | permuter 26.6->43.6; /GS |
| 0x005b3b80 | 726 | 43.4 | 53.0 | b | stack-aliased local_38[(int)param_1-4] kept; S7 struct copies and memcpy intrinsic done, permuter 300 s |
| 0x005bb000 | 883 | 42.7 | 48.8 | b | compile fix (IID typedef) 0->36.8, permuter ->42.7 |
| 0x005b2820 | 439 | 42.6 | 32.0 | b | fields reloaded each use (volatile approximates), flag reg allocation differs |
| 0x005b2b00 | 736 | 42.3 | 67.8 | b | compile fixed (LPLONG), struct form; original hoists *(f118+0x20) and keeps a stack local, reg alloc/ordering not reproduced (budget) |
| 0x005b6a40 | 191 | 40.8 | 85.9 | b | nibble via param slot done; diff/nibble eax-edx role swap remains; file written from odis+g2s |
| 0x005b6820 | 230 | 39.8 | 53.0 | b | permuter 34.9->39.8; /GS |
| 0x005ba1d0 | 1351 | 33.8 | 41.6 | b | raw body kept, 402 vs 461 ins, no structural pass (budget) |
| 0x005bad30 | 459 | 33.8 | 21.8 | c | unaff_EBP / stack0xffffff70 pseudo-variables, boundary/convention wrong |
| 0x005bc050 | 160 | 32.4 | 29.6 | b | permuter 19.7->32.4; structure not found |
| 0x005b6be0 | 611 | 31.6 | 35.1 | b | 102 vs 171 ins, not iterated |
| 0x005b92e0 | 64 | 31.6 | 89.5 | b | tail call + volatile store done, param register assignment (eax/ecx vs ecx/edx) not found |
| 0x005b9f30 | 610 | 31.6 | 35.1 | b | macro that re-calls FUN_005aee20(0x28) at each use not reproduced (110 vs 171 ins); no structural pass (budget) |
| 0x005b35e0 | 58 | 30.8 | 69.2 | b | intrusive-list find loop + unlink; loop/register order not found (flags, cond order, while form, permuter) |
| 0x005bd6f0 | 748 | 30.7 | 39.4 | b | raw body kept, 221 vs 254 ins, no structural pass (budget) |
| 0x005b74c0 | 611 | 30.4 | 33.9 | b | permuter 29.2->30.4 |
| 0x005b8c40 | 1676 | 30.0 | 31.0 | b | permuter 21.3->30.0 |
| 0x005bc470 | 458 | 29.7 | 56.5 | b | rebuilt from asm (old file did not compile); store interleave of W/Desc init and register reuse not reproduced |
| 0x005b6b00 | 123 | 26.8 | 51.2 | b | permuter 22.0->26.8; structure not found |
| 0x005b5860 | 471 | 25.3 | 41.6 | b | raw body kept, 178 vs 160 ins, no structural pass (budget) |
| 0x005bc0f0 | 160 | 23.3 | 26.0 | b | decomp wrong (8-arg stdcall vtable call plus two variable memcpy), not rewritten |
| 0x005bc190 | 611 | 21.3 | 33.7 | b | 291 vs 199 ins, not iterated |
| 0x005bb5b0 | 382 | 20.9 | 35.1 | b | permuter 17.9->20.9; structure not found |
| 0x005ba7f0 | 591 | 18.9 | 38.2 | b | 285 vs 222 ins, not iterated |
| 0x005bbdb0 | 287 | 18.8 | 25.9 | b | 38 vs 85 ins, decomp drops vtable-call args, not rewritten |
| 0x005b5f70 | 741 | 18.3 | 34.4 | b | raw body kept, 260 vs 215 ins, no structural pass (budget) |
| 0x005b2220 | 1512 | 18.0 | 40.9 | b | compile fixed (LPLONG); raw body, frame 0x98 vs 0x80, no structural pass (budget) |
| 0x005b6260 | 211 | 17.3 | 37.0 | b | raw body kept; hand forms 8-14%; permuter 42% was semantics-changing (rejected); memory-heavy frame/reg alloc not found |
| 0x005b53b0 | 1189 | 17.2 | 29.2 | b | permuter 9.5->17.2; /GS; ROUND/ABS are C++-only helpers |
| 0x005b30e0 | 540 | 16.7 | 40.6 | b | raw Ghidra shape, 64-bit div form and loop not iterated (budget to small rows) |
| 0x005b5e00 | 359 | 16.0 | 36.8 | b | raw Ghidra shape with CONCAT11, not iterated |
| 0x005b4850 | 1405 | 15.2 | 34.3 | b | ROUND calls, same inline truncation note, not iterated |
| 0x005b41f0 | 1019 | 13.7 | 34.0 | b | ROUND calls; original inlines fnstcw/fistp as in 0x005b4150, not iterated |
