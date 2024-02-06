;NASM file
global _returnSmaller
global returnSmaller
section .text
_returnSmaller:
returnSmaller:
returnSmaller:
	mov r15, rcx
	mov rax, 100
	cmp r15, rax
	cmovl rax,r15
	ret 
