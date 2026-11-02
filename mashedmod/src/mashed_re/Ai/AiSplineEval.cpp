// Mashed RE — FUN_00443300 AI Catmull-Rom spline eval (D4-DC-INC-ai, 2026-11-01; MATCH-S08, 2026-11-01).
//
// Before D4-DC-INC-ai the RVA had three bodies:
//   Ai/AiSplineHooks.cpp  .asi-only: AiSplineEval_x87 (naked verbatim, the installed body the C3 self-test and
//                         diff judged) and AiSplineEval (a portable C reimpl, 1-4 ULP off, never called; deleted)
//   AL_443300             exe-only, a private verbatim naked copy in Ai/AiLeafTargets_naked.inc, called 12 times
//                         by FUN_00443440 / FUN_00443dc0's transcriptions
// D4-DC-INC-ai made the naked AiSplineEval_x87 in this TU the one body for both targets. MATCH-S08 replaced it with
// the byte-matched source (below). The exe's naked callers `call AiSplineEval_x87` (re/tools/naked_specs/aileaf_g1.json);
// the .asi installs it through Ai/AiSplineHooks.cpp's AiSpline_Entry (self-test dispatcher), as before.
// The naked body read its six .rdata constants from exact local copies (the standalone's .rdata reads 0); the matched
// source uses the same values as float literals (3.0, 1.0, 0.5, 5.0, 4.0, 0.0).
//
// Anchored to MASHED.exe SHA-256
//   BDCAE093A30FBF226BDD852B9C36798A987AEE33B3AE82BF7404B0336EFD3C0E
//   (preserved in original\MASHED.exe.unpatched)

// MATCH-S08: the body of 0x00443300 (AiSplineEval_x87) moved to Match/MatchS08_00443300.cpp,
// which compiles the byte-matched source re/match/s08/00443300.c.
