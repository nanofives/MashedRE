; Synthetic pairs for re/tools/match/equiv.py (EQUIV-CHECK). Each case is two procedures <name>_a (plays "ours")
; and <name>_b (plays "the original"). Expected verdicts live in re/tools/match/test_equiv.py (CASES).
; Assembled with MSVC ml.exe /c /coff. db sequences force one encoding where MASM would pick another.
; Globals are absolute addresses (G1 = 006A0000h ...), external callees are real MASHED.exe functions:
;   FUN_0040bb30: cdecl, 1 stack argument, no register argument
;   FUN_0048cee0: reads EDI as a register argument
.686
.model flat
EXTERN FUN_0040bb30:PROC
EXTERN FUN_0048cee0:PROC
G1 EQU 006A0000h
G2 EQU 006A0004h
G3 EQU 006A0008h
.code

; ------------------------------------------------------------------ ENC: encoding alternatives
enc_movrr_a PROC
    db 89h, 0C8h                        ; mov eax, ecx (89 /r)
    mov dword ptr ds:[G1], eax
    ret
enc_movrr_a ENDP
enc_movrr_b PROC
    db 8Bh, 0C1h                        ; mov eax, ecx (8B /r)
    mov dword ptr ds:[G1], eax
    ret
enc_movrr_b ENDP

enc_imm_a PROC
    mov eax, [esp+4]
    db 83h, 0C0h, 05h                   ; add eax, 5 (imm8)
    mov dword ptr ds:[G1], eax
    ret
enc_imm_a ENDP
enc_imm_b PROC
    mov eax, [esp+4]
    db 81h, 0C0h, 05h, 0, 0, 0          ; add eax, 5 (imm32)
    mov dword ptr ds:[G1], eax
    ret
enc_imm_b ENDP

enc_jmp_a PROC
    mov eax, [esp+4]
    test eax, eax
    jmp short @enc_jmp_a1
@enc_jmp_a1:
    mov dword ptr ds:[G1], eax
    ret
enc_jmp_a ENDP
enc_jmp_b PROC
    mov eax, [esp+4]
    test eax, eax
    db 0E9h                             ; jmp rel32
    dd 0
    mov dword ptr ds:[G1], eax
    ret
enc_jmp_b ENDP

enc_nop_a PROC
    mov eax, [esp+4]
    nop
    nop
    nop
    mov dword ptr ds:[G1], eax
    ret
enc_nop_a ENDP
enc_nop_b PROC
    mov eax, [esp+4]
    db 8Dh, 49h, 00h                    ; lea ecx, [ecx+0]
    db 8Bh, 0FFh                        ; mov edi, edi
    mov dword ptr ds:[G1], eax
    ret
enc_nop_b ENDP

neg_enc_imm_a PROC
    mov eax, [esp+4]
    add eax, 5
    mov dword ptr ds:[G1], eax
    ret
neg_enc_imm_a ENDP
neg_enc_imm_b PROC
    mov eax, [esp+4]
    add eax, 6
    mov dword ptr ds:[G1], eax
    ret
neg_enc_imm_b ENDP

; ------------------------------------------------------------------ SIB: base / index order, scale 1
sib_a PROC
    mov ecx, [esp+4]
    mov edx, [esp+8]
    db 8Bh, 04h, 11h                    ; mov eax, [ecx+edx]
    mov dword ptr ds:[G1], eax
    ret
sib_a ENDP
sib_b PROC
    mov ecx, [esp+4]
    mov edx, [esp+8]
    db 8Bh, 04h, 0Ah                    ; mov eax, [edx+ecx]
    mov dword ptr ds:[G1], eax
    ret
sib_b ENDP

neg_sib_scale_a PROC
    mov ecx, [esp+4]
    mov edx, [esp+8]
    mov eax, [ecx+edx*2]
    mov dword ptr ds:[G1], eax
    ret
neg_sib_scale_a ENDP
neg_sib_scale_b PROC
    mov ecx, [esp+4]
    mov edx, [esp+8]
    mov eax, [edx+ecx*2]
    mov dword ptr ds:[G1], eax
    ret
neg_sib_scale_b ENDP

; ------------------------------------------------------------------ REG: register bijection
reg_swap_a PROC
    push esi
    mov esi, [esp+8]
    add esi, 3
    mov dword ptr ds:[G1], esi
    pop esi
    ret
reg_swap_a ENDP
reg_swap_b PROC
    push edi
    mov edi, [esp+8]
    add edi, 3
    mov dword ptr ds:[G1], edi
    pop edi
    ret
reg_swap_b ENDP

