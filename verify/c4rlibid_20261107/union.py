import csv, sys, collections
def load(n):
    return list(csv.DictReader(open(f"{n}.tsv", encoding="utf-8"), delimiter="\t"))
def ident(n):
    d = {}
    for r in load(n):
        if r["status"] in ("identical", "identical-unlisted"):
            d[r["rva"]] = (r["lib_function"], r["hooks_name"])
    return d
groups = {"lua": ["lua401", "lua_ob2", "lua_o2g5", "lua_o2ob1g5", "lua_g6", "lua_op"],
          "qhull": ["qhull_o2g5", "qhull_ob1", "qhull_ox", "qhull_op"],
          "crt_libcmt": ["crt_libc"], "zlib": ["zlib_ox"], "libpng": ["libpng_ox"]}
for base, vs in groups.items():
    b = ident(base)
    print(f"== {base}: {len(b)} identical (unique)")
    for v in vs:
        x = ident(v)
        new = {k: x[k] for k in x if k not in b}
        lost = [k for k in b if k not in x]
        print(f"  {v}: +{len(new)} new, -{len(lost)}  new: " + ", ".join(f"{k} {fn}" for k, (fn, h) in sorted(new.items())))
