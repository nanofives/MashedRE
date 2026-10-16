"""Rewrite `path2_tests` of the 7 registry entries whose path2 call-through could not be driven (2026-10-16 run:
RESULT_dupinstall.md). path1 vectors and every other key are untouched; the new shapes are consumed by the
`p2bufs/p2args` branch added to re/frida/verify_hook_install_template.js (or plain scalars where the signature is scalar).

    py -3.12 fix_path2_vectors.py            # dry run
    py -3.12 fix_path2_vectors.py --apply
"""
import pathlib
import re
import sys

ROOT = pathlib.Path(__file__).resolve().parent.parent.parent
REG = ROOT / "re" / "frida" / "hooks_registry.py"

B = "{'p2bufs': [%s], 'p2args': [%s]}"
NEW = {
    # 1-arg scalar key; was [[key, placeAt], ...] (the early-window vector shape)
    "copter_find_639dc4": "[0xAB000001, 0xAB000002, 0xDEAD]",
    # void f(out*) declared 'uint32'; was small ints, i.e. out = 0x0 / 0x1 -> AV before entry
    "bool0_out_8a94e0": "[" + ", ".join([B % ("16", "{'buf': 0}")] * 3) + "]",
    "boolmin_out_8a94e0": "[" + ", ".join([B % ("16", "{'buf': 0}")] * 3) + "]",
    # void f(out*, mode): was [0, 1] -> one argument ("bad argument count")
    "maxscoreflags_40b9a0": "[" + B % ("0x20", "{'buf': 0}, 0") + ", " + B % ("0x20", "{'buf': 0}, 1") + "]",
    # void f(out*): was [0] -> "expected a pointer"
    "active4slots_40ba60": "[" + B % ("0x20", "{'buf': 0}") + "]",
    # void f(hdr*): was raw 0xDEADBEEF / 0 / 0xFFFFFFFF -> "expected a pointer" (and a wild write if it had been accepted)
    "audio_list_init": "[" + ", ".join([B % ("0x40", "{'buf': 0}")] * 3) + "]",
    # void f(int page): was [[8], [0x1e], [0x63]] -> "expected an integer"
    "frontend_page_id_dispatch": "[8, 0x1e, 0x63]",
    # uint32 f(obj*): was {'seeds': [...128 u32...]} (struct_call_observe shape) -> "expected a pointer". Window 2 only:
    # these two are race-scenario rows and could not be reached before the Fwd494fd0 finding. 0x200 covers the live
    # plugin offsets 0x60 / 0x88.
    "plugin_data_dword_a": "[" + B % ("0x200", "{'buf': 0}") + "]",
    "plugin_data_dword_b": "[" + B % ("0x200", "{'buf': 0}") + "]",
}


def span_of_list(s: str, start: int):
    """s[start] == '[': index one past the matching ']'."""
    depth = 0
    for i in range(start, len(s)):
        if s[i] == "[":
            depth += 1
        elif s[i] == "]":
            depth -= 1
            if depth == 0:
                return i + 1
    raise ValueError("unbalanced")


def main() -> int:
    raw = REG.read_bytes().decode("utf-8")
    out = raw
    for key, new in NEW.items():
        m = re.search(r"^    '" + re.escape(key) + r"': \{", out, re.M)
        assert m, key
        # entry end = first line that starts with '    },' or the same line closing '},' (single-line entries)
        line_end = out.index("\n", m.start())
        first = out[m.start():line_end]
        if first.rstrip().endswith("},") or first.rstrip().endswith("}"):
            ent_end = line_end
        else:
            ent_end = out.index("\n    },", m.start()) + 7
        ent = out[m.start():ent_end]
        pm = re.search(r"'path2_tests':\s*", ent)
        assert pm, key
        ls = pm.end()
        assert ent[ls] == "[", (key, ent[ls:ls + 20])
        le = span_of_list(ent, ls)
        old = ent[ls:le]
        ent2 = ent[:ls] + new + ent[le:]
        print(f"{key:28s} {old[:60]!r} -> {new[:60]!r}")
        out = out[:m.start()] + ent2 + out[ent_end:]
    if "--apply" in sys.argv:
        REG.write_bytes(out.encode("utf-8"))
        print("applied")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
