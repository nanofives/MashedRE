# statediff capture format (MSD1)

Binary file, little-endian, produced by the per-frame vehicle-record capture
(`capture_vehicle.js` via the runner) on each side of an A/B run.

```
header:
  magic      4 bytes   "MSD1"
  rec_size   u32       bytes per snapshot record (vehicle record = 0xd04)
  base_va    u32       VA the snapshot was read from (for provenance)
  reserved   u32       0
records (repeated until EOF):
  frame_idx  u32       replay-clock frame index (render frames)
  payload    rec_size  raw bytes of the state surface at that frame
```

Frames are captured in increasing `frame_idx` order but may skip indices
(capture starts when the surface becomes valid). The differ aligns by
`frame_idx`, never by record ordinal.

Diff semantics (`statediff.py`):
- first divergent frame = lowest common frame_idx where payloads differ
- diverging bytes are clustered into dword-aligned fields (offset, width)
- each field reports first-divergence frame and both sides' values as
  hex / i32 / f32 — interpretation is left to the analyst (NO-GUESSING)
