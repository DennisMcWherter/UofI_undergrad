.data
newline:.asciiz "\n"		# useful for printing commands
.word 0 0
node_0: .word	node_4, node_2
pad__0: .word	0, 0, 0, 0, 0, 0, 0, 0
node_1: .word	node_2, node_0
pad__1: .word	0, 0, 0, 0, 0, 0
node_2: .word	node_5, node_4
pad__2: .word	0, 0, 0, 0
node_3: .word	node_4, node_1
pad__3: .word	0, 0
node_4: .word	node_0, node_1
pad__4: .word	0, 0, 0, 0, 0, 0
node_5: .word	node_2, node_3
pad__5: .word	0, 0, 0, 0, 0, 0, 0, 0

.text
# main function
main:
	sub  	$sp, $sp, 4
	sw   	$ra, 0($sp) # save $ra on stack

	la	$a0, node_0
	jal	bit_reverse_ptr
	la	$a0, node_1
	jal	bit_reverse_ptr
	la	$a0, node_2
	jal	bit_reverse_ptr
	la	$a0, node_3
	jal	bit_reverse_ptr
	la	$a0, node_4
	jal	bit_reverse_ptr
	la	$a0, node_5
	jal	bit_reverse_ptr

	# test count_nodes
	la	$a0, node_0
	jal	count_nodes
	move	$a0, $v0
	jal	print_int_and_space
	# this should print 6

	# print a newline
	li	$v0, 4
	la	$a0, newline
	syscall	

	lw   	$ra, 0($sp) 	# restore $ra from stack
	add  	$sp, $sp, 4
	jr	$ra



print_int_and_space:
	li   	$v0, 1         	# load the syscall option for printing ints
	syscall              	# print the element

	li   	$a0, 32        	# print a black space (ASCII 32)
	li   	$v0, 11        	# load the syscall option for printing chars
	syscall              	# print the char
	
	jr      $ra          	# return to the calling procedure

print_newline:
	li	$v0, 4		# at the end of a line, print a newline char.
	la	$a0, newline
	syscall	    
	jr	$ra

bit_reverse_ptr:		# takes a pointer to a pair of memory locations that we should reverse (and put back)
	sub	$sp, $sp, 8
	sw	$ra, 0($sp)
	sw 	$a0, 4($sp)
	
	lw	$a0, 0($a0)	# load the first value to reverse
	jal	bit_reverse
	lw 	$a0, 4($sp)
	sw	$v0, 0($a0)	# write it back

	lw	$a0, 4($a0)	# load the second value to reverse
	jal	bit_reverse
	lw 	$a0, 4($sp)
	sw	$v0, 4($a0)	# write it back

	lw	$ra, 0($sp)
	add	$sp, $sp, 8
	jr	$ra

## unsigned bit_reverse(unsigned in);
bit_reverse:
sub     $sp, $sp, 4     # Allocate stackframe
    sw      $ra, 0($sp)     # Save return address
    li	    $v0, 0
    li      $s0, 31         # int i = 31
    jal     br_loop
    lw      $ra, 0($sp)
    add     $sp, $sp, 4     # Deallocate stackframe
	jr	$ra

br_loop:
    and     $t0, $a0, 0x01  # $t0 = $a0 & 0x01 
    or      $v0, $v0, $t0   # $v0 |= $t0
    sll     $v0, $v0, 1     # $v0 <<= 1
    srl     $a0, $a0, 1     # $a0 >>= 1
    sub     $s0, $s0, 1     # $s0 = $s0 - 1 / i--
    bne     $s0, $0, br_loop
    jr $ra

# ALL your code goes below this line.
#
# We will delete EVERYTHING above the line; DO NOT delete 
# the line.
#
# ---------------------------------------------------------------------
	
## unsigned count_nodes(node_t *node);
count_nodes:
  sub  $sp, $sp, 16   # Allocate stack-frame
  sw   $ra, 0($sp)    # Save return address
  sw   $a0, 4($sp)    # Save original $a0
	li	 $v0, 0
  lw   $t0, 0($a0)    # $t0 = node->p[0]
  add  $t1, $0, 0x01  # $t1 = 0x01
  and  $t1, $t0, $t1  # $t1 &= $t0 (i.e. 0x01)
  beq  $t1, 0x01, cn_return
  add  $v0, $v0, 1    # $v0 += 1
  lw   $a0, 0($a0)    # $a0 = p->[0]
  sw   $v0, 8($sp)    # Save current return value
  ##### Call Bit reverse here on $a0 #####
  jal bit_reverse
  sw   $v0, 12($sp)   # Save bit_reverse(value)
  lw   $a0, 4($sp)    # Restore original $a0
  add  $t0, $0, $v0   # $t0 = $v0 (i.e. p = bit_reverse(value))
  or   $t0, $t0, 0x01 # $t0 = $t0 | 0x01
  sw   $t0, 0($a0)    # Mark node
  lw   $a0, 4($a0)    # $a0 = $a0->p[1]
  ##### Call bit reverse here on $a0 #####
  jal bit_reverse
  lw   $a0, 4($sp)    # Restore original $a0
  add  $t0, $0, $v0   # $t0 = $v0
  sw   $t0, 4($a0)    # $a0->p[1] = $t0
  lw   $a0, 12($sp)   # Restore bit_reversed $a0->p[0]
  jal count_nodes
  lw   $t0, 8($sp)    # Restore orginal return value
  add  $v0, $v0, $t0  # $v0 += $t0
  sw   $v0, 8($sp)    # Save current return value
  lw   $a0, 4($sp)    # Restore original $a0
  lw   $a0, 4($a0)    # $a0 = $a0->p[1] (reversed version)
  jal count_nodes
  lw   $t0, 8($sp)    # Restore original return value
  add  $v0, $v0, $t0  # $v0 += $t0

cn_return:
  lw   $ra, 0($sp)   # Restore return address
  add  $sp, $sp, 16  # Deallocate stack-frame
	jr   $ra
