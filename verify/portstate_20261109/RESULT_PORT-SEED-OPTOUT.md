# RESULT PORT-SEED-OPTOUT (2026-11-09)

PREREG: `PREREG_SEED_OPTOUT.md` (committed alone, 338e93aab). Branch `orch/PORT-SEED-OPTOUT` from `orch/integration`
25d6fccc2.

## 1. Answer

**GREEN.** The opt-out is now safe. With `MASHED_NO_PORTSTATE_SEED=1` and nothing else set, the exe exits 0 in all 5 opt-out
boots (probes P1, P2 and P4, and the arms H_NOPSSO and B_NOPSSO_K0). With videocfg.bin absent it runs the first-run write,
`ret=1`, and the file is `7f130171`, byte-identical to the default seed path. The default gates are unchanged:
G-CTRL `8284be46/67931834/31fe043e`, B `1d5bf4f9` x3. The matched sources (`re/match/s16`) are not touched.

## 2. Root cause (arm 1, build `0ce1aee9` = `orch/integration` 25d6fccc2 + the PREREG)

Probe `seed_optout/optout_probe.py`: Frida on the exe's `ConfigLoad`, UCRT `_fsopen` and the invalid-parameter entry
points (addresses from `mashed_re.map`). Env: `MASHED_NO_PORTSTATE_SEED=1` only. Observed, in order:

1. `ConfigLoad`, called from `VideoCfgFirstRunWrite+0x2a8` (`Boot/VideoCfgFirstRun.cpp:154`), from `InitD3D9`.
2. `ConfigLogDebug`, from `ConfigLoad+0x14`, with the format string 0x005d013c (zeros, an empty format, harmless).
3. `_fsopen("videocfg.bin", mode)`, from `HostFopen+0x12` (`Save/SettingsConfig.cpp`, the exe's
   `_fsopen(p, m, _SH_DENYNO)`). Mode pointer **0x005cf010**, bytes `00 00 00 00`: the empty string, because only the
   seed fills that address and the opt-out skips it.
4. `_invalid_parameter_noinfo` from `common_fsopen+0x23`, then `_invalid_parameter` and
   `_invalid_parameter_internal`. Backtrace: `HostFopen+0x12` <- `SettingsCfgFopen+0x10` <- `ConfigLoad+0x20` <-
   `InitD3D9+0x4a`.
5. Exit `0xC0000409`.

So **H1 is confirmed**. The read is the fopen mode at 0x005cf010 in the matched 0x00498950 body. The buffer is the
original .rdata address that the seed fills (zero-filled in the exe's image pad without it). The check that fails
is UCRT's parameter validation of the `_fsopen` mode, and its default invalid-parameter handler fast-fails with
0xC0000409. No stack cookie or buffer overrun is involved. The same holds for 0x004989b0 (mode 0x005d00d8): it was
never reached, because the load ended the process first.

## 3. Change (commits 193a6dd1b, 08280c61d)

- `Save/SettingsConfig.cpp` (exe only, `#ifdef MASHED_STANDALONE`): five private strings with the original's bytes,
  read from MASHED.exe.unpatched (0x005d013c `Loading video cfg from %s\n`, 0x005d0158 `Saving video cfg to %s\n`,
  0x005cfe70 `\tFAILED\n`, 0x005cf010 `rb`, 0x005d00d8 `wb`). `SettingsCfgGlobal` returns them only when
  `PortStateSeed_On()` is false. With the seed ON it returns the original addresses as before, so the default build
  takes the same path. The matched bodies bind through their MGLOBALA dynamic initializers, which run before
  WinMain's `PortStateSeed_Apply`, so the binding follows the knob, not the apply result. A refused seed (FNV
  mismatch) is a build fault, and the seed `--check` lint stops it.
- `Compat/PortStateSeed.h`: the knob comment says the opt-out boots on its own since this step.
- `verify/savelink_20261023/run_arms.py`: the `NOPSS` comment updated (the stack keeps `MASHED_VIDEOCFG_WRITE=0`, so its
  arms stay comparable), new stack `NOPSSO` = the opt-out alone.

The first build of 193a6dd1b failed to compile: a shell heredoc had turned the `\n` / `\t` escapes into real
characters. 08280c61d fixes them. Every arm below ran on build `2e53b808` (08280c61d). rva-lint, asm-lint and the
seed check passed in that build.

## 4. Acceptance

### 4a. Default gates (seed ON), build `2e53b808`

| arm | hashes | ref |
|---|---|---|
| H_DEF | `8284be46 / 67931834 / 31fe043e` | same |
| B_DEF_K0 / K1 / K26 | `1d5bf4f9` x3 | same |

### 4b. Opt-out, nothing else set

