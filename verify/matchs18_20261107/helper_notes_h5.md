0x004b61c0  701  95.9%  95.9%  cxx:ok  (a)  9176 unrolls the vertex loop x8 (orig x4); hand x4 unroll matches all code but adds a 2-byte loop-top align pad
0x004b5d90  816  60.1%  60.1%  cxx:ok  (b)  x87 scheduling of the 24 box-corner stores: original dups negs with fld st(0), routes copies via int temps packed over the white-colour slot, keeps matrix consts after; shape not found
