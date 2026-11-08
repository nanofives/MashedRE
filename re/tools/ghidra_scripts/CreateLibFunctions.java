// Create functions at library-identical addresses with a known extent (C4R-DXLIB-GHIDRAFN, 2026-11-08).
//
// usage: CreateLibFunctions.java <list.tsv> <report.tsv> [apply]
//   list.tsv    lines "0x<va> <size> [anything...]"; blank / '#' lines skipped. size = the library symbol's
//               size (libmatch: body to the next function symbol, trailing int3 fill trimmed).
//   report.tsv  per-address outcome
//   apply       create functions. Without it only the guards run (nothing is disassembled or created).
//               Run apply with analyzeHeadless -readOnly for a full rehearsal that discards every change.
//
// Guards (same family as CreateMissedFunctions.java, extended to the whole extent [va, va+size)):
//   SKIP_NOT_EXEC      va not in executable memory
//   SKIP_OVERLAP       an existing function body intersects [va, va+size)
//   SKIP_DEFINED_DATA  a defined data unit intersects [va, va+size)
//   SKIP_MID_INSTR     va falls inside an existing instruction
//   SKIP_NO_CODE       disassemble() produced no instruction at va
// After createFunction(va) (body by flow) the body is compared with [va, va+size):
//   CREATED_EXACT      body == [va, va+size)
//   CREATED_PAD        body starts at va, lies inside the extent, and the extent bytes outside it are alignment fill
//                      (isFill: 0x90 / 0xCC / mov r,r / lea r,[r+0]). The body keeps Ghidra's flow extent.
//   REJECT_LONG        body reaches outside the extent        -> function removed again
//   REJECT_SHORT       extent bytes outside the body are code -> function removed again
// A rejected address keeps the instructions disassemble() created (no function); the report says so.
// Functions are left with the default FUN_<addr> name. Nothing existing is renamed or modified.
import ghidra.app.script.GhidraScript;
import ghidra.program.model.address.Address;
import ghidra.program.model.address.AddressRange;
import ghidra.program.model.address.AddressSet;
import ghidra.program.model.address.AddressSetView;
import ghidra.program.model.listing.Function;
import ghidra.program.model.listing.Instruction;
import ghidra.program.model.listing.Listing;
import ghidra.program.model.mem.MemoryBlock;
import java.io.FileWriter;
import java.io.PrintWriter;
import java.nio.charset.StandardCharsets;
import java.nio.file.Files;
import java.nio.file.Paths;

public class CreateLibFunctions extends GhidraScript {

