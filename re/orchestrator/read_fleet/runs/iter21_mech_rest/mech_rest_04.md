I have all the data I need. Here are the eleven MECHANISM lines:

---

### allocator_nonnull
MECHANISM: Calls fn() with zero arguments and no seeds; observes only null vs non-null of the return pointer (1=non-null, 0=null) ÔÇö a reimpl returning any non-null pointer passes even if size or contents are wrong; CONFIG: none beyond standard; tests[i] values are ignored (list length = call count).
EVIDENCE: re/frida/diff_template.js:4441-4474

---

### arena_block_free_predicate
MECHANISM: Builds a 16-byte block (block+8 -> 12-byte headNode; block+0xc = hardcoded sentinel 0x13572468); headNode+0 = sentinel (free=true) or sentinel^0xff (free=false); calls fn(block) and observes boolean return (1/0); NARROW: offsets +8/+0xc and sentinel value are hardcoded ÔÇö only correct for a `**(p+8)==*(p+0xc)` predicate; CONFIG: none; tests[i] = { free:bool }.
EVIDENCE: re/frida/diff_template.js:3270-3294

---

### audio_list_count
MECHANISM: Builds a harness-only circular linked list of n nodes with next@+4 and ring closed back to anchor; calls fn(anchor) and observes integer return; both sides share the identical structure (read-only traversal, no per-side copy); no CONFIG fields; broader: any fn(anchor) counting a circular list traversed via next@+4; tests[i] = n (node count).
EVIDENCE: re/frida/diff_template.js:3145-3168

---

### audio_list_find_index
MECHANISM: Builds a harness-only circular list (next@+4, key@+8 per node) from test.payloads; calls fn(anchor, key) and observes int return (index or -1); both sides share the identical list (read-only, no per-side copy); no CONFIG; broader: any circular-list linear search fn(anchor, int_key) whose nodes carry key@+8 and next@+4; tests[i] = { payloads:[int...], key:int }.
EVIDENCE: re/frida/diff_template.js:3170-3198

---

### audio_list_min_select
MECHANISM: Builds a harness-only circular list (next@+4, payload_ptr@+8; payload+0x54 -> keystruct; keystruct+0x10 = key); calls fn(anchor, thresh); maps the raw return pointer back to its payload index (-1=null, -2=unknown ptr) so comparison is logical rather than pointer-identical across sides; no CONFIG; NARROW: node/payload/keystruct layout fully hardcoded; tests[i] = { keys:[uint...], thresh:uint }.
EVIDENCE: re/frida/diff_template.js:3227-3268

---

### audio_list_remove
MECHANISM: Per-side: builds a fresh sentinel-based circular list and optionally inserts one node via the original fn at CONFIG.insert_rva_str; calls fn(sentinel, payload) and observes null vs non-null return (1=found, 0=not-found) only; NARROW: insert always uses the original fn; post-removal list state is never fingerprinted; CONFIG: insert_rva_str; tests[i] = { payload:int, present:bool }.
EVIDENCE: re/frida/diff_template.js:3047-3083

---

### buf_field_set
MECHANISM: Per-side zero-filled buffer of CONFIG.buf_size (default 0x120); calls fn(buf, param_2_u32); reads dwords at CONFIG.field_offsets[0] and [1] (default [0x74, 0x78]), folds each to 16-bit XOR of its two halves, packs as a uint32 fingerprint; NARROW: only 2 offsets observed ÔÇö other buffer writes are invisible; broader: parameterized offsets make it usable for different struct layouts; CONFIG: buf_size, field_offsets.
EVIDENCE: re/frida/diff_template.js:4268-4311

---

### cache_roundtrip
MECHANISM: Scatter-seeds absolute globals (input.seed=[{addr,val}]); poisons harness buf with 0xCCCCCCCC; calls fn(args: null->buf, others->u32); fingerprints `<ret_hex>:<out_hex>` from return value and buf; save/restores all seeded globals; broader: fits any fn(mixed_u32_args + one_out_ptr) requiring pre-seeded globals for non-degenerate discrimination; no CONFIG keys beyond standard; per-test: seed, args.
EVIDENCE: re/frida/diff_template.js:487-516

---

### car_slot_init
MECHANISM: NARROW ÔÇö hardcoded to global array at 0x7f1058, stride 0x4c: saves/zeros fields at offsets +0/+0xC/+0x10/+0x14, sets guard at 0x7f105c+idx*0x4c = input.guard_val, calls fn(idx), packs low bytes of 4 fields as uint32 fingerprint, restores all; CONFIG: none; tests[i] = { idx:int, guard_val:uint }.
EVIDENCE: re/frida/diff_template.js:894-930

---

### contact_history
MECHANISM: Seeds outer-scope shared buffers vehicleBuf (0xC80 b) / geomBuf (0x40 b): vehicleBuf+0xBFC=slot_contact_id, +0xC7C=slot_active; geomBuf+0x34=geom_contact_id; calls fn(geomBuf, vehicleBuf) and observes return value only; NARROW: offsets hardcoded; buffer mutations are never read back ÔÇö a reimpl returning the correct scalar while miswriting the struct passes silently; CONFIG: none; tests[i] = { slot_contact_id, slot_active, geom_contact_id }.
EVIDENCE: re/frida/diff_template.js:568-577, 1185-1189

---

### container_find_scalar
MECHANISM: Builds a harness-only [ptr@+0, count@+4] container backed by a fresh scratch array of input.data ints; calls fn(container, input.key) and observes int32 return; input.count overrides length (may be negative to exercise count<=0 edge cases); both sides share the identical container (read-only); no CONFIG; broader: any fn(container_ptr, int_key) with this 2-field layout; tests[i] = { data:[int...], key [, count] }.
EVIDENCE: re/frida/diff_template.js:460-482
