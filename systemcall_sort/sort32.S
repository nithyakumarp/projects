    .global main

    .data
usage_msg:      .string "Usage: ./sort input output"

    .text
length:
    enter $0,$0
    mov 8(%ebp),%ebx
    mov %ebx, %esi
loopo: cmpb $0,(%ebx)
    je outofloop
    inc %ebx
    jmp loopo
outofloop: sub %esi,%ebx
    mov %ebx,%eax

    leave
    ret

inssort:
    enter $0, $0
    push %ebx
    push %esi
    push %edi
    mov 8(%ebp), %edi // %edi is array addr.
    mov 12(%ebp), %esi  // %esi is size

    mov $1, %ecx        //%ecx is i

floop: cmp %esi, %ecx // i<size
    jge fordone
    
    mov (%edi, %ecx, 4), %edx  // %edx is key
    dec %ecx
    mov %ecx, %ebx  // %ebx is j
    inc %ecx
//while (j >= 0 && arr[j] > key)
    wloop: cmp $0, %ebx
        jl whiledone
        cmp %edx, (%edi, %ebx, 4)
        jle whiledone
        mov (%edi,%ebx, 4), %eax// %eax is arr[j]
        inc %ebx
        mov %eax, (%edi,%ebx,4) // arr[j+1]= arr[j]
        dec %ebx
        dec %ebx
        jmp wloop
    whiledone:
    inc %ebx
    mov %edx, (%edi, %ebx,4)
    dec %ebx
    inc %ecx
    jmp floop
fordone:
    mov %edi, %eax 

    pop %edi
    pop %esi
    pop %ebx    
    leave
    ret

main:
    enter $128, $0
    push %ebx
    push %edi
    push %esi
//check for argc
    cmp $3, 8(%ebp)
    jne usage

    mov 12(%ebp), %eax
    mov 4(%eax), %ebx
    mov 8(%eax), %edi

//input_fd=open(argv[1],0 for O_RDONLY)
    mov $5, %eax
    mov %ebx, %ebx
    mov $0, %ecx
    mov $0, %edx
    int $0x80
    mov %eax, %esi

//checking sys_open 
    cmp $0, %eax
    jl  err
    

//newstat
    mov $0x6c, %eax
    mov %esi, %ebx
    lea 0(%esp), %ecx
    int $0x80
//checking newstat
    cmp $-1, %eax
    je err

//allocating stack mem
    mov 20(%esp), %edx
    sub %edx, %esp
//    lea 0(%esp),%edx

//realign
mov $15, %eax
    not %eax
    and %eax, %esp


//sys_read
    mov $3, %eax
    mov %esi, %ebx
    lea 0(%esp),%ecx
    int $0x80
    mov %eax, %edx
    push %edx

//checking failure
    cmp $-1, %eax
    je err

//close 
    mov %esi, %ebx
    mov $6, %eax
    int $0x80

//calling sort

    mov %edx, %eax
    mov $4, %ebx
    cdq
    idiv %ebx
    lea 4(%esp), %ebx
    push %eax
    push %ebx
    call inssort
    add $8, %esp
    push %eax

//output_fd=open()
    mov $5, %eax
    mov %edi,%ebx
    mov $578, %ecx
    mov $0644, %edx
    int $0x80
    mov %eax, %edi
//checking failure
    cmp $0, %eax
    jl err

//write()
    pop %ecx
    pop %edx
    mov $4, %eax
    mov %edi, %ebx
    int $0x80
    
//checking failure
    cmp $-1, %eax
    je err

//close(output_fd)
    mov $6, %eax
    mov %edi, %ebx
    int $0x80
    mov $0, %eax

    jmp maindone

usage:
    push $usage
    call length
    mov $26, %edx
    add $4, %esp
    mov $4, %eax
    mov $1, %ebx
    mov $usage_msg, %ecx
    int $0x80
    mov $1, %eax
    mov $1, %ebx
    int $0x80
//    jmp maindone

err:
    neg %eax
    push %eax
    call strerror
    add $4, %esp
    mov %eax, %esi
    push %eax
    call length
    mov %eax, %edx
    add $4, %esp

//write()
    mov $4, %eax
    mov $2, %ebx
    mov %edi, %ecx
    int $0x80
    
    mov $1, %eax
    mov $1, %ebx
    int $0x80



maindone:
    pop %esi
    pop %edi
    pop %ebx
    leave
    ret

 
