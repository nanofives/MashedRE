I have all the information I need. Here are the 10 MECHANISM lines, each derived strictly from the code I read:

---

### int_scalar
MECHANISM: Passes one uint32 stack arg (`input>>>0`) directly to fn and returns fn's raw return value; no buffer allocation, no global read-back, no save/restore, and no CONFIG parameterization beyond `arg_type`. Applies to any single-integer-in, any-return-type function regardless of name.
EVIDENCE: diff_template.js:397-400

---

### void_write_observe
MECHANISM: Per-test writes sentinel `t` to `CONFIG.target_global`, optionally seeds `CONFIG.seed_globals` (array of {addr,val}) for both sides, calls fn with no args or fixed `CONFIG.call_args`, reads `target_global` back as uint32; no save/restore (idempotent sentinel pre-write); `crash_equal_ok` counts identical crashes as pass. CONFIG: `target_global`, `seed_globals`, `call_args`, `crash_equal_ok`.
EVIDENCE: diff_template.js:4934-4987

---

### void_setter_observe
MECHANISM: Calls fn(input>>>0) [void return], then reads `CONFIG.target_global` as uint32 observable; no buffer allocation, no save/restore. CONFIG: `target_global` only. Broader than name: any void(uint32) that writes its param to one absolute global ÔÇö not limited to simple direct-assign setters.
EVIDENCE: diff_template.js:533-540

---

### scalars_to_scattered_globals
MECHANISM: Calls fn(t.args... as uint32 scalars); saves/restores arbitrary non-contiguous byte-windows (`CONFIG.observe`:{addr,len}) and stride-indexed array slots (`CONFIG.idx_arrays`:{base,stride,elem_len}) under a live index cursor (`CONFIG.idx_call_str`); fingerprints with FNV-1a; optionally pre-fills windows, runs a prep call, or folds the return value. CONFIG: `observe`, `idx_arrays`, `idx_call_str`, `fold_ret`, `prep_call_str`, `pre_fill_byte`.
EVIDENCE: diff_template.js:4750-4831

---

### int_pair
MECHANISM: Passes two uint32 stack args (input[0], input[1]) directly to fn and returns fn's return value; no buffer, no globals, no save/restore, no CONFIG parameterization beyond `arg_type`. Applies to any two-integer-in, any-return-type function.
EVIDENCE: diff_template.js:369-372

---

### state_machine_observe
MECHANISM: Saves all (input Ôê¬ output) globals, writes test values into `CONFIG.input_globals` (array of {addr,type}), calls fn() [void, no args], reads `CONFIG.output_globals` as packed hex fingerprint, then restores everything; per-entry types: u8/u16/u32/s8/s16/s32. CONFIG: `input_globals`, `output_globals`. Broader: any void() function whose entire observable is global-to-global mutation; input and output global sets are fully independent.
EVIDENCE: diff_template.js:725-787

---

### struct_call_observe
MECHANISM: Per side allocates a zeroed struct buffer (`CONFIG.struct_size`) + up to 2 eight-byte out-bufs (`CONFIG.out_ptrs`); seeds fields and nested sub-struct graphs identically on each side; calls fn(struct[,out0[,out1]]); fingerprints selected offsets (u8/u16/u32/s32/u64) from struct or out-bufs and optionally return; each side's pointers differ in address but match in seeded content so relative-offset reads compare cleanly. CONFIG: `struct_size`, `out_ptrs`, `observe_ret`, `observe`; test: `seeds`, `nested`.
EVIDENCE: diff_template.js:1200-1292

---

### ptr_arg_int_get
MECHANISM: Allocates one SHARED scratch buffer (size `CONFIG.struct_size` or 256), fills it with a deterministic per-seed dword pattern (`seed + o*0x01010101`), passes the SAME physical pointer to both Orig and Reimpl, returns fn's uint32 return; a double-deref function faults on the bad inner pointer (caught as mismatch, never false GREEN). CONFIG: `struct_size`. Broader: any fn(ptr)->int single-level-deref getter, regardless of struct type.
EVIDENCE: diff_template.js:401-421

---

### entity_field_set
MECHANISM: Calls fn(input[0]|0, input[1]>>>0) [void], reads `CONFIG.target_global + input[0] * CONFIG.entity_byte_stride` as uint32 observable; no buffer, no save/restore (assumes idempotent slot write). CONFIG: `target_global`, `entity_byte_stride`. Broader: any fn(int_index, uint32_value) that writes into a strided global array ÔÇö both the base and stride are fully configurable.
EVIDENCE: diff_template.js:618-627

---

### ptr_seed_observe
MECHANISM: Allocates N paired scratch buffers per side (`CONFIG.num_bufs`├ù`buf_size`); seeds identically per-test (flat field values or cross-buffer `ptr_to` pointer wires to build struct graphs); calls fn via `CONFIG.arg_layout` ({buf:i}ÔåÆpointer, {f32}/{i32}ÔåÆscalar from test.scalars, per-test `null_args`ÔåÆNULL); fingerprints selected buffer offsets (f32 as raw bits for bit-identity) and optionally return. CONFIG: `num_bufs`, `buf_size`, `arg_layout`, `observe`, `observe_ret`.
EVIDENCE: diff_template.js:1294-1418
