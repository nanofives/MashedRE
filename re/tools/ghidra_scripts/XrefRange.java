// XrefRange.java — every instruction referencing any address in [lo,hi], with its
// containing function and read/write direction.
//
// Built for per-player struct fields: a consumer usually addresses them as
// base + player*stride + field, so a symbol-level xref on one DAT_ misses the
// sites that reach the field through a computed index. Scanning the whole
// instruction stream for references landing in a range catches both.
//
// args: <lo_hex> <hi_hex> <out_file>
import ghidra.app.script.GhidraScript;
import ghidra.program.model.address.Address;
import ghidra.program.model.listing.Function;
import ghidra.program.model.listing.Instruction;
import ghidra.program.model.listing.InstructionIterator;
import ghidra.program.model.symbol.RefType;
import ghidra.program.model.symbol.Reference;
import java.io.FileWriter;
import java.io.PrintWriter;

public class XrefRange extends GhidraScript {
    @Override
    public void run() throws Exception {
        String[] args = getScriptArgs();
        if (args.length < 3) {
            throw new IllegalArgumentException("usage: XrefRange <lo> <hi> <out>");
        }
        long lo = Long.parseLong(args[0].replaceFirst("^0x", ""), 16);
        long hi = Long.parseLong(args[1].replaceFirst("^0x", ""), 16);
        PrintWriter w = new PrintWriter(new FileWriter(args[2]));
        w.println("# refs into [" + args[0] + ".." + args[1] + "]");
        w.println("# target | site | function | dir | instruction");

        int n = 0;
        InstructionIterator it = currentProgram.getListing().getInstructions(true);
        while (it.hasNext()) {
            Instruction ins = it.next();
            for (Reference r : ins.getReferencesFrom()) {
                if (!r.isMemoryReference()) continue;
                RefType rt = r.getReferenceType();
                if (rt.isFlow()) continue;
                Address to = r.getToAddress();
                long v = to.getOffset();
                if (v < lo || v > hi) continue;
                Function fn = currentProgram.getFunctionManager()
                        .getFunctionContaining(ins.getAddress());
                String dir = rt.isWrite() ? "WRITE" : (rt.isRead() ? "read" : rt.getName());
                w.println(to + " | " + ins.getAddress() + " | "
                        + (fn == null ? "(none)" : fn.getName() + "@" + fn.getEntryPoint())
                        + " | " + dir + " | " + ins.toString());
                n++;
            }
        }
        w.close();
        println("XrefRange: " + n + " refs -> " + args[2]);
    }
}
