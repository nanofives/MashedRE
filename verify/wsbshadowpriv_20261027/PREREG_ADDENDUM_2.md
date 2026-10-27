# PREREG D4-WSB-SHADOW-PRIVATE, addendum 2 (committed alone, before arm N2)

Arm N (`0x00564040`, NEED_PAGES): 0 counted samples in the 60 s hold. The 64 logged EMPTY lines all read `rv=00000000`,
1 NOISY sample. Discards after the 64th were not logged, so N cannot tell NO-WRITES-SAMPLED from BUFFER-UNTRACKED.

Arm N2 = arm N with one logging change: an EMPTY line is also written for every discard with rv != 0 (up to 256) and
for every 250th discard (running total). Read-out of addendum 1 applies unchanged: any EMPTY with rv > 0 means
BUFFER-UNTRACKED; EMPTY only with rv = 0 means NO-WRITES-SAMPLED; a counted HEAP-CLEAN sample set means HEAP-CLEAN.
