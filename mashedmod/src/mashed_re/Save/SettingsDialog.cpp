// Mashed RE - Save/SettingsDialog cluster (.asi only).
//
// Binary anchor: MASHED.exe size 2,846,720
//   SHA-256 BDCAE093A30FBF226BDD852B9C36798A987AEE33B3AE82BF7404B0336EFD3C0E
//   (preserved in original\MASHED.exe.unpatched)
//
// MATCH-S16 (2026-11-02): the three bodies this file held moved to wrappers that compile the byte-matched sources
// (re/match/s16/<va8>.c), each with its RH_ScopedInstall and Frida export name unchanged:
//   ReadModeFromCombo_j5     -> Match/MatchS16_00498d20.cpp (EAX adapter over ReadModeFromCombo_Body)
//   PopulateModeCombo_s2     -> Match/MatchS16_00498d60.cpp
//   VideoSettingsDlgProc_s2  -> Match/MatchS16_004991f0.cpp
// The C2 plates stay at re/analysis/promote_c2_settings_dialog/{00498d20,00498d60,004991f0}.md.
// No code remains here.
