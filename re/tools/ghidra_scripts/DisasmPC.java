// DisasmPC.java - dump raw disassembly (addr : bytes : mnemonic) for one function.
// usage: analyzeHeadless <proj> <name> -process MASHED.exe -readOnly \
//   -scriptPath <dir> -postScript DisasmPC.java 0x<rva> <out.txt>
// Prints every instruction of the function CONTAINING <rva>. No MCP needed.
import ghidra.app.script.GhidraScript;
import ghidra.program.model.address.Address;
import ghidra.program.model.listing.Function;
import ghidra.program.model.listing.Instruction;
import ghidra.program.model.listing.Listing;
import java.io.PrintWriter;

public class DisasmPC extends GhidraScript {
    @Override
    public void run() throws Exception {
        String[] args = getScriptArgs();
        if (args.length < 2) { println("need <rva> <out.txt>"); return; }
        long rva = Long.decode(args[0]);
        Address a = toAddr(rva);
        Listing lst = currentProgram.getListing();
        Function fn = getFunctionContaining(a);
        Address start = (fn != null) ? fn.getEntryPoint() : a;
        Address end = (fn != null) ? fn.getBody().getMaxAddress() : a.add(0x400);
        PrintWriter out = new PrintWriter(args[1], "UTF-8");
        out.println("# function " + (fn != null ? fn.getName() : "?") + " @ " + start);
        Instruction ins = lst.getInstructionAt(start);
        while (ins != null && ins.getAddress().compareTo(end) <= 0) {
            StringBuilder b = new StringBuilder();
            byte[] by = ins.getBytes();
            for (byte x : by) b.append(String.format("%02x", x & 0xff));
            out.println(ins.getAddress() + "  " + String.format("%-16s", b.toString()) + "  " + ins.toString());
            ins = ins.getNext();
        }
        out.close();
        println("DisasmPC: wrote " + args[1]);
    }
}
