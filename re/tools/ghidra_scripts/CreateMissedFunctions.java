// Create functions at addresses Ghidra's auto-analysis missed.
//
// usage: CreateMissedFunctions.java <addrs.txt> <report.tsv> [apply]
//   addrs.txt   one VA per line (hex, 0x optional); blank / '#' lines skipped
//   report.tsv  per-address outcome
//   apply       WRITE MODE. Without it the script only reports what it would do.
//
// Context: UnanalyzedRefs.java found 4,612 referenced .text addresses with no containing
// function. The actionable subset is the ones whose ADDRESS IS TAKEN by analyzed code -
// function pointers being installed that were never turned into functions. See
// re/analysis/ghidra_unanalyzed_text_regions_20260911.md.
//
// This is a MASTER-PROJECT WRITE. It refuses an address unless every check passes, because
// creating a function over data or mid-instruction is worse than leaving the gap:
//
//   SKIP_NOT_EXEC      not in executable memory
//   SKIP_HAS_FUNCTION  already inside a function (nothing missing)
//   SKIP_MID_INSTR     falls inside an existing instruction - a bad address, not an entry
//   SKIP_NO_CODE       could not be disassembled into an instruction
//   SKIP_DEFINED_DATA  Ghidra has a data definition here; treating it as code is a guess
//   CREATED / FAILED
//
// Disassembly is attempted only when the address has no instruction yet, and only at the
// exact address. Nothing is renamed and no existing function is modified.
import ghidra.app.script.GhidraScript;
import ghidra.program.model.address.Address;
import ghidra.program.model.address.AddressSet;
import ghidra.program.model.listing.Data;
import ghidra.program.model.listing.Function;
import ghidra.program.model.listing.Instruction;
import ghidra.program.model.listing.Listing;
import ghidra.program.model.mem.MemoryBlock;
import ghidra.program.model.symbol.SourceType;
import java.io.FileWriter;
import java.io.PrintWriter;
import java.nio.charset.StandardCharsets;
import java.nio.file.Files;
import java.nio.file.Paths;
import java.util.ArrayList;
import java.util.List;

public class CreateMissedFunctions extends GhidraScript {

    @Override
    public void run() throws Exception {
        String[] args = getScriptArgs();
        if (args.length < 2) {
            throw new IllegalArgumentException(
                    "usage: CreateMissedFunctions.java <addrs.txt> <report.tsv> [apply]");
        }
        boolean apply = args.length > 2 && "apply".equalsIgnoreCase(args[2].trim());

        List<String> raw = new ArrayList<>();
        for (String line : Files.readAllLines(Paths.get(args[0]), StandardCharsets.UTF_8)) {
            String t = line.trim();
            if (!t.isEmpty() && !t.startsWith("#")) {
                raw.add(t);
            }
        }

        Listing listing = currentProgram.getListing();
        PrintWriter w = new PrintWriter(new FileWriter(args[1]));
        w.println("address\toutcome\tdetail");

        int created = 0, skipped = 0, failed = 0;

        for (String s : raw) {
            if (monitor.isCancelled()) {
                break;
            }
            long va = Long.parseLong(s.replaceFirst("^0[xX]", ""), 16);
            Address a = currentProgram.getAddressFactory()
                    .getDefaultAddressSpace().getAddress(va);

            MemoryBlock blk = currentProgram.getMemory().getBlock(a);
            if (blk == null || !blk.isExecute()) {
                w.println(s + "\tSKIP_NOT_EXEC\t" + (blk == null ? "(no block)" : blk.getName()));
                skipped++;
                continue;
            }
            Function existing = currentProgram.getFunctionManager().getFunctionContaining(a);
            if (existing != null) {
                w.println(s + "\tSKIP_HAS_FUNCTION\t" + existing.getName()
                        + " @0x" + existing.getEntryPoint());
                skipped++;
                continue;
            }
            Data d = listing.getDefinedDataAt(a);
            if (d != null) {
                w.println(s + "\tSKIP_DEFINED_DATA\t" + d.getDataType().getName());
                skipped++;
                continue;
            }
            Instruction containing = listing.getInstructionContaining(a);
            if (containing != null && !containing.getAddress().equals(a)) {
                w.println(s + "\tSKIP_MID_INSTR\tinside instruction at 0x"
                        + containing.getAddress());
                skipped++;
                continue;
            }

            if (!apply) {
                Instruction at = listing.getInstructionAt(a);
                w.println(s + "\tWOULD_CREATE\t"
                        + (at != null ? "already disassembled: " + at.toString()
                                      : "needs disassembly"));
                created++;
                continue;
            }

            // WRITE PATH
            if (listing.getInstructionAt(a) == null) {
                disassemble(a);
                if (listing.getInstructionAt(a) == null) {
                    w.println(s + "\tSKIP_NO_CODE\tdisassemble() produced no instruction");
                    skipped++;
                    continue;
                }
            }
            Function f = createFunction(a, null);   // null = let Ghidra name it FUN_<addr>
            if (f == null) {
                w.println(s + "\tFAILED\tcreateFunction returned null");
                failed++;
            }
            else {
                w.println(s + "\tCREATED\t" + f.getName() + "\tsize="
                        + f.getBody().getNumAddresses());
                created++;
            }
        }
        w.close();
        println("CreateMissedFunctions: mode=" + (apply ? "APPLY" : "DRY-RUN")
                + "  requested=" + raw.size()
                + "  " + (apply ? "created=" : "would_create=") + created
                + "  skipped=" + skipped + "  failed=" + failed + " -> " + args[1]);
    }
}
