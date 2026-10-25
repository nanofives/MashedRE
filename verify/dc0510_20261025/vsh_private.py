# Runs re/frida/verify_scoring_hooks.py against a private root (MASHED.exe + this worktree's .asi), not the shared original\.
# usage: py -3.12 vsh_private.py <worktree> <private_original_dir> [--crashlog <file>] [script args...]
import sys
from pathlib import Path
wt, orig = Path(sys.argv[1]), Path(sys.argv[2])
rest = sys.argv[3:]
crash = None
if '--crashlog' in rest:
    i = rest.index('--crashlog'); crash = rest[i + 1]; del rest[i:i + 2]
sys.path.insert(0, str(wt / 're' / 'frida'))
import verify_scoring_hooks as v
v.ORIG = orig
v.EXE = orig / 'MASHED.exe'
if crash:
    p = crash.replace('\\', '/')
    v.AGENT = v.AGENT + (
        "\nProcess.setExceptionHandler(function(d){ try{ const f=new File('" + p + "','a');"
        " const m=Process.findModuleByAddress(d.address); const ctx=d.context;"
        " let bt=''; try{ bt=Thread.backtrace(ctx, Backtracer.FUZZY).slice(0,12).map(function(a){"
        " const mm=Process.findModuleByAddress(a); return mm? mm.name+'+0x'+a.sub(mm.base).toString(16) : a.toString(); }).join(' '); }catch(e){}"
        " f.write(JSON.stringify({type:d.type, address:d.address.toString(), mod: m? m.name+'+0x'+d.address.sub(m.base).toString(16):null,"
        " memory: d.memory? {op:d.memory.operation, addr:d.memory.address.toString()}:null,"
        " eax:ctx.eax.toString(), ecx:ctx.ecx.toString(), edx:ctx.edx.toString(), esp:ctx.esp.toString(), bt:bt})+'\\n'); f.close(); }catch(e){} return false; });\n")
sys.argv = [str(wt / 're' / 'frida' / 'verify_scoring_hooks.py')] + rest
sys.exit(v.main())
