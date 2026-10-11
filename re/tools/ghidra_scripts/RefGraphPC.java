// RefGraphPC.java - dump every function's address ranges and every memory reference, once, for offline reuse.
// usage: analyzeHeadless <proj> <name> -process MASHED.exe -readOnly -noanalysis \
//   -scriptPath <dir> -postScript RefGraphPC.java <out_dir>
//
// Writes two TSVs into <out_dir>:
//   functions.tsv  entry  name  thunk  ranges      ranges = "lo-hi;lo-hi" hex, hi EXCLUSIVE
//   refs.tsv       from   from_func  to  type  primary
//       from_func = entry of the function whose body holds `from`, or "-" (data, or code outside any function)
//       type      = Ghidra RefType name (UNCONDITIONAL_CALL, DATA, PARAM, READ, WRITE, COMPUTED_CALL, ...)
// Only memory references are kept (no stack/register/external refs).
// Written for re/tools/pointer_callers.py, which needs (a) the function ranges to classify a dword as code or data and
// (b) Ghidra's own reference database as an independent witness for its precision gate.
import ghidra.app.script.GhidraScript;
import ghidra.program.model.address.Address;
import ghidra.program.model.address.AddressIterator;
import ghidra.program.model.address.AddressRange;
import ghidra.program.model.listing.Function;
import ghidra.program.model.listing.FunctionIterator;
import ghidra.program.model.listing.FunctionManager;
import ghidra.program.model.symbol.Reference;
import ghidra.program.model.symbol.ReferenceManager;
import java.io.File;
import java.io.PrintWriter;

public class RefGraphPC extends GhidraScript {
    @Override
    public void run() throws Exception {
        String[] args = getScriptArgs();
        if (args.length < 1) { println("need <out_dir>"); return; }
        File dir = new File(args[0]);
        dir.mkdirs();
        FunctionManager fm = currentProgram.getFunctionManager();

        PrintWriter fo = new PrintWriter(new File(dir, "functions.tsv"), "UTF-8");
        fo.println("entry\tname\tthunk\tranges");
        FunctionIterator it = fm.getFunctions(true);
        int nf = 0;
        while (it.hasNext()) {
            Function f = it.next();
            StringBuilder sb = new StringBuilder();
            for (AddressRange r : f.getBody()) {
                if (sb.length() > 0) sb.append(';');
                sb.append(String.format("%08x-%08x", r.getMinAddress().getOffset(), r.getMaxAddress().getOffset() + 1));
            }
            fo.println(String.format("%08x\t%s\t%d\t%s", f.getEntryPoint().getOffset(), f.getName(),
                    f.isThunk() ? 1 : 0, sb));
            nf++;
        }
        fo.close();

        PrintWriter ro = new PrintWriter(new File(dir, "refs.tsv"), "UTF-8");
        ro.println("from\tfrom_func\tto\ttype\tprimary");
        ReferenceManager rm = currentProgram.getReferenceManager();
        AddressIterator src = rm.getReferenceSourceIterator(currentProgram.getMinAddress(), true);
        int nr = 0;
        while (src.hasNext()) {
            Address a = src.next();
            Function owner = fm.getFunctionContaining(a);
            String ff = owner == null ? "-" : String.format("%08x", owner.getEntryPoint().getOffset());
            for (Reference r : rm.getReferencesFrom(a)) {
                if (!r.isMemoryReference()) continue;
                ro.println(String.format("%08x\t%s\t%08x\t%s\t%d", a.getOffset(), ff, r.getToAddress().getOffset(),
                        r.getReferenceType().getName(), r.isPrimary() ? 1 : 0));
                nr++;
            }
        }
        ro.close();
        println("RefGraphPC: wrote " + nf + " functions and " + nr + " references to " + dir);
    }
}
