# RESULT D4-WSF-TRACKDATA-RT (2026-10-26)

Verdict: **GREEN**. The AI path-data writer `FUN_00423540` round-trips byte-identical,
and the original's own writer confirms it. 4 of the 5 UNCERTAIN anim fields in
`track_anim_data.md` are resolved from cited decomp. 2 items stay open, each with its
missing evidence named. No default changed. The default exe and .asi are code- and
data-identical to `orch/integration`, so the gates cannot move and were not re-run.

Scope: S-DoD criterion 5 under L-22 (a) (`ROADMAP.md:152`). The AI path data is the one
format in this row that the game writes.

## 1. Writer identity (FUN_00423540)

Decomp (`re/tools/decomp_pc.py`, pool slot, raw in `decomp1.txt`):
- `0x00423540`: `FUN_00423480()` builds `DAT_00644110`, then `FUN_004cc230(2,2,path)`,
  `FUN_004cc580(h, 0x13269902, 0x11884, 0x37002, 10)`, `FUN_004cbe80(h, &DAT_007f1a9c, 0x11884)`,
  `FUN_004cc160(h, 0)`, return 1.
- `0x004cc580`: `version = ((p4 + 0x10000 & 0x3ff00) << 0xe) | ((p4 & 0x3f) << 0x10) | (p5 & 0xffff)`,
  writes `{type, size, version}` as one 0xc-byte `FUN_004cbe80`. `(0x37002, 10)` gives `0x1c02000a`.
- `0x00423480`: path = `""` + `"AI%d.AI"` of `FUN_00426c00()` (relative to the game dir).

Emit added: `AiData_Emit` + `kAiWriterVersion` (with a `static_assert` on the pack) in
`mashedmod/src/mashed_re/Ai/AiData.h`, test-only (no caller). Python twin
`re/tools/ai_data.py roundtrip`. Test `mashedmod/src/mashed_re/Ai/tests/aidata_rt_test.cpp`
(not in either rsp list).

| leg | result |
|---|---|
| C++ parse -> emit, 13 shipped members (`roundtrip_cpp.txt`) | 12/13 identical, AI30 differs in bytes 8..11 only (`0x1803ffff` shipped vs `0x1c02000a`) |
| Python parse -> emit, same members (`roundtrip_py.txt`) | same: 12 PASS, 1 STAMP, 0 other |
| Original track 0: stock MASHED.exe in a race, dump `DAT_007f1a9c`, call `FUN_00423540` | ret 1, wrote `AI30.AI`, 71,824 B, **0 bytes differ** from `AiData_Emit(DAT_007f1a9c)` |
| Original track 2: same | ret 1, wrote `AI39.AI`, **0 bytes differ** from the emit, and SHA-256 equal to the shipped `AI39.AI` |
| Loaded image vs shipped payload, both tracks | equal, and unchanged by the call (mem_before == mem_after) |

Note: the original's own `AI30.AI` carries `0x1c02000a`, 4 bytes away from the shipped
file. The writer always stamps `0x1c02000a`, and the emit does too. The reader
`FUN_004235b0` ignores the stamp (`ai_path_data.md:29`).

Original-side harness: `re/frida/scenario_launch.py --ai-save-capture <dir>` (new,
opt-in, same pattern as `--ghost-capture`). Stock original (`MASHED_RE_NO_AUTO_HOOK=1`),
private root `D:\MashedWT\D4-WSF-TRACKDATA-RT_root`, both runs under `mashed_lock.py`.
Only the spawned pids (20240, 7348) were touched.

## 2. UNCERTAIN anim fields (track_anim_data.md)

