D = "dot = fr->m.at.x * b.at.x + fr->m.at.y * b.at.y + fr->m.at.z * b.at.z;"
VARIANTS = [("Lzxy", [(D, "dot = (float)((double)fr->m.at.z * b.at.z + (double)fr->m.at.x * b.at.x + (double)fr->m.at.y * b.at.y);")])]
