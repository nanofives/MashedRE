#!/bin/bash
# PORT-COLOURSEL-LEFTRIGHT arm 1 (original): menu_draw_burst.py --nav to the cup-flow colour select, then right / left,
# peeking the choice 0x0067f0c0, the exe-side cursor 0x0067ea98 and the 0x0067eaf0 records. Inside test-slot + mashed_lock.
O=D:/MashedWT/PORT-COLOURSEL-LEFTRIGHT_data
P=0x0067f0c0,0x0067ea98,0x0067eaf0,0x0067eafc,0x0067ed70,0x0067f0c4
export MASHED_ROOT=D:/MashedWT/PORT-TOP-04D-WALK04_root
py -3.12 re/frida/menu_draw_burst.py --nav confirm,confirm,confirm,right --frames 2 --label r1 --out $O/lr_r1.json --peek $P
py -3.12 re/frida/menu_draw_burst.py --nav confirm,confirm,confirm,right,right,right,right,right,right --frames 2 --label r6 --out $O/lr_r6.json --peek $P
py -3.12 re/frida/menu_draw_burst.py --nav confirm,confirm,confirm,right,right,left --frames 2 --label rrl --out $O/lr_rrl.json --peek $P
py -3.12 re/frida/menu_draw_burst.py --nav confirm,confirm,confirm,left --frames 2 --label l1 --out $O/lr_l1.json --peek $P
