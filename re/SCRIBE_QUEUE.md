# Scribe Queue

Buckets queued for the parallel-fanout sweep. See `re/SESSION_RULES.md` § "Parallel-fanout scribe-queue pattern" for the protocol.

Format per row:

```
YYYY-MM-DD  <SESSION_SHORT_ID>  bucket=<bucket>  rvas=<comma-separated list>
```

The sweep session moves rows from "Queued" to "Drained" as it processes them. Drained rows are kept for audit. Queued rows are never deleted — only moved.

## Queued

```
2026-05-02  boot_app_init-20260502-1724  bucket=boot_app_init  rvas=0x00402750,0x00402a40,0x00492270,0x00492290,0x004924f0,0x00493540,0x00493550,0x00493560,0x00493900,0x004963e0,0x004996f0,0x00499ba0,0x00499cc0,0x004c5930,0x005c9d00
```

## Drained

```
```
