// DecompBatch.java — decompile + disassemble a set of functions in ONE headless run.
//
// Headless startup costs 30-60 s, so batching is the difference between a
// 1-minute run and a 7-minute one. Emits, per VA, a single .txt containing the
// signature, the decompilation, and the raw listing (the listing is what you
// cite for byte-exact constants; the decompiler can fold or retype them).
//
// args: <va_list_file> <out_dir>
//
// The VA list is a FILE (one hex VA per line, blank lines and #comments ok),
// NOT a comma-separated argument: analyzeHeadless.bat runs under cmd.exe, where
// comma and semicolon are argument delimiters, so an inline list silently gets
// split and the second VA is consumed as out_dir.
import ghidra.app.decompiler.DecompInterface;
import ghidra.app.decompiler.DecompileResults;
import ghidra.app.script.GhidraScript;
import ghidra.program.model.address.Address;
import ghidra.program.model.listing.CodeUnit;
import ghidra.program.model.listing.Function;
import ghidra.program.model.listing.InstructionIterator;
import ghidra.program.model.listing.Instruction;
import ghidra.program.model.symbol.Reference;
import java.io.File;
import java.io.FileWriter;
import java.io.PrintWriter;

public class DecompBatch extends GhidraScript {
    @Override
    public void run() throws Exception {
        String[] args = getScriptArgs();
        if (args.length < 2) {
            throw new IllegalArgumentException("usage: DecompBatch <va_list_file> <out_dir>");
        }
        File listFile = new File(args[0]);
        if (!listFile.isFile()) {
            throw new IllegalArgumentException(
                    "arg0 must be a file with one hex VA per line, got: " + args[0]);
        }
        java.util.List<String> vas = new java.util.ArrayList<>();
        for (String line : java.nio.file.Files.readAllLines(listFile.toPath())) {
            String s = line.trim();
            if (s.isEmpty() || s.startsWith("#")) continue;
            vas.add(s);
        }
        File outDir = new File(args[1]);
        outDir.mkdirs();

        DecompInterface di = new DecompInterface();
        di.openProgram(currentProgram);

        for (String raw : vas) {
            String vaStr = raw.trim();
            if (vaStr.isEmpty()) continue;
            long va = Long.parseLong(vaStr.replaceFirst("^0x", ""), 16);
            Address a = currentProgram.getAddressFactory()
                    .getDefaultAddressSpace().getAddress(va);
            File out = new File(outDir, String.format("0x%08x.txt", va));
            PrintWriter w = new PrintWriter(new FileWriter(out));

            Function fn = currentProgram.getFunctionManager().getFunctionAt(a);
            if (fn == null) {
                // NO-GUESSING: report the miss, never round to a nearby address.
                w.println("// NO FUNCTION AT " + vaStr);
                CodeUnit cu = currentProgram.getListing().getCodeUnitContaining(a);
                if (cu != null) {
                    w.println("// containing code unit: " + cu.getMinAddress()
                            + "  " + cu);
                    Function encl = currentProgram.getFunctionManager()
                            .getFunctionContaining(a);
                    if (encl != null) {
                        w.println("// enclosing function: " + encl.getName()
                                + " @ " + encl.getEntryPoint()
                                + " body " + encl.getBody().getMinAddress()
                                + ".." + encl.getBody().getMaxAddress());
                    }
                }
                w.close();
                println("DecompBatch: MISS at " + vaStr);
                continue;
            }

            w.println("//// " + fn.getName() + " @ " + a
                    + "   body " + fn.getBody().getMinAddress()
                    + ".." + fn.getBody().getMaxAddress()
                    + "   (" + fn.getBody().getNumAddresses() + " bytes)");
            w.println("//// signature: " + fn.getSignature().getPrototypeString());
            w.println("//// calling convention: " + fn.getCallingConventionName());
            w.println();

            DecompileResults res = di.decompileFunction(fn, 120, monitor);
            w.println("//// ---- DECOMPILATION ----");
            if (res.decompileCompleted()) {
                w.println(res.getDecompiledFunction().getC());
            } else {
                w.println("// DECOMPILE FAILED: " + res.getErrorMessage());
            }

            w.println();
            w.println("//// ---- LISTING (cite constants from here) ----");
            InstructionIterator it = currentProgram.getListing()
                    .getInstructions(fn.getBody(), true);
            while (it.hasNext()) {
                Instruction ins = it.next();
                StringBuilder sb = new StringBuilder();
                sb.append(ins.getAddress()).append("  ");
                byte[] bytes = ins.getBytes();
                StringBuilder hex = new StringBuilder();
                for (byte b : bytes) hex.append(String.format("%02x ", b));
                sb.append(String.format("%-26s", hex.toString()));
                sb.append(ins.toString());
                Reference[] refs = ins.getReferencesFrom();
                for (Reference r : refs) {
                    if (r.isMemoryReference() && !r.getReferenceType().isFlow()) {
                        sb.append("   ; -> ").append(r.getToAddress());
                    }
                }
                w.println(sb.toString());
            }
            w.close();
            println("DecompBatch: " + fn.getName() + " -> " + out.getPath());
        }
        di.dispose();
    }
}
