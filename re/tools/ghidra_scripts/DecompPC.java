// Batch-decompile / inspect PC (MASHED.exe) functions in ONE headless invocation.
//
// usage: DecompPC.java <manifest.txt> <out.json> [modes...]
//   manifest.txt  one VA per line (hex, leading 0x optional); blank / '#' lines skipped
//   out.json      output path
//   modes         zero or more of {decomp,callees,callers,xrefs,strings} as SEPARATE
//                 args; default "decomp"
//
// GOTCHA: modes are separate args, never one comma-separated string. analyzeHeadless
// is a .bat, and cmd.exe splits arguments on commas, so "decomp,callers" arrives as
// just "decomp" and the rest is silently dropped. Avoid commas in any script arg.
//
// xrefs vs callers: `callers` is the call graph (functions that call this one).
// `xrefs` is every reference to the entry point INCLUDING data refs — which is how you
// find a function reached only through a vtable or function-pointer table, where the
// call graph shows nothing.
import ghidra.app.decompiler.DecompInterface;
import ghidra.app.decompiler.DecompileResults;
import ghidra.program.model.pcode.HighFunction;
import ghidra.program.model.pcode.HighSymbol;
import ghidra.program.model.pcode.FunctionPrototype;
import ghidra.program.model.pcode.GlobalSymbolMap;
import ghidra.program.model.listing.Parameter;
import java.util.Iterator;
import ghidra.app.script.GhidraScript;
import ghidra.program.model.address.Address;
import ghidra.program.model.listing.Data;
import ghidra.program.model.listing.Function;
import ghidra.program.model.listing.Instruction;
import ghidra.program.model.listing.InstructionIterator;
import ghidra.program.model.listing.Listing;
import ghidra.program.model.mem.MemoryBlock;
import ghidra.program.model.symbol.Reference;
import ghidra.program.model.symbol.ReferenceIterator;
import java.io.FileWriter;
import java.io.PrintWriter;
import java.nio.charset.StandardCharsets;
import java.nio.file.Files;
import java.nio.file.Paths;
import java.util.ArrayList;
import java.util.List;
import java.util.Set;
import java.util.TreeSet;

public class DecompPC extends GhidraScript {

    // Cap list output: a hot leaf can have hundreds of xrefs, which buries the signal
    // and bloats the JSON. Overflow is reported as a "+N more" entry.
    private static final int MAX_LIST = 64;
    private static final int MAX_STR_LEN = 200;

    private boolean mDecomp;
    private boolean mCallees;
    private boolean mCallers;
    private boolean mXrefs;
    private boolean mStrings;
    private boolean mPort;      // Lane 2: decompiler prototype + typed globals + callee prototypes

    @Override
    public void run() throws Exception {
        String[] args = getScriptArgs();
        if (args.length < 2) {
            throw new IllegalArgumentException("usage: DecompPC.java <manifest.txt> "
                    + "<out.json> [decomp] [callees] [callers] [xrefs] [strings]");
        }
        mDecomp = (args.length < 3);   // default when no modes given
        for (int i = 2; i < args.length; i++) {
            String m = args[i].trim().toLowerCase();
            if (m.equals("decomp")) {
                mDecomp = true;
            }
            else if (m.equals("callees")) {
                mCallees = true;
            }
            else if (m.equals("callers")) {
                mCallers = true;
            }
            else if (m.equals("xrefs")) {
                mXrefs = true;
            }
            else if (m.equals("strings")) {
                mStrings = true;
            }
            else if (m.equals("port")) {
                mPort = true; mDecomp = true;
            }
            else if (m.equals("metadata")) {
                continue;   // name/entry/size/signature only; emitted unconditionally
            }
            else if (!m.isEmpty()) {
                throw new IllegalArgumentException("unknown mode: " + m);
            }
        }

        List<String> vas = new ArrayList<>();
        for (String line : Files.readAllLines(Paths.get(args[0]), StandardCharsets.UTF_8)) {
            String t = line.trim();
            if (t.isEmpty() || t.startsWith("#")) {
                continue;
            }
            vas.add(t);
        }

        DecompInterface di = null;
        if (mDecomp) {
            di = new DecompInterface();
            di.openProgram(currentProgram);
        }

        PrintWriter w = new PrintWriter(new FileWriter(args[1]));
        w.println("{");
        w.println("  \"program\": \"" + esc(currentProgram.getName()) + "\",");
        w.println("  \"functions\": [");

        for (int i = 0; i < vas.size(); i++) {
            if (i > 0) {
                w.println("    },");
            }
            w.println("    {");
            String raw = vas.get(i);
            w.println("      \"requested\": \"" + esc(raw) + "\",");
            try {
                emitOne(w, raw, di);
            }
            catch (Exception e) {
                w.println("      \"error\": \"" + esc(e.toString()) + "\"");
            }
        }
        if (!vas.isEmpty()) {
            w.println("    }");
        }
        w.println("  ]");
        w.println("}");
        w.close();

        if (di != null) {
            di.dispose();
        }
        println("DecompPC: " + vas.size() + " request(s) -> " + args[1]);
    }

