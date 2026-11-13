; Synthetic pairs for re/tools/match/equiv.py (EQUIV-CHECK). Each case is two procedures <name>_a (plays "ours")
; and <name>_b (plays "the original"). Expected verdicts live in re/tools/match/test_equiv.py (CASES).
; Assembled with MSVC ml.exe /c /coff. db sequences force one encoding where MASM would pick another.
; Globals are absolute addresses (G1 = 006A0000h ...), external callees are real MASHED.exe functions:
;   FUN_0040bb30: cdecl, 1 stack argument, no register argument
;   FUN_0048cee0: reads EDI as a register argument
.686
.xmm
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

; integer motion across x87 (EQUIV-X87-INTEGER-MOTION, L-147 (a)): admitted only when independent
x87_move_indep_a PROC                   ; was neg_x87_sched: the load crosses fld [G1], no shared reg/memory
    fld dword ptr ds:[G1]
    mov eax, [esp+4]
    fmul dword ptr ds:[G2]
    fstp dword ptr ds:[G3]
    mov dword ptr ds:[G1], eax
    ret
x87_move_indep_a ENDP
x87_move_indep_b PROC
    mov eax, [esp+4]
    fld dword ptr ds:[G1]
    fmul dword ptr ds:[G2]
    fstp dword ptr ds:[G3]
    mov dword ptr ds:[G1], eax
    ret
x87_move_indep_b ENDP

x87_move_rr_a PROC                      ; integer load of [G1] crosses fld [G1]: read-read
    fld dword ptr ds:[G1]
    mov eax, dword ptr ds:[G1]
    fstp dword ptr ds:[G3]
    mov dword ptr ds:[G2], eax
    ret
x87_move_rr_a ENDP
x87_move_rr_b PROC
    mov eax, dword ptr ds:[G1]
    fld dword ptr ds:[G1]
    fstp dword ptr ds:[G3]
    mov dword ptr ds:[G2], eax
    ret
x87_move_rr_b ENDP

neg_x87_fnstsw_a PROC                   ; mov eax crosses fnstsw ax (both write AX/EAX)
    fld dword ptr ds:[G1]
    fcomp dword ptr ds:[G2]
    fnstsw ax
    mov eax, [esp+4]
    mov dword ptr ds:[G3], eax
    ret
neg_x87_fnstsw_a ENDP
neg_x87_fnstsw_b PROC
    fld dword ptr ds:[G1]
    fcomp dword ptr ds:[G2]
    mov eax, [esp+4]
    fnstsw ax
    mov dword ptr ds:[G3], eax
    ret
neg_x87_fnstsw_b ENDP

neg_x87_sahf_a PROC                     ; sahf crosses fnstsw ax (sahf reads AH)
    fld dword ptr ds:[G1]
    fcomp dword ptr ds:[G2]
    sahf
    fnstsw ax
    seta al
    movzx eax, al
    ret
neg_x87_sahf_a ENDP
neg_x87_sahf_b PROC
    fld dword ptr ds:[G1]
    fcomp dword ptr ds:[G2]
    fnstsw ax
    sahf
    seta al
    movzx eax, al
    ret
neg_x87_sahf_b ENDP

neg_x87_alias_a PROC                    ; integer store to [G3] crosses fstp [G3] (write-write alias)
    fld dword ptr ds:[G1]
    mov dword ptr ds:[G3], 0
    fstp dword ptr ds:[G3]
    ret
neg_x87_alias_a ENDP
neg_x87_alias_b PROC
    fld dword ptr ds:[G1]
    fstp dword ptr ds:[G3]
    mov dword ptr ds:[G3], 0
    ret
neg_x87_alias_b ENDP

neg_x87_eflags_a PROC                   ; xor (EFLAGS writer) crosses fcomip (EFLAGS writer)
    fld dword ptr ds:[G1]
    fld dword ptr ds:[G2]
    fcomip st, st(1)
    xor ecx, ecx
    fstp st(0)
    seta cl
    mov eax, ecx
    ret
neg_x87_eflags_a ENDP
neg_x87_eflags_b PROC
    fld dword ptr ds:[G1]
    fld dword ptr ds:[G2]
    xor ecx, ecx
    fcomip st, st(1)
    fstp st(0)
    seta cl
    mov eax, ecx
    ret
neg_x87_eflags_b ENDP

