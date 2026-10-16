# D1-residue R3: baseline vs post-deletion race captures

## BMPs

56/65 byte-identical (SHA-256).

- 00_Arctic  00_challengeselect.bmp:SAME  01_action.bmp:SAME  01_grid.bmp:SAME  01_inrace_track.bmp:SAME  02_back_to_menu.bmp:SAME
- 01_Egypt  00_challengeselect.bmp:SAME  01_action.bmp:SAME  01_grid.bmp:SAME  01_inrace_track.bmp:SAME  02_back_to_menu.bmp:SAME
- 02_City  00_challengeselect.bmp:SAME  01_action.bmp:SAME  01_grid.bmp:SAME  01_inrace_track.bmp:SAME  02_back_to_menu.bmp:SAME
- 03_Forest  00_challengeselect.bmp:DIFF(45700, 0, 307200)  01_action.bmp:SAME  01_grid.bmp:SAME  01_inrace_track.bmp:SAME  02_back_to_menu.bmp:DIFF(44511, 0, 307200)
- 04_Highway  00_challengeselect.bmp:DIFF(45700, 0, 307200)  01_action.bmp:DIFF(293671, 135681, 307200)  01_grid.bmp:SAME  01_inrace_track.bmp:SAME  02_back_to_menu.bmp:DIFF(44511, 0, 307200)
- 05_Neustein  00_challengeselect.bmp:SAME  01_action.bmp:SAME  01_grid.bmp:SAME  01_inrace_track.bmp:SAME  02_back_to_menu.bmp:SAME
- 06_Storm  00_challengeselect.bmp:SAME  01_action.bmp:SAME  01_grid.bmp:SAME  01_inrace_track.bmp:SAME  02_back_to_menu.bmp:SAME
- 07_SuperG  00_challengeselect.bmp:DIFF(45700, 0, 307200)  01_action.bmp:SAME  01_grid.bmp:SAME  01_inrace_track.bmp:SAME  02_back_to_menu.bmp:DIFF(44511, 0, 307200)
- 08_Warzone  00_challengeselect.bmp:DIFF(45700, 0, 307200)  01_action.bmp:SAME  01_grid.bmp:SAME  01_inrace_track.bmp:SAME  02_back_to_menu.bmp:DIFF(44511, 0, 307200)
- 09_rouabout  00_challengeselect.bmp:SAME  01_action.bmp:SAME  01_grid.bmp:SAME  01_inrace_track.bmp:SAME  02_back_to_menu.bmp:SAME
- 10_sands  00_challengeselect.bmp:SAME  01_action.bmp:SAME  01_grid.bmp:SAME  01_inrace_track.bmp:SAME  02_back_to_menu.bmp:SAME
- 11_dump  00_challengeselect.bmp:SAME  01_action.bmp:SAME  01_grid.bmp:SAME  01_inrace_track.bmp:SAME  02_back_to_menu.bmp:SAME
- 12_training  00_challengeselect.bmp:SAME  01_action.bmp:SAME  01_grid.bmp:SAME  01_inrace_track.bmp:SAME  02_back_to_menu.bmp:SAME

## drawstream3d (MASHED_DBG_DRAWSTREAM3D=60:62)

Categories present on both sides with DIFFERENT numbers: 0
- base-only category `props`: 39 frame-records over 13 tracks, distinct values: ['{"batches": 0, "textured": 0, "verts": 0}']
- base-only category `world`: 39 frame-records over 13 tracks, distinct values: ['{"batches": 0, "textured": 0, "verts": 0}']

## librw scene line / registered model count

Mismatching tracks: 2
- ('08_Warzone', 'ok: scene built â€” sectors=42 mats=24 tris=11469 verts=12767 (dicts=1)', 'ok: scene built â€” sectors=16 mats=56 tris=23496 verts=26965 (dicts=1)', 14, 44)
- ('11_dump', 'ok: scene built â€” sectors=42 mats=24 tris=11469 verts=12767 (dicts=1)', 'ok: scene built â€” sectors=25 mats=40 tris=26152 verts=28083 (dicts=2)', 14, 20)
