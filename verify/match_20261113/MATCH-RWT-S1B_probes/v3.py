exec(open("v1.py").read().split("COMBOS =")[0])
COMBOS = [(tc, fl) for tc in ("ddk", "rw9044") for fl in (
    "/O2 /Ob1 /G5 /Os /Oy-", "/O1 /G5 /Oy-", "/O1 /Ob1 /G5 /Oy-", "/Ogsy /Ob1 /G5 /Oy-", "/Ogisy /Ob1 /G5 /Oy-",
    "/Og /Os /Ob1 /G5 /Oy- /Gs /GF", "/Og /Oi /Os /Ob1 /G5 /Oy- /Gs /GF")]
