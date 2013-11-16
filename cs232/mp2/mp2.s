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

	# test LSB_set 
	li	$a0, 0x02480248
	jal	LSB_set
	move	$a0, $v0
	jal	print_int_and_space
	# this should print 38273609

	# test LSB_clear 
	li	$a0, 0x02480249
	jal	LSB_clear
	move	$a0, $v0
	jal	print_int_and_space
	# this should print 38273608 

	# test bit_reverse
	li	$a0, 0x12345678
	jal	bit_reverse
	move	$a0, $v0
	jal	print_int_and_space
	# this should print 510274632

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

# ALL your code goes below this line.
#
# We will delete EVERYTHING above the line; DO NOT delete 
# the line.
#
# ---------------------------------------------------------------------
	
## unsigned LSB_set(unsigned in); 
LSB_set:
    sub     $sp, $sp, 4     # Allocated stack-frame
    sw      $ra, 0($sp)     # Store return address
    jal     LSB_clear       # Make sure LSB is not set first
    move    $a0, $v0        # Store return value
    or      $a0, $a0, 1     # $a0 = $a0 | 0x01;
    lw      $ra, 0($sp)     # Restore return address
    add     $sp, $sp, 4     # Deallocate stack-frame
    move    $v0, $a0
	jr	$ra

## unsigned LSB_clear(unsigned in); 
LSB_clear:
    li      $t0, 1          # $t0 = 1
    nor     $t0, $t0, $0    # $t0 = ~$t0
    and     $a0, $a0, $t0   # $a0 = $a0 & $t0 (i.e. ~0x01)
    move    $v0, $a0        # return value
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

## unsigned count_nodes(node_t *node);
count_nodes:
    sub     $sp, $sp, 4     # Allocate stackframe
    sw      $ra, 0($sp)     # Save return address
	li	    $v0, 0          # zero-out return value
    jal     cn_recurse
    lw      $ra, 0($sp)     # Restore return address
    add     $sp, $sp, 4     # Deallocate stackframe
	jr	$ra

cn_recurse:
    sub     $sp, $sp, 8     # Allocate stackframe
    sw      $ra, 0($sp)     # Save return address
    sw      $a0, 4($sp)     # Save original argument
    lw      $t0, 0($a0)
    and     $t1, $t0, 0x01  # $t1 = $t0 & 0x01
    beq     $t1, 0x01, cn_done
    add     $v0, $v0, 0x01  # $v0 += 0x01;
    or      $t1, $t0, 0x01  # $t1 = $t0 | 0x01
    sw      $t1, 0($a0)     # Mark node
    move    $a0, $t0        # Pass the original address to check
    jal     cn_recurse
    lw      $t0, 4($sp)     # Restore original argument
    lw      $a0, 4($t0)     # Check p[1]
    jal     cn_recurse

cn_done:
    lw      $ra, 0($sp)     # Restore return address
    add     $sp, $sp, 8     # Deallocate stackframe
    jr $ra

