// Decompile one function to a file. Arg 0 = VA (hex, 0x ok), arg 1 = out path.
import ghidra.app.decompiler.DecompInterface;
import ghidra.app.decompiler.DecompileResults;
import ghidra.app.script.GhidraScript;
import ghidra.program.model.address.Address;
import ghidra.program.model.listing.Function;
import java.io.FileWriter;
import java.io.PrintWriter;

public class DecompTwin extends GhidraScript {
    @Override
    public void run() throws Exception {
        String[] args = getScriptArgs();
        if (args.length < 2) {
            throw new IllegalArgumentException("usage: DecompTwin.java <va> <out.c>");
        }
        long va = Long.parseLong(args[0].replaceFirst("^0x", ""), 16);
        Address a = currentProgram.getAddressFactory()
                .getDefaultAddressSpace().getAddress(va);
        Function fn = currentProgram.getFunctionManager().getFunctionAt(a);
        if (fn == null) {
            throw new IllegalStateException("no function at " + args[0]);
        }
        DecompInterface di = new DecompInterface();
        di.openProgram(currentProgram);
        DecompileResults res = di.decompileFunction(fn, 90, monitor);
        PrintWriter w = new PrintWriter(new FileWriter(args[1]));
        if (res.decompileCompleted()) {
            w.println("// " + fn.getName() + " @ " + a + " ("
                    + currentProgram.getName() + ")");
            w.println(res.getDecompiledFunction().getC());
        }
        else {
            w.println("// DECOMPILE FAILED: " + res.getErrorMessage());
        }
        w.close();
        di.dispose();
        println("DecompTwin: " + fn.getName() + " -> " + args[1]);
    }
}
