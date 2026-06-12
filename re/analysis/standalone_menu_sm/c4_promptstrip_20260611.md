# C4 evidence — 0x00432b30 MenuPromptStrip (PromptStripTwin)

Date: 2026-06-11. Lane: subset-install canonical observe
(`re/frida/canonical_install_observe.py "PromptStripTwin,0x00432b30"`).

## C3 base (bit-identity)
- `re/frida/menu_prompt_diff.py` synthetic on-game-thread A/B sweep:
  **GREEN 264/264** vectors (6 samples × 44 vectors), fired=168 rows via the
  FUN_0042ad10 witness. Coverage: mode 0 × keys 1..10 × both b920-gate sides;
  mode 1 (pop-reveal) × keys 1..10 × cmp∈{0, b920}; mode 2 organic reload
  shapes (key 5 / key −1); bounds keys −1/11. Sampled at 3 nav depths
  (post-pop states after pushing 1→8→19). Evidence:
  `log/diff_menu_prompt_navdriven.json`.
- Organic-domain probe: `log/probe_prompt_calls.json` — single call site
  ret=0x0043d79c (FUN_0043d2a0); pushes = mode 0/key kind/cmp pushed-screen;
  reloads = mode 2/key −1; pops = mode 1/key revealed-kind/cmp 0.
- Synthetic-domain caveat (documented, not a gap): mode 1 requires
  stack[depth+1] to hold a previously-popped entry (the original AVs
  otherwise — reproduced); the sweep therefore samples on the pop-descent,
  matching the only organic mode-1 context.

## C4 canonical runs (inline JMP live)
Three spawns, ONLY=[PromptStripTwin] (manifest 535 hooks seen, 1 installed):

| run | window | result | JMP byte @0x00432b30 |
|-----|--------|--------|----------------------|
| 1 | 50 s | survived, alive=True | 0xE9 JMP-LIVE |
| 2 | 30 s | survived, alive=True | 0xE9 JMP-LIVE |
| 3 | 40 s | survived, alive=True | 0xE9 JMP-LIVE |

Organic traffic through the live JMP: the boot nav push (mode 0, key 1,
cmp 22 — probe row 1) on every run. Title screen rendered correctly with the
twin building the prompt records: `verify/c4_promptstrip_installed_menu.png`.
CORRECTION 2026-06-12: the PostMessage Enter presses sent during runs 1/3
were originally credited as extra nav fires, but PostMessage keystrokes do
not reach the game's DirectInput polling (established during the parity2
capture work) — they are NOT evidence. The C4 case rests on the boot fire +
JMP-live byte + survival + the C3 bit-identity sweep.

## Implementation
`mashedmod/src/mashed_re/Frontend/PromptStripTwin.cpp` — cdecl body + naked
true-ABI thunk (EAX=mode, ESI=rec-index ptr, stack=(key, cmp), plain ret),
`RH_ScopedInstall` at 0x00432b30. Jump tables re-derived from binary dwords
(0x433060 outer; 0x433088/b0/d8/100/128/150/178 inner) + linear cell-body
walk; the diff caught 4 mistranscribed rows (key5/6/8 frozen-cell position,
key10 cell[1]) before promotion.
