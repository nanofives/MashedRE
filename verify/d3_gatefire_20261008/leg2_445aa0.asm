  00445aa0  sub    esp, 0x50
  00445aa3  fild   dword ptr [0x7f1008]
  00445aa9  push   ebx
  00445aaa  push   ebp
  00445aab  push   esi
  00445aac  push   edi
  00445aad  mov    edi, dword ptr [esp + 0x68]
  00445ab1  fmul   dword ptr [0x5cc948]
  00445ab7  lea    eax, [esp + 0x48]
  00445abb  push   edi
  00445abc  push   eax
  00445abd  fstp   dword ptr [esp + 0x18]
  00445ac1  call   0x441c80
  00445ac6  mov    eax, dword ptr [edi]
  00445ac8  mov    esi, dword ptr [esp + 0x6c]
  00445acc  xor    ebx, ebx
  00445ace  add    esp, 8
  00445ad1  cmp    eax, ebx
  00445ad3  je     0x445b3e
  00445ad5  mov    ecx, dword ptr [edi + 4]
  00445ad8  fld    dword ptr [esi + 0x3c]
  00445adb  mov    edx, dword ptr [edi + 0xc]
  00445ade  mov    dword ptr [esp + 0x48], ecx
  00445ae2  fsub   dword ptr [esp + 0x48]
  00445ae6  mov    dword ptr [esp + 0x50], edx
  00445aea  lea    eax, [esp + 0x38]
  00445aee  fstp   dword ptr [esp + 0x38]
  00445af2  push   eax
  00445af3  fld    dword ptr [esi + 0x44]
  00445af6  fsub   dword ptr [esp + 0x54]
  00445afa  fstp   dword ptr [esp + 0x40]
  00445afe  call   0x4c3bf0
  00445b03  fcom   dword ptr [0x5cc35c]
  00445b09  add    esp, 4
  00445b0c  fnstsw ax
  00445b0e  test   ah, 0x41
  00445b11  jne    0x445b1b
  00445b13  fstp   st(0)
  00445b15  fld    dword ptr [0x5cc35c]
  00445b1b  fmul   dword ptr [0x5ccac8]
  00445b21  mov    ecx, dword ptr [edi + 8]
  00445b24  fld    dword ptr [esp + 0x48]
  00445b28  mov    dword ptr [esi + 0xa4], ecx
  00445b2e  fadd   st(1)
  00445b30  fstp   dword ptr [esp + 0x48]
  00445b34  fadd   dword ptr [esp + 0x50]
  00445b38  fstp   dword ptr [esp + 0x50]
  00445b3c  jmp    0x445b5c
  00445b3e  mov    dword ptr [esi + 0xa4], 0x41400000
  00445b48  call   0x426c00
  00445b4d  cmp    eax, 0x1a
  00445b50  jne    0x445b5c
  00445b52  mov    dword ptr [esi + 0xa4], 0x41a00000
  00445b5c  fld    dword ptr [esi + 0x40]
  00445b5f  fsub   dword ptr [esi + 0xa4]
  00445b65  fcom   dword ptr [0x5cc9a0]
  00445b6b  fst    dword ptr [esp + 0x64]
  00445b6f  fnstsw ax
  00445b71  test   ah, 5
  00445b74  jp     0x445b8c
  00445b76  fld    dword ptr [esp + 0x10]
  00445b7a  fmul   dword ptr [0x5cc31c]
  00445b80  fadd   dword ptr [esi + 0x90]
  00445b86  fstp   dword ptr [esi + 0x90]
  00445b8c  fcomp  dword ptr [0x5cc9a0]
  00445b92  fnstsw ax
  00445b94  test   ah, 0x41
  00445b97  jne    0x445baf
  00445b99  fld    dword ptr [esp + 0x10]
  00445b9d  fmul   dword ptr [0x5cc31c]
  00445ba3  fsubr  dword ptr [esi + 0x90]
  00445ba9  fstp   dword ptr [esi + 0x90]
  00445baf  fld    dword ptr [0x5cc320]
  00445bb5  fsub   dword ptr [esp + 0x10]
  00445bb9  fst    dword ptr [esp + 0x20]
  00445bbd  fld    dword ptr [esp + 0x64]
  00445bc1  fcomp  dword ptr [0x5cc320]
  00445bc7  fnstsw ax
  00445bc9  test   ah, 5
  00445bcc  jp     0x445bed
  00445bce  fld    dword ptr [esp + 0x64]
  00445bd2  fcomp  dword ptr [0x5cc33c]
  00445bd8  fnstsw ax
  00445bda  test   ah, 0x41
  00445bdd  jne    0x445bed
  00445bdf  fstp   st(0)
  00445be1  fld    dword ptr [esp + 0x10]
  00445be5  fadd   st(0), st(0)
  00445be7  fsubr  dword ptr [0x5cc320]
  00445bed  fmul   dword ptr [esi + 0x90]
  00445bf3  lea    ebp, [esi + 0x3c]
  00445bf6  lea    edx, [esp + 0x38]
  00445bfa  push   edx
  00445bfb  fstp   dword ptr [esi + 0x90]
  00445c01  lea    eax, [esp + 0x44]
  00445c05  fld    dword ptr [ebp]
  00445c08  push   eax
  00445c09  fsub   dword ptr [esp + 0x50]
  00445c0d  mov    dword ptr [esp + 0x3c], ebp
  00445c11  fstp   dword ptr [esp + 0x40]
  00445c15  fld    dword ptr [esi + 0x44]
  00445c18  fsub   dword ptr [esp + 0x58]
  00445c1c  fstp   dword ptr [esp + 0x44]
  00445c20  call   0x4c3c60
  00445c25  lea    ecx, [esp + 0x40]
  00445c29  fstp   st(0)
  00445c2b  push   ecx
  00445c2c  call   0x4c3bf0
  00445c31  fstp   dword ptr [esp + 0x70]
  00445c35  mov    eax, dword ptr [esi + 0xa8]
  00445c3b  mov    edx, dword ptr [esp + 0x70]
  00445c3f  add    esp, 0xc
  00445c42  cmp    eax, ebx
  00445c44  mov    dword ptr [esp + 0x18], edx
  00445c48  jne    0x445d74
  00445c4e  mov    eax, dword ptr [edi]
  00445c50  cmp    eax, ebx
  00445c52  mov    dword ptr [esp + 0x68], 0
  00445c5a  jne    0x445cda
  00445c5c  fld    dword ptr [esp + 0x64]
  00445c60  mov    dword ptr [esp + 0x68], 0x40e00000
  00445c68  fcomp  dword ptr [esp + 0x68]
  00445c6c  fnstsw ax
  00445c6e  test   ah, 5
  00445c71  jp     0x445c7d
  00445c73  mov    dword ptr [esi + 0xa8], 1
  00445c7d  fld    dword ptr [esp + 0x10]
  00445c81  fmul   dword ptr [0x5cd96c]
  00445c87  mov    eax, dword ptr [edi]
  00445c89  fld    dword ptr [esp + 0x64]
  00445c8d  cmp    eax, ebx
  00445c8f  fsub   dword ptr [esp + 0x68]
  00445c93  je     0x445caa
  00445c95  fcom   dword ptr [0x5cc950]
  00445c9b  fnstsw ax
  00445c9d  test   ah, 5
  00445ca0  jp     0x445caa
  00445ca2  fstp   st(0)
  00445ca4  fld    dword ptr [0x5cc950]
  00445caa  fld    dword ptr [esp + 0x64]
  00445cae  fcomp  dword ptr [0x5d757c]
  00445cb4  fnstsw ax
  00445cb6  test   ah, 5
  00445cb9  jp     0x445cc3
  00445cbb  mov    dword ptr [esp + 0x64], 0
  00445cc3  fcom   dword ptr [0x5d757c]
  00445cc9  fnstsw ax
  00445ccb  test   ah, 5
  00445cce  jp     0x445d27
  00445cd0  fstp   st(0)
  00445cd2  fld    dword ptr [0x5d757c]
  00445cd8  jmp    0x445d3c
  00445cda  fld    dword ptr [esp + 0x64]
  00445cde  fcomp  dword ptr [0x5ce18c]
  00445ce4  fnstsw ax
  00445ce6  test   ah, 5
  00445ce9  jp     0x445cf4
  00445ceb  push   ebx
  00445cec  call   0x4430a0
  00445cf1  add    esp, 4
  00445cf4  mov    ecx, 0x7f1042
  00445cf9  lea    esp, [esp]
  00445d00  cmp    byte ptr [ecx], bl
  00445d02  je     0x445d0d
  00445d04  push   ebx
  00445d05  call   0x4430a0
  00445d0a  add    esp, 4
  00445d0d  add    ecx, 0x4c
  00445d10  cmp    ecx, 0x7f12a2
  00445d16  jl     0x445d00
  00445d18  fld    dword ptr [esp + 0x10]
  00445d1c  fmul   dword ptr [0x5cd0a0]
  00445d22  jmp    0x445c87
  00445d27  fcom   dword ptr [0x5cc358]
  00445d2d  fnstsw ax
  00445d2f  test   ah, 0x41
  00445d32  jne    0x445d3c
  00445d34  fstp   st(0)
  00445d36  fld    dword ptr [0x5cc358]
  00445d3c  fmulp  st(1)
  00445d3e  mov    eax, dword ptr [esi + 0xa8]
  00445d44  cmp    eax, ebx
  00445d46  fmul   dword ptr [0x5cc9c0]
  00445d4c  fld    dword ptr [esp + 0x40]
  00445d50  fmul   st(1)
  00445d52  fsubr  dword ptr [esi + 0x8c]
  00445d58  fstp   dword ptr [esi + 0x8c]
  00445d5e  fld    dword ptr [esp + 0x44]
  00445d62  fmul   st(1)
  00445d64  fsubr  dword ptr [esi + 0x94]
  00445d6a  fstp   dword ptr [esi + 0x94]
  00445d70  fstp   st(0)
  00445d72  je     0x445dec
  00445d74  fld    dword ptr [esp + 0x64]
  00445d78  fcomp  dword ptr [0x5cc55c]
  00445d7e  fnstsw ax
  00445d80  test   ah, 0x41
  00445d83  jne    0x445d8b
  00445d85  mov    dword ptr [esi + 0xa8], ebx
  00445d8b  cmp    dword ptr [edi], ebx
  00445d8d  je     0x445d95
  00445d8f  mov    dword ptr [esi + 0xa8], ebx
  00445d95  fld    dword ptr [esp + 0x10]
  00445d99  fmul   dword ptr [0x5cc728]
  00445d9f  fld    dword ptr [0x5cc9b8]
  00445da5  fsub   dword ptr [esp + 0x64]
  00445da9  fcom   dword ptr [0x5d757c]
  00445daf  fnstsw ax
  00445db1  test   ah, 5
  00445db4  jp     0x445dbe
  00445db6  fstp   st(0)
  00445db8  fld    dword ptr [0x5d757c]
  00445dbe  fmulp  st(1)
  00445dc0  fmul   dword ptr [0x5ce188]
  00445dc6  fld    dword ptr [esp + 0x40]
  00445dca  fmul   st(1)
  00445dcc  fadd   dword ptr [esi + 0x8c]
  00445dd2  fstp   dword ptr [esi + 0x8c]
  00445dd8  fld    dword ptr [esp + 0x44]
  00445ddc  fmul   st(1)
  00445dde  fadd   dword ptr [esi + 0x94]
  00445de4  fstp   dword ptr [esi + 0x94]
  00445dea  fstp   st(0)
  00445dec  fld    dword ptr [esp + 0x20]
  00445df0  fld    dword ptr [esp + 0x64]
  00445df4  fcomp  dword ptr [0x5cc55c]
  00445dfa  fnstsw ax
  00445dfc  test   ah, 5
  00445dff  jp     0x445e17
  00445e01  cmp    dword ptr [esi + 0xa8], ebx
  00445e07  jne    0x445e17
  00445e09  fstp   st(0)
  00445e0b  fld    dword ptr [esp + 0x10]
  00445e0f  fadd   st(0), st(0)
  00445e11  fsubr  dword ptr [0x5cc320]
  00445e17  fld    st(0)
  00445e19  push   2
  00445e1b  fmul   dword ptr [esi + 0x8c]
  00445e21  fstp   dword ptr [esi + 0x8c]
  00445e27  fmul   dword ptr [esi + 0x94]
  00445e2d  fstp   dword ptr [esi + 0x94]
  00445e33  mov    eax, dword ptr [esi + 0x8c]
  00445e39  mov    ecx, dword ptr [esi + 0x90]
  00445e3f  mov    edx, dword ptr [esi + 0x94]
  00445e45  mov    dword ptr [esp + 0x58], eax
  00445e49  fld    dword ptr [esp + 0x58]
  00445e4d  fmul   dword ptr [esp + 0x14]
  00445e51  mov    dword ptr [esp + 0x5c], ecx
  00445e55  mov    dword ptr [esp + 0x60], edx
  00445e59  lea    eax, [esp + 0x58]
  00445e5d  fstp   dword ptr [esp + 0x58]
  00445e61  fld    dword ptr [esp + 0x5c]
  00445e65  push   eax
  00445e66  fmul   dword ptr [esp + 0x18]
  00445e6a  lea    ecx, [esi + 0xc]
  00445e6d  push   ecx
  00445e6e  fstp   dword ptr [esp + 0x64]
  00445e72  fld    dword ptr [esp + 0x68]
  00445e76  fmul   dword ptr [esp + 0x1c]
  00445e7a  fstp   dword ptr [esp + 0x68]
  00445e7e  call   0x4c51a0
  00445e83  mov    eax, dword ptr [edi]
  00445e85  add    esp, 0xc
  00445e88  cmp    eax, ebx
  00445e8a  je     0x445f16
  00445e90  lea    edx, [esp + 0x48]
  00445e94  push   edi
  00445e95  push   edx
  00445e96  call   0x441c80
  00445e9b  fld    dword ptr [esp + 0x20]
  00445e9f  fcomp  dword ptr [0x5cc9f4]
  00445ea5  add    esp, 8
  00445ea8  fnstsw ax
  00445eaa  test   ah, 5
  00445ead  jp     0x445f16
  00445eaf  fld    dword ptr [edi + 0x10]
  00445eb2  fsubr  dword ptr [esp + 0x48]
  00445eb6  fld    dword ptr [edi + 0x14]
  00445eb9  fsubr  dword ptr [esp + 0x4c]
  00445ebd  fld    dword ptr [edi + 0x18]
  00445ec0  fsubr  dword ptr [esp + 0x50]
  00445ec4  fstp   dword ptr [esp + 0x5c]
  00445ec8  fld    dword ptr [0x5cc9f4]
  00445ece  fsub   dword ptr [esp + 0x18]
  00445ed2  fmul   dword ptr [0x5cd050]
  00445ed8  fstp   dword ptr [esp + 0x64]
  00445edc  fxch   st(1)
  00445ede  fmul   dword ptr [esp + 0x64]
  00445ee2  fstp   dword ptr [esp + 0x54]
  00445ee6  fmul   dword ptr [esp + 0x64]
  00445eea  fld    dword ptr [esp + 0x5c]
  00445eee  fmul   dword ptr [esp + 0x64]
  00445ef2  fld    dword ptr [esp + 0x48]
  00445ef6  fsub   dword ptr [esp + 0x54]
  00445efa  fstp   dword ptr [esp + 0x48]
  00445efe  fld    dword ptr [esp + 0x4c]
  00445f02  fsub   st(2)
  00445f04  fstp   dword ptr [esp + 0x4c]
  00445f08  fld    dword ptr [esp + 0x50]
  00445f0c  fsub   st(1)
  00445f0e  fstp   dword ptr [esp + 0x50]
  00445f12  fstp   st(0)
  00445f14  fstp   st(0)
  00445f16  fld    dword ptr [esp + 0x48]
  00445f1a  mov    eax, dword ptr [0x7f101c]
  00445f1f  fsub   dword ptr [ebp]
  00445f22  add    eax, 0xfffffe00
  00445f27  and    eax, 0x3ff
  00445f2c  mov    dword ptr [esp + 0x64], eax
  00445f30  fst    dword ptr [esp + 0x24]
  00445f34  fld    dword ptr [esp + 0x4c]
  00445f38  fsub   dword ptr [esi + 0x40]
  00445f3b  fstp   dword ptr [esp + 0x28]
  00445f3f  fld    dword ptr [esp + 0x50]
  00445f43  fsub   dword ptr [esi + 0x44]
  00445f46  fstp   dword ptr [esp + 0x68]
  00445f4a  fild   dword ptr [esp + 0x64]
  00445f4e  fmul   dword ptr [0x5ce184]
  00445f54  fsin   
  00445f56  fcom   dword ptr [0x5d757c]
  00445f5c  fnstsw ax
  00445f5e  test   ah, 5
  00445f61  jp     0x445f65
  00445f63  fchs   
  00445f65  fcom   dword ptr [0x5cc9c0]
  00445f6b  fnstsw ax
  00445f6d  test   ah, 5
  00445f70  jp     0x445f84
  00445f72  fsubr  dword ptr [0x5cc9c0]
  00445f78  fmul   dword ptr [0x5cc564]
  00445f7e  fsubr  dword ptr [0x5cc9c0]
  00445f84  fmul   dword ptr [0x5cc32c]
  00445f8a  fadd   dword ptr [0x5cc56c]
  00445f90  fstp   dword ptr [esi + 0xc0]
  00445f96  mov    eax, dword ptr [edi]
  00445f98  cmp    eax, ebx
  00445f9a  je     0x445fa6
  00445f9c  mov    dword ptr [esi + 0xc0], 0x3f19999a
  00445fa6  fld    dword ptr [esi + 0xc0]
  00445fac  fmul   dword ptr [0x5cc9b0]
  00445fb2  fst    dword ptr [esp + 0x14]
  00445fb6  fcomp  dword ptr [0x5cc358]
  00445fbc  fnstsw ax
  00445fbe  test   ah, 0x41
  00445fc1  jne    0x445fcb
  00445fc3  mov    dword ptr [esp + 0x14], 0x40a00000
  00445fcb  mov    ecx, dword ptr [edi]
  00445fcd  cmp    ecx, ebx
  00445fcf  je     0x445fea
  00445fd1  fld    dword ptr [esp + 0x18]
  00445fd5  fcomp  dword ptr [0x5cc35c]
  00445fdb  fnstsw ax
  00445fdd  test   ah, 5
  00445fe0  jp     0x445fea
  00445fe2  mov    edx, dword ptr [esp + 0x18]
  00445fe6  mov    dword ptr [esp + 0x14], edx
  00445fea  fld    dword ptr [esp + 0x68]
  00445fee  fcomp  dword ptr [0x5d757c]
  00445ff4  fnstsw ax
  00445ff6  test   ah, 0x44
  00445ff9  jnp    0x44602a
  00445ffb  fdiv   dword ptr [esp + 0x68]
  00445fff  fld1   
  00446001  fpatan 
  00446003  fmul   qword ptr [0x5ccae0]
  00446009  fchs   
  0044600b  fsubr  dword ptr [0x5cd09c]
  00446011  fld    dword ptr [esp + 0x68]
  00446015  fcomp  dword ptr [0x5d757c]
  0044601b  fnstsw ax
  0044601d  test   ah, 0x41
  00446020  jne    0x446045
  00446022  fadd   dword ptr [0x5cd09c]
  00446028  jmp    0x446045
  0044602a  fcomp  dword ptr [0x5d757c]
  00446030  fnstsw ax
  00446032  test   ah, 5
  00446035  jp     0x44603f
  00446037  fld    dword ptr [0x5cd324]
  0044603d  jmp    0x446045
  0044603f  fld    dword ptr [0x5ccad0]
  00446045  cmp    ecx, ebx
  00446047  fld    dword ptr [esp + 0x10]
  0044604b  fmul   dword ptr [0x5cc358]
  00446051  fst    dword ptr [esp + 0x30]
  00446055  fstp   dword ptr [esp + 0x1c]
  00446059  je     0x44607a
  0044605b  fld    dword ptr [esp + 0x18]
  0044605f  fcomp  dword ptr [0x5cc574]
  00446065  fnstsw ax
  00446067  test   ah, 0x41
  0044606a  jne    0x44607a
  0044606c  fld    dword ptr [esp + 0x10]
  00446070  fmul   dword ptr [0x5cc55c]
  00446076  fstp   dword ptr [esp + 0x1c]
  0044607a  fsubr  dword ptr [esi + 0xb0]
  00446080  fcom   dword ptr [0x5ccac4]
  00446086  fnstsw ax
  00446088  test   ah, 1
  0044608b  jne    0x4460a3
  0044608d  lea    ecx, [ecx]
  00446090  fsub   dword ptr [0x5ccac4]
  00446096  fcom   dword ptr [0x5ccac4]
  0044609c  fnstsw ax
  0044609e  test   ah, 1
  004460a1  je     0x446090
  004460a3  fcom   dword ptr [0x5d757c]
  004460a9  fnstsw ax
  004460ab  test   ah, 5
  004460ae  jp     0x4460c3
  004460b0  fadd   dword ptr [0x5ccac4]
  004460b6  fcom   dword ptr [0x5d757c]
  004460bc  fnstsw ax
  004460be  test   ah, 5
  004460c1  jnp    0x4460b0
  004460c3  fcom   dword ptr [0x5cd09c]
  004460c9  fnstsw ax
  004460cb  test   ah, 0x41
  004460ce  jne    0x4460d8
  004460d0  fsubr  dword ptr [0x5ccac4]
  004460d6  fchs   
  004460d8  fcom   dword ptr [esp + 0x14]
  004460dc  fnstsw ax
  004460de  test   ah, 0x41
  004460e1  jne    0x4460ef
  004460e3  fld    dword ptr [esi + 0xbc]
  004460e9  fsub   dword ptr [esp + 0x1c]
  004460ed  jmp    0x446121
  004460ef  fcom   dword ptr [0x5d757c]
  004460f5  fnstsw ax
  004460f7  test   ah, 5
  004460fa  jp     0x4460fe
  004460fc  fchs   
  004460fe  fcom   dword ptr [esp + 0x14]
  00446102  fnstsw ax
  00446104  test   ah, 0x41
  00446107  jne    0x446115
  00446109  fld    dword ptr [esp + 0x1c]
  0044610d  fadd   dword ptr [esi + 0xbc]
  00446113  jmp    0x446121
  00446115  fld    dword ptr [esi + 0xbc]
  0044611b  fmul   dword ptr [0x5cc32c]
  00446121  fstp   dword ptr [esi + 0xbc]
  00446127  fdiv   dword ptr [esp + 0x14]
  0044612b  fst    dword ptr [esp + 0x64]
  0044612f  fcomp  dword ptr [0x5cc35c]
  00446135  fnstsw ax
  00446137  test   ah, 5
  0044613a  jp     0x446146
  0044613c  mov    dword ptr [esp + 0x64], 0x42c80000
  00446144  jmp    0x446175
  00446146  fld    dword ptr [esp + 0x64]
  0044614a  fcomp  dword ptr [0x5cc55c]
  00446150  fnstsw ax
  00446152  test   ah, 5
  00446155  jp     0x44616d
  00446157  fld    dword ptr [0x5cc55c]
  0044615d  fsub   dword ptr [esp + 0x64]
  00446161  fmul   dword ptr [0x5ce180]
  00446167  fstp   dword ptr [esp + 0x64]
  0044616b  jmp    0x446175
  0044616d  mov    dword ptr [esp + 0x64], 0
  00446175  fld    dword ptr [ebp]
  00446178  lea    eax, [esp + 0x54]
  0044617c  fsub   dword ptr [esp + 0x48]
  00446180  push   eax
  00446181  fstp   dword ptr [esp + 0x58]
  00446185  fld    dword ptr [esi + 0x40]
  00446188  fsub   dword ptr [esp + 0x50]
  0044618c  fstp   dword ptr [esp + 0x5c]
  00446190  fld    dword ptr [esi + 0x44]
  00446193  fsub   dword ptr [esp + 0x54]
  00446197  fstp   dword ptr [esp + 0x60]
  0044619b  call   0x4c3ac0
  004461a0  fsub   dword ptr [esi + 0xa4]
  004461a6  add    esp, 4
  004461a9  fcom   dword ptr [0x5d757c]
  004461af  fnstsw ax
  004461b1  test   ah, 5
  004461b4  jp     0x4461b8
  004461b6  fchs   
  004461b8  fld    st(0)
  004461ba  fadd   st(1)
  004461bc  fsubr  dword ptr [esp + 0x64]
  004461c0  fcom   dword ptr [0x5d757c]
  004461c6  fnstsw ax
  004461c8  test   ah, 5
  004461cb  jp     0x4461d5
  004461cd  fstp   st(0)
  004461cf  fld    dword ptr [0x5d757c]
  004461d5  mov    ecx, dword ptr [esp + 0x20]
  004461d9  fstp   dword ptr [esi + 0xc8]
  004461df  fld    dword ptr [esi + 0xbc]
  004461e5  mov    dword ptr [esp + 0x64], ecx
  004461e9  fadd   dword ptr [esi + 0xb0]
  004461ef  fstp   dword ptr [esi + 0xb0]
  004461f5  fld    dword ptr [esp + 0x14]
  004461f9  fadd   st(0), st(0)
  004461fb  fstp   dword ptr [esp + 0x2c]
  004461ff  fcomp  dword ptr [esp + 0x2c]
  00446203  fnstsw ax
  00446205  test   ah, 5
  00446208  jp     0x44621c
  0044620a  fld    dword ptr [esp + 0x10]
  0044620e  fmul   dword ptr [0x5cc35c]
  00446214  fsubr  dword ptr [0x5cc320]
  0044621a  jmp    0x446220
  0044621c  fld    dword ptr [esp + 0x64]
  00446220  fmul   dword ptr [esi + 0xbc]
  00446226  fstp   dword ptr [esi + 0xbc]
  0044622c  fld    dword ptr [esp + 0x68]
  00446230  fmul   dword ptr [esp + 0x68]
  00446234  fld    dword ptr [esp + 0x24]
  00446238  fmul   dword ptr [esp + 0x24]
  0044623c  faddp  st(1)
  0044623e  fst    dword ptr [esp + 0x68]
  00446242  fcomp  dword ptr [0x5d757c]
  00446248  fnstsw ax
  0044624a  test   ah, 0x44
  0044624d  jnp    0x4462a0
  0044624f  mov    edx, dword ptr [esp + 0x68]
  00446253  push   edx
  00446254  call   0x4c3b30
  00446259  fst    dword ptr [esp + 0x6c]
  0044625d  fcomp  dword ptr [0x5d757c]
  00446263  add    esp, 4
  00446266  fnstsw ax
  00446268  test   ah, 0x44
  0044626b  jnp    0x4462a0
  0044626d  fld    dword ptr [esp + 0x28]
  00446271  fdiv   dword ptr [esp + 0x68]
  00446275  fld1   
  00446277  fpatan 
  00446279  fmul   qword ptr [0x5ccae0]
  0044627f  fchs   
  00446281  fsubr  dword ptr [0x5cd09c]
  00446287  fld    dword ptr [esp + 0x68]
  0044628b  fcomp  dword ptr [0x5d757c]
  00446291  fnstsw ax
  00446293  test   ah, 0x41
  00446296  jne    0x4462bf
  00446298  fadd   dword ptr [0x5cd09c]
  0044629e  jmp    0x4462bf
  004462a0  fld    dword ptr [esp + 0x28]
  004462a4  fcomp  dword ptr [0x5d757c]
  004462aa  fnstsw ax
  004462ac  test   ah, 5
  004462af  jp     0x4462b9
  004462b1  fld    dword ptr [0x5cd324]
  004462b7  jmp    0x4462bf
  004462b9  fld    dword ptr [0x5ccad0]
  004462bf  fsubr  dword ptr [0x5ccac4]
  004462c5  mov    eax, dword ptr [esp + 0x30]
  004462c9  mov    dword ptr [esp + 0x1c], eax
  004462cd  fsubr  dword ptr [esi + 0xac]
  004462d3  fcom   dword ptr [0x5ccac4]
  004462d9  fnstsw ax
  004462db  test   ah, 1
  004462de  jne    0x4462f3
  004462e0  fsub   dword ptr [0x5ccac4]
  004462e6  fcom   dword ptr [0x5ccac4]
  004462ec  fnstsw ax
  004462ee  test   ah, 1
  004462f1  je     0x4462e0
  004462f3  fcom   dword ptr [0x5d757c]
  004462f9  fnstsw ax
  004462fb  test   ah, 5
  004462fe  jp     0x446313
  00446300  fadd   dword ptr [0x5ccac4]
  00446306  fcom   dword ptr [0x5d757c]
  0044630c  fnstsw ax
  0044630e  test   ah, 5
  00446311  jnp    0x446300
  00446313  fcom   dword ptr [0x5cd09c]
  00446319  fnstsw ax
  0044631b  test   ah, 0x41
  0044631e  jne    0x446328
  00446320  fsubr  dword ptr [0x5ccac4]
  00446326  fchs   
  00446328  fcom   dword ptr [esp + 0x14]
  0044632c  fnstsw ax
  0044632e  test   ah, 0x41
  00446331  jne    0x44633f
  00446333  fld    dword ptr [esi + 0xb8]
  00446339  fsub   dword ptr [esp + 0x1c]
  0044633d  jmp    0x446370
  0044633f  fld    dword ptr [esp + 0x14]
  00446343  fchs   
  00446345  fld    st(1)
  00446347  fcompp 
  00446349  fnstsw ax
  0044634b  test   ah, 5
  0044634e  jp     0x446364
  00446350  fld    dword ptr [esp + 0x1c]
  00446354  fadd   dword ptr [esi + 0xb8]
  0044635a  fstp   dword ptr [esi + 0xb8]
  00446360  fchs   
  00446362  jmp    0x446376
  00446364  fld    dword ptr [esi + 0xb8]
  0044636a  fmul   dword ptr [0x5cc32c]
  00446370  fstp   dword ptr [esi + 0xb8]
  00446376  fld    dword ptr [esi + 0xb8]
  0044637c  mov    ecx, dword ptr [esp + 0x20]
  00446380  fadd   dword ptr [esi + 0xac]
  00446386  mov    dword ptr [esp + 0x64], ecx
  0044638a  fstp   dword ptr [esi + 0xac]
  00446390  fcomp  dword ptr [esp + 0x2c]
  00446394  fnstsw ax
  00446396  test   ah, 5
  00446399  jp     0x4463ad
  0044639b  fld    dword ptr [esp + 0x10]
  0044639f  fmul   dword ptr [0x5cc35c]
  004463a5  fsubr  dword ptr [0x5cc320]
  004463ab  jmp    0x4463b1
  004463ad  fld    dword ptr [esp + 0x64]
  004463b1  fmul   dword ptr [esi + 0xb8]
  004463b7  fstp   dword ptr [esi + 0xb8]
  004463bd  mov    edx, dword ptr [0x7f101c]
  004463c3  and    edx, 0x3ff
  004463c9  mov    dword ptr [esp + 0x64], edx
  004463cd  fild   dword ptr [esp + 0x64]
  004463d1  mov    eax, dword ptr [edi]
  004463d3  cmp    eax, ebx
  004463d5  fmul   dword ptr [0x5ce17c]
  004463db  fsin   
  004463dd  fmul   dword ptr [0x5cc72c]
  004463e3  je     0x446421
  004463e5  fld    dword ptr [esp + 0x18]
  004463e9  fcomp  dword ptr [0x5cc35c]
  004463ef  fnstsw ax
  004463f1  test   ah, 5
  004463f4  jp     0x446421
  004463f6  fld    dword ptr [esp + 0x18]
  004463fa  fsub   dword ptr [0x5cc320]
  00446400  fcom   dword ptr [0x5d757c]
  00446406  fnstsw ax
  00446408  test   ah, 5
  0044640b  jp     0x446415
  0044640d  fstp   st(0)
  0044640f  fld    dword ptr [0x5d757c]
  00446415  fxch   st(1)
  00446417  fmul   st(1)
  00446419  fmul   dword ptr [0x5ccac8]
  0044641f  fstp   st(1)
  00446421  mov    ecx, dword ptr [esi + 0x58]
  00446424  fstp   dword ptr [esi + 0xb4]
  0044642a  mov    eax, 0x3f800000
  0044642f  lea    edi, [esi + 0x4c]
  00446432  mov    dword ptr [esi + 0x74], eax
  00446435  mov    dword ptr [esi + 0x60], eax
  00446438  mov    dword ptr [edi], eax
  0044643a  mov    eax, dword ptr [esi + 0xb0]
  00446440  push   2
  00446442  push   eax
  00446443  or     ecx, 0x20003
  00446449  push   0x6146fc
  0044644e  lea    ebp, [esi + 0x6c]
  00446451  push   edi
  00446452  mov    dword ptr [esi + 0x5c], ebx
  00446455  mov    dword ptr [esi + 0x54], ebx
  00446458  mov    dword ptr [esi + 0x50], ebx
  0044645b  mov    dword ptr [esi + 0x70], ebx
  0044645e  mov    dword ptr [ebp], ebx
  00446461  mov    dword ptr [esi + 0x64], ebx
  00446464  mov    dword ptr [esi + 0x84], ebx
  0044646a  mov    dword ptr [esi + 0x80], ebx
  00446470  mov    dword ptr [esi + 0x7c], ebx
  00446473  mov    dword ptr [esi + 0x58], ecx
  00446476  call   0x4c4d20
  0044647b  mov    ecx, dword ptr [esi + 0xac]
  00446481  push   2
  00446483  push   ecx
  00446484  push   edi
  00446485  push   edi
  00446486  call   0x4c4d20
  0044648b  mov    edx, dword ptr [esi + 0xb4]
  00446491  push   2
  00446493  push   edx
  00446494  push   ebp
  00446495  push   edi
  00446496  call   0x4c4d20
  0044649b  mov    eax, dword ptr [esp + 0x64]
  0044649f  push   2
  004464a1  push   eax
  004464a2  push   edi
  004464a3  call   0x4c51a0
  004464a8  add    esp, 0x3c
  004464ab  pop    edi
  004464ac  pop    esi
  004464ad  pop    ebp
  004464ae  pop    ebx
  004464af  add    esp, 0x50
  004464b2  ret    
