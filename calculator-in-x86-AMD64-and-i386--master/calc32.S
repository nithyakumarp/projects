.global main
.text

//calculations are performed on the values after copying them onto temporary registers
// Result = %eax
add:
   mov %ebx, %ecx
   mov %esi, %eax
   add %ecx, %eax 
   jmp done
sub:
   mov %ebx, %ecx
   mov %esi, %eax
   sub %eax, %ecx
   mov %ecx, %eax
   jmp done
mul:
     mov %ebx, %ecx
     mov %esi, %eax
     imul %ecx, %eax
     jmp done
div:
// eax/ecx
    cdq                   //simlar to cqo (clears %edx)
    mov %esi, %ecx
    mov %ebx, %eax
    idiv %ecx
    jmp done
done:
                    // print result
    push %eax
    push %esi
    push %edi
    push %ebx
    push $printfmt1
    call printf
    add $20, %esp     
    leave
    ret

main:
    enter $16, $0

    mov 8(%ebp), %eax    
    cmp $4, %eax
    jne error 
/* Variable mapping: %ebx = argv[1] = N1
                     %edi = argv[2] = op (+, -, *, /)
                     %esi = argv[3] = N2
                     %eax = result
*/
    mov 12(%ebp), %ecx
    mov 4(%ecx), %ebx
    mov 8(%ecx), %edi
    mov 12(%ecx), %esi

     

    push %ebx
    call atoi
    add $4, %esp
    mov %eax, %ebx
    
    push %esi
    call atoi
    add $4, %esp
    mov %eax, %esi
                    //op is compared with argv[2][0]
    cmpb $'+, (%edi)
    je add
    cmpb $'-, (%edi)
    je sub
    cmpb $'*, (%edi)
    je mul
    cmpb $'/, (%edi)
    je div
   
error:
    push $usage
    call puts
    leave
    ret
   
.data
    printfmt1: .string "%d %s %d = %d"
    usage: .string "Usage: ./calc32 N op N"
