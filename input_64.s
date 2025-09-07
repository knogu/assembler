start:
    mov eax, ecx
    mov ecx, eax
    mov eax, 2222222
    mov ecx, 2222222
    mov rax, 8589934592
    mov rcx, 8589934592
    add eax, 2222222
    add ecx, 2222222
    add rax, 1073741824
    add rcx, 1073741824
    add eax, ecx
    add ecx, eax
    sub eax, 2222222
    sub ecx, 2222222
    sub eax, ecx
    sub ecx, eax
    sub eax, 2222222
    sub rax, 1073741824
    jmp short start
    ret
