# RESULT RO-ORACLE-GATE

Run 46 comes first, run 45 (history, unchanged) below it.

# Run 46 (2026-11-14): pizfs mask (L-152 (a)) and the oracle forward

Worker on claude3, lead cmwggc54z02ehmz1bcgkcra7l. Branch `orch/RO-ORACLE-GATE`. Merged `orch/integration` `2c7f97e41`
(merge `635286e09`, so RWC-FRAME-CLUMP and DDKOBJ-REGCONV-FALLBACK-7 are in). Part 1 ran before the LIVING-PC reboot
(handoff `e2e47db2a`). Part 2 ran after it (sections 0, 5, 6, 7).

## 0. Verdict (run 46, final)

**The oracle on the current head, with the pizfs mask in place, gives first divergence at record 0 (tick 1).**

- Pair: exe arm `RO_ROIMG-og1i` (object build `294fae63...`, `MASHED_PORTSTATE_IMAGE=1`) against the original `og1`. Both
  were run in this row on the current head.
- **First VALUE dword: `0x00604604`**, ref `0x00000004` (signed 4), run `0x00000000` (signed 0).
  - It is the FUN_0045d460 store `0x0045d6a5` of run 45 (a), reached from FUN_004669b0 at `0x004669f6`.
  - The exe arm's write watch on `0x00604604` had 0 hits over 600 ticks.
  - This needs a boot-side body (FUN_0045d460 on the ORIG_TOP audio-init path), not a mask. No mask, admission or stand-in
    was added for it. A row for the lead to file.
- **detorig_ticks verdict (NEW blocks): IDENTICAL-FOR-N 17 of 566.** The first NEW block is `0x00898c00` at record 17, the
  `0x0043dfd0` body lane (RO-FIRST-DIVERGENCE (c), PORT-TOP-08). Unchanged from run 44.
- **No reference race is named.** The original-vs-original control (o2 against og1) is not clean (section 6). That noise
  belongs to row RO-REF-CONTROL-NOISE.
