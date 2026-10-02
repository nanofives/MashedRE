// GrepDecompAll.java -- decompile EVERY function in an address range and report
// only the ones whose decompiled C matches a regex.
//
// Why this exists: a struct-field writer that uses a COMPUTED base
// (`add eax,0xbfc` then `mov [eax-4],2`) encodes no 0xbf8 displacement, so byte
// scans, findoffset.py and Ghidra's own reference manager all miss it
// (memories: offset-grep-misses-dword-index, findoffset-blind-to-computed-bases).
// The DECOMPILER folds the arithmetic back to `*(int *)(rec + 0xbf8)`, so a grep
// over decompiled text is the one instrument that sees it.
//
// args: <regex> <out_file> [lo_va] [hi_va]
// Defaults: lo=0x00401000 hi=0x005cb000 (.text).
//
// Output: one block per matching function with the matching lines and their
// context, so a hit can be cited without a second run.
import ghidra.app.decompiler.DecompInterface;
import ghidra.app.decompiler.DecompileResults;
import ghidra.app.script.GhidraScript;
import ghidra.program.model.address.Address;
import ghidra.program.model.listing.Function;
import ghidra.program.model.listing.FunctionIterator;
import java.io.File;
import java.io.FileWriter;
import java.io.PrintWriter;
import java.util.regex.Matcher;
import java.util.regex.Pattern;

public class GrepDecompAll extends GhidraScript {
    @Override
    public void run() throws Exception {
        String[] args = getScriptArgs();
        if (args.length < 2) {
            throw new IllegalArgumentException("usage: GrepDecompAll <regex> <out_file> [lo] [hi]");
        }
        Pattern pat = Pattern.compile(args[0]);
        File out = new File(args[1]);
        long lo = args.length > 2 ? Long.parseLong(args[2].replaceFirst("^0x", ""), 16) : 0x00401000L;
        long hi = args.length > 3 ? Long.parseLong(args[3].replaceFirst("^0x", ""), 16) : 0x005cb000L;

        PrintWriter w = new PrintWriter(new FileWriter(out));
        w.println("// GrepDecompAll regex=" + args[0]
                + String.format(" range=0x%08x..0x%08x", lo, hi));

        DecompInterface di = new DecompInterface();
        di.openProgram(currentProgram);

        int scanned = 0, matched = 0, failed = 0;
        FunctionIterator it = currentProgram.getFunctionManager().getFunctions(true);
        while (it.hasNext()) {
            if (monitor.isCancelled()) break;
            Function fn = it.next();
            Address ep = fn.getEntryPoint();
            long va = ep.getOffset();
            if (va < lo || va >= hi) continue;
            if (fn.isThunk() || fn.isExternal()) continue;
            scanned++;
            DecompileResults res = di.decompileFunction(fn, 60, monitor);
            if (!res.decompileCompleted()) { failed++; continue; }
            String c = res.getDecompiledFunction().getC();
            Matcher m = pat.matcher(c);
            if (!m.find()) continue;
            matched++;
            w.println();
            w.println("==== " + fn.getName() + " @ " + ep + "  ("
                    + fn.getBody().getNumAddresses() + " bytes)");
            String[] lines = c.split("\r?\n");
            for (int i = 0; i < lines.length; i++) {
                if (pat.matcher(lines[i]).find()) {
                    int a = Math.max(0, i - 3), b = Math.min(lines.length - 1, i + 3);
                    for (int j = a; j <= b; j++) {
                        w.println(String.format("  %5d%s %s", j + 1, j == i ? ">" : " ", lines[j]));
                    }
                    w.println("  ---");
                }
            }
            w.flush();
            println("GrepDecompAll: MATCH " + fn.getName() + " @ " + ep);
        }
        w.println();
        w.println("// scanned=" + scanned + " matched=" + matched + " decompile_failed=" + failed);
        w.close();
        di.dispose();
        println("GrepDecompAll: scanned=" + scanned + " matched=" + matched + " failed=" + failed);
    }
}
