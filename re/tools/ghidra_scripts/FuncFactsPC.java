// FuncFactsPC.java - dump per-function facts for EVERY function, once, for offline reuse by
// re/tools/xref.py and re/tools/c2_note.py (the mechanical C2 note generator).
// usage: analyzeHeadless <proj> <name> -process MASHED.exe -readOnly -noanalysis \
//   -scriptPath <dir> -postScript FuncFactsPC.java <out.jsonl> [create.txt]
//
// create.txt (optional): one VA per line. Each VA that is neither a function entry nor inside a
// function body gets a function DEFINED for this session only (disassemble + createFunction), and its
// line carries "created":true. Safe by construction: the clone is opened -readOnly, so nothing is
// persisted (same contract as DecompPC.java `create`). Ghidra's auto-analysis misses functions reached
// only through a data table (the Lua binding callbacks registered by 0x0047b980 are one such set).
//
// One JSON object per line:
//   {"entry":"004a4541","name":"FUN_004a4541","cc":"__stdcall","proto":"int __stdcall FUN_...(...)",
//    "purge":8,"thunk":"00512340"|null,"ranges":[["004a4541","004a45a0"],...],   (end INCLUSIVE)
//    "refs":[["00412345","UNCONDITIONAL_CALL","00412300"],...]}                    (from, type, from-fn)
// Read-only: it only walks the FunctionManager and ReferenceManager. Run it against a pool
// clone opened -readOnly, never the master. Cache the output; it only changes if the master
// project is re-analysed.
import ghidra.app.script.GhidraScript;
import ghidra.program.model.address.Address;
import ghidra.program.model.address.AddressRange;
import ghidra.program.model.listing.Function;
import ghidra.program.model.listing.FunctionIterator;
import ghidra.program.model.symbol.Reference;
import ghidra.program.model.symbol.ReferenceIterator;
import java.io.PrintWriter;

public class FuncFactsPC extends GhidraScript {
    private static String q(String s) {
        if (s == null) return "null";
        StringBuilder b = new StringBuilder("\"");
        for (char c : s.toCharArray()) {
            if (c == '"' || c == '\\') b.append('\\').append(c);
            else if (c < 0x20) b.append(String.format("\\u%04x", (int) c));
            else b.append(c);
        }
        return b.append('"').toString();
    }

    private static String hx(Address a) {
        return String.format("%08x", a.getOffset());
    }

    @Override
    public void run() throws Exception {
        String[] args = getScriptArgs();
        if (args.length < 1) { println("need <out.jsonl>"); return; }
        java.util.Set<Long> created = new java.util.HashSet<>();
        if (args.length >= 2) {
            for (String line : java.nio.file.Files.readAllLines(java.nio.file.Paths.get(args[1]))) {
                String t = line.trim();
                if (t.isEmpty() || t.startsWith("#")) continue;
                if (t.startsWith("0x") || t.startsWith("0X")) t = t.substring(2);
                Address a = toAddr(Long.parseLong(t, 16));
                if (getFunctionAt(a) != null || getFunctionContaining(a) != null) continue;
                // Disassemble FIRST: createFunction on undisassembled bytes succeeds with a 1-byte body
                // (observed at 0x00449710, 0x0044ca40, 0x004714f0, 0x004496a0 on 2026-10-11).
                Function fn = null;
                try { disassemble(a); fn = createFunction(a, null); } catch (Exception e) { }
                if (fn != null) created.add(a.getOffset());
                else println("FuncFactsPC: create failed at " + t);
            }
        }
        PrintWriter out = new PrintWriter(args[0], "UTF-8");
        FunctionIterator it = currentProgram.getFunctionManager().getFunctions(true);
        int n = 0;
        while (it.hasNext()) {
            Function f = it.next();
            StringBuilder b = new StringBuilder();
            b.append("{\"entry\":").append(q(hx(f.getEntryPoint())));
            b.append(",\"name\":").append(q(f.getName()));
            b.append(",\"cc\":").append(q(f.getCallingConventionName()));
            b.append(",\"proto\":").append(q(f.getPrototypeString(false, false)));
            b.append(",\"purge\":").append(f.getStackPurgeSize());
            if (created.contains(f.getEntryPoint().getOffset())) b.append(",\"created\":true");
            Function t = f.isThunk() ? f.getThunkedFunction(true) : null;
            if (t != null && t.isExternal()) {
                b.append(",\"thunk\":").append(q("EXTERNAL:" + t.getName(true)));
            } else {
                b.append(",\"thunk\":").append(t == null ? "null" : q(hx(t.getEntryPoint())));
            }
            b.append(",\"ranges\":[");
            boolean first = true;
            for (AddressRange r : f.getBody()) {
                if (!first) b.append(',');
                first = false;
                b.append('[').append(q(hx(r.getMinAddress()))).append(',')
                 .append(q(hx(r.getMaxAddress()))).append(']');
            }
            b.append("],\"refs\":[");
            first = true;
            ReferenceIterator ri = currentProgram.getReferenceManager().getReferencesTo(f.getEntryPoint());
            while (ri.hasNext()) {
                Reference r = ri.next();
                Address from = r.getFromAddress();
                Function ff = getFunctionContaining(from);
                if (!first) b.append(',');
                first = false;
                b.append('[').append(q(from.isMemoryAddress() ? hx(from) : from.toString())).append(',')
                 .append(q(r.getReferenceType().getName())).append(',')
                 .append(ff == null ? "null" : q(hx(ff.getEntryPoint()))).append(']');
            }
            b.append("]}");
            out.println(b.toString());
            n++;
        }
        out.close();
        println("FuncFactsPC: wrote " + n + " functions to " + args[0]);
    }
}
