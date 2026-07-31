I have everything I need. Here are the four MECHANISM lines:

---

### write_global_call_int0
MECHANISM: Seeds `CONFIG.target_global` with `input>>>0` (uint32), calls `fn(0)` ÔÇö one stack arg always literal 0, no out-pointers, no buffers ÔÇö and returns fn's return value as the sole observable; no post-call global reads. CONFIG: `target_global` only. Broader than name: any fn whose only arg must be 0 and whose result depends on one injectable global.
EVIDENCE: re/frida/diff_template.js:550-554

---

### audio_list_insert
MECHANISM: Allocates two fresh 12-byte isolated sentinel nodes (next=self, prev=self, val=0), one per side; calls fn(sentinel_ptr, int32_payload) void(ptr,int32); fingerprint = int32 at newHead+8 (ÔêÆ1 if no new head). CONFIG: `tests[]` (int32 payloads) only. Broader than name: any insert-head that allocates a node, stores int32 payload at node+8, and writes the new head to sentinel[0].
EVIDENCE: re/frida/diff_template.js:3013-3044

---

### audio_pool_free
MECHANISM: Allocates two live pool nodes via `CONFIG.alloc_rva_str`(poolPtr, `CONFIG.alloc_tag`), one per side; calls fn(poolPtr, node) void(ptr,ptr) on live game memory; observable is crash-or-no-crash only (1/0) ÔÇö a no-op reimpl that doesn't crash passes. CONFIG: `alloc_rva_str`, `alloc_tag`, `pool_addr_str`; tests[] length drives iteration count, test values unused.
EVIDENCE: re/frida/diff_template.js:2983-3010

---

### music_vol_set
MECHANISM: Allocates a zeroed `CONFIG.buf_size`-byte (default 0x120) struct, sets buf+0x0c as a self-loop (empty circular list); calls fn(buf_ptr, float) void(ptr,float); fingerprint = (buf[+0x38]&0xffffff)|(sentinel_intact<<24). List-walk and secondary-pointer (buf+0x11c=0) paths are both skipped by the zeroed+self-loop setup. CONFIG: `buf_size`, `tests[]` (floats). NARROW: only the +0x38 direct-write path is exercised.
EVIDENCE: re/frida/diff_template.js:4364-4405