| field (old line) | status | evidence |
|---|---|---|
| SPL `+0x00` 32-byte sub-header (:45) | **resolved**: name, copied to object `+0x18`; `+0x24` is the spline type (1/2 accepted) | `FUN_004b3cc0` (chunk 0xc) -> `FUN_00545260` (one 0x28 read, `RwGlobals+0xd0(obj+0x18, blk, 0x20)`) -> `FUN_00546380` (type 1/2, else error 0x102) -> `FUN_00545340` branches on `obj+0xc` |
| UVA `+0x14` u32 (:128) | **resolved**: read into a stack local and discarded | `0x00552340` (Ghidra-missed, `--create`), scheme 0x1c1 stream-read from info `0x00623e54` |
| UVA `+0x38` "pad/nodeToUV[8]" (:130) | **resolved (meaning)**: per-node output-slot index, custom `+0x20`, value <2 selects material UV matrix 0/1, else skipped. **The doc's "all 0" was wrong**: 5 of 12 entries carry non-zero words in `[1..7]`, Storm `MUL_watertest_M` has `(0,1,..)` | `FUN_00551c40` (custom read), `FUN_00551d40` (consumer, `*(u32*)(custom+0x20+node*4)`, loop bound `interp+0x2c`) |
| UVA keyframe element order (:133) | **resolved**: uv[0] angle (rad, x 57.2957763671875, wrap ±π), uv[1] right.x, uv[2] up.y, uv[3] right.y, uv[4] pos.x, uv[5] pos.y, rotation about (0.5, 0.5) | `FUN_00552050` apply, `0x005520f0` interpolate, constants `0x005e488c`/`0x005e4898`/`0x005cd100`/`0x005e48a4`/`0x005ce2f4`/`0x005cd2c0` |
| UVA `+0x1c` keyframe link | clarified: a keyframe **index**, stored as `frames + i*0x20` | `0x00552340` |
| F2 copter cadence / phase / nose orientation (:108) | **open** | missing: the per-frame update the `SetCopter` / `KTC_NewCopter` bindings register, and the delta it passes to `RtAnimInterpolatorAddAnimTime` `0x0052e0e0`. The two `RpHAnim` add-time sites `0x00451ee7`/`0x00452048` are in Ghidra-missed code in the pontoon block (`0x00451690` neighbour) and are not shown to be the copter path |
| UVA `outSlot[1..7]` reads | **open** | missing: the writer of `interp+0x2c` (node count) for a material UV anim |

Shipped values cross-check (all 8 `.UVA`, 12 entries): uv[0] = -0.0, uv[1] = uv[2] = 1.0,
uv[3] = 0.0 everywhere, so only pos.x/pos.y animate. The Arctic scroll rates in the note
stand. All are scheme 0x1c1. `+0x14` is 0 in all 12.

Side finding (not edited, trackers only via re-classify): `hooks.csv` names `0x0052daf0`
`image_library_format_handler_registry`. Its decomp is the RtAnim animation stream-read:
version `0x100` check, scheme lookup in `0x00912160` (stride 0x30), header words via
`FUN_004cc790`, then the scheme's `+0x20` callback (`decomp3.txt`). It is reached as the UVAnim
dict schema's `+0x28` (`0x00623df0` words).

## 3. Gates

Not run, because the default path is byte-identical. `AiData.h` is included by
`D3d9Render/TrackRenderer.cpp`, and the additions are unused inline/constexpr. Built both
targets twice with `MASHED_NO_DEPLOY=1`, once with `AiData.h` at `orch/integration`
content and once with the change. Section compare (`pecmp.py`):

| target | .text | .data | .rdata | headers | .reloc/.fptable |
|---|---|---|---|---|---|
| mashed_re.exe | 0 diff | 0 diff | 2 B, all in the debug directory | 2 B, TimeDateStamp | 0 |
| mashed_re_dev.asi | 0 diff | 0 diff | 2 B, all in the debug directory | 2 B, TimeDateStamp | 0 |

The other changes are tools (`ai_data.py`, `scenario_launch.py` opt-in flag), the test
file and two notes, none linked into either binary. Reference gates (merge_train.json):
G-CTRL 5943c5f1/b493262b/09551e6a, B 3a03670c/e85ea6fb/53778664.

## 4. Raw evidence (D:/MashedWT/D4-WSF-TRACKDATA-RT_data), SHA-256