reg_call_a PROC
    push esi
    mov esi, [esp+8]
    push esi
    call FUN_0040bb30
    add esp, 4
    mov dword ptr ds:[G1], esi
    pop esi
    ret
reg_call_a ENDP
reg_call_b PROC
    push edi
    mov edi, [esp+8]
    push edi
    call FUN_0040bb30
    add esp, 4
    mov dword ptr ds:[G1], edi
    pop edi
    ret
reg_call_b ENDP

neg_reg_ret_a PROC
    mov eax, [esp+4]
    ret
neg_reg_ret_a ENDP
neg_reg_ret_b PROC
    mov ecx, [esp+4]
    ret
neg_reg_ret_b ENDP

neg_reg_regarg_a PROC
    push edi
    mov edi, [esp+8]
    call FUN_0048cee0
    pop edi
    ret
neg_reg_regarg_a ENDP
neg_reg_regarg_b PROC
    push esi
    mov esi, [esp+8]
    call FUN_0048cee0
    pop esi
    ret
neg_reg_regarg_b ENDP

neg_reg_nosave_a PROC
    push esi
    mov esi, 1
    mov dword ptr ds:[G1], esi
    pop esi
    ret
neg_reg_nosave_a ENDP
neg_reg_nosave_b PROC
    mov esi, 1
    mov dword ptr ds:[G1], esi
    ret
neg_reg_nosave_b ENDP

neg_reg_one_a PROC
    mov eax, [esp+4]
    mov ecx, [esp+8]
    mov dword ptr ds:[G1], eax
    ret
neg_reg_one_a ENDP
neg_reg_one_b PROC
    mov eax, [esp+4]
    mov ecx, [esp+8]
    mov dword ptr ds:[G1], ecx
    ret
neg_reg_one_b ENDP

; ------------------------------------------------------------------ ORDER: independent adjacent instructions
order_st_a PROC
    mov eax, [esp+4]
    mov dword ptr [eax], 1
    mov dword ptr [eax+4], 2
    ret
order_st_a ENDP
order_st_b PROC
    mov eax, [esp+4]
    mov dword ptr [eax+4], 2
    mov dword ptr [eax], 1
    ret
order_st_b ENDP

order_alu_a PROC
    mov eax, [esp+4]
    mov ecx, [esp+8]
    add eax, 1
    add ecx, 2
    mov dword ptr ds:[G1], eax
    mov dword ptr ds:[G2], ecx
    ret
order_alu_a ENDP
order_alu_b PROC
    mov eax, [esp+4]
    mov ecx, [esp+8]
    add ecx, 2
    add eax, 1
    mov dword ptr ds:[G1], eax
    mov dword ptr ds:[G2], ecx
    ret
order_alu_b ENDP

order_ldst_a PROC
    mov eax, [esp+4]
    mov ecx, [eax+8]
    mov dword ptr [eax], 1
    mov dword ptr ds:[G1], ecx
    ret
order_ldst_a ENDP
order_ldst_b PROC
    mov eax, [esp+4]
    mov dword ptr [eax], 1
    mov ecx, [eax+8]
    mov dword ptr ds:[G1], ecx
    ret
order_ldst_b ENDP

neg_order_alias_a PROC
    mov eax, [esp+4]
    mov ecx, [esp+8]
    mov dword ptr [eax], 1
    mov dword ptr [ecx], 2
    ret
neg_order_alias_a ENDP
neg_order_alias_b PROC
    mov eax, [esp+4]
    mov ecx, [esp+8]
    mov dword ptr [ecx], 2
    mov dword ptr [eax], 1
    ret
neg_order_alias_b ENDP

neg_order_overlap_a PROC
    mov eax, [esp+4]
    mov dword ptr [eax], 1
    mov dword ptr [eax+2], 2
    ret
neg_order_overlap_a ENDP
neg_order_overlap_b PROC
    mov eax, [esp+4]
    mov dword ptr [eax+2], 2
    mov dword ptr [eax], 1
    ret
neg_order_overlap_b ENDP

neg_order_ldst_a PROC
    mov eax, [esp+4]
    mov ecx, [esp+8]
    mov edx, [ecx]
    mov dword ptr [eax], 1
    mov dword ptr ds:[G1], edx
    ret
neg_order_ldst_a ENDP
neg_order_ldst_b PROC
    mov eax, [esp+4]
    mov ecx, [esp+8]
    mov dword ptr [eax], 1
    mov edx, [ecx]
    mov dword ptr ds:[G1], edx
    ret
neg_order_ldst_b ENDP

neg_order_flags_a PROC
    mov eax, [esp+4]
    mov edx, [esp+8]
    cmp eax, 5
    add edx, 1
    jl @nof_a1
    mov dword ptr ds:[G1], 1
