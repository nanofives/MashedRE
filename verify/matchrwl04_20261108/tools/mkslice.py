import csv
hk = {}
for x in csv.DictReader(open('hooks.csv', encoding='utf-8')):
    if x['rva'].startswith('#'):
        continue
    n = int(x['rva'], 16)
    n = n + 0x400000 if n < 0x400000 else n
    hk.setdefault(n, x)
rows = list(csv.DictReader(open('D:/MashedWT/MATCH-RWLIB_data/rwl04_rows.tsv'), delimiter='\t'))
with open('D:/MashedWT/MATCH-RWL04_data/slice_rows.tsv', 'w') as f:
    f.write('rva\tsub\tconf\texe_file\n')
    for r in rows:
        h = hk.get(int(r['rva'], 16), {})
        f.write(f"{r['rva']}\t{r['sub']}\t{r['conf']}\t{h.get('exe_file', '')}\n")
print(len(rows))