    private void emitOne(PrintWriter w, String raw, DecompInterface di) {
        long va = Long.parseLong(raw.replaceFirst("^0[xX]", ""), 16);
        Address a = currentProgram.getAddressFactory()
                .getDefaultAddressSpace().getAddress(va);

        Function fn = currentProgram.getFunctionManager().getFunctionAt(a);
        boolean exact = (fn != null);
        if (fn == null) {
            // Address may be mid-function; report which function contains it rather
            // than failing, so a wrong-by-a-few-bytes RVA still yields something.
            fn = currentProgram.getFunctionManager().getFunctionContaining(a);
        }
        if (fn == null) {
            // Don't just fail: a tracker RVA with no Ghidra function is itself a finding
            // (e.g. a stub reached only via a DATA table, which auto-analysis never
            // disassembled). Report enough to tell those cases apart.
            w.println("      \"error\": \"no function at or containing " + esc(raw) + "\",");
            probe(w, a);
            return;
        }

        w.println("      \"exact_entry\": " + exact + ",");
        w.println("      \"name\": \"" + esc(fn.getName()) + "\",");
        w.println("      \"entry\": \"0x" + fn.getEntryPoint() + "\",");
        w.println("      \"size\": " + fn.getBody().getNumAddresses() + ",");
        w.println("      \"signature\": \"" + esc(fn.getSignature().getPrototypeString()) + "\",");

        if (mCallees) {
            w.println("      \"callees\": [" + jsonList(funcSet(fn, true)) + "],");
        }
        if (mCallers) {
            w.println("      \"callers\": [" + jsonList(funcSet(fn, false)) + "],");
        }
        if (mXrefs) {
            w.println("      \"xrefs\": [" + jsonList(xrefsTo(fn.getEntryPoint())) + "],");
        }
        if (mStrings) {
            w.println("      \"strings\": [" + jsonList(stringsIn(fn)) + "],");
        }

        if (mDecomp && di != null) {
            DecompileResults res = di.decompileFunction(fn, 90, monitor);
            if (res.decompileCompleted()) {
                if (mPort) {
                    emitPort(w, fn, res, di);
                }
                w.println("      \"decomp\": \""
                        + esc(res.getDecompiledFunction().getC()) + "\"");
            }
            else {
                w.println("      \"decomp_error\": \"" + esc(res.getErrorMessage()) + "\"");
            }
        }
        else {
            w.println("      \"decomp\": null");
        }
    }


