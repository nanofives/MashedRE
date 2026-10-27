"""D4-WSB-C4-SOLV-WS re-classify transaction (run once from the worktree root).

C3 -> C4 for the rows in PROMOTE: heap-tracked write-set leg HEAP-CLEAN + canonical-scenario racediff C4-CLEAN, both on
this branch's build, default gates unchanged. Rows in STAY get a note only. CHANGELOG lines below <!-- ENTRIES -->.
Line-level edits only: every other byte of the two files is left as it is.
"""
from pathlib import Path

EV = "verify/wsbsolvws_20261027/RESULT_D4-WSB-C4-SOLV-WS.md"
RD = "verify/wsbsolvws_20261027/racediff_summary.tsv"
QB = "c4_racediff_bvh:QuickBattle-f300-1200"
CEN = "c4_racediff_bvh:census-a1-3000"
HEAP = "shadow_tracked_heap:track0-mode10-cars4"
# rva: (racediff scenario tag, racediff read, heap leg read)
PROMOTE = {
    "00568dd0": (QB, "QuickBattle 6/6 calls", "24/24 samples, 1 heap page per sample, 24 total"),
    "00568fd0": (QB, "QuickBattle 6/6 calls", "MASHED_SHADOW_NEED_PAGES arm: 3 counted samples, 1..2 heap pages each, "
                                              "4 total, 1000..1249 calls with no write discarded"),
    "00564c80": (QB, "QuickBattle 6/6 calls, 4 sigs", "24/24 samples, 5..7 heap pages per sample, 129 total; the BVH-FU "
                                                      "+1/+1 at 0x007dc8c8/0x007dc8cc does not recur with heap pages restored"),
    "00561390": (QB, "QuickBattle 6/6 calls", "24/24 samples, 11 heap pages per sample, 264 total"),
    "00563e70": (CEN, "census A4 calls 1..3000", "24/24 samples, 2..3 heap pages per sample, 60 total"),
}
STAY = {
    "00568990": "heap-tracked leg compared 0 pages: every sampled call returns 0 before any store (param_3 == 0 || "
                "param_5 == 0), NEED_PAGES arm 0 counted samples, 1000..1249 discards all rv 0. Racediff QuickBattle "
                "C4-CLEAN 6/6. Needs a scenario where the pair pump receives pairs",
    "0056b7a0": "heap-tracked leg compared 0 pages: NEED_PAGES arm 0 counted samples, 1500..1749 discards all rv 0. "
                "Racediff QuickBattle C4-CLEAN 6/6. Needs a scenario where the call writes",
    "0056ba30": "heap-tracked leg HEAP-CLEAN (24/24, 24 heap pages) but racediff QuickBattle HOLD BEHAVIOR-DIFF: 4th arg "
                "of the call to 0x0056b7a0 labelled h1 OFF / A0+118 ON (same expression both sides, 0x0056ba6f / asi). "
                "Raw-value arm D: LAYOUT (OFF 4th arg = param_1 - 0x190, ON + 0x78, first ON + 0x118: a separate heap "
                "object whose placement relative to param_1 varies per boot; the v7 label window [arg, arg + 0x4000) "
                "makes it A0+ only when it lands above param_1). Tool finding, re-run the racediff once the label is fixed",
}


def edit_hooks():
    p = Path("hooks.csv")
    lines = p.read_bytes().decode("utf-8").split("\r\n")
    hit = set()
    for i, ln in enumerate(lines):
        rva = ln.split(",", 1)[0]
        if rva in PROMOTE:
            scen, rd, hp = PROMOTE[rva]
            assert ",C3,impl," in ln and '",' in ln, rva
            f = ln.split(",", 8)   # rva,name,sub,conf,status,file,scenario,frida_diff,rest
            assert f[3] == "C3" and f[4] == "impl", rva
            f[3], f[4], f[6], f[7] = "C4", "verified", scen + "+" + HEAP, RD
            ln = ",".join(f)
            seg = (f" | C3->C4 2026-10-27 (D4-WSB-C4-SOLV-WS): heap-tracked write-set shadow HEAP-CLEAN "
                   f"(ShadowAB::RunTracked, MASHED_SHADOW_PRIVATE=1 + STW, hook installed alone, PATCHBYTE A/B-IS-REAL "
                   f"installed=e9, census track0 mode10 cars4, {hp}, 0 divergent, caller 4 KB stack window and return "
                   f"compared) + canonical racediff C4-CLEAN ({rd}, hook installed alone, 0xE9 at the RVA), both on this "
                   f"step's build; default gates unchanged. No stub in the body. {EV}")
            k = ln.rfind('",'); ln = ln[:k] + seg + ln[k:]
            hit.add(rva)
        elif rva in STAY:
            assert ",C3,impl," in ln and '",' in ln, rva
            seg = f" | 2026-10-27 (D4-WSB-C4-SOLV-WS): stays C3. {STAY[rva]}. {EV}"
            k = ln.rfind('",'); ln = ln[:k] + seg + ln[k:]
            hit.add(rva)
        lines[i] = ln
    assert hit == set(PROMOTE) | set(STAY), hit
    p.write_bytes("\r\n".join(lines).encode("utf-8"))


def edit_changelog():
    p = Path("re/analysis/CHANGELOG.md")
    raw = p.read_bytes().decode("utf-8")
    nl = "\r\n" if "\r\n" in raw else "\n"
    marker = "<!-- ENTRIES -->" + nl
    assert raw.count(marker) == 1
    add = "".join(
        f"2026-10-27  {r}  FUN_{r}  C3->C4  D4-WSB-C4-SOLV-WS: heap-tracked write-set leg HEAP-CLEAN ({PROMOTE[r][2]}) "
        f"+ racediff C4-CLEAN ({PROMOTE[r][1]}); {EV}{nl}" for r in PROMOTE)
    add += "".join(f"2026-10-27  {r}  FUN_{r}  C3 (no change)  D4-WSB-C4-SOLV-WS: {STAY[r]}; {EV}{nl}" for r in STAY)
    p.write_bytes(raw.replace(marker, marker + add, 1).encode("utf-8"))


if __name__ == "__main__":
    edit_hooks(); edit_changelog()
    print("applied")
