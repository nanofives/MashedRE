// Seed function starts in the flattened toast.exe XBE image (raw BinaryLoader
// import gets no entry points). Runs as -preScript so auto-analysis can
// propagate from the seeds. .text range and entry VA come from toast_flat.json.
import ghidra.app.cmd.disassemble.DisassembleCommand;
import ghidra.app.cmd.function.CreateFunctionCmd;
import ghidra.app.script.GhidraScript;
import ghidra.program.model.address.Address;
import ghidra.program.model.address.AddressSpace;
import ghidra.program.model.mem.Memory;
import ghidra.program.model.mem.MemoryBlock;

public class SeedXbeFunctions extends GhidraScript {
    static final long ENTRY = 0x396f9L;
    static final long TEXT_START = 0x11000L;
    static final long TEXT_END = 0x11000L + 1930564L;

    @Override
    public void run() throws Exception {
        AddressSpace sp = currentProgram.getAddressFactory().getDefaultAddressSpace();
        Memory mem = currentProgram.getMemory();

        MemoryBlock blk = mem.getBlock(sp.getAddress(TEXT_START));
        if (blk != null && !blk.isExecute()) {
            blk.setExecute(true);
        }

        seed(sp.getAddress(ENTRY));

        // MSVC prologue scan: push ebp; mov ebp, esp
        byte[] pat = new byte[] { 0x55, (byte) 0x8b, (byte) 0xec };
        Address end = sp.getAddress(TEXT_END);
        Address cur = sp.getAddress(TEXT_START);
        int count = 0;
        while (cur.compareTo(end) < 0) {
            Address hit = mem.findBytes(cur, end, pat, null, true, monitor);
            if (hit == null) {
                break;
            }
            seed(hit);
            count++;
            cur = hit.add(1);
        }
        println("SeedXbeFunctions: entry + " + count + " prologue seeds");
    }

    private void seed(Address a) {
        if (currentProgram.getListing().getInstructionAt(a) == null) {
            new DisassembleCommand(a, null, true).applyTo(currentProgram, monitor);
        }
        if (currentProgram.getFunctionManager().getFunctionAt(a) == null) {
            new CreateFunctionCmd(a).applyTo(currentProgram, monitor);
        }
    }
}
