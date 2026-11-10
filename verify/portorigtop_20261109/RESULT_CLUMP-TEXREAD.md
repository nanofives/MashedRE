# RESULT CLUMP-TEXREAD (2026-11-10): GREEN, knob-path texture lookups follow RW's find rule; oil slick reaches 0x0053fd30 4 times like the original; 0 default-path materials affected (no knob needed)

L2 worker `cmwb3bps015lwsc1bjg9b2bc5` for lead `cmw7xu1960f37p41557tefpnd`. Branch `orch/CLUMP-TEXREAD` on
`orch/ATOMIC-VIEW-2` `0fd2195bb`, rebased onto `orch/integration` `e714c02ab`, which merged ATOMIC-VIEW-2. There is no
`mashedmod/` difference between those two bases. Resolves U-9802.

## 1. Why the texture was null

### The original: an original-side read (own pid 1848, test-slot + mashed_lock)

The Frida script logged:
- every RwTexDictionarySetCurrent 0x004c5c80: argument and caller
- every RwTextureRead 0x004c5cb0: the name, the current dictionary at entry `*(*0x007d3ff8 + *0x007d4054 + 0x10)`, the
  result, and the result's dictionary (texture +0x04)
- every 0x0042a5d0 load name, until the four 0x0040cda0 calls had run

The script `texread.py` is not committed. Its output is `D:/MashedWT/CLUMP-TEXREAD_data/texread_orig.json` (445
events).

- **The current dictionary is 0 at almost every read.** Each TXD read sets it and then resets it to 0 (callers
  0x0054f962 set, 0x0054fd43 reset). The powerup inits then call 0x004c5c80(0) as well (0x00456797, 0x0045490c,
  0x00455458, 0x004583c9, 0x0045b84d, 0x00454eaa).
- **The reads still resolve, from several dictionaries:**
  - `PowerUpModelsFinal` (every powerup DFF, oilslicker.dff included) -> 0x0F120410 in dictionary 0x0D70D1DC
    (= DAT_0068d4e0, PowerUpModels.txd)
  - `flash` -> the same dictionary
  - `Oil` -> 0x0F9363A4
  - `NFL*` -> 0x0D70D1C4
  - `sun` -> 0x0408EFBC
  - and so on
  Only `test` misses (returns 0), which matches the exe.
- **RwTextureRead** calls the texture module's find callback (+0x18 slot, 0x004c5cc5) first and the read callback
  (+0x14, 0x004c5cee) second.
- **The find callback 0x004c7490** (disassembly, MASHED.exe.unpatched):
  - With a current dictionary (module +0x10 != 0) it walks that dictionary's textures only (0x004c74af..).
  - With none it walks the module's dictionary list from the head (0x004c7529: `mov eax, [ecx]; cmp eax, ecx`). For
    every dictionary it walks every texture, comparing names case-insensitively (0x004c755d..0x004c7577: both chars
    folded `a..z` -> `-0x20`). The first hit wins.
- **RwTexDictionaryCreate 0x004c5890** links a new dictionary in at the list head (0x004c58d4..), so the walk is newest
  first.

So memory `original-texture-lookup-is-current-dictionary-only` covered only the current-dictionary case. It is now
corrected.

### The exe

- `MashedRw_ClumpStreamRead_004e7420` reads through `rw::Clump::streamRead`.
- librw's material reader resolves texture names through `Texture::read`, which calls `rw::Texture::findCB`.
- The default is `defaultFindCB` (deps/librw/src/texture.cpp:304). It searches the current dictionary only, and with
  none it returns nil. Its source comment says so: "TODO: RW searches *all* TXDs otherwise".
- The exe's current dictionary is also 0 there (the 0x004c5c80 binding is `rw::TexDictionary::setCurrent`). So
  `PowerUpModelsFinal` came back null.
- `MashedRw_TextureRead_004c5cb0` called the same `rw::Texture::findCB` too. For `sun` that fell through to re-reading
  sun.png from piz, where the original finds it in a dictionary.

## 2. Default-path exposure: 0 materials

- `rw::Clump::streamRead` is called in two places in the exe (grep over mashedmod/src):
  - **RwApiBind.cpp `MashedRw_ClumpStreamRead_004e7420`.** Its callers are Match/PortTop_004b3bf0.cpp,
    PortTop_004b3e40.cpp and PortTop_004781b0.cpp, all MASHED_ORIG_TOP-only TUs.
  - **LibRw/RwRaceFx.cpp:1355 (HUD panels, default path).** It installs its own `rw::Texture::findCB =
    HudFindTexture` (PANEL.TXD + INTERFACE.TXD) around its reads and restores it (:1385 / :1455), so librw's default
    find is not used there.
- `MashedRw_TextureRead_004c5cb0`'s callers are PortTop_0040bbb0.cpp, PortTop_004283a0.cpp and PortTop_0048bbe0.cpp,
  also ORIG_TOP-only.
- Nothing else in the exe calls `rw::Texture::read` or `Material::streamRead`. The default-path track and car loaders
  resolve textures themselves.

