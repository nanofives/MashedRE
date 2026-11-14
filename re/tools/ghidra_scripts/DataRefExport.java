// DataRefExport.java -- IMG-4-GHIDRA-DATAREF (L-149 (a) step 4).
//
// Every memory reference whose SOURCE lies in one of the file-backed image ranges and whose DESTINATION lies in a
// code block (.text / _rwcseg), with Ghidra's data type at the source. Spec: RESULT_PORTSTATE-IMAGE.md section 3
// ("Sources of the list", item 2). Read-only: run against a pool clone with -readOnly -noanalysis.
//
// args: <out_tsv> <lo1> <hi1> [<lo2> <hi2> ...]   (hi exclusive, hex with or without 0x)
//
// Output columns (tab separated):
//   slot        source address of the reference (the dword the loader would rewrite)
//   target      destination address (original VA in code)
//   ref_type    Ghidra RefType name
//   data_type   data type of the defined data containing the source ("undefined" when none)
//   data_start  start of that defined data (the slot may be a component of a struct / array)
//   component   path of the component at the slot inside that data ("-" when the data is the slot itself)
//   src_label   primary symbol at data_start ("-" when none)
//   target_fn   function whose ENTRY is the target ("-" when the target is not a function entry)
//   target_in   function CONTAINING the target ("-" when none)
//   target_kind instruction | data | undefined (what the listing holds at the target)
//   dword       the 4 bytes at the slot as a little-endian dword (to compare with target)
import ghidra.app.script.GhidraScript;
import ghidra.program.model.address.Address;
import ghidra.program.model.address.AddressSet;
import ghidra.program.model.address.AddressSpace;
import ghidra.program.model.data.DataType;
import ghidra.program.model.listing.Data;
import ghidra.program.model.listing.Function;
import ghidra.program.model.listing.FunctionManager;
import ghidra.program.model.listing.Listing;
import ghidra.program.model.mem.MemoryBlock;
import ghidra.program.model.symbol.Reference;
import ghidra.program.model.symbol.ReferenceManager;
import ghidra.program.model.symbol.Symbol;
import java.io.FileWriter;
import java.io.PrintWriter;
import java.util.ArrayList;
import java.util.Collections;
import java.util.List;

public class DataRefExport extends GhidraScript {
    private long hex(String s) { return Long.parseLong(s.replaceFirst("^0x", ""), 16); }

    @Override
    public void run() throws Exception {
        String[] args = getScriptArgs();
        if (args.length < 3 || (args.length - 1) % 2 != 0) {
            throw new IllegalArgumentException("usage: DataRefExport <out> <lo> <hi> [<lo> <hi> ...]");
        }
        AddressSpace sp = currentProgram.getAddressFactory().getDefaultAddressSpace();
        AddressSet src = new AddressSet();
        for (int i = 1; i < args.length; i += 2) {
            src.addRange(sp.getAddress(hex(args[i])), sp.getAddress(hex(args[i + 1]) - 1));
        }
        Listing listing = currentProgram.getListing();
        FunctionManager fm = currentProgram.getFunctionManager();
        ReferenceManager rm = currentProgram.getReferenceManager();

        List<String> rows = new ArrayList<>();
        int skippedNonCode = 0;
        var it = rm.getReferenceSourceIterator(src, true);
        while (it.hasNext()) {
            Address from = it.next();
            for (Reference r : rm.getReferencesFrom(from)) {
                if (!r.isMemoryReference()) continue;
                Address to = r.getToAddress();
                MemoryBlock b = currentProgram.getMemory().getBlock(to);
                if (b == null || !b.isExecute()) { ++skippedNonCode; continue; }
                Data d = listing.getDataContaining(from);
                String dtName = "undefined", dStart = "-", comp = "-", label = "-";
                if (d != null) {
                    Data leaf = d.getPrimitiveAt((int) from.subtract(d.getAddress()));
                    DataType dt = (leaf != null ? leaf : d).getDataType();
                    dtName = dt.getDisplayName();
                    dStart = d.getAddress().toString();
                    if (leaf != null && leaf != d) comp = leaf.getPathName();
                    Symbol s = currentProgram.getSymbolTable().getPrimarySymbol(d.getAddress());
                    if (s != null) label = s.getName();
                }
                Function fe = fm.getFunctionAt(to);
                Function fc = fm.getFunctionContaining(to);
                String kind = listing.getInstructionAt(to) != null ? "instruction"
                        : (listing.getDefinedDataContaining(to) != null ? "data" : "undefined");
                long dword = 0;
                try { dword = currentProgram.getMemory().getInt(from) & 0xffffffffL; } catch (Exception e) { dword = -1; }
                rows.add(String.format("%08x\t%08x\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%08x",
                        from.getOffset(), to.getOffset(), r.getReferenceType().getName(), dtName, dStart, comp, label,
                        fe == null ? "-" : fe.getName(), fc == null ? "-" : fc.getName() + "@" + fc.getEntryPoint(),
                        kind, dword));
            }
        }
        Collections.sort(rows);
        PrintWriter w = new PrintWriter(new FileWriter(args[0]));
        w.println("slot\ttarget\tref_type\tdata_type\tdata_start\tcomponent\tsrc_label\ttarget_fn\ttarget_in\ttarget_kind\tdword");
        for (String row : rows) w.println(row);
        w.close();
        println("DataRefExport: " + rows.size() + " refs into code, " + skippedNonCode + " refs to non-code skipped -> " + args[0]);
    }
}
