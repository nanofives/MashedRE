# FUN_004cae90 — rwD3D9GetCaps / caps-block address-of getter

**RVA:** 0x004cae90  
**Body:** 0x004cae90 – 0x004cae95  
**Status promoted:** C1 → C2 (batch-render-4-s5)  
**Confidence rubric:** C2 — decompilation read; callee/caller graph confirmed; semantics documented.

---

## Signature

```c
undefined4 * FUN_004cae90(void);
```

Returns: pointer to `DAT_00618220`.

---

## Callers

- `FUN_004c30b0` @ 0x004c30b0 — caller checks return value non-null to gate execution.

## Callees

None (pure leaf — single `LEA + RET`).

---

## Mechanics

Trivial getter:
```c
return &DAT_00618220;
```

`DAT_00618220` is the D3D9 caps block (0xe DWORDs, as copied by `FUN_004c7a70` case 0x4). This function provides the address so callers can read caps fields directly.

---

## Notes

- Previously had C1 plate at `re/analysis/rw_engine_init_d2/004cae90.md` (note: "address-of getter; returns &DAT_00618220; caller checks non-null to gate execution").
- No new UNCERTAINTIEs raised.
