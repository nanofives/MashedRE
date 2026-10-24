import json, sys, time
led = r'C:\Users\maria\Desktop\Proyectos\Mashed\log\orch\roadmap.json'
rid, stages, limit = sys.argv[1], sys.argv[2].split(','), int(sys.argv[3])
t0 = time.time(); last = None
while time.time() - t0 < limit:
    try:
        r = [x for x in json.load(open(led, encoding='utf-8-sig'))['rows'] if x['id'] == rid][0]
    except Exception:
        time.sleep(5); continue
    if r['stage'] != last:
        print(f"{int(time.time()-t0)}s {rid} -> {r['stage']} owner={r.get('owner')}", flush=True); last = r['stage']
    if r['stage'] in stages:
        sys.exit(0)
    time.sleep(10)
sys.exit(1)