- **Self-test:** exe `meta.json` `selftest` gives checks 27,745, fails 0. There are no `[TRACE]` / `[REPLAY]` self-test log
  lines, because the arm AVs in AppDestroy after frame 480 (`exit=3221225477`, IMG-4's teardown AV, the same as run 44).
  `ticks.bin` holds 566 complete records, and all checkpoint dumps are complete.
- **Object build:** `ddkobj.py check` IDENTICAL 1229/1263, DIFFERENT 19, the same as the integration head (section 3).

The run-44 → run-46 change at record 0, image ON:

| | run 44 | run 46 (current head, pizfs mask) |
|---|---|---|
| counted blocks | 260 | 139 |
| VALUE dwords | 32,942 | 5,687 |
| `MASK:pizfs` dwords dropped | n/a | 239 |
| first VALUE dword | `0x00604604` | `0x00604604` |

`0x005f9800` is now explained (IMGPTR) by the fresh exe map. Report: `oraclegate/run46_fd_og1_og1i.md`.

## 1. The mask (done)

`re/frida/replay_masks_manual.tsv` gets family `pizfs` (bound family, L-152 (a), described in the header) with 3 rows. The
same rows are appended to `re/frida/replay_masks.tsv`:

| va | width (B) | what | writers | readers |
|---|---|---|---|---|
| `0x008ad9c0` | 131,072 (`0x008ad9c0..0x008cd9bf`) | piz chunk cache | `0x004b69b4` `rep stosd` (EDI = `0x008ad9c0` at `0x004b69a3`, FUN_004b6940), `0x004b6b88` `rep movsd` (EDI = `0x008ad9c0` at `0x004b6b83`, ECX = `0x8000` at `0x004b6b7e`, FUN_004b6b20) | `0x004b6db2` only |
| `0x008ad868` | 4 | FUN_004b6cd0 counter | `0x004b6ce0` | `0x004b6cd0` |
| `0x008ad878` | 20 (`0x008ad878 0x008ad87c 0x008ad880 0x008ad884 0x008ad888`) | FUN_004b6cd0 counters | `0x004b6d7d 0x004b6d82 0x004b6df2 0x004b6df7 0x004b6e12` | `0x004b6d6c 0x004b6d72 0x004b6de1 0x004b6de7 0x004b6e04` |

Not masked, as the brief requires: `0x008cd9c0`, `0x008eda38`, `0x0090dac0`, `0x007d3e4c..0x007d3e68`
(RO-PIZFS-UNMASKED-READERS). The other words of the `0x008ad800` counter block (`0x008ad860 0x008ad864 0x008ad86c
0x008ad870 0x008ad874`, `0x008ad8a0..`, `0x008ad9a0..0x008ad9ac`) are not masked either. They are outside L-152's list.

**Sole reader `0x004b6db2`: confirmed by a static scan of the anchored binary, this run.**

- Tool: `oraclegate/pizfs_refscan.py` (capstone over `original/MASHED.exe.unpatched`, SHA-256 `BDCAE093...3C0E`).
  Output: `oraclegate/pizfs_refscan.txt`.
- The scan covers every `.text` operand whose memory displacement (any base or index register) or immediate falls in
  `0x008ad800..0x008cd9bf`.
- Exactly three instructions name an address inside the cache:
  - `0x004b69a3 mov edi, 0x8ad9c0`
  - `0x004b6b83 mov edi, 0x8ad9c0`
  - `0x004b6db2 lea esi, [eax + esi + 0x8ad9c0]`
- **The first two are destinations of the store.** Disassembly is in `oraclegate/pizfs_writers_disasm.txt`:
  - EDI from `0x004b69a3` feeds `0x004b69b4 rep stosd`.
  - EDI from `0x004b6b83` feeds `0x004b6b88 rep movsd`.
- **`0x004b6db2` is the one read.** It loads ESI as the source of `0x004b6dc0 rep movsd` and `0x004b6dc7 rep movsb`. Their
  destination is EDI = `[esp+0x24]` (`0x004b6dae`), the caller's buffer. ESI is overwritten at `0x004b6dc9`
  (`mov esi, [ebp+0x20070]`), so the cache pointer does not leave the function.
- The six counters are named only by instructions inside FUN_004b6cd0 (`0x004b6cd0..0x004b6ec1`), as the table lists.
- [UNCERTAIN] Six `.data` dwords hold values that fall inside the cache range:
  - `0x0061d05c` = `0x008b0a07`
  - `0x0061d474` = `0x008c0045`
  - `0x0061eeec` = `0x008c0077`
  - `0x0061eef0` = `0x008c007c`
  - `0x0061ef08` = `0x008c007c`
  - `0x0061f018` = `0x008c0124`

  None is 4-byte aligned. Run 45 found no code reference to `0x0061eee0..0x0061ef00`. This run did not check code
  references to the tables that hold `0x0061d05c`, `0x0061d474` and `0x0061f018`. So "sole reader" is proven for
  instruction operands only. A pointer loaded from one of these data words is not excluded by this scan.
- Section 1 of `replay_masks.tsv` was not regenerated. Re-running `gen_replay_masks.py` on the merged head (facts
  `log/ghidra_funcfacts.jsonl` of the main checkout) also adds an unrelated bound-family row, `0x00914700 MASK:audio`, from
  hooks.csv drift since RO-MASKS. That row is outside this brief, so it was left out. The committed file equals the
  regenerated file minus that row and the count line. **For the lead:** regenerate it in its own row.

## 2. Effect of the mask (score-only, no launch)

`re/frida/detorig_firstdiv.py`, image ON exe arm `RO_ROIMG-fdimg` (run 44, object build `ca1854fc...`), masks
`warp_normalize.tsv,replay_masks.tsv`:

| | run 44 (no pizfs mask) | run 46, ref o2 | run 46, ref og1 (fresh) |
|---|---|---|---|
| first divergent record | 0 (tick 1) | 0 (tick 1) | 0 (tick 1) |
| counted blocks at record 0 | 260 | **140** | **140** |
| VALUE dwords at tick 1 | 32,942 | **5,696** | 5,696-class (same 140 blocks) |
| `MASK:pizfs` dwords dropped at tick 1 | n/a | 239 | |
| first NEW block (detorig_ticks verdict) | `0x00898c00` @ 17, IDENTICAL-FOR-N 17 of 566 | the same | the same |

- 121 of the 123 cache-range blocks leave the count.
- The two that stay:
  - `0x008ad800`: holds the unmasked counter words listed in section 1.
  - `0x008cd800`: holds the head of slot 0, `0x008cd9c0`, which is not masked.
- Reports: `oraclegate/run46_fd_o2_fdimg_pizmask.md`, `oraclegate/run46_fd_og1_fdimg_pizmask.md`.
- **The first VALUE dword of the 140 blocks** is listed without an exe map, because the run-44 exe and map no longer exist.
  The report's first rows are `0x005f99ac..0x005f9bac`, stride `0x40`, 9 dwords:
  - Ref: the file initializer (e.g. `0x00456760`).
  - Run: `0x001cd420..0x001cd500`.
  - Without the map the IMGPTR rule cannot run. In run 44 the same pair, with its map, put the first VALUE dword at
    `0x00604604`.

  So the next divergence after the pizfs mask is, as far as the code reading goes, `0x00604604` at record 0 (tick 1):
  ref `0x00000004` (signed 4), run `0x00000000` (signed 0). That is the FUN_004669b0 / FUN_0045d460 audio-init word of
  run 45 (a) (store `0x0045d6a5`).

  [UNCERTAIN] This rests on run 44's map and on a run-44 exe, not on a run of the current head. Missing evidence: the
  `RO_ROIMG-og1i` arm below, scored with `--exe-map mashedmod/build/ddkobj/mashed_re_ddk.map`.

## 3. Builds, checks and runs done

- **Object build** (`cmd /c "set MASHED_NO_DEPLOY=1&& set MASHED_DDKOBJ=1&& mashedmod\build.bat"`): rc 0.
  `mashedmod/build/ddkobj/mashed_re_ddk.exe` SHA-256 `294fae63b3c06bbdb5bbedd009e273dfcb232e2279232c98f83b2afe45965710`.
  The VS2022 exe was not rebuilt separately.
- **`ddkobj.py check`:**
  - IDENTICAL 1229/1263, DIFFERENT 19 (`0x00401690 0x004332a0 0x0043c5b0 0x0045a3a0 0x00475a00 0x004929d0 0x00493710
    0x00493900 0x00498510 0x00498810 0x004c0c20 0x004c0f00 0x00552890 0x005530f0 0x005533d0 0x005538a0 0x00554200
    0x00554940 0x005572c0`), not-in-map 15, units linked 1210/1488.
  - Refusals 9: externalized-static-register-convention 5, wrapper-redefines-globals 4.
  - These figures equal the merge train's own figures for `orch/integration` at DDKOBJ-REGCONV-FALLBACK-7
    (`INTEGRATION_LOG.md`, 2026-11-13 22:32). This row changes no exe source.
- **Original reference `og1`:**
  - `oraclegate/og_arms.sh orig og1 --ckpt-ticks 0,1,17,18,299,301,600`, under test-slot and mashed_lock, root
    `D:\MashedWT\RO-ORACLE-GATE_root`.
  - rc 0, 600 records x 3245 blocks, 22 ckpts, warp at N=3524.
- **Reference control (original vs original), o2 (run 43) vs og1:** not identical.
  - 3 counted blocks at record 0, and the first NEW block is `0x00614400` at record 3 (IDENTICAL-FOR-N 3 of 600).
  - VALUE dwords at tick 1:
    - `0x0068bc2c`: o2 `0x00000000`, og1 `0x00000004` (signed 4).
    - `0x00771998`, `0x0077199c`, `0x007719a0`: o2 `0x0001a4b2` (signed 107,698), og1 `0x0001a4af` (signed 107,695).
    - `0x007f1030` (frame timer block): o2 `0x1341c1c3` (signed 323,076,547), og1 `0x13419e9b` (signed 323,067,547).
  - Report: `oraclegate/run46_fd_o2_og1.md`.
  - [UNCERTAIN] Whether these are the run-to-run original drift already handled in RO-CONTROL-DRIFT / `ticks_o1_o2_masked`.
    This run did not check them against those masks. They decide whether og1 or o2 is the reference race for the verdict.
- **Self-test:** see section 0 (exe `meta.json` selftest 27,745 checks, 0 fails, post-reboot run). On the original side
  `og1` exited rc 0 with every checkpoint written.
- **Tool fix:** `re/frida/detorig_firstdiv.py` `ExeMap` now sets `lo` / `hi` to None when no map is given. Before, the
  report stopped with `AttributeError: 'ExeMap' object has no attribute 'lo'` whenever `--exe-map` was absent.

## 5. The counter rows: FUN_004b65c0 (caller `0x00495312`)

From disassembly of the anchored binary and of the patched reference binary. The tool is `oraclegate/disrange.py`; output
is in `oraclegate/fun_004b65c0_unpatched.txt`, `oraclegate/fun_004b65c0_patched.txt` and
`oraclegate/fun_004b65c0_callers.txt`. The scan of section 1 started at `0x008ad800`, so it did not see this copy, whose
source base is `0x008ab7e0`.

**FUN_004b65c0:**

| addr | instruction |
|---|---|
| `0x004b65c1` | `mov edi,[esp+8]` |
| `0x004b65c5` / `0x004b65c7` | `test edi,edi` / `je` |
| `0x004b65ca` | `mov ecx,0x82b` |
| `0x004b65cf` | `mov esi,0x8ab7e0` |
| `0x004b65d4` | `rep movsd` |

- The copy is 0x82b dwords = 0x20ac B, from `0x008ab7e0..0x008ad88b`. The source range ends at the last counter
  (`0x008ad888` + 4 = `0x008ad88c`), so it holds all six masked counters.
- Its only caller in `.text` is `0x00495312 call 0x4b65c0`: one rel32 call, no other rel32 jump, no absolute dword
  reference.

**(a) Where the destination buffer lives: on the stack of the function at `0x004952f0`.**

| addr | instruction | effect |
|---|---|---|
| `0x004952f0` | `mov eax,0x20b0` | |
| `0x004952f5` | `call 0x4a3440` | moves ESP by EAX. [UNCERTAIN] I did not read `0x004a3440`; the frame size follows from the epilogue `0x00495339 add esp,0x20bc` |
| `0x00495306` | | stores a cookie (`[0x00616038]` xor) at `[esp+0x20ac]` |
| `0x0049530d` / `0x00495311` | `lea eax,[esp]` / `push eax` | |
| `0x00495312` | `call 0x4b65c0` | |

- So the 0x20ac bytes land at the frame's `[esp+0..0x20ab]`, just below the cookie.
- The buffer is a local of that frame. It is not in the image.

**(b) Is the buffer in the oracle's compared set: no.**

- The compare hashes 3245 blocks x 1024 B from `DATA_LO = 0x005ea000` (`re/frida/detorig.py:22`, `meta.json` `nblk`
  3245, `block` 1024), so `0x005ea000..0x00915400`.
- Its raw words lie inside that range (the `meta.json` `raw` list).
- A stack address of the main thread is outside it. Nothing in the frame copies the buffer back into `.data`. After
  `0x00496400` the frame does:
  - `0x00495326 call 0x4b6590`
  - `0x0049532b..0x00495339`: the cookie check and `add esp,0x20bc`
  - `0x0049533f jmp 0x4a2be9`

**(c) What the patched `0x00496400` does with it in the reference binary: nothing. It returns at once.**

- The reference binary is the root's `original\MASHED.exe`, SHA-256 `b9977dab70607720...fe87`, byte-identical to the
  patched `original\MASHED.exe`.
- Its byte at `0x00496400` is `C3` (`ret`), the disable_log patch. The unpatched binary has `81 EC 04 02 00 00`
  (`sub esp,0x204`) there.
- The caller passes it `push ecx` (`lea ecx,[esp+4]` at `0x00495317`, the buffer, since the `push eax` before
  `0x004b65c0` is still on the stack) and `push 0x5cfe94` (`"Closing piz file %s\n"`).
- Unpatched, when `[0x00772fbc]` is non-zero:
  - `0x00496437` calls `0x004a42c5` with a 0x200-byte local, the format argument and `lea eax,[esp+0x20c]`.
  - `0x00496447` calls `0x004a4254` with that local and `[0x00772fbc]`.
  - [UNCERTAIN] what those two callees are. They were not read, and with the patch they are not reached.
- After the `ret`, `0x00495326` calls `0x004b6590`, which is `jmp 0x4b67a0`. `0x004b67a0..0x004b67d1` reads no caller
  stack word: it calls `0x004b6770`, pushes `[0x007d3e58]` for `0x00550790`, writes `0x007d3e4c`, `0x007d3e58` and
  `0x007d3e48`, then `jmp 0x4b65e0`. [UNCERTAIN] `0x004b6770` was not read.

**Conclusion for the counter rows.** In the reference binary the counters reach the stack copy, and nothing consumes the
copy before the frame returns, apart from the [UNCERTAIN] `0x004b6770` and the `0x004a3440` probe named above. No compared
word is written from it. The counter rows stand on this disassembly, not on a run: no watch was set on the stack buffer.
The cache rows keep their own evidence, the reader `0x004b6db2` (section 1).

## 6. Reference control (o2 vs og1): moved to RO-REF-CONTROL-NOISE

Per the lead, this noise belongs to row **RO-REF-CONTROL-NOISE**, together with the `0x00914700 MASK:audio` regeneration
drift (section 1). Found here, not dug further:

- **18 blocks** differ between o2 (run 43) and og1 (run 46) at any of the 600 records, and the set does not grow later in
  the race:
  - 13 differ at all 600 records, of which 10 are explained at tick 1 by mask, rule or PTR.
  - `0x00771800`: records 0..301, then single records from 334 onward (364 records in all).
  - `0x00614400`: from record 3, 201 records.
  - Three blocks differ only at record 301: `0x007dcc00`, `0x007dd000`, `0x007f0c00`.
  - Script output: `D:/MashedWT/RO-ORACLE-GATE_data/rawdiff_o2_og1.txt`, plus the per-block census in this run's log.
- **Raw-word evidence** (`oraclegate/rawdiff.py`, records 0..12):
  - In block `0x00614400` the only differing raw word is `0x006147d8`, the window-message flag already masked
    `MASK:os` (writers `0x00493eeb`, `0x0049479a`). Values alternate 0/1.
  - **`0x007f102c` / `0x007f1030`** (frame timer):
    - Both runs advance by 50,000 per record.
    - og1 sits 9,000 (`0x2328`) below o2 at every record from 0.
    - Block `0x007f1000` differs at records 0..300 only, which ends at the warp's `pinTimer()` (RESULT_RO-WARP-NORMALIZE).
  - **`0x007f1010`:** o2 minus og1 = 393 at each of records 1..3.
- **Not explained:**
  - `0x0068bc2c`: o2 `0x00000000`, og1 `0x00000004` (signed 4), at all 600 records.
  - The `0x00771800` block after the warp.

## 7. Lock and runs after the reboot

- `mashed_lock.py status` showed `HELD pid=19856 alive=True label=SOAK-P0 since 11:11:26`.
  - PID 19856 was by then an unrelated `pwsh.exe` started 14:30:57, after the 13:56:52 boot.
  - The record holds `owner_start` 1794665447.1, so the tool's acquire path (`_holder_stale` → `_owner_dead`, PID-reuse
    check) treats it as stale.
  - My run's acquire took the lock (`HELD 'mashed_machine' pid=14908 (RO-ORACLE-GATE)`, then released). Nothing was deleted
    by hand.
  - Only `status` prints `alive=True`, because it uses the plain PID check. That is a display fault for whoever owns
    mashed_lock.
