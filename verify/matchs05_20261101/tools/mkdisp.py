import pathlib, sys
PRE = pathlib.Path("D:/MashedWT/MATCH-S05_data/tools/preamble.h").read_text()
# va -> (vtable offset, register in the original's mov)
T = {0x0041e8b0:(0x14,'ecx'),0x0041e8c0:(0x18,'edx'),0x0041e8d0:(0x1c,'ecx'),0x0041e8e0:(0x20,'ecx'),
     0x0041e8f0:(0x24,'ecx'),0x0041e900:(0x28,'ecx'),0x0041e910:(0x2c,'ecx'),0x0041e920:(0x30,'ecx'),
     0x0041e930:(0x34,'ecx'),0x0041e940:(0x38,'ecx'),0x0041e950:(0x3c,'ecx'),0x0041e960:(0x40,'ecx'),0x0041e970:(0x44,'ecx')}
for va,(off,reg) in T.items():
    n = {'ecx':1,'edx':2}[reg]
    P = ", ".join(f"int a{i}" for i in range(1,n+1)); A = ", ".join(f"a{i}" for i in range(1,n+1))
    ty = ", ".join(["int"]*n)
    txt = f"""// FLAGS: /O2 /GS /Ob1
/* MATCH-S05 0x{va:08x}: 100% by hand. Original: mov {reg}, [0x0063d7e4]; jmp [{reg} + 0x{off:x}] (a tail call
   through the function pointer at +0x{off:x} of the record DAT_0063d7e4 points at, stack args passed through).
   Under this compiler the scratch register is set by the number of stack args: 0/3 -> eax, 1/4 -> ecx, 2/5 -> edx
   (checked for 0..4 args, return type has no effect). {n} args is the smallest count that gives {reg}; the bytes and
   the behaviour (a jmp leaves the caller's args untouched) are the same for {n + 3}. */
""" + PRE + f"""typedef void (*MatchFn{va:08x})({ty});
MGLOBAL(char *, DAT_0063d7e4, 0x0063d7e4);
// MATCH: 0x{va:08x} FUN_{va:08x}
void FUN_{va:08x}({P})
{{
  (*(MatchFn{va:08x} *)(DAT_0063d7e4 + 0x{off:x}))({A});
}}
"""
    pathlib.Path(f"re/match/s05/{va:08x}.c").write_bytes(txt.encode())
