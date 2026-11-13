# S1B attempt 2: the 9044 c2 switch table (strings in tools/vc6pp/bin/c2.dll) passed through /d2, rw9044 only.
exec(open("v1.py").read().split("COMBOS =")[0])
VARIANTS = {k: VARIANTS[k] for k in ("dn", "idx")}
X = ["", "/d2Oq", "/d2u5", "/d2P", "/d2qt", "/d2nolock", "/d2noehopt", "/d2ehopt", "/d2bzalign", "/d2datasort",
     "/d2dslow0", "/d2dshigh0", "/d2dslow100", "/d2dshigh100", "/d2EHa", "/d2EHs", "/d2Zd", "/d2Za", "/d2Zl",
     "/d2inlt0", "/d2inls0", "/d2inlt100", "/d2inls100", "/d2isize0", "/d2loopopt", "/d2nodlp", "/d2dlp",
     "/d2QIfpinit", "/d2QIdivrem64", "/d2ide", "/d2cbstring", "/d2fastcap",
     "/d2off100", "/d2off255", "/d2off1000", "/d2off65535", "/d2db4", "/d2db8", "/d2db16", "/d2db255",
     "/d2vol4", "/d2vol8", "/d2vol255", "/d2ef3", "/d2ef8", "/d2ef255", "/d2Loop5", "/d2Loop6", "/d2Loop7", "/d2Loop16",
     "/d2stkfill0", "/d2Gt0", "/d2Gt100"]
COMBOS = [("rw9044", ("/O2 /Ob1 /G5 /Oy- " + x).strip()) for x in X]
