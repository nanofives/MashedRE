# Ghidra Jython script: decompile a list of addresses to <outdir>/<rva>.c files.
# Run via the ghidra MCP `ghidra_script` tool against an open program session.
# script_args: [outdir, addr1, addr2, ...]  (addrs as 8-hex strings, no 0x)
#
# Used by the shape-only decomp-transcript pass (2026-05-29): dumps verbatim
# DecompInterface output server-side so the C never flows through the
# orchestrator's context. Equivalent to the MCP decomp_function output.
import os
from ghidra.app.decompiler import DecompInterface
from ghidra.util.task import ConsoleTaskMonitor

args = getScriptArgs()
outdir = args[0]
addrs = args[1:]

if not os.path.isdir(outdir):
    os.makedirs(outdir)

ifc = DecompInterface()
ifc.openProgram(currentProgram)
monitor = ConsoleTaskMonitor()

ok = 0
for a in addrs:
    a = a.strip()
    outpath = os.path.join(outdir, a + ".c")
    addr = toAddr(int(a, 16))
    func = getFunctionAt(addr)
    if func is None:
        f = open(outpath, "w")
        f.write("// NO_FUNCTION_AT " + a + "\n")
        f.close()
        print("MISSING " + a)
        continue
    res = ifc.decompileFunction(func, 60, monitor)
    if res is None or not res.decompileCompleted():
        f = open(outpath, "w")
        f.write("// DECOMPILE_FAILED " + a + "\n")
        f.close()
        print("FAILED " + a)
        continue
    code = res.getDecompiledFunction().getC()
    f = open(outpath, "w")
    f.write(code)
    f.close()
    ok += 1
    print("OK " + a + " (" + str(len(code)) + " bytes)")

print("DONE ok=" + str(ok) + " total=" + str(len(addrs)))
