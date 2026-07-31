# `out3_idx` false-GREEN audit — all 4 rows affected (orch-iter21)

Surfaced while writing MECHANISM lines for `ARG_TYPES.md`. The handler is one line:

    if (CONFIG.arg_type === 'out3_idx') {
        return fn(buf, input >>> 0);
    }

It passes a scratch buffer and returns **the function's return value only**. The buffer is
never read back, never fingerprinted, never even poisoned. The codebase already knew — the
`out1_idx` comment immediately below says so in as many words — but the generated index never
surfaced it, so four rows were promoted on it.

## Why this is the worst case, not a mild one

All four functions were read from the listing (`Mashed_pool11`). Every one has the same shape:

    MOV EAX,[ESP+8]          ; idx
    CMP EAX,0x10
    JC  in_range
    XOR EAX,EAX              ; return 0
    RET
  in_range:
    MOV ECX,[ESP+4]          ; out ptr
    IMUL EAX,EAX,0xd04       ; vehicle record stride
    ... three dword moves through ECX ...
    MOV EAX,0x1              ; return LITERAL 1
    RET

**The return value is a constant.** `1` on the in-range path, `0` out of range. It is a pure
function of `idx < 0x10` and carries **zero information about the data moved**. The three
dwords are the entire payload, and they are exactly what `out3_idx` does not look at.

Therefore, for every one of these rows:

> A reimplementation whose entire body is `return idx < 16 ? 1 : 0` — moving no data at all —
> passes the recorded A/B **9/9 GREEN**.

The "non-degeneracy" recorded in `hooks.csv` is real but measures the wrong thing.
`0x0046d510`'s note says it outright: *"out3_idx bounds-return GREEN 9/9 non-degen (1 for
idx<16 else 0)"*. That is non-degeneracy of the **bounds check**, not of the vector.

## The four rows

| RVA | name | conf | shape | what the A/B actually proved |
|---|---|---|---|---|
| `0x0046d700` | VehicleVec3At9C8Get | **C4** | getter, 3 dwords from `0x881f68` | bounds check only |
| `0x0046bce0` | FUN_0046bce0 | **C4** | getter, 3 dwords from `0x882094` | bounds check only |
| `0x0046d740` | FUN_0046d740 | C3 | **SETTER**, writes globals at `0x8816e4` | bounds check only |
| `0x0046d510` | VehicleVelocityWorldGet | C3 | getter + matrix transform via `FUN_004c3df0` | bounds check only |

`0x0046d740` is the most thoroughly untested: it is a **setter**, so its observable is the
global block it writes, and `out3_idx` observes neither the buffer nor the globals. Both sides
also read the same undifferentiated scratch buffer as their source, so even the values written
are uncontrolled.

`0x0046d510` is the most likely to actually be wrong: it is the only one doing real work,
transforming `+0xac` through matrix `DAT_00614708` via `FUN_004c3df0`. A transform error is
invisible to a bounds-flag comparison.

## Two rows are C4, and their C4 evidence is separately weak

- `0x0046d700` — *"C3->C4 2026-06-19 racediff in-race (jmp=0xe9 off==on)"*. The
  canonical-scenario half exists, but the bit-identity half it rests on is the flag-only diff
  above. `off==on` at race level would not detect a wrong velocity vector on one vehicle.
- `0x0046bce0` — **no `C3->C4` line at all**. Its note stops at `C2->C3 c3_batch_ad`, and its
  `frida_diff` column is **empty**. The row is C4 in `hooks.csv` with no recorded C4 evidence
  and no cited diff artifact. That is a tracker defect independent of the handler problem.

## No new handler is needed to fix this

`ptr_out_table_get` (4 uses) already models this shape exactly:

> `u32 fn(out_ptr,idx): if(idx>=bound) return 0; out[0..n-1]=*(u32*)(base+idx*stride+j*4); return 1`

— and it **observes `out[0..n-1]`**. It is the correct handler for `0x0046d700`,
`0x0046bce0`, and (modulo the transform callee) `0x0046d510`. Note `out1_idx` is *not*
sufficient here: it fingerprints one written dword and these write three.

For the setter `0x0046d740`, the observable is the global block at `0x8816e4..0x8816ef`;
`indexed_table_set` or `abs_ranges_setter` is the right shape.

## Recommendation — NOT applied, this needs a ruling

Every one of these four rows should be **demoted pending re-verification**: the two C4s to C3
at most, and arguably all four to C2, since the A/B never exercised the behaviour the function
exists for. `0x0046bce0` additionally needs its unsupported C4 resolved regardless of the
handler question.

This is left for the user to rule on rather than applied unilaterally: demoting two C4s is
consequential, tracker mutations go through `re-classify`, and CLAUDE.md makes a substantive
tracker conflict a stop-and-ask. The evidence above is unambiguous about what was measured;
the ladder decision is not mine.

`out3_idx` itself should be retired once these four are re-verified — there is no call shape
for which it is the right choice over `ptr_out_table_get` or `out1_idx`.
