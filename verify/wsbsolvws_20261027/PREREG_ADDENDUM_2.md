# PREREG D4-WSB-C4-SOLV-WS, addendum 2 (committed before arm D)

Leg R, Quick Battle, `0x0056ba30`: **HOLD BEHAVIOR-DIFF**, 6/6 calls each side, 1 signature each:

- OFF `[c56b7a0,A0+0,h0,1,h1] [c568990,h2,h3,0,h4,258] eax=0`
- ON  `[c56b7a0,A0+0,h0,1,A0+118] [c568990,h1,h2,0,h3,258] eax=0`

The only difference is the 4th argument of the call to `0x0056b7a0`. Both sides compute it the same way: original
`mov edx,[esi+0x70]; mov eax,[edx]; mov edx,[eax]; mov eax,[edx+0xc018]; push eax` (`0x0056ba68..0x0056ba78`), port
`mov eax,[esi+0x70]; mov eax,[eax]; mov eax,[eax]; push [eax+0xc018]` (asi `0x10099d16..0x10099d1d`), other 3 pushes the
same. The tool labels a value `A<k>+off` only when it lies in `[arg_k, arg_k + 0x4000)` and otherwise gives it a heap
ordinal (`c4_racediff_v7.py`, `nrm`). So on ON the value is `param_1 + 0x118`, and on OFF it is not within 0x4000 above
`param_1`. Two readings fit and the labels cannot tell them apart:
(a) heap layout differs between the OFF and ON boots (a placement artifact of the label), or
(b) the value differs between the boots (a real difference upstream of `0x0056ba30`).
The heap-tracked shadow leg of `0x0056ba30` (port vs original in the same boot, pre-state restored) was CLEAN 24/24.

## Arm D (diagnostic, `0x0056ba30` only)

`C4_TOOL=c4_racediff_v8_raw.py C4_TAG=_raw run_bvh4.py 0056ba30`, same Quick Battle window, same build. v8 = v7 plus a
RAW constant: every normalized value prints as `label=hex`, and each call record starts with `{A:<raw entry args>}`.
The v8 verdict line is not used (raw values make the signatures unique per call). Read by hand, per call k = 1..6,
OFF call k vs ON call k:

- **LAYOUT**: the raw 4th argument minus raw `param_1` differs between OFF and ON, and the abstract signatures (labels
  only, `=hex` stripped, after replacing `A0+118` by a heap ordinal) are equal. Reading (a).
- **VALUE**: same raw `param_1` (or same heap ordinal structure) and the 4th argument relates to different objects,
  in particular OFF's 4th argument is not `param_1 + 0x118` while every other argument agrees by label. Reading (b).
- Anything else: reported raw, no reading picked.

Rule: arm D does not change the PREREG rule. `0x0056ba30` needs leg R = C4-CLEAN, and v7 gave HOLD. So it **stays C3 in
this step** whatever arm D shows. A LAYOUT reading goes to the RESULT as a tool finding (label window) for the next
step to fix and re-run. A VALUE reading is filed as a U-n.

Driver change (the only one): `run_bvh4.py` reads `C4_TOOL` (default `c4_racediff_v7.py`) and `C4_TAG` (default empty,
appended to the row key and folder).
