.global main

.text

//arithmetic operations are defined
//N1 and N2 in the safe registers are copied to temporary registers %r10 and %r11 and operations are performed on them
//The result is in %rax

add:
    mov %r15, %r10
    mov %r12, %r11
    add %r10, %r11
    mov %r11, %rax
    jmp done
sub:
    mov %r15, %r10
    mov %r12, %r11
    sub %r10, %r11
    mov %r11, %rax
    jmp done
mul:
    mov %r15, %r10
    mov %r12, %r11
    imul %r11, %r10
    mov %r10, %rax
    jmp done
div:
    mov %r15, %r10
    mov %r12, %r11
    cqo                 //cqo clears the %rdx register
    mov %r11, %rax
    idiv %r10
    jmp done

done:
    //print the result
    mov $result, %rdi
    mov %r12, %rsi
    mov %rbx, %rdx
    mov %r15, %rcx
    mov %rax, %r8
    mov $0, %al 
    call printf

//restore the saved registers back, leave and return
    pop %r12
    pop %r15
    pop %rbx
    leave
    ret    
   
main:
    enter $24, $0
// save the safe registers: callee save
    push %r12
    push %r15
    push %rbx
/*Variable mapping: %r12 = argv[1] = N1, 
                    %rbx = argv[2] = op (+, -, *, /),
                    %r15 = argv[3] = N2
*/
                 //%rdi = argc and it is compared to check if there are 4 arguments as needed
    cmp $4, %rdi
    jne error        
    
    mov 8(%rsi), %r12
    mov 16(%rsi), %rbx
    mov 24(%rsi), %r15
    
// convert argc numbers that are recieved as string to integers for calculation

    mov %r12, %rdi
    call atoi
    mov %rax, %r12
    mov %r15, %rdi
    call atoi
    mov %rax, %r15    
    
   //compare (%rbx) which is the first character of the string argv[2] (argv[2][0]).

    cmpb $'+ , (%rbx)
    je add
    cmpb $'- , (%rbx)
    je sub
    cmpb $'* , (%rbx)
    je mul
    cmpb $'/ , (%rbx)
    je div
            //This part of the code will be executed if the operator entered doesn't match with the ones above
error:
           //print usage message, pop out the safe registers, leave and return
    mov $usage, %rdi
    call puts
    pop %r12
    pop %r15
    pop %rbx
    leave
    ret

.data
    usage: .string "Usage: ./calc64 N op N"    
    result: .string "%d %s %d = %d"
