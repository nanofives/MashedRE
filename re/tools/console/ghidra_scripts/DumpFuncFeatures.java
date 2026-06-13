// Dump per-function matching features to CSV. Arg 0 = output path.
// Columns: entry,size,nmnem,mnemhash,strings,callees
//   mnemhash = sha1 of the concatenated mnemonic sequence (operands dropped)
//   strings  = |-joined ASCII strings referenced from the body (read raw from
//              memory so it works on raw BinaryLoader imports too)
//   callees  = ;-joined entry VAs of direct CALL targets, in call-site order
import ghidra.app.script.GhidraScript;
import ghidra.program.model.address.Address;
import ghidra.program.model.listing.Function;
import ghidra.program.model.listing.FunctionIterator;
import ghidra.program.model.listing.Instruction;
import ghidra.program.model.listing.InstructionIterator;
import ghidra.program.model.mem.Memory;
import ghidra.program.model.symbol.Reference;
import java.io.FileWriter;
import java.io.PrintWriter;
import java.security.MessageDigest;
import java.util.ArrayList;
import java.util.LinkedHashSet;

public class DumpFuncFeatures extends GhidraScript {
    @Override
    public void run() throws Exception {
        String[] args = getScriptArgs();
        if (args.length < 1) {
            throw new IllegalArgumentException("usage: DumpFuncFeatures.java <out.csv>");
        }
        Memory mem = currentProgram.getMemory();
        MessageDigest md = MessageDigest.getInstance("SHA-1");
        PrintWriter w = new PrintWriter(new FileWriter(args[0]));
        w.println("entry,size,nmnem,mnemhash,strings,callees");
        FunctionIterator it = currentProgram.getFunctionManager().getFunctions(true);
        int n = 0;
        while (it.hasNext()) {
            Function fn = it.next();
            StringBuilder mnems = new StringBuilder();
            LinkedHashSet<String> strs = new LinkedHashSet<>();
            ArrayList<String> callees = new ArrayList<>();
            int nm = 0;
            InstructionIterator ii =
                currentProgram.getListing().getInstructions(fn.getBody(), true);
            while (ii.hasNext()) {
                Instruction ins = ii.next();
                mnems.append(ins.getMnemonicString()).append(' ');
                nm++;
                for (Reference r : ins.getReferencesFrom()) {
                    Address to = r.getToAddress();
                    if (r.getReferenceType().isCall()) {
                        Function callee =
                            currentProgram.getFunctionManager().getFunctionAt(to);
                        if (callee != null && !callee.isExternal()) {
                            callees.add(String.format("%x", to.getOffset()));
                        }
                    }
                    else if (r.getReferenceType().isData() && mem.contains(to)) {
                        String s = readAscii(mem, to);
                        if (s != null) {
                            strs.add(s);
                        }
                    }
                }
            }
            md.reset();
            byte[] h = md.digest(mnems.toString().getBytes("UTF-8"));
            StringBuilder hx = new StringBuilder();
            for (byte b : h) {
                hx.append(String.format("%02x", b));
            }
            w.println(String.format("0x%08x,%d,%d,%s,\"%s\",\"%s\"",
                    fn.getEntryPoint().getOffset(),
                    fn.getBody().getNumAddresses(), nm, hx,
                    String.join("|", strs).replace("\"", "'"),
                    String.join(";", callees)));
            n++;
        }
        w.close();
        println("DumpFuncFeatures: " + n + " functions -> " + args[0]);
    }

    // Read a printable ASCII string (>=6 chars, <=48 kept) at addr, else null.
    private String readAscii(Memory mem, Address a) {
        try {
            byte[] buf = new byte[48];
            int got = mem.getBytes(a, buf);
            StringBuilder sb = new StringBuilder();
            for (int i = 0; i < got; i++) {
                byte b = buf[i];
                if (b == 0) {
                    break;
                }
                if (b < 0x20 || b > 0x7e) {
                    return null;
                }
                sb.append((char) b);
            }
            if (sb.length() >= 6) {
                return sb.toString();
            }
        }
        catch (Exception e) {
            // unreadable -> not a string
        }
        return null;
    }
}
