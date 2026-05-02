#!/bin/bash
# ghidra_pool.sh — Manage a pool of Ghidra project clones for parallel sessions.
#
# Ghidra locks .gpr files at the project level, so multiple Claude Code sessions
# cannot share a single project. This script maintains N clones of the master
# Mashed.gpr/Mashed.rep so each session can acquire its own slot. Clones are
# created ON DEMAND — not pre-allocated — to save disk.
#
# Usage:
#   ghidra_pool.sh init              — One-time setup of pool dir; verify master exists
#   ghidra_pool.sh acquire           — Print first available slot name; create if needed
#   ghidra_pool.sh release [N]       — Clear lock files for slot N (or all if omitted)
#   ghidra_pool.sh sync              — Refresh all unlocked slots from master
#   ghidra_pool.sh status            — Show pool state
#   ghidra_pool.sh cleanup           — Remove stale lock files everywhere
#   ghidra_pool.sh add [N]           — Force-create N additional slots
#   ghidra_pool.sh remove <N>        — Drop slot N (only if unlocked)

set -euo pipefail

PROJECT_ROOT="C:/Users/maria/Desktop/Proyectos/Mashed"
MASTER_GPR="$PROJECT_ROOT/Mashed.gpr"
MASTER_REP="$PROJECT_ROOT/Mashed.rep"
HEADLESS_GPR="$PROJECT_ROOT/Mashed_headless.gpr"
POOL_DIR="$PROJECT_ROOT/mashed_pool"
MAX_SLOTS=16

slot_gpr()  { echo "$POOL_DIR/Mashed_pool${1}.gpr"; }
slot_rep()  { echo "$POOL_DIR/Mashed_pool${1}.rep"; }
slot_lock() { echo "$POOL_DIR/Mashed_pool${1}.lock"; }

is_locked() { [[ -f "$(slot_lock "$1")" ]]; }

next_free_index() {
    for i in $(seq 0 $((MAX_SLOTS - 1))); do
        if [[ ! -f "$(slot_gpr "$i")" ]]; then
            echo "$i"; return 0
        fi
    done
    return 1
}

create_slot() {
    local i="$1"
    local gpr="$(slot_gpr "$i")"
    local rep="$(slot_rep "$i")"
    if [[ ! -f "$MASTER_GPR" || ! -d "$MASTER_REP" ]]; then
        echo "ERROR: Master $MASTER_GPR / $MASTER_REP not found. Run Ghidra GUI once and import MASHED.exe into a project named 'Mashed', then retry." >&2
        exit 1
    fi
    echo "Creating slot $i (clone of master $MASTER_REP)..." >&2
    cp "$MASTER_GPR" "$gpr"
    [[ -d "$rep" ]] && rm -rf "$rep"
    cp -r "$MASTER_REP" "$rep"
    rm -f "$(slot_lock "$i")" "$(slot_lock "$i")~"
}

cmd_init() {
    mkdir -p "$POOL_DIR"
    if [[ ! -f "$MASTER_GPR" ]]; then
        echo "WARNING: Master $MASTER_GPR not found yet."
        echo "         Open Ghidra GUI, create project 'Mashed' at $PROJECT_ROOT,"
        echo "         import MASHED.exe, run auto-analysis, then save & exit."
        echo "         (Optionally also create 'Mashed_headless' for the headless MCP backend.)"
        exit 0
    fi
    echo "Pool dir ready: $POOL_DIR"
    echo "Master:        $MASTER_GPR"
    [[ -f "$HEADLESS_GPR" ]] && echo "Headless:      $HEADLESS_GPR" || echo "Headless:      not created (optional)"
    cmd_status
}

cmd_acquire() {
    [[ ! -d "$POOL_DIR" ]] && { mkdir -p "$POOL_DIR"; }
    for gpr in "$POOL_DIR"/Mashed_pool*.gpr; do
        [[ -f "$gpr" ]] || continue
        local slot
        slot=$(echo "$gpr" | sed 's/.*pool\([0-9]*\).*/\1/')
        if ! is_locked "$slot"; then echo "Mashed_pool${slot}"; exit 0; fi
    done
    local idx
    if ! idx=$(next_free_index); then
        echo "ERROR: All $MAX_SLOTS slots exist and are LOCKED. Run release/cleanup or raise MAX_SLOTS." >&2
        exit 1
    fi
    create_slot "$idx"
    echo "Mashed_pool${idx}"
}

