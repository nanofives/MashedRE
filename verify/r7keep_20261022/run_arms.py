r"""r7keep arm runner (copy of verify/dcmix_20261022/run_arms.py, paths changed; adds the K / R7 / R7K stacks). dcmix arm runner (copy of verify/dcleaf_20261022/run_arms.py, paths changed). dcleaf arm runner (copy of verify/dcphys2_20261022/run_arms.py, paths changed). dcphys2 arm runner (copy of verify/dcphys_20261021/run_arms.py, paths changed). dcphys arm runner (copy of verify/d3close_20261021/run_arms.py, paths changed). d3close arm runner (copy of verify/soflip_20261021/run_arms.py; adds the NOPC24 bisect stack and the report-only SR stack). soflip arm runner (copy of verify/substep_20261021/run_arms.py; IL stacks removed with the fiber form, adds NOSX). substep arm runner (copy of verify/fflip_20261021/run_arms.py; adds the IL / SX / SXC / SXF stacks). fflip arm runner (copy of verify/rbflip_20261021/run_arms.py; base_env is the faithful recipe F: rule 0, game mode 10). rbflip arm runner (copy of verify/u9777_20261020/run_arms.py). u9777 arm runner (copy of verify/u9776_20261020/run_arms.py). u9776 arm runner (copy of verify/trinflip_20261020/run_arms.py). trinflip arm runner (copy of verify/wcsflip_20261020/run_arms.py). u9772 arm runner (copy of verify/u9771_20261019/run_arms.py). u9771 arm runner (copy of verify/u9770_20261019/run_arms.py). u9770 arm runner (copy of verify/u9769_20261019/run_arms.py). u9769 arm runner (copy of verify/d3bc_20261019/run_arms.py). d3bc arm runner (copy of verify/u9767_20261019/run_arms.py; B_ arms accept a -<rep> suffix). u9764 arm runner (copy of verify/u9762_20261019/run_arms.py). u9760 arm runner (copy of verify/u9759_20261018/run_arms.py). dflip arm runner (copied from verify/d3b_20261016/run_arms.py, same H_ / B_ / D2_ recipes; the W_ recipe is
verify/pontoon_20261016/run_arms.py's). Worktree build, explicit PIDs only (never by name).
usage: py -3.12 run_arms.py <arm> [<arm> ...]
  H_<tag>[-<rep>]     G-CTRL recipe: 14400 frames, PLAYERTRACE, AI_STEPDUMP, U9186_GATES
  B_<tag>_K<k>        D3 (b) arm: 1200 frames, MASHED_RING_SKIP=k, step dump only
  D2_<tag>_<n>        D2 arm: a8_run_port.py base env (90 s), MEASURE_SOLO / TRACK_SEL=12 / STEER_HOLD_AFTER=0
  W_<tag>[-<rep>]     Warzone live (pontoon recipe): rule 8, 3600 frames, PONTOON_TRACE + SEEK_TRACE + AI_STEPDUMP
  WL_<tag>[-<rep>]    the same at 18000 frames (300 s, the length of the original's OP3 capture): full-race no-crash arm
<tag> selects a knob stack from STACK. Tag DEF = the plain (new, dflip) default, no knob.
The exe runs with MASHED_ROOT = a private copy of original/ (the root CONTAINS original\), so CWD-relative
logs land in ROOT, never in the shared main tree. Every run appends one provenance line to RUNLOG.txt.
"""
import hashlib, os, shutil, subprocess, sys, time
W = os.path.dirname(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
OUT = os.path.normpath(os.environ.get('A4_OUT', 'D:/MashedWT/r7keep_data/arms'))
EXE_NEW = os.environ.get('A4_EXE', os.path.join(W, r'mashedmod\build\mashed_re.exe'))
EXE_BASE = EXE_NEW
ROOT = os.path.normpath(os.environ.get('A4_ROOT', 'D:/MashedWT/r7keep_root'))
OPTOUT = ['MASHED_NO_HEADING_ASSIST', 'MASHED_NO_PONTOON_CARRY', 'MASHED_NO_PONTOON_STATE', 'MASHED_NO_PONTOON_ANIM']
STACK = {'DEF': [],
         # every dflip opt-out: must give the pre-dflip default (964c31f04)
         'OPT': OPTOUT,
         'NOHA': ['MASHED_NO_HEADING_ASSIST'], 'NOPC': ['MASHED_NO_PONTOON_CARRY'], 'NOP': ['MASHED_NO_PONTOON_ANIM'],
         # the AI RW-LUT lookahead stacked on the new default (report only, not flipped)
         'L5': ['MASHED_AI_LUT=5'],
         # d3bB2: the U-9684 stacks (verify/d3_u9684_20261013/run_arms.py) on the dflip default
         'PP': ['MASHED_PATHPROG'], 'TGT': ['MASHED_PATHPROG', 'MASHED_TGT3'],
         'RG': ['MASHED_PATHPROG', 'MASHED_ROUND_GRID', 'MASHED_ROUND_GRID_ORDER'],
         'TGTRG': ['MASHED_PATHPROG', 'MASHED_ROUND_GRID', 'MASHED_ROUND_GRID_ORDER', 'MASHED_TGT3']}
# d3bB2 exploration: fix B (MASHED_POS_MODEL = parts 1-5, verify/d2posmodel_20261016 + d2engage_20261016) under the
# U-9684 stacks, so the parked player's first-step settle (FUN_004704c0's FUN_0046f6c0(0), 0x00470638) gives it the
# original's grid progress 29.999977 (d2engage G-PROG)
STACK['CC'] = ['MASHED_AI_COUNTDOWN_CLOCK']           # d3bB2 PREREG section 1
STACK['CCB'] = ['MASHED_AI_COUNTDOWN_CLOCK', 'MASHED_POS_MODEL', 'MASHED_PATHPROG', 'MASHED_ROUND_GRID',
                'MASHED_ROUND_GRID_ORDER', 'MASHED_TGT3']   # report: the knob on the full U-9684 chain
STACK['CCBL'] = STACK['CCB'] + ['MASHED_CARCAR_LOG=@OUT@\\@NAME@.cc.txt']   # + the port's car-car pair log
STACK['CCBL5'] = STACK['CCBL'] + ['MASHED_AI_LUT=5']                          # + the d3b AI RW-LUT lookahead
STACK['CC5'] = STACK['CC'] + ['MASHED_AI_LUT=5']
STACK['CCBD'] = STACK['CCB'] + ['MASHED_GRID_DIR']                            # + FUN_00408b00's spawn-yaw law (gridyaw)
STACK['CCBDL'] = STACK['CCBD'] + ['MASHED_CARCAR_LOG=@OUT@\\@NAME@.cc.txt']
STACK['CCD'] = STACK['CC'] + ['MASHED_GRID_DIR']
# PREREG amendment 1: S = the knob + the U-9684 chain + GRID_DIR + both new knobs; SmE / SmT drop one of them
_S = STACK['CCBD'] + ['MASHED_AI_CONTACT_EVENT', 'MASHED_AI_TICK_FIRST']
STACK['S'] = _S
STACK['SmE'] = [k for k in _S if k != 'MASHED_AI_CONTACT_EVENT']
STACK['SmT'] = [k for k in _S if k != 'MASHED_AI_TICK_FIRST']
for _t in ('S', 'SmE', 'SmT'):
    STACK[_t + 'L'] = STACK[_t] + ['MASHED_CARCAR_LOG=@OUT@\\@NAME@.cc.txt', 'MASHED_AI_CONTACT_EVENT_TRACE=@OUT@\\@NAME@.ev.txt',
                                   'MASHED_TGT3_TRACE=@OUT@\\@NAME@.tgt3.csv']
STACK['E'] = ['MASHED_AI_CONTACT_EVENT']; STACK['T'] = ['MASHED_AI_TICK_FIRST']; STACK['CCE'] = ['MASHED_AI_COUNTDOWN_CLOCK', 'MASHED_AI_CONTACT_EVENT']
STACK['CCET'] = STACK['CCE'] + ['MASHED_AI_TICK_FIRST']
STACK['EL'] = STACK['E'] + ['MASHED_CARCAR_LOG=@OUT@\\@NAME@.cc.txt', 'MASHED_AI_CONTACT_EVENT_TRACE=@OUT@\\@NAME@.ev.txt']
STACK['DEFL'] = ['MASHED_CARCAR_LOG=@OUT@\\@NAME@.cc.txt']
STACK['SLP'] = STACK['SL'] + ['MASHED_PATHPROG_TRACE=@OUT@\\@NAME@.pathprog.csv']
STACK['B'] = ['MASHED_POS_MODEL']
# a4order part 1: the countdown clock is default ON; NOCC is the opt-out (= the pre-a4order default, dflip)
STACK['NOCC'] = ['MASHED_NO_AI_COUNTDOWN_CLOCK']
# a4order part 2 (PREREG_a4order.md section 3): MASHED_A4_ORDER implies MASHED_AI_TICK_FIRST
_A4 = ['MASHED_A4_ORDER', 'MASHED_A4_ORDER_TRACE=@OUT@\\@NAME@.a4.txt']
STACK['A'] = _A4
STACK['AE'] = _A4 + ['MASHED_AI_CONTACT_EVENT']
STACK['TE'] = ['MASHED_AI_TICK_FIRST', 'MASHED_AI_CONTACT_EVENT']
STACK['SA'] = _S + _A4                                         # S + the A4 order
STACK['SAP'] = STACK['SA'] + ['MASHED_PLAYERTRACE']            # + the player trace (G-COAST2 cause)
# u9749: record dumps (MSD1) of slot 0 / slot 2, end of frame and at the A4 site
_HP = ['MASHED_HEADING_ASSIST_PLAYER']
STACK['NOHP'] = ['MASHED_NO_HEADING_ASSIST_PLAYER']
# u9745ov: the clock add at the end of the frame
STACK['CL'] = ['MASHED_AI_CLOCK_LATE']; STACK['SACL'] = STACK['SA'] + ['MASHED_AI_CLOCK_LATE']
# c2k93: the FUN_00470c70 speed refresh 0x004710a1..0x004710da (LUT form / exact form) on top of SACL
STACK['SACLR'] = STACK['SACL'] + ['MASHED_D2LAT_REFRESH']; STACK['SACLS'] = STACK['SACL'] + ['MASHED_SPEED_REFRESH']
STACK['SACL5'] = STACK['SACL'] + ['MASHED_AI_LUT=5']
STACK['SACLR5'] = STACK['SACLR'] + ['MASHED_AI_LUT=5']
STACK['CLR'] = STACK['CL'] + ['MASHED_D2LAT_REFRESH']; STACK['CL5'] = STACK['CL'] + ['MASHED_AI_LUT=5']
STACK['CLR5'] = STACK['CLR'] + ['MASHED_AI_LUT=5']
# d2ref (PREREG_d2ref.md): the D2 refresh chain on the 6242c84b6 default
STACK['R'] = ['MASHED_D2LAT_REFRESH']; STACK['G'] = ['MASHED_GRID_DIR']; STACK['RG'] = STACK['R'] + STACK['G']
STACK['RY'] = STACK['R'] + ['MASHED_D2LAT_SPAWN_YAW=-1.5707964']
STACK['RGB'] = STACK['RG'] + ['MASHED_POS_MODEL']
# d2ref amendment 1: U-9751 (A5 race timer) alone and on the two refresh pairs, with the timer log
_T = ['MASHED_A5_RACE_TIMER', 'MASHED_A5_RACE_TIMER_LOG']
STACK['T5'] = _T; STACK['RGT'] = STACK['RG'] + _T; STACK['RYT'] = STACK['RY'] + _T
STACK['SR3'] = STACK['S'] + ['MASHED_RECDUMP=@OUT@\\@NAME@.rec.msd', 'MASHED_RECDUMP_SLOT=3', 'MASHED_AI_DIAG']   # c3k44
STACK['TPPT'] = ['MASHED_AI_TICK_FIRST', 'MASHED_PATHPROG', 'MASHED_PATHPROG_TRACE=@OUT@\\@NAME@.pathprog.csv',
                 'MASHED_TGT3_TRACE=@OUT@\\@NAME@.tgt3.csv']   # u9747: tick first + path progress, traced
STACK['TPPG'] = STACK['TPPT'] + ['MASHED_PATHPROG_GATE_LOG=@OUT@\\@NAME@.gate.txt']
STACK['NOCS'] = ['MASHED_NO_COUNTDOWN_SPAWN']   # after the flip: the opt-out (= the 438566c88 default)
STACK['CS'] = ['MASHED_COUNTDOWN_SPAWN']; STACK['TCS'] = ['MASHED_AI_TICK_FIRST', 'MASHED_COUNTDOWN_SPAWN']
STACK['TPPGC'] = STACK['TPPG'] + ['MASHED_COUNTDOWN_SPAWN']; STACK['SCS'] = STACK['S'] + ['MASHED_COUNTDOWN_SPAWN']
STACK['TGTE'] = STACK['TGT'] + ['MASHED_AI_CONTACT_EVENT']   # cev PREREG section 2
STACK['TGTEL'] = STACK['TGTE'] + ['MASHED_CARCAR_LOG=@OUT@\\@NAME@.cc.txt', 'MASHED_AI_CONTACT_EVENT_TRACE=@OUT@\\@NAME@.ev.txt',
                                  'MASHED_TGT3_TRACE=@OUT@\\@NAME@.tgt3.csv']
STACK['SPL'] = STACK['S'] + ['MASHED_PATHPROG_LATE']; STACK['SPLE'] = [k for k in STACK['SPL'] if k != 'MASHED_AI_CONTACT_EVENT']   # cev amendment 2
STACK['SPLL'] = STACK['SPL'] + ['MASHED_CARCAR_LOG=@OUT@\\@NAME@.cc.txt', 'MASHED_AI_CONTACT_EVENT_TRACE=@OUT@\\@NAME@.ev.txt']
STACK['TGTPL'] = STACK['TGT'] + ['MASHED_PATHPROG_LATE']
STACK['SB'] = ['MASHED_SPAWN_BASIS_RW']; STACK['SPLSB'] = STACK['SPL'] + ['MASHED_SPAWN_BASIS_RW']   # lk21 PREREG section 2
STACK['SPLSBR1'] = STACK['SPLSB'] + ['MASHED_RECDUMP=@OUT@\\@NAME@.rec.msd', 'MASHED_RECDUMP_SLOT=1']
STACK['SPLUT'] = STACK['SPL'] + ['MASHED_UP_TRINORMAL']; STACK['SPLSBUT'] = STACK['SPLSB'] + ['MASHED_UP_TRINORMAL']   # lk21 amendment 1
STACK['SPLSBUTR1'] = STACK['SPLSBUT'] + ['MASHED_RECDUMP=@OUT@\\@NAME@.rec.msd', 'MASHED_RECDUMP_SLOT=1']
STACK['KN'] = ['MASHED_REASSERT_KEEP_NORMAL']; STACK['SPLKN'] = STACK['SPL'] + STACK['KN']   # u9754 PREREG section 2
STACK['SPLKNR1'] = STACK['SPLKN'] + ['MASHED_RECDUMP=@OUT@\\@NAME@.rec.msd', 'MASHED_RECDUMP_SLOT=1']
STACK['SPLSBKN'] = STACK['SPLKN'] + ['MASHED_SPAWN_BASIS_RW']; STACK['SBKN'] = ['MASHED_SPAWN_BASIS_RW'] + STACK['KN']   # u9754 amendment 1
STACK['SPLSBKNR1'] = STACK['SPLSBKN'] + ['MASHED_RECDUMP=@OUT@\\@NAME@.rec.msd', 'MASHED_RECDUMP_SLOT=1']
STACK['SY'] = ['MASHED_SPAWN_Y_LINE']; STACK['SPLSBKNSY'] = STACK['SPLSBKN'] + STACK['SY']   # u9755 PREREG section 2
STACK['SPLSBKNSYR1'] = STACK['SPLSBKNSY'] + ['MASHED_RECDUMP=@OUT@\\@NAME@.rec.msd', 'MASHED_RECDUMP_SLOT=1']; STACK['TPPGSY'] = STACK['TPPG'] + STACK['SY']
STACK['SYL'] = STACK['SY'] + ['MASHED_SPAWN_Y_LOG=@OUT@\\@NAME@.sy.txt']   # u9755 diagnostic log
STACK['SPLSBKNSYD'] = STACK['SPLSBKNSY'] + ['MASHED_SETTLE_DIAG=@OUT@\\@NAME@.settle.txt']   # u9755 diagnostic
STACK['SPLSBKND'] = STACK['SPLSBKN'] + ['MASHED_SETTLE_DIAG=@OUT@\\@NAME@.settle.txt']   # u9755 diagnostic (knob off)
STACK['NOSY'] = ['MASHED_NO_SPAWN_Y_LINE']   # syflip opt-out (= the 5d9f197be default)
STACK['SSD'] = ['MASHED_SPAWN_SOLVER_DIAG=@OUT@\\@NAME@.ssd.txt']; STACK['SPLSBKNSSD'] = STACK['SPLSBKN'] + STACK['SSD']   # u9756 diagnostic
STACK['F60'] = ['MASHED_FIX_5CEA60']; STACK['F44'] = ['MASHED_FIX_5CEA44']; STACK['F6044'] = STACK['F60'] + STACK['F44']   # u9756 PREREG section 2
STACK['SPLSBKNF60'] = STACK['SPLSBKN'] + STACK['F60']; STACK['SPLSBKNF60SSD'] = STACK['SPLSBKNF60'] + STACK['SSD']
STACK['SPLSBKNF60R1'] = STACK['SPLSBKNF60'] + ['MASHED_RECDUMP=@OUT@\\@NAME@.rec.msd', 'MASHED_RECDUMP_SLOT=1']
# u9152 PREREG section 2: the original's +0x928 / +0x968 double buffer (MASHED_RING_DBUF) and the exhausted-retry discard
STACK['DB'] = ['MASHED_RING_DBUF']; STACK['DBX'] = STACK['DB'] + ['MASHED_RING_DBUF_DISCARD']
STACK['SPLSBKNF60DB'] = STACK['SPLSBKNF60'] + STACK['DB']; STACK['SPLSBKNF60DBX'] = STACK['SPLSBKNF60'] + STACK['DBX']
STACK['SPLSBKNF60DBR1'] = STACK['SPLSBKNF60DB'] + ['MASHED_RECDUMP=@OUT@\\@NAME@.rec.msd', 'MASHED_RECDUMP_SLOT=1']
STACK['SPLSBKNF60DBL'] = STACK['SPLSBKNF60DB'] + ['MASHED_CARCAR_LOG=@OUT@\\@NAME@.cc.txt']; STACK['SPLSBKNF60L'] = STACK['SPLSBKNF60'] + ['MASHED_CARCAR_LOG=@OUT@\\@NAME@.cc.txt']
STACK['DBL'] = STACK['DB'] + ['MASHED_CARCAR_LOG=@OUT@\\@NAME@.cc.txt']
STACK['SPLSBKNF60UTR1'] = STACK['SPLSBKNF60R1'] + ['MASHED_UP_TRINORMAL']; STACK['SPLSBKNF60DBUTR1'] = STACK['SPLSBKNF60DBR1'] + ['MASHED_UP_TRINORMAL']   # u9152 amendment 1
# u9758 PREREG section 2: record +0x9d4 = the basis `at` row (MASHED_FWD_AT)
STACK['FA'] = ['MASHED_FWD_AT']; STACK['SPLSBKNF60FA'] = STACK['SPLSBKNF60'] + STACK['FA']
STACK['SPLSBKNF60FAR1'] = STACK['SPLSBKNF60R1'] + STACK['FA']; STACK['SPLSBKNF60UTFAR1'] = STACK['SPLSBKNF60UTR1'] + STACK['FA']
STACK['F60FA'] = STACK['F60'] + STACK['FA']
# u9759: car-1 batch dump (wheel points + up vector at the classifier) on the u9756 drift stack, UP_TRINORMAL on / off
STACK['UTBQ1'] = STACK['SPLSBKNF60UTR1'] + ['MASHED_BATCH_DUMP=@OUT@\\@NAME@.bq.csv', 'MASHED_BATCH_DUMP_SLOT=1']
STACK['BQ1'] = STACK['SPLSBKNF60R1'] + ['MASHED_BATCH_DUMP=@OUT@\\@NAME@.bq.csv', 'MASHED_BATCH_DUMP_SLOT=1']
# u9759 PREREG section 2: MASHED_RW_SSE_POINTS (slot +0xc = 0x004dc370's order)
STACK['SSE'] = ['MASHED_RW_SSE_POINTS']; STACK['UTBQ1SSE'] = STACK['UTBQ1'] + STACK['SSE']; STACK['BQ1SSE'] = STACK['BQ1'] + STACK['SSE']
# u9760 PREREG section 2: MASHED_A5_SUSP_LUT (A5 Phase 6 all-grounded force, 0x0046e895..0x0046e925)
STACK['A5L'] = ['MASHED_A5_SUSP_LUT']
STACK['XR1'] = STACK['SPLSBKNF60R1'] + ['MASHED_UP_TRINORMAL', 'MASHED_RW_SSE_POINTS', 'MASHED_RING_DBUF', 'MASHED_FWD_AT']
STACK['XA5LR1'] = STACK['XR1'] + STACK['A5L']
STACK['XFS'] = STACK['XR1'] + ['MASHED_FORCE_SPLIT=@OUT@/@NAME@.fs.csv']; STACK['XA5LFS'] = STACK['XA5LR1'] + ['MASHED_FORCE_SPLIT=@OUT@/@NAME@.fs.csv']   # u9760 split
STACK['A6L'] = ['MASHED_A6A_LUT']; STACK['XA6FS'] = STACK['XFS'] + STACK['A6L']; STACK['XA5A6FS'] = STACK['XA5LFS'] + STACK['A6L']   # u9760 amendment 1
# u9761 PREREG section 2: MASHED_A6A_F32 (A6a torque chain at float precision), alone and with MASHED_A6A_LUT
STACK['F32'] = ['MASHED_A6A_F32']; STACK['A6LF32'] = STACK['A6L'] + STACK['F32']
STACK['XA6F32FS'] = STACK['XA6FS'] + STACK['F32']
STACK['XW3'] = STACK['XA6F32FS'] + ['MASHED_FORCE_SPLIT_WHEEL=3']   # u9762: wheel-3 split on the u9761 pair stack
STACK['XW3A5'] = STACK['XW3'] + ['MASHED_A5_SUSP_LUT']; STACK['XA5F'] = STACK['XA6F32FS'] + ['MASHED_A5_SUSP_LUT']   # u9762: + A5's original arithmetic
STACK['TRI'] = ['MASHED_A6A_LUT', 'MASHED_A6A_F32', 'MASHED_A5_SUSP_LUT']   # u9762 candidate
STACK['XW1'] = STACK['XFS'] + ['MASHED_FORCE_SPLIT_WHEEL=1']   # u9764: wheel-1 split on the u9756 drift stack + UT/DBUF; A5/A6a knobs now default
STACK['LC'] = ['MASHED_A6A_LC0']; STACK['XW1LC'] = STACK['XW1'] + STACK['LC']   # u9764 PREREG section 1
STACK['RL'] = ['MASHED_ROTATE_LUT']; STACK['XW1RL'] = STACK['XW1'] + STACK['RL']   # u9764 PREREG amendment 1
STACK['LA'] = ['MASHED_RW_LUT_ALL']; STACK['XW1LA'] = STACK['XW1'] + STACK['LA']   # u9764 PREREG amendment 2
STACK['OL'] = ['MASHED_ORTHO_LUT']; STACK['RLO'] = STACK['RL'] + STACK['OL']   # u9765 PREREG section 1
STACK['XW1OL'] = STACK['XW1'] + STACK['OL']; STACK['XW1RLO'] = STACK['XW1'] + STACK['RLO']
STACK['XW0RLO'] = STACK['XFS'] + ['MASHED_FORCE_SPLIT_WHEEL=0'] + STACK['RLO']   # u9766: wheel-0 split on RL + OL
STACK['RLOLC'] = STACK['RLO'] + STACK['LC']; STACK['XW0RLOLC'] = STACK['XW0RLO'] + STACK['LC']   # u9766 PREREG section 1
STACK['P24'] = ['MASHED_ROTATE_PC24']; STACK['RLOLCP'] = STACK['RLOLC'] + STACK['P24']; STACK['XW0RLOLCP'] = STACK['XW0RLOLC'] + STACK['P24']   # u9766 amendment 1
STACK['F'] = ['MASHED_HEADING_FWD_LIVE']; STACK['RLOLCPF'] = STACK['RLOLCP'] + STACK['F']; STACK['XW0RLOLCPF'] = STACK['XW0RLOLCP'] + STACK['F']   # u9767 PREREG section 1
STACK['SPLR1'] = STACK['SPL'] + ['MASHED_RECDUMP=@OUT@\\@NAME@.rec.msd', 'MASHED_RECDUMP_SLOT=1']   # lk21
STACK['KT'] = STACK['TGT'] + ['MASHED_AI_TICK_FIRST']; STACK['KTE'] = STACK['KT'] + ['MASHED_AI_CONTACT_EVENT']   # cev amendment 1
STACK['KTEL'] = STACK['KTE'] + ['MASHED_CARCAR_LOG=@OUT@\\@NAME@.cc.txt', 'MASHED_AI_CONTACT_EVENT_TRACE=@OUT@\\@NAME@.ev.txt']
STACK['ELR'] =STACK['EL'] + ['MASHED_RECDUMP=@OUT@\\@NAME@.rec.msd', 'MASHED_RECDUMP_SLOT=3']   # cev: event + pair logs + car 3 records
STACK['RGX'] = STACK['RG'] + ['MASHED_A5_RACE_TIMER']   # RGT without the diagnostic log: the (b) / G-CTRL arms
# d2ref flip: the three opt-outs (-> the 6242c84b6 default)
STACK['NOD2'] = ['MASHED_NO_D2LAT_REFRESH', 'MASHED_NO_GRID_DIR', 'MASHED_NO_A5_RACE_TIMER']
# c2k93 flip: opt-outs (both -> the previous default; LUT only -> the CL arm)
STACK['NOCL5'] = ['MASHED_NO_AI_CLOCK_LATE', 'MASHED_NO_AI_LUT']; STACK['NOL5'] = ['MASHED_NO_AI_LUT']
STACK['SACLR2'] =STACK['SACLR'] + ['MASHED_RECDUMP=@OUT@\\@NAME@.rec.msd', 'MASHED_RECDUMP_SLOT=2']
# u9745ov (after the flip: the player assist is default): SA + the slot-2 record dumps
STACK['SAR2F'] = STACK['SA'] + ['MASHED_RECDUMP=@OUT@\\@NAME@.rec.msd', 'MASHED_RECDUMP_A4=@OUT@\\@NAME@.a4rec.msd', 'MASHED_RECDUMP_SLOT=2']   # after the flip: the opt-out (= the pre-flip default)
STACK['P'] = _HP; STACK['AHP'] = STACK['A'] + _HP; STACK['SAH'] = STACK['SA'] + _HP
STACK['SAHR0'] = STACK['SAH'] + ['MASHED_RECDUMP=@OUT@\\@NAME@.rec.msd', 'MASHED_RECDUMP_A4=@OUT@\\@NAME@.a4rec.msd', 'MASHED_RECDUMP_SLOT=0']
for _s in (0, 2):
    STACK['SAR%d' % _s] = STACK['SA'] + ['MASHED_RECDUMP=@OUT@\\@NAME@.rec.msd', 'MASHED_RECDUMP_A4=@OUT@\\@NAME@.a4rec.msd', 'MASHED_RECDUMP_SLOT=%d' % _s]
STACK['BRG'] = STACK['B'] + STACK['RG']
STACK['BPP'] = STACK['B'] + STACK['PP']
STACK['BTGT'] = STACK['B'] + STACK['TGT']
STACK['BTGTRG'] = STACK['B'] + STACK['TGTRG']
STACK['BTGTRGD'] = STACK['BTGTRG'] + ['MASHED_GRID_DIR']
# d3bc: regime C targeting stacks (FUN_00414a70 MASHED_TGT3 needs MASHED_PATHPROG; FUN_00414c30 MASHED_MODE7)
STACK['M7'] = ['MASHED_MODE7']; STACK['TGTM7'] = STACK['TGT'] + STACK['M7']; STACK['TGTEM7'] = STACK['TGTE'] + STACK['M7']
STACK['EM7'] = STACK['E'] + STACK['M7']
# d3bc: EX = the u9767 car-1-exact stack (XW0RLOLCPF) without the force-split diagnostics; R1 / R2 / R3 add a record dump
STACK['EX'] = [k for k in STACK['XW0RLOLCPF'] if not k.startswith('MASHED_FORCE_SPLIT')]
for _s in (1, 2, 3):
    STACK['EXR%d' % _s] = STACK['EX'] + ['MASHED_RECDUMP=@OUT@\\@NAME@.rec.msd', 'MASHED_RECDUMP_SLOT=%d' % _s]
STACK['EXL7'] = STACK['EX'] + ['MASHED_AI_LUT=7']; STACK['L7'] = ['MASHED_AI_LUT=7']   # d3bc: + FUN_00415e20's LUT normalize (bit 2)
STACK['EXL7R1'] = STACK['EXL7'] + ['MASHED_RECDUMP=@OUT@\\@NAME@.rec.msd', 'MASHED_RECDUMP_SLOT=1']   # d3bc
STACK['A64'] = ['MASHED_STEER_ACOS_CRT']; STACK['L7A'] = STACK['L7'] + STACK['A64']; STACK['EXL7A'] = STACK['EXL7'] + STACK['A64']   # d3bc PREREG section 1; amendment 4: A64 was MASHED_STEER_ACOS64 (double acos) until 555738fd9
STACK['EXL7AR1'] = STACK['EXL7A'] + ['MASHED_RECDUMP=@OUT@\\@NAME@.rec.msd', 'MASHED_RECDUMP_SLOT=1']
STACK['F2'] = ['MASHED_AI_FWD_ATROW']; STACK['L7AF'] = STACK['L7A'] + STACK['F2']; STACK['EXL7AF'] = STACK['EXL7A'] + STACK['F2']   # d3bc amendment 1
STACK['EXL7AFR1'] = STACK['EXL7AF'] + ['MASHED_RECDUMP=@OUT@\\@NAME@.rec.msd', 'MASHED_RECDUMP_SLOT=1']
for _s in (2, 3): STACK['EXL7AFR%d' % _s] = STACK['EXL7AF'] + ['MASHED_RECDUMP=@OUT@\\@NAME@.rec.msd', 'MASHED_RECDUMP_SLOT=%d' % _s]
STACK['SC'] = ['MASHED_STEER_SCALE_RDATA']; STACK['L7AFS'] = STACK['L7AF'] + STACK['SC']; STACK['EXL7AFS'] = STACK['EXL7AF'] + STACK['SC']   # d3bc amendment 2
for _s in (1, 2, 3): STACK['EXL7AFSR%d' % _s] = STACK['EXL7AFS'] + ['MASHED_RECDUMP=@OUT@\\@NAME@.rec.msd', 'MASHED_RECDUMP_SLOT=%d' % _s]
STACK['M4SET'] = ['MASHED_REGISTRY', 'MASHED_PICKUPS_ORIG', 'MASHED_REG_CARS', 'MASHED_REG_CALLBACKS', 'MASHED_MODE7']   # d3bc amendment 3
STACK['EXL7AFSM7'] = STACK['EXL7AFS'] + STACK['M4SET']; STACK['EXL7AFSM7R1'] = STACK['EXL7AFSR1'] + STACK['M4SET']
for _s in (2, 3): STACK['EXL7AFSM7R%d' % _s] = STACK['EXL7AFSR%d' % _s] + STACK['M4SET']   # d3bc amendment 4
STACK['EXL7AFSM7R0'] = STACK['EXL7AFSM7'] + ['MASHED_RECDUMP=@OUT@\\@NAME@.rec.msd', 'MASHED_RECDUMP_SLOT=0']   # u9769: the player's record
STACK['MI'] = ['MASHED_CC_MATRIX_INVERT']; STACK['EXL7AFSM7MI'] = STACK['EXL7AFSM7'] + STACK['MI']; STACK['EXL7AFSM7MIR0'] = STACK['EXL7AFSM7R0'] + STACK['MI']   # u9769 amendment 1
_TR = ['MASHED_CC_TRACE=@OUT@\\@NAME@.cctrace.tsv']; STACK['EXL7AFSM7TR'] = STACK['EXL7AFSM7R0'] + _TR; STACK['EXL7AFSM7MITR'] = STACK['EXL7AFSM7MIR0'] + _TR   # u9769
STACK['EXL7AFSM7MIATR'] = STACK['EXL7AFSM7MITR'] + ['MASHED_A4_ORDER']; STACK['EXL7AFSM7MIA'] = STACK['EXL7AFSM7MI'] + ['MASHED_A4_ORDER']   # u9769 amendment 2
STACK['PC'] = ['MASHED_CC_PC24']; STACK['EXL7AFSM7PCA'] = STACK['EXL7AFSM7'] + STACK['PC'] + ['MASHED_A4_ORDER']; STACK['EXL7AFSM7PCATR'] = STACK['EXL7AFSM7TR'] + STACK['PC'] + ['MASHED_A4_ORDER']   # u9769 amendment 3
for _s in (2, 3): STACK['EXL7AFSM7PCAR%d' % _s] = [k for k in STACK['EXL7AFSM7PCA'] if not k.startswith('MASHED_RECDUMP')] + ['MASHED_RECDUMP=@OUT@\\@NAME@.rec.msd', 'MASHED_RECDUMP_SLOT=%d' % _s]   # u9769
STACK['FULLM7T'] = STACK['EXL7AFSM7PCA'] + ['MASHED_MODE7_TRACE=@OUT@\\@NAME@.m7.tsv']   # u9770
STACK['RL'] = ['MASHED_REG_LATE']; STACK['FULLRL'] = STACK['FULLM7T'] + STACK['RL']   # u9770 PREREG section 1
for _s in (2, 3): STACK['FULLRLR%d' % _s] = [k for k in STACK['FULLRL'] if not k.startswith('MASHED_RECDUMP')] + ['MASHED_RECDUMP=@OUT@' + chr(92) + '@NAME@.rec.msd', 'MASHED_RECDUMP_SLOT=%d' % _s]   # u9770
for _s in (2, 3):
    for _w in range(4):   # u9771: force split of slot s wheel w on FULLRL, plus that slot's record dump
        STACK['FS%d%d' % (_s, _w)] = [k for k in STACK['FULLRL'] if not k.startswith('MASHED_RECDUMP')] + [
            'MASHED_FORCE_SPLIT=@OUT@/@NAME@.fs.csv', 'MASHED_FORCE_SPLIT_SLOT=%d' % _s, 'MASHED_FORCE_SPLIT_WHEEL=%d' % _w,
            'MASHED_RECDUMP=@OUT@/@NAME@.rec.msd', 'MASHED_RECDUMP_SLOT=%d' % _s]
STACK['DL'] = ['MASHED_A5_DRAFT_LUT']; STACK['FS30DL'] = STACK['FS30'] + STACK['DL']; STACK['FULLRLDL'] = STACK['FULLRL'] + STACK['DL']   # u9771 PREREG section 1
STACK['FULLRLA5G'] = STACK['FULLRL'] + ['MASHED_A5GDIAG']   # u9771 diagnostic (writes a5g_diag.log in the root)
STACK['VZ3'] = STACK['FULLRL'] + ['MASHED_FORCE_SPLIT_SLOT=3', 'MASHED_A6A_VZLOG=@OUT@/@NAME@.vz.csv']   # u9771 diagnostic
STACK['TL3'] = STACK['VZ3'] + ['MASHED_A6A_TAILLOG=@OUT@/@NAME@.tail.csv']   # u9771 diagnostic
STACK['DN'] = ['MASHED_A6A_DRIVE_NORM_LUT']; STACK['TL3DN'] = STACK['TL3'] + STACK['DN']; STACK['FULLRLDN'] = STACK['FULLRL'] + STACK['DN']   # u9771 amendment 1
STACK['FULLRLDNR3'] = [k for k in STACK['FULLRLDN'] if not k.startswith('MASHED_RECDUMP')] + ['MASHED_RECDUMP=@OUT@/@NAME@.rec.msd', 'MASHED_RECDUMP_SLOT=3']
STACK['FULLRLDNR2'] = [k for k in STACK['FULLRLDN'] if not k.startswith('MASHED_RECDUMP')] + ['MASHED_RECDUMP=@OUT@/@NAME@.rec.msd', 'MASHED_RECDUMP_SLOT=2']
STACK['DR3'] = STACK['TL3'] + ['MASHED_A6A_DRIVELOG=@OUT@/@NAME@.drive.csv']   # u9771 diagnostic
STACK['GO'] = ['MASHED_A6A_GEAR_ORDER']; STACK['TL3GO'] = STACK['TL3'] + STACK['GO']; STACK['FULLRLGO'] = STACK['FULLRL'] + STACK['GO']   # u9771 amendment 2
for _s in (2, 3): STACK['FULLRLGOR%d' % _s] = [k for k in STACK['FULLRLGO'] if not k.startswith('MASHED_RECDUMP')] + ['MASHED_RECDUMP=@OUT@/@NAME@.rec.msd', 'MASHED_RECDUMP_SLOT=%d' % _s]
STACK['FULLGO'] = STACK['FULLRLGO']   # u9772: the u9771 stack + MASHED_A6A_GEAR_ORDER
for _s in (2, 3):
    STACK['TLG%d' % _s] = [k for k in STACK['FULLRLGO'] if not k.startswith('MASHED_RECDUMP')] + ['MASHED_FORCE_SPLIT_SLOT=%d' % _s,
        'MASHED_A6A_VZLOG=@OUT@/@NAME@.vz.csv', 'MASHED_A6A_TAILLOG=@OUT@/@NAME@.tail.csv', 'MASHED_A6A_DRIVELOG=@OUT@/@NAME@.drive.csv',
        'MASHED_A5GDIAG', 'MASHED_RECDUMP=@OUT@/@NAME@.rec.msd', 'MASHED_RECDUMP_SLOT=%d' % _s]
for _t in list(STACK):   # <tag>T = the same stack with the FUN_00414a70 trace (MASHED_TGT3_TRACE) per arm
    # a4order: never overwrite a defined tag. In d3bB2's copy this loop replaced CCET (clock + event + tick first) with
    # CCE + traces, so every d3bB2 CCET run ran without MASHED_AI_TICK_FIRST (its RUNLOG lines carry no such knob).
    if _t + 'T' in STACK: continue
    STACK[_t + 'T'] =STACK[_t] + ['MASHED_TGT3_TRACE=@OUT@\\@NAME@.tgt3.csv', 'MASHED_PATHPROG_TRACE=@OUT@\\@NAME@.pathprog.csv']
D2BASE = {'MASHED_REAL_PHYSICS': '1', 'MASHED_RACE_DEMO': '1', 'MASHED_PLAY_DEMO': '1', 'MASHED_GOTO': '6',
          'MASHED_TRACK_SEL': '12', 'MASHED_CAR_SEL': '0', 'MASHED_DRIVE_HOLD': '1', 'MASHED_WIN_POS': 'primary-bl',
          'MASHED_MOTION_DIAG': '1', 'MASHED_STEER_HOLD': '1', 'MASHED_STEER_HOLD_AFTER': '0', 'MASHED_MUTE': '1',
          'MASHED_MEASURE_SOLO': '1'}


def sha8(p):
    return hashlib.sha256(open(p, 'rb').read()).hexdigest()[:8]


def clean_env():
    e = {k: v for k, v in os.environ.items() if not k.upper().startswith('MASHED_')}
    e['MASHED_ROOT'] = ROOT
    return e


def base_env(frames):
    e = clean_env()
    e.update(MASHED_MUTE='1', MASHED_TRACK_VIEW='Training', MASHED_CAR='1', MASHED_ROUND='1',
             MASHED_ROUND_RULE='0', MASHED_GAME_MODE='10', MASHED_WIN_POS='primary-bl', MASHED_DETERMINISTIC='1',
             MASHED_DET_FRAMES=str(frames))
    return e


CUR = {'name': ''}


def knobs(e, ks):
    for k in ks:
        if k.startswith('-'):   # fflip: '-MASHED_X' removes a base_env variable
            e.pop(k[1:], None)
        elif '=' in k:
            a, b = k.split('=', 1); e[a] = b.replace('@OUT@', OUT).replace('@NAME@', CUR['name'])
        else:
            e[k] = '1'


def tag_of(rest):
    return rest.split('-', 1)[0] or 'DEF'


def run(name, env, files, limit, exe, cwd_logs=('mashed_re.log', 'player_trace.log', 'motion_diag.log', 'a5g_diag.log'), wall=None):
    for f in [os.path.join(ROOT, c) for c in cwd_logs] + files:
        if os.path.exists(f): os.remove(f)
    t0 = time.time()
    p = subprocess.Popen([exe], cwd=ROOT, env=env)
    print(f'=== {name} pid={p.pid} exe={sha8(exe)}', flush=True)
    ok = True
    try:
        if wall:
            time.sleep(wall)
            if p.poll() is None:
                print(f'    wall {wall}s reached - killing pid {p.pid} ONLY', flush=True); p.kill()
            rc = p.wait(); ok = False
        else:
            rc = p.wait(limit)
    except subprocess.TimeoutExpired:
        print(f'    TIMEOUT - killing pid {p.pid} ONLY', flush=True); p.kill(); rc = p.wait(); ok = False
    for c, ext in (('mashed_re.log', '.log'), ('player_trace.log', '.ptrace.log'), ('motion_diag.log', '.motion_diag.log'), ('a5g_diag.log', '.a5g.log')):
        src = os.path.join(ROOT, c)
        if os.path.exists(src): shutil.copy2(src, os.path.join(OUT, name + ext))
    print(f'    exit={rc} self_exited={ok} secs={time.time() - t0:.0f}', flush=True)
    shas = []
    for f in files + [os.path.join(OUT, name + '.ptrace.log'), os.path.join(OUT, name + '.motion_diag.log')]:
        if os.path.exists(f):
            n = sum(1 for _ in open(f, 'rb'))
            print(f'    {os.path.basename(f)} lines={n} sha8={sha8(f)}', flush=True)
            shas.append(f'{os.path.basename(f)}:{n}:{sha8(f)}')
    ks = ' '.join(f'{k}={v}' for k, v in sorted(env.items()) if k.startswith('MASHED_') and k != 'MASHED_ROOT')
    with open(os.path.join(OUT, 'RUNLOG.txt'), 'a') as lf:
        lf.write(f'{time.strftime("%Y-%m-%d %H:%M:%S")} {name} exe={sha8(exe)} pid={p.pid} exit={rc} '
                 f'self_exited={ok} secs={time.time() - t0:.0f} | {ks} | {" ".join(shas)}\n')


def exe_for(tag):
    return EXE_NEW


STACK['FL'] = ['MASHED_A5_FD0_LIVE=1']   # u9772 PREREG: A5 / 442ce0 / 442c80 read the live DAT_007f0fd0
STACK['FULLGOF'] = STACK['FULLGO'] + STACK['FL']
for _s in (2, 3): STACK['TLG%dF' % _s] = STACK['TLG%d' % _s] + STACK['FL']
STACK['FLD'] = STACK['FL'] + ['MASHED_A5_DRAFT_LUT=1']   # + u9771's float/LUT drafting test
STACK['FULLGOFD'] = STACK['FULLGO'] + STACK['FLD']
for _s in (2, 3): STACK['TLG%dFD' % _s] = STACK['TLG%d' % _s] + STACK['FLD']


STACK['RD'] = ['MASHED_RECDUMP=@OUT@/@NAME@.rec.msd', 'MASHED_RECDUMP_SLOT=0']   # d2off: the player's record per race frame
STACK['RDS'] = STACK['RD'] + ['MASHED_SPAWN_Y_LOG=@OUT@/@NAME@.sy.txt', 'MASHED_SETTLE_DIAG=@OUT@/@NAME@.settle.txt']
STACK['FAITH'] = [k for k in STACK['FULLGO'] if not k.startswith(('MASHED_RECDUMP', 'MASHED_MODE7_TRACE'))] + ['MASHED_A5_FD0_LIVE=1', 'MASHED_A5_DRAFT_LUT=1', 'MASHED_FIX_5CEA44']   # d2off PREREG: the full faithful stack
STACK['FD'] = STACK['FAITH'] + STACK['RD']
STACK['FNPM'] = [k for k in STACK['FAITH'] if k != 'MASHED_POS_MODEL']   # FAITH without the position model
STACK['PM'] = ['MASHED_POS_MODEL']
STACK['FAITH2'] = STACK['FAITH'] + ['MASHED_COLL_LUT=1', 'MASHED_A5_RING_ORDER=1']   # amendment 1
STACK['FD2'] = STACK['FAITH2'] + STACK['RD']
STACK['FRO'] = STACK['FAITH'] + ['MASHED_A5_RING_ORDER=1']
STACK['FCL'] = STACK['FAITH'] + ['MASHED_COLL_LUT=1']
STACK['FNF'] = [k for k in STACK['FAITH'] if not k.startswith(('MASHED_A5_FD0_LIVE', 'MASHED_A5_DRAFT_LUT'))]   # amendment 2


STACK['NF'] = ['MASHED_NO_FAITH=1']   # faithflip: the master opt-out = the pre-flip default
STACK['NFRD'] = STACK['NF'] + STACK['RD']
STACK['FLRD'] = STACK['RD']   # the new default + the player record dump


STACK['CL'] = ['MASHED_COLL_LUT=1']   # u9774: the new default + the collision LUT leaves
STACK['CLT'] = STACK['CL'] + STACK['RD'] + ['MASHED_A5GDIAG', 'MASHED_FORCE_SPLIT_SLOT=0', 'MASHED_A6A_TAILLOG=@OUT@/@NAME@.tail.csv',
    'MASHED_A6A_VZLOG=@OUT@/@NAME@.vz.csv', 'MASHED_FORCE_SPLIT=@OUT@/@NAME@.fs.csv']   # + player record dump and the A5 / A6a logs on slot 0
for _w in (1, 2, 3): STACK['CLTW%d' % _w] = STACK['CLT'] + ['MASHED_FORCE_SPLIT_WHEEL=%d' % _w]
STACK['SO'] = ['MASHED_A6A_SPIN_ORDER=1']   # amendment 1
STACK['CLS'] = STACK['CL'] + STACK['SO']
STACK['CLST'] = STACK['CLT'] + STACK['SO']
STACK['FSO'] = ['MASHED_FIXUP_SLIDE_ORDER=1']   # amendment 2
STACK['SF'] = STACK['SO'] + STACK['FSO']
STACK['CLSF'] = STACK['CL'] + STACK['SF']
STACK['CLSFT'] = STACK['CLST'] + STACK['FSO']
STACK['CLSFTL'] = STACK['CLSFT'] + ['MASHED_FIXUP_LOG=@OUT@/@NAME@.fix.txt']
STACK['F3'] = ['MASHED_TER_SPIN_F32=1', 'MASHED_FIXUP_F32=1']   # amendment 3
STACK['CLSF3'] = STACK['CLSF'] + STACK['F3']
STACK['CLSF3TL'] = STACK['CLSFTL'] + STACK['F3']
STACK['F4'] = STACK['F3'] + ['MASHED_FIXUP_SUM_ORDER=1']   # amendment 4
STACK['CLSF4'] = STACK['CLSF'] + STACK['F4']
STACK['CLSF4TL'] = STACK['CLSFTL'] + STACK['F4']
STACK['F5'] = STACK['SF'] + STACK['F4']   # the five knobs without COLL_LUT
for _w in (1, 2, 3): STACK['CLSTW%d' % _w] = STACK['CLST'] + ['MASHED_FORCE_SPLIT_WHEEL=%d' % _w]


STACK['N5'] = ['MASHED_NO_A6A_SPIN_ORDER=1', 'MASHED_NO_TER_SPIN_F32=1', 'MASHED_NO_FIXUP_F32=1', 'MASHED_NO_FIXUP_SUM_ORDER=1',
               'MASHED_NO_FIXUP_SLIDE_ORDER=1']   # f5flip: the five opt-outs = the faithflip default


STACK['NT'] = ['MASHED_NO_WCS_TRINORMAL=1']   # wcsflip: the opt-out = the f5flip default
# u9776: car records + the alignment log (tri-probe twin) on the trinflip default
STACK['TC'] = ['MASHED_WCS_COMBINE_PC24=1']; STACK['TM'] = ['MASHED_MATMUL_PC24=1']
STACK['LT3'] = STACK['TC'] + ['MASHED_RECDUMP=@OUT@\@NAME@.rec.msd', 'MASHED_RECDUMP_SLOT=3', 'MASHED_LT_LOG=@OUT@\@NAME@.lt.csv', 'MASHED_WCS_TRILOG=@OUT@\@NAME@.tri.txt']
STACK['LR3'] = ['MASHED_RECDUMP=@OUT@\@NAME@.rec.msd', 'MASHED_RECDUMP_SLOT=3', 'MASHED_LT_LOG=@OUT@\@NAME@.lt.csv', 'MASHED_WCS_TRILOG=@OUT@\@NAME@.tri.txt']
STACK['TC0'] = STACK['TC'] + ['MASHED_RECDUMP=@OUT@\@NAME@.rec.msd', 'MASHED_RECDUMP_SLOT=0']
STACK['TC0RO'] = STACK['TC0'] + ['MASHED_A5_RING_ORDER=1']
STACK['BQ0'] = STACK['TC0'] + ['MASHED_BATCH_DUMP=@OUT@/@NAME@.batch.csv', 'MASHED_BATCH_DUMP_SLOT=0']
STACK['FR'] = STACK['TC'] + ['MASHED_FI_RAND=1']
for _c in (0, 1, 2, 3): STACK[f'FR{_c}'] = STACK['FR'] + [f'MASHED_RECDUMP=@OUT@\@NAME@.rec.msd', f'MASHED_RECDUMP_SLOT={_c}']
STACK['LT1'] = STACK['TC'] + ['MASHED_RECDUMP=@OUT@\@NAME@.rec.msd', 'MASHED_RECDUMP_SLOT=1', 'MASHED_LT_LOG=@OUT@\@NAME@.lt.csv']
for _c in (1, 2, 3):
    STACK[f'TC{_c}'] = STACK['TC'] + [f'MASHED_RECDUMP=@OUT@\@NAME@.rec.msd', f'MASHED_RECDUMP_SLOT={_c}', 'MASHED_WCS_TRILOG=@OUT@\@NAME@.tri.txt']
    STACK[f'TM{_c}'] = STACK['TM'] + [f'MASHED_RECDUMP=@OUT@\@NAME@.rec.msd', f'MASHED_RECDUMP_SLOT={_c}', 'MASHED_WCS_TRILOG=@OUT@\@NAME@.tri.txt']
    STACK[f'TR{_c}'] = [f'MASHED_RECDUMP=@OUT@\@NAME@.rec.msd', f'MASHED_RECDUMP_SLOT={_c}', 'MASHED_WCS_TRILOG=@OUT@\@NAME@.tri.txt']


# u9777: the player (slot 0) on the cpcflip default: record, classifier batch, A5 diag, A6a tail / vz / force split, solver poslog
STACK['P0'] = STACK['RD'] + ['MASHED_BATCH_DUMP=@OUT@/@NAME@.batch.csv', 'MASHED_BATCH_DUMP_SLOT=0', 'MASHED_A5GDIAG',
    'MASHED_FORCE_SPLIT_SLOT=0', 'MASHED_A6A_TAILLOG=@OUT@/@NAME@.tail.csv', 'MASHED_A6A_VZLOG=@OUT@/@NAME@.vz.csv',
    'MASHED_FORCE_SPLIT=@OUT@/@NAME@.fs.csv', 'MASHED_WCS_POSLOG=@OUT@/@NAME@.pos.txt']
STACK['P0S'] = STACK['P0'] + ['MASHED_D2SINK=@OUT@/@NAME@.sink.txt']   # + the slot-0 velocity at every A4 / substep stage
STACK['RL'] = ['MASHED_A5_RUBBER_LIVE=1']   # PREREG_u9777: A5's rubber-band leaves read the live refdist / reference car
STACK['P0SR'] = STACK['P0S'] + STACK['RL']
STACK['RLTR'] = STACK['RL'] + ['MASHED_REFDIST_TRACE=@OUT@/@NAME@.rd.tsv']
STACK['DEFTR'] = ['MASHED_REFDIST_TRACE=@OUT@/@NAME@.rd.tsv']
for _c in (1, 2, 3): STACK[f'RLR{_c}'] = STACK['RL'] + ['MASHED_RECDUMP=@OUT@/@NAME@.rec.msd', f'MASHED_RECDUMP_SLOT={_c}']

STACK['NR'] = ['MASHED_NO_A5_RUBBER_LIVE=1']   # rbflip: the opt-out = the cpcflip default
# fflip (PREREG_fflip.md): the new default runs base_env = recipe F (rule 0, game mode 10) with MODE_LIVE / SLOT_TYPE_LIVE seeded.
STACK['NOK'] = ['MASHED_NO_MODE_LIVE=1', 'MASHED_NO_SLOT_TYPE_LIVE=1']   # both opt-outs on recipe F (= brecipe F0M)
# r7keep (PREREG_r7keep.md): K = the knob on recipe F (P1); R7 / R7K = rule 7, knob OFF / ON, with the rule-7 trace (P2)
STACK['K'] = ['MASHED_RULE7_KEEP']
STACK['R7'] = ['MASHED_ROUND_RULE=7', 'MASHED_RULE7_TRACE=@OUT@\\@NAME@.r7.txt']
STACK['R7K'] = STACK['R7'] + ['MASHED_RULE7_KEEP']
# flip (2026-10-22): the keep law is default ON; NR7 = the opt-out (= the pre-flip default)
STACK['NR7'] = ['MASHED_NO_RULE7_KEEP']
STACK['R7NR7'] = STACK['R7'] + ['MASHED_NO_RULE7_KEEP']
STACK['OLD'] = ['MASHED_ROUND_RULE=4', '-MASHED_GAME_MODE']               # the old recipe (rule 4, no game mode word)
STACK['OLDNOK'] = STACK['OLD'] + STACK['NOK']                             # old recipe + both opt-outs = the rbflip default
STACK['OLDNF'] = STACK['OLD'] + STACK['NF']                               # old recipe + MASHED_NO_FAITH=1 = the pre-faithflip trio
# soflip: STACK['IL'] / ['ILCC'] (MASHED_SUBSTEP_INTERLEAVE) removed, the fiber form is deleted
# substep (PREREG_substep.md): MASHED_SUBSTEP_OUTER, the phase-function substep-outer / car-inner driver (default OFF)
STACK['SX'] = ['MASHED_SUBSTEP_OUTER=1']                                        # FUN_004709a0 order (all pre-parts first)
STACK['SXC'] = STACK['SX'] + ['MASHED_SUBSTEP_OUTER_ORDER=car']                # mechanism control: per-car order
STACK['SXF'] = STACK['SX'] + ['MASHED_SUBSTEP_OUTER_ORDER=fiber']              # mechanism control: the fiber's yield order
STACK['SXCC'] = STACK['SX'] + ['MASHED_CARCAR_RECDUMP=@OUT@/@NAME@.ccrec.tsv']  # + the car-car probe twin (ccdiff.py)
# soflip (PREREG_soflip.md): MASHED_SUBSTEP_OUTER is seeded; NOSX = its opt-out (= the fflip default)
STACK['NOSX'] = ['MASHED_NO_SUBSTEP_OUTER=1']
STACK['NOSXNF'] = STACK['NOSX'] + STACK['NF']
# dcphys (PREREG_dcphys.md section 4): A6a's three 3-term dots in the original's association
STACK['DOT'] = ['MASHED_A6A_DOT_ORDER']
STACK['NODOT'] = ['MASHED_NO_A6A_DOT_ORDER']   # PREREG amendment 1: the opt-out after the flip
# d3close (PREREG_d3close.md): bisect opt-outs for (e) / (b) if a gate moves
STACK['NOPC24'] = ['MASHED_NO_WCS_COMBINE_PC24=1']
STACK['SR'] = ['MASHED_SPEED_REFRESH=1']   # report only: (e) sensitivity to the +0x9e4 refresh
# dcphys2 (PREREG_dcphys2.md section 4): the A6b / A5 / A4 shared-body knobs (all default OFF in the exe)
STACK['P2'] = ['MASHED_A6B_ACOS_CRT', 'MASHED_A6B_NORM_LUT', 'MASHED_A5_RING_ORDER', 'MASHED_A5_DOT_ORDER', 'MASHED_A5_AIR_F32',
               'MASHED_A5_CONTACT_INT', 'MASHED_A5_SPIN_FILD', 'MASHED_A4_DOT_ORDER']
STACK['NOP2'] = [k.replace('MASHED_', 'MASHED_NO_', 1) for k in STACK['P2']]   # PREREG amendment 1: the opt-out after the flip

for name in sys.argv[1:]:
    CUR['name'] = name
    if name.startswith('H_'):
        tag = tag_of(name[2:])
        e = base_env(14400); e['MASHED_PLAYERTRACE'] = '1'
        step = os.path.join(OUT, name + '.step.csv'); gates = os.path.join(OUT, name + '.gates.csv')
        e['MASHED_AI_STEPDUMP'] = step; e['MASHED_U9186_GATES'] = gates
        knobs(e, STACK[tag])
        run(name, e, [step, gates], 900, exe_for(tag))
    elif name.startswith('B_'):
        stack, k = name[2:].rsplit('_K', 1); k = k.split('-', 1)[0]
        e = base_env(1200); e['MASHED_RING_SKIP'] = str(int(k))
        step = os.path.join(OUT, name + '.step.csv'); e['MASHED_AI_STEPDUMP'] = step
        knobs(e, STACK[stack])
        run(name, e, [step], 600, exe_for(stack))
    elif name.startswith('D2_') or name.startswith('P_'):
        tag = name.split('_', 1)[1].rsplit('_', 1)[0]
        e = clean_env(); e.update(D2BASE); e['MASHED_TITLE'] = 'substep-' + name
        knobs(e, STACK[tag])
        files = []
        if name.startswith('P_'):
            e['MASHED_BATCH_DUMP'] = os.path.join(OUT, name + '.batch.csv'); files.append(e['MASHED_BATCH_DUMP'])
        run(name, e, files, 0, exe_for(tag), wall=90)
    elif name.startswith('W_') or name.startswith('WL_'):
        tag = tag_of(name.split('_', 1)[1])
        e = base_env(18000 if name.startswith('WL_') else 3600)
        e['MASHED_TRACK_VIEW'] = 'Warzone'; e['MASHED_ROUND_RULE'] = '8'
        step = os.path.join(OUT, name + '.step.csv'); pt = os.path.join(OUT, name + '.pontoon.txt')
        sk = os.path.join(OUT, name + '.seek.txt')
        e['MASHED_AI_STEPDUMP'] = step; e['MASHED_PONTOON_TRACE'] = pt; e['MASHED_SEEK_TRACE'] = sk
        knobs(e, STACK[tag])
        run(name, e, [step, pt, sk], 2400 if name.startswith('WL_') else 900, exe_for(tag))
    elif name.startswith('LR_'):
        # d3b look replay (PREREG G-REPLAY-LOOK): the original's aistep rows through SplineLookahead, exact and LUT
        e = base_env(1200)
        e['MASHED_AI_LOOK_REPLAY'] = ';'.join(os.path.join(W, 'verify', 'd3_regimes_20261013', f'{r}.msd.aistep.csv') for r in ('o_reg_r1', 'o_reg_r13', 'o_reg_r19'))
        e['MASHED_AI_LOOK_REPLAY_OUT'] = os.path.join(OUT, name + '.look.csv')
        if name.startswith('LR_M'):
            e['MASHED_AI_LOOK_REPLAY_MASK'] = name[4:]   # LR_M5 -> mode 1 runs MASHED_AI_LUT=5
        run(name, e, [e['MASHED_AI_LOOK_REPLAY_OUT']], 300, exe_for('DEF'))
    elif name.startswith('CR_'):
        # u9769: VehicleCarCarContact replay of the original's --carcar-probe rows (A4_CC_IN), knobs from the tag
        tag = tag_of(name[3:])
        e = base_env(120)
        e['MASHED_CC_REPLAY'] = os.environ['A4_CC_IN']; e['MASHED_CC_REPLAY_OUT'] = os.path.join(OUT, name + '.ccreplay.tsv')
        knobs(e, STACK[tag])
        run(name, e, [e['MASHED_CC_REPLAY_OUT']], 300, exe_for(tag))
    elif name.startswith('R_'):
        tag = tag_of(name[2:])
        e = base_env(120)
        e['MASHED_BSP_REPLAY'] = os.environ['D2B_REPLAY_IN']
        e['MASHED_BSP_REPLAY_OUT'] = os.path.join(OUT, name + '.replay.csv')
        knobs(e, STACK[tag])
        run(name, e, [e['MASHED_BSP_REPLAY_OUT']], 300, exe_for(tag))
    else:
        print('unknown arm', name)
print('DONE')
