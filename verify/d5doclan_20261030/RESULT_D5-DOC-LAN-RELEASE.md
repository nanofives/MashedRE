# RESULT D5-DOC-LAN-RELEASE

Verdict: GREEN. Text-only. One file changed: CLAUDE.md, first paragraph of "LAN release of the standalone (mini-server)". The Release rule paragraph is untouched.

Facts, each read on orch/integration (e23f260b7):

1. The stage ships no MASHED.exe. The icon texture is embedded.
   - scripts/lan_release/stage_lan_release.py:19-22 (release carries no original binary, RWTEXDICTIONARY/404 embedded in mashed_re.exe)
   - scripts/lan_release/stage_lan_release.py:24-25 (updater removes a MASHED.exe an earlier release left)
2. Boot movies come from TOASTART PC/MOVIES/backup/.
   - scripts/lan_release/stage_lan_release.py:43-44 (DATA_SKIP_DIRS skips the backup folder, MOVIES_DIR)
   - scripts/lan_release/stage_lan_release.py:100-103 (for each file in PC\MOVIES the stage uses backup\<name> when it exists, "the real boot movie, not the skip_intro stub")
   - header text at :9-11
3. build.bat embeds mashedmod/assets/mashed_re.rc with rc.exe.
   - mashedmod/build.bat:233-238 (rc /nologo /fo mashed_re.res assets\mashed_re.rc, then mashed_re.res on the cl link line)
   - mashedmod/assets/mashed_re.rc:5 (404 RWTEXDICTIONARY "loadicon_404.txd")

No gates run: no code or build input changed.
