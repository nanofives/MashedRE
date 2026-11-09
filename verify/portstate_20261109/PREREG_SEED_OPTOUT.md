# PREREG PORT-SEED-OPTOUT (2026-11-09)

Step: PORT-SEED-OPTOUT, parent cmw7xu1960f37p41557tefpnd. Branch `orch/PORT-SEED-OPTOUT` from `orch/integration`
25d6fccc2. Committed alone, before any arm.

## 1. Problem

PORT-STATE-02 RESULT section 6a (`verify/portstate_20261108/RESULT_PORT-STATE-02.md`): with `MASHED_NO_PORTSTATE_SEED=1`
alone the exe exits `0xC0000409` at the first-run videocfg load. With `MASHED_VIDEOCFG_WRITE=0` as well it reaches the
menu and exits 0. Since `dbedbfcbe`, the exe's matched bodies 0x00498950 / 0x004989b0 / 0x00496400 / 0x004963e0
(`Match/PortSave01_Config.cpp`, `re/match/s16`) read five .rdata strings at their original addresses, and only the
seed fills them (`Save/SettingsConfig.cpp` `SettingsCfgGlobal`). The original's bytes, read from MASHED.exe.unpatched:

| addr | bytes | read by |
|---|---|---|
| 0x005d013c | `Loading video cfg from %s\n` | 0x00498950, log format |
| 0x005d0158 | `Saving video cfg to %s\n` | 0x004989b0, log format |
| 0x005cfe70 | `\tFAILED\n` | 0x00498950, error text |
| 0x005cf010 | `rb` | 0x00498950, fopen mode |
| 0x005d00d8 | `wb` | 0x004989b0, fopen mode |

Hypothesis H1 (to be confirmed or refuted by arm 1, not assumed): without the seed, 0x005cf010 reads `""`, the exe's
`HostFopen` passes it to UCRT `_fsopen`, the mode check fails, and UCRT's default invalid-parameter handler ends the
process with `__fastfail(FAST_FAIL_INVALID_ARG)`, reported as `0xC0000409`. If so, this is a CRT parameter abort, and
no stack cookie or buffer overrun is involved.

## 2. Arms

1. **Root cause** (build of this branch before the change). Opt-out boot under Frida with hooks on the exe's
   `_fsopen`, UCRT's invalid-parameter entry points as named in `mashed_re.map`, and `ConfigLoad`. Record the mode
   string `_fsopen` receives, whether the invalid-parameter path is entered and from which return address, and the
   exit code. RESULT states H1 confirmed or refuted, with the observed call chain.
2. **Change.** In the exe only (`#ifdef MASHED_STANDALONE`), `SettingsCfgGlobal` returns private copies of the five
   strings above when `PortStateSeed_On()` is false. With the seed ON (default) it keeps returning the original
   addresses (seed-filled), so the default build is unchanged. The matched sources are not edited. The opt-out then
   runs the matched bodies on the original's bytes (copies), not on zeros.
3. **Acceptance.**
   - Default gates (seed ON): G-CTRL `8284be46/67931834/31fe043e`, B `1d5bf4f9` x3 (`merge_train.json` refs win).
   - Opt-out boot with nothing else set (`MASHED_NO_PORTSTATE_SEED=1`, the run_arms boot env otherwise default): exit 0,
     and `log/mashed_re.log` holds a `[videocfg]` line (present-no-write or first-run write) rather than a crash. Both
     cases are run: videocfg.bin present in the private root, and absent.
   - Default boot with videocfg.bin absent: the first-run line and file bytes as in PORT-STATE-02 6a (file `7f130171`
     on that machine), as a check that the seed path is untouched.

## 3. Rules

Every launch through `mashed_lock.py run` inside test-slot. No build during a train PRIORITY-HOLD or SOLO. Raw data in
`D:/MashedWT/PORT-SEED-OPTOUT_data` with SHA-256 in the RESULT. If arm 1 refutes H1, the RESULT says so, and the change
is reconsidered before the gates.
