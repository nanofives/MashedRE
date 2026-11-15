import sys
sys.path.insert(0,'re/tools/match')
import equiv
s=dict(equiv.summary(0x5ae800)); s['N']=2; s['n_why']=False
equiv._SUMM[0x5ae800]=s; equiv._TOP[0x5ae800]=s
sys.argv=['equiv.py','re/match/s26/005ae650.c','--only','0x005ae650']
equiv.main()
