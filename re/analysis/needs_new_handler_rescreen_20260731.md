# NEEDS_NEW_HANDLER re-screen — the 3 iter21 rows (orch-iter21 cycle 5)

Standing rule #3 says a NEEDS_NEW_HANDLER verdict is a hypothesis about the handler
inventory, not a fact about it. This is the **sixth consecutive run** where that held.

**Result: 0 of 3 need a new handler.** One needs nothing at all, one needs a single
defaulted config field, and the third's stated blocker does not exist.

All three were checked against the **raw listing** (Mashed_pool11), not their plates.

---

## `0x004b6b00` — NO CHANGE NEEDED. Use `eax_ecx_insert`.

Listing, 3 bytes:

    004b6b00  89 01     MOV dword ptr [ECX],EAX
    004b6b02  c3        RET

The brief rejected the inventory on the grounds that *"no handler can both (a) control the
implicit EAX register before the call, (b) pass ECX = a fresh scratch buffer, and (c)
observe `*ECX` after."* **`eax_ecx_insert` does all three**, and has since it was written.
It was not among the handlers the brief considered, despite its name describing this exact
shape.

`re/frida/early_window_leaf_diff.py:2980` builds the trampoline

    mov eax, bufA
    mov ecx, bufC
    [mov edx, imm32]      ; optional, cfg.edx_val
    jmp target

and `bufA` / `bufC` are allocated **once and shared by both the original and the reimpl
call**. That is what makes this row work: the value stored into `*ECX` is `bufA`'s address,
which is *identical on both sides*, so it compares equal instead of being a per-side
allocation that always diverges. Snapshot it with `ecx_observe: [0]`.

Non-degeneracy is thin but real: the handler makes a single call (`test ignored`), so there
is one observation. A port that stored the wrong register, or stored nothing, leaves
`bufC[0]` at its seeded 0 against the original's `bufA` address, so the comparison does
discriminate. If a stronger vector set is wanted, `edx_val` is the precedent for adding a
raw-immediate `eax_val` — but nothing is blocked without it.

## `0x00407550` — ONE DEFAULTED FIELD on `esi_global_search`. Not a new handler.

Listing, 35 bytes:

    00407550  MOV EDX,dword ptr [0x0063a5d0]   ; count
    00407556  XOR ECX,ECX                      ; i = 0
    00407558  TEST EDX,EDX
    0040755a  JLE 0x00407570                   ; count <= 0 -> return 0
    0040755c  MOV EAX,0x639d80                 ; record base
    00407561  CMP dword ptr [EAX + 0x44],ESI   ; key field at +0x44
    00407564  JZ  0x00407572                   ; hit -> return EAX (record ptr)
    00407566  INC ECX
    00407567  ADD EAX,0xec                     ; stride
    0040756c  CMP ECX,EDX
    0040756e  JL  0x00407561
    00407570  XOR EAX,EAX                      ; miss -> 0
    00407572  RET

`esi_global_search` already models exactly this: ESI-delivered key, linear scan of a global
table with a base, a stride and a count-global, returning a pointer or 0, driven through a
`mov esi,key; jmp` trampoline with a `__cdecl(key)` reimpl compared on the RESULT, not the
ABI.

The single divergence is the key field offset. `early_window_leaf_diff.py:1166` seeds

    ptr(cfg.tgt).add(sidx * sstride).writeU32(skey);

i.e. the key at entry **+0**, where this function compares at **+0x44**. Adding
`.add(cfg.key_off | 0)` with `key_off` defaulting to 0 is additive and leaves every existing
caller of the handler byte-identical. Direct precedent for exactly this shape of change:
`stub_at` on `stub_dispatch_observe` and `null_args` on `ptr_seed_observe` (both iter20),
and `this_reg: 'stack'` on `reg_this_call_observe` (2026-07-31).