| run | videocfg.bin | fopen mode read from | ConfigSave | exit |
|---|---|---|---|---|
| probe before the fix (build `0ce1aee9`) | absent | 0x005cf010 = `""` | not reached | **0xC0000409** |
| H_NOPSSO (G-CTRL recipe) | present | private copy | no | 0, `8284be46/67931834/31fe043e` |
| B_NOPSSO_K0 | present | private copy | no | 0, `1d5bf4f9` |
| P1 / P2 (B recipe, probe) | present | 0x002dd17c = `rb` (private) | no | 0 / 0 |
| **P4** (B recipe, probe) | **absent** | 0x002dd17c = `rb`, then 0x002dd180 = `wb` | yes, `first-run write ret=1` | **0** |
| P3 / P5 control, seed ON | present / absent | 0x005cf010 = `rb`, then 0x005d00d8 = `wb` | P5 yes, `ret=1` | 0 / 0 |

The first-run file is `7f1301713a118a86...` from P4 (opt-out) and from P5 (default), which equals PORT-STATE-02 6a's
`7f130171`. The opt-out gates equal the default gates, because the seeded data that differs is not read on these
recipes once the videocfg path holds.

The exe resolves `videocfg.bin` in `MASHED_ROOT`, not in the spawn cwd. P1 to P3 therefore found the file that this
step's own H_DEF arm left in the private root (sha8 `cdba8772`, provenance not traced). P4 and P5 moved it aside first.

## 5. Not done / notes

- The other seed consumers under the opt-out (0x004769f0's 0x00613290 and the rest of the inventory) read zeros and do
  not crash on these recipes. That is not a claim of value parity under the opt-out. The opt-out remains a diagnostic
  path, not a supported game mode.
- No tracker change. The matched rows keep their C4 (match route) and port values.

## 6. Commits

| commit | content |
|---|---|
| 338e93aab | PREREG (alone) |
| 193a6dd1b | the change, knob comment, NOPSS comment, `seed_optout/optout_probe.py` |
| 08280c61d | escape fix, `NOPSSO` stack |
| this commit | `seed_optout/run_probes.sh`, `seed_optout/run_probes_absent.sh`, this RESULT |

## 7. Raw files (`D:/MashedWT/PORT-SEED-OPTOUT_data`, not committed)

| file | sha256 |
|---|---|
| exe_before/mashed_re.exe | 0ce1aee9a1f6e590c3e1ba21e11bd686d5f2930461312604e60da7c79b2d8f39 |
| exe_after/mashed_re.exe | 2e53b808cc6477b6fb3425004ca801a54c919430e889af59867ae4a2448e0c2e |
| probe_before.json | a44e7727db42050722666080f12336fafff00635b79cc5b3d78955c25db501cd |
| probe_before.log | 5784da59c6104d26fad8a8ba3ec1a244f326b3dd773472835053290bbce1b844 |
| probe_p1.json | 93e8d83fce20ba37fcda324a9d314e2213c51bc6ddafe6114d669d5abcaa8fcc |
| probe_p2.json | 037f7dd2dbed1793d2a74ca4137926930a7ae2dfaff213fd9d2a55006e78d29c |
| probe_p3.json | 6422c8245461326722568465a7fc3bde8d8034b4a864e0887b0ec56f055bc3df |
| probe_p4.json | 44f5150f67aacfc5fde838979017c66850ac1a19167cdc8591d53a31ebde4596 |
| probe_p5.json | a15dfcf3ae1f0603c818e6bfdee0fc7543f56190bf07fb06a27ad838d749790b |
| probes.log | 4fbbe816f2c097f2a8fffce1486b57a4addf093184030b397890bbb878bda1b4 |
| probes_absent.log | 748edbe7ea71f5b93fa4e7ad4dfedfa3a75d24ad65bce7af59ade5c8b9fa4db8 |
| arms.log | a75326047bc8e93e5141a29532ebc207e660def34f10d44e5d27510111ebffb8 |
| arms/H_DEF.step.csv | 8284be46b3c692f3ba129f252367b5f5097eef9e39bc49a1be6514e4d33b6198 |
| arms/H_DEF.gates.csv | 6793183492e0c0822570ad71254f9fdf27568b77c81a25c56347f1616863d58c |
| arms/H_DEF.ptrace.log | 31fe043e261d2df943fa3dd5688b74da4e8329eb00ea87f090539bb7e854c5e3 |
| arms/B_DEF_K0.step.csv | 1d5bf4f980bf7eb48ac98211f4fb50ff58a04d1ad408d18695d79ddd8fcf8457 |
| arms/H_NOPSSO.step.csv | 8284be46b3c692f3ba129f252367b5f5097eef9e39bc49a1be6514e4d33b6198 |
| arms/B_NOPSSO_K0.step.csv | 1d5bf4f980bf7eb48ac98211f4fb50ff58a04d1ad408d18695d79ddd8fcf8457 |
| vcfg/p4.bin, vcfg/p5.bin | 7f1301713a118a86372d06b89d503334ddeb6619b7f23088e09ed453d12bab88 |
| vcfg/root_before.bin | cdba8772972fbf72b7c2d4aab142b6c09f02863fc93ccf853447a230f6fee3a7 |
| build_after.log | 120864f9df28e57ab0f77f9282a52bab76257cc4cd96275acd4508305743ef8d |
