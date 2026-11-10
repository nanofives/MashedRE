o="vbr.x = vby.x - param_1->a964.x;\n    vbr.y"
R=lambda e: e+";\n    vbr.y"
forms = {
 "comma": R("vbr.x = vby.x - (0, param_1->a964.x)"),
 "uplus": R("vbr.x = vby.x - +param_1->a964.x"),
 "fcast": R("vbr.x = vby.x - (float)param_1->a964.x"),
 "vol": R("vbr.x = vby.x - *(volatile float *)&param_1->a964.x"),
 "cond": R("vbr.x = vby.x - (1 ? param_1->a964.x : 0.0f)"),
 "dbl_both_f": R("vbr.x = (float)vby.x - (float)(double)param_1->a964.x"),
 "dd": R("vbr.x = (float)((double)vby.x - (float)(double)param_1->a964.x)"),
 "neg_add": R("vbr.x = (float)(-(double)param_1->a964.x + vby.x)"),
 "subRl": R("vbr.x = vby.x - (float)(long double)param_1->a964.x"),
 "dright": R("vbr.x = (float)(vby.x - (double)(param_1->a964.x))"),
}
V=[(k,[(o,v)]) for k,v in forms.items()]