- **Exe arm:** `RO_ROIMG-og1i`, under test-slot and mashed_lock. It waited 600 s for memory, then started under the cap
  in slot 5/8.
  - Run: pid 5756, 24 s, `exit=3221225477`.
  - Root: `D:\MashedWT\RO-ORACLE-GATE_root`.

## 4. Next (historical: the post-reboot plan, now carried out in sections 0, 5, 6 and 7)

1. `py -3.12 C:\Users\maria\Desktop\Proyectos\Mashed\.worktrees\orch-integration\re\orchestrator\mashed_lock.py status`
   must show free or no PRIORITY-HOLD / SOLO.
2. From the worktree:
   `test-slot bash -c 'A4_RO_CKPT=0,1,17,18,299,301,600 A4_RO_WATCH=0x00604604,0x008ad9c0 bash verify/replayoracle_20261113/oraclegate/og_arms.sh exe ddk RO_ROIMG-og1i'`
   - Use background mode with a 3,600,000 ms timeout.
   - The exe is already built: `294fae63...`. No rebuild is needed unless the branch moves.
   - Read the `[TRACE]` / `[REPLAY]` self-test lines. The image-ON arm can AV in AppDestroy (IMG-4), and then the
     self-test sits in `meta.json` `selftest`.
3. Score it against the fresh reference. `D` is `D:/MashedWT/RO-ORACLE-GATE_data`:
   `py -3.12 re/frida/detorig_firstdiv.py $D/og1 $D/RO_ROIMG-og1i --exe-map mashedmod/build/ddkobj/mashed_re_ddk.map --json $D/fd_og1_og1i.json --md $D/fd_og1_og1i.md`
