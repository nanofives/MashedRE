H = "            font->height = (float)(y1 - y0);\n"
V = [
 ("bufslot", [(H, "            buf = (char *)(y1 - y0);\n            font->height = (float)(int)buf;\n")]),
 ("fileslot", [(H, "            file = y1 - y0;\n            font->height = (float)file;\n")]),
 ("y1slot", [(H, "            y1 = y1 - y0;\n            font->height = (float)y1;\n")]),
]