    // Lane 2 (decomp2port): what the plain C text does not carry. The DECOMPILER's
    // prototype (not the stored signature, which is often `undefined f(void)` while the
    // body uses params), every global the body references with its data type and
    // address, and each direct callee's stored prototype + calling convention so the
    // transcriber can emit raw-RVA thunks for unported callees.
    private void emitPort(PrintWriter w, Function fn, DecompileResults res, DecompInterface di) {
        HighFunction hf = res.getHighFunction();
        StringBuilder sb = new StringBuilder();
        if (hf != null) {
            FunctionPrototype fp = hf.getFunctionPrototype();
            sb.append("      \"proto\": {\"ret\": \"").append(esc(fp.getReturnType().getName()))
              .append("\", \"cc\": \"").append(esc(String.valueOf(fn.getCallingConventionName())))
              .append("\", \"varargs\": ").append(fp.isVarArg())
              .append(", \"params\": [");
            for (int i = 0; i < fp.getNumParams(); i++) {
                HighSymbol p = fp.getParam(i);
                if (i > 0) sb.append(", ");
                sb.append("{\"name\": \"").append(esc(p.getName())).append("\", \"type\": \"")
                  .append(esc(p.getDataType().getName())).append("\", \"size\": ")
                  .append(p.getSize()).append(", \"storage\": \"")
                  .append(esc(p.getStorage().toString())).append("\"}");
            }
            sb.append("]},\n");
            sb.append("      \"globals\": [");
            GlobalSymbolMap gm = hf.getGlobalSymbolMap();
            Iterator<HighSymbol> it = gm.getSymbols();
            boolean first = true;
            while (it.hasNext()) {
                HighSymbol g = it.next();
                Address ga = g.getStorage().getMinAddress();
                MemoryBlock blk = (ga == null) ? null : currentProgram.getMemory().getBlock(ga);
                if (!first) sb.append(", ");
                first = false;
                sb.append("{\"name\": \"").append(esc(g.getName())).append("\", \"addr\": \"")
                  .append(ga == null ? "" : "0x" + ga).append("\", \"type\": \"")
                  .append(esc(g.getDataType().getName())).append("\", \"size\": ")
                  .append(g.getSize()).append(", \"block\": \"")
                  .append(blk == null ? "" : esc(blk.getName())).append("\", \"writable\": ")
                  .append(blk != null && blk.isWrite()).append("}");
            }
            sb.append("],\n");
        }
        sb.append("      \"callee_protos\": [");
        try {
            Set<Function> set = fn.getCalledFunctions(monitor);
            TreeSet<String> rows = new TreeSet<>();
            for (Function f : set) {
                StringBuilder r = new StringBuilder();
                r.append("{\"name\": \"").append(esc(f.getName())).append("\", \"entry\": \"0x")
                 .append(f.getEntryPoint()).append("\", \"proto\": \"")
                 .append(esc(f.getSignature().getPrototypeString())).append("\", \"cc\": \"")
                 .append(esc(String.valueOf(f.getCallingConventionName()))).append("\", \"ret\": \"")
                 .append(esc(f.getReturnType().getName())).append("\", \"thunk\": ")
                 .append(f.isThunk()).append(", \"external\": ").append(f.isExternal())
                 .append(", \"params\": [");
                Parameter[] ps = f.getParameters();
                for (int i = 0; i < ps.length; i++) {
                    if (i > 0) r.append(", ");
                    r.append("{\"name\": \"").append(esc(ps[i].getName())).append("\", \"type\": \"")
                     .append(esc(ps[i].getDataType().getName())).append("\"}");
                }
                r.append("]");
                // stored signatures are mostly `undefined f(void)` in this project; the
                // DECOMPILER's prototype is what a thunk for an unported callee needs
                if (di != null && !f.isExternal()) {
                    try {
                        DecompileResults cr = di.decompileFunction(f, 30, monitor);
                        HighFunction chf = cr.decompileCompleted() ? cr.getHighFunction() : null;
                        if (chf != null) {
                            FunctionPrototype cp = chf.getFunctionPrototype();
                            r.append(", \"dproto\": {\"ret\": \"").append(esc(cp.getReturnType().getName()))
                             .append("\", \"varargs\": ").append(cp.isVarArg()).append(", \"params\": [");
                            for (int i = 0; i < cp.getNumParams(); i++) {
                                HighSymbol p = cp.getParam(i);
                                if (i > 0) r.append(", ");
                                r.append("{\"name\": \"").append(esc(p.getName())).append("\", \"type\": \"")
                                 .append(esc(p.getDataType().getName())).append("\", \"storage\": \"")
                                 .append(esc(p.getStorage().toString())).append("\"}");
                            }
                            r.append("]}");
                        }
                    }
                    catch (Exception e) { r.append(", \"dproto_error\": \"").append(esc(String.valueOf(e))).append("\""); }
                }
                r.append("}");
                rows.add(r.toString());
            }
            boolean first = true;
            for (String r : rows) { if (!first) sb.append(", "); first = false; sb.append(r); }
        }
        catch (Exception e) {
            sb.append("{\"error\": \"").append(esc(String.valueOf(e))).append("\"}");
        }
        sb.append("],");
        w.println(sb.toString());
    }

    // Sorted "name@entry" set, so output is stable across runs.
    private Set<String> funcSet(Function fn, boolean callees) {
        Set<String> sorted = new TreeSet<>();
        try {
            Set<Function> set = callees ? fn.getCalledFunctions(monitor)
                                        : fn.getCallingFunctions(monitor);
            for (Function f : set) {
                sorted.add(f.getName() + "@0x" + f.getEntryPoint());
            }
        }
        catch (Exception e) {
            sorted.add("ERROR: " + e);
        }
        return sorted;
    }