@nof_a1:
    ret
neg_order_flags_a ENDP
neg_order_flags_b PROC
    mov eax, [esp+4]
    mov edx, [esp+8]
    add edx, 1
    cmp eax, 5
    jl @nof_b1
    mov dword ptr ds:[G1], 1
@nof_b1:
    ret
neg_order_flags_b ENDP

; ------------------------------------------------------------------ LAYOUT: block placement / polarity
layout_pol_a PROC
    mov eax, [esp+4]
    test eax, eax
    je @lp_a1
    mov dword ptr ds:[G1], 1
    ret
@lp_a1:
    mov dword ptr ds:[G1], 2
    ret
layout_pol_a ENDP
layout_pol_b PROC
    mov eax, [esp+4]
    test eax, eax
    jne @lp_b1
    mov dword ptr ds:[G1], 2
    ret
@lp_b1:
    mov dword ptr ds:[G1], 1
    ret
layout_pol_b ENDP

layout_dup_a PROC
    mov eax, [esp+4]
    test eax, eax
    je @ld_a1
    mov dword ptr ds:[G1], 1
@ld_a1:
    ret
layout_dup_a ENDP
layout_dup_b PROC
    mov eax, [esp+4]
    test eax, eax
    jne @ld_b1
    ret
@ld_b1:
    mov dword ptr ds:[G1], 1
    ret
layout_dup_b ENDP

neg_layout_a PROC
    mov eax, [esp+4]
    test eax, eax
    je @nl_a1
    mov dword ptr ds:[G1], 1
    ret
@nl_a1:
    mov dword ptr ds:[G1], 2
    ret
neg_layout_a ENDP
neg_layout_b PROC
    mov eax, [esp+4]
    test eax, eax
    jne @nl_b1
    mov dword ptr ds:[G1], 1
    ret
@nl_b1:
    mov dword ptr ds:[G1], 2
    ret
neg_layout_b ENDP

; ------------------------------------------------------------------ COMM: commutative operand order
comm_add_a PROC
    mov ecx, [esp+4]
    mov edx, [esp+8]
    mov eax, ecx
    add eax, edx
    mov dword ptr ds:[G1], eax
    ret
comm_add_a ENDP
comm_add_b PROC
    mov ecx, [esp+4]
    mov edx, [esp+8]
    mov eax, edx
    add eax, ecx
    mov dword ptr ds:[G1], eax
    ret
comm_add_b ENDP

comm_imul_a PROC
    mov eax, dword ptr ds:[G1]
    imul eax, dword ptr ds:[G2]
    mov dword ptr ds:[G3], eax
    ret
comm_imul_a ENDP
comm_imul_b PROC
    mov eax, dword ptr ds:[G2]
    imul eax, dword ptr ds:[G1]
    mov dword ptr ds:[G3], eax
    ret
comm_imul_b ENDP

neg_comm_sub_a PROC
    mov ecx, [esp+4]
    mov edx, [esp+8]
    mov eax, ecx
    sub eax, edx
    mov dword ptr ds:[G1], eax
    ret
neg_comm_sub_a ENDP
neg_comm_sub_b PROC
    mov ecx, [esp+4]
    mov edx, [esp+8]
    mov eax, edx
    sub eax, ecx
    mov dword ptr ds:[G1], eax
    ret
neg_comm_sub_b ENDP

; ------------------------------------------------------------------ FRAME: register kept vs slot reloaded
frame_reload_a PROC
    push ebx
    mov ebx, [esp+8]
    mov ecx, [esp+0Ch]
    mov dword ptr [ecx], 7
    mov dword ptr [ebx], 1
    pop ebx
    xor ecx, ecx
    ret
frame_reload_a ENDP
frame_reload_b PROC
    mov ecx, [esp+8]
    mov dword ptr [ecx], 7
    mov ecx, [esp+4]
    mov dword ptr [ecx], 1
    xor ecx, ecx
    ret
frame_reload_b ENDP

neg_frame_escape_a PROC
    push ebx
    mov ebx, [esp+8]
    lea eax, [esp+8]
    push eax
    call FUN_0040bb30
    add esp, 4
    mov dword ptr [ebx], 1
    pop ebx
    ret
neg_frame_escape_a ENDP
neg_frame_escape_b PROC
    lea eax, [esp+4]
    push eax
    call FUN_0040bb30
    add esp, 4
    mov eax, [esp+4]
    mov dword ptr [eax], 1
    ret
neg_frame_escape_b ENDP

