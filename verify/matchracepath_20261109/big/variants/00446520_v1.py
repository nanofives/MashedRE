V = [
 ("dblR", [("vln.x += vah.x * param_1->f9a4;", "vln.x = (float)(vln.x + vah.x * (double)param_1->f9a4);")]),
 ("dblL", [("vln.x += vah.x * param_1->f9a4;", "vln.x = (float)(vln.x + (double)vah.x * param_1->f9a4);")]),
 ("swap", [("vln.x += vah.x * param_1->f9a4;", "vln.x += param_1->f9a4 * vah.x;")]),
 ("dblswap", [("vln.x += vah.x * param_1->f9a4;", "vln.x = (float)(vln.x + (double)param_1->f9a4 * vah.x);")]),
 ("dblswap2", [("vln.x += vah.x * param_1->f9a4;", "vln.x += (float)((double)param_1->f9a4 * vah.x);")]),
 ("addL", [("vbr.x = param_1->a964.x + param_1->a970.x;", "vbr.x = (float)((double)param_1->a964.x + param_1->a970.x);")]),
 ("addR", [("vbr.x = param_1->a964.x + param_1->a970.x;", "vbr.x = (float)(param_1->a964.x + (double)param_1->a970.x);")]),
 ("subL", [("vbr.x = vby.x - param_1->a964.x;", "vbr.x = (float)((double)vby.x - param_1->a964.x);")]),
 ("subR", [("vbr.x = vby.x - param_1->a964.x;", "vbr.x = (float)(vby.x - (double)param_1->a964.x);")]),
]
