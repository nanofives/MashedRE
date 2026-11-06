// Mashed RE - Save/SettingsAndIO_i3 cluster (.asi only).
//
// Binary anchor: MASHED.exe size 2,846,720
//   SHA-256 BDCAE093A30FBF226BDD852B9C36798A987AEE33B3AE82BF7404B0336EFD3C0E
//   (preserved in original\MASHED.exe.unpatched)
//
// The file wrappers 0x004b3b70 / 0x004b3bb0 moved to Save/FileWrappers.cpp (lane savelink2, 2026-10-23).
// MATCH-S16 (2026-11-02): the three video-dialog bodies this file held moved to wrappers that compile the
// byte-matched sources (re/match/s16/<va8>.c), each with its RH_ScopedInstall and Frida export name unchanged:
//   VideoDialogInit_i3             -> Match/MatchS16_00498f60.cpp (EAX adapter over VideoDialogInit_Body)
//   SubsystemSelectionChanged_i3   -> Match/MatchS16_00499170.cpp (EAX adapter over SubsystemSelectionChanged_Body)
//   SetControlTextFromResource_i3  -> Match/MatchS16_00499740.cpp
// The C2 plates stay at re/analysis/promote_c2_settings_dialog/{00498f60,00499170,00499740}.md.
// No code remains here.
