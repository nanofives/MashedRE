#!/bin/bash
# Run inside `mashed_lock.py run --solo`. Family smokes + .ps1 boot CLI, no dinput8 root.
cd /c/Users/maria/Desktop/Proyectos/Mashed/.worktrees/D5-SHIM-DINPUT8-SPAWNERS || exit 2
D=D:/MashedWT/D5-SHIM-DINPUT8-SPAWNERS_data
R=D:/MashedWT/D5-SHIM-DINPUT8-SPAWNERS_root
echo "AUDIO VB-Audio endpoints=$(pwsh -NoProfile -Command "(Get-PnpDevice -Class AudioEndpoint -PresentOnly | ? FriendlyName -like '*VB-Audio*').Count")"
test-slot py -3.12 verify/d5dinput8spawners_20261101/run_family_smokes.py "$R" "$D" "$@"
echo "DRIVER_RC=$?"
MASHED_ROOT=$R test-slot py -3.12 re/frida/dev_asi.py boot --seconds 20 > "$D/smoke_boot_cli.log" 2>&1
echo "BOOTCLI_RC=$?"
tail -n 3 "$D/smoke_boot_cli.log"
