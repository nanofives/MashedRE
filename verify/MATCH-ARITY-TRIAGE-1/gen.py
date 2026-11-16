"""py -3.12 gen.py <tri.tsv> <out_table.md>  -> markdown tables for RESULT_MATCH-ARITY-TRIAGE-1.md"""
import sys, csv, collections

rows = list(csv.DictReader(open(sys.argv[1], encoding="utf-8"), delimiter="\t"))

GETTER = "callee is a 0-arg getter; the pushes before it belong to the next call, whose callee reads them"
OV = {  # (callee, src or None) -> (verdict, note)
    ("0x0042aae0", "re/match/s08/00433f40.c"): ("CALLER-EXTRA-UNREAD", "push esi 0x00433ff3 is followed by mov esi,[0x0067eca8] 0x00433ff4 (save); the arg is push 0 0x00433ffd; callee reads 0"),
    ("0x0042aae0", "re/match/s08/0043c5b0.c"): ("CALLER-EXTRA-UNREAD", "push eax 0x0043c621 still pending at the indirect call 0x0043c630 [UNCERTAIN attribution]; callee reads 0"),
    ("0x004d8560", None): ("CALLER-EXTRA-UNREAD", "push eax 0x004d8b88 still pending at the indirect call 0x004d8b9e [UNCERTAIN attribution]; callee reads 0"),
    ("0x0045cbe0", None): ("CALLER-EXTRA-UNREAD", "push esi 0x0045cc5a precedes xor esi,esi 0x0045cc5b (save); args -1/esi/1 at 0x0045cc60..63 = 3; callee reads arg2 max (0x0045cbe0)"),
    ("0x00474890", None): ("SAME-LAYOUT", "9 dwords: cleanup add esp,0x24 at 0x0042e7ba / 0x0042e80c; the args are pushed around an inner call"),
    ("0x00493fc0", None): ("CALLEE-RIGHT", "0x00493fc0 = mov eax,[0x00771a18]; ret (0x00493fc0..0x00493fc5): " + GETTER),
    ("0x00499710", None): ("CALLEE-RIGHT", "0x00499710 = mov eax,[0x007e9584]; ret (0x00499710..0x00499715): " + GETTER),
    ("0x005aad40", None): ("CALLEE-RIGHT", "0x005aad40 = mov eax,[0x007dcd20]; ret (0x005aad40..0x005aad45): " + GETTER),
    ("0x004938e0", None): ("CALLEE-RIGHT+HAZARD", "2 dwords: push ecx 0x00494491 (before getter 0x00493fc0) + push eax 0x00494497, cleanup add esp,8 0x0049449d; callee reads [esp+8] at 0x004938ee"),
    ("0x00493fd0", None): ("CALLEE-RIGHT+HAZARD", "7 dwords: push eax/ecx 0x0049545a/5b (before getter 0x00493fc0) + 5 pushes 0x00495461..6e; cleanup add esp,0x20 0x0049547a covers these 7 + push esi 0x00495474; callee reads arg7 [esp+0xd8] 0x00494071"),
    ("0x004c1be0", None): ("CALLEE-RIGHT+HAZARD", "3 dwords: push 0 0x0049547d (before getter 0x00499710) + push eax/esi 0x00495484/85, cleanup add esp,0xc 0x0049548b; callee reads [esp+0xc] at 0x004c1be0"),
    ("0x005aab00", None): ("CALLEE-RIGHT+HAZARD", "2 dwords: push 0 0x005a60bf (before getter 0x005aad40) + push eax 0x005a60c6, cleanup add esp,8 0x005a60cc; callee reads [esp+8] at 0x005aab00"),
    ("0x005aab70", None): ("CALLEE-RIGHT+HAZARD", "2 dwords: push 0 0x005a60e0 (before getter 0x005aad40) + push eax 0x005a60e7; callee reads [esp+8] at 0x005aab70"),
    ("0x00499ba0", None): ("CALLER-EXTRA-UNREAD", "4 dwords: push esi 0x00492381 (esi = [esp+0x14] = arg4, 0x0049237d), eax/ecx/edx 0x00492382..84; push esi 0x0049237c is a save; callee reads 3 (entry [esp+0xc])"),
    ("0x004d3090", None): ("UNCERTAIN", "3 pushes 0x004d3d15..18 are not released before call 0x004d2340 at 0x004d3d2e; attribution needs that callee's reads"),
    ("0x004d4f90", None): ("SAME-LAYOUT", "reference is variadic (Pipe4f90*,uint*,void*,...): 4 pushes are 3 fixed + 1 variadic"),
    ("0x004d7ff0", "re/match/rwl01/004d8a80.c"): ("CALLEE-RIGHT", "push 0x80000013 0x004d8abc; push edi 0x004d8ab9 sits before jne 0x004d8aba [UNCERTAIN attribution]; callee 0x004d7ff0 (5 bytes) reads only [esp+4]"),
    ("0x004d7ff0", "re/match/rwl01/004de1b0.c"): ("CALLER-EXTRA-UNREAD", "2 pushes 0x004de322/27 still pending at jmp 0x004de337 [UNCERTAIN attribution]; callee reads only [esp+4]"),
    ("0x004df120", None): ("UNCERTAIN", "3 pushes 0x004d3d33..36; ref n=2; the linear scan of 0x004df120 (1420 bytes) is not conclusive on a read of arg3"),
    ("0x005a6df0", None): ("CALLER-RIGHT (pass-through)", "4 pushes; 0x005a6df0 reads [esp+4] then tail-jumps at 0x005a6dfd to 0x005a6d60, which reads arg4 at 0x005a6d66"),
    ("0x005a6d60", None): ("CALLEE-RIGHT+HAZARD (tail jmp)", "jmp 0x005a6d60 at 0x005a6dfd reuses 0x005a6df0's frame; its 4 args (callers 0x005a675a, 0x005a6ff6, 0x005a70f1, 0x005a7343, 0x005a73bd, 0x005a74af push 4) reach 0x005a6d60"),
    ("0x0042a5d0", None): ("CALLER-RIGHT (pass-through)", "3 pushes; 0x0042a5d0 rewrites [esp+4] at 0x0042a627 and tail-jumps at 0x0042a62f to 0x004b3bf0, which reads args 2/3 at 0x004b3c0e/0x004b3c12"),
    ("0x0042a6b0", None): ("CALLER-RIGHT (pass-through)", "3 pushes; tail-jumps at 0x0042a714 / 0x0042a727 to 0x004b3d80 / 0x004b3d20, which read args 2/3 (0x004b3d9e / 0x004b3d3e)"),
    ("0x00552b60", None): ("CALLER-RIGHT (pass-through)", "0x00552b60 tail-jumps at 0x00552b88 to 0x00552a60, which reads [esp+4] at 0x00552a60"),
    ("0x0040acd0", None): ("UNCERTAIN", "0x0040acd0 jumps through table 0x0040ad04 (0x0040acdb) and to 0x00409b00/0x0040a130/0x0040a650/0x0040a3a0/0x00409e90/0x00409ad0; none is in pc_funcs.csv; whether a target reads the float arg is not checked"),
    ("0x00426c10", "re/match/t01g/004111c0.c"): ("CALLER-EXTRA-UNREAD", "site 0x0040fefe in 0x0040feb0 (not in pc_funcs.csv): push eax 0x0040fefd; 0x00426c10 reads 0 and tail-jumps at 0x00426c26 to 0x00480100, which reads 0 stack args"),
    ("0x0042a5d0", "re/match/s03/h1_0041ad60.c"): ("STAND-IN", "the call is in caller_0041adxx, a stand-in with no original counterpart (file header)"),
    ("0x00431b80", None): ("CALLER-RIGHT (ref over-declares)", "push 1 then add esp,4 (0x00440352/0x0044035d and 5 more); callee reads only arg1 ([esp+8] after push ebp 0x00431b8f, at 0x00431c88)"),
}
for t in ("0x004b3bf0", "0x004b3c60", "0x004b3cc0", "0x004b3d20", "0x004b3d80", "0x004b3de0"):
    OV[(t, None)] = ("CALLEE-RIGHT+HAZARD (tail jmp)", "jmp from the 1-param wrapper reuses its caller's frame: the 3 args its callers push (see 0x0042a5d0 / 0x0042a6b0 rows and RESULT section 2) reach the callee, which reads arg3")

