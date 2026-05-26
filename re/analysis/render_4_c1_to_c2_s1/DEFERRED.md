# Deferred RVAs — batch-render-4-s1

These 4 RVAs had no function defined in Ghidra at their address (error: "no function found").
Per hooks.csv they are LAB_ label entries (rwID_CAMERAMODULE and rwID_VECTORMODULE plugin ctor/dtor).

| RVA       | hooks.csv name                         | Reason                                   |
|-----------|----------------------------------------|------------------------------------------|
| 004c1940  | sub_004c1940 (rwID_CAMERAMODULE dtor)  | No Ghidra function at this address; LAB_ |
| 004c1980  | sub_004c1980 (rwID_CAMERAMODULE ctor)  | No Ghidra function at this address; LAB_ |
| 004c3e20  | sub_004c3e20 (rwID_VECTORMODULE dtor)  | No Ghidra function at this address; LAB_ |
| 004c3e90  | sub_004c3e90 (rwID_VECTORMODULE ctor)  | No Ghidra function at this address; LAB_ |

These are already documented at C1 in re/analysis/librw_plugin_compat/REPORT.md (U-2887).
A listing-level disassembly session is required to promote these to C2.