; ------------------------------------------------------------------ x87 (excluded on purpose)
x87_same_a PROC
    fld dword ptr ds:[G1]
    fmul dword ptr ds:[G2]
    fstp dword ptr ds:[G3]
    ret
x87_same_a ENDP
x87_same_b PROC
    fld dword ptr ds:[G1]
    fmul dword ptr ds:[G2]
    fstp dword ptr ds:[G3]
    ret
x87_same_b ENDP

neg_x87_comm_a PROC
    fld dword ptr ds:[G1]
    fmul dword ptr ds:[G2]
    fstp dword ptr ds:[G3]
    ret
neg_x87_comm_a ENDP
neg_x87_comm_b PROC
    fld dword ptr ds:[G2]
    fmul dword ptr ds:[G1]
    fstp dword ptr ds:[G3]
    ret
neg_x87_comm_b ENDP

neg_x87_sched_a PROC
    fld dword ptr ds:[G1]
    mov eax, [esp+4]
    fmul dword ptr ds:[G2]
    fstp dword ptr ds:[G3]
    mov dword ptr ds:[G1], eax
    ret
neg_x87_sched_a ENDP
neg_x87_sched_b PROC
    mov eax, [esp+4]
    fld dword ptr ds:[G1]
    fmul dword ptr ds:[G2]
    fstp dword ptr ds:[G3]
    mov dword ptr ds:[G1], eax
    ret
neg_x87_sched_b ENDP

; ------------------------------------------------------------------ calls
neg_call_arg_a PROC
    push 1
    call FUN_0040bb30
    add esp, 4
    ret
neg_call_arg_a ENDP
neg_call_arg_b PROC
    push 2
    call FUN_0040bb30
    add esp, 4
    ret
neg_call_arg_b ENDP

neg_call_order_a PROC
    push 1
    call FUN_0040bb30
    push 2
    call FUN_0040bb30
    add esp, 8
    ret
neg_call_order_a ENDP
neg_call_order_b PROC
    push 2
    call FUN_0040bb30
    push 1
    call FUN_0040bb30
    add esp, 8
    ret
neg_call_order_b ENDP

neg_call_storeorder_a PROC
    mov dword ptr ds:[G1], 1
    push 0
    call FUN_0040bb30
    add esp, 4
    ret
neg_call_storeorder_a ENDP
neg_call_storeorder_b PROC
    push 0
    call FUN_0040bb30
    add esp, 4
    mov dword ptr ds:[G1], 1
    ret
neg_call_storeorder_b ENDP

; ------------------------------------------------------------------ loops
loop_reg_a PROC
    mov ecx, 10
@lr_a1:
    dec ecx
    mov dword ptr ds:[G1], ecx
    jne @lr_a1
    xor ecx, ecx
    xor edx, edx
    ret
loop_reg_a ENDP
loop_reg_b PROC
    mov edx, 10
@lr_b1:
    dec edx
    mov dword ptr ds:[G1], edx
    jne @lr_b1
    xor ecx, ecx
    xor edx, edx
    ret
loop_reg_b ENDP

neg_loop_bound_a PROC
    mov ecx, 10
@nlb_a1:
    dec ecx
    mov dword ptr ds:[G1], ecx
    jne @nlb_a1
    ret
neg_loop_bound_a ENDP
neg_loop_bound_b PROC
    mov ecx, 11
@nlb_b1:
    dec ecx
    mov dword ptr ds:[G1], ecx
    jne @nlb_b1
    ret
neg_loop_bound_b ENDP

loop_ptr_a PROC
    mov eax, [esp+4]
    mov ecx, [esp+8]
@lp2_a1:
    mov dword ptr [eax], 0
    add eax, 4
    dec ecx
    jne @lp2_a1
    xor eax, eax
    xor edx, edx
    ret
loop_ptr_a ENDP
loop_ptr_b PROC
    mov edx, [esp+4]
    mov ecx, [esp+8]
@lp2_b1:
    mov dword ptr [edx], 0
    add edx, 4
    dec ecx
    jne @lp2_b1
    xor eax, eax
    xor edx, edx
    ret
loop_ptr_b ENDP

neg_loop_stride_a PROC
    mov eax, [esp+4]
    mov ecx, [esp+8]
@nls_a1:
    mov dword ptr [eax], 0
    add eax, 4
    dec ecx
    jne @nls_a1
    ret
neg_loop_stride_a ENDP
neg_loop_stride_b PROC
    mov eax, [esp+4]
    mov ecx, [esp+8]
@nls_b1:
    mov dword ptr [eax], 0
    add eax, 8
    dec ecx
    jne @nls_b1
    ret
neg_loop_stride_b ENDP
END
