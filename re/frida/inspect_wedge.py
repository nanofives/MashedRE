# inspect_wedge.py — interrogate a WEDGED MASHED process instead of bisecting a flaky failure.
#
# U-9025 (2026-07-28): with a 75-hook set installed, the menu-navigated race wedges at race
# entry. The failure is NON-DETERMINISTIC — the byte-identical installed set (verified by
# diffing the two runs' MASHED_HOOK_MANIFEST outputs) both wedged and completed — so a
# single-run pass/fail predicate is unsound and a bisect built on one cannot be trusted.
#
# What IS solid is the wedge itself: the main thread blocks in
#     0x005ab63b  CALL EDI      ; EDI = [0x005cc090] = PTR_WaitForSingleObject
#                               ; args: PUSH -1 (INFINITE), PUSH [0x007dd618] (a semaphore)
# with return address 0x005ab63d, frame-for-frame identical across three processes. So the
# question is not "which hook" but "who was supposed to release 0x007dd618 and didn't".
#
# The most likely mechanism for a blocked-forever main thread is that the releasing thread is
# GONE (exited/died) or is itself blocked. This dumps enough to tell those apart:
#   * every thread, its pc, and the module/RVA it sits in  -> is the audio worker still alive?
#   * the audio globals around the semaphore pair
#   * the semaphore HANDLEs and their signal state via WaitForSingleObject(h, 0)
#
# Run against a wedged pid; compare with a HEALTHY pid (--pid of a running game) to see which
# thread is missing. Kill ONLY pids you spawned (CLAUDE.md).
#
# Frida 17: the static Module.findExportByName was REMOVED — use
# Process.findModuleByName(m).findExportByName(n).
import argparse, json, sys
import frida

# Audio globals cited from the wedged frame FUN_005ab620 (0x005ab620..0x005ab708).
GLOBALS = {
    # --- THE ACTUAL WEDGE SITE (measured 2026-07-28, GUI thread ret addr 0x005a840c) ---
    # FUN_005a8390:
    #   005a83f4  MOV EAX,[0x007dcb68] / TEST / JZ            <- gate read #1
    #   005a8406  CALL [0x005cc090]  WaitForSingleObject([0x007dcae0], INFINITE)   <- BLOCKS
    #   005a8423  MOV EAX,[0x007dcb68] / TEST / JZ            <- gate read #2 (independent!)
    #   005a843d  CALL [0x005cc094]  ReleaseSemaphore([0x007dcae0], 1, NULL)
    #   005a8449  INC EAX / MOV [0x007dcb68],EAX              <- increment AFTER the release
    # FUN_005a8460:
    #   005a846a  DEC EAX / MOV [0x007dcb68],EAX              <- decrement BEFORE its own use
    # 0x007dcae0 = CreateSemaphoreA(NULL, initial=1, max=1, NULL) (pushes at 0x005a8290 /
    # 0x005a829c, call 0x005a82d2) -- a binary semaphore used as a mutex, starts SIGNALLED.
    #
    # HYPOTHESIS UNDER TEST: acquire happens under gate read #1, release under gate read #2.
    # If 0x007dcb68 hits 0 in between (FUN_005a8460 on another thread), the release is SKIPPED
    # and the count stays 0 forever -> every later waiter deadlocks.
    # CONFIRMED IF, at wedge time: 0x007dcb68 == 0 AND 0x007dcae0 probes WAIT_TIMEOUT.
    # REFUTED IF: 0x007dcb68 != 0, or 0x007dcae0 probes WAIT_OBJECT_0 (then the GUI thread is
    # not actually queued on it and the whole reading is wrong).
    "0x007dcae0": "THE WEDGE SEMAPHORE (binary, init=1 max=1) - GUI thread waits here INFINITE",
    "0x007dcb68": "GATE/REFCOUNT - read twice in FUN_005a8390; ==0 skips the RELEASE",
    "0x007dcad8": "head of the linked list the semaphore guards",
    "0x007dcadc": "tail/second list pointer, set beside 0x007dcad8 at 0x005a82cd",
    # --- the previously-blamed audio pair: measured SIGNALLED in two separate wedges, so the
    # main thread was never queued on these. Kept to keep disproving it cheaply. ---
    "0x007dd608": "cleared by FUN_005ab620 when [0x007dd608]==ESI",
    "0x007dd60c": "cleared alongside 0x007dd608",
    "0x007dd618": "audio streaming lock (U-6700) - NOT the wedge object",
    "0x007dd61c": "passed to FUN_005aeed0 at 0x005ab648",
    "0x007dd620": "audio wait-list lock - NOT the wedge object",
    "0x007dd62c": "compared against ESI at 0x005ab65f",
}

