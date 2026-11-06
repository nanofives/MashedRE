# RESULT D4-SAVE-B: option B (file wrappers as the exe save path), stopped at a scope decision

Verdict: **DECISION.** No code, no PREREG, no arms, no default touched. The four bodies W0-3 names can be
written as shared code, but with them alone the exe's save would write and read nothing. VFS_Open
dispatches into the RtFS manager and a registered file system, and the exe has neither. Making option B
work means porting about 30 more functions, all vendored RW toolkit code. That is a scope call for Mariano.

Source: `re/tools/disasm_fn.py` on `original/MASHED.exe.unpatched` (anchor
BDCAE093A30FBF226BDD852B9C36798A987AEE33B3AE82BF7404B0336EFD3C0E). Call xrefs: an E8 rel32 scan of `.text`.
That scan finds direct calls only, so indirect callers are not listed.

## 1. The four bodies, read from the bytes (for whoever picks this up)

### RwStreamClose 0x004cc160 (`int f(RwStream* s, void* data)`)
- 0x004cc164..0x004cc169: `mov esi,[esp+0x10]` / `mov eax,[esi]`. **No NULL test** (confirms NEXT_SESSION:97-98).
- Jump table 0x004cc214 on (type - 1), type > 4 takes the default at 0x004cc1eb:
  - type 1 -> 0x004cc1be: result 1.
  - type 2 -> 0x004cc178: `FUN_00550910(s[3])`. 0x004cc186..0x004cc18a `neg/sbb/inc` gives result = (ret == 0).
  - type 3 -> 0x004cc18d: result 1. If `s[1] != 1` and `data != NULL`: `data[0] = s[5]`, `data[1] = s[3]` (0x004cc1a1..0x004cc1a9).
  - type 4 -> 0x004cc1ae: if `s[3] != 0`, call `s[3](s[7])`. Result 1.
  - default 0x004cc1eb: error block {1, FUN_004d7ff0(0xe)} into FUN_004d8480, return 0. **The stream is not freed on this path.**
- 0x004cc1c3..0x004cc1e0: if `s[8] != 0`: `(*(RwGlobals+0x11c))(*(RwGlobals + DAT_007d45c4), s)`. Return the result.

### RwStreamOpen 0x004cc230 (`RwStream* f(int type, int access, void* data)`)
- 0x004cc230..0x004cc24b: `s = (*(RwGlobals+0x118))(*(RwGlobals + DAT_007d45c4), 0x30404)`.
  If it returns NULL, 0x004cc25a jumps to 0x004cc31f, which calls the free slot +0x11c with that NULL and returns 0.
- 0x004cc268..0x004cc276: `s[0] = type`, `s[1] = access`, `s[8] = 1`.
- Jump table 0x004cc3e8 on (type - 1), type > 4 takes the default at 0x004cc3bd (error 0xe, free, return 0):
  - type 1 -> 0x004cc286: `FUN_00550bc0(data)`. A result of -1 means free and return 0, with no error emitted.
    Otherwise `s[3] = data`, return s.
  - type 2 -> 0x004cc2a3: the mode is picked by access: 1 = "rb" `0x0061737c`, 2 = "wb" `0x006182a8`, 3 = "ab" `0x006182ac`.
    Then `h = FUN_005507b0(name, mode)` (0x004cc304). If h is non-zero, `s[3] = h`, return s.
    If h is 0, 0x004cc2ce emits {1, FUN_004d7ff0(0x80000002, name)}, frees, returns 0.
    Any other access emits error 0xd (0x004cc2b2), then falls into the same 0x80000002 emit, so two errors.
  - type 3 -> 0x004cc342. Read (access 1): `s[3]=0`, `s[4]=data[1]`, `s[5]=data[0]`. Write (access 2): `s[3]=s[4]=s[5]=0`.
    Append (access 3): `s[3]=data[1]`, `s[4]=data[1]`, `s[5]=data[0]`. Any other access: error 0xd, free, return 0.
  - type 4 -> 0x004cc3aa: copy 5 dwords from data to `s[3..7]`, return s.

### VFS_Open 0x005507b0 (`void* f(const char* name, const char* mode)`)
- Mode flags (ebx) come from FUN_004a3860 substring tests (0x005507c7..0x0055084b), on the strings
  "+" 0x00623de4 (esi), "b" 0x00623de0 (ebp), "r" 0x00617378, "w" 0x00623ddc, "a" 0x00623dd8:
  `r`: ebx = 1, or 0xb with "+". `w`: ebx |= 6, and |= 1 with "+". `a`: ebx |= 0xa, |= 1 with "+", |= 4 with "b".
