#!/usr/bin/env bash
# D5-SHIM-STEP3-FRIDA5: one short original boot per migrated tool, private root without dinput8,
# plus a hooks-OFF control boot of catch_crash_eip. Run under mashed_lock (see RESULT). Each tool
# spawns its own pid via dev_asi.spawn_original and kills only that pid.
set -u
ROOT=D:/MashedWT/D5-SHIM-STEP3-FRIDA5_root
DATA=D:/MashedWT/D5-SHIM-STEP3-FRIDA5_data
RUN=${RUN:-run2}
export MASHED_ROOT=$ROOT
echo "[1/3] auto_count_at_menu (hooks ON, 12 s)"
test-slot py -3.12 re/frida/auto_count_at_menu.py --seconds 12 --scenario d5frida5 2>&1 | tee $DATA/${RUN}_auto_count_at_menu.log
echo "auto_count exit ${PIPESTATUS[0]}"
echo "[2/3] catch_crash_eip (hooks ON, 15 s)"
test-slot py -3.12 re/frida/catch_crash_eip.py --seconds 15 2>&1 | tee $DATA/${RUN}_catch_crash_eip.log
echo "catch_crash_eip exit ${PIPESTATUS[0]}"
echo "[3/3] catch_crash_eip control (hooks OFF, 15 s)"
test-slot py -3.12 re/frida/catch_crash_eip.py --seconds 15 --no-hooks 2>&1 | tee $DATA/${RUN}_catch_crash_eip_nohooks.log
echo "catch_crash_eip_nohooks exit ${PIPESTATUS[0]}"