```
4a30fc734e5a6c5205f410a0c5d04590512e55ca47ece9bcb4168efed77f67e9 roundtrip_cpp.txt
7763be9bba9ac18f9211b798ded07ad0608d62305df8d477c3985ff09c52b505 roundtrip_py.txt
6a2c3366ba75ad2a7ed985eeddcc41339814fd501ab67e6a16e8dd34f5a3036b live_t0/run.log
2fdf6ec7a66f6e7c7e11538dd53b6fdb8809b6a2bc93f8fca6460b98e8dddeea live_t0/written_AI30.AI
16b0d54fc72894d65a3c5028a1e88f8902a8a68fbd61539afbd633210e1714b1 live_t0/mem_before.bin
16b0d54fc72894d65a3c5028a1e88f8902a8a68fbd61539afbd633210e1714b1 live_t0/mem_after.bin
0edc308163e5cc6f12ae2d66e981d0a73c4adafcfe8d18094c993dd15a0b6006 live_t0/ai_save_capture.json
f2f0e000addac62454c813b6d680b10c5769afb6cbab4e4c34de19481980a016 live_t2/run.log
33f26d8abb229eb2d4c9a577a7916e72b5cd011b99a1a979a714915265145129 live_t2/written_AI39.AI
fef8fc7563f2f674b81910e12e3ecaf4f5c04f34d574b3360ab4299686f5d5cd live_t2/mem_before.bin
fef8fc7563f2f674b81910e12e3ecaf4f5c04f34d574b3360ab4299686f5d5cd live_t2/mem_after.bin
b6827b015f5d1a4926f5377e826e864cc90d5cf0ed610d644b0ca21fc08a28a4 live_t2/ai_save_capture.json
de3eb6a08815b425954023036d95499712e5d7a80cf39be46798fbf52b6b8a91 ai_members/AI30.AI
33f26d8abb229eb2d4c9a577a7916e72b5cd011b99a1a979a714915265145129 ai_members/AI39.AI
f0a13838f7aba434b8277875750c45465617899b32ae8b2d62b1785a91a964f9 decomp1.txt   0x00423540 0x004cc580 0x00423480 0x00558df0 0x00546380
eefbb134034cdb7a42ffe7c4216b416e9d88b70db6b6e72ee072b5759c347214 decomp2.txt   0x00545260 0x00545340 0x0045e160 0x0045e2a0 0x0042a740
01b750d9ab4c653f6a11b7b4c6eac6c2de18060440d6b3d68ae9db2e451d624b decomp3.txt   0x0052daf0
5a3db9a100a0b49872ce4eb0fd780f6762c63d544e43aa478e5d7891cc3b66c6 decomp4.txt   0x00552050 0x005524a0
09c12fb3064d0d3ee6b85c9059ed1eabe40f4caed1ab4cc6e96885214b07cf54 decomp5.txt   0x00552340 0x005520f0 0x00551ce0 (--create)
f4fd578f319397a5e2bb00413938f14c6bba3348b5166b349bbb127cee6e15b9 decomp6.txt   0x00551c40 0x0052e0e0
de955ee533d0cb3c7702678ff8f24ee2d47e0334e3cacac2bc2e4d7d25b35ef3 decomp7.txt   0x004b47c0 0x00551fe0
497d9a54fe84cd4309ea90d53c725db175b2a5a79ec3877c68b2bde655cba2dd decomp8.txt   0x004b47c0 0x00474f50 (xrefs)
deb64abd47e6bc711c4e4a30d570d6b2e336757a044a07fac9bb53880bfeac0a decomp9.txt   0x00451ee7 0x00452048 0x00475010 (--create)
442a818122b1f6377d25c251edcba9e00eef04a8aa005c13740e4fb2c262e64a decomp10.txt  0x00551d40 0x00552020
f5cd2ae894af78f4759f42b804daebf524edf07146d18c1bfdbeb976a13dc22f decomp11.txt  0x004b3cc0
C7136589D3D1A80D6980A6689AD3140D72EC52BA397C49DC3F984F8DCBA8C615 bin_base/mashed_re.exe
DB180DE3B61663C52A88E6034584FEFD98C90AA0572345DFE38783DDF97D9EF3 bin_base/mashed_re_dev.asi
B66D6D992CC7214A594647D94159E2AE35DC5192C695FA50C29F2FF55DCDCC5D bin_with/mashed_re.exe
8DC43197420857C05D7DF7E8B0C08B9CD76BA1CE69092E64AADECFC8FC50BEA7 bin_with/mashed_re_dev.asi
```

Static words were read from `original/MASHED.exe.unpatched` (anchor
`BDCAE093...3C0E`): schema `0x00623df0`, scheme infos `0x00623e24`/`0x00623e54`, floats
listed in section 2.

## 5. Next

- Copter cadence: find the `SetCopter` binding's update function (Lua registration
  strings), then its `0x0052e0e0` / `FUN_004b47c0` delta.
- `interp+0x2c` writer for material UV anims, to close the `outSlot[1..7]` item.
- `0x0052daf0` rename via re-classify.
- No C-level change claimed for `0x00423540` (C2). The original-side call is fixture
  evidence for the format, not a hook diff. Promotion would go through re-classify.
