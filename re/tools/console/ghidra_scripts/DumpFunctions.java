// Dump every function's entry/extent to CSV. Arg 0 = output path.
import ghidra.app.script.GhidraScript;
import ghidra.program.model.listing.Function;
import ghidra.program.model.listing.FunctionIterator;
import java.io.FileWriter;
import java.io.PrintWriter;

public class DumpFunctions extends GhidraScript {
    @Override
    public void run() throws Exception {
        String[] args = getScriptArgs();
        if (args.length < 1) {
            throw new IllegalArgumentException("usage: DumpFunctions.java <out.csv>");
        }
        PrintWriter w = new PrintWriter(new FileWriter(args[0]));
        w.println("entry,body_end,body_size,name");
        FunctionIterator it = currentProgram.getFunctionManager().getFunctions(true);
        int n = 0;
        while (it.hasNext()) {
            Function fn = it.next();
            w.println(String.format("0x%08x,0x%08x,%d,%s",
                    fn.getEntryPoint().getOffset(),
                    fn.getBody().getMaxAddress().getOffset(),
                    fn.getBody().getNumAddresses(),
                    fn.getName()));
            n++;
        }
        w.close();
        println("DumpFunctions: " + n + " functions -> " + args[0]);
    }
}
