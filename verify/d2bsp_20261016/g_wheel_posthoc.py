# POST-HOC (not registered): G-WHEEL-F with every wheel-call whose original next-entry is the hold pattern
# (query frame < 1000, fv 00000000, +0x204 3f800000, state 1, any key) left out.
import csv, bisect, sys
csv.field_size_limit(1 << 30)
Q, O = {}, []
for r in csv.DictReader(open(sys.argv[1], newline='')):
    s = int(r['seq'])
    if r['kind'] == 'Q': Q[s] = r
    elif r['kind'] == 'O': O.append((s, r['cx'].split(';')))
O.sort(); os_ = [s for s, _ in O]
PART = {0: 1, 1: 0, 2: 3, 3: 2}
for rep in sys.argv[2:]:
    R = {int(r['seq']): r for r in csv.DictReader(open(rep, newline=''))}
    for arm in ('clsW', 'clsS'):
        n = ok = held = held_ok = 0
        for seq in sorted(Q):
            if seq not in R: continue
            j = bisect.bisect_right(os_, seq)
            if j >= len(O): continue
            out = O[j][1]; c = R[seq][arm].split(';'); fr = int(Q[seq]['frame'])
            for w in range(4):
                ow = out[w*6:(w+1)*6]; own = c[w*6:(w+1)*6]; par = c[PART[w]*6:(PART[w]+1)*6]
                sel = [0, 1, 3, 4, 5]
                good = [ow[k] for k in sel] in ([own[k] for k in sel], [par[k] for k in sel])
                hold = fr < 1000 and ow[0] == '00000000' and ow[4] == '3f800000' and out[24+w] == '1'
                if hold: held += 1; held_ok += good; continue
                n += 1; ok += good
        print(f'{rep} {arm}: excluded {held} (of which matched {held_ok}); G-WHEEL-F rest {ok}/{n} = {100.0*ok/n:.2f} %')