**This row is a BETTER test than `0x00407580` was**, and for a structural reason:
`esi_global_search` **seeds** the table (count=4, zeroes four entries, plants a distinct
non-zero key per test) instead of reading whatever the live array happens to hold. The
degeneracy that made `0x00407580`'s GREEN thin does not apply here.

Config: `tgt = 0x00639d80`, `glob = 0x0063a5d0`, `stride = 0xec`, `key_off = 0x44`.

**Same record array as `0x00407580`** (base `0x00639d80`, stride `0xec`, key at `+0x44`).
The pair is coherent: `0x00407580` returns record[i]'s key, `0x00407550` finds the record
whose key matches. [UNCERTAIN] `0x0063a5d0` is also cited as the "rule-5 collect counter"
in `scenario_launch.py`; whether that is the same counter or an address collision in the
notes is unresolved and is NOT relied on here — the count global is read from the listing.

## `0x005bfb90` — the brief's stated blocker DOES NOT EXIST. Verdict: unsettled, not "new handler".

Listing, 30 bytes:

    005bfb90  PUSH ESI
    005bfb91  MOV ESI,dword ptr [ESP + 0x8]    ; param_1
    005bfb95  PUSH 0x0                         ; lpPreviousCount = NULL
    005bfb97  PUSH 0x1                         ; lReleaseCount = 1
    005bfb99  MOV EAX,dword ptr [ESI + 0x150]  ; handle
    005bfb9f  PUSH EAX
    005bfba0  CALL dword ptr [0x005cc094]      ; INDIRECT, through a fixed IAT slot
    005bfba6  MOV ECX,dword ptr [ESI]          ; vtable
    005bfba8  PUSH ESI
    005bfba9  CALL dword ptr [ECX + 0x8]       ; vtable slot +8
    005bfbac  POP ESI
    005bfbad  RET

The brief concluded a dedicated handler must *"create a real semaphore via CreateSemaphore,
store the handle at param_1+0x150, install a no-op Release stub at vtable[+8], call the
function, verify the semaphore count incremented"*.

**No real semaphore is needed.** The call is `CALL dword ptr [0x005cc094]` — an indirect
call through a **fixed IAT slot at a known image address**, not a direct call into
kernel32. Point that slot at a recorder and the OS never participates: what the function
passes (`handle`, `1`, `NULL`) becomes directly observable as *arguments*, which is
strictly better evidence than inferring the effect from a semaphore count. The brief's
premise — a live OS handle — came from reading `ReleaseSemaphore(...)` in the decompiler
output rather than the `CALL [0x005cc094]` in the listing.

Both remaining pieces have existing machinery: absolute-global seeding (`[[addr, val], ...]`,
e.g. `seed_globals_arg_multiobs`) can plant the IAT stub, and `ptr_seed_observe`'s `ptr_to`
seeds the pointer chain for `[ESI] -> vtable -> [+8]`. No single handler combines
global-seeding, a pointer chain and two recorded stubs today, so this row is **genuinely
unsettled** — it is the only one of the three where a real extension may be justified.

It is also the **least attractive** of the three: it is a teardown path whose whole
observable behaviour is two outbound calls, and running it live releases a real semaphore
and invokes a COM `Release`. Recommend routing it to the mutator/defer lane rather than
building for it. Do NOT record it as NEEDS_NEW_HANDLER on the brief's reasoning, which is
refuted above.

---

## Why this keeps happening

Six runs, same failure. The screen is asked "does a handler cover this?" and answers from
the ARG_TYPES.md one-line summaries, which describe each handler's *original use case*
rather than its *mechanism*. `eax_ecx_insert` reads as "cross-link insert" — a use case —
so a screen looking for "store EAX into *ECX" never matches it, even though the mechanism
is exactly that. Likewise `esi_global_search` reads as a fixed recipe rather than a
parameterised scan.

Two of these three were also mis-screened because the brief reasoned from decompiler output
(`ReleaseSemaphore(...)`, `in_EAX`) instead of the listing, which is standing rule #4.
