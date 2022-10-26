public SGE1

extern puts: PROC
extern sge_return_addy: QWORD

OPTION PROLOGUE: NONE
OPTION EPILOGUE: NONE

.data
number_of_args dq 0
current_args dq 0
sender_id dq 0

.code

SGE1 PROC; fastcall
	pushfq
	test rdx, rdx
	je exit2
	push rdi
	mov rdi, rcx
	add rdi, 548
	test rdi, rdi
	je exit1
	mov [number_of_args], rdi
	cmp dword ptr [rdi], 1AFh
	jg exit1
	mov rdi, rcx
	add rdi, 112
	test rdi, rdi
	je exit1
	mov [current_args], rdi
	mov [sender_id], rdi
	mov rdi, [number_of_args]
	mov edi, [rdi]
	sar edi, 3
	push rax
	xor rax, rax
	args_loop:
		inc rax
		cmp eax, edi
		jne args_loop
	;pop rax
apartment_invite:
	mov rdi, [current_args]
	mov rax, [rdi]
	xor rax, -1253256204 ; apartment invite
	;test rax, rax
	jne transaction_error_screen
	mov rax, 1
	ret
transaction_error_screen:
	mov rax, [rdi]
	xor rax, -881249731 ; transaction error screen
	;test rax, rax
	jne remote_kill_engine
	mov rax, 1
	ret
remote_kill_engine:
	mov rax, [rdi]
	xor rax, 472658729 ; remote kill engine
	;test rax, rax
	jne remote_vehicle_kick
	mov rax, 1
	ret
remote_vehicle_kick:
	mov rax, [rdi]
	xor rax, -130330100 ; remote vehicle kick
	;test rax, rax
	jne remote_off_radar
	mov rax, 1
	ret
remote_off_radar:
	mov rax, [rdi]
	xor rax, -1363050950 ; remote off radar
	;test rax, rax, 
	jne remote_ceo_kick
	mov rax, 1
	ret
remote_ceo_kick:
	mov rax, [rdi]
	xor rax, -1775205375 ; remote CEO kick
	;test rax, rax
	jne remote_ceo_ban
	mov rax, 1
	ret
remote_ceo_ban:
	mov rax, [rdi]
	xor rax, -1028825138 ; remote CEO ban
	;test rax, rax
	jne sound_spam1
	mov rax, 1
	ret
sound_spam1:
	mov rax, [rdi]
	xor rax, 1337731455 ; sound spam 1
	;test rax, rax
	jne sound_spam2
	mov rax, 1
	ret
sound_spam2:
	mov rax, [rdi]
	xor rax, 323449272 ; sound spam 2
	;testr rax, rax
	jne default
	mov rax, 1
	ret
default:
	pop rax
exit1:
	pop rdi
exit2:
	popfq
	push rax
	mov rax, sge_return_addy
	xchg rax, [rsp]
	ret
SGE1 ENDP
END