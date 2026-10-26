# WIP run 11 (held): matched-speed decel, first read of existing captures (no new runs)

- Low-speed constant decel per frame: original 5.1 to 5.2 per frame on BOTH captures (ww3 about 19 fps:
  71.05 -> 65.93 -> 60.79 ... 24.98 -> 19.87 -> 0, b0c ~0; o60 about 60 fps: 30.77 -> 25.68 -> 20.57 -> 0).
  So the decel is per frame, not dt-scaled. Port X1: 10.2 to 10.4 per frame from about 600 down to 25.3 -> 0. Ratio 2.0.
- Snap to 0.0: both sides zero the speed when the next value would be about 15 (orig 20.57 - 5.1, port 25.3 - 10.2).
- Original's early fast decel after a kill (175 -> 145 -> 122 -> 105) runs with b0c 106 / 70 / 47 (skid term), port's
  b0c stays near 0, so the high-speed part is not comparable without matched slip.
- Next: find why the port's constant term is 2x (candidates to check: dead car stepped twice per frame by the opponent
  loop plus another path, substep count, or a +0x4 == 0 dependent term). Then slot 2 brake-byte pairing.
