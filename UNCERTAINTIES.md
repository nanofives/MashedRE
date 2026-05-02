# Uncertainties

Every `[UNCERTAIN]` marker dropped in an analysis note or comment gets one row here. Uncertainties are not bugs — they are explicit holes in our knowledge with a path-to-resolution. Knowledge gaps without rows here are **invisible** and forbidden.

A function cannot reach C3 while it has unresolved uncertainties **of type semantic or structural** (see Type column). Cosmetic uncertainties (e.g., "name doesn't match RW convention exactly") may stay open through C4 if scoped tightly.

## Active uncertainties

| ID | Type | Where | Statement | Evidence missing | Path to resolution | Blocks |
|----|------|-------|-----------|------------------|--------------------|--------|
|    |      |       |           |                  |                    |        |

## Resolved (audit trail)

| ID | Type | Resolved date | Resolution |
|----|------|---------------|------------|
|    |      |               |            |

## Types

- **semantic** — what does this value mean? (e.g., "is +0x14 a velocity or an acceleration?")
- **structural** — what is the shape of this struct/buffer?
- **environmental** — does this depend on game state we don't track? (RNG seed, frame parity, save flag)
- **provenance** — is this code from RW SDK or game-specific?
- **cosmetic** — naming/style; doesn't block correctness

## Conventions

- ID format: `U-NNNN`, monotonic, never reused.
- Every `[UNCERTAIN]` in source/notes must cite a U-NNNN.
- Resolution must include the cited evidence (RVA, log line, diff output).
