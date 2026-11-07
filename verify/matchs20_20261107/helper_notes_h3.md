# MATCH-S20 helper h3 notes

## Per-function log (appended as finished)
0x004cc790  80  100.0  100.0  cxx:ok  -  RwError struct local
0x004ccce0  254  100.0  100.0  cxx:ok  -  freelist struct, byte-cast bit test
0x004cd070  196  100.0  100.0  cxx:ok  -  module struct, error branch first
0x004cd140  47  100.0  100.0  cxx:ok  -  memset intrinsic on the request record
0x004ccf20  312  87.7  88.7  cxx:ok  b  part-2 unlink address form (lea/add of &link fields) not found
0x004cd170  318  100.0  100.0  cxx:ok  -  case body order 3,5,4,1,2, store order
0x004cd2d0  319  100.0  100.0  cxx:ok  -  same shape as 004cd170, /G5
0x004cd430  198  100.0  100.0  cxx:ok  -  request struct, success branch first
0x004cd500  71  83.3  100.0  cxx:ok  b  global load order in the re-read after the store
0x004ce2d0  186  100.0  100.0  cxx:ok  -  module struct, memcpy intrinsic, /G5
0x004ce390  240  100.0  100.0  cxx:ok  -  format-list struct, readFn test before ctx fill
0x004cde50  198  100.0  100.0  cxx:ok  -  indexed loops give the +2-biased pointer, /G5
0x004ce6c0  39  100.0  100.0  cxx:ok  -  callback as a function, not a global
0x004ce790  328  100.0  100.0  cxx:ok  -  format struct, while(fmt && !found) loop
0x004ceaf0  162  100.0  100.0  cxx:ok  -  RwImage struct, memcpy intrinsic
0x004ceba0  271  100.0  100.0  cxx:ok  -  block-scoped RwError shares the frame slot, unsigned switch
0x004ce970  375  71.5  38.7  cxx:ok  b  image/mask register swap, cross-jumped destroy tail
0x004cc230  440  48.0  45.4  cxx:ok  b  result register not threaded in the original, RwError slots not shared
0x004c8e50  579  100.0  100.0  cxx:ok  -  local RwStandard initialiser, guard through element 0
0x004c8c70  472  66.2  89.7  cxx:ok  a  MXCSR AND narrowed to and ch,0x7f by 9176, register cascade
0x004cfa00  967  100.0  100.0  cxx:ok  -  one struct-array global, entry order with entry 81 after 83
0x004cd550  644  50.0  60.4  cxx:ok  b  if(d) h=d; else h=0 folded into one copy by ours, kept by the original
0x004ce480  333  89.7  89.7  cxx:ok  b  epilogue placement of the NULL returns
0x004d1020  1460  42.1  57.6  cxx:ok  b  per-case local packing into a 0x48 frame not reproduced, out of budget

## Final list (brief order)
0x004cc790  80  100.0  100.0  cxx:ok  -  RwError struct local
0x004ccce0  254  100.0  100.0  cxx:ok  -  freelist struct, byte-cast bit test
0x004ccf20  312  87.7  88.7  cxx:ok  b  part-2 unlink address form (lea/add of &link fields) not found
0x004cd070  196  100.0  100.0  cxx:ok  -  module struct, error branch first
0x004cd140  47  100.0  100.0  cxx:ok  -  memset intrinsic on the request record
0x004cd170  318  100.0  100.0  cxx:ok  -  case body order 3,5,4,1,2, store order
0x004cd2d0  319  100.0  100.0  cxx:ok  -  same shape as 004cd170, /G5
0x004cd430  198  100.0  100.0  cxx:ok  -  request struct, success branch first
0x004cd500  71  83.3  100.0  cxx:ok  b  global load order in the re-read after the store
0x004cde50  198  100.0  100.0  cxx:ok  -  indexed loops give the +2-biased pointer, /G5
0x004ce2d0  186  100.0  100.0  cxx:ok  -  module struct, memcpy intrinsic, /G5
0x004ce390  240  100.0  100.0  cxx:ok  -  format-list struct, readFn test before ctx fill
0x004ce480  333  89.7  89.7  cxx:ok  b  epilogue placement of the NULL returns
0x004ce6c0  39  100.0  100.0  cxx:ok  -  callback as a function, not a global
0x004ce790  328  100.0  100.0  cxx:ok  -  format struct, while(fmt && !found) loop
0x004ce970  375  71.5  38.7  cxx:ok  b  image/mask register swap, cross-jumped destroy tail
0x004ceaf0  162  100.0  100.0  cxx:ok  -  RwImage struct, memcpy intrinsic
0x004ceba0  271  100.0  100.0  cxx:ok  -  block-scoped RwError shares the frame slot, unsigned switch
0x004cc230  440  48.0  45.4  cxx:ok  b  result register not threaded in the original, RwError slots not shared
0x004c8c70  472  66.2  89.7  cxx:ok  a  MXCSR AND narrowed to and ch,0x7f by 9176, register cascade
0x004c8e50  579  100.0  100.0  cxx:ok  -  local RwStandard initialiser, guard through element 0
0x004cd550  644  50.0  60.4  cxx:ok  b  if(d) h=d; else h=0 folded into one copy by ours, kept by the original
0x004cfa00  967  100.0  100.0  cxx:ok  -  one struct-array global, entry order with entry 81 after 83
0x004d1020  1460  42.1  57.6  cxx:ok  b  per-case local packing into a 0x48 frame not reproduced, out of budget