JS = r"""
var GLOBALS = %s;

function modOf(addr) {
    var m = Process.findModuleByAddress(addr);
    if (!m) return { mod: "?", rva: "?" };
    return { mod: m.name, rva: "0x" + addr.sub(m.base).toString(16) };
}

var wfso = Process.findModuleByName("kernel32.dll").findExportByName("WaitForSingleObject");
var WaitForSingleObject = new NativeFunction(wfso, 'uint32', ['pointer', 'uint32']);

var out = { threads: [], globals: {}, semaphores: {}, mainThread: null };

// Which ntdll routine is a parked thread sitting in? Every thread's pc was in ntdll, and the
// raw address says nothing. Resolving to the nearest EXPORT at or below pc names the KIND of
// wait -- NtWaitForSingleObject vs NtWaitForAlertByThreadId vs NtRemoveIoCompletion vs
// NtUserGetMessage are completely different stories about why the game is stuck.
var ntdllExports = null;
function nearestExport(addr) {
    var m = Process.findModuleByAddress(addr);
    if (!m) return "?";
    if (ntdllExports === null) {
        ntdllExports = {};
        Process.enumerateModules().forEach(function (mod) {
            ntdllExports[mod.name] = mod.enumerateExports()
                .filter(function (e) { return e.type === 'function'; })
                .sort(function (a, b) { return a.address.compare(b.address); });
        });
    }
    var list = ntdllExports[m.name] || [];
    var best = null;
    for (var i = 0; i < list.length; i++) {
        if (list[i].address.compare(addr) <= 0) best = list[i]; else break;
    }
    if (!best) return m.name + "+?";
    return m.name + "!" + best.name + "+0x" + addr.sub(best.address).toString(16);
}

// The GUI thread is the one that owns the game window -- ask the OS instead of guessing that
// "the first thread Frida enumerates" is main. A wedged GUI thread is what IsHungAppWindow
// actually reports, so this is the thread the whole investigation is about.
try {
    var u32 = Process.findModuleByName("user32.dll");
    var FindWindowW = new NativeFunction(u32.findExportByName("FindWindowW"), 'pointer', ['pointer','pointer']);
    var GetWindowThreadProcessId = new NativeFunction(
        u32.findExportByName("GetWindowThreadProcessId"), 'uint32', ['pointer','pointer']);
    var EnumWindows = new NativeFunction(u32.findExportByName("EnumWindows"), 'int', ['pointer','pointer']);
    var IsWindowVisible = new NativeFunction(u32.findExportByName("IsWindowVisible"), 'int', ['pointer']);
    var pidBuf = Memory.alloc(4);
    var myPid = Process.id;
    var cb = new NativeCallback(function (hwnd, lparam) {
        GetWindowThreadProcessId(hwnd, pidBuf);
        if (pidBuf.readU32() === myPid && IsWindowVisible(hwnd)) {
            var tid = GetWindowThreadProcessId(hwnd, pidBuf);
            if (out.mainThread === null) out.mainThread = tid;
        }
        return 1;
    }, 'int', ['pointer','pointer']);
    EnumWindows(cb, ptr(0));
} catch (e) { out.mainThreadError = "" + e; }

Process.enumerateThreads().forEach(function (t) {
    var pc = t.context.pc;
    var w = modOf(pc);
    out.threads.push({
        id: t.id, state: t.state, pc: "" + pc, mod: w.mod, rva: w.rva,
        sym: nearestExport(pc),
        esp: "" + t.context.esp
    });
});

Object.keys(GLOBALS).forEach(function (g) {
    var p = ptr(g);
    try { out.globals[g] = { desc: GLOBALS[g], value: "0x" + p.readU32().toString(16) }; }
    catch (e) { out.globals[g] = { desc: GLOBALS[g], value: "<unreadable: " + e + ">" }; }
});

// A semaphore that is SIGNALLED (count > 0) while the main thread sits in an INFINITE wait on
// it would mean the wait is not the whole story. A 0-timeout probe answers that without
// consuming a count on WAIT_TIMEOUT. NOTE: on WAIT_OBJECT_0 this DOES decrement the count —
// it is a diagnostic on an already-dead process, never on a run under measurement.
["0x007dcae0", "0x007dd618", "0x007dd620"].forEach(function (g) {
    try {
        var h = ptr(g).readPointer();
        var r = WaitForSingleObject(h, 0);
        out.semaphores[g] = {
            handle: "" + h,
            probe: r,
            meaning: r === 0 ? "WAIT_OBJECT_0 (was signalled - and this probe just took a count)"
                   : r === 258 ? "WAIT_TIMEOUT (count 0 - nobody has released it)"
                   : r === 0x80 ? "WAIT_ABANDONED (owner thread DIED holding it)"
                   : "0x" + r.toString(16)
        };
    } catch (e) { out.semaphores[g] = { error: "" + e }; }
});

// Poor-man's stack walk. Frida cannot Thread.backtrace() a thread it is not running on, and
// every thread here is parked in ntdll, so instead scan each thread's stack upward for dwords
// that land inside MASHED.exe's executable range AND are immediately preceded by a CALL. That
// over-reports (stale frames linger on the stack) but it is the only way to see which threads
// are inside game code at all.
// [UNCERTAIN] How the PRIOR session captured its 0x005ab63d chain is NOT recorded anywhere in
// the repo -- do not assume it used this method, and do not cite this comment as evidence that
// it did. Its "identical frame-for-frame across three processes" claim is consistent with a
// heuristic scan (stale frames are deterministic too), but that is an observation, not a proof.
// TREAT THE RESULT AS CANDIDATE RETURN ADDRESSES, NOT A VERIFIED CALL CHAIN.
var mashed = Process.findModuleByName("MASHED.exe");
function looksLikeRet(a) {
    // A return address is preceded by the call that pushed it. Check the three encodings the
    // game actually uses: E8 rel32 (5 bytes), FF /2 via register (2 bytes, e.g. FF D7 CALL EDI),
    // FF /2 via memory (6 bytes, e.g. FF 15 CALL [imm32]).
    try {
        if (a.sub(5).readU8() === 0xE8) return true;
        if (a.sub(2).readU8() === 0xFF) return true;
        if (a.sub(6).readU8() === 0xFF) return true;
    } catch (e) {}
    return false;
}
if (mashed) {
    var lo = mashed.base, hi = mashed.base.add(mashed.size);
    out.stacks = [];
    Process.enumerateThreads().forEach(function (t) {
        var frames = [], sp = t.context.esp;
        for (var i = 0; i < 512 && frames.length < 16; i++) {
            try {
                var v = sp.add(i * 4).readPointer();
                if (v.compare(lo) >= 0 && v.compare(hi) < 0 && looksLikeRet(v))
                    frames.push("0x" + v.toString(16));
            } catch (e) { break; }
        }
        if (frames.length) out.stacks.push({ id: t.id, frames: frames });
    });
}

send(out);
""" % json.dumps(GLOBALS)


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--pid", type=int, required=True,
                    help="explicit pid. NEVER 'first MASHED by name' - other sessions run "
                         "their own game (CLAUDE.md multi-session hygiene).")
    ap.add_argument("--probe-semaphores", action="store_true",
                    help="also probe semaphore signal state (mutates counts; wedged pids only)")
    args = ap.parse_args()

    js = JS
    if not args.probe_semaphores:
        js = js.replace('["0x007dd618", "0x007dd620"].forEach', '[].forEach')

    session = frida.attach(args.pid)
    result = {}

    def on_message(msg, data):
        if msg["type"] == "send":
            result.update(msg["payload"])
        else:
            print("ERROR:", msg, file=sys.stderr)

    script = session.create_script(js)
    script.on("message", on_message)
    script.load()

    if not result:
        print("no payload - the script did not run", file=sys.stderr)
        return 1

    main = result.get("mainThread")
    threads = result.get("threads", [])
    print("== threads (%d)  GUI/main tid=%s ==" % (len(threads), main))
    # Group by the ntdll routine they are parked in: 40 identical worker waits are noise, the
    # one-off is the signal.
    from collections import Counter
    counts = Counter(t.get("sym", "?") for t in threads)
    for t in threads:
        tag = " <== GUI/MAIN" if main is not None and t["id"] == main else ""
        if tag or counts[t.get("sym", "?")] <= 2:
            print("  tid=%-6s %-9s %-46s %s+%s%s"
                  % (t["id"], t["state"], t.get("sym", "?"), t["mod"], t["rva"], tag))
    print("  -- parked-thread histogram --")
    for sym, n in counts.most_common():
        print("     %4d x %s" % (n, sym))
    print("\n== audio globals ==")
    for g, v in result.get("globals", {}).items():
        print("  %s = %-12s  %s" % (g, v.get("value"), v.get("desc")))
    if result.get("stacks"):
        print("\n== threads with MASHED.exe frames on their stack (CANDIDATE return "
              "addresses, includes stale frames) ==")
        for s in result["stacks"]:
            print("  tid=%-6s %s" % (s["id"], " ".join(s["frames"])))
    if result.get("semaphores"):
        print("\n== semaphore probe ==")
        for g, v in result["semaphores"].items():
            print("  %s handle=%s -> %s" % (g, v.get("handle"), v.get("meaning", v.get("error"))))
    session.detach()
    return 0


if __name__ == "__main__":
    sys.exit(main())
