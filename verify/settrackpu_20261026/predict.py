"""Offline prediction for D4-FU-SETTRACKPU: the D4-WSF-SAVE-SPANINIT first-run image (predict.py there) plus
the boot script's Set_Track_Powerups rows (0x0047b280 / 0x00430250, course-row table 0x005f2728), diffed
against the original's first-run file O_0. Usage:
    py -3.12 predict.py <O_0.bin> <POWERUPS_ALL.LUA> [<out.bin>]"""
import importlib.util, re, struct, sys
from pathlib import Path

_SPEC = importlib.util.spec_from_file_location(
    "spaninit_predict", Path(__file__).resolve().parents[1] / "wsfspan_20261026" / "predict.py")
span = importlib.util.module_from_spec(_SPEC)   # init_span / first_run_image of D4-WSF-SAVE-SPANINIT
_SPEC.loader.exec_module(span)

SIZE, SPAN, CNT = span.SIZE, span.SPAN, span.CNT
ROW_COURSE = [30, 3, 39, 0, 38, 11, 25, 2, 36, 34, 37, 33, 26]   # **[0x005f2728 + r*4]


def script_calls(text):
    loc, calls, skipped = {}, [], 0
    for raw in text.splitlines():
        line = raw.split("--", 1)[0].strip()
        if not line:
            continue
        m = re.fullmatch(r"local\s+(\w+)\s*=\s*(-?\d+)", line)
        if m:
            loc[m.group(1)] = int(m.group(2)); continue
        m = re.fullmatch(r"Set_Track_Powerups\s*\((.*)\)", line)
        if m:
            a = [x.strip() for x in m.group(1).split(",")]
            if len(a) == 7:
                calls.append([int(x) if re.fullmatch(r"-?\d+", x) else loc.get(x, 0) for x in a]); continue
        skipped += 1
    return calls, skipped


def apply(img, calls):
    img = bytearray(img)
    for a in calls:
        if a[1] == 0 or a[0] not in ROW_COURSE:
            continue
        r = ROW_COURSE.index(a[0])
        for k in range(5):
            struct.pack_into("<i", img, SPAN + 0x270 + r * 0x14 + 4 * k, a[2 + k])
    return bytes(img)


def bucket(i):
    if 0x24CB0 <= i < 0x24DB4: return "set_track_powerups_rows"
    if SPAN <= i < SPAN + 13 * 0x30 and (i - SPAN) % 0x30 in range(0x10, 0x14): return "col4_unlock"
    if 0x24F2C <= i < 0x24F30: return "gate_4ec"
    if 0x24F54 <= i < 0x24F60: return "gather"
    if CNT <= i < CNT + 4: return "state_24efc"
    return "other"


def main():
    o = Path(sys.argv[1]).read_bytes()
    calls, skipped = script_calls(Path(sys.argv[2]).read_bytes().decode("latin-1"))
    before = span.first_run_image()
    p = apply(before, calls)
    for tag, img in (("before (SPANINIT model)", before), ("predicted (SPANINIT + script)", p)):
        d = [i for i in range(SIZE) if o[i] != img[i]]
        bk = {}
        for i in d:
            bk[bucket(i)] = bk.get(bucket(i), 0) + 1
        print(f"{tag} vs O_0: {len(d)} differ {bk}")
    print(f"script: {len(calls)} calls, {skipped} skipped statements")
    if len(sys.argv) > 3:
        Path(sys.argv[3]).write_bytes(p)


if __name__ == "__main__":
    main()
