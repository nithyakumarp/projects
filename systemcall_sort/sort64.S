    .global main
    .data
usage_msg:  .string "Usage: ./sort input output"
printtest:  .string "Size: %d\n"
err: .string "Error occured\n"
    .text

length:
    enter $0,$0
    mov %rdi, %rcx
    // %rcx is text
    // z=text
    //(%rdi) is *z
loop: cmpb $0, (%rdi)
    je outofloop
    //++z
    //z is %rdi
    inc %rdi
    jmp loop
outofloop: sub %rcx,%rdi        //return z-text
    mov %rdi,%rax
    leave
    ret
   
inssort:
    enter $0, $0
    push %r15
    push %r14
    push %r13
    push %r12
    push %rbx 
//i=1;
// i is %rcx
//i=1
    mov $1, %rcx
//for (i=1;i<size;i++)
//size is %esi
floop: cmp %esi, %ecx
    jge fordone
    // key = %rdx
    //j= %r10
    mov $0, %rdx
    //key = arr[i]
    mov (%rdi,%rcx,4), %rdx
    mov %rcx, %r8
    //j=i-1
    dec %r8
    mov %r8, %r10

//while (j >= 0 && arr[j] > key)
whloop: cmp $0,%r10
    jl whiledone
    cmp (%rdi,%r10,4), %edx
    jge whiledone
    mov (%rdi,%r10,4), %r8
    //arr[j+1]= arr[j]
    inc %r10
    mov %r8d,(%rdi,%r10,4)
    dec %r10
    //j= j-1;
    dec %r10
    jmp whloop
whiledone:
    //arr[j+1] = key
    mov %r10, %r8
    inc %r8
    mov %edx, (%rdi,%r8,4)
    inc %rcx
    jmp floop
fordone:
    mov %rdi,%rax
    pop %rbx
    pop %r12
    pop %r13
    pop %r14
    pop %r15
    leave 
    ret
error:
	neg %rax
    mov $err, %rdi  
    call perror   
    
    mov $60, %rax 
    mov $1, %rdi 
    syscall
main:
    enter $256, $0
 
    cmp $3,%rdi
    jne usage_err
    
//%r12= argv[1]; 
//%r13= argv[2];
    mov 8(%rsi), %r12
    mov 16(%rsi), %r13
    
    //input_fd= open(argv[1], 0 for O_RDONLY)
    mov $2, %rax
    mov %r12,%rdi
    mov $0,%rsi
    mov $0, %rdx
    syscall
    mov %rax, %r14  // input_fd is %r14
	
	cmp $0, %rax
	jl error
    
    //stat to find size
    mov $4, %rax
    mov %r12, %rdi
    mov %rsp, %rsi
    syscall
    //size in %r15
    mov 48(%rsp),%r15
    mov %r15, %rax
    mov $4, %rbx
    cqo
    idiv %rbx
    mov %rax, %rbx
  /*  mov $printtest, %rdi
    mov %r15, %rsi
    call printf
*/
  /*realigning stack
     mov $15, %r10
    not %r10
    and %r10, %rsp
  */
    //allocate stack space
    sub %r15, %rsp
    lea 0(%rsp), %r12

   
    //read(input,read_buf, size)
    mov %r14, %rdi
    mov $0, %rax
    mov %r12, %rsi
    mov %r15, %rdx
    syscall

	cmp $-1, %rax
	je error
/*
    mov $printtest, %rdi
    mov %rax, %rsi
    call printf
   */ 
    //close(input_fd)
    mov $3, %rax
    mov %r14, %rdi
    syscall


    //sort
    mov %r12, %rdi
    mov %rbx, %rsi
    call inssort 
 
//output_fd = open(argv[2],578 (for O_RDWR, O_CREAT and O_TRUNC))
    mov $2, %rax
    mov %r13, %rdi
    mov $577, %rsi 
    mov $0644, %rdx
    syscall
    mov %rax, %r8
   //output_fd is %r8
   
	cmp $0, %rax
	jl error

    ////write(output_fd,read_buf,size);
    mov %r8, %rdi
    mov %r12, %rsi
    mov %r15, %rdx
    mov $1, %rax
    syscall    

	cmp $-1,%rax
	je error
    /*
    mov $printtest, %rdi
    mov %rax, %rsi
    call printf
  */
      mov $0, %rax
    jmp main_done
usage_err:
    // to print usage message
    mov $usage_msg, %rdi
    call length
    mov %rax,%rdx
    mov $1, %rax
    mov $1, %rdi
    mov $usage_msg, %rsi
    syscall
    mov $1, %rax
main_done:
    leave
    ret
