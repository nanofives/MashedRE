#!/usr/bin/env bash
# D4-DC-77b0-MATCHEND-SUBSTATE: every arm of the PREREG (section 4), one lock hold. Run from the worktree root under
# test-slot and mashed_lock.py run. Raw output to D:/MashedWT/D4-DC-77b0-MATCHEND-SUBSTATE_data.
set -u
S=D4-DC-77b0-MATCHEND-SUBSTATE
D=D:/MashedWT/${S}_data
R=D:/MashedWT/${S}_root
EXE=$D/mashed_re.exe
mkdir -p "$D/arms"
cp mashedmod/build/mashed_re.exe mashedmod/build/mashed_re.map "$D/"
sha256sum "$EXE" > "$D/exe.sha256"
export A4_OUT="$D/arms" A4_EXE="$EXE" A4_ROOT="$R"
py -3.12 verify/d4dc77b0matchend_20261101/run_arms.py H_DEF B_DEF_K0 B_DEF_K1 B_DEF_K26 H_ME B_ME_K0 B_ME_K1 B_ME_K26
P=verify/d4dc77b0matchend_20261101/exe_vsprobe.py
grab() { cp "$R/mashed_re.log" "$D/$1.mashed_re.log" 2>/dev/null; }
rm -f "$R/mashed_re.log"; py -3.12 $P --exe "$EXE" --root "$R" --out "$D/x_off" --env MASHED_R7_INPUT_PRESS=150; grab x_off
rm -f "$R/mashed_re.log"; py -3.12 $P --exe "$EXE" --root "$R" --out "$D/x_me" --env MASHED_R7_INPUT_PRESS=150 --env MASHED_MATCHEND_SUBSTATE=1; grab x_me
rm -f "$R/mashed_re.log"; py -3.12 $P --exe "$EXE" --root "$R" --out "$D/x_mep" --env MASHED_R7_INPUT_PRESS=150 --env MASHED_MATCHEND_SUBSTATE=1 --env MASHED_MATCHEND_PRESS=150; grab x_mep
echo RUN_ME_DONE