    @Override
    public void run() throws Exception {
        String[] args = getScriptArgs();
        if (args.length < 2) {
            throw new IllegalArgumentException("usage: CreateLibFunctions.java <list.tsv> <report.tsv> [apply]");
        }
        boolean apply = args.length > 2 && "apply".equalsIgnoreCase(args[2].trim());
        Listing listing = currentProgram.getListing();
        PrintWriter w = new PrintWriter(new FileWriter(args[1]));
        w.println("address\tsize\toutcome\tflow_size\tdetail");
        int ok = 0, skip = 0, rej = 0;
        for (String line : Files.readAllLines(Paths.get(args[0]), StandardCharsets.UTF_8)) {
            String t = line.trim();
            if (t.isEmpty() || t.startsWith("#")) {
                continue;
            }
            String[] p = t.split("\\s+");
            long va = Long.parseLong(p[0].replaceFirst("^0[xX]", ""), 16);
            int size = Integer.parseInt(p[1]);
            Address a = toAddr(va);
            Address end = a.add(size - 1);
            AddressSet ext = new AddressSet(a, end);
            String s = p[0] + "\t" + size + "\t";

            MemoryBlock blk = currentProgram.getMemory().getBlock(a);
            if (blk == null || !blk.isExecute()) {
                w.println(s + "SKIP_NOT_EXEC\t\t" + (blk == null ? "(no block)" : blk.getName()));
                skip++;
                continue;
            }
            java.util.Iterator<Function> fi = currentProgram.getFunctionManager().getFunctionsOverlapping(ext);
            if (fi.hasNext()) {
                Function f = fi.next();
                w.println(s + "SKIP_OVERLAP\t\t" + f.getName() + " @0x" + f.getEntryPoint());
                skip++;
                continue;
            }
            if (listing.getDefinedData(ext, true).hasNext()) {
                w.println(s + "SKIP_DEFINED_DATA\t\t" + listing.getDefinedData(ext, true).next().getAddress());
                skip++;
                continue;
            }
            Instruction cont = listing.getInstructionContaining(a);
            if (cont != null && !cont.getAddress().equals(a)) {
                w.println(s + "SKIP_MID_INSTR\t\tinside instruction at 0x" + cont.getAddress());
                skip++;
                continue;
            }
            if (!apply) {
                w.println(s + "WOULD_CREATE\t\t" + (listing.getInstructionAt(a) != null ? "already disassembled"
                        : "needs disassembly"));
                ok++;
                continue;
            }
            if (listing.getInstructionAt(a) == null) {
                disassemble(a);
                if (listing.getInstructionAt(a) == null) {
                    w.println(s + "SKIP_NO_CODE\t\tdisassemble() produced no instruction");
                    skip++;
                    continue;
                }
            }
            Function f = createFunction(a, null);
            if (f == null) {
                w.println(s + "SKIP_NO_CODE\t\tcreateFunction returned null");
                skip++;
                continue;
            }
            AddressSetView body = f.getBody();
            long flow = body.getNumAddresses();
            if (!ext.contains(body)) {
                AddressSetView out = body.subtract(ext);
                currentProgram.getFunctionManager().removeFunction(a);
                w.println(s + "REJECT_LONG\t" + flow + "\tbody outside extent from 0x" + out.getMinAddress()
                        + " (function removed)");
                rej++;
                continue;
            }
            AddressSetView rest = ext.subtract(body);
            boolean pad = true;
            for (AddressRange r : rest) {
                if (!isFill(r.getMinAddress(), (int) r.getLength())) {
                    pad = false;
                }
            }
            if (!pad) {
                currentProgram.getFunctionManager().removeFunction(a);
                w.println(s + "REJECT_SHORT\t" + flow + "\tnon-padding extent bytes outside the body from 0x"
                        + rest.getMinAddress() + " (function removed)");
                rej++;
                continue;
            }
            w.println(s + (rest.isEmpty() ? "CREATED_EXACT" : "CREATED_PAD") + "\t" + flow + "\t" + f.getName()
                    + (rest.isEmpty() ? "" : " pad " + rest.getNumAddresses() + " B"));
            ok++;
        }
        w.close();
        println("CreateLibFunctions: mode=" + (apply ? "APPLY" : "GUARDS") + " ok=" + ok + " skipped=" + skip
                + " rejected=" + rej + " -> " + args[1]);
    }

    // True when [a, a+n) is alignment fill only: 0x90 / 0xCC bytes, `mov r,r` (8B /r, mod 3, reg == rm), or
    // `lea r,[r+0]` (8D /r, mod 1 or 2, reg == rm, zero displacement; rm 4 needs SIB 0x24). All are no-ops; MASM
    // `align` emits the lea / mov forms (e.g. 8D A4 24 00 00 00 00, 8B FF).
    private boolean isFill(Address a, int n) throws Exception {
        byte[] b = new byte[n];
        currentProgram.getMemory().getBytes(a, b);
        int i = 0;
        while (i < n) {
            int c = b[i] & 0xff;
            if (c == 0x90 || c == 0xcc) {
                i++;
                continue;
            }
            if (i + 1 >= n) {
                return false;
            }
            int m = b[i + 1] & 0xff, mod = m >> 6, reg = (m >> 3) & 7, rm = m & 7;
            if (c == 0x8b && mod == 3 && reg == rm) {
                i += 2;
                continue;
            }
            if (c == 0x8d && (mod == 1 || mod == 2) && reg == rm) {
                int j = i + 2;
                if (rm == 4) {
                    if (j >= n || (b[j] & 0xff) != 0x24) {
                        return false;
                    }
                    j++;
                }
                int d = mod == 1 ? 1 : 4;
                if (j + d > n) {
                    return false;
                }
                for (int k = 0; k < d; k++) {
                    if (b[j + k] != 0) {
                        return false;
                    }
                }
                i = j + d;
                continue;
            }
            return false;
        }
        return true;
    }
}
