.global returnSmaller
returnSmaller:
        cmp     %edi, %esi
        mov     %eax, %esi
        cmovle  %eax, %edi
        ret
