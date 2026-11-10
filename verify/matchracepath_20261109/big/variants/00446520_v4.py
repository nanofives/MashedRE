A="vln.x += vah.x * param_1->f9a4;"
C="param_1->a970.x += vbr.x;"
D="vbr.x = param_1->a964.x + param_1->a970.x;"
V=[
 ("A_comma",[(A,"vln.x += vah.x * (0, param_1->f9a4);")]),
 ("A_cond",[(A,"vln.x += vah.x * (1 ? param_1->f9a4 : 0.0f);")]),
 ("A_swapcomma",[(A,"vln.x += (0, param_1->f9a4) * vah.x;")]),
 ("C_expl",[(C,"param_1->a970.x = param_1->a970.x + vbr.x;")]),
 ("C_comma",[(C,"param_1->a970.x = (0, param_1->a970.x) + vbr.x;")]),
 ("C_comma2",[(C,"param_1->a970.x += (0, vbr.x);")]),
 ("C_dbl",[(C,"param_1->a970.x = (float)((double)param_1->a970.x + vbr.x);")]),
 ("D_comma",[(D,"vbr.x = (0, param_1->a964.x) + param_1->a970.x;")]),
 ("D_commaR",[(D,"vbr.x = param_1->a964.x + (0, param_1->a970.x);")]),
 ("D_dbl",[(D,"vbr.x = (float)((double)param_1->a964.x + param_1->a970.x);")]),
]