4. Settle the reference-control drift (section 3) before naming the reference race in the verdict.

## Evidence (run 46)

- **Committed:**
  - this section
  - `re/frida/replay_masks_manual.tsv`, `re/frida/replay_masks.tsv`
  - `re/frida/detorig_firstdiv.py` (ExeMap fix)
  - `oraclegate/pizfs_refscan.py`, `oraclegate/pizfs_refscan.txt`, `oraclegate/pizfs_writers_disasm.txt`
  - `oraclegate/og_arms.sh`
  - `oraclegate/run46_fd_o2_fdimg_pizmask.md`, `oraclegate/run46_fd_og1_fdimg_pizmask.md`, `oraclegate/run46_fd_o2_og1.md`
  - `raw_sha256_RO-ORACLE-GATE_run46.txt`
- **Raw, not committed** (folder `D:/MashedWT/RO-ORACLE-GATE_data`; SHA-256 below and in
  `raw_sha256_RO-ORACLE-GATE_run46.txt`):

| file | SHA-256 |
|---|---|
| `og1/meta.json` | `2d211470c238d14eab63b1fda1fc7f20024d9125717a6d27be5c0edfee48ccd4` |
| `og1/ticks.bin` | `8cb3819320ad0d6a083e7902d468c0f5e2bd4245d2eaedb5ded873d672e37c00` |
| `og1.log` | `bc4ec66fc5d03846018c395b78d0867729820582aad03ed68c06812958442a73` |
| `fd_o2_fdimg_pizmask.json` | `fa9aa6f258bc17e3ae36601ca7efd600048736f9a9c576e99ec0b7b12f63f9dd` |
| `fd_og1_fdimg_pizmask.json` | `1b7334577796df10918fa0bce757e09d6679bce5f3516ac567c96583596352ed` |
| `fd_o2_og1.json` | `0ba7fe5964cfd9437921e52b84e425434e215807a9fab1099608faef409b4e4b` |
| `build_ddk.log` | `48e472d87834c656f4cd431b03c3c6f7fdf4dc93bded03703648a31ed80d11ab` |
| `ddkobj_check.txt` | `e59f25fb7d7f4e7175504685966762a3b6295e0e6e1ea4b8523e9cab8f359f93` |
| `D:/MashedWT/RO-FIRST-DIVERGENCE_data/o2/meta.json` | `ac4cb0da3384339b4f1dd372f4d39dd9a39ccd0a2efe2cfdbfe4855f07cd6441` |
| `D:/MashedWT/RO-FIRST-DIVERGENCE_data/RO_ROIMG-fdimg/meta.json` | `bc2756f167b4c210eb1925e2a54ad66a3b3bc482cddd29b93454718709a2fc74` |
| `D:/MashedWT/RO-FIRST-DIVERGENCE_data/RO_ROIMG-fdimg/ticks.bin` | `401d1e0582c2f60e53da25ba04b0e31aa693f6ca527d7be2807bab9245cd7026` |
| root `original/mashed_image.bin` | `81cc77758ffe93ac0b681eb1fed3d63912d25e7d059503af6d4f871dea9afcd8` |
| `RO_ROIMG-og1i/meta.json` | `c1ab908bb7a4e18e0e62e157348ab9f07294609847ef258a3690e9f0b132957f` |
| `RO_ROIMG-og1i/ticks.bin` | `338feb35362f4179764444c08b1916409b339fbf527808a4d7bf4c0919db6508` |
| `RO_ROIMG-og1i.out` | `4f3c92976e2db74be613a9b7c05e86cd66e619f4fbfec3f5d8e09d1589dbe79f` |
| `fd_og1_og1i.json` | `e48bfb99e9801ff16a117a13bf5ab4c48e339a719a3e6557408760c400c23a98` |
| `rawdiff_o2_og1.txt` | `aaf96e3e1bc4b8d61c9acb6fa2e3e672ab64721d8dac7c4fd75d1ab5fbc9b9e6` |
| root `original/MASHED.exe` (reference binary) | `b9977dab70607720cf58bc7832937c203b1e0b33ff5896da586baf56aa79fe87` |

