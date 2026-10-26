// EntryBefore.java - for a code address with no containing Ghidra function, walk back through
// the listing to the nearest preceding RET (or INT3/NOP padding run) and report the first
// instruction after it as a CANDIDATE entry. A candidate only; confirm with
// decomp_pc.py --create <entry> that the transient body covers the address.
//
// usage: analyzeHeadless ... -postScript EntryBefore.java <out.txt> 0x<site> [0x<site> ...]
import ghidra.app.script.GhidraScript;
import ghidra.program.model.address.Address;
import ghidra.program.model.listing.Instruction;
import java.io.PrintWriter;

public class EntryBefore extends GhidraScript {
    @Override
    public void run() throws Exception {
        String[] args = getScriptArgs();
        PrintWriter w = new PrintWriter(args[0], "UTF-8");
        for (int i = 1; i < args.length; i++) {
            Address a = toAddr(Long.decode(args[i]));
            Instruction ins = getInstructionContaining(a);
            Instruction last = ins;
            int n = 0;
            while (ins != null && n < 4000) {
                Instruction p = ins.getPrevious();
                if (p == null) break;
                String m = p.getMnemonicString();
                boolean gap = p.getMaxAddress().next() == null
                        || !p.getMaxAddress().next().equals(ins.getAddress());
                if (m.startsWith("RET") || m.equals("INT3") || gap) { last = ins; break; }
                ins = p;
                last = ins;
                n++;
            }
            Address e = last == null ? null : last.getAddress();
            // skip NOP/INT3 padding forward
            Instruction q = last;
            while (q != null && (q.getMnemonicString().equals("NOP") || q.getMnemonicString().equals("INT3")))
                q = q.getNext();
            if (q != null) e = q.getAddress();
            w.println(args[i] + " candidate_entry " + e + " | fn_at_entry "
                    + (getFunctionAt(e) == null ? "none" : getFunctionAt(e).getName())
                    + " | first " + (q == null ? "?" : q.toString()));
        }
        w.close();
    }
}
