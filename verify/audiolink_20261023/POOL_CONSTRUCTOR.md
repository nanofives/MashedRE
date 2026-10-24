# Audio pool constructor chain (audiolink, 2026-10-23)

Static, from MASHED.exe.unpatched: `xref_scan.py` / `caller_scan.py` / `disasm_slice1.py` /
`read_bytes.py` in this folder. Raw outputs: `xref_pools.txt`, `callers_pools.txt`, `disasm_pools.txt`.

## Who writes the pool globals

All five globals the survey asked about are BSS (zero at load). Each has exactly one init / shutdown pair, and
every init calls AudioPoolConstruct 0x005ae650.

| Global(s) | Init | Shutdown | 0x005ae650 args (as pushed, last first) | "Inited" flag |
|---|---|---|---|---|
| head 0x007dda80 / +4, pool 0x007dda88, result 0x007ddab0 | 0x005ae5b0 | 0x005ae610 | 0x24, 0x10, 4, 0, &0x007dda88, 0x40800 | 0x007ddaac |
| 0x007dda28 | 0x005adf90 | 0x005adfc0 | 0x10, 8, 4, 0, &0x007dda28, 0x40802 | 0x007dda4c |
| 0x009146c0 | 0x005add80 | 0x005addb0 | 0xc, 0x20, 0, 0, &0x009146c0, 0x40804 | 0x007dda24 |
| 0x007dd634 | 0x005ab9d0 | 0x005aba00 | 0x5c, 0x10, 4, 0, &0x007dd634, 0x40806 | 0x007dd658 |

- 0x005ae5b0 makes 0x007dda80 a self-referencing list head before constructing (`mov eax,0x7dda80 / mov
  [0x7dda80],eax / mov [0x7dda84],eax`, 0x005ae5be..0x005ae5cc). It then stores the 0x005ae650 result in 0x007ddab0
  and unlinks that block from the list (0x005ae5f2..0x005ae60a).
- 0x005ae610 destroys every block still linked at 0x007dda80, then 0x007ddab0, all through 0x005ae7e0, and zeroes
  0x007ddab0 / 0x007ddaac.
- The shutdowns all call 0x005ae7e0. AudioRws.cpp's AudioPoolDestroy is 0x005ae780, a different RVA, so 0x005ae7e0
  has no port [UNCERTAIN: its relation to 0x005ae780 is not read].

## Who calls the inits

- The inits / shutdowns are never called directly. They are entries of a 16-pair {init, shutdown} table at
  0x00632ba8 (.data): (005a5e70, 005a5ee0) (005ae5b0, 005ae610) (005ae1d0, 005ae210) (005adf90, 005adfc0)
  (005add80, 005addb0) (005ada90, 005adb70) (005a9970, 005a9a70) (005aa480, 005aa520) (005acff0, 005ad030)
  (005ac550, 005ac590) (005a7810, 005aba00) (005ab9d0, 005a78b0) (005a6e50, 005a6e90) (005ab4a0, 005ab510)
  (005a6af0, 005a6b30) ...
  - In two rows the second word is not the matching shutdown: (005a7810, 005aba00) and (005ab9d0, 005a78b0). Recorded
    raw. The walkers' pairing rule is not read [UNCERTAIN].
- **Open:** 0x005a5f00 pushes (table 0x00632ba8, count 0x10, 0x005a5fd0, 0x005a6010) into 0x005a5f60. On success it
  sets 0x007dc94c = 1. It is called once, from 0x0046280e inside 0x004627f0, which also writes 0x00632ba0
  (0x00462813).
- **Close:** 0x005a6030 pushes (0x00632ba8, 0x10, 0x005a6010) into 0x005a6070. It is reached by `jmp` from 0x00466b40.

## Consequence for the exe arm of the AudioRws pool / list family

Before the audio open runs, every one of these globals is its BSS zero. Private zero storage in the exe arm is
therefore the original's pre-open state. To make the pool family live in the exe, a port of the open chain is needed
(0x005a5f00 / 0x005a5f60 and the init table): 0x005ae5b0 for the self-linked head, 0x005ae650 for each pool. The exe's
own AudioEngine does not use these pools, so the family stays dead code in the exe until that port exists.
