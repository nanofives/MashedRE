// Enumerate .text addresses that are REFERENCED but have no containing function.
//
// usage: UnanalyzedRefs.java <out.tsv>
//
// Ghidra auto-analysis missed a set of real functions in MASHED.exe: the binary calls
// or references them, but no Function was created, so they return "no function at or
// containing" and their callees show an empty call graph. Several uncertainty rows drew
// conclusions from that silence (see re/analysis/ghidra_unanalyzed_text_regions_20260911.md).
//
// This measures the gap instead of guessing at it. For every reference in the program
// whose TARGET lands in an executable block with no containing function, emit one row:
//
//   target  block  refType  isCall  fromAddr  fromFunction
//
// Read-only: it creates nothing and changes nothing.
import ghidra.app.script.GhidraScript;
import ghidra.program.model.address.Address;
import ghidra.program.model.address.AddressIterator;
import ghidra.program.model.listing.Function;
import ghidra.program.model.mem.MemoryBlock;
import ghidra.program.model.symbol.Reference;
import ghidra.program.model.symbol.ReferenceIterator;
import ghidra.program.model.symbol.ReferenceManager;
import java.io.FileWriter;
import java.io.PrintWriter;
import java.util.Set;
import java.util.TreeSet;

public class UnanalyzedRefs extends GhidraScript {

    @Override
    public void run() throws Exception {
        String[] args = getScriptArgs();
        if (args.length < 1) {
            throw new IllegalArgumentException("usage: UnanalyzedRefs.java <out.tsv>");
        }

        ReferenceManager rm = currentProgram.getReferenceManager();
        PrintWriter w = new PrintWriter(new FileWriter(args[0]));
        w.println("target\tblock\trefType\tisCall\tfrom\tfromFunction");

        Set<String> distinct = new TreeSet<>();
        int rows = 0, calls = 0;

        // Iterate every address that is the destination of at least one reference.
        AddressIterator it = rm.getReferenceDestinationIterator(
                currentProgram.getMemory(), true);
        while (it.hasNext()) {
            if (monitor.isCancelled()) {
                break;
            }
            Address target = it.next();

            MemoryBlock blk = currentProgram.getMemory().getBlock(target);
            if (blk == null || !blk.isExecute()) {
                continue;   // only executable memory can hold a missed function
            }
            if (currentProgram.getFunctionManager().getFunctionContaining(target) != null) {
                continue;   // already inside a known function - not a gap
            }

            distinct.add(target.toString());
            ReferenceIterator ri = rm.getReferencesTo(target);
            while (ri.hasNext()) {
                Reference r = ri.next();
                boolean isCall = r.getReferenceType().isCall();
                if (isCall) {
                    calls++;
                }
                Function owner = currentProgram.getFunctionManager()
                        .getFunctionContaining(r.getFromAddress());
                w.println("0x" + target + "\t" + blk.getName() + "\t"
                        + r.getReferenceType().getName() + "\t" + isCall + "\t"
                        + "0x" + r.getFromAddress() + "\t"
                        + (owner != null ? owner.getName() : "(no function)"));
                rows++;
            }
        }
        w.close();
        println("UnanalyzedRefs: " + distinct.size() + " distinct referenced addresses in "
                + "executable memory have NO containing function; " + rows
                + " references to them, of which " + calls + " are CALLs -> " + args[0]);
    }
}
