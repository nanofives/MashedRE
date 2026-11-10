// FuncRangesPC.java - C4R-AC80-EXTENT (2026-11-07): dump a function's full body address set (every range, not
// only min/max), each instruction in it, the function owning each probe address, and the raw bytes of a window.
// usage: analyzeHeadless <proj> <name> -process MASHED.exe -readOnly -noanalysis \
//   -scriptPath <dir> -postScript FuncRangesPC.java <out.txt> <entry_hex> <win_lo_hex> <win_hi_hex> [probe_hex ...]
import ghidra.app.script.GhidraScript;
import ghidra.program.model.address.Address;
import ghidra.program.model.address.AddressRange;
import ghidra.program.model.address.AddressSetView;
import ghidra.program.model.listing.CodeUnit;
import ghidra.program.model.listing.Function;
import ghidra.program.model.listing.Instruction;
import ghidra.program.model.listing.InstructionIterator;
import java.io.PrintWriter;

public class FuncRangesPC extends GhidraScript {
    private Address a(String h) { return toAddr(Long.parseLong(h.replace("0x", ""), 16)); }

    @Override
    public void run() throws Exception {
        String[] args = getScriptArgs();
        PrintWriter out = new PrintWriter(args[0], "UTF-8");
        Address entry = a(args[1]);
        Function f = currentProgram.getFunctionManager().getFunctionAt(entry);
        out.println("function " + (f == null ? "NONE" : f.getName()) + " entry " + entry);
        if (f != null) {
            AddressSetView body = f.getBody();
            out.println("body_min " + body.getMinAddress() + " body_max " + body.getMaxAddress()
                    + " num_addresses " + body.getNumAddresses() + " num_ranges " + body.getNumAddressRanges());
            for (AddressRange r : body) {
                out.println("range " + r.getMinAddress() + " " + r.getMaxAddress() + " len " + r.getLength());
            }
            InstructionIterator it = currentProgram.getListing().getInstructions(body, true);
            while (it.hasNext()) {
                Instruction ins = it.next();
                byte[] b = ins.getBytes();
                StringBuilder sb = new StringBuilder();
                for (byte x : b) sb.append(String.format("%02x", x & 0xff));
                out.println("insn " + ins.getAddress() + " " + sb + " " + ins);
            }
        }
        Address lo = a(args[2]), hi = a(args[3]);
        // every function entry in the window, and the code unit kind at each address of the window
        for (Function g : currentProgram.getFunctionManager().getFunctions(lo, true)) {
            if (g.getEntryPoint().compareTo(hi) > 0) break;
            out.println("func_entry_in_window " + g.getEntryPoint() + " " + g.getName()
                    + " body " + g.getBody().getMinAddress() + ".." + g.getBody().getMaxAddress()
                    + " n=" + g.getBody().getNumAddresses());
        }
        Address cur = lo;
        while (cur.compareTo(hi) <= 0) {
            CodeUnit cu = currentProgram.getListing().getCodeUnitContaining(cur);
            Function own = currentProgram.getFunctionManager().getFunctionContaining(cur);
            byte v = currentProgram.getMemory().getByte(cur);
            String kind = cu == null ? "none" : (cu instanceof Instruction ? "insn" : "data");
            out.println(String.format("byte %s %02x %s owner=%s", cur, v & 0xff, kind,
                    own == null ? "-" : own.getEntryPoint().toString()));
            cur = cur.add(1);
        }
        for (int i = 4; i < args.length; i++) {
            Address p = a(args[i]);
            Function own = currentProgram.getFunctionManager().getFunctionContaining(p);
            Function at = currentProgram.getFunctionManager().getFunctionAt(p);
            out.println("probe " + p + " function_at=" + (at == null ? "-" : at.getName())
                    + " containing=" + (own == null ? "-" : own.getName() + "@" + own.getEntryPoint()));
        }
        out.close();
    }
}