x87_move_frame_a PROC                   ; frame-slot load crosses fstp through an argument pointer (A1)
    sub esp, 4
    mov eax, [esp+8]
    fld dword ptr ds:[G1]
    fstp dword ptr [eax]
    mov ecx, [esp]
    mov dword ptr ds:[G2], ecx
    add esp, 4
    ret
x87_move_frame_a ENDP
x87_move_frame_b PROC
    sub esp, 4
    mov eax, [esp+8]
    fld dword ptr ds:[G1]
    mov ecx, [esp]
    fstp dword ptr [eax]
    mov dword ptr ds:[G2], ecx
    add esp, 4
    ret
x87_move_frame_b ENDP

neg_x87_slot_a PROC                     ; load of an argument slot crosses fstp into that same slot
    fld dword ptr ds:[G1]
    mov ecx, [esp+4]
    fstp dword ptr [esp+4]
    mov dword ptr ds:[G2], ecx
    ret
neg_x87_slot_a ENDP
neg_x87_slot_b PROC
    fld dword ptr ds:[G1]
    fstp dword ptr [esp+4]
    mov ecx, [esp+4]
    mov dword ptr ds:[G2], ecx
    ret
neg_x87_slot_b ENDP

neg_x87_call_a PROC                    ; integer load crosses a call inside an x87 region: never admitted
    fld dword ptr ds:[G1]
    mov ecx, dword ptr ds:[G2]
    call FUN_0040bb30
    fstp dword ptr ds:[G3]
    mov dword ptr ds:[G2], ecx
    ret
neg_x87_call_a ENDP
neg_x87_call_b PROC
    fld dword ptr ds:[G1]
    call FUN_0040bb30
    mov ecx, dword ptr ds:[G2]
    fstp dword ptr ds:[G3]
    mov dword ptr ds:[G2], ecx
    ret
neg_x87_call_b ENDP

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

; ------------------------------------------------------------------ A6: calls through a pointer (EQUIV-CALLEE-POPS)
; A COM method call: this in ECX, vtable in EAX, __stdcall (the callee pops its arguments, no add esp after it).
com_ident_a PROC
    mov ecx, [esp+4]
    mov eax, [ecx]
    push 7
    push ecx
    call dword ptr [eax+6Ch]
    mov dword ptr ds:[G1], eax
    ret
com_ident_a ENDP
com_ident_b PROC
    mov ecx, [esp+4]
    mov eax, [ecx]
    push 7
    push ecx
    call dword ptr [eax+6Ch]
    mov dword ptr ds:[G1], eax
    ret
com_ident_b ENDP

com_stdcall_a PROC
    mov ecx, [esp+4]
    mov eax, [ecx]
    push 7
    push ecx
    call dword ptr [eax+6Ch]
    test eax, eax
    jl @cs_a9
    push 1
    call FUN_0040bb30
    add esp, 4
    mov eax, [esp+4]
    mov eax, [eax]
    push 0
    push dword ptr [esp+8]
    call dword ptr [eax+70h]
@cs_a9:
    ret
com_stdcall_a ENDP
com_stdcall_b PROC
    mov ecx, [esp+4]
    mov eax, [ecx]
    db 68h, 07h, 0, 0, 0                ; push 7 (imm32)
    push ecx
    call dword ptr [eax+6Ch]
    test eax, eax
    jl @cs_b9
    db 68h, 01h, 0, 0, 0                ; push 1 (imm32)
    call FUN_0040bb30
    add esp, 4
    mov eax, [esp+4]
    mov eax, [eax]
    push 0
    push dword ptr [esp+8]
    call dword ptr [eax+70h]
@cs_b9:
    ret
com_stdcall_b ENDP

com_cdecl_a PROC
    mov ecx, [esp+4]
    mov eax, [ecx]
    push 7
    push ecx
    call dword ptr [eax+6Ch]
    add esp, 8
    mov dword ptr ds:[G1], eax
    ret
com_cdecl_a ENDP
com_cdecl_b PROC
    mov ecx, [esp+4]
    mov eax, [ecx]
    db 6Ah, 07h                         ; push 7 (imm8)
    push ecx
    call dword ptr [eax+6Ch]
    add esp, 8
    mov dword ptr ds:[G1], eax
    ret
com_cdecl_b ENDP

com_loop_a PROC
    push esi
    mov esi, [esp+8]
@cl_a1:
    mov ecx, [esp+12]
    mov eax, [ecx]
    push esi
    push ecx
    call dword ptr [eax+6Ch]
    dec esi
    jne @cl_a1
    pop esi
    ret
