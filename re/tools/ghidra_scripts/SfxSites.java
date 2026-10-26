// SfxSites.java - static SFX call-site census for D4-WSJ-SFXMAP.
//
// Part 1: for each target function entry given on the command line, every reference to it,
// the containing function, and the 10 instructions before the call site (so the pushed
// arguments, e.g. the permdict wave index, can be read off the listing).
// Part 2: every instruction referencing the permdict wave descriptor table
// [0x006044f0, 0x006044f0 + 43*0x94), decoded to (wave index, field offset). Stride 0x94 and
// count 43 are from verify/wsjref_20261026/RESULT_D4-WSJ-REF.md (descriptor dump).
//
// usage: analyzeHeadless <proj> <name> -process MASHED.exe -readOnly -noanalysis \
//   -scriptPath <dir> -postScript SfxSites.java <out.txt> 0x<rva> [0x<rva> ...]
import ghidra.app.script.GhidraScript;
import ghidra.program.model.address.Address;
import ghidra.program.model.listing.Function;
import ghidra.program.model.listing.Instruction;
import ghidra.program.model.listing.InstructionIterator;
import ghidra.program.model.symbol.RefType;
import ghidra.program.model.symbol.Reference;
import ghidra.program.model.symbol.ReferenceIterator;
import java.io.PrintWriter;

public class SfxSites extends GhidraScript {
    static final long PERM_LO = 0x006044f0L, PERM_STRIDE = 0x94L, PERM_N = 43;

    String fname(Address a) {
        Function f = getFunctionContaining(a);
        return f == null ? "(none)" : f.getName() + "@" + f.getEntryPoint();
    }

    @Override
    public void run() throws Exception {
        String[] args = getScriptArgs();
        PrintWriter w = new PrintWriter(args[0], "UTF-8");
        for (int i = 1; i < args.length; i++) {
            Address t = toAddr(Long.decode(args[i]));
            w.println("## target " + t + " " + fname(t));
            ReferenceIterator it = currentProgram.getReferenceManager().getReferencesTo(t);
            while (it.hasNext()) {
                Reference r = it.next();
                Address site = r.getFromAddress();
                w.println("SITE " + site + " | " + fname(site) + " | " + r.getReferenceType().getName());
                Instruction ins = getInstructionAt(site);
                Instruction[] prev = new Instruction[10];
                Instruction p = ins;
                int n = 0;
                while (p != null && n < 10) {
                    p = p.getPrevious();
                    if (p == null) break;
                    prev[n++] = p;
                }
                for (int k = n - 1; k >= 0; k--)
                    w.println("    " + prev[k].getAddress() + "  " + prev[k]);
                if (ins != null) w.println("  > " + ins.getAddress() + "  " + ins);
            }
        }
        long hi = PERM_LO + PERM_STRIDE * PERM_N;
        w.println("## permdict descriptor refs [" + Long.toHexString(PERM_LO) + "," + Long.toHexString(hi) + ")");
        InstructionIterator ii = currentProgram.getListing().getInstructions(true);
        while (ii.hasNext()) {
            Instruction ins = ii.next();
            for (Reference r : ins.getReferencesFrom()) {
                if (!r.isMemoryReference()) continue;
                RefType rt = r.getReferenceType();
                if (rt.isFlow()) continue;
                long v = r.getToAddress().getOffset();
                if (v < PERM_LO || v >= hi) continue;
                long idx = (v - PERM_LO) / PERM_STRIDE, off = (v - PERM_LO) % PERM_STRIDE;
                w.println("PERM idx=" + idx + " off=0x" + Long.toHexString(off) + " | " + ins.getAddress()
                        + " | " + fname(ins.getAddress()) + " | " + (rt.isWrite() ? "WRITE" : "read")
                        + " | " + ins);
            }
        }
        w.close();
        println("SfxSites done -> " + args[0]);
    }
}
