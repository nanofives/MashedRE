B1 = "DOT(&v, &m->up)"
B2 = "DOT(&v, &zaxis)"
B3 = "DOT(&v, &m->right)"
b1 = [(B1, "(0, m->up.x) * v.x + v.y * m->up.y + v.z * m->up.z")]
b3 = [(B3, "v.x * m->right.x + v.y * m->right.y + (0, m->right.z) * v.z")]
b2s = {
 "a": "(0, v.x * zaxis.x) + v.y * zaxis.y + v.z * zaxis.z",
 "b": "v.x * zaxis.x + (0, v.y * zaxis.y) + v.z * zaxis.z",
 "c": "(v.y * zaxis.y + v.x * zaxis.x) + v.z * zaxis.z",
 "d": "(0, v.x) * zaxis.x + v.y * zaxis.y + v.z * zaxis.z",
 "e": "v.x * zaxis.x + (0, v.y) * zaxis.y + v.z * zaxis.z",
 "f": "(0, zaxis.x) * v.x + (0, zaxis.y) * v.y + v.z * zaxis.z",
}
V = [("b1", b1), ("b3", b3), ("b13", b1 + b3)] + [("b13_2" + k, b1 + b3 + [(B2, s)]) for k, s in b2s.items()]