Post-reboot committed additions: `oraclegate/run46_fd_og1_og1i.md`, `oraclegate/disrange.py`, `oraclegate/rawdiff.py`,
`oraclegate/fun_004b65c0_unpatched.txt`, `oraclegate/fun_004b65c0_patched.txt`, `oraclegate/fun_004b65c0_callers.txt`.

- Gates: not run (top-down row, no G-CTRL, no B, no 12 captures). No tracker, hooks.csv, ROADMAP or `.asi` edit. Nothing pushed.

# Run 45 (2026-11-13, history): owners of the record-0 residue left after RO-FIRST-DIVERGENCE run 44

Row: replay oracle 10. Kind: top-down / harness, scoped by the lead to diagnosis this run. Worker cmwflg95v2y7slh1nj82cq1l4
(claude2, moved to claude3 for Ghidra MCP), lead cmwfldzyw2xjolh1npz5ac1rm. Branch `orch/RO-ORACLE-GATE` from
`orch/integration` `ba39eeb6b`.

**Verdict: DECISION.** Each residue word now has its owning code, cited from the decompilation. The audio words are a fix
for the ORIG_TOP audio-init lane, not a mask. The `0x008b1400` block is private state of the original piz file layer that
the ORIG_TOP exe replaces by design. Masking it is the proposed outcome, so it is filed as a decision and not applied.