The fix therefore lives in those two ORIG_TOP-only bindings, and the default path cannot reach it. No default-OFF
knob, PREREG or stack file is needed. The OFF captures and the default gates below are unchanged.

## 3. The fix (LibRw/RwApiBind.cpp)

- **`RwRuleFindTexture(name)`:** the 0x004c7490 rule. It searches the current dictionary (`rw::TexDictionary::getCurrent`)
  only, and with none it searches every dictionary newest first, using `TexDictionary::find`, which is case-insensitive
  (`strncmp_ci`).
- **`s_rwTexDicts`:** the RW dictionary list. librw keeps its own list private, so the bindings keep this one:
  - `DictFromTxdBytes` appends each dictionary it creates. That covers every knob-path dictionary: 0x0054f8d0 and
    0x004b3eb0 both go through it.
  - `MashedRw_TexDictionaryDestroy_004c5930` removes the dictionary before destroying it.
- **`MashedRw_TextureRead_004c5cb0`:** finds through `RwRuleFindTexture`.
- **`MashedRw_ClumpStreamRead_004e7420`:** sets `rw::Texture::findCB = RwRuleFindTexture` for its librw read and
  restores the previous callback right after, the same pattern as RwRaceFx.

No librw source change.

## Acceptance

Machine rules as before:
- Builds waited for the priolinger / SOLO locks.
- Every launch went through test-slot and mashed_lock.
- Roots: `D:/MashedWT/POWERUPINIT-C_root` / `_rootA`.
- The first build's console pipeline was cut by an output filter; it was rebuilt in full, `=== Build OK ===`.

| check | result |
|---|---|
| build (`MASHED_NO_DEPLOY=1`) | `=== Build OK ===`, mashed_re.exe sha256 `3c68b72f0521778441539df4e29af725c2c3185fc410adafe6eee85c1459996e` |
| knob ON (ORIG_TOP), RWMOD off | `exited rc=0`, `returned 0 after 3000 frames`. **4 x `0x0053fd30(material 10BAC2B8, texture 08EBE758, frame 08D26648, fbAlpha 1, coef 0.4)`** (knob_on_1.log:435/441/447/453), and `0x0053fbb0(material, 2)` x4, `(material, 0)` x0. Original: 4 x 0x0053fd30 on one material with the "shine" texture, the camera frame, fbAlpha 1 and the 0.4 coefficient (ATOMIC-VIEW-2 oilread.json) |
| knob ON, RWMOD on | `exited rc=0`, the same 4 x 0x0053fd30 |
| RwTextureRead results | `sun` now resolves through the find rule (the original: dictionary 0x0408EFBC), not from sun.png. `test` still misses (as the original). MashedNEWLogo / bigE / proLogicII / DolbyDig still come from their piz images (the original returns them from its read callback with no dictionary) |
| 12 captures, RWMOD off (OFF/ON/OFF/ON) | 4/4 `rc=0 pinned pos=55 hash=28538c74 class=E eqE=12`. Both ON logs: 4 x 0x0053fd30 |
| 12 captures, RWMOD on | 4/4 class E eqE=12, the same |
| walk_04 | `ba34030d1b7e2fe4` in all 8 rows. No capture moved. runs.tsv sha256 f560646e… |
| G-CTRL (H_DEF) | `8284be46 / 67931834 / 31fe043e`, unchanged |
| B (K0 / K1 / K26) | `1d5bf4f9` x3, unchanged |
| rva-lint | OK, 35 known, 0 new |

## BINDINGS, port column, trackers

- **BINDINGS:** `verify/portorigtop_20261109/update_bindings_clumptexread.py` runs `update_bindings_atomicview2.py`
  first (the chain tip, which reproduces 506 rows). It then adds 0x004c7490 as bound (`RwRuleFindTexture`) and notes on
  0x004c5cb0 and 0x004e7420. 506 -> 507 rows.
- **Port column:** `backfill_exe_file.py` and `backfill_port.py` change 0 rows. No body moved, only binding behaviour.
- **Trackers:** U-9802 is resolved with `uncertainty_resolve.py`. Memory `original-texture-lookup-is-current-dictionary-only`
  now carries the corrected rule.

## Raw files (D:/MashedWT/CLUMP-TEXREAD_data, not committed)

| file | sha256 (first 16) |
|---|---|
| build.log / mashed_re.exe | 4c81f5734dec97e8 / 3c68b72f05217784 |
| knob_on_1.log / knob_on_rwmod.log | 86a7369929b43675 / 6664246973141ace |
| runs.tsv | f560646e9825f27b |
| capflake_nav1.stdout / capflake_rwmod.stdout | 519528f6c18c5c82 / ac153beafd024ab9 |
| arms.stdout | 7a05866ea30802ef |
| arms H_DEF step / gates / ptrace | 8284be46 / 67931834 / 31fe043e |
| arms B_DEF_K0, K1, K26 step | 1d5bf4f9 x3 |
| texread_orig.json (original-side read) | 5f879029503d7d4b |
