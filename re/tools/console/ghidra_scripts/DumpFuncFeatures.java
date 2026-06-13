// Dump per-function matching features to CSV. Arg 0 = output path.
// Columns: entry,size,nmnem,mnemhash,strings,callees,imms,datafps
//   mnemhash = sha1 of the concatenated mnemonic sequence (operands dropped)
//   strings  = |-joined ASCII strings referenced from the body (read raw from
//              memory so it works on raw BinaryLoader imports too)
//   callees  = ;-joined entry VAs of direct CALL targets, in call-site order
//   imms     = |-joined unusual scalar operands (>=0x10000, not in-image —
//              float bit patterns, magic constants; survive relinking)
//   datafps  = |-joined 8-byte hex fingerprints of referenced non-string,
//              non-zero initialized data (tables keep their bytes per-source)
import ghidra.app.script.GhidraScript;
import ghidra.program.model.address.Address;
import ghidra.program.model.listing.Function;
import ghidra.program.model.listing.FunctionIterator;
import ghidra.program.model.listing.Instruction;
import ghidra.program.model.listing.InstructionIterator;
import ghidra.program.model.mem.Memory;
import ghidra.program.model.scalar.Scalar;
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
        long imgLo = currentProgram.getMinAddress().getOffset();
        long imgHi = currentProgram.getMaxAddress().getOffset();
        MessageDigest md = MessageDigest.getInstance("SHA-1");
        PrintWriter w = new PrintWriter(new FileWriter(args[0]));
        w.println("entry,size,nmnem,mnemhash,strings,callees,imms,datafps");
        FunctionIterator it = currentProgram.getFunctionManager().getFunctions(true);
        int n = 0;
        while (it.hasNext()) {
            Function fn = it.next();
            StringBuilder mnems = new StringBuilder();
            LinkedHashSet<String> strs = new LinkedHashSet<>();
            LinkedHashSet<String> imms = new LinkedHashSet<>();
            LinkedHashSet<String> fps = new LinkedHashSet<>();
            ArrayList<String> callees = new ArrayList<>();
            int nm = 0;
            InstructionIterator ii =
                currentProgram.getListing().getInstructions(fn.getBody(), true);
            while (ii.hasNext()) {
                Instruction ins = ii.next();
                mnems.append(ins.getMnemonicString()).append(' ');
                nm++;
                for (int op = 0; op < ins.getNumOperands(); op++) {
                    for (Object o : ins.getOpObjects(op)) {
                        if (o instanceof Scalar) {
                            long v = ((Scalar) o).getUnsignedValue() & 0xFFFFFFFFL;
                            if (v >= 0x10000 && v != 0xFFFFFFFFL
                                    && !(v >= imgLo && v <= imgHi)) {
                                imms.add(String.format("%x", v));
                            }
                        }
                    }
                }
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
                        else {
                            String fp = dataFingerprint(mem, to);
                            if (fp != null) {
                                fps.add(fp);
                            }
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
            w.println(String.format("0x%08x,%d,%d,%s,\"%s\",\"%s\",\"%s\",\"%s\"",
                    fn.getEntryPoint().getOffset(),
                    fn.getBody().getNumAddresses(), nm, hx,
                    String.join("|", strs).replace("\"", "'"),
                    String.join(";", callees),
                    String.join("|", imms),
                    String.join("|", fps)));
            n++;
        }
        w.close();
        println("DumpFuncFeatures: " + n + " functions -> " + args[0]);
    }

    // 8-byte hex fingerprint of initialized, non-zero data at addr, else null.
    private String dataFingerprint(Memory mem, Address a) {
        try {
            byte[] buf = new byte[8];
            if (mem.getBytes(a, buf) != 8) {
                return null;
            }
            boolean nonzero = false;
            StringBuilder sb = new StringBuilder();
            for (byte b : buf) {
                nonzero |= b != 0;
                sb.append(String.format("%02x", b));
            }
            return nonzero ? sb.toString() : null;
        }
        catch (Exception e) {
            return null;
        }
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
