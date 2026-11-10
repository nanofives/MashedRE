D = "    float x2, y2;\n"
X = "        x2 = p.x + s.x;\n"
END = "        FUN_0045b350(0);\n    }\n}"
V = [
 ("blk_xy", [(D, ""), (X, "        {\n        float x2, y2;\n" + X), (END, "        FUN_0045b350(0);\n        }\n    }\n}")]),
 ("blk_y", [(D, "    float x2;\n"), (X, "        {\n        float y2;\n" + X), (END, "        FUN_0045b350(0);\n        }\n    }\n}")]),
 ("blk_yx", [(D, ""), (X, "        {\n        float y2, x2;\n" + X), (END, "        FUN_0045b350(0);\n        }\n    }\n}")]),
 ("decl_yx", [(D, "    float y2, x2;\n")]),
 ("decl_uv_after", [("    float uu, vv;\n    float x2, y2;\n", "    float x2, y2;\n    float uu, vv;\n")]),
 ("ab_outer", [("        {\n            RwV2d a, b;\n", "        {\n"), ("    float x2, y2;\n", "    float x2, y2;\n    RwV2d a, b;\n")]),
]
