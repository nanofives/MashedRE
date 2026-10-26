# RESULT D4-WSF-SCOPE (L-22 (a)): GREEN

Text-only. Nothing built or run. Edit: ROADMAP.md S-DoD section, new paragraph "Criterion 5 scope (user decision L-22 (a))" after the S-DoD paragraph (~line 150).

Rule written: criterion 5 round-trip covers only formats the game writes. Parse-only formats are exempt and meet criterion 5 by parse coverage.

## In criterion 5 (game writes them)
gamesave.bin, videocfg.bin, contcfg%d.bin, AI path data (original writer FUN_00423540, re/analysis/formats/ai_path_data.md:27).

## Exempt (parse-only), evidence
- Vehicle DFF: re/analysis/formats/vehicle_dff.md:46 (parse-only, dff_dump.py). Loader RVA not recorded in the note.
- Track world BSP: re/analysis/formats/track_world_bsp.md:121 ("Round-trip writer: not yet needed"). Loader RVA not recorded.
- Track SPL/ANM/UVA/MTS: re/analysis/formats/track_anim_data.md (parser rw_track_data.py, no writer).
- PIZ: opened read-only, FUN_004b6710 dwShareMode 0 at 0x004b6738 (CLAUDE.md). pack_archive is a dev tool.
- TXD: re/analysis/txd_on_disk_format/README.md (census only).
- RWS: rws_inspect.py; rws_voice_replace.py is a dev tool.

## WS-F rows that would build writers for read-only formats
(Rows from verify/d4scope_20261026/SCOPING_D4-WSF.md:25-29.)
- D4-WSF-PIZ-RT: pack(extract(x)) writer for PIZ. Read-only format. Reshape to parse coverage over every piz.
- D4-WSF-TXD-RT: TXD re-emit writer. Read-only. Reshape to parse coverage.
- D4-WSF-RWS-RT: RWS re-emit writer. Read-only. Reshape to parse coverage.
- D4-WSF-TRACKDATA-RT: emit for track anim/SPL/ANM/UVA/MTS/COURSE/LAPDATA. Read-only except the AI path data half, which stays (original writer exists).
- D4-WSF-SAVE-RT: NOT affected. Saves and videocfg/contcfg are game-written.
