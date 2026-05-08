import os, re, json, sys

BASE = 'C:/Users/maria/Desktop/Proyectos/Mashed/re/analysis'

def normalize(rva):
    r = rva.lower()
    if r.startswith('0x'): r = r[2:]
    return r.zfill(8)

def find_file(bucket, rva_raw):
    norm = normalize(rva_raw)
    d = os.path.join(BASE, bucket)
    for candidate in [f'0x{norm}.md', f'{norm}.md']:
        p = os.path.join(d, candidate)
        if os.path.exists(p): return p
    return None

def truncate(s, max_chars=120):
    if len(s) <= max_chars: return s
    cut = s[:max_chars]
    sp = cut.rfind(' ')
    if sp > 0: cut = cut[:sp]
    return cut + '…'

def extract_plate(path, bucket):
    with open(path, encoding='utf-8', errors='replace') as f:
        raw = f.read()
    text = re.sub(r'^---\s*\n.*?\n---\s*\n', '', raw, flags=re.DOTALL)

    if bucket == 'texture_loader_d3':
        m = re.search(r'^#[^#].*?FUN_\w+\s*\(([^)]+)\)', text, re.MULTILINE)
        if m: return m.group(1)
        m = re.search(r'^#[^#].*?[^\w]([A-Za-z]\w+(?:[A-Z]\w+)+)\s*$', text, re.MULTILINE)
        if m: return m.group(1)

    def first_from(names):
        for name in names:
            m = re.search(r'##\s+' + name + r'[^\n]*\n(.*?)(?:\n##|\Z)', text, re.DOTALL|re.IGNORECASE)
            if not m: continue
            block = re.sub(r'```.*?```', '', m.group(1), flags=re.DOTALL)
            for line in block.split('\n'):
                s = line.strip()
                if not s or s.startswith('|') or s.startswith('#'): continue
                if re.match(r'^[-|=]+$', s): continue
                c = re.sub(r'^[*\-]+\s*', '', s).strip()
                c2 = re.sub(r'^\*\*[\w\s]+\*\*[:\s]+', '', c).strip()
                if re.match(r'^(Fact|Address|RVA|Role|Name|Value|Size)\s*[|$]', c2, re.I): continue
                if len(c2) > 10: return c2
                if len(c) > 10: return c
        return None

    for group in [
        [r'Mechanical\w*\s+description', r'Mechanistic\s+description'],
        [r'Role', r'What it does', r'Summary'],
        [r'Analysis'],
        [r'Observations?', r'Description'],
    ]:
        r = first_from(group)
        if r: return r

    m = re.search(r'^#\s+\S+\s+[^\w\n]+\s*(.+)$', text, re.MULTILINE)
    if m:
        t = re.sub(r'^FUN_[0-9a-fA-F]+\s*[-—]*\s*', '', m.group(1)).strip()
        if t and len(t) > 3: return t

    for group in [[r'Signature', r'Notes?', r'Finding', r'Classification', r'Facts']]:
        r = first_from(group)
        if r: return r

    lines = text.split('\n')
    past_h1 = False; in_code = False
    for line in lines:
        s = line.strip()
        if s.startswith('```'): in_code = not in_code; continue
        if in_code: continue
        if s.startswith('# '): past_h1 = True; continue
        if not past_h1 or not s or s.startswith('#') or s.startswith('|'): continue
        if re.match(r'^[-|=]+$', s): continue
        if re.match(r'^\*\*[\w\s]+\*\*[:\s]', s): continue
        c = re.sub(r'^[*\-]+\s*', '', s).strip()
        if len(c) > 10: return c
    return 'NO_TEXT_FOUND'

buckets = {
  'input_lua_d3':        ['0x004ba1b0','0x004b7be0','0x004ba210','0x004b9850','0x004b64e0'],
  'render_d3d_reset_d2': ['004cb8a0','004cba80','004cc7f0','004ccc50','004ccde0','004d1d70','004d53b0','004d5480','004d54f0','004d5570','004db550','004dc8e0','004dc9e0','004e08b0'],
  'render_frame_d3':     ['00403050','00404320','0040de30','0040df20','0040df60','00410b30','00426030','00426670','004266b0','0042a9f0','0042d390','0042f530','00433f40','00492440','00492e60'],
  'render_pipeline_d2':  ['004c30b0'],
  'audio_sfx_dispatch_d2':['0045efe0','0045f5f0','0045faa0','0045ff50','00460350','00460df0','00461650','00463640','00463c80','00463f40','00464e10','00465a30','00465b20','004661f0'],
  'render_pipeline_d3':  ['004c4600','004c4a50','004c4dc0','004c4eb0','004c5010','004c51a0','00552d10','00552da0','00552df0','00552e40'],
  'rw_engine_init_d3':   ['00498a00','00498c00','00498e40','00498ea0','004a2b60','004a42c5','004ad1e0','004c2d70','004c2d90','004c2de0','004c2e10','004c2e40','004c2e70','004c2ea0','004c2f30','004c7690','004cfa00','004d7de0','004e7d40'],
  'rw_engine_teardown_d3':['0x004ccce0'],
  'texture_loader_d2':   ['0x0042a470','0x004cc5e0','0x004cf7d0','0x0054f8d0'],
  'texture_loader_d3':   ['0x004c5890','0x004c5bc0','0x004c77c0','0x004cbd30','0x004cc050','0x004cc400','0x004cc4f0','0x004cdd60','0x004cee90','0x004cefd0','0x004e1b60','0x00550130'],
  'title_screen_d2':     ['0x00401ee0','0x0040d250','0x004288a0','0x0042e590','0x0042f0b0'],
  'track_loader_d3':     ['0042a5d0','0042a640','0042a740','0042a7f0','0042a860','0045de80','00474fd0','00478200','004783f0','004785e0','004790e0','0047bf70','0047ce40','0047f840','00480100','00491590','004b46b0','004b5030','004cc5e0'],
  'vehicle_dynamics':    ['00467300','0046ef70','004709a0','004c3b90'],
  'vehicle_update_d2':   ['0040fc00','00420230','00422ba0','00425a40','0046da80','00470670','004709a0','00470c70','00480720','00480b70'],
  'vehicle_update_d3':   ['00467350','00467650','00468980','0046dc20','0046ddb0','0046f6c0','0047eb30','004809e0'],
  'video_mci_d2':        ['0x00493ac0','0x0049ec10','0x004a3b84'],
  'window_fullscreen':   ['00498bf0'],
}

result = {}
for bucket, rvas in buckets.items():
    result[bucket] = {}
    for rva_raw in rvas:
        norm = normalize(rva_raw)
        addr = '0x' + norm
        path = find_file(bucket, rva_raw)
        if not path:
            result[bucket][norm] = {'addr': addr, 'plate': None, 'missing': True}
        else:
            raw = extract_plate(path, bucket)
            plate = '[C1 2026-05-07] ' + truncate(raw)
            result[bucket][norm] = {'addr': addr, 'plate': plate, 'missing': False}

print(json.dumps(result, ensure_ascii=False, indent=2))
