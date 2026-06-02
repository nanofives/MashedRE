# SCRIBE_QUEUE fragment — batch_aj session 2 (aj_s2)

Author-only promote-c2 pass. Bucket plates are the C2 deliverable; central
finalize (ghidra-sweep) writes hooks.csv / trackers and commits.

## Queued

2026-06-02  aj_s2  bucket=re/analysis/bucket_input_luajoy_004b64e0_004c06c0  confidence=C1->C2  rvas=004b64e0,004b7140,004b7200,004b7250,004b7330,004b7480,004b7570,004b7960,004b7a70,004b7aa0,004b7be0,004b7df0,004b7ff0,004b8340,004b9540,004b9600,004b9650,004b9730,004b9850,004b9aa0,004ba1b0,004ba210,004c0510,004c06c0

## Notes for the sweep

- **Count**: 24 RVAs, 24 plates authored in the bucket dir. None drift-skipped
  (all 24 were `input/C1` in hooks.csv at session start; none already C2+;
  verified by anchored first-column grep `^0*<rva>,`).
- **Subsystem confirmation**: all 24 CONFIRMED `input` (no reclassifications).
  **Refinement of the prompt's working hypothesis**: these are NOT joypad-remap-
  specific code — they are the **Lua 5.x VM core + base library** that Mashed
  embeds for joypad-remap scripting (CLAUDE.md: "Lua 5.x for joypad remap").
  Decisive string/data evidence read live in Mashed_pool1:
  - `"%d is not a valid tag"` @0x00617574 (FUN_004b9600)
  - `"`%.50s' is not a valid event name"` @0x0061758c, `"event `%.50s' is
    deprecated"` @0x006175d4, `"event `gc' for tables is deprecated"`
    @0x006175b0 (FUN_004b9650)
  - `"cannot change `%.20s' tag method%s"` @0x00617618, `"tag method must be a
    function or nil"` @0x006175f0, tag-method name table PTR_s_gettable_005d8830,
    type-name table PTR_s_userdata_005d8818 (FUN_004b9730 = `lua_settagmethod`)
  - `"memory allocation error (block too big)"` @0x006176a0 (FUN_004ba1b0 =
    `luaM_realloc`-style lua_Alloc)
  - base-library opener FUN_004c0510 registers 23 `{name,cfunc}` pairs from
    PTR_PTR_005d8a70 (entry0 = {name@0x617f00, cfunc@0x4bfeb0}, verified by
    memory_read), sets a tag method, pushes the IEEE-754 double π
    `0x400921fb54442d18`, and sets the Lua global `PI` (DAT_00617f34 = "PI").
- **Cluster role map** (all `lua-vm-core`):
  - lua_State lifecycle: 0x004b7330 (alloc+init, `lua_newstate`),
    0x004b7480 (teardown/close), 0x004b9850 (string/GC block teardown),
    0x004ba210 (6-call sub-table dispatcher), 0x004ba1b0 (lua_Alloc).
  - stack/exec: 0x004b7570 (`luaD_checkstack`), 0x004b7960 (protected reset),
    0x004b7be0 (`luaD_rawrunprotected`/setjmp), 0x004b7a70 + 0x004b7aa0
    (load-chunk exec; ESC=0x1b precompiled-chunk marker), 0x004b7df0
    (`luaL_error` variadic formatter).
  - stack push/pop + globals: 0x004b7140 (push number), 0x004b7200
    (thunk→0x004b7fd0, push C-func), 0x004b7ff0 (capture N slots, push type=5),
    0x004b7250 (pop+setglobal), 0x004b8340 (table setter w/ tag-method dispatch),
    0x004b9aa0 (strlen+intern), 0x004b64e0 (inline memset leaf).
  - tag-method API: 0x004b9540 (settability table @DAT_005d8880, stride 0xf),
    0x004b9600 (tag range validate), 0x004b9650 (event-name validate),
    0x004b9730 (`lua_settagmethod`).
  - base-lib registration: 0x004c0510 (`luaopen_base`-style),
    0x004c06c0 (openlib register loop).
- **Pre-existing stubs noted as resolved-by-reversal** (leave the actual
  STUBS.md edit to the sweep): S-2400 (004b7a70), S-2401/S-2402 (004b7330),
  S-2403 (004c06c0), S-2403..S-2407 (004c0510), S-2404 (004b7200),
  S-2405 (004b9730), S-2406 (004b7140), S-2407 (004b7250), S-2408/S-2409
  (004b7480). Depth-1 callee stubs into functions OUTSIDE this bucket are
  carried, not resolved: FUN_004b7b00/004b7ba0/004b7c70/004b79c0/004bc440/
  004bee20/004b9950/004b96c0/004b9bb0/004b9630/004b7740/004b9ef0/004a42c5 and
  the six FUN_004ba2xx/3xx/4xx sub-inits (S-2923..S-2928 band).
- **Pre-existing uncertainties carried** (all data-semantic, NON-BLOCKING for
  C2 of bit-identical leaves): U-2408..U-2411 (004b7330 lua_State field roles),
  U-2412/U-2413 (004c0510 LAB_004c0560 identity + tag-method value),
  U-2414..U-2417 (004b7480 array element types + +0x60 accumulator),
  U-2927..U-2931 (004b7be0 errorJmp fields), U-3694/U-3695/U-3696 (tag/type
  enum values + slot layout). **No new U-IDs or S-IDs minted** — central sweep
  owns tracker numbering.
- **No new arg_types, no Frida, no build, no re-classify, no hooks.csv write.**
  Per author-only mission: only the bucket dir, this fragment, and
  `.pool_slot_aj_s2` were created. Slot Mashed_pool1 opened read-only and
  `program_close`d; lock released.