- 0x0055084e: if `DAT_007dc75c == 0`, the error callback `DAT_007dc768` (if set) gets 6, and the function returns 0.
- Otherwise the scan, with length `(*(RwGlobals+0xf4))(name)`, looks for ':'. The prefix keeps its ':', the same
  layout as VfsFileExists 0x00550b00. List `DAT_007dc754`, compare slot `*(RwGlobals+0xf0)` against node+0xc.
  A miss resumes the scan. With no match it takes the default `DAT_007dc76c`, and if that is NULL, cb(6) and return 0.
- 0x005508ee..0x005508f5: `return FUN_00550580(fs, name, flags, 0, 0)`.

### VFS close 0x00550910 (`int f(file* f)`)
- `fs = f[0x38/4] + 0x28`. EnterCriticalSection(0x009124e0) through the import `[0x005cc068]`, then `f[8] = 1`,
  `(*(fs+4))(f)`, LeaveCriticalSection `[0x005cc06c]`, return 0.

## 2. Why these four alone do nothing in the exe

- **FUN_00550580** (VFS_Open's dispatch, 0x00550580..0x0055066c): returns 0 with callback 4 for a NULL fs.
  It takes the critical section 0x009124e0 and walks the fs's file pool: count `fs[1]`, getter `fs[+0x14](fs, i)`.
  It takes the first object with state `+0x20 == 1`, sets that state to 5, and calls the fs open virtual
  `fs[+0x28](fs, obj, name, flags)`. With no free object it sets `DAT_007dc764 = 8` and callback 8.
- Every later file op goes through `obj[0x38/4] + 0x28`: read `+8` (0x00550950), write `+0xc` (0x00550980),
  seek (0x005509b0), eof `+0x44` (0x00550af0), close `+4` (0x00550910).
- The file systems are registered at boot. `FUN_00551190(0x14, 0x005cfee0, ...)` at 0x0049564e / 0x004956f1 builds one,
  and `0x00550430` list-prepends it (calls at 0x00495663 / 0x00495700; the import calls at 0x00495678 / 0x00495686
  are between them). `0x00550790` sets the default (0x00495725). The manager opens at 0x0049375f -> 0x00550350.
  The piz loader registers through the same manager (0x00550430 at 0x004b66e9, 0x00550790 at 0x004b67b5 / 0x004b6ad6),
  and so does the audio path (0x00550430 at 0x004624d7).
- The exe has no RtFS manager. `Save/GameSaveVFS.cpp` already keeps the VFS words as private zeros, so an exe
  VFS_Open takes the 0x0055084e inactive branch and returns 0. RwStreamOpen(2, ...) then frees its stream and returns NULL, and
  both wrappers return 0. With a knob ON, the exe would write no save and load none. That is a regression, not
  an arm worth measuring, so no PREREG was written.

## 3. The decision (filed as a ledger decision row)

- **A. Port the RtFS chain for the exe.** That means the manager 0x00550350 / 0x00550430 / 0x00550790 / 0x00550580, the
  Win32 file system FUN_00551190 and its vtable (the 0x00551090..0x00551fe0 cluster, about 20 C2 rows in hooks.csv
  plus 0x005514e0 / 0x00551180 at C3), the boot registration in 0x00495630..0x00495725, the stream ops
  0x00550950 / 0x00550980 / 0x005509b0 / 0x00550af0, then the four bodies above and the declared NULL guard. That is
  several lanes. Its scope touches the piz loader, which registers through the same manager.
- **B. The four bodies, plus a declared CRT-backed stand-in** where VFS_Open dispatches (FUN_00550580). The open,
  close and mode code that runs would be faithful, but the bytes reaching the disk would be the same as today.
  This adds a second declared deviation next to the NULL guard.
- **C. Close option B unbuilt.** Keep option C as merged in savelink2: the wrappers are shared and dead in the exe, and the exe
  saves through `Save/GameSave.cpp`'s CRT stand-ins, declared as the exe-only save path. The Save TU link then
  closes at the wrapper boundary, as W0-4 already closes it at the dialog boundary.

Recommendation: **C** for D4. Its cost is zero, and the bytes on disk are the same as A's or B's. Take A only if
the exe should own a real RtFS. If it should, file A with the piz-loader FS question, because that loader uses the same manager.

## Gates
Not run: no source change on `orch/D4-SAVE-B`, so the exe is byte-identical to `orch/integration`.
