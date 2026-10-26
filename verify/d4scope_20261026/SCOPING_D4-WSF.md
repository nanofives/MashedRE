# SCOPING D4-WSF: data formats (S-DoD criterion 5, "formats round-trip")

Source: ROADMAP.md:3004 (WS-F row, "No work since 2026-06-16") and S-DoD text at ROADMAP.md:146-149.
Text-only read of `re/analysis/formats/`, `re/analysis/piz_validation/`, `re/tools/`, `DEFERRED.md`. Nothing was run.

## Format matrix (what exists on disk at c2bf772f8)

| Format | Parser | Writer | Round-trip test | Open items (cite) |
|---|---|---|---|---|
| `.piz` archive | `re/tools/piz_extract.py` (`read_archive`) | `pack_archive` (piz_extract.py:234) | none recorded | U-0129 entry+0x7C field meaning (`piz_validation/findings.md:17,69`); header byte 0x0C fill differs from SciLor (`byte_check.md:138`) |
| `.txd` texture dict | `re/tools/txd_format_census.py`, `re/analysis/txd_on_disk_format/README.md` | none | none | census only |
| `.rws` audio | `rws_inspect.py`, `rws_extract_wav.py`, `rws_voice_index.py` | `rws_voice_replace.py` (partial, voice swap) | none | identity re-emit untested |
| `.dff` vehicle clump | `dff_dump.py` | none | none | skin/anim plugin EXTENSION payloads unexamined (`formats/vehicle_dff.md:46`) |
| track world BSP | `track_dump.py` | none | none | sector flag bits unconfirmed (`track_world_bsp.md:120`); "writer not yet needed" (`:121`) |
| track anim data | `rw_track_data.py` | none | none | 5 `[UNCERTAIN]` fields (`track_anim_data.md:45,108,128,130,133`) |
| AI path data | `ai_data.py`, `ai_headattrib.py` | original writer `FUN_00423540` documented (`ai_path_data.md:27`) | none | writer not ported as a round-trip |
| `gamesave.bin` | `gamesave_parse.py`, `savefmt_cli` | `gamesave_edit.py` | `re/tools/save_roundtrip.py` (savelink 2026-10-23) covers the 13x12 table span only | ghost block `re/analysis/save_ghost_block_20261023.md` |
| `videocfg.bin` / `contcfg%d.bin` | standalone reads | standalone must write defaults (P-DoD first-run line, ROADMAP.md:2984) | none | tied to D5-FIRSTRUN |
| Lua 4.0 data (`COURSE.LUA`, `LAPDATA.LUA`, `powerups*.lua`, `KTCScript.lua`) | in port | n/a (read-only text) | none | `Lap_Line_End` resolved D-11058 |

## Gap in one line
Zero of ten formats has a committed round-trip test, and 6 of 10 have no writer at all. Whether criterion 5 needs a writer for formats the game only reads is a scope choice, so it is filed as a decision (`D4-WSF-SCOPE`) and does not block the rows that need no writer.

## Steps emitted
- `D4-WSF-PIZ-RT` (wave 3): `pack(extract(x))` byte identity over every piz in `original\`. Text/py only.
- `D4-WSF-TXD-RT` (wave 3): TXD parse -> re-emit identity.
- `D4-WSF-RWS-RT` (wave 3): RWS parse -> re-emit identity, reusing `rws_voice_replace.py` write path.
- `D4-WSF-SAVE-RT` (wave 3): full-file `gamesave.bin` identity (beyond the 13x12 span) incl. ghost block, plus `videocfg.bin`/`contcfg%d.bin` default-write bytes.
- `D4-WSF-TRACKDATA-RT` (wave 3): AI path data + track anim data + LAPDATA/COURSE parse -> emit identity, and a cited list of the 5 UNCERTAIN anim fields.
- `D4-WSF-SCOPE` (decision): are DFF and world BSP in criterion 5 (read-only formats, no use-case for a writer)?
