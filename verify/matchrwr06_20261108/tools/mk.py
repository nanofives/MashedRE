"""mk.py <body.c> <dest.c> <header-note> : FLAGS line + note + preamble + body, CRLF."""
import sys
body = open(sys.argv[1]).read()
pre = open('D:/MashedWT/MATCH-RWR06_data/tools/preamble.txt').read().replace('\r\n', '\n')
t = '// FLAGS: /O2 /Ob1 /G5\n/* ' + sys.argv[3] + ' */\n' + pre.rstrip('\n') + '\n' + body.lstrip('\n')
open(sys.argv[2], 'w', newline='').write(t.replace('\n', '\r\n'))
