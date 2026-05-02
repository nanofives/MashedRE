# Ghidra Setup

Pinned record of how the master project was created. If any of these change, the project must be re-imported (analysis output isn't deterministic across Ghidra versions).

## Versions and paths

| Item | Pinned value |
|---|---|
| Ghidra | 12.0.3 PUBLIC |
| Ghidra install dir | `C:\Users\maria\Desktop\Proyectos\TD5RE\ghidra_12.0.3_PUBLIC\` (shared with TD5RE) |
| Java | bundled with Ghidra 12.0.3 |
| Project location | `C:\Users\maria\Desktop\Proyectos\Mashed` |
| Project name | `Mashed` (master) |
| Imported binary | `C:\Users\maria\Desktop\Proyectos\Mashed\original\MASHED.exe` |
| Binary SHA-256 | `BDCAE093A30FBF226BDD852B9C36798A987AEE33B3AE82BF7404B0336EFD3C0E` |
| Binary size | 2,846,720 bytes |
| Functions identified after auto-analysis | **5,800** (committed inventory at `re/analysis/function_inventory.csv`) |

## Analysis options used

The master was created via `analyzeHeadless` with **default analyzers only**. No custom checkboxes flipped. Headless analysis ran in 117 seconds.

```
analyzeHeadless C:/Users/maria/Desktop/Proyectos/Mashed Mashed
    -import C:/Users/maria/Desktop/Proyectos/Mashed/original/MASHED.exe
    -analysisTimeoutPerFile 1800
```

Default 12.0.3 analyzers active for x86 PE32 include (excerpt from log):
- ASCII Strings
- Apply Data Archives
- Call-Fixup Installer
- DWARF
- Data Reference
- Decompiler Parameter ID **(yes — default in 12.0.3)**
- Demangler MS / Demangler GNU
- Disassemble Entry Points
- Embedded Media
- External Entry References
- Function ID
- Function Start Pre-Search / Start Search / Start Search After Code / Start Search After Data / Start Search Delayed - One Time
- Non-Returning Functions - Discovered / Known
- PDB Universal
- Reference / Subroutine References / Shared Return Calls
- Scalar Operand References
- Stack
- Windows x86 PE Exception Handling / RTTI Analyzer / TEB Analyzer
- WindowsResourceReference
- X86 Function Callee Purge
- x86 Constant Reference Analyzer

## To re-import (e.g., after Ghidra upgrade)

```bash
# 1. Move existing master out of the way (don't delete; archive)
mv Mashed.gpr Mashed.gpr.bak.YYYYMMDD
mv Mashed.rep Mashed.rep.bak.YYYYMMDD

# 2. Re-run headless analysis
bash "/c/Users/maria/Desktop/Proyectos/TD5RE/ghidra_12.0.3_PUBLIC/support/analyzeHeadless.bat" \
    "C:/Users/maria/Desktop/Proyectos/Mashed" Mashed \
    -import "C:/Users/maria/Desktop/Proyectos/Mashed/original/MASHED.exe" \
    -overwrite

# 3. Sync pool clones
bash scripts/ghidra_pool.sh sync
```

## Adding a `Mashed_headless` write target (optional)

```bash
bash "/c/Users/maria/Desktop/Proyectos/TD5RE/ghidra_12.0.3_PUBLIC/support/analyzeHeadless.bat" \
    "C:/Users/maria/Desktop/Proyectos/Mashed" Mashed_headless \
    -import "C:/Users/maria/Desktop/Proyectos/Mashed/original/MASHED.exe"
```

Use this if you want a separate project for headless MCP writes that won't interfere with GUI editing of `Mashed.gpr`.
