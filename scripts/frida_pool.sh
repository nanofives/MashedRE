#!/bin/bash
# frida_pool.sh — Manage a pool of MASHED.exe processes for parallel Frida sessions.
#
# Mirror of ghidra_pool.sh, but each "slot" is a (PID, lockfile) pair instead of
# a project clone. MASHED.exe has no single-instance mutex (confirmed in
# re/analysis/launch_handshake/notes.md), so we can spawn N copies. Each parallel
# diff/verify session acquires a slot index, spawns its own MASHED.exe under that
# slot, runs its Frida harness, and releases the slot (killing the process).
#
# Slots themselves are short-lived: this script just hands out unique indices
# and tracks PIDs for cleanup. There is no "warm process reuse" in v0 — every
# acquire spawns a fresh MASHED.exe. The harness does the spawning, not this
# script; this script just allocates the slot number and records the PID.
#
# Usage:
#   frida_pool.sh acquire           — Print next free slot index (0..MAX-1)
#   frida_pool.sh record <N> <PID>  — Record PID into slot N's tracker file
#   frida_pool.sh release <N>       — Release slot N (kills PID if recorded)
#   frida_pool.sh status            — List slot state
#   frida_pool.sh cleanup           — Kill all tracked PIDs, clear all locks
#   frida_pool.sh killall           — Kill all MASHED.exe processes (managed AND orphan)

set -euo pipefail

PROJECT_ROOT="C:/Users/maria/Desktop/Proyectos/Mashed"
POOL_DIR="$PROJECT_ROOT/.frida_pool"
MAX_SLOTS=4

# Lock is a DIRECTORY: `mkdir` is atomic on both POSIX and Windows, which
# `: > file` is not. Two parallel `acquire` calls race on file creation but
# never race on `mkdir`. The PID tracker lives inside the lock dir.
slot_lock() { echo "$POOL_DIR/slot_${1}.lock"; }
slot_pid()  { echo "$POOL_DIR/slot_${1}.lock/pid"; }

is_locked() { [[ -d "$(slot_lock "$1")" ]]; }

pid_alive() {
    local pid="$1"
    [[ -z "$pid" ]] && return 1
    # Windows: tasklist returns 0 + "INFO: No tasks" or a header on miss; check for PID match.
    tasklist /FI "PID eq $pid" /NH 2>/dev/null | grep -qE "^\S+\s+$pid\s"
}

ensure_pool() { mkdir -p "$POOL_DIR"; }

cmd_acquire() {
    ensure_pool
    for i in $(seq 0 $((MAX_SLOTS - 1))); do
        # Atomic acquire via `mkdir` — only one caller can create the dir.
        if mkdir "$(slot_lock "$i")" 2>/dev/null; then
            echo "$i"
            return 0
        fi
        # Dir already exists. If the recorded PID is dead, reclaim.
        if [[ -f "$(slot_pid "$i")" ]]; then
            local pid
            pid=$(head -1 "$(slot_pid "$i")" 2>/dev/null || echo '')
            if [[ -n "$pid" ]] && ! pid_alive "$pid"; then
                rm -rf "$(slot_lock "$i")"
                if mkdir "$(slot_lock "$i")" 2>/dev/null; then
                    echo "$i"
                    return 0
                fi
            fi
        fi
    done
    echo "ERROR: all $MAX_SLOTS Frida pool slots busy. Run 'frida_pool.sh status'." >&2
    exit 1
}

cmd_record() {
    local i="${1:-}" pid="${2:-}"
    [[ -z "$i" || -z "$pid" ]] && { echo "Usage: frida_pool.sh record <N> <PID>" >&2; exit 1; }
    ensure_pool
    is_locked "$i" || { echo "ERROR: slot $i not acquired" >&2; exit 1; }
    printf '%s\n%s\n' "$pid" "$(date -Iseconds)" > "$(slot_pid "$i")"
    echo "Recorded PID $pid in slot $i"
}

cmd_release() {
    local i="${1:-}"
    [[ -z "$i" ]] && { echo "Usage: frida_pool.sh release <N>" >&2; exit 1; }
    if [[ -f "$(slot_pid "$i")" ]]; then
        local pid
        pid=$(head -1 "$(slot_pid "$i")" 2>/dev/null || echo '')
        if [[ -n "$pid" ]] && pid_alive "$pid"; then
            taskkill /F /PID "$pid" >/dev/null 2>&1 || true
        fi
    fi
    rm -rf "$(slot_lock "$i")"
    echo "Released slot $i"
}

cmd_status() {
    if [[ ! -d "$POOL_DIR" ]]; then echo "Pool not initialized."; exit 0; fi
    echo "Mashed Frida Pool"
    echo "  Pool dir: $POOL_DIR"
    echo "  Max:      $MAX_SLOTS slots"
    for i in $(seq 0 $((MAX_SLOTS - 1))); do
        if is_locked "$i"; then
            local pid='?' started='?'
            if [[ -f "$(slot_pid "$i")" ]]; then
                pid=$(sed -n 1p "$(slot_pid "$i")" 2>/dev/null || echo '?')
                started=$(sed -n 2p "$(slot_pid "$i")" 2>/dev/null || echo '?')
            fi
            local liveness='DEAD'
            pid_alive "$pid" && liveness='alive'
            echo "  Slot $i: LOCKED  pid=$pid  state=$liveness  started=$started"
        else
            echo "  Slot $i: available"
        fi
    done
}

cmd_cleanup() {
    ensure_pool
    echo "Cleaning Frida pool..."
    for i in $(seq 0 $((MAX_SLOTS - 1))); do
        if [[ -f "$(slot_pid "$i")" ]]; then
            local pid
            pid=$(head -1 "$(slot_pid "$i")" 2>/dev/null || echo '')
            if [[ -n "$pid" ]] && pid_alive "$pid"; then
                taskkill /F /PID "$pid" >/dev/null 2>&1 || true
                echo "  killed pid $pid (slot $i)"
            fi
        fi
    done
    rm -rf "$POOL_DIR"/*.lock 2>/dev/null || true
    echo "Done."
}

cmd_killall() {
    echo "Killing every MASHED.exe (managed and orphan)..."
    taskkill /F /IM MASHED.exe >/dev/null 2>&1 || true
    rm -rf "$POOL_DIR"/*.lock 2>/dev/null || true
    echo "Done."
}

case "${1:-status}" in
    acquire) cmd_acquire ;;
    record)  cmd_record "${2:-}" "${3:-}" ;;
    release) cmd_release "${2:-}" ;;
    status)  cmd_status ;;
    cleanup) cmd_cleanup ;;
    killall) cmd_killall ;;
    *)
        echo "Usage: frida_pool.sh {acquire|record N PID|release N|status|cleanup|killall}"
        exit 1 ;;
esac
