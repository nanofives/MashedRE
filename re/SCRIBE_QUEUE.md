# Scribe Queue

Buckets queued for the parallel-fanout sweep. See `re/SESSION_RULES.md` § "Parallel-fanout scribe-queue pattern" for the protocol.

Format per row:

```
YYYY-MM-DD  <SESSION_SHORT_ID>  bucket=<bucket>  rvas=<comma-separated list>
```

The sweep session moves rows from "Queued" to "Drained" as it processes them. Drained rows are kept for audit. Queued rows are never deleted — only moved.

## Queued

```
```

## Drained

```
```
