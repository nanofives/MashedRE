"""D4-WSB-SHADOW-PRIVATE re-classify transaction (run once from the worktree root).

C3 -> C4: 0055fea0, 0055ff90, 00565260 (heap-tracked write-set leg CLEAN + existing racediff C4-CLEAN).
00564040 stays C3 (heap leg compared 0 pages). U-9792 narrowed to 00564040. 4 CHANGELOG lines below <!-- ENTRIES -->.
Line-level edits only: every other byte of the three files is left as it is.
"""
from pathlib import Path

EV = "verify/wsbshadowpriv_20261027/RESULT_D4-WSB-SHADOW-PRIVATE.md"
PROMOTE = {"0055fea0": "1", "0055ff90": "4", "00565260": "2..4"}
STAY = "00564040"


def edit_hooks():
    p = Path("hooks.csv")
    lines = p.read_bytes().decode("utf-8").split("\r\n")
    hit = set()
    for i, ln in enumerate(lines):
        rva = ln.split(",", 1)[0]
        if rva in PROMOTE:
            assert ",C3,impl," in ln and '",' in ln, rva
            ln = ln.replace(",C3,impl,", ",C4,verified,", 1)
            ln = ln.replace("+shadow_tracked:track0-mode10-cars4,", "+shadow_tracked_heap:track0-mode10-cars4,", 1)
            seg = (f" | C3->C4 2026-10-27 (D4-WSB-SHADOW-PRIVATE): heap-tracked write-set shadow CLEAN "
                   f"(ShadowAB::RunTracked, MASHED_SHADOW_PRIVATE=1 + STW, hook installed alone, PATCHBYTE A/B-IS-REAL, "
                   f"census track0 mode10 cars4, 24/24 samples, 0 divergent, {PROMOTE[rva]} heap pages compared per sample, "
                   f"caller 4 KB stack window and return compared). Racediff C4-CLEAN leg unchanged. No stub in the body. "
                   f"Resolves U-9792 for this row. {EV}")
            k = ln.rfind('",'); ln = ln[:k] + seg + ln[k:]
            hit.add(rva)
        elif rva == STAY:
            assert ",C3,impl," in ln and '",' in ln, rva
            seg = (" | 2026-10-27 (D4-WSB-SHADOW-PRIVATE): stays C3. Heap-tracked write-set leg CLEAN 24/24 but 0 pages "
                   "compared: in the census scenario the sampled calls return 0 and write nothing (arms N/N2, "
                   "MASHED_SHADOW_NEED_PAGES, 500..749 calls, every rv 0). U-9792 open for this row: needs a scenario "
                   f"where it collects entries. {EV}")
            k = ln.rfind('",'); ln = ln[:k] + seg + ln[k:]
            hit.add(rva)
        lines[i] = ln
    assert hit == set(PROMOTE) | {STAY}, hit
    p.write_bytes("\r\n".join(lines).encode("utf-8"))


def edit_uncertainties():
    p = Path("UNCERTAINTIES.md")
    lines = p.read_bytes().decode("utf-8").split("\r\n")
    n = 0
    for i, ln in enumerate(lines):
        if ln.startswith("| U-9792 |"):
            old = "| C4 of the four rows |"
            assert ln.endswith(old)
            ln = ln[: -len(old)] + (
                "| C4 of `0x00564040` |")
            ln = ln.replace(
                "re-promote only on CLEAN. `verify/wsbbvhfu_20261027/RESULT_D4-WSB-BVH-FU.md` section 4 |",
                "re-promote only on CLEAN. `verify/wsbbvhfu_20261027/RESULT_D4-WSB-BVH-FU.md` section 4. "
                "**2026-10-27 D4-WSB-SHADOW-PRIVATE: resolved for `0x0055fea0`, `0x0055ff90`, `0x00565260`** (freeze fixed, "
                "heap-tracked leg CLEAN 24/24 each with 1 / 4 / 2..4 heap pages compared per sample, re-promoted C4). "
                "Open for `0x00564040` only: its leg compared 0 pages because the sampled calls return 0 and write nothing "
                "(500..749 calls). Next: the same leg in a scenario with car contacts. " + EV + " |", 1)
            n += 1
        lines[i] = ln
    assert n == 1
    p.write_bytes("\r\n".join(lines).encode("utf-8"))


def edit_changelog():
    p = Path("re/analysis/CHANGELOG.md")
    raw = p.read_bytes().decode("utf-8")
    nl = "\r\n" if "\r\n" in raw else "\n"
    marker = "<!-- ENTRIES -->" + nl
    assert raw.count(marker) == 1
    add = "".join(
        f"2026-10-27  {r}  FUN_{r}  C3->C4  D4-WSB-SHADOW-PRIVATE: heap-tracked write-set leg CLEAN 24/24 "
        f"({PROMOTE[r]} heap pages/sample, PRIVATE+STW, hook installed alone) + existing racediff C4-CLEAN; U-9792 resolved for this row; {EV}{nl}"
        for r in PROMOTE)
    add += (f"2026-10-27  {STAY}  FUN_{STAY}  C3 (no change)  D4-WSB-SHADOW-PRIVATE: heap-tracked leg compared 0 pages "
            f"(sampled calls write nothing); U-9792 stays open for this row; {EV}{nl}")
    p.write_bytes(raw.replace(marker, marker + add, 1).encode("utf-8"))


if __name__ == "__main__":
    edit_hooks(); edit_uncertainties(); edit_changelog()
    print("applied")
