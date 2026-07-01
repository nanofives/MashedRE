# arg_type index (GENERATED — do not hand-edit)

Regenerate: `py -3.12 scripts\gen_arg_types_index.py`
Handlers: 109 in `re/frida/diff_template.js` | registry uses: 1042 across 289 distinct arg_types.

Answer "does an arg_type exist for this signature?" HERE. Open
diff_template.js only to author a NEW handler (its header comments,
lines 1-150, document test-vector shapes per family), then rerun this script.
A registry entry naming an arg_type with no handler here is FATAL at
run_diff pre-flight (see `worker-invented arg_types` feedback memory).

| arg_type | diff_template.js line | registry uses | note |
|---|---|---|---|
| `none` | 213 | 145 |  |
| `int_scalar` | 274 | 135 | int_scalar — single uint32 arg, any integer return type |
| `read_global` | 219 | 96 |  |
| `void_write_observe` | 4101 | 51 | This detects whether both functions write the same value to the same address; the sentinel also confirms the function actually touches that address (if it do... |
| `void_setter_observe` | 369 | 36 | Use for void(uint32) setters that write param_1 directly to a global. Strategy: call fn(value), read back target_global. Both orig and reimpl must have writt... |
| `scalars_to_scattered_globals` | 3921 | 30 | Tests: list of { args:[...] } (or a raw array of args). Unlocks: 0x00417450/0x00417530 sparse-grid (fixed globals + 1/0 return), 0x004299d0 TimeRecord (3 sca... |
| `int_pair` | 246 | 23 | int_pair — two uint32 args |
| `state_machine_observe` | 558 | 13 |  Returns a hex string packing all output globals (32 bits each) so BigInt-sized observables don't lose precision through JSON. |
| `void` | 242 | 13 | void — no args, no return value of interest |
| `draw_quad_observe` | 2831 | 10 | CONFIG fields: vbuf_addr_str   string (hex) — override DAT_00898a20 if needed vbuf_len        int          — override 112 if buffer size differs |
| `struct_call_observe` | 1028 | 9 | tests[i]: { seeds:[{off,type,value}], nested:[{ptr_off,size,fields:[{off,type,value}]}] } type for seeds: u8|u16|u32|s32|f32|u64 ; for observe: u8|u16|u32|s3... |
| `entity_field_set` | 438 | 8 | entity_field_set — fn(int param_1, uint32 param_2): void write to global array. input: [param_1, param_2].  Calls fn, then reads back the written address as ... |
| `ptr_arg_int_get` | 289 | 8 | is left Queued, never falsely GREEN. NOT in SEEDED_ARG_TYPES: we rely on the natural non-degeneracy of a real deref, so a getter that ignores its arg stays t... |
| `track_record_deref` | 3197 | 8 | is_getter         bool      — if true, compare return value; if false (dispatcher), use crash_equal_ok (both sides deref through fn-ptr) record_global_str st... |
| `bgra_encode` | 1191 | 6 | Unblocks: 0x004df8d0 PixEncode1555, 0x004df910 PixEncode4444, 0x004df950 PixEncodeA8R3G3B2, 0x004df980 PixEncodeX4R4G4B4, 0x004df9e0 PixEncodeX8R8G8B8. |
| `float_scalar` | 224 | 6 | ── Simple scalar types ─────────────────────────────────────────────────── |
| `fmt_desc_pair_compare` | 2738 | 6 | bufA fingerprint ^ (bufB fingerprint << 8) -- both buffers since some comparators may set flag bits in either side. For 4-arg form: same shape, with p3/p4 ro... |
| `multi_arg_global_write` | 616 | 6 | out_base      — hex addr of the first written global out_count     — number of consecutive u32 slots to read back input         — array of N param values to ... |
| `cache_setter_observe` | 341 | 4 | tests[i] = { seed:[{addr:'0x..', val:<u32>}], args:[...], obs:['0x..', ...] } obs may be omitted to fall back to CONFIG.obs_globals (array of hex strings). A... |
| `count_header_list_ring` | 4005 | 4 | Observables are ADDRESS-NORMALIZED (count + cmp-field value / found field / -1), never raw pointers, so per-side allocations compare cleanly. Unlocks: 0x005b... |
| `out3_idx` | 385 | 4 | out3_idx — fn(out_buf_ptr, uint32_idx); buf is first arg (12 bytes); returns fn return value. Used for functions like VehicleVec3At9C8Get where output buffer... |
| `outbuf_only` | 1684 | 4 | whose *out = a per-frame-moving DAT_0063d588). Unlocks the single-out-ptr class: SlotSortByModeScore 0x0040b620 (round 19), plus 0041da90 / 00484c70 / 004952... |
| `thiscall_field_get` | 2490 | 4 | CONFIG.struct_size  : int    bytes to allocate (>= field_off + 8). Default field_off + 64. CONFIG.tests        : flat list of seed values (u32/int/float per ... |
| `bytes_inplace` | 1149 | 3 |  |
| `int_copy_outbuf` | 1853 | 3 | CONFIG.out_buf_size (default 24) bytes as a position-sensitive XOR fingerprint. Real GREEN requires both fingerprints to be non-zero AND equal — proof the fu... |
| `int_ptr2_out` | 250 | 3 | int_ptr2_out — fn(uint32, out_ptr1, out_ptr2); two 4-byte out-slots; returns packed u32 |
| `slot_block_zero` | 529 | 3 | target_global       — hex address of array base (e.g. 0x006403e8) entity_byte_stride  — bytes per slot (e.g. 0xf40) sentinel_value      — optional uint32 pre... |
| `struct_three_write` | 3621 | 3 | observe_offsets  array of byte offsets to read back (default [12, 16, 20]) Tests: list of [val_a, val_b] pairs. Unblocks: 0x005be140 FUN_005be140. |
| `teardown_call_pair` | 3735 | 3 | Tests: flat list (length = call count; values ignored). Unblocks: engine_stop_dispatch (0x00493550), hw_exit_dispatch (0x00493560), engine_stop_helper (0x004... |
| `audio_list_drain` | 2169 | 2 | Build a fresh sentinel, insert N nodes via FUN_005addd0 (original), call orig/reimpl to drain. Observable: 1 = sentinel self-loops (empty) after drain, 0 = n... |
| `audio_sub_struct_dual` | 3301 | 2 | LinkBuffer(p1,p3); returns p1 on success, 0 on failure. With a zeroed 12-byte scratch buf both cleanups are no-ops and both link calls write to the buffer. T... |
| `audio_sub_struct_link` | 3261 | 2 | Prior arg_type 'audio_sub_struct_link' did not exist in this file and fell through to default fn(input), passing a bare uint32 as the pointer arg — both side... |
| `audio_sub_struct_zero` | 3476 | 2 |  |
| `entity_field_add` | 452 | 2 | the residue of the first call. Snapshot the field, call fn(idx,delta), pack (return_value, post-add field) into a fingerprint, then RESTORE the field so Orig... |
| `fastcall_reg` | 2609 | 2 | a 2-element [ecxVal, edxVal] (nargs==2, int mode). For a ptr register the value is replaced by a scratch-buffer address. |
| `font_ctx_float2` | 1548 | 2 | FontSys_InitRenderState (0x00552c10) once before the test loop to guarantee g_FontCtxPtrs[0] is allocated. Without this, the function derefs a NULL slot ptr ... |
| `int2_ptr2_out` | 2283 | 2 | void fn(uint a, uint b, uint* hi, uint* lo).  Two scalar args + two 4-byte out-slots; observable = "hi,lo" hex fingerprint.  Used for AudioShiftAddMul64 (0x0... |
| `sort_dispatch_out4` | 647 | 2 | live globals at quiescent menu → bit-identical sorted output.  input: {sel, dir} |
| `sprite_table_dispatch` | 3027 | 2 | function takes 9 args — only the first arg matters; remaining stack bytes are garbage but the patched callee only reads its first arg. Tests: flat list of sl... |
| `vec3_global_mul_observe` | 2684 | 2 | Strategy: write test vec3 to globals[idx*stride+0/4/8], save original, call fn(idx), read back globals as 3 u32 fingerprints, restore originals. Both orig an... |
| `vec3_ptr` | 235 | 2 |  |
| `alloc_check` | 1462 | 1 | ── alloc_check ────────────────────────────────────────────────────────── Call(size, tag) for each test; encode result as (align_mod4 * 256 + header_diff). A... |
| `allocator_nonnull` | 3523 | 1 | CONFIG fields: none beyond standard. Tests: flat list (length = call count; values ignored). Unblocks: 0x004c5890 RwTexDictionaryCreate (demoted in frida-swe... |
| `arena_block_free_predicate` | 2354 | 1 | block+8 -> headNode, block+0xc = end-sentinel value; headNode+0 (its next) == sentinel for the fully-free case, != sentinel otherwise.  No pool. Used for Aud... |
| `audio_list_count` | 2229 | 1 | N nodes WITHOUT the audio pool (pool is not ready at diff-attach), so the traversal body is actually exercised.  Read-only => one structure, both sides.  Use... |
| `audio_list_find_index` | 2253 | 1 | fn(anchor, key) -> int index-of-key or -1.  Hand-build circular list (next@+4, key@+8) from test.payloads; query test.key.  Read-only. Used for AudioListInde... |
| `audio_list_min_select` | 2313 | 1 | pointer; map it back to its index so the A/B compares logical selection (per-side pointer identity is meaningless).  Used for AudioListMinKeySelect (0x005b07... |
| `audio_list_remove` | 2131 | 1 | Build a fresh sentinel, optionally insert a node via the ORIGINAL FUN_005addd0 (insert_rva_str), then call orig/reimpl to remove. Observable: 1 if found (non... |
| `buf_field_set` | 3344 | 1 | COM branch is never taken; the function exercises only the two field writes. Tests: flat list of param_2 values. Required CONFIG: buf_size, field_offsets (de... |
| `cache_roundtrip` | 314 | 1 | args entries: a number is passed verbatim; `null` is the out-ptr slot (replaced by a poisoned 4-byte buf). signature.args must match (the out-ptr slot is 'po... |
| `car_slot_init` | 666 | 1 | input: { idx, guard_val } — param_1 = idx; guard field at 0x7f105c+idx*0x4c is set to guard_val. Calls fn(idx), then reads back the 4 fields (offsets +0, +0x... |
| `contact_history` | 375 | 1 | contact_history — set up slot 0 of a fake vehicle contact table, call fn(geom, vehicle) input: { slot_contact_id, slot_active, geom_contact_id } |
| `cstr_ret_offset` | 1221 | 1 | compares the returned pointer as a byte offset from buf (orig vs reimpl). -1 means the returned pointer was null. test: { str: "filename.ext" }. Harness-exte... |
| `cursor_back` | 473 | 1 | DAT_0067f17c and DAT_0067f184 as observable output packed into a uint32. Also saves/restores DAT_0067e9fc (written by callee FUN_0042f6b0) so it doesn't leak... |
| `device_transform_dispatch` | 762 | 1 |  |
| `dsound_secondary_init` | 1967 | 1 | Calls vtable[0] (QI), vtable[5] (secondary init), vtable[2] (Release). Strategy: build fake IUnknown with 6-slot vtable; stubs anchored in array to prevent G... |
| `eax_implicit_ptr` | 2398 | 1 | scratch buffers (so dereferences inside the target don't AV). The harness allocates N scratch buffers (32 bytes each) and rewrites the input list so each tes... |
| `endian_pack` | 1395 | 1 | into a 4-byte source slot; construct a pointer-to-pointer (out_ptr_ptr) and call fn. Read the output buffer bytes as a fingerprint and compare orig/reimpl. T... |
| `esi_idx_ecx_outbuf4` | 1760 | 1 | C3                 ret  CONFIG.tests : scalar integer indices (incl. negatives / >=5 / the 3 case). |
| `float3_scalar_ret` | 232 | 1 | cosine-ease lerp 0x00422440). input is a [a, b, t] triple; registry signature must be {ret:'float', args:['float','float','float']}. The framework reads the ... |
| `fmt_desc_copy` | 853 | 1 | input: { f00, f04, f05, f10, zero_init } — src field values; zero_init flag. Uses two 0x20-byte buffers (fmtSrcBuf, fmtDstBuf). Returns packed u32: dst[+0x04... |
| `fmt_desc_ptr` | 841 | 1 | fmt_desc_ptr — fn(ptr_to_fmt_desc) -> int32. input: { f04, f10, f14 } — u32 values written to struct offsets. buf must be pre-allocated (0x20 bytes); zeroed ... |
| `fmt_global_scan` | 884 | 1 | fmt_global_scan — fn(key_ptr) -> pointer. input: array of 16 u8 bytes forming the format key. Returns pointer value as uint32 (comparable; NULL=0). |
| `fmt_key_compare` | 1514 | 1 | Each test is { a: [16 bytes], b: [16 bytes] }. Allocates a 32-byte scratch buffer; writes a to [0..15], b to [16..31]. Calls fn(buf, buf+16) for both Orig an... |
| `fmt_table_search` | 871 | 1 | fmt_table_search — fn(ctx_ptr, desc_ptr) -> uint32 (1 match, 0 no-match). input: { count, entry_ptr } — writes count at ctx+0x24, ptr at ctx+0x28. fmtCtxBuf ... |
| `font_matrix_push` | 1600 | 1 | safely exercisable. Tests with depth in (1..30) deref unallocated slots and AV both sides identically — they're not on the registry's test list, but the prel... |
| `free_via_alloc` | 1488 | 1 | ── free_via_alloc ─────────────────────────────────────────────────────── Allocate two blocks (via alloc_rva), free each via Orig/Reimpl. Success = 1 (no cra... |
| `guid_from_tag` | 1274 | 1 | test: tag (uint32). Calls fn(tag, outX) into two 16-byte buffers (preset to 0xCC), compares 16-byte fingerprints. Harness-extension arg_type added 2026-06-04... |
| `idx_out2` | 390 | 1 | idx_out2 — fn(uint32_idx, out_ptr1, out_ptr2); two 4-byte out-slots in shared buf. Returns fn return value. Used for functions like VehicleCarStateRead. |
| `int2out` | 1720 | 1 | 4-byte value to each out and returns a value. Compares both out buffers AND the return (packed fingerprint "<a>,<b>:<ret>"). CONFIG.tests is a list of int in... |
| `int_outbuf4` | 1642 | 1 | Strategy: allocate two 4-byte buffers (one per path), zero each before each call, call fn(idx, buf), read back 4 bytes as packed uint32 (little-endian finger... |
| `int_with_out_ptr` | 299 | 1 | int_with_out_ptr — uint32 arg + 4-byte output buffer; returns function's return value |
| `large_buffer_save_restore` | 3790 | 1 | Unblocks (harness side): data_zero_fill (0x004924f0) — NOTE: C3 promotion still blocked by anti-island rule (5 of 6 callees at C1). This arg_type is infrastr... |
| `matrix_rotate` | 783 | 1 |  |
| `matrix_rotate_inner` | 803 | 1 |  |
| `matrix_scale` | 821 | 1 |  |
| `out_buf_fmt_2` | 2899 | 1 | Tests: [p1, p2] pair (or a single int for the common p2=0 case). CONFIG.out_buf_size: per-buffer size (default 32). Both buffers zeroed before each call. |
| `pcm_pack` | 1115 | 1 | Added 2026-06-04 (c3-batch-ab-s4) for 0x005c9770. Per test {src:[int32...], count:N}: write source ints into a shared src buffer, call Orig/Reimpl into two s... |
| `pcm_sat_add` | 1246 | 1 | Writes a[]/b[] as int16 into shared src buffers, zeroes two out buffers, calls fn(outX, srcA, srcB, n*2), compares out fingerprints (n*2 bytes). Harness-exte... |
| `ptr_nonnull_check` | 511 | 1 | where pointer equality is meaningless but null/non-null is the observable. `input` is an optional mode-flag value to pre-write to CONFIG.target_global before... |
| `ptr_ptr_entity_set` | 3140 | 1 |  If *target_global == NULL at call time, the write crashes — the harness returns 0 for both sides (null-guard observable). Both paths must agree. |
| `ptr_scratch_field` | 2205 | 1 | Allocate one zeroed scratch buffer (read-only target), seed the test byte at CONFIG.field_offset (default 0x54), call Orig/Reimpl, compare returns. Used for ... |
| `ptr_zero_pair` | 1296 | 1 | fn(uint32* p): zeroes p[0] and p[1]. Preload both dwords with a sentinel plus a guard dword at +8 (must stay untouched), call, compare 12 bytes. test: sentin... |
| `renderer_field3c_set` | 1321 | 1 | with hwvoice embedded at +0x140 (so +0x11c -> base+0x140, mirror at +0x174). test: { val: uint32, hw: 0|1 }. Observable: [+0x3c]:[hwvoice+0x34] hex. Harness-... |
| `resource_loader_4arg` | 3562 | 1 | type_str is embedded as a NUL-terminated UTF-8 string in scratch memory. CONFIG fields: none beyond standard. Unblocks: 0x004997b0 Win32ResourceLoader. |
| `seed_field_read_field` | 3846 | 1 | seed values (uint32). Observable = the read_off bytes after the call. Unlocks: 0x00483a30 Replay_Rewind (seed_off=0x18, read_off=0x1c, 4 bytes: copies *(p+0x... |
| `semaphore_create` | 3390 | 1 | bit0 = (ret-non-null) ; bit1 = (handle stored at *buf was non-null). Per-side buf addrs differ — ret-pointer-identity is meaningless across sides; but ret-no... |
| `sentinel_array_ptr` | 398 | 1 | Writes the array into buf. For orig, calls via a hand-written thunk that sets ECX=0 and EDX=buf before jumping to the target. For reimpl, calls as cdecl(0, b... |
| `slot_quad_set` | 3669 | 1 | slot_field_count int (default 4) — number of dwords to read back. Tests: list of { idx: int, vals: [v0, v1, v2, v3] } objects. Unblocks: 0x00422ac0 FUN_00422... |
| `source_loop_set` | 1358 | 1 | test: { loop: 0|1, hw: 0|1, pre28: uint32, prehw: uint32 }. Observable: [+0x28]:[hwvoice+0xcc] hex. Harness-extension arg_type added 2026-06-04 (c3_batch_ab ... |
| `spin_angle_observe` | 3077 | 1 | vbuf_addr_str   string (hex) — vertex buffer base (default '0x00898a20') vbuf_len        int          — vertex buffer size (default 112) angle_global_str str... |
| `structptr_seeded_array` | 3883 | 1 | callee may re-read them as float). Observable = concat of read_offs bytes. Unlocks: 0x004c1c80 ViewportDimsSet (struct_size>=0x78, gate@+0x04 stays 0, array ... |
| `sub_struct_dispatcher` | 1932 | 1 | Strategy: allocate 3 scratch buffers, call fn(b0, b1, b2); compare return address == b0 address (return value must equal first arg). Both paths route through... |
| `thiscall_nested_field_get` | 2538 | 1 | the field at inner+inner_off. CONFIG: outer_off, inner_off, ret_kind('u32'| 'float'), struct_size, inner_size. Precedent consumer: 0x004c0b10 (*(*(this+0xa0)... |
| `thread_desc_init` | 1891 | 1 | Strategy: allocate 5x4=20 byte scratch buf; fill with sentinel 0xDEAD????; call fn(buf, p2, p3, p4); read back 5 fields; return packed fingerprint. Both orig... |
| `time_diff_decompose` | 259 | 1 | time_diff_decompose — fn(int time_a, int time_b, u32* sign, int* min, int* sec, float* csec). void return; four out-ptrs in a single 16-byte buf. input: [tim... |
| `transform_point` | 714 | 1 |  |
| `trig_text_draw` | 2957 | 1 | CONFIG.draw_callee_rva_str: hex addr of the draw callee (default '0x00427ff0'). Signature of the callee is void(uint32, float, float). Tests: [sprite_id, x, ... |
| `uint32_scalar` | 700 | 1 | float_scalar |
| `vec2_normalize` | 729 | 1 |  |
| `vec2_ptr` | 704 | 1 | float_scalar (default) |
| `vec3_lerp` | 1820 | 1 | { a:[x,y,z], b:[x,y,z], t:float }. Fills a/b/t, calls fn(out,a,b,t), reads the 3 out floats as a packed-bits fingerprint, compares. Validated on Vec3Lerp 0x0... |
| `vec3_normalize` | 744 | 1 |  |
| `void_step_global` | 422 | 1 | Writes DAT_0067e9f8=0 (slot 0). Calls fn(step). Returns cursor at 0x0067ed40 after the call (as int32, compared between orig/reimpl). Used for MenuCursorStep... |
| `wavefmt_copy` | 1430 | 1 | Tests AudioWaveFmtCopy-style fn(src_ptr, dst_ptr, swap_flag) -> src_ptr. For each test {src:[16 bytes], swap}: write src data into srcBuf, zero dstBuf, call ... |
| `write_global_call_int0` | 361 | 1 | write_global_call_int0 — write sentinel to target_global, call fn(0), return value Use for getters where non-trivial domain requires injecting known values. |
| `audio_list_insert` | 2097 | 0 | Build a self-referential 12-byte sentinel in fresh memory (isolated from live game state), call fn(sentinel, payload), read back new head node[2]. Observable... |
| `audio_pool_free` | 2067 | 0 | Allocate a node via FUN_005ae800(&DAT_009146c0, tag), then free it. Success = 1 (no crash), 0 on crash. Both orig and reimpl must return 1. CONFIG: alloc_rva... |
| `music_vol_set` | 3441 | 0 | runs zero iterations and the secondary branch is skipped. Tests: flat list of float volume values. Observable: low-24 fingerprint of buf packed with (sentine... |

## Registry arg_types with NO dispatch handler in diff_template.js

run_diff.py pre-flight refuses these (FATAL) unless the name happens to
appear elsewhere in the JS. Most are historical worker entries that
predate the 2026-06-12 pre-flight and were never run; treat any hook
using one as NOT diffable until a handler is authored.

- `aabb_sphere_overlap` (1 uses)
- `abs_ranges_setter` (4 uses)
- `abs_region_zeroer` (1 uses)
- `abs_scan_flag` (1 uses)
- `abs_table_state_setter` (1 uses)
- `abstable_ptr_zero` (1 uses)
- `any_slot_nonzero` (1 uses)
- `arg_default_memcpy_abs` (1 uses)
- `arg_flag_branch_getter` (1 uses)
- `arg_scattered_globals` (1 uses)
- `arg_table_linear_search` (1 uses)
- `arg_to_global_ret` (1 uses)
- `array_fill_2way` (1 uses)
- `bitfield_range_set` (1 uses)
- `bitmap_alloc_slot` (1 uses)
- `bitmap_blit` (1 uses)
- `bounded_struct_push` (1 uses)
- `bounded_table_signselect_clamp` (1 uses)
- `bounded_thunk_orflag` (1 uses)
- `byte_args_to_globals` (1 uses)
- `byte_counter_struct` (1 uses)
- `byte_idx_table_bitclear` (1 uses)
- `bytes_inplace_3` (2 uses)
- `case_insensitive_ncmp` (1 uses)
- `circular_dll_search` (1 uses)
- `circular_list_search_node` (1 uses)
- `circular_str_search_ci` (1 uses)
- `cond_deref_get` (1 uses)
- `cond_table_get` (1 uses)
- `const_return` (5 uses)
- `container_record_set` (3 uses)
- `copy_arg_to_globals` (1 uses)
- `deref_byte_flag` (1 uses)
- `deref_float_field_rmw` (1 uses)
- `deref_p1field_glob_set` (6 uses)
- `deref_struct_set` (10 uses)
- `dll_get_nth` (1 uses)
- `dll_head_insert` (1 uses)
- `dll_insert_head` (1 uses)
- `dll_merge_swap` (1 uses)
- `dll_remove_count` (1 uses)
- `dll_unlink` (1 uses)
- `double_deref_ptr_get` (1 uses)
- `double_deref_vec3_get` (2 uses)
- `double_indexed_float_mul` (1 uses)
- `eax_dest_memcpy_init` (1 uses)
- `eax_ecx_float_hash` (1 uses)
- `eax_ecx_insert` (7 uses)
- `eax_edi_out` (1 uses)
- `eax_implicit_int` (2 uses)
- `eax_implicit_void` (2 uses)
- `eax_out_2float` (1 uses)
- `eax_struct_deref_write` (1 uses)
- `eax_struct_stack_out` (1 uses)
- `ebx_edi_global_find` (1 uses)
- `edx_ebx_edi_find` (1 uses)
- `engine_register_funcs` (1 uses)
- `eq_predicate_get` (1 uses)
- `esi_edx_predicate` (1 uses)
- `esi_global_search` (2 uses)
- `esi_struct_init` (2 uses)
- `fastcall_float_clamp` (1 uses)
- `find_node_struct_copy` (1 uses)
- `flag_branch_struct_2way` (1 uses)
- `flag_multibit` (3 uses)
- `float_2ptr_ret` (2 uses)
- `float_planes6_predicate` (1 uses)
- `float_table_read` (2 uses)
- `float_threshold_predicate` (1 uses)
- `float_vec3_lerp_out` (1 uses)
- `gated_args_to_globals` (1 uses)
- `gated_int_predicate` (1 uses)
- `gated_record_eq2` (2 uses)
- `global4_bool_out` (2 uses)
- `global_2level_list_search` (1 uses)
- `global_dll_insert_head` (1 uses)
- `global_field_read` (12 uses)
- `global_fieldoff_clear` (1 uses)
- `global_fieldoff_set` (1 uses)
- `global_float_predicate` (1 uses)
- `global_float_step` (1 uses)
- `global_indexed_float` (1 uses)
- `global_ptr_strided_clear` (1 uses)
- `global_ptrtable_match` (1 uses)
- `global_rec_clear_ret` (1 uses)
- `global_swap` (1 uses)
- `global_switch_member` (1 uses)
- `global_table_linear_search` (1 uses)
- `grid_getter_multiout` (1 uses)
- `harness_limited` (7 uses)
- `heap_alloc_aligned` (1 uses)
- `idx2_record_condset` (1 uses)
- `idx2_table_get` (3 uses)
- `idx2_table_get_outlast` (1 uses)
- `idx_src_abs_memcpy` (1 uses)
- `idx_table_out` (1 uses)
- `index_then_ptr_array` (2 uses)
- `indexed_bit_toggle` (1 uses)
- `indexed_bound_array_get` (1 uses)
- `indexed_const2_set` (1 uses)
- `indexed_float_accum16` (1 uses)
- `indexed_float_sq` (1 uses)
- `indexed_float_sum2` (1 uses)
- `indexed_global_2lvl` (1 uses)
- `indexed_global_field_read` (2 uses)
- `indexed_global_field_write` (2 uses)
- `indexed_global_idiv` (1 uses)
- `indexed_masked_get_out` (1 uses)
- `indexed_table_set` (9 uses)
- `indexed_vec_set` (2 uses)
- `int2_scalar` (2 uses)
- `int_write_observe` (1 uses)
- `linear_scan_find` (1 uses)
- `list_node_const_init` (1 uses)
- `list_walk_self_write` (1 uses)
- `load_be32` (1 uses)
- `multi_array_scatter` (1 uses)
- `multi_deref_global_set` (1 uses)
- `multi_state_list_setter` (1 uses)
- `near_leaf_abs_table` (1 uses)
- `near_leaf_accum_table` (1 uses)
- `near_leaf_arr_to_table` (1 uses)
- `near_leaf_dot_plane` (1 uses)
- `near_leaf_global_str_search` (1 uses)
- `near_leaf_memcmp16` (1 uses)
- `near_leaf_memset2` (1 uses)
- `near_leaf_ptr_array_search` (1 uses)
- `near_leaf_record_builder` (1 uses)
- `near_leaf_seed_arg_obs` (1 uses)
- `near_leaf_seed_globals` (3 uses)
- `near_leaf_seed_multi_obs` (12 uses)
- `near_leaf_seed_outbuf` (3 uses)
- `near_leaf_seed_ret` (1 uses)
- `near_leaf_struct_array_predicate` (1 uses)
- `nested_list_search` (1 uses)
- `nested_struct_op` (1 uses)
- `particle_pool_alloc` (1 uses)
- `pixel_max_alpha` (1 uses)
- `pool_freelist_init` (1 uses)
- `pool_insert_snapshot` (1 uses)
- `pool_remove_snapshot` (1 uses)
- `ptr_buffer_op` (2 uses)
- `ptr_compute_get` (1 uses)
- `ptr_fields_clear` (3 uses)
- `ptr_out_table_get` (4 uses)
- `ptr_table_field_read` (1 uses)
- `quad_buffer_build` (1 uses)
- `range_init` (14 uses)
- `record_array_filter_update` (1 uses)
- `reg_scalar_compute` (1 uses)
- `register_abi_record` (1 uses)
- `ring_copy_5ab980` (1 uses)
- `seed_globals_arg_multiobs` (6 uses)
- `seed_indirect_ctx_obs` (1 uses)
- `stack_pop_snapshot` (1 uses)
- `stack_push_snapshot` (1 uses)
- `state_list_insert` (1 uses)
- `store_be32` (1 uses)
- `strided_color_fill` (1 uses)
- `struct_const_init` (5 uses)
- `struct_delta_flag_init` (1 uses)
- `struct_div_mod_compute` (1 uses)
- `struct_init_3arg_sub` (1 uses)
- `struct_list_float_set` (1 uses)
- `struct_tag_equals` (1 uses)
- `struct_to_out_build` (1 uses)
- `succ_approx_quantize` (1 uses)
- `table_accum_clamp` (1 uses)
- `table_bool_predicate` (3 uses)
- `table_clear` (1 uses)
- `table_ret_ptrout` (2 uses)
- `thiscall_struct_from_table` (2 uses)
- `thunk_cond_or` (1 uses)
- `thunk_field_copy` (1 uses)
- `thunk_float_sub` (1 uses)
- `thunk_list_count` (1 uses)
- `thunk_node_write` (1 uses)
- `transform_vector` (1 uses)
- `trie_walk` (1 uses)
- `two_global_predicate` (2 uses)
- `vec16_copy_set` (1 uses)
- `void_global_transition` (1 uses)
- `write_global_setter` (3 uses)
