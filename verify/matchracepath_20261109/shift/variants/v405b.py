B3 = "v.x * m->right.x + v.y * m->right.y + (0, m->right.z) * v.z"
V = [(k, [(B3, s)]) for k, s in {
 "p": "v.x * m->right.x + v.y * m->right.y + v.z * m->right.z",
 "q": "(0, v.x * m->right.x + v.y * m->right.y) + (0, m->right.z) * v.z",
 "r": "(0, v.x * m->right.x) + v.y * m->right.y + (0, m->right.z) * v.z",
 "s": "v.x * m->right.x + (0, v.y * m->right.y) + (0, m->right.z) * v.z",
 "t": "(0, (0, v.x * m->right.x) + v.y * m->right.y) + (0, m->right.z) * v.z",
 "u": "v.x * m->right.x + (0, v.y * m->right.y) + v.z * m->right.z",
 "w": "v.x * m->right.x + v.y * m->right.y + (0, m->right.z * v.z)",
}.items()]
