"""MATCH-RWT-S5 Fable attempt, step 3: tail forms on f2.py's tk (first 75 instructions equal).
v = vb local before the if, g = DAT_007d70ec[DAT_007d70c8] inline at every use, b = vb assigned inside each branch,
p = pointer to the slot."""
import pathlib
_ns = {}
exec(pathlib.Path(__file__).with_name("f2.py").read_text(), _ns)
BASE = _ns["VARIANTS"]["tk"]
G = "DAT_007d70ec[DAT_007d70c8]"
i0 = BASE.index("  vb = DAT_007d70ec[DAT_007d70c8];\n  if (off == 0)")
i1 = BASE.index("  *param_3")
HEAD, REST = BASE[:i0], BASE[i1:]


def tail(pre, v1, v2, in1="", in2=""):
    t = pre + """  if (off == 0) {
@IN1@    hr = (*(vfn_t *)(*V1 + 0x2c))(V1,0,size,param_4,0x2800);
    DAT_007d70cc[DAT_007d70c8] = size;
    *param_5 = 0;
  }
  else {
@IN2@    hr = (*(vfn_t *)(*V2 + 0x2c))(V2,off,size,param_4,0x1800);
    DAT_007d70cc[DAT_007d70c8] = end;
    *param_5 = off / param_1;
  }
"""
    return t.replace("V1", v1).replace("V2", v2).replace("@IN1@", in1).replace("@IN2@", in2)


ASG = "    vb = " + G + ";\n"
TAILS = {
    "v": tail("  vb = " + G + ";\n", "vb", "vb"),
    "g": tail("", G, G),
    "b": tail("", "vb", "vb", ASG, ASG),
    "p": tail("  pvb = &" + G + ";\n", "(*pvb)", "(*pvb)"),
    "vg": tail("  vb = " + G + ";\n", "vb", G),
    "gv": tail("  vb = " + G + ";\n", G, "vb"),
}
VARIANTS = {}
for tn, t in TAILS.items():
    VARIANTS[tn] = (HEAD + t + REST).replace("  uint a0;", "  uint a0;\n  int **pvb;")
