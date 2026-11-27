HOLD (draft, not a verdict): BIND-00420420 read-only evidence phase done; bind, builds and game arms held by L0's ruling until BIND-TOOLS-SMALL-3 merges. Measured execution count of 0x00420420's body on a knob-on arm: not measured yet (arms held).

# RESULT BIND-00420420 (run 56, BIND section) — DRAFT

Branch `orch/BIND-00420420` from `orch/integration` 1379467a6131ef8515937136b133cd7fa0f77404.
Row kind: top-down binding row, data only (RULE 1). L0 ruling (RULE 2): the bind goes through the bulk profile, and
BIND-TOOLS-SMALL-3 fixes the bulk `bindings_tag` KeyError and moved.tsv first. Until then only steps that change no state
ran. No edit under `re/tools/bind/state/`, no `write` / `sync` / `bindings`, no build, no game arm.

## Checks run (all read-only)

| Check | Result |
|---|---|
| `match.py --bytes re/match/s05/00420420.c` | 4/4 symbols 100.0%, BYTE-IDENTICAL (relocations masked): 0x0041f330 47 B, 0x0041f360 97 B, 0x0041fe10 401 B, 0x00420420 1099 B (317/317 ins). `BIND-00420420_match.txt` |
| `relocheck.py re/match/s05/00420420.c` | 4/4 OK: 0x0041f330 checked=3, 0x0041f360 checked=1, 0x0041fe10 checked=8, 0x00420420 checked=62 unchecked=2 (`@__security_check_cookie@4`, `___security_cookie`). `BIND-00420420_relocheck.txt` |
| `match.py --bytes re/match/s05/0041fe10.c` (the source the bulk profile binds for 0x0041fe10 today) | 100.0%, 401 B BYTE-IDENTICAL |
| `porttop_bind.py plan --profile bulk` (head roots, 362) | `Counter({'reuse': 204, 'stop': 188, 'bind': 97}) Counter({'match': 96, 'replay': 1})` = moved.tsv totals line. `BIND-00420420_plan_bulk_head.txt` |
| `porttop_bind.py plan --profile bulk --roots <roots.txt>,00420420` | `Counter({'reuse': 211, 'stop': 191, 'bind': 97}) Counter({'match': 96, 'replay': 1})`. `BIND-00420420_plan_bulk_add00420420.txt`, diff `BIND-00420420_plan_bulk.diff` |
| `porttop_bind.py samesrc --profile bulk` | `total 0` |
| Direct-call scan of `original/MASHED.exe.unpatched` (SHA-256 BDCAE093...3C0E = anchor), E8 rel32 only | one call to 0x00420420 at 0x0040d1fb (`56 53 ... e8 20 32 01 00 83 c4 08`: push esi, push ebx, call, add esp,8); one call to 0x0041fe10 at 0x00420763 (`8b c7 e8 a8 f6 ff ff`: mov eax,edi then call, record in EAX), inside 0x00420420's body |

## What the bind plan does (dry run, not written)

Diff head -> with 0x00420420 as a bulk root (plan listing lines):
- `00420420`: stop "outside the bulk cp1 list" -> **bind re/match/s05/00420420.c C4**.
- `0041fe10`: bind re/match/s05/0041fe10.c -> **stop "file-local copy of this rva in the source of 0x00420420 (one body per RVA)"**
  (profile `drop_local_copies`). 00420420.c carries a file-static 0x0041fe10 (byte-identical, above). The only direct
  caller of 0x0041fe10 in the original is 0x00420763 inside 0x00420420; in re/match/s05/0041fe10.c the call to
  FUN_00420420 is from the stand-in `Caller_0041fe10`. So the bind count stays 97 (one traded for one). This moves a
  cp1-list RVA (0x0041fe10) from bind to stop as a direct effect of this row's bind: its moved.tsv line will carry
  cause "file-local copy in re/match/s05/00420420.c, now bound as 0x00420420". [Flag for the lead: 0x0041fe10's state
  line changes as a consequence of the owned bind; no other row owns 0x0041fe10 this run.]
- New stops (5 next stops of the gate-rank row, three new in the listing): 0x00420290 (C2), 0x00544a70 (C2),
  0x00544ad0 (C2); 0x004222f0 and 0x0053d460 were already stops. 0x00544a70 / 0x00544ad0 are owned by
  BIND-0048AE00-CALLEES: this row lists them as stops only and binds nothing there; if that row binds them first,
  `bound_wins` reuses its bodies.
- New reuse (7): 0x0042a6b0 (PortTop_Bound), 0x004b52c0, 0x004c15c0, 0x004c5c00, 0x004c5c80, 0x0053fb70, 0x0053fd30 (RwCore).
- Match / replay split of bound bodies unchanged: 96 match + 1 replay.

## Gate evidence

`verify/bindgates_20261121/T2_GATE_RANK_allknobs_after.tsv` rank 1: 0x00420420 c4=match bindable, g_calls 4, route_state
stop (bulk), callers_on_path 0x0040d110, next_stops 0x00420290 0x004222f0 0x0053d460 0x00544a70 0x00544ad0.
0x0040d110 is bound today in bulk (`bind re/match/topm01/0040d110.c`) and calls the named stop
`PortTop_BulkStop_00420420` (Match/PortTop_BulkStops.cpp:41-43).

## Not run yet (held)

`write --profile bulk`, `refs --profile raceload`, `sync --profile bulk`, `bindings`, the roots.txt line, moved.tsv
lines, both builds, `ddkobj.py check`, the binder selftest, the knob boot and the execution count of 0x00420420 on a
knob-on arm. Binding numbers (1)-(3) and ddkobj counts (3) follow after SMALL-3 merges and the branch is rebased.

frida-<hash> folders in %LOCALAPPDATA%\Temp at start: 698 (no Frida run by this row so far).
