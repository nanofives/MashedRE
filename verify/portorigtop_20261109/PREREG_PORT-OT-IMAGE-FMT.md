# PREREG PORT-OT-IMAGE-FMT: the image formats of 0x00493600 on MASHED_ORIG_TOP_RWMOD

Committed alone, before any arm. Branch `orch/PORT-OT-IMAGE-FMT`, stacked on orch/PORT-OT-RWG-DEVSLOTS `fd0f11771`
(orch/integration is at `086427678`, not past it, so no merge). PREREG_PORT-OT-RWG-MODULES.md defines the knob.

## What 0x00493600 registers

`re/match/s15/00493600.c` (C4 match): `FUN_004ce790(&PTR_DAT_005cf74c, 0x004935a0, 0x0052d530)`, then, if that
returned non-zero, `FUN_004ce790(&PTR_DAT_005cf748, 0x004935d0, 0x00513f90)`. Returns 1 when both are non-zero.

The two "pointers" are the strings themselves. Bytes read from `original/MASHED.exe.unpatched`:
- 0x005cf74c: `62 6d 70 00` = `"bmp"`
- 0x005cf748: `70 6e 67 00` = `"png"`

0x004ce790 (`re/match/s20/004ce790.c`, C4 match) is the format registration. It builds `".<ext>"` lower case and
upper case through RwGlobals +0xd0 / +0xfc / +0xf8. It finds the node in the image module's list at +0x21c by
comparing through +0xe8, or allocates a 0x34-byte node through +0x118 on the freelist at image module +0x218 and links
it at the head. Then it stores the read function at node +0x28 and the write function at +0x2c, each only when non-null.
All five slots are bound under the knob (PORT-OT-RWG-MODULES, PORT-OT-RWG-STRINGS).

The four function arguments (capstone over `original/MASHED.exe.unpatched`, `re/tools/disasm_va.py`):

| rva | what the disassembly shows | port |
|---|---|---|
| 0x004935a0 | `push esi` at 0x004935a5, then `push 0x5cf734` ("Reading texture %s\n") and `call 0x496400` (0x004935ab). Then `push 0` and `call 0x4c5c80` (0x004935b2), then `push esi` and `call 0x52cb00` (0x004935b8). Then `add esp,0x10` / `pop esi` / `ret`. Nothing writes eax after the last call, so it returns 0x0052cb00's result. | hand port: the exe stand-in of 0x00496400 (`PortTop_CfgLogDebug`), the bound 0x004c5c80 (`MashedRw_TexDictionarySetCurrent_004c5c80`), then the 0x0052cb00 binding, returning its result. The matched C (void, a 2-argument call through a 1-parameter prototype) does not compile as C++. |
| 0x004935d0 | the same 9 instructions, with `call 0x514330` at 0x004935e8 | the same, with the 0x00514330 binding |
| 0x0052cb00 | BMP reader: RwStream open/read, `'BM'` magic, image create (`re/match/s23/0052cb00.c` header, 22.9%) | RW toolkit routine. librw provides it: `rw::readBMP` (deps/librw/src/bmp.cpp). Bind. |
| 0x0052d530 | (image `[esp+4]`, name `[esp+8]`). `0x4cc230(2, 2, name)` at 0x0052d53e. Writes `0x42` (0x0052d56d) and `0x4d` (0x0052d57f), one byte each, through 0x4cbe80. Reads image +0x18 / +0xc (0x0052d55d / 0x0052d562). Returns the image (`mov eax, ebp` at 0x0052d7c5), or 0 when the open fails (0x0052d7d0). | BMP writer. librw: `rw::writeBMP`. Bind, return the image. |
| 0x00514330 | (name `[ebp+8]`). `0x4cc230(2, 1, name)` at 0x0051434b. Then 0x5173f0 with the string `"1.0.12"` (0x00620e1c) and callbacks 0x5142d0 / 0x5142f0 (0x0051435d..0x00514385). Calls 0x4cdca0 at 0x0051451b. | PNG reader. librw: `rw::readPNG` (deps/librw/src/png.cpp). Bind. |
| 0x00513f90 | (image `[ebp+8]`, name `[ebp+0xc]`). `0x4cc230(2, 2, name)` at 0x00513fb6. Then 0x514d00 with `"1.0.12"` and the same two callbacks (0x00513fce..0x00513ff6). Returns `ebx` = the image (0x005142bd). | PNG writer. librw: `rw::writePNG`. Bind, return the image. |

librw also registers `"bmp"` / `"png"` with these functions in its own format list, at `rw::Engine::init`
(deps/librw/src/engine.cpp:321-322). The node list at image module +0x21c is a separate RW 3.x-layout list, read by
0x004ce8e0 (`re/match/s20/004ce8e0.c`: it walks +0x21c and calls node +0x2c). So 0x004ce790 runs as its matched body on
the RwGlobals record, and its node functions are the bindings above. Neither 0x004ce790 nor 0x00493600 has another
exe body, so no dual copy is added. Both new TUs carry `// RVA-LINT: ORIGTOP-ONLY` and are exe-only.

## When it runs

On the knob path 0x00493710 calls 0x00493600 right after RwEngineStart (`probe_mashed_re.log` of PORT-OT-RWG-DEVSLOTS,
line 184). The RwGlobals record is installed later (line 205), and the modules are registered after piece 4
(line 322). In the original the RW engine has opened its modules by the time 0x00493600 runs. So with
MASHED_ORIG_TOP_RWMOD=1:
- the call site in 0x00493710 records that 0x00493600 was reached and returns 1, as the missing stub did
- `RegisterRwModules` runs the matched 0x00493600 once, right after it has set up the image module (+0x218 freelist,
  +0x21c = 0). The result is logged.

With the knob OFF, the call site keeps the old missing stub (returns 1, no registration).

## Arms and pass criteria

| arm | what | pass |
|---|---|---|
| A0 default gates | `run_arms.py H_DEF B_DEF_K0 B_DEF_K1 B_DEF_K26` | G-CTRL 8284be46 / 67931834 / 31fe043e, B 1d5bf4f9 x3 |
| A1 knob boot | MASHED_ORIG_TOP=1 MASHED_ORIG_TOP_RWMOD=1 (`ot_measure.py probe`) | exit 0. The log has the 0x00493600 line with result 1. A first try that stops inside `rw::Engine::open` right after a memory wait is retried once, and both runs are kept. |
| A2 measure | `ot_measure.py verdict --orig` | Prediction: RwGlobals OK 69 / needs binding 3 (was 67 / 5). Released: 0x004ce790, 0x004ce8e0. +0x21c is non-zero. No row goes from OK to needs binding. Seed columns unchanged (111 / 13). |
| A3 captures | `capflake_runs.py` OFF@0 ON@0 OFF@0 ON@0, RWMOD=1 in every run's environment | all 12 hashes equal across OFF and ON, and equal to the PORT-OT-RWG-DEVSLOTS runs (walk_04 = 4d53d93f87f0332d) |
| A4 lint | `py -3.12 scripts/lint_rva_bodies.py` | `[rva-lint] OK` |
