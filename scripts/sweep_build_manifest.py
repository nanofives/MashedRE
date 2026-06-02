"""sweep_build_manifest.py — Phase A of the scripted ghidra-sweep.

Reads the queued sweep rows (from re/SCRIBE_QUEUE.md's "## Queued" section, or
from one/more fragment files), resolves each RVA to its analysis plate, extracts
the first content chunk of the plate's "## Mechanical description", truncates it
to <=120 chars at a word boundary, and emits a nested-by-bucket JSON manifest
that scripts/ghidra/sweep_apply_eval.py applies to master Ghidra in a single
transaction (via the MCP `ghidra_eval` tool).

This replaces the per-RVA MCP round-trip loop in the ghidra-sweep skill: the
manifest is built locally (filesystem only, no Ghidra), and the master write
becomes one batched eval instead of ~5 round-trips per RVA.

Generalized from the 2026-05-19 batch_y one-off. Two robustness fixes that broke
the original:
  - plates are indexed by their frontmatter `rva:` (with a filename-stem
    fallback), so bare-hex `0042f760.md` and prefixed `0x0042f760.md` both
    resolve — the original globbed only `0x*.md`.
  - level/date/category are parametrized, not hardcoded.

Renames stay manifest-driven and conservative: an entry only carries a `rename`
if its plate frontmatter declares `rename: <symbol>`. The apply step additionally
honors a pre-existing single-line "Library Function:" attestation, and in both
cases re-checks that Ghidra still shows FUN_ before touching the name.

Usage:
  py -3.12 scripts/sweep_build_manifest.py \
      --rows re/SCRIBE_QUEUE_ah_s1.md ... re/SCRIBE_QUEUE_ah_s6.md \
      --out sweep_manifest_ah.json --date 2026-06-01 --level C2
  py -3.12 scripts/sweep_build_manifest.py \
      --queue re/SCRIBE_QUEUE.md --out sweep_manifest.json
"""
from __future__ import annotations

import argparse
import json
import re
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
ANALYSIS = ROOT / "re" / "analysis"

ROW_DATE = re.compile(r"^\s*(\d{4}-\d{2}-\d{2})\b")
BUCKET_RE = re.compile(r"\bbucket=(\S+)")
RVAS_RE = re.compile(r"\brvas=([0-9a-fA-Fx,\s]+?)(?:\s+\w+=|\s*$)")
CONF_RE = re.compile(r"\bconfidence=\S*?->\s*(C\d)")
FM_RVA = re.compile(r"^\s*rva:\s*(?:0x)?([0-9a-fA-F]+)", re.MULTILINE)
FM_RENAME = re.compile(r"^\s*rename:\s*(\S+)", re.MULTILINE)


def norm_addr(token: str):
    t = token.strip().lower()
    if t.startswith("0x"):
        t = t[2:]
    if not t:
        return None
    try:
        return int(t, 16)
    except ValueError:
        return None


def hexstr(addr: int) -> str:
    return f"0x{addr:08x}"


def resolve_bucket_dir(raw: str) -> Path:
    raw = raw.strip().rstrip("/")
    p = Path(raw)
    norm = str(p).replace("\\", "/")
    if not norm.startswith("re/analysis/"):
        p = Path("re/analysis") / p.name
    return ROOT / p


def index_bucket(bucket_dir: Path) -> dict[int, Path]:
    """Map normalized RVA -> plate path, keyed on each plate's frontmatter rva,
    falling back to the filename stem."""
    idx: dict[int, Path] = {}
    if not bucket_dir.is_dir():
        return idx
    for md in sorted(bucket_dir.glob("*.md")):
        head = md.read_text(encoding="utf-8", errors="replace")[:600]
        m = FM_RVA.search(head)
        addr = norm_addr(m.group(1)) if m else None
        if addr is None:
            addr = norm_addr(md.stem)
        if addr is not None and addr not in idx:
            idx[addr] = md
    return idx


def first_chunk(md_text: str):
    """First content chunk under '## Mechanical description': join lines until
    the next heading, a code fence, or a blank line after real content. Bullet/
    numbered markers stripped. Never paraphrased — truncation is the only edit."""
    lines = md_text.splitlines()
    start = None
    for i, ln in enumerate(lines):
        if ln.strip().lower().startswith("## mechanical description"):
            start = i + 1
            break
    if start is None:
        return None
    buf: list[str] = []
    for ln in lines[start:]:
        s = ln.strip()
        if s.startswith("## ") or s.startswith("```"):
            break
        if not s:
            if buf:
                break
            continue
        if set(s) <= {"-", "*", "_", "|", " "}:  # rule / table separator
            continue
        buf.append(s)
    if not buf:
        return None
    chunk = " ".join(buf)
    chunk = re.sub(r"^(?:[-*]\s+|\d+\.\s+)", "", chunk)
    chunk = re.sub(r"\s+", " ", chunk).strip()
    return chunk or None


