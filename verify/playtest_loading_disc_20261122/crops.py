"""Crop the disc region (x 496..592, y 36..124, the 80x80 quad at centre 544 / top 40 plus margin) to PNGs,
and build a side-by-side montage: original | before | after, for the front and back pairs."""
import os
from PIL import Image

S = os.path.dirname(os.path.abspath(__file__))
BOX = (496, 36, 592, 124)
pairs = {
    'back': ('orig/orig_t084.bmp', 'cap_before/before_001.bmp', 'cap_after/after_006.bmp'),
    'front': ('orig/orig_t069.bmp', 'cap_before/before_002.bmp', 'cap_after/after_000.bmp'),
}
rows = []
for name, files in pairs.items():
    crops = []
    for tag, f in zip(('orig', 'before', 'after'), files):
        c = Image.open(os.path.join(S, f)).convert('RGB').crop(BOX)
        c.save(os.path.join(S, 'crop_%s_%s.png' % (name, tag)))
        crops.append(c)
    rows.append(crops)
w, h = BOX[2] - BOX[0], BOX[3] - BOX[1]
m = Image.new('RGB', (w * 3 * 2, h * len(rows) * 2))
for r, crops in enumerate(rows):
    for i, c in enumerate(crops):
        m.paste(c.resize((w * 2, h * 2)), (i * w * 2, r * h * 2))
m.save(os.path.join(S, 'disc_orig_before_after.png'))
print('montage', os.path.join(S, 'disc_orig_before_after.png'))
