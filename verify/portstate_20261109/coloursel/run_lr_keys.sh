#!/bin/bash
# PORT-COLOURSEL-LEFTRIGHT arm 6: exe left/right with MASHED_COLOURSEL_ICON on (and off, control). Inside test-slot + mashed_lock.
O=D:/MashedWT/PORT-COLOURSEL-LEFTRIGHT_data; X=$O/exe/mashed_re.exe; R=D:/MashedWT/PORT-COLOURSEL-LEFTRIGHT_root
py -3.12 lr_exe_poll.py --exe $X --root $R --out $O/lr_exe_on.json --keys e,e,r,r,r,r,r,r,l --env MASHED_COLOURSEL_ICON=1
py -3.12 lr_exe_poll.py --exe $X --root $R --out $O/lr_exe_off.json --keys e,e,r,r,r,r,r,r,l