def truncate(text: str, limit: int = 120) -> str:
    if len(text) <= limit:
        return text
    cut = text[:limit]
    sp = cut.rfind(" ")
    if sp > 40:
        cut = cut[:sp]
    return cut.rstrip(" ,.;:!?-") + "…"


def read_rows(text: str, queued_only: bool) -> list[str]:
    if queued_only and "## Queued" in text:
        seg = text.split("## Queued", 1)[1]
        nxt = re.search(r"\n##\s", seg)
        if nxt:
            seg = seg[: nxt.start()]
        text = seg
    return [ln.rstrip() for ln in text.splitlines()
            if "bucket=" in ln and "rvas=" in ln]


def main() -> int:
    ap = argparse.ArgumentParser()
    ap.add_argument("--queue", help="SCRIBE_QUEUE.md; reads its '## Queued' section")
    ap.add_argument("--rows", nargs="*", default=[], help="fragment files (whole-file scan)")
    ap.add_argument("--out", required=True)
    ap.add_argument("--date", default=None, help="override plate date; default = each row's leading date")
    ap.add_argument("--level", default="C2", help="confidence level for plate prefix when row omits it")
    args = ap.parse_args()

    raw_rows: list[str] = []
    if args.queue:
        qp = Path(args.queue)
        qp = qp if qp.is_absolute() else ROOT / qp
        raw_rows += read_rows(qp.read_text(encoding="utf-8", errors="replace"), True)
    for rf in args.rows:
        rp = Path(rf)
        rp = rp if rp.is_absolute() else ROOT / rf
        raw_rows += read_rows(rp.read_text(encoding="utf-8", errors="replace"), False)

    buckets_out: list[dict] = []
    seen: set[int] = set()
    dups: list[str] = []
    tot_rvas = tot_plated = tot_missing = 0
    manifest_date = args.date or "0000-00-00"

    for row in raw_rows:
        if "HOLD=" in row:
            continue
        bm, rm = BUCKET_RE.search(row), RVAS_RE.search(row)
        if not bm or not rm:
            continue
        bucket_dir = resolve_bucket_dir(bm.group(1))
        bucket = bucket_dir.name
        dm = ROW_DATE.search(row)
        date = args.date or (dm.group(1) if dm else "0000-00-00")
        if not args.date and dm:
            manifest_date = dm.group(1)
        cm = CONF_RE.search(row)
        level = cm.group(1) if cm else args.level

        rvas = [a for a in (norm_addr(t) for t in rm.group(1).split(",")) if a is not None]
        idx = index_bucket(bucket_dir)
        entries: list[dict] = []
        missing_md: list[str] = []
        for addr in rvas:
            tot_rvas += 1
            if addr in seen:
                dups.append(hexstr(addr))
                continue
            seen.add(addr)
            plate_file = idx.get(addr)
            if plate_file is None:
                missing_md.append(hexstr(addr))
                tot_missing += 1
                continue
            ptext = plate_file.read_text(encoding="utf-8", errors="replace")
            chunk = first_chunk(ptext)
            if not chunk:
                missing_md.append(hexstr(addr))
                tot_missing += 1
                continue
            rn = FM_RENAME.search(ptext[:600])
            entries.append({
                "rva": hexstr(addr),
                "plate": f"[{level} {date}] {truncate(chunk)}",
                "rename": rn.group(1) if rn else None,
            })
            tot_plated += 1
        buckets_out.append({
            "bucket": bucket,
            "bm_category": "promote-c2" if level == "C2" else "first-pass",
            "rvas": entries,
            "missing_md": missing_md,
        })

    out = {
        "date": manifest_date,
        "level": args.level,
        "buckets": buckets_out,
        "totals": {"rvas": tot_rvas, "plated": tot_plated,
                   "missing_md": tot_missing, "cross_bucket_dups": dups},
    }
    op = Path(args.out)
    op = op if op.is_absolute() else ROOT / args.out
    op.write_text(json.dumps(out, indent=1), encoding="utf-8")
    print(f"manifest -> {op}")
    print(f"  total rvas={tot_rvas} plated={tot_plated} missing_md={tot_missing} dups={len(dups)}")
    for b in buckets_out:
        flag = "" if not b["missing_md"] else f"  !! {len(b['missing_md'])} MISSING"
        print(f"  {b['bucket']}: {len(b['rvas'])} plated{flag}")
    if dups:
        print(f"  cross-bucket dup RVAs (kept once): {', '.join(dups[:12])}{'...' if len(dups) > 12 else ''}")
    return 0


if __name__ == "__main__":
    sys.exit(main())
