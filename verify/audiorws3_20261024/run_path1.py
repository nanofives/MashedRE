# D4-AUDIO-RWS3 path1 runner: run_diff.py per registry name, each under mashed_lock.py run, output teed to
# D:/MashedWT/D4-AUDIO-RWS3_data/path1/P_<name>.txt. Prints one "[N/M] name STATUS" line per hook.
# usage: py -3.12 verify/audiorws3_20261024/run_path1.py [name ...]   (from the worktree root)
# RWS3_NOLOCK=1: no per-hook lock (the caller already holds one around the whole run).
import os, subprocess, sys
W = os.path.dirname(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
LOCK = r"C:\Users\maria\Desktop\Proyectos\Mashed\.worktrees\orch-integration\re\orchestrator\mashed_lock.py"
OUT = "D:/MashedWT/D4-AUDIO-RWS3_data/path1"
NAMES = ["audio_fmt_table_search", "audio_fmt_global_scan", "audio_sub_struct_a_free", "audio_sub_struct_b_free",
         "audio_sub_struct_cleanup", "audio_sub_struct_two_call_init", "audio_sub_struct_link_device",
         "audio_sub_struct_link_buffer", "audio_sub_struct_dual_init", "audio_wave_node_free",
         "audio_sub_struct_buf_cleanup", "audio_context_lookup", "audio_sub_struct_link_both"]
names = sys.argv[1:] or NAMES
os.makedirs(OUT, exist_ok=True)
for n, name in enumerate(names, 1):
    cmd = [sys.executable, os.path.join(W, "re", "frida", "run_diff.py"), name]
    if not os.environ.get("RWS3_NOLOCK"):
        cmd = [sys.executable, LOCK, "run", "--label", "D4-AUDIO-RWS3", "--"] + cmd
    p = subprocess.run(cmd, cwd=W, capture_output=True, text=True, errors="replace")
    text = p.stdout + p.stderr
    with open(os.path.join(OUT, f"P_{name}.txt"), "w", encoding="utf-8") as f:
        f.write(text)
    status = "GREEN" if "GREEN:" in text else ("RED" if "mismatches:" in text else "ERROR")
    mm = [l.strip() for l in text.splitlines() if "total cases" in l or "mismatches:" in l]
    print(f"[{n}/{len(names)}] {name} {status}  {' '.join(mm)}  rc={p.returncode}", flush=True)