**No build was needed.** No code, mask, tracker, ROADMAP or `.asi` change, no game launch, no gates (diagnosis only).
Self-test: not applicable, nothing was built or run. The RO-FIRST-DIVERGENCE compare was not re-run (the brief keeps it for
after RWC-FRAME-CLUMP merges).

## Inputs

- `verify/replayoracle_20261113/RESULT_RO-FIRST-DIVERGENCE.md` (run 44), image ON pair o2 vs `RO_ROIMG-fdimg`: record 0,
  260 counted blocks, 32,942 VALUE dwords.
- The full dword list of that pair, `D:/MashedWT/RO-FIRST-DIVERGENCE_data/fd_o2_RO_ROIMG-fdimg_full.json`, re-read here.
- Ghidra: pool slot `Mashed_pool0`, read only, MCP `decomp_function`, `listing_disassemble_range`, `reference_to`,
  `function_callers`, `memory_read`. Slot released after.
- The exe's arm log `D:/MashedWT/RO-FIRST-DIVERGENCE_data/RO_ROIMG-fdimg.log` (which ORIG_TOP pieces ran).

## Range census of the record-0 residue (new)

`oraclegate/record0_ranges.txt` groups the 260 blocks into contiguous ranges. "nonheap" counts the dwords that the block
hash still sees: `detorig_firstdiv.block_hashes` zeroes every value in `0x00995000..0x2fffffff` with the low 2 bits clear
(the heap rule), and the exe holds 0 there.

| range | blocks | VALUE dwords | nonheap | owner (below) |
|---|---|---|---|---|
| `0x008ad800..0x008cdc00` (7 ranges) | 123 | 27,276 | 26,348 | piz chunk cache `0x008ad9c0`, (b) |
| `0x008ed800`, `0x0090d800..0x0090e800`, `0x00911800..0x00913400` | 12 | 772 | 687 | piz file slots / directory, (b) |
| `0x0068f400..0x00690800` | 5 | 347 | 79 | audio voice pool and 0x004669b0 globals, (a) |
| `0x00604400..0x00606000` | 7 | 27 | 27 | permanent-wave table, (a) |
| every other range | 113 | 4,520 | n/a | not classified in this run |

So (b) alone holds 135 of 260 blocks and 28,048 of 32,942 VALUE dwords (85.1%). The cache range includes block `0x008cd800`, which also holds the head of slot 0 at `0x008cd9c0`.

## (a) `0x00604604`, owner FUN_0045d460, and `0x0068f800`

**Store instruction.** The watch hit pc is the trap eip, which is the instruction after the store:

- `0x00604604`: hit pc `0x0045d6a8` (a `JBE`), so the store is `0x0045d6a5 MOV dword ptr [EDI + -0x8],ESI`.
  - `EDI` starts at `0x00604578` (`0x0045d68c MOV EDI,0x604578`) and steps `0x94` per entry.
  - `0x00604604 = 0x00604578 + 1*0x94 - 8`, which is entry 1.
  - `ESI` is the running voice index. Decompilation: `piVar4[-2] = iVar2`, with `iVar2 += 1` per voice of the entry.
  - Value `0x00000004` (signed 4) is the count of entry 0: `0x00604574` holds `0x00000004` in the file image (Ghidra
    `memory_read`).
- `0x0068f800`: hit pc `0x0045d600`, so the store is `0x0045d5fa MOV dword ptr [ESI + 0x68f640],EAX`.
  - `EAX` is the return of `CALL 0x005a6280` at `0x0045d5f5`.
  - The loop at `0x0045d5f0..0x0045d60c` steps `ESI` by 8 up to `0x640`, so 200 slots `0x0068f640..0x0068fc7f`.
  - `0x0068f800` is slot `(0x0068f800 - 0x0068f640) / 8 = 56`.
  - This closes run 44's [UNCERTAIN] on that store.
- **Caller chain.**
  - `0x004669f6 CALL 0x0045d460` (return `0x004669fb`) is in FUN_004669b0, the sound-init hub (hooks.csv C4, match).
  - FUN_004669b0 is reached from boot `0x004028e0`, as the run-44 backtrace `0x0045d6a8 | 0x004669fb | 0x004028e0` shows.

**All 27 differing dwords of `0x00604400..0x00606000`** sit at `0x00604570 + k*0x94` (k = 1..40, offset 0 in each entry).
Their ref values run 4, 8, 12 .. 133, the cumulative counts. So all 27 are the `0x0045d6a5` store
(`oraclegate/audio_blocks.txt`).

**`0x0068f400..0x00690800`** (79 nonheap dwords, `oraclegate/audio_blocks.txt`):

- **The 200 slot handles** at `0x0068f640 + 8*i` are heap values. Example: `0x0068f800` = `0x0d779458` (signed 225,940,568)
  in o2, `0x0d8f9458` (signed 227,513,432) in o5. The heap rule zeroes them in the block hash, so they make no block differ
  now. They need no mask if the exe later writes its own heap handles there.
