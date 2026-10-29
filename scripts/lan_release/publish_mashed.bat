@echo off
rem Build mashed_re.exe from race/first-frame-parity and publish it to the mini-server (http://mariano-server.local:8088/mashed/).
rem Pass --no-build to re-publish the last build, --ref <ref> to build another ref. Players then run update.bat in their Mashed folder.
"%ProgramFiles%\Git\bin\bash.exe" "%~dp0publish.sh" %*
pause
