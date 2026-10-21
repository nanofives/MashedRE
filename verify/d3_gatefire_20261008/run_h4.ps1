# H4 driver: PREREG_H4.md section 1. Five arms on the 18:44 build:
#   A / A2 = default (determinism floor), B = SLOT_PLAYER,
#   C = full branch-2 arm, D = C minus SLOT_PLAYER.
# Every arm: PLAYERTRACE (car 0) + U9186_GATES (reader; GF1 unset) + AI_STEPDUMP (cars 1..3).
#
# MEASUREMENT ONLY. Spawns and stops ONLY its own PIDs.
$ErrorActionPreference = 'Stop'
$root = 'C:\Users\maria\Desktop\Proyectos\Mashed'
$out  = Join-Path $root 'verify\d3_gatefire_20261008'
$exe  = Join-Path $root 'mashedmod\build\mashed_re.exe'
Set-Location $root

$FRAMES     = 14400
$WAIT_LIMIT = 900

$LANE = @('MASHED_REFDIST','MASHED_SLOTSTATE_SEED','MASHED_SLOT_PLAYER',
          'MASHED_A364_RESET','MASHED_RACEPCT_BRIDGE','MASHED_NO_ELIM',
          'MASHED_WIRE_B2','MASHED_U9186_GATES','MASHED_AI_STEPDUMP',
          'MASHED_NO_SLOTSTATE_SEED','MASHED_NO_SLOT_PLAYER','MASHED_GF1',
          'MASHED_PLAYERTRACE','MASHED_PLAYERTRACE_OUT','MASHED_SUBSTATE','MASHED_MODE5_RESET','MASHED_SUBSTATE_CD',
          'MASHED_L1B_INPUTS','MASHED_P3_MEASURE','MASHED_LEG2_TRACE','MASHED_CAM_TICKS_REPLAY','MASHED_NO_WIRE_B2','MASHED_NO_RACEPCT_BRIDGE','MASHED_NO_A364_RESET','MASHED_NO_REFDIST','MASHED_PATHPROG','MASHED_PATHPROG_TRACE','MASHED_PATHPROG_PC','MASHED_TGT3','MASHED_TGT3_TRACE','MASHED_TGT3_PC','MASHED_REGISTRY','MASHED_REGISTRY_PC','MASHED_PICKUPS_ORIG','MASHED_PICKUPS_PC','MASHED_REGISTRY_TRACE','MASHED_REG_CARS','MASHED_REG_CALLBACKS','MASHED_PKCB_TRACE','MASHED_PU_STEPDUMP','MASHED_MODE7','MASHED_MODE7_TRACE','MASHED_MODE7_PC','MASHED_AILEAF','MASHED_AILEAF_PC','MASHED_AILEAF_TRACE','MASHED_AILEAF_SEED4C0','MASHED_AILEAF_LIVEROWS','MASHED_ROUND_GRID','MASHED_ROUND_GRID_TRACE','MASHED_ROUND_GRID_CHECK','MASHED_ROUND_GRID_REPLAY','MASHED_ROUND_GRID_REPLAY_OUT','MASHED_ROUND_GRID_ORDER','MASHED_ROUND_GRID_ORDER_REPLAY','MASHED_ROUND_GRID_ORDER_REPLAY_OUT')

