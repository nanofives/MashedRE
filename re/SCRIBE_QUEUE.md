# Scribe Queue

Buckets queued for the parallel-fanout sweep. See `re/SESSION_RULES.md` § "Parallel-fanout scribe-queue pattern" for the protocol.

Format per row:

```
YYYY-MM-DD  <SESSION_SHORT_ID>  bucket=<bucket>  rvas=<comma-separated list>
```

The sweep session moves rows from "Queued" to "Drained" as it processes them. Drained rows are kept for audit. Queued rows are never deleted — only moved.

## Queued

```
2026-05-02  rw_engine_teardown-20260502-1440  bucket=rw_engine_teardown  rvas=0x004938c0,0x00558470,0x00550390,0x004c2f60,0x004c3040,0x004c3270
2026-05-02  rw_engine_init-20260502-1734  bucket=rw_engine_init  rvas=0x00493710,0x0045b350,0x00493600,0x00493640,0x004951e0,0x004951f0,0x00495270,0x004963e0,0x004a2cbd,0x004c2ed0,0x004c2f00,0x004c2fb0,0x004c3040,0x004c30b0,0x004c3270,0x004c32b0,0x004c5c80,0x004c9eb0
```

## Drained

```
2026-05-02  boot_app_init-20260502-1724  bucket=boot_app_init  rvas=0x00402750,0x00402a40,0x00492270,0x00492290,0x004924f0,0x00493540,0x00493550,0x00493560,0x00493900,0x004963e0,0x004996f0,0x00499ba0,0x00499cc0,0x004c5930,0x005c9d00
2026-05-02  boot_crt_exit-20260502-1733  bucket=boot_crt_exit  rvas=0x004a2c2f,0x004a3258,0x004a31b1,0x004a333c,0x004a40fe,0x004a415e,0x004a467e,0x004a57e4,0x004a774d,0x004a87f7,0x004aa3e4,0x004aa44f,0x004ab8d6,0x004aba4d
```