- **Five floats at the slots' +4 word:**
  - `0x0068f83c` = `0x3f79999a` (signed 1,064,933,786, float 0.975)
  - `0x0068f844` = `0x3f79999a` (the same value)
  - `0x0068fa64` = `0x3f79999a` (the same value)
  - `0x0068fa8c` = `0x3f79999a` (the same value)
  - `0x0068fa94` = `0x3f366d21` (signed 1,060,531,489, float 0.7126)

  The only static writer of `0x0068f644 + 8*i` is `0x0045e157 FSTP float ptr [ESI*0x8 + 0x68f644]` in FUN_0045e0f0. Ghidra
  lists 39 callers, all in `0x0045e460..0x004669a8`. [UNCERTAIN] which of them ran before tick 1. Missing: a watch on
  `0x0068f83c`.
- **`0x0068fc84`** = `0x0000008b` (signed 139): `numpermvirtualvoices`. Writers in FUN_0045d460: `0x0045d5c5`, `0x0045d5e2`,
  `0x0045d76f`.
- **`0x0068fc8c`, `0x0068fcbc`, `0x006900d4`, `0x006904e8` = 1, `0x006904fc` = 1, and the path strings at
  `0x00690158..0x00690404`** (`toastaudio\pc\audio\pcdics\english\<name>.rws`): these sit in the FUN_004669b0 global
  set. hooks.csv `004669b0`: "sets 11 globals". The exe's `Audio/AudioCharacterBankPaths.cpp:34` names FUN_004669b0 as the
  only caller of the path builder. [UNCERTAIN] the per-dword store pc for these. They were not watched, and this run read
  no decompilation of FUN_004669b0.

**What the exe does on this path.**

- The RO arm (`run_arms.py` `RO_` kind, stack `ROIMG`) does not set `MASHED_OT_AUDIOINIT`.
- So `0x004669b0` runs the hand piece 3. Arm log line 209: `[ORIG_TOP] 0x004669b0 sound init: piece 3 HandTop_InitAudio`.
- With `MASHED_OT_AUDIOINIT=1` the matched `0x004669b0` body runs. But `0x0045d460` is still a stand-in that returns
  without running: `Match/PortTop_004669b0.cpp:70` `PortTop_AInit_0045d460`, log text
  `rva 0x0045d460: stand-in, not run (permdict.rws; piece 3 loaded the bank)`.
- The body needs RW stream reads (`FUN_004cc230` with `"toastaudio\\pc\\audio\\pcdics\\PERMDICT.RWS"`, `FUN_004cc5e0` chunk
  `0x809`) and the RW audio layer (`FUN_005a7b60`, `FUN_005a7af0`, `FUN_005a7aa0`, `FUN_005b8570`, `FUN_005a71f0`,
  `FUN_005a6280`, `FUN_005a6dc0`, `FUN_005a6710`).

**Verdict (a): a boot-side fix, not a mask.** It belongs to the ORIG_TOP audio-init lane:

- `MASHED_OT_AUDIOINIT` (PORT-TOP-4669B0, merged).
- PORT-TOP-AUDIOTICK-STOPS (handoff).
- PORT-TOP-AUDIO-HWINIT-4627F0 (blocked: the RW-audio init/term table `0x00632ba8` reads 0 in the exe).
- PORT-TOP-AUDIO-RWA (blocked: `0x005a6ea0` / `0x005a6f30`).

No ledger row names a body for `0x0045d460` (hooks.csv C2, `new`). The work is a body for `0x0045d460` on the ORIG_TOP path,
plus the AUDIOINIT knob on the RO arm, once those two blocked rows clear. The lead files that row. The `0x00632c00..0x00633c00`
range (4 blocks, 286 dwords) is adjacent to that table. [UNCERTAIN] whether it is the same table, because this run did not
classify it.

## (b) `0x008b1400`, owner FUN_004b6b20

**Store instruction.**

- The hit pc `0x004b6b88` is `MOVSD.REP ES:EDI,ESI` itself, with `EDI = 0x008ad9c0` (`0x004b6b83`) and `ECX = 0x8000`
  (`0x004b6b7e`).
- A REP string store traps inside the instruction, so the eip stays at `0x004b6b88`.
- It copies 0x8000 dwords (0x20000 B) from the file slot `+0x64` (`in_EAX + 100`, filled by `FUN_004b67e0`) into
  `0x008ad9c0..0x008cd9bf`.
- `0x008b1400` is offset `0x3a40` in that cache.
- Ref values there are file bytes: `0x00000001`, `0x0000001c`, `0x1c02000a` (signed 469,893,130), `0x80ffffff` (signed
  -2,130,706,433), `0x3e99999a`, ...

**Every static reference into `0x008ad9c0..0x008cd9c0`:**

| from | function | kind |
|---|---|---|
| `0x004b69a3` / `0x004b69b4` | FUN_004b6940 (PizOpenAndParse) | write |
| `0x004b6b83` / `0x004b6b88` | FUN_004b6b20 | write |
| `0x004b6db2` | FUN_004b6cd0 | read: copies the bytes into the caller's buffer |
| `0x004b663a` | FUN_004b6630 | end bound only: `return &DAT_008cd9c0 + param_2*0x20078` |

