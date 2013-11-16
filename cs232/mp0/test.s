# comments are delimited by hash marks

.data
silly_str: .asciiz "dmcwhe2\n"

.text
main:
    li $v0, 4           # load value "4" into register $v0
    la $a0, silly_str   # load the address of "silly_str" into reg $a0
    syscall             # perform the "print_string" system call
    jr $ra              # return to the calling proc