$B2 = @('MASHED_A364_RESET','MASHED_REFDIST','MASHED_RACEPCT_BRIDGE','MASHED_NO_ELIM','MASHED_WIRE_B2')
$runs = @(
  # D3 bulk G-AILEAF (verify/d3_bulk_20261011/PLAN.md section 4): AL_CTRL = G-CTRL knob OFF (must equal ae67b13a/c8317ce2/31db9384);
  # AL_ON = knob ON with the trace, Phase 2 only (PREREG first)
  # round-restart grid Stage A (verify/d3_bulk_20261011/roundgrid/FIX_BRIEF.md): RG_CTRL = G-CTRL knob OFF; RG_PP / RG_ON / RG_ON2 = Phase 2 (PREREG first)
  @{ name = 'RG_CTRL'; knobs = @() },
  @{ name = 'RG_PP';   knobs = @('MASHED_PATHPROG') },
  @{ name = 'RG_ON';   knobs = @('MASHED_PATHPROG', 'MASHED_ROUND_GRID', 'MASHED_ROUND_GRID_TRACE=@OUT@\RG_ON.roundgrid.txt') },
  @{ name = 'RG_ON2';  knobs = @('MASHED_PATHPROG', 'MASHED_ROUND_GRID', 'MASHED_ROUND_GRID_TRACE=@OUT@\RG_ON2.roundgrid.txt') },
  # round grid Stage B (verify/d3_bulk_20261011/roundgrid/stageb/PREREG_STAGE_B.md): knob-ON arms, PREREG first
  @{ name = 'RG_B';    knobs = @('MASHED_PATHPROG', 'MASHED_ROUND_GRID', 'MASHED_ROUND_GRID_ORDER', 'MASHED_ROUND_GRID_TRACE=@OUT@\RG_B.roundgrid.txt') },
  @{ name = 'RG_B2';   knobs = @('MASHED_PATHPROG', 'MASHED_ROUND_GRID', 'MASHED_ROUND_GRID_ORDER', 'MASHED_ROUND_GRID_TRACE=@OUT@\RG_B2.roundgrid.txt') },
  @{ name = 'AL_CTRL'; knobs = @() },
  @{ name = 'AL_PP';   knobs = @('MASHED_PATHPROG') },   # base arm for AL_ON (knob stacked on PATHPROG)
  @{ name = 'AL_ON';   knobs = @('MASHED_PATHPROG', 'MASHED_AILEAF', 'MASHED_AILEAF_TRACE=@OUT@\AL_ON.aileaf.tsv') },
  @{ name = 'AL_ON2';  knobs = @('MASHED_PATHPROG', 'MASHED_AILEAF', 'MASHED_AILEAF_TRACE=@OUT@\AL_ON2.aileaf.tsv') },   # same-arm repeat (collateral floor)
  @{ name = 'AL_ON53'; knobs = @('MASHED_PATHPROG', 'MASHED_AILEAF', 'MASHED_AILEAF_PC=53', 'MASHED_AILEAF_TRACE=@OUT@\AL_ON53.aileaf.tsv') },   # G-AILEAF-PC
  @{ name = 'AL_SEED'; knobs = @('MASHED_PATHPROG', 'MASHED_AILEAF', 'MASHED_AILEAF_SEED4C0=2', 'MASHED_AILEAF_TRACE=@OUT@\AL_SEED.aileaf.tsv') },   # diagnostic: AI factor seeded to the original's 2
  @{ name = 'AL_LIVE'; knobs = @('MASHED_PATHPROG', 'MASHED_AILEAF', 'MASHED_AILEAF_TRACE=@OUT@\AL_LIVE.aileaf.tsv', 'MASHED_AILEAF_LIVEROWS=@OUT@\AL_LIVE.liverows.tsv') },   # G-AILEAF-LIVE input capture
  @{ name = 'H4_A';  knobs = @() },
  @{ name = 'H4_A2'; knobs = @() },
  @{ name = 'H4_B';  knobs = @('MASHED_SLOT_PLAYER') },
  @{ name = 'H4_C';  knobs = @('MASHED_SLOT_PLAYER') + $B2 },
  @{ name = 'H4_D';  knobs = $B2 },
  # post-flip (18:56 build, SLOT_PLAYER default-ON): must equal B and A respectively
  @{ name = 'H4_post';   knobs = @() },
  @{ name = 'H4_optout'; knobs = @('MASHED_NO_SLOT_PLAYER') },
  # D-11073 leg 1a (PREREG_LEG1A.md G-INERT): L1A_A = H4_A recipe on the new build (G-CTRL); L1A_SUB = + MASHED_SUBSTATE (G-INERT); both must equal H4_A
  @{ name = 'L1A_A';     knobs = @() },
  @{ name = 'L1A_SUB';   knobs = @('MASHED_SUBSTATE') },
  # D-11073 leg 1b (PREREG_LEG1B.md): G-CTRL after each step, knob OFF; must equal L1A_A
  @{ name = 'L1B_S1';    knobs = @() },
  @{ name = 'L1B_CTRL';  knobs = @() },
  # PREREG_LEG1B.md section 5 knob arms (K=V entries set V; bare K sets 1)
  @{ name = 'L1B_SUB';   knobs = @('MASHED_SUBSTATE', 'MASHED_L1B_INPUTS=@OUT@\L1B_SUB.inputs.csv') },
  @{ name = 'L1B_M5';    knobs = @('MASHED_SUBSTATE', 'MASHED_MODE5_RESET', 'MASHED_L1B_INPUTS=@OUT@\L1B_M5.inputs.csv') },
  @{ name = 'L1B_M5CD';  knobs = @('MASHED_SUBSTATE', 'MASHED_MODE5_RESET', 'MASHED_SUBSTATE_CD=3', 'MASHED_L1B_INPUTS=@OUT@\L1B_M5CD.inputs.csv') },
  # FUN_00414180 port (PREREG_414180.md): CTRL = G-CTRL (knob OFF); P414 = G-EXIT arm; P414_CD3 = isolation arm (report only)
  @{ name = 'P414_CTRL'; knobs = @() },
  @{ name = 'P414';      knobs = @('MASHED_SUBSTATE', 'MASHED_MODE5_RESET', 'MASHED_L1B_INPUTS=@OUT@\P414.inputs.csv') },
  @{ name = 'P414_CD3';  knobs = @('MASHED_SUBSTATE', 'MASHED_MODE5_RESET', 'MASHED_SUBSTATE_CD=3', 'MASHED_L1B_INPUTS=@OUT@\P414_CD3.inputs.csv') },
  # D-11073 leg 2 (PREREG_LEG2.md): CTRL = G-CTRL; MEAS = release held 800 phase-3 frames (target measurement); L2 = gate arm
  @{ name = 'L2_CTRL'; knobs = @() },
  @{ name = 'L2_MEAS'; knobs = @('MASHED_SUBSTATE', 'MASHED_MODE5_RESET', 'MASHED_P3_MEASURE=800', 'MASHED_LEG2_TRACE=@OUT@\L2_MEAS.trace.csv', 'MASHED_L1B_INPUTS=@OUT@\L2_MEAS.inputs.csv') },
  @{ name = 'L2';      knobs = @('MASHED_SUBSTATE', 'MASHED_MODE5_RESET', 'MASHED_LEG2_TRACE=@OUT@\L2.trace.csv', 'MASHED_L1B_INPUTS=@OUT@\L2.inputs.csv') },
  # U-9201 step 2 (PREREG_U9201.md §4): L2 + the original run's QPC tick series replayed into RaceCamera
  @{ name = 'L2_R1'; knobs = @('MASHED_SUBSTATE', 'MASHED_MODE5_RESET', 'MASHED_LEG2_TRACE=@OUT@\L2_R1.trace.csv', 'MASHED_L1B_INPUTS=@OUT@\L2_R1.inputs.csv', 'MASHED_CAM_TICKS_REPLAY=@OUT@\u9201_replay_r1.txt') },
  @{ name = 'L2_R2'; knobs = @('MASHED_SUBSTATE', 'MASHED_MODE5_RESET', 'MASHED_LEG2_TRACE=@OUT@\L2_R2.trace.csv', 'MASHED_L1B_INPUTS=@OUT@\L2_R2.inputs.csv', 'MASHED_CAM_TICKS_REPLAY=@OUT@\u9201_replay_r2.txt') },
  # B2 default flip (verify/d3_b2flip_20261009/PREREG_B2FLIP.md): new default x2, and the opt-out arm (must equal the old default)
  @{ name = 'F_NEW';    knobs = @() },
  @{ name = 'F_NEW2';   knobs = @() },
  @{ name = 'F_OPTOUT'; knobs = @('MASHED_NO_WIRE_B2', 'MASHED_NO_RACEPCT_BRIDGE', 'MASHED_NO_A364_RESET', 'MASHED_NO_REFDIST') },
  # D-11072 P1 (verify/d3_modes37_20261009/PREREG_PATHPROG.md G-P1-OFF): knob OFF, must equal F_NEW (ae67b13a / c8317ce2 / 31db9384)
  @{ name = 'P1_CTRL'; knobs = @() },
  # D-11072 P2 (verify/d3_modes37_20261009/PREREG_P2.md): CTRL = G-P2-OFF; LIVE = knob ON with the trace (report)
  @{ name = 'P2_CTRL'; knobs = @() },
  @{ name = 'P2_LIVE'; knobs = @('MASHED_PATHPROG', 'MASHED_PATHPROG_TRACE=@OUT@\P2_LIVE.pathprog.csv') },
  # D-11072 P3 (verify/d3_modes37_20261009/PREREG_P3.md): CTRL = G-P3-OFF (knob OFF); LIVE = PATHPROG + TGT3 (G-P3-COUNT)
  @{ name = 'P3_CTRL'; knobs = @() },
  @{ name = 'P3_PP';   knobs = @('MASHED_PATHPROG') },   # TGT3 off: must equal P2_LIVE step 4f675870
  @{ name = 'P3_LIVE'; knobs = @('MASHED_PATHPROG', 'MASHED_TGT3', 'MASHED_TGT3_TRACE=@OUT@\P3_LIVE.tgt3.csv') },
  # mode 7 M1 (verify/d3_mode7_20261010/PREREG_MODE7.md): G-M1-OFF (knob OFF) and G-M1-INERT (knob ON, no registrant): both must equal ae67b13a/c8317ce2/31db9384
  @{ name = 'M1_CTRL'; knobs = @() },
  @{ name = 'M1_ON';   knobs = @('MASHED_REGISTRY') },
  # PcScope restore fix (verify/d3_mode7_20261010/cw_check*.txt): the M1_ON / P3_PP / P3_LIVE arms above ran the game thread at PC_24 after the first scoped call. F = rerun on the fixed build.
  @{ name = 'M1F_ON';   knobs = @('MASHED_REGISTRY') },
  @{ name = 'P3F_PP';   knobs = @('MASHED_PATHPROG') },
  @{ name = 'P3F_LIVE'; knobs = @('MASHED_PATHPROG', 'MASHED_TGT3', 'MASHED_TGT3_TRACE=@OUT@\P3F_LIVE.tgt3.csv') },
  # mode 7 M2a (verify/d3_mode7_m2_20261010/PREREG_M2.md): G-M2a-INERT + G-M2a-LIVE (registry trace) -- must equal ae67b13a/c8317ce2/31db9384
  @{ name = 'M2A_CTRL'; knobs = @() },
  @{ name = 'M2A_ON'; knobs = @('MASHED_REGISTRY', 'MASHED_PICKUPS_ORIG', 'MASHED_REGISTRY_TRACE=@OUT@\M2A_ON.registry.csv') },
  # mode 7 M2b (PREREG_M2.md section 5): G-M2b-OFF + G-M2b-INERT/LIVE
  @{ name = 'M2B_CTRL'; knobs = @() },
  @{ name = 'M2B_ON';   knobs = @('MASHED_REGISTRY', 'MASHED_PICKUPS_ORIG', 'MASHED_REG_CARS', 'MASHED_REGISTRY_TRACE=@OUT@\M2B_ON.registry.csv') },
  # mode 7 M3 (verify/d3_mode7_m3_20261010/PREREG_M3.md section 3): G-M3-OFF = M3_CTRL and M3_M2B (trace on, knob off);
  # M3_ON = knob on (G-M3-STANDDOWN / G-M3-LIVE / events / (b) / collateral)
  @{ name = 'M3_CTRL'; knobs = @() },
  @{ name = 'M3_M2B';  knobs = @('MASHED_REGISTRY', 'MASHED_PICKUPS_ORIG', 'MASHED_REG_CARS', 'MASHED_PKCB_TRACE=@OUT@\M3_M2B.cb.tsv') },
  @{ name = 'M3_ON';   knobs = @('MASHED_REGISTRY', 'MASHED_PICKUPS_ORIG', 'MASHED_REG_CARS', 'MASHED_REG_CALLBACKS', 'MASHED_PKCB_TRACE=@OUT@\M3_ON.cb.tsv', 'MASHED_REGISTRY_TRACE=@OUT@\M3_ON.registry.csv') },
  # mode 7 M3b (PREREG_M3.md section 5): respawn seed FUN_00458fa0 in InitReal, not knob-gated
  @{ name = 'M3B_CTRL'; knobs = @() },
  @{ name = 'M3B_M2B';  knobs = @('MASHED_REGISTRY', 'MASHED_PICKUPS_ORIG', 'MASHED_REG_CARS', 'MASHED_PKCB_TRACE=@OUT@\M3B_M2B.cb.tsv') },
  @{ name = 'M3B_ON';   knobs = @('MASHED_REGISTRY', 'MASHED_PICKUPS_ORIG', 'MASHED_REG_CARS', 'MASHED_REG_CALLBACKS', 'MASHED_PKCB_TRACE=@OUT@\M3B_ON.cb.tsv', 'MASHED_REGISTRY_TRACE=@OUT@\M3B_ON.registry.csv') },
  # mode 7 M4 (verify/d3_mode7_m4_20261010/PREREG_M4.md section 2)
  @{ name = 'M4_CTRL'; knobs = @() },
  @{ name = 'M4_M3B';  knobs = @('MASHED_REGISTRY', 'MASHED_PICKUPS_ORIG', 'MASHED_REG_CARS', 'MASHED_REG_CALLBACKS') },
  @{ name = 'M4_ON';   knobs = @('MASHED_REGISTRY', 'MASHED_PICKUPS_ORIG', 'MASHED_REG_CARS', 'MASHED_REG_CALLBACKS', 'MASHED_MODE7', 'MASHED_MODE7_TRACE=@OUT@\M4_ON.c30.tsv', 'MASHED_PKCB_TRACE=@OUT@\M4_ON.cb.tsv') }
)
$sel = @($args)   # capture: inside a Where-Object block $args is the block's own
if ($sel.Count -gt 0) { $runs = $runs | Where-Object { $sel -contains $_.name } }
else { $runs = $runs | Where-Object { $_.name -notlike 'H4_post*' -and $_.name -ne 'H4_optout' -and $_.name -notlike 'L1A_*' -and $_.name -notlike 'L1B_*' -and $_.name -notlike 'P414*' -and $_.name -notlike 'L2*' -and $_.name -notlike 'F_*' -and $_.name -notlike 'P1_*' -and $_.name -notlike 'P2_*' -and $_.name -notlike 'P3_*' -and $_.name -notlike 'M1_*' -and $_.name -notlike 'M1F_*' -and $_.name -notlike 'P3F_*' -and $_.name -notlike 'M2A_*' -and $_.name -notlike 'M2B_*' -and $_.name -notlike 'M3_*' -and $_.name -notlike 'M3B_*' -and $_.name -notlike 'M4_*' -and $_.name -notlike 'AL_*' -and $_.name -notlike 'RG_*' } }

