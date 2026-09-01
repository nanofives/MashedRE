// CallersPC.java — list the CALL sites that reach a function, with each caller's entry point.
//
// Why this exists: XrefRange.java scans instruction operand references and answers
// "who touches this DATA address". It does NOT answer "who calls this FUNCTION" — a
// range scan over a code range returns 0 refs (verified 2026-09-01 on 0x0040d020 /
// 0x0040d440). The C3 gate in re/CONFIDENCE.md requires "at least one caller AND one
// callee at C2+", and the analysis plates record callees_depth1 only, so the caller half
// of the gate had no tool behind it. This is that tool.
//
// Reports BOTH directions of reachability so an indirect-only callee is visible rather
// than silently reported as uncalled: direct CALL/JMP references, plus any other
// reference type landing on the entry point (e.g. a function pointer stored into a table).
//
// usage: analyzeHeadless <proj_dir> <proj_name> -process MASHED.exe -readOnly \
//   -scriptPath <dir> -postScript CallersPC.java 0x<rva> <out.txt>
import ghidra.app.script.GhidraScript;
import ghidra.program.model.address.Address;
import ghidra.program.model.listing.Function;
import ghidra.program.model.symbol.Reference;
import ghidra.program.model.symbol.ReferenceIterator;
import ghidra.program.model.symbol.RefType;
import java.io.PrintWriter;

public class CallersPC extends GhidraScript {
    @Override
    public void run() throws Exception {
        String[] args = getScriptArgs();
        if (args.length < 2) { println("need <rva> <out.txt>"); return; }
        long rva = Long.decode(args[0]);
        Address target = toAddr(rva);
        Function fn = getFunctionContaining(target);
        Address entry = (fn != null) ? fn.getEntryPoint() : target;

        PrintWriter out = new PrintWriter(args[1], "UTF-8");
        out.println("# callers of " + (fn != null ? fn.getName() : "?") + " @ " + entry);
        out.println("# site | caller function | caller entry | ref type");

        int direct = 0, other = 0;
        ReferenceIterator it = currentProgram.getReferenceManager().getReferencesTo(entry);
        while (it.hasNext()) {
            Reference r = it.next();
            Address site = r.getFromAddress();
            RefType t = r.getReferenceType();
            Function cf = getFunctionContaining(site);
            String cname = (cf != null) ? cf.getName() : "(no function)";
            String centry = (cf != null) ? cf.getEntryPoint().toString() : "-";
            out.println(site + " | " + cname + " | " + centry + " | " + t.getName());
            if (t.isCall() || t.isJump()) direct++; else other++;
        }
        out.println("# direct call/jump refs: " + direct + "   other refs (incl. table/ptr): " + other);
        out.close();
        println("CallersPC: " + (direct + other) + " refs (" + direct + " call/jump) -> " + args[1]);
    }
}
