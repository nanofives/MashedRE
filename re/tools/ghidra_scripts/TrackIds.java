// usage: TrackIds.java <out.txt> : track records at 0x005f33f8 stride 0x48 (FUN_0041e980), count DAT_005f37a0
import ghidra.app.script.GhidraScript;
import ghidra.program.model.address.Address;
import java.io.PrintWriter;

public class TrackIds extends GhidraScript {
    @Override
    public void run() throws Exception {
        PrintWriter pw = new PrintWriter(getScriptArgs()[0], "UTF-8");
        int n = getInt(toAddr(0x005f37a0L));
        pw.println("count DAT_005f37a0 = " + n);
        for (int i = 0; i < n && i < 64; i++) {
            Address a = toAddr(0x005f33f8L + 0x48L * i);
            StringBuilder sb = new StringBuilder();
            for (int k = 0; k < 0x10; k++) { int b = getByte(a.add(k)) & 0xff; if (b == 0) break; sb.append((char) b); }
            int id = getInt(a.add(0x10));
            long ptr = (id >= 0 && id < 40) ? (getInt(toAddr(0x0060c820L + 4L * id)) & 0xffffffffL) : -1;
            pw.println(i + "\t" + a + "\t" + sb + "\tid=" + id + "\tcourse_tbl=0x" + Long.toHexString(ptr));
        }
        pw.close();
    }
}
