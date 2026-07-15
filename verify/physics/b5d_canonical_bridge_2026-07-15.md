# B5d canonical acceptance â€” bridge FUN_0047eb30 + 8 B5c hooks (2026-07-15)

Command:
```
py -3.12 re/frida/scenario_launch.py --track 0 --mode 10 --cars 4 --hold 35 \
  --hooks 0x0047eb30,0x0055dff0,0x0055b800,0x0055ac00,0x0055deb0,0x0057c210,0x0055c000,0x0055e200,0x0047ea40
```

Transcript (pid 30268, exit 0):
```
=== scenario_launch  pid=30268  track=0 mode=10 cars=4 ===
  attaching ASAP...
  [agent] ready
    phase=0  (waiting for menu (phase 1))
    phase=1  (waiting for menu (phase 1))
  [setup] set track=0 mode=10 cars=4 car=0 rule=0 team=0
  [launch] poke DAT_00771968 = 2 -> 1
    phase=2  (waiting for race running (phase 3))
    phase=3  (waiting for race running (phase 3))

  *** RACE RUNNING (phase 3) ***
  car spawn fired: 12   grounded=4  airflag=0
  vel=[0, 0, 0]  fwd=[-8.742277657347586e-08, 0, -1]

  VERDICT: launcher reached a running race and spawned a car. [OK]

  racing 35s — pulsing control 4 (confirm/accel) to skip the start intro + continue rounds...
    +  4s  spawnFired=12  p0.grounded=4 airflag=1  vel=[358.5, 0, -1302.2]       +  9s  spawnFired=12  p0.grounded=4 airflag=1  vel=[-83.7, 0.2, 331.1]       + 13s  spawnFired=12  p0.grounded=4 airflag=1  vel=[2328.2, -23.2, 2478.8]       + 18s  spawnFired=12  p0.grounded=0 airflag=1  vel=[0, 0, 0]       + 23s  spawnFired=12  p0.grounded=0 airflag=1  vel=[0, 0, 0]       + 28s  spawnFired=12  p0.grounded=0 airflag=1  vel=[0, 0, 0]       + 33s  spawnFired=12  p0.grounded=0 airflag=1  vel=[0, 0, 0]   
```
