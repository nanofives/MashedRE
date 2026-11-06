import time, sys
t = time.time(); f = 'D:/MashedWT/MATCH-S16_data/gates.log'
def body():
    return [l for l in open(f, errors='replace').read().splitlines() if 'WAITING' not in l and l.strip()]
b0 = body()
while time.time() - t < 570:
    if body() != b0: break
    time.sleep(30)
print('\n'.join(body()[-12:]) or 'still waiting')
