"""registry_retire.py <key>=<kept-key> ... : remove the HOOKS entries (and the comment block directly above each) from
re/frida/hooks_registry.py and leave a one-line tombstone naming the kept key. Bytes in, bytes out (the file carries
mojibake and CRLF; neither is touched). Refuses if the entry is not a plain 4-space dict entry."""
import pathlib, re, sys

root = pathlib.Path(__file__).resolve().parent.parent.parent
p = root / "re" / "frida" / "hooks_registry.py"
raw = p.read_bytes()
nl = b"\r\n" if b"\r\n" in raw else b"\n"
ls = raw.split(nl)
for arg in sys.argv[1:]:
    key, kept = arg.split("=")
    pat = re.compile(rb"^    '" + key.encode() + rb"':\s*\{")
    idx = [i for i, l in enumerate(ls) if pat.match(l)]
    assert len(idx) == 1, (key, idx)
    i = idx[0]
    # the entry is either one line (balanced braces) or ends at the first "    }," line
    if ls[i].count(b"{") == ls[i].count(b"}"):
        j = i
    else:
        j = next(k for k in range(i + 1, len(ls)) if ls[k].rstrip() in (b"    },", b"    }"))
    s = i
    while s > 0 and ls[s - 1].lstrip().startswith(b"#") and ls[s - 1].startswith(b"    #"):
        s -= 1
    rva = re.search(rb"'rva':\s*(0x[0-9a-fA-F]+)", b" ".join(ls[i:j + 1]))
    exp = re.search(rb"'export':\s*'(\w+)'", b" ".join(ls[i:j + 1]))
    tomb = (b"    # dualcopy4 2026-10-16: key '" + key.encode() + b"' (" + (rva.group(1) if rva else b"?") + b", export " +
            (exp.group(1) if exp else b"?") + b") RETIRED: it measured a second body of the same RVA; the one body is key '" +
            kept.encode() + b"'.")
    print(f"{key}: lines {s + 1}..{j + 1} -> tombstone")
    ls[s:j + 1] = [tomb]
p.write_bytes(nl.join(ls))
