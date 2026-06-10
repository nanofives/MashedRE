# SCRIBE_QUEUE fragment — batch_ai session 1 (ai_s1)

Author-only promote-c2 pass. Bucket plates are the C2 deliverable; central
finalize (ghidra-sweep) writes hooks.csv / trackers and commits.

## Queued

2026-06-02  ai_s1  bucket=re/analysis/bucket_audio_005b8be0_005bcb80  confidence=C1->C2  rvas=005b8be0,005b8c40,005b92e0,005b9320,005b93a0,005b9410,005b9460,005b9e30,005b9f30,005ba720,005ba760,005ba780,005ba7f0,005baa60,005bac00,005baf40,005bb000,005bb5b0,005bb820,005bba60,005bbac0,005bbb20,005bbb70,005bbbb0,005bbc00,005bbc10,005bbd50,005bbdb0,005bbed0,005bbf30,005bbf60,005bc020,005bc050,005bc0f0,005bc190,005bc450,005bc470,005bc640,005bc690,005bc750,005bc800,005bc850,005bc860,005bc880,005bc890,005bc900,005bc920,005bcab0,005bcb80

## Notes for the sweep

- **Count**: 49 RVAs, 49 plates authored in the bucket dir. None drift-skipped
  (all 49 were `audio,C1` in hooks.csv at session start; none already C2+).

- **Pool slot**: PRE-ASSIGNED `Mashed_pool0` returned `LockException` on open
  (fresh lock held by a sibling/leftover; MCP server reported 0 open programs).
  Per leaked-lock guidance I did NOT retry pool0 — fell back to verified-free
  **`Mashed_pool6`** (recorded in `.pool_slot_ai_s1`). `opened_in_slot` in every
  plate frontmatter reads `Mashed_pool6` accordingly.

- **`function_create` REQUIRED ON MASTER for 2 LAB rows.** `005ba780` and
  `005ba7f0` had NO Ghidra function object. I opened pool6 read-write (disposable
  clone) and `function_create`d both, then decompiled. **The clone was closed
  WITHOUT save**, so those function objects do NOT persist. The sweep MUST
  `mcp__ghidra__function_create` at `0x005ba780` and `0x005ba7f0` on the master
  before plating/symbol work. Bodies: `005ba780`=0x005ba780..0x005ba7ee (110 B,
  renderer teardown method, obj slot +0x34); `005ba7f0`=0x005ba7f0..0x005baa3e
  (590 B, renderer per-frame update method, obj slot +0x30). Both are installed
  indirectly by the constructor `005b9f30` (which writes the four method pointers
  at obj `+0x28/+0x30/+0x34/+0x3c`), hence neither has direct callers.
  - `005b9f30` is named `LAB_005b9f30` in hooks.csv but the master ALREADY carries
    a function object there (no create needed). Recommend hooks.csv rename
    `LAB_005b9f30 -> FUN_005b9f30`.

- **Subsystem confirmation**: all 49 CONFIRMED `audio` (no subsystem
  reclassifications). The bucket is the **RwaDS audio renderer** — two functional
  families sharing one renderer/source object:
  - **DirectSound** SW/HW voice + DS3D: the per-frame source commit `005b8c40`,
    voice accessors `005b92e0/005b9320/005b93a0/005b9410/005b9460/005b8be0`, the
    DS3D listener update `005ba7f0`, 3D-property dispatch `005baa60`, buffer
    create/QI/release chain `005bbd50/005bbdb0/005bbed0/005bbf30/005bbf60/005bc020`,
    streaming-buffer driver `005bb000`, software 16-bit saturated mixer `005bb5b0`,
    capture-device init/teardown `005bc470/005bc640/005bc450/005bc690/005bc750`,
    Lock/Unlock commits `005bc050/005bc0f0`, dB/category classifier `005bc190`.
  - **DirectShow** media-type glue: AM_MEDIA_TYPE deep-clone `005bc890`, deep-free
    `005bc900`, WAVEFORMATEX/MPEGLAYER3 builder `005bc920`, WAVEFORMATEX→
    AM_MEDIA_TYPE wrap `005bcab0`, MEDIASUBTYPE-GUID-from-tag `005bcb80`.

- **Subsystem-characterization CORRECTION (batch_x s6).** The prior batch_x s6
  rows label the `005bba60` cluster "RwaDSRenderer **DirectShow** filter; audio
  backend implemented via DirectShow filter graph". Live decomp shows this is only
  partly true:
  - `005bba60`, `005bbac0`, `005bc050`, `005bc0f0` contain **no DirectShow
    construct** — they are DirectSound software-vs-hardware dispatch (keyed on
    `caps+0x50 & 8`) + IDirectSoundBuffer Lock/Unlock. NOT DirectShow.
  - `005bc890/005bc900/005bc920/005bcab0/005bcb80` ARE genuine DirectShow
    (AM_MEDIA_TYPE, CoTaskMemAlloc 0x48, MEDIASUBTYPE GUID `{tag-0000-0010-8000-
    00AA00389B71}`).
  Subsystem stays `audio` either way; flagged in plate `005bba60.md` + [[U-7011]].

