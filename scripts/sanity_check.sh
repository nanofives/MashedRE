#!/bin/bash
# sanity_check.sh — run all session-startup checks in one command.
# Use this at the top of every Claude session, and as a smoke test after
# infrastructure changes.
set -uo pipefail

ROOT="C:/Users/maria/Desktop/Proyectos/Mashed"
EXPECT_MASHED_SHA="BDCAE093A30FBF226BDD852B9C36798A987AEE33B3AE82BF7404B0336EFD3C0E"
EXPECT_MASHED_SIZE=2846720
EXPECT_LAUNCH_SHA="694AA949B86AE26E5F1496A48383D1027244752A468BE7F678C54770B973EF79"
EXPECT_LAUNCH_SIZE=978944

ok=0
fail=0

pass()  { echo "  [PASS] $1"; ok=$((ok+1)); }
warn()  { echo "  [WARN] $1"; }
fail()  { echo "  [FAIL] $1"; fail=$((fail+1)); }

echo "== Mashed RE sanity check =="

# 1. Binary anchors
echo "-- Binary anchors --"
if [[ -f "$ROOT/original/MASHED.exe" ]]; then
    actual_sha=$(certutil -hashfile "$ROOT/original/MASHED.exe" SHA256 2>/dev/null | sed -n '2p' | tr -d '\r ' | tr 'a-f' 'A-F')
    actual_size=$(stat -c '%s' "$ROOT/original/MASHED.exe" 2>/dev/null || stat -f '%z' "$ROOT/original/MASHED.exe")
    if [[ "$actual_sha" == "$EXPECT_MASHED_SHA" && "$actual_size" == "$EXPECT_MASHED_SIZE" ]]; then
        pass "MASHED.exe anchor matches"
    else
        fail "MASHED.exe anchor MISMATCH"
        echo "         expected sha=$EXPECT_MASHED_SHA size=$EXPECT_MASHED_SIZE"
        echo "         actual   sha=$actual_sha size=$actual_size"
    fi
else
    fail "original/MASHED.exe missing"
fi

if [[ -f "$ROOT/original/launch.exe" ]]; then
    actual_sha=$(certutil -hashfile "$ROOT/original/launch.exe" SHA256 2>/dev/null | sed -n '2p' | tr -d '\r ' | tr 'a-f' 'A-F')
    actual_size=$(stat -c '%s' "$ROOT/original/launch.exe" 2>/dev/null || stat -f '%z' "$ROOT/original/launch.exe")
    if [[ "$actual_sha" == "$EXPECT_LAUNCH_SHA" && "$actual_size" == "$EXPECT_LAUNCH_SIZE" ]]; then
        pass "launch.exe anchor matches"
    else
        fail "launch.exe anchor MISMATCH"
    fi
else
    fail "original/launch.exe missing"
fi

# 2. Ghidra master
echo "-- Ghidra master --"
if [[ -f "$ROOT/Mashed.gpr" && -d "$ROOT/Mashed.rep" ]]; then
    pass "Mashed.gpr / Mashed.rep present"
else
    fail "Mashed.gpr or Mashed.rep missing — run analyzeHeadless"
fi

# 3. Pool dir
echo "-- Ghidra pool --"
if bash "$ROOT/scripts/ghidra_pool.sh" status >/dev/null 2>&1; then
    pass "pool script runs"
else
    fail "pool script failed"
fi

stale_locks=$(find "$ROOT/mashed_pool" "$ROOT" -maxdepth 1 -name '*.lock' -o -name '*.lock~' 2>/dev/null | wc -l)
if [[ $stale_locks -gt 0 ]]; then
    warn "$stale_locks lock file(s) present — consider 'ghidra_pool.sh cleanup' if no session is active"
fi

# 4. Trackers parseable
echo "-- Trackers --"
for f in hooks.csv STUBS.md UNCERTAINTIES.md DEFERRED.md; do
    if [[ -f "$ROOT/$f" ]]; then
        pass "$f present"
    else
        fail "$f missing"
    fi
done

# 5. MCP servers reachable (smoke import only — actual MCP loads happen in Claude)
echo "-- Python MCP modules --"
if py -3.12 -c "import frida_game_hacking_mcp" 2>/dev/null; then
    pass "frida-game-hacking-mcp importable"
else
    fail "frida-game-hacking-mcp not importable (pip install -e tools/frida-game-hacking-mcp)"
fi

if [[ -f "C:/Users/maria/Desktop/Proyectos/TD5RE/ghidra-headless-mcp/ghidra_headless_mcp.py" ]]; then
    pass "ghidra-headless-mcp script present"
else
    fail "ghidra-headless-mcp script missing in TD5RE share"
fi

# 6. Git identity
echo "-- Git --"
who=$(git -C "$ROOT" config user.name 2>/dev/null)
mail=$(git -C "$ROOT" config user.email 2>/dev/null)
if [[ "$who" == "nanofives" && "$mail" == "nanofives@gmail.com" ]]; then
    pass "git identity = nanofives"
else
    fail "git identity wrong: $who <$mail>"
fi

remote=$(git -C "$ROOT" remote get-url origin 2>/dev/null)
if [[ "$remote" == *"nanofives/MashedRE"* ]]; then
    pass "remote points to nanofives/MashedRE"
else
    fail "remote wrong: $remote"
fi

# Summary
echo "----"
echo "PASS=$ok  FAIL=$fail"
[[ $fail -eq 0 ]]