foreach ($r in $runs) {
  $log = Join-Path $root 'mashed_re.log'
  # 2026-10-16: the ptrace goes to a private per-arm path (MASHED_PLAYERTRACE_OUT, e6498ad6).
  # The CWD files are shared by every exe run from the main root, so nothing here deletes them.
  $pt  = Join-Path $out "$($r.name).ptrace.log"
  if (Test-Path $pt) { Remove-Item $pt -Force }
  foreach ($k in $LANE) { Remove-Item "Env:\$k" -ErrorAction SilentlyContinue }

  $env:MASHED_MUTE          = '1'
  $env:MASHED_TRACK_VIEW    = 'Training'
  $env:MASHED_CAR           = '1'
  $env:MASHED_ROUND         = '1'
  $env:MASHED_ROUND_RULE    = '0'   # [fflip 2026-10-21] recipe F: rule 0, game mode 10
  $env:MASHED_GAME_MODE     = '10'
  $env:MASHED_WIN_POS       = 'primary-bl'
  $env:MASHED_DETERMINISTIC = '1'
  $env:MASHED_DET_FRAMES    = "$FRAMES"
  $env:MASHED_PLAYERTRACE   = '1'
  $env:MASHED_PLAYERTRACE_OUT = $pt

  $step  = Join-Path $out "$($r.name).step.csv"
  $gates = Join-Path $out "$($r.name).gates.csv"
  foreach ($f in @($step, $gates)) { if (Test-Path $f) { Remove-Item $f -Force } }
  $env:MASHED_AI_STEPDUMP = $step
  $env:MASHED_U9186_GATES = $gates
  foreach ($k in $r.knobs) {
    if ($k -like '*=*') { $kv = $k.Split('=', 2); Set-Item "Env:\$($kv[0])" ($kv[1].Replace('@OUT@', $out)) }
    else { Set-Item "Env:\$k" '1' }
  }

  Write-Host "=== $($r.name)  knobs=[$($r.knobs -join ',')] ==="
  $p = Start-Process -FilePath $exe -WorkingDirectory $root -PassThru
  Write-Host "    pid=$($p.Id)"
  $exited = $p.WaitForExit($WAIT_LIMIT * 1000)
  if (-not $exited) {
    Write-Host "    TIMEOUT - stopping pid $($p.Id) ONLY"
    Stop-Process -Id $p.Id -Force
    $p.WaitForExit(15000) | Out-Null
  }
  Write-Host "    exit=$($p.ExitCode)  self_exited=$exited"
  if (Test-Path $log) { Copy-Item $log (Join-Path $out "$($r.name).log") -Force }
  foreach ($f in @($step, $gates, (Join-Path $out "$($r.name).ptrace.log"))) {
    if (Test-Path $f) {
      Write-Host ("    {0}  lines={1}  sha8={2}" -f (Split-Path $f -Leaf),
        (Get-Content $f | Measure-Object -Line).Lines,
        (Get-FileHash $f -Algorithm SHA256).Hash.Substring(0,8).ToLower())
    } else { Write-Host "    MISSING $(Split-Path $f -Leaf)" }
  }
}
Write-Host 'H4 DONE'