cmd_release() {
    if [[ -n "${1:-}" ]]; then
        rm -f "$(slot_lock "$1")" "$(slot_lock "$1")~"
        echo "Released slot $1"
    else
        rm -f "$POOL_DIR"/*.lock "$POOL_DIR"/*.lock~ 2>/dev/null || true
        echo "Released all pool slots"
    fi
}

cmd_sync() {
    [[ ! -d "$POOL_DIR" ]] && { echo "ERROR: pool not initialized" >&2; exit 1; }
    echo "Syncing pool slots from master..."
    # Stamp BEFORE the cp loop so any slot refreshed below has mtime > .last_master_sync.
    # Locked slots keep their pre-sync mtime, which marks them stale to ghidra_assert.sh.
    : > "$PROJECT_ROOT/.last_master_sync"
    local synced=0 skipped=0
    for gpr in "$POOL_DIR"/Mashed_pool*.gpr; do
        [[ -f "$gpr" ]] || continue
        local slot
        slot=$(echo "$gpr" | sed 's/.*pool\([0-9]*\).*/\1/')
        local rep="$(slot_rep "$slot")"
        if is_locked "$slot"; then
            echo "  Slot $slot: LOCKED (skipped)"; skipped=$((skipped+1))
        else
            echo "  Slot $slot: refreshing..."
            rm -rf "$rep"; cp -r "$MASTER_REP" "$rep"; synced=$((synced+1))
        fi
    done
    rm -f "$PROJECT_ROOT/Mashed.lock" "$PROJECT_ROOT/Mashed.lock~"
    rm -f "$PROJECT_ROOT/Mashed_headless.lock" "$PROJECT_ROOT/Mashed_headless.lock~"
    echo "Sync done: $synced refreshed, $skipped skipped"
}

cmd_status() {
    if [[ ! -d "$POOL_DIR" ]]; then echo "Pool not initialized."; exit 0; fi
    echo "Mashed Ghidra Pool"
    echo "  Master: $MASTER_GPR"
    echo "  Pool:   $POOL_DIR"
    echo "  Max:    $MAX_SLOTS slots (on-demand)"
    local n=0
    for gpr in "$POOL_DIR"/Mashed_pool*.gpr; do
        [[ -f "$gpr" ]] || continue
        local slot
        slot=$(echo "$gpr" | sed 's/.*pool\([0-9]*\).*/\1/')
        if is_locked "$slot"; then echo "  Slot $slot: LOCKED"; else echo "  Slot $slot: available"; fi
        ((n++))
    done
    [[ $n -eq 0 ]] && echo "  (no slots created yet — first acquire will create slot 0)"
    echo ""
    [[ -f "$PROJECT_ROOT/Mashed.lock" ]] && echo "  Master Mashed.gpr: LOCKED" || echo "  Master Mashed.gpr: available"
    [[ -f "$PROJECT_ROOT/Mashed_headless.lock" ]] && echo "  Mashed_headless.gpr: LOCKED" || echo "  Mashed_headless.gpr: available"
}

cmd_cleanup() {
    echo "Cleaning all stale Ghidra lock files..."
    rm -f "$POOL_DIR"/*.lock "$POOL_DIR"/*.lock~ 2>/dev/null || true
    rm -f "$PROJECT_ROOT/Mashed.lock" "$PROJECT_ROOT/Mashed.lock~"
    rm -f "$PROJECT_ROOT/Mashed_headless.lock" "$PROJECT_ROOT/Mashed_headless.lock~"
    echo "Done."
}

cmd_add() {
    local n="${1:-1}"
    for ((k=0; k<n; k+=1)); do
        local idx
        if ! idx=$(next_free_index); then
            echo "ERROR: pool full ($MAX_SLOTS)" >&2; exit 1
        fi
        create_slot "$idx"
    done
    cmd_status
}

cmd_remove() {
    local i="${1:-}"
    [[ -z "$i" ]] && { echo "Usage: ghidra_pool.sh remove <N>" >&2; exit 1; }
    if is_locked "$i"; then echo "ERROR: slot $i is LOCKED" >&2; exit 1; fi
    rm -f "$(slot_gpr "$i")"
    rm -rf "$(slot_rep "$i")"
    echo "Removed slot $i"
}

case "${1:-status}" in
    init)    cmd_init ;;
    acquire) cmd_acquire ;;
    release) cmd_release "${2:-}" ;;
    sync)    cmd_sync ;;
    status)  cmd_status ;;
    cleanup) cmd_cleanup ;;
    add)     cmd_add "${2:-}" ;;
    remove)  cmd_remove "${2:-}" ;;
    *)
        echo "Usage: ghidra_pool.sh {init|acquire|release|sync|status|cleanup|add|remove}"
        exit 1 ;;
esac
