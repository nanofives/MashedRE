# RESULT HB-LAN-REAL-MOVIES (L-63, 2026-10-30)

Change: `scripts/lan_release/stage_lan_release.py` stages `original/TOASTART/PC/MOVIES/backup/<name>.mpg`
in place of the 167,414-byte skip_intro stub `PC/MOVIES/<name>.mpg` (new `MOVIES_DIR` check in the data walk).
`backup/` itself is still not staged. `original/` is only read, its stubs stay. No .ps1 touched, nothing published or deployed.

Run: no MASHED.exe / mashed_re.exe process present before the stage (tasklist empty).
`stage_lan_release.py --app D:/MashedWT/HB-LAN-REAL-MOVIES_data/app --install original --out D:/MashedWT/HB-LAN-REAL-MOVIES_data/stage`
(app dir holds a dummy mashed_re.exe). Output: 205 files, 425.5 MB.

Manifest (D:/MashedWT/HB-LAN-REAL-MOVIES_data/stage/manifest.json):

| movie | size | sha256 | equals |
|---|---|---|---|
| intro.mpg | 37,072,900 | dbb08d35c970109c3f7402697dd924b6b6eb3c1109650caaa740c92dfed4cf62 | backup/intro.mpg |
| renderware.mpg | 4,463,728 | 177ddb4d65223b09f6c23d0940ee12ea014002f66b357775dba872714ef05423 | backup/renderware.mpg |
| supersonic.mpg | 6,445,060 | 33a6de9296b65f8716339dd78496abbcd70d9e142cb2695328f10e7335db74f8 | backup/supersonic.mpg |
| empire.mpg | 4,751,364 | f89f7d92d51a41846a94212f22b45ce95868e674f55dfe5f9851d1a1e8326118 | backup/empire.mpg |
| small.mpg | 24,580 | e709ef15fded167c2506c12d685a1660b49cd8c82aa9444ec4c2c7e5145c21d5 | backup/small.mpg |
| frontend.mpg | 38,778,884 | 6d12b398284bd3d6470545e88a85663d001ca25c408ac0c610290925382303b9 | original frontend.mpg (untouched) |

All five hashes match `sha256sum` of the backup files. No `backup/` path in the manifest. Dev `original/PC/MOVIES/intro.mpg`
still 167,414 bytes (sha a0e085d4...).
