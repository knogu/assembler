start:
    mov eax, ecx
    mov ecx, eax
    mov eax, 2222222
    add eax, 2222222
    sub eax, 2222222
    add ecx, 2222222
    sub ecx, 2222222
    mov ecx, 2222222
    mov eax, [ecx]
    mov ecx, [eax]
    push eax
    push 42
    jmp short start
    ret
