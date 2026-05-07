# track_loader_d3 — SESSION_END

**Session:** track_loader_d3-20260506  
**Pool slot used:** Mashed_pool0 (pool5 was stale; pool0 current)  
**SHA-256 verified:** BDCAE093... ✓  
**Bucket:** re/analysis/track_loader_d3/  
**Parent:** track_loader_d2 (D-2620..D-2645, bucket track_loader_d2-cont1)

## Functions analyzed (26 total — all C1)

| Address | Stub cleared | Notes |
|---------|-------------|-------|
| 0x0042a640 | S-0912, D-2620 | BSP world file loader; appends ".bsp" |
| 0x0042a5d0 | S-0913, D-2621 | DFF clump file loader; appends ".dff" |
| 0x0042a740 | S-0914, D-2622 | UVA animation file loader; appends ".uva" |
| 0x0042a7f0 | S-0915, D-2623 | Spline file loader; appends ".spl" |
| 0x0042a860 | S-0916, D-2624 | Animation path file loader; appends ".anm" |
| 0x00478200 | S-0918, D-2625 | Course load fatal error: log + no-return exit |
| 0x004790e0 | D-2626 | Course post-load thunk → FUN_004e5c70(course, 0x479030, 0) |
| 0x00474fd0 | S-0917, D-2627 | Sky dome clump node iterator wrapper |
| 0x00426060 | D-2628 | Getter: returns DAT_0065742c |
| 0x00426070 | D-2629 | Getter: returns DAT_00656ee8 |
| 0x0047f840 | D-2630 | Physics world lazy-init; gravity (0,−9.8,0) |
| 0x0047f940 | D-2631 | Cylinder physics body creator |
| 0x0047fc40 | D-2632 | Box physics body creator |
| 0x0047fe00 | D-2633 | Sphere physics body creator |
| 0x0047ff70 | D-2634 | Cone/capsule physics body creator |
| 0x00480100 | D-2635 | Physics body post-init; finalize + broadphase |
| 0x0047ce40 | S-0907, D-2636 | Physics body slot lookup by polygon ID (scan 200) |
| 0x004b5030 | S-0908, D-2637 | AI blob chunk query via FUN_004e5c70 |
| 0x004b46b0 | S-0909 | Float3 vector equality check (stub desc was wrong) |
| 0x004785e0 | S-0910, D-2638 | Triangle cross-product with optional normalize (stub desc was wrong) |
| 0x004783f0 | S-0911, D-2639 | AI polygon heading angle calculator |
| 0x0047bf70 | S-0919, D-2640 | Collision sector record builder from RpWorldSector |
| 0x00491590 | D-2641 | Rain particle field initializer (→ RainColorInit) |
| 0x004cc5e0 | S-0900, D-2642 | RWS chunk scanner; size range 0x35000..0x37002 |
| 0x0045de80 | S-0904, D-2643 | Track audio zone-to-channel mapper |
| 0x0045e2a0 | S-0905, D-2644 | Track-0 audio waypoint init (15 pts, type 2) |
| 0x0045e160 | S-0906, D-2645 | Track-0x24 audio waypoint init (11 pts, type 1) |

## New stubs generated (U-1947..U-1962)

| ID | Address | Description |
|----|---------|-------------|
| U-1947 | 0x004b3c60 | BSP/RpWorld stream reader |
| U-1948 | 0x00558df0 | UVAnim chunk loader |
| U-1949 | 0x004b3cc0 | Spline stream reader |
| U-1950 | 0x004b3de0 | Animation stream reader |
| U-1951 | 0x00479030 | Course post-load callback table |
| U-1952 | 0x00474fb0 | DFF clump node iterator |
| U-1953 | 0x00474f30 | Sky dome per-node callback |
| U-1954 | 0x0047f4c0 | Physics world constructor |
| U-1955 | 0x0047d080 | Activate physics body slot |
| U-1956 | 0x0047d100 | Secondary enable physics body |
| U-1957 | 0x00487280 | Broadphase body registration |
| U-1958 | 0x0047be80 | Triangle mesh init |
| U-1959 | 0x0047bcc0 | Collect portal/neighbor list |
| U-1960 | 0x004b53b0 | Bounding sphere builder |
| U-1961 | 0x004c3d90 | Sector bounding geometry builder |
| U-1962 | 0x00546380 | Audio waypoint set constructor |

## Deferred to track_loader_d3-cont1 (D-5740..D-5755)

All 16 new stubs deferred to next depth session.

## Corrections to prior stub descriptions

- S-0909 (0x004b46b0): was "vertex in-triangle test" — actual: float3 equality check
- S-0910 (0x004785e0): was "AI polygon-start flag setter" — actual: triangle cross-product calculator