def ov(r):
    return OV.get((r["callee"], r["src"])) or OV.get((r["callee"], None))

AUTO = {
    "SAME-LAYOUT": "same dword layout, parameter grouping differs only",
    "CALLER-EXTRA-UNREAD": "caller count = site pushes; callee reads fewer (extra args unread, no effect)",
    "SITE-SHORT-UNREAD": "site = caller count; callee reads no more than the site pushes",
}

out = []
groups = collections.OrderedDict()
nocall = collections.defaultdict(list)
for r in rows:
    if r["verdict"] == "NO-CALL" or (r["callee"], r["src"]) in (("0x0052d950", "re/match/rwl03/0052d8e0.c"), ("0x005c9d00", "re/match/s20/004c8e50.c")):
        nocall[r["src"]].append(f"{r['callee']}:{r['line']}")
        continue
    groups.setdefault((r["callee"], r["src"]), []).append(r)

counts = collections.Counter()
out.append("| callee | ref decl (n, dwords) | caller fn | source:line decl (n, dwords) | call site: pushes (address) | callee stack reads | verdict |")
out.append("|---|---|---|---|---|---|---|")
for (c, src), rs in groups.items():
    r0 = rs[0]
    o = ov(r0)
    sites = []
    for r in rs[:4]:
        if r["call_at"] == "NO_SITE":
            sites.append("no direct site")
        elif r["pushes"] == "TAILJMP":
            sites.append(f"jmp @ {r['call_at']}")
        else:
            ck = f", cleanup {'=' if r['cleanup_ok'] == 'yes' else r['cleanup_ok']}" if r["cleanup_ok"] else ""
            rel = r["release"].split(" (")[0].split("->")[0]
            sites.append(f"{r['call_at']}: S={r['S_dw']} [{r['pushes'].replace(' | ', '; ')}]{ck}; {rel}")
    more = f" (+{len(rs) - 4} sites, same verdict class in TSV)" if len(rs) > 4 else ""
    if o:
        v, note = o
    else:
        v = r0["verdict"].replace("+REGARG?", "")
        vs = {x["verdict"].replace("+REGARG?", "") for x in rs}
        if len(vs) > 1:
            v = "/".join(sorted(vs))
        note = AUTO.get(v, "")
    counts[v.split(" ")[0]] += 1
    reads = r0["callee_maxarg"].replace("|", "/")
    out.append(f"| {c} | {r0['ref']} ({r0['refn']}, {r0['R_dw']}) | {r0['caller'] or '-'} | {src}:{r0['line']} `{r0['proto']}` ({r0['n']}, {r0['C_dw']}) | "
               f"{'<br>'.join(sites)}{more} | {reads} | **{v}**{': ' + note if note else ''} |")

out.append("")
out.append("### NO-CALL pairs (declaration only, or the name is used as an address; the declared arity emits no code)")
out.append("")
out.append("| source | callee:decl line |")
out.append("|---|---|")
for src in sorted(nocall):
    out.append(f"| {src} | {', '.join(nocall[src])} |")
open(sys.argv[2], "w", encoding="utf-8").write("\n".join(out) + "\n")
print(dict(counts), "call-bearing pairs", len(groups), "no-call pairs", sum(len(v) for v in nocall.values()))
