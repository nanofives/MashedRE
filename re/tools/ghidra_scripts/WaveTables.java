// usage: WaveTables.java <out.tsv>
// Dumps the exe-side wave descriptor tables read by FUN_0045d460 (permdict, stride 0x94, 43 rows)
// and FUN_00462950 (course, stride 0x90, selected by PTR table 0x0060c820[course]).
import ghidra.app.script.GhidraScript;
import ghidra.program.model.address.Address;
import java.io.PrintWriter;
import java.util.*;

public class WaveTables extends GhidraScript {
    String cstr(Address a, int max) throws Exception {
        StringBuilder sb = new StringBuilder();
        for (int i = 0; i < max; i++) { int b = getByte(a.add(i)) & 0xff; if (b == 0) break; sb.append((char) b); }
        return sb.toString();
    }
    String hx(int v) { return String.format("0x%08x", v); }
    String fl(int v) { return Float.toString(Float.intBitsToFloat(v)); }
    @Override
    public void run() throws Exception {
        PrintWriter pw = new PrintWriter(getScriptArgs()[0], "UTF-8");
        pw.println("table\tbase_va\trow\trow_va\tname\tp80\tp84_count\tp88\tp8c_hex\tp8c_as_f32\tp90_hex\tp90_as_f32");
        Address t = toAddr(0x006044f0L);
        for (int r = 0; r < 43; r++) {
            Address a = t.add(r * 0x94);
            int p90 = getInt(a.add(0x90));
            pw.println("permdict\t0x006044f0\t" + r + "\t" + a + "\t" + cstr(a, 0x80) + "\t" + getInt(a.add(0x80)) + "\t"
                + getInt(a.add(0x84)) + "\t" + getInt(a.add(0x88)) + "\t" + hx(getInt(a.add(0x8c))) + "\t" + fl(getInt(a.add(0x8c)))
                + "\t" + hx(p90) + "\t" + fl(p90));
        }
        // pointer table: 0x0060c820 .. 0x0060c8bc inclusive (40 entries); next dword 0x0060c8c0 = 7 (not a pointer)
        TreeSet<Long> starts = new TreeSet<>();
        StringBuilder idx = new StringBuilder("# course_ptr_table 0x0060c820:");
        for (int i = 0; i < 40; i++) {
            long p = getInt(toAddr(0x0060c820L + 4L * i)) & 0xffffffffL;
            idx.append(" [").append(i).append("]=0x").append(Long.toHexString(p));
            starts.add(p);
        }
        pw.println(idx);
        for (long s : starts) {
            Long nx = starts.higher(s);
            for (int r = 0; r < 64; r++) {
                long va = s + 0x90L * r;
                if (nx != null && va >= nx) break;
                if (va + 0x90L > 0x0060c820L) break; // the PTR table itself starts here
                Address a = toAddr(va);
                String nm = cstr(a, 0x80);
                if (nm.isEmpty()) break;
                pw.println("course\t0x" + Long.toHexString(s) + "\t" + r + "\t" + a + "\t" + nm + "\t" + getInt(a.add(0x80)) + "\t"
                    + getInt(a.add(0x84)) + "\t" + getInt(a.add(0x88)) + "\t" + hx(getInt(a.add(0x8c))) + "\t" + fl(getInt(a.add(0x8c)))
                    + "\t-\t-");
            }
        }
        pw.close();
    }
}

