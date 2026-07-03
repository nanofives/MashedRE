# D-11057 config-edit (s18/s24) — Fable-scoping survey (claude2, 2026-07-03)

Scopes the Ghidra pull needed to activate the standalone config-edit primitive
(`Nav_ConfigEditWrap`, landed a8924d1b) as LIVE LEFT/RIGHT editing on the game-setup
screens 18/24. Mirrors the it5 rule-5 survey: enumerate exactly what Fable must pull so its
one MCP session is a tight, closed set. Read/draft only; no MCP; no guessing. Sources: the
d11057 confirm note, `frontend_config_screens_REmap_20260614.md`, and the `FUN_0043af10`
plate (`frontend_c1_to_c2_followup_main/FUN_0043af10.md`, C2).

## Two views of the same row→setting relationship

**EDIT side (d11057 confirm note, dec handler @0x00440283+):** the LEFT/RIGHT handler reads a
per-item selector `EDI = DAT_0067ed40[item]` and edits a fixed global with a wrap range:
| selector | global | wrap | note |
|---|---|---|---|
| 1 | `DAT_0067ea74` | 0..2 | game-type |
| 2 | `DAT_0067ea90` | 1..4 | (unnamed) |
| 3 | `DAT_0067ea94` | **[range not dumped]** | vehicle |
Cursor/key-repeat re-arms via `DAT_0067ea88`. **Only selectors 1/2/3 were dumped**; the
increment/right half of the handler (within `0x00440283..0x00440820`) was **not** dumped.

**DRAW side (config REmap + `FUN_0043af10` 0x0043af10, C2):** the 7-row option renderer maps
each visible row → its setting global (`switch(fStack_3c)`; `iStack_38` row-type picks the
branch). Documented row→setting (config REmap PROGRESS): s24 = PlayGame · PowerUps(`ea80`) ·
Difficulty(`ea7c`) · Vehicle(`ea94`) · Opp1/2/3(`ea98/9c/a0`) · GameMode; s18 = PlayGame ·
PowerUps(`ea80`) · GameLength(`ea88`/msg 0x24c) · ExtraOpp · Vehicle(`ea94`) · AirStrike ·
GameMode. Probed fresh defaults: `ea74=1 ea7c=0 ea80=0 ea88=0 ea90=1 ea94=0 ea98=4 ea9c=2 eaa0=3`.

**The missing link:** the DRAW side tells us which `ea*` each row shows; the EDIT side needs
the **`DAT_0067ed40[row]` selector integer** per row (and selectors exist beyond 1/2/3 for the
difficulty/powerups/length/opponent rows, which weren't dumped).

## Minimal Ghidra set for Fable (2 pulls, closes the lane)

1. **`reference_to 0x0067ed40`** → find the writer(s) (the s18/s24 enter/setup handler that
   populates `DAT_0067ed40[]`) and **dump the per-row selector values for screen 18 and 24**.
   This is the authoritative row→selector map — the ONE thing that unblocks the wiring.
2. **decode `0x00440283..0x00440820` in full** (both the dumped **dec** and the un-dumped
   **inc** arms) → the complete `selector → {target global, [lo..hi] wrap}` table, incl.:
   - `ea94`'s wrap range (RESIDUAL from the confirm note),
   - the increment-with-wrap direction (confirm it mirrors dec: hi→lo — the standalone
     primitive already assumes this),
   - any selectors **beyond 1/2/3** (difficulty `ea7c`, powerups `ea80`, length `ea88`,
     opponents `ea98/9c/a0`) so ALL rows become editable, not just ea74/ea90/ea94.
3. *(optional cross-check)* `FUN_0043af10` (plate exists) row→setting switch — confirm the
   `ed40[]` map from #1 agrees with the draw side.

## Standalone wiring once Fable returns the data

1. Extend `Nav_ConfigEditWrap` (MenuNavSM.cpp) with the additional selector arms + the `ea94`
   range from pull #2 (the primitive's shape is already there; add cases + fill ranges).
2. Add a `kEd40RowSelector[sid][row]` table (from pull #1) in `exe_main`.
3. Wire LEFT/RIGHT on `sid==18||sid==24` in `exe_main` (mirror the existing sid 6/7 [D-11054]
   tier-ring block) → `Nav_ConfigEditWrap(kEd40RowSelector[sid][cursorRow], dir)`, gated
   behind `MASHED_CONFIG_EDIT` (default ON once verified, or OFF pending a parity check).
4. Feed the edited values where they matter (e.g. `ea94` vehicle → the launch car / preview;
   `ea88` length → `RaceRuleFromGameLength`, already wired for MP modes).

This closes D-11057's config-edit tail; combined with hand-off #1/#2 (continue-cup) it
finishes D-11057 breadth.
