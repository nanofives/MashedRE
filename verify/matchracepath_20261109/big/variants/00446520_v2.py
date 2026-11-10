o="vbr.x = vby.x - param_1->a964.x;\n    vbr.y"
forms = {
 "ff": "vbr.x = vby.x - (float)(double)param_1->a964.x;\n    vbr.y",
 "subR": "vbr.x = (float)(vby.x - (double)param_1->a964.x);\n    vbr.y",
 "subRR": "vbr.x = (float)((double)vby.x - (double)param_1->a964.x);\n    vbr.y",
 "ld": "vbr.x = (float)(vby.x - (long double)param_1->a964.x);\n    vbr.y",
 "neg": "vbr.x = -param_1->a964.x + vby.x;\n    vbr.y",
 "neg2": "vbr.x = -(param_1->a964.x - vby.x);\n    vbr.y",
 "plusneg": "vbr.x = vby.x + -param_1->a964.x;\n    vbr.y",
 "parr": "vbr.x = vby.x - (&param_1->a964)->x;\n    vbr.y",
 "idx": "vbr.x = vby.x - (&param_1->a964.x)[0];\n    vbr.y",
 "dbl_assign": "vbr.x = (float)(vby.x - (double)(float)param_1->a964.x);\n    vbr.y",
}
V=[(k,[(o,v)]) for k,v in forms.items()]
