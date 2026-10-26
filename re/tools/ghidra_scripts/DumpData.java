// DumpData.java - print the raw dword, its float view and any C string at each address.
// Reads the program's initialized bytes (the anchored image), not a live process.
// usage: analyzeHeadless ... -postScript DumpData.java <out.txt> 0x<addr> [0x<addr> ...]
import ghidra.app.script.GhidraScript;
import ghidra.program.model.address.Address;
import java.io.PrintWriter;

public class DumpData extends GhidraScript {
    @Override
    public void run() throws Exception {
        String[] args = getScriptArgs();
        PrintWriter w = new PrintWriter(args[0], "UTF-8");
        for (int i = 1; i < args.length; i++) {
            Address a = toAddr(Long.decode(args[i]));
            String line = args[i];
            try {
                int v = getInt(a);
                line += String.format(" dword=0x%08x float=%s", v, Float.toString(Float.intBitsToFloat(v)));
            } catch (Exception e) { line += " (unreadable: " + e.getMessage() + ")"; }
            StringBuilder s = new StringBuilder();
            try {
                for (int k = 0; k < 64; k++) {
                    byte b = getByte(a.add(k));
                    if (b == 0) break;
                    if (b < 0x20 || b > 0x7e) { s.setLength(0); break; }
                    s.append((char) b);
                }
            } catch (Exception e) { s.setLength(0); }
            if (s.length() >= 3) line += " str=\"" + s + "\"";
            w.println(line);
        }
        w.close();
    }
}
