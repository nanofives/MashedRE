O = open("verify/matchracepath_20261109/big2/variants/dup_old.txt").read()
N = open("verify/matchracepath_20261109/big2/variants/dup_new.txt").read()
V = [("dup", [(O, N)]), ("duplt", [(O, N.replace("if (side[0] >= side[1]) {", "if (!(side[0] < side[1])) {"))])]
