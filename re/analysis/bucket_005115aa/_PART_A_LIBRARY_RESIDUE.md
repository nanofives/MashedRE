---
finding_type: library-residue-halt
applies_to: 71 RVAs in part A (0x005115aa..0x00516a70)
session: batch-z-s4
session_date: 2026-05-19
opened_in_slot: Mashed_pool9
---

# Part A is D3DX9 HLSL shader-compiler band backward-extension

The pre-batch calibration placed the D3DX9 HLSL band end at 0x005112c9 and
the libpng band start at 0x00516bb0, leaving a ~22 KB "gap" of 71 candidates
(0x005115aa..0x00516a70). **All sampled functions in this range are D3DX9 /
libpng library residue.** The batch_z-s4 brief's
D3DX9-BACKWARD-EXTENSION CAVEAT explicitly triggers a halt.

## Evidence (cited)

### 0x005115aa — `ShaderCompiler::AST::FUN_005115aa`
- Pre-existing Ghidra C1 comment (from a prior sweep): "&PTR_FUN_005de67c
  at 0x005de67c — pass-node vtable."
- Function name lives in `ShaderCompiler::AST::` namespace.
- Allocates an AST node, writes a vtable pointer, zeros 23 dwords.

### 0x005115e6 — `ShaderCompiler::AST::FUN_005115e6`
- Same namespace, same vtable pointer at 0x005de67c.
- Copy-constructor variant of 0x005115aa.

### 0x0051167d — `ShaderCompiler::AST::FUN_0051167d`
- `ShaderCompiler::AST::` namespace.
- Pre-existing C1 comment: "0x60 = 96 — allocation rounding for 23-dword (92 B) object."
- AST node factory using `operator_new(0x60)`.

### 0x0051251f — symbol-lookup with HLSL grammar
- Embedded HLSL keyword table: `"dword"`, `"float"`, `"vector"`, `"matrix"`,
  `"string"`, `"texture"`, `"pixelshader"`, `"vertexshader"`, `"NULL"`, `"int"`,
  `"double"`.
- Error code `0x80004005` (E_FAIL / D3DERR_INVALIDARG family).
- Error messages: `"'%s': identifier represents a %s, not a %s"`,
  `"'%s': ambiguous function call"`,
  `"'%s': loop control variable used outside for-loop scope conflicts with a previous declaration in the outer scope; most recent definition used"`,
  `"undeclared identifier '%s'"`.
- Calls `ShaderCompiler::Validation::FUN_00511879` and
  `ShaderCompiler::Validation::FUN_005118cd` (diagnostic emitters).

### 0x00516a70 — libpng `png_set_text_2`-shaped helper (tail of part A)
- Calls `FUN_005208c0` and `FUN_00520930` — these are the `png_malloc` /
  `png_free` thunks (libpng allocator pair, used by `png_set_text` /
  `png_set_text_2` to grow a `png_struct->text_ptr` array).
- Field offsets `+0xbc` (text array), `+0xc0` (text count), `+0xb8` (mode
  bits, OR with 0x200 — `PNG_INFO_tEXt`) match `png_struct` layout.
- Function shape: dword-copy + tail-byte loop is the libpng-style
  `memcpy`/`memmove` inlined twice (palette + text key/value copies).
- Just before the named `png_write_data 0x00516bb0` C1 entry — libpng band
  starts earlier than calibrated.

## Library-residue conclusion

- 0x005115aa..0x00516a70 (part A, 71 RVAs) is **a contiguous mix of D3DX9
  shader-compiler frontend (`ShaderCompiler::AST::*` + `ShaderCompiler::Validation::*`)
  and libpng tail (last 200 B before 0x00516bb0)**.
- The D3DX9 band-end at 0x005112c9 was undercalibrated; the actual D3DX9
  band extends backward through at least 0x005115aa..0x0051685f.
- The libpng band-start at 0x00516bb0 was overcalibrated; libpng extends
  backward at least to 0x00516a70.
- No plates were authored for the 71 part-A RVAs — per the brief
  ("Sessions should NOT promote library residue") only this halt note is
  produced.

## Recommended band updates

- Extend D3DX9 HLSL band backward: end := `0x0051685f` (was 0x005112c9).
- Extend libpng band backward: start := `0x00516a70` (was 0x00516bb0).
- Re-classify the ~22 KB "gap" as fully library-residue; remove from
  future candidate buckets.
- Cross-reference `ShaderCompiler::AST::*` and `ShaderCompiler::Validation::*`
  namespace functions already tagged in the master Ghidra project — at
  least 0x005115aa, 0x005115e6, 0x0051167d are already C1-tagged from a
  prior sweep, implying the calibration list was not refreshed.

## Plate count

- Part B (0x00540080..0x005405c0, 9 RVAs): C1 plates written.
- Part A (71 RVAs): NO plates, library-residue halt per brief.

## Affected RVAs (part A, library residue — NOT plated)

0x005115aa, 0x005115e6, 0x00511636, 0x0051167d, 0x00511741, 0x00511774,
0x005117ba, 0x00511841, 0x0051185d, 0x00511879, 0x005118cd, 0x00511918,
0x0051196e, 0x005119dc, 0x00511a4e, 0x00511ad7, 0x00511b85, 0x00511c4d,
0x00511e02, 0x00511f0f, 0x00512024, 0x00512352, 0x0051251f, 0x00512cbe,
0x00512ea7, 0x005130c0, 0x005131c0, 0x005137a0, 0x005138a0, 0x00513c58,
0x00513cb6, 0x00513f90, 0x00514330, 0x00514780, 0x00514920, 0x00514bd0,
0x00514d00, 0x00514e60, 0x00514ec0, 0x005150e0, 0x005151c0, 0x00515260,
0x00515370, 0x00515660, 0x00515680, 0x005156b0, 0x00515710, 0x00515740,
0x00515770, 0x005157c0, 0x005159d0, 0x00515aa0, 0x00515ae0, 0x00515d50,
0x00515e50, 0x00515ee0, 0x00515f30, 0x00515fd0, 0x005161a0, 0x005161d0,
0x00516320, 0x00516370, 0x005163a0, 0x00516430, 0x00516460, 0x00516570,
0x00516650, 0x00516830, 0x00516870, 0x00516920, 0x00516a70
