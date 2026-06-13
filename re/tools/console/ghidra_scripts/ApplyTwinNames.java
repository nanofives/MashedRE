// Apply PC-side names onto matched Xbox functions. Arg 0 = xbuild_match_v2.csv.
// Semantic PC names transfer as-is; anonymous PC functions get pc_<va> so the
// cross-reference is visible either way. Tier + PC VA recorded in the plate.
import ghidra.app.script.GhidraScript;
import ghidra.program.model.address.Address;
import ghidra.program.model.listing.Function;
import ghidra.program.model.symbol.SourceType;
import java.io.BufferedReader;
import java.io.FileReader;

public class ApplyTwinNames extends GhidraScript {
    @Override
    public void run() throws Exception {
        String[] args = getScriptArgs();
        if (args.length < 1) {
            throw new IllegalArgumentException("usage: ApplyTwinNames.java <match.csv>");
        }
        BufferedReader r = new BufferedReader(new FileReader(args[0]));
        String line = r.readLine(); // header
        int renamed = 0, missing = 0;
        while ((line = r.readLine()) != null) {
            String[] c = line.split(",", -1);
            if (c.length < 6) {
                continue;
            }
            long pcVa = Long.parseLong(c[0].substring(2), 16);
            long xbVa = Long.parseLong(c[1].substring(2), 16);
            String tier = c[2];
            String pcName = c[3];
            Address a = currentProgram.getAddressFactory()
                    .getDefaultAddressSpace().getAddress(xbVa);
            Function fn = currentProgram.getFunctionManager().getFunctionAt(a);
            if (fn == null) {
                missing++;
                continue;
            }
            String name = (!pcName.isEmpty() && !pcName.startsWith("FUN_")
                    && !pcName.startsWith("sub_") && !pcName.startsWith("LAB_"))
                            ? pcName
                            : String.format("pc_%08x", pcVa);
            try {
                fn.setName(name, SourceType.IMPORTED);
            }
            catch (Exception e) {
                try {
                    fn.setName(String.format("%s_%08x", name, pcVa),
                            SourceType.IMPORTED);
                }
                catch (Exception e2) {
                    // keep existing name
                }
            }
            String plate = String.format("[xbuild] PC twin 0x%08x tier=%s", pcVa, tier);
            String old = fn.getComment();
            fn.setComment(old == null ? plate
                    : old.contains("[xbuild]") ? old : old + "\n" + plate);
            renamed++;
        }
        r.close();
        println("ApplyTwinNames: " + renamed + " renamed, " + missing + " missing");
    }
}