`0x0061eeec` / `0x0061eef0` are two more Ghidra references, typed as pointers to `0x008c0077` / `0x008c007c`. They are
16-bit values in a table (bytes at `0x0061eee0`: `81 00 2c 00 79 00 7b 00 db 01 7c 00 77 00 8c 00 7c 00 8c 00 ...`), and no
code reference reaches `0x0061eee0..0x0061ef00`. No other code reads the cache.

**The layer:**

- FUN_004b6cd0 and FUN_004b6630 have no direct callers.
- FUN_004b6640 stores both into a 0x58-byte callback table: `+0x14 = FUN_004b6630`, `+0x30 = FUN_004b6cd0`, plus
  `FUN_004b6c00`, `FUN_004b6f60`, `FUN_004b6f90`, `FUN_004b6ed0`, `FUN_004b6ba0`, `FUN_0045b350`. It registers the table
  with `FUN_00551330(puVar1, 2, ...)` / `FUN_00550430`.
- FUN_004b6640 is reached only by `0x004b6540 JMP` (the file-system setup).
- The two file slots are `0x008cd9c0` and `0x008eda38`, each `0x20078` B, ending at `0x0090dab0`.
- The slot `+0x60` words written there are `0x008cda20` and `0x008eda98`.

**Correction to run 44:**

- The backtrace frame `0x004b6e3e` is inside FUN_004b6cd0 (`0x004b6cd0..0x004b6ec1`), not FUN_004b6b90. FUN_004b6b90 is
  the 6-byte floor-align leaf it calls.
- FUN_004b6b20's only caller is FUN_004b6cd0.

**What the exe does.**

- Arm log line 156: `[ORIG_TOP] [PIZ_FS] 0x004b6540 file system setup, mode flag 0 (exe piz reader, Piz/PizReader.h)`.
  Every mount goes through `0x00495280` to the exe reader.
- That is the binding PORT-TOP-04E made ("bind the piz file system"), so the exe never runs `0x004b6640` / `0x004b6cd0` /
  `0x004b6b20`, and the cache and slots stay 0.
- The bytes the cache would hold reach the consumers through the exe reader instead.

**Verdict (b): a mask is the proposed outcome, so a decision row is filed** (below) and no mask is applied.

- **Cache:** the 123 blocks of `0x008ad9c0..0x008cd9bf` and the FUN_004b6cd0 counters `0x008ad868`, `0x008ad878`,
  `0x008ad87c`, `0x008ad880`, `0x008ad884`, `0x008ad888` are written and read only inside the replaced layer.
- **Slots, directory and globals:** this run does not show them private. The slots are handed to the RW file-system
  manager (`FUN_00551330`, `FUN_00550430`) as file objects. [UNCERTAIN] whether RW code at `0x0055xxxx` reads slot fields.
  Missing: the reference list for slot fields `+0x20064..+0x20070` from outside `0x004b6xxx`. The directory `0x0090dac0`
  (FUN_004b6940) and `0x007d3e4c` / `0x007d3e50` / `0x007d3e54` / `0x007d3e64` / `0x007d3e68` are in the same position.

## Other items

- **`0x004325c0` never runs in the exe's ORIG_TOP replay menu.** The exe runs only the `0x0043dfd0` stand-in
  (`Match/PortTop_StandIns05.cpp:24`). The `0x0043dfd0` body is the PORT-TOP-08 lane. Recorded here, no work done.
- `0x0068f800` and the other 199 slot handles are heap values, which the heap rule already handles. Neither (a) nor (b)
  asks for a mask on them.

## Decision filed

`roadmap.ps1 decision RO-ORACLE-GATE` asks this question: on the ORIG_TOP replay oracle, mask the original piz layer's
private chunk cache `0x008ad9c0..0x008cd9bf` and the FUN_004b6cd0 counters, or port the original piz layer onto
ORIG_TOP. The recommendation is to mask the cache and counters now as a bound family. The slots, directory and
`0x007d3eXX` globals come later, after their outside readers are checked. The full text is in the ledger row.

## Evidence

- Committed:
  - this RESULT
  - `oraclegate/record0_ranges.txt` (range census)
  - `oraclegate/audio_blocks.txt` (the perm-table and voice-pool dwords)
  - `raw_sha256_RO-ORACLE-GATE.txt`
- Raw, not committed (SHA-256 in `raw_sha256_RO-ORACLE-GATE.txt`):

| file | SHA-256 |
|---|---|
| `D:/MashedWT/RO-FIRST-DIVERGENCE_data/fd_o2_RO_ROIMG-fdimg_full.json` | `2734b1f22f3e1b370cd1848e762332488864820ca2b3ee19cd5e726e8f61c752` |
| `D:/MashedWT/RO-FIRST-DIVERGENCE_data/RO_ROIMG-fdimg.log` | `00b9a78cd302f2da466d33b8b2d7eae18328e783efe05da31cb282f07bdc3193` |
| `D:/MashedWT/RO-ORACLE-GATE_data/record0_ranges.txt` | `eba6132989692ddebad158ffa431a694bb0e45053bb26d52953bdb25808a9030` |
| `D:/MashedWT/RO-ORACLE-GATE_data/audio_blocks.txt` | `db4d496d7c3e4630f8af474501a03842d8c69ef87340acd23fbb799312e71573` |

- Gates: not run (diagnosis row, no build). No 12 captures. Nothing pushed.