    // Diagnostics for an address with no function: what does Ghidra think is here?
    private void probe(PrintWriter w, Address a) {
        MemoryBlock blk = currentProgram.getMemory().getBlock(a);
        if (blk == null) {
            w.println("      \"probe_block\": \"(address not in any memory block)\",");
        }
        else {
            w.println("      \"probe_block\": \"" + esc(blk.getName()) + " "
                    + (blk.isRead() ? "r" : "-") + (blk.isWrite() ? "w" : "-")
                    + (blk.isExecute() ? "x" : "-") + "\",");
        }

        Listing listing = currentProgram.getListing();
        String what;
        if (listing.getInstructionAt(a) != null) {
            what = "instruction (disassembled, but not inside any function)";
        }
        else if (listing.getInstructionContaining(a) != null) {
            what = "inside an instruction (misaligned address)";
        }
        else {
            Data d = listing.getDataAt(a);
            if (d != null && d.isDefined()) {
                what = "defined-data: " + d.getDataType().getName();
            }
            else if (d != null) {
                what = "undefined bytes (never disassembled)";
            }
            else {
                what = "nothing (no code unit)";
            }
        }
        w.println("      \"probe_at\": \"" + esc(what) + "\",");

        StringBuilder hex = new StringBuilder();
        try {
            byte[] buf = new byte[16];
            int n = currentProgram.getMemory().getBytes(a, buf);
            for (int i = 0; i < n; i++) {
                hex.append(String.format("%02x ", buf[i]));
            }
        }
        catch (Exception e) {
            hex.append("unreadable: ").append(e);
        }
        w.println("      \"probe_bytes\": \"" + esc(hex.toString().trim()) + "\",");
        // Refs to a non-function address are the whole point: a DATA ref from a table
        // explains why auto-analysis never made this a function.
        w.println("      \"probe_xrefs\": [" + jsonList(xrefsTo(a)) + "]");
    }

    // Every reference to an address, including DATA (vtable / fn-pointer table).
    // Format: "<REFTYPE> 0x<from> in <containing fn or (data)>".
    private Set<String> xrefsTo(Address target) {
        Set<String> sorted = new TreeSet<>();
        try {
            ReferenceIterator it = currentProgram.getReferenceManager()
                    .getReferencesTo(target);
            while (it.hasNext()) {
                Reference r = it.next();
                Address from = r.getFromAddress();
                Function owner = currentProgram.getFunctionManager()
                        .getFunctionContaining(from);
                sorted.add(r.getReferenceType().getName() + " 0x" + from
                        + " in " + (owner != null ? owner.getName() : "(data)"));
            }
        }
        catch (Exception e) {
            sorted.add("ERROR: " + e);
        }
        return sorted;
    }

    // String literals referenced from instructions in the function body.
    private Set<String> stringsIn(Function fn) {
        Set<String> sorted = new TreeSet<>();
        try {
            Listing listing = currentProgram.getListing();
            InstructionIterator ii = listing.getInstructions(fn.getBody(), true);
            while (ii.hasNext()) {
                Instruction insn = ii.next();
                for (Reference r : insn.getReferencesFrom()) {
                    Data d = listing.getDataAt(r.getToAddress());
                    if (d != null && d.hasStringValue()) {
                        Object v = d.getValue();
                        String s = (v == null) ? "" : v.toString();
                        if (s.length() > MAX_STR_LEN) {
                            s = s.substring(0, MAX_STR_LEN) + "...(truncated)";
                        }
                        sorted.add("0x" + r.getToAddress() + ": " + s);
                    }
                }
            }
        }
        catch (Exception e) {
            sorted.add("ERROR: " + e);
        }
        return sorted;
    }

    private String jsonList(Set<String> items) {
        StringBuilder b = new StringBuilder();
        int n = 0;
        for (String s : items) {
            if (n >= MAX_LIST) {
                b.append(", \"+").append(items.size() - MAX_LIST).append(" more\"");
                break;
            }
            if (n > 0) {
                b.append(", ");
            }
            b.append('"').append(esc(s)).append('"');
            n++;
        }
        return b.toString();
    }

    private static String esc(String s) {
        if (s == null) {
            return "";
        }
        StringBuilder b = new StringBuilder();
        for (char c : s.toCharArray()) {
            switch (c) {
                case '"':  b.append("\\\""); break;
                case '\\': b.append("\\\\"); break;
                case '\n': b.append("\\n");  break;
                case '\r': b.append("\\r");  break;
                case '\t': b.append("\\t");  break;
                default:
                    if (c < 0x20) {
                        b.append(String.format("\\u%04x", (int) c));
                    }
                    else {
                        b.append(c);
                    }
            }
        }
        return b.toString();
    }
}
