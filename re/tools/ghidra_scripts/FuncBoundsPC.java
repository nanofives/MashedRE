// FuncBoundsPC.java - dump EVERY function's bounds to a CSV, once, for offline reuse.
// usage: analyzeHeadless <proj> <name> -process MASHED.exe -readOnly -noanalysis \
//   -scriptPath <dir> -postScript FuncBoundsPC.java <out.csv>
//
// Columns: rva,end,size,name,thunk    (rva/end hex without 0x, size decimal)
//   end  = body max address INCLUSIVE, so size = end - start + 1.
// Written for the TT-2 operand-correspondence sweep (re/tools/matchdiff_sweep.py),
// which needs an original-side size for ~1300 RVAs and must not page them through
// an MCP session. Cache the CSV; it only changes if the master project is re-analysed.
import ghidra.app.script.GhidraScript;
import ghidra.program.model.listing.Function;
import ghidra.program.model.listing.FunctionIterator;
import java.io.PrintWriter;

public class FuncBoundsPC extends GhidraScript {
    @Override
    public void run() throws Exception {
        String[] args = getScriptArgs();
        if (args.length < 1) { println("need <out.csv>"); return; }
        PrintWriter out = new PrintWriter(args[0], "UTF-8");
        out.println("rva,end,size,name,thunk");
        FunctionIterator it = currentProgram.getFunctionManager().getFunctions(true);
        int n = 0;
        while (it.hasNext()) {
            Function f = it.next();
            long s = f.getEntryPoint().getOffset();
            long e = f.getBody().getMaxAddress().getOffset();
            out.println(String.format("%08x,%08x,%d,%s,%s",
                    s, e, (e - s + 1), f.getName(), f.isThunk() ? "1" : "0"));
            n++;
        }
        out.close();
        println("FuncBoundsPC: wrote " + n + " functions to " + args[0]);
    }
}