- **Recurring struct** (cited across the bucket; see [[U-7000]]): the audio
  source/renderer object — flags bitfield at `+0x28` (source) / `+0x78`
  (renderer), caps-owner ptr at `+0x94` (source) / `+0x20` (renderer), caps byte
  `caps+0x50` (bit `0x8` = hardware-mix select, see [[U-7001]]), hardware-voice
  handle `+0x11c`, COM interface ptrs `+0xdc/+0xe0/+0xe4`, intrusive list ptrs
  `+0x120/+0x124`. The constructor `005b9f30` installs method pointers at
  `+0x28/+0x30/+0x34/+0x3c`.

- **New uncertainties filed this session** (range U-7000..U-7099 → used
  **U-7000..U-7025, U-7027, U-7028, U-7029**; U-7026 unused). All are
  data/struct/COM-vtable-identity semantics, **NON-BLOCKING** for C2 of these
  bit-identical leaves. Highlights:
  - U-7000 audio source/renderer struct layout; U-7001 `caps+0x50` bitfield.
  - U-7003 `005b9f30` capability-tier table (`FUN_005aee20(0x28)`/`DAT_00617ff8`).
  - U-7005/U-7006 the two created LAB methods' struct usage.
  - U-7009 streaming-buffer object (`005bb000` thread/semaphore/DSBUFFERDESC).
  - U-7011 the `005bba60` wrapper layout + DirectShow-mislabel correction.
  - U-7015/U-7019 decompiler return-aliasing idioms (`puVar2 & ((iVar1<0)-1)` =
    buffer/status-or-null) in `005bbd50`/`005bc020` — behavior unambiguous from
    caller contract.
  - U-7016 `unaff_retaddr` artifact in `005bbdb0`; U-7024 unrecovered 2nd arg in
    `005bc750` (caller passes `obj+0x44`).
  - U-7022 capture-device block + CLSID/IID (`DAT_005e7180`/`DAT_005e71a0`).
  - U-7027/U-7029 AM_MEDIA_TYPE layout (cbFormat `+0x40`, pbFormat `+0x44`,
    majortype `[0..3]`, subtype `[4..7]`, formattype `[0xb..0xe]`).
  - U-7028 format-GUID identities `DAT_005e6424`(PCM)/`005e6464`/`005e6474`(MP3).

- **Referenced (not re-filed) existing U-IDs**: U-1370 (`005ba760` jumptable),
  U-3188/U-3189 (`005bc860`/`005bc880` tracker), U-3192 (`005baa60`), U-3674/U-3675
  (`005bc190`). Prior `005b9f30` first-pass also holds U-0347..U-0350 for its four
  method-pointer slots.

- **New S-IDs minted** for genuinely out-of-bucket callees:
  **S-5600..S-5629**. Mapping (RVA → S-ID): 005aaa00→S-5600, 005a9280→S-5601,
  005aeb90→S-5602, 005c75b0→S-5603, 005c7570→S-5604, 005c75c0→S-5605,
  005aea50→S-5613, 005aef00→S-5614, 005aef30→S-5615, 005aba20→S-5616,
  005abe30→S-5618, 005ad080→S-5619, 005ad570→S-5620, 005ad540→S-5621,
  005ad420→S-5622, 005ad5f0→S-5623, 005be140→S-5624, 005ae250→S-5625,
  005ae330→S-5626, 005adf30→S-5627, 005ae3a0→S-5628, 005ac650→S-5629,
  005aab70→S-5612, 005abd30→S-5617, 005aabe0→S-5611, 005aa560→S-5608,
  005a9e10→S-5609, 005aee20→S-5610.
  - **SPURIOUS S-IDs to drop**: `S-5606` (was tagged to `005bbf30`) and `S-5607`
    (was tagged to `005bbf60`) in plate `005b9460.md` — both targets are
    **in-bucket** (candidates #30/#31), so treat as in-bucket resolutions, NOT
    new stubs. (Plate text mislabeled them "not in bucket".)

- **Strong C3 candidates (clean, fully-determined leaves, no external data
  semantics)**: `005bb5b0` (16-bit signed-saturated 2-source PCM mixer; lower
  clamp is `-0x7fff` not -32768) and `005bcb80` (MEDIASUBTYPE GUID template — all
  5 writes fixed). `005bc450` (2-field zero-init) is trivially C3 too.

- No master writes were persisted (clone closed without save). All deliverables
  are the 49 bucket plates + this fragment, left UNTRACKED for central finalize.