com_loop_a ENDP
com_loop_b PROC
    push esi
    mov esi, [esp+8]
@cl_b1:
    db 8Bh, 8Ch, 24h, 0Ch, 0, 0, 0      ; mov ecx, [esp+12] (disp32)
    mov eax, [ecx]
    push esi
    push ecx
    call dword ptr [eax+6Ch]
    dec esi
    jne @cl_b1
    pop esi
    ret
com_loop_b ENDP

mxcsr_a PROC
    push ecx
    stmxcsr dword ptr [esp]
    mov eax, [esp]
    mov dword ptr ds:[G1], eax
    ldmxcsr dword ptr ds:[G2]
    pop ecx
    ret
mxcsr_a ENDP
mxcsr_b PROC
    push ecx
    stmxcsr dword ptr [esp]
    mov edx, [esp]
    mov dword ptr ds:[G1], edx
    ldmxcsr dword ptr ds:[G2]
    pop ecx
    mov eax, edx
    ret
mxcsr_b ENDP

; wrong prototype arg count: ours passes one argument, the original two
neg_com_argcount_a PROC
    mov ecx, [esp+4]
    mov eax, [ecx]
    push 7
    push ecx
    call dword ptr [eax+6Ch]
    ret
neg_com_argcount_a ENDP
neg_com_argcount_b PROC
    mov ecx, [esp+4]
    mov eax, [ecx]
    push 8
    push 7
    push ecx
    call dword ptr [eax+6Ch]
    ret
neg_com_argcount_b ENDP

; swapped call order
neg_com_order_a PROC
    mov ecx, [esp+4]
    mov eax, [ecx]
    push ecx
    call dword ptr [eax+6Ch]
    mov ecx, [esp+4]
    mov eax, [ecx]
    push ecx
    call dword ptr [eax+70h]
    ret
neg_com_order_a ENDP
neg_com_order_b PROC
    mov ecx, [esp+4]
    mov eax, [ecx]
    push ecx
    call dword ptr [eax+70h]
    mov ecx, [esp+4]
    mov eax, [ecx]
    push ecx
    call dword ptr [eax+6Ch]
    ret
neg_com_order_b ENDP

; ours typed cdecl (caller cleans), the original __stdcall
neg_com_conv_a PROC
    mov ecx, [esp+4]
    mov eax, [ecx]
    push 7
    push ecx
    call dword ptr [eax+6Ch]
    add esp, 8
    ret
neg_com_conv_a ENDP
neg_com_conv_b PROC
    mov ecx, [esp+4]
    mov eax, [ecx]
    push 7
    push ecx
    call dword ptr [eax+6Ch]
    ret
neg_com_conv_b ENDP

; two calls through a pointer whose pops the stack equations cannot separate (P1 + P2 = 4): a deferred cdecl
; cleanup and a __stdcall callee read the same as the reverse. Proven once per solution (both pass: same shape).
com_ambiguous_a PROC
    mov eax, [esp+4]
    push 1
    call dword ptr [eax+10h]
    mov eax, [esp+8]
    db 68h, 02h, 0, 0, 0                ; push 2 (imm32)
    call dword ptr [eax+14h]
    add esp, 4
    ret
com_ambiguous_a ENDP
com_ambiguous_b PROC
    mov eax, [esp+4]
    push 1
    call dword ptr [eax+10h]
    mov eax, [esp+8]
    push 2
    call dword ptr [eax+14h]
    add esp, 4
    ret
com_ambiguous_b ENDP

; the same ambiguity, but ours is right only under one solution (P1 = 0, P2 = 4): under (4, 0) ours pushes its
; second argument over its own return address
neg_com_solution_a PROC
    mov eax, [esp+4]
    push 1
    call dword ptr [eax+10h]
    add esp, 4
    mov eax, [esp+8]
    push 2
    call dword ptr [eax+14h]
    ret
neg_com_solution_a ENDP
neg_com_solution_b PROC
    mov eax, [esp+4]
    push 1
    call dword ptr [eax+10h]
    mov eax, [esp+8]
    push 2
    call dword ptr [eax+14h]
    add esp, 4
    ret
neg_com_solution_b ENDP

; MXCSR loaded from different words
neg_mxcsr_a PROC
    ldmxcsr dword ptr ds:[G2]
    ret
neg_mxcsr_a ENDP
neg_mxcsr_b PROC
    ldmxcsr dword ptr ds:[G3]
    ret
neg_mxcsr_b ENDP
END
