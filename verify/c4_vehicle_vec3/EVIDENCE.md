# C4 evidence — 0x0046d700 + 0x0046bce0 (orch-iter21, 2026-07-31)

Both rows held C4 before the `out3_idx` audit demoted them to C2. They were restored to
C3 on payload-observing evidence earlier today; this supplies the canonical-scenario half
that C4 additionally requires, so the level they held is re-earned rather than restored on
the strength of the old (void) evidence.

## The measurement

Canonical race, `scenario_launch --track 0 --cars 6 --hold 15 --hooks 0x0046d700,0x0046bce0`,
counters on the **.asi exports** — reachable only through the installed inline JMP, so a
non-zero count is positive proof the port executed and carries no inference.

| run | asi:VehicleVec3At9C8Get | asi:VehicleVec3At94Get | asi:VehicleVec3At6E4Set (control) |
|-----|-------------------------|------------------------|-----------------------------------|
| 1   | 5866                    | 2928                   | **0**                             |
| 2   | 7443                    | 3708                   | **0**                             |

`VehicleVec3At6E4Set` is the non-degeneracy control. Its export resolves and its counter
arms (`armed@0x739e5190`) in both runs, but it is **not** in `MASHED_HOOK_ONLY`, so no JMP
routes to it. 0 there while the two rows under test count in the thousands rules out a
counter that counts anything.

Both races completed normally (`spawn fired: 12`, matching the stock-original run), so the
installed hooks caused no regression.

Rate check: ~390/s and ~195/s over the 15 s hold, combined ~585/s — under the 1000/s
hot-path limit in CLAUDE.md. Neither run destabilised.

## The bit-identity half

`log/diff_vehicle_vec3_at_9c8_get.csv` and `log/diff_vehicle_vec3_at_94_get.csv`, GREEN
11/11 each under `ptr_out_table_get`, which fingerprints `out[0..2]` **and** the return.
Each in-range idx yields three distinct dwords tracking idx; 16/17/255 give `0,0,0|ret=0`.

**This is the evidence that did not exist when these rows were previously C4.** Their old
A/B ran under `out3_idx`, which never read the out buffer back — see
`re/analysis/out3_idx_false_green_audit_20260731.md`.

## Scope of the claim

The canonical run proves the ports EXECUTE in the real scenario with the JMP live and
without regression. It does not compare outputs against the original in that scenario;
bit-identity comes from the synthetic A/B above. The two together are what the C4 clause
asks for, and the canonical-scenario half is what was missing.
