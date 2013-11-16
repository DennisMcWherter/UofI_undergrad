	.data
bonked:	.word		0
clue: .space 4096
clue_avail: .word 0
block: .word -1
lockedblock: .word -1
clue_req: .word -1
w00t: .asciiz "Rohan waz h3r3...\n"
dos: .asciiz "Key Computed...\n"
tres: .asciiz "Key Computing...\n"
	.text
main:	
 	li	$t4, 0x5001	# flag for bonk interrupt set, and global interrupt enable
 	mtc0 	$t4, $12	# Enable interrupt mask (Status register)

	li	$t3, 0

m_loop:	
 	li	$t0, 10
 	sub	$t0, $t0, $t3
 	
	sw	$t0, 0xffff0070($zero)
	lw	$a0, 0xffff0070($zero)	# block x and y location
	lw	$a1, 0xffff0074($zero)

	blt	$a0, 140, next		# already on my side
	bgt	$a0, 280, next 		# not reachable
	
	sw	$t0, block($0)
	
	
	add 	$a0, $a0, 20		# try to over shoot it

# GET ALIGNED IN +X DIRECTION	
	sw	$zero, 0xffff0014($zero) # turn to drive in +x
	li	$t0, 1
	sw	$t0, 0xffff0018($zero)
	
# Double check that locked block exists
	lw   $t5, lockedblock($0)
	beq  $t5, -1, x_pos
	  
# Otherwise, check if we have a clue available
# If available, solve, set clue_avail = 0 and clue_req = -1
check_clue:

	lw  $t5, clue_avail($0)
	beq $t5, 0, x_pos

        li	$t7, 0			
	sw	$t7, 0xffff0010($zero)		# set velocity


	la  $t5, clue
	add $t6, $a0, $0
	add $a0, $t5, $0
	
	jal count_nodes
	li	$t7, 10				
	sw	$t7, 0xffff0010($zero)		# set velocity
	
	
	lw  $t7, lockedblock($0)
	sw  $t7, 0xffff0070($0)
	sw  $v0, 0xffff0090($0)
	add $a0, $t6, $0
	li  $t5, -1
	sw  $0, clue_avail($0)
	sw  $t5, lockedblock($0)
		
x_pos:

	lw  $t7, block
	sw  $t7, 0xffff0070($0)
	lw  $a0, 0xffff0070($0)
	add $a0, $a0, 20
	li	$t0, 10				
	sw	$t0, 0xffff0010($zero)		# set velocity
	lw	$t2, 0xffff0020($zero)	# get my X position
	blt	$t2, $a0, x_pos		# keep going until I pass block



# GET ALIGNED IN +/-Y DIRECTION	
	lw	$t2, 0xffff0024($zero)	# get my Y position
	bgt	$t2, $a1, too_high

too_low:
	li	$t1, 90			# turn to drive in +y
	j	angle_y

too_high:
	li	$t1, 270		# turn to drive in -y

angle_y:
	sw	$t1, 0xffff0014($zero) 
	li	$t1, 1
	sw	$t1, 0xffff0018($zero)

y_pos:
	lw	$t2, 0xffff0024($zero)	# get my Y position
	sub	$t2, $a1, $t2
	abs	$t2, $t2
	bge	$t2, 2, y_pos		# keep going until I'm within 2


# GET ALIGNED IN THE -X DIRECTION 
	li	$t1, 180
	sw	$t1, 0xffff0014($zero) 
	li	$t1, 1
	sw	$t1, 0xffff0018($zero)

still_pushing:
	lw	$t4, bonked($zero)
	beq	$t4, 1, next	
	lw	$t2, 0xffff0020($zero)	# get my X position
	bgt	$t2, 140, still_pushing	# keep going until I pass block

next:	
	sw	$zero, bonked($zero)
	add	$t3, $t3, 1
	blt	$t3, 11, m_loop		# if block_num < 11
	li	$t3, 0			# otherwise start at block 0 again
	j	m_loop

	##  infinite outer loop	






##################### SOLVE OUR PUZZLEZZZ #######################


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
bit_reverse:				# code worth looking at.
	li	$t0, 0		# i 
	li	$v0, 0		# return value
	li	$t2, 31		# 31 - i
	
bit_reverse_loop:
	srl	$t1, $a0, $t0
	and	$t1, $t1, 1
	sll	$t1, $t1, $t2
	or	$v0, $v0, $t1

	sub	$t2, $t2, 1
	add	$t0, $t0, 1
	blt	$t0, 32, bit_reverse_loop

	jr	$ra


# ALL your code goes below this line.
#
# We will delete EVERYTHING above the line; DO NOT delete 
# the line.
#
# ---------------------------------------------------------------------
	
## unsigned count_nodes(node_t *node);
count_nodes:
	li	$v0, 0		# for base case
	lw	$t0, 0($a0)	# $t0 = value = node->p[0]
	and	$t1, $t0, 1	# 
	beq	$t1, 1, count_nodes_end

	# save registers	# recursive case
	sub    	$sp, $sp, 64
	sw	$ra, 0($sp)
	sw	$s0, 4($sp)	
	sw	$s1, 8($sp)	
	move	$s0, $a0	# $s0 = node

	move	$a0, $t0	# passing value to bit_reverse
	jal	bit_reverse

	or	$t1, $v0, 1	
	sw	$t1, 0($s0)	# node->p[0] = value | 1
	move	$s1, $v0	# $s1 = value (w/o the LSB_set)

	lw	$a0, 4($s0)	# passing node->p[1] to bit_reverse
	jal	bit_reverse
	sw	$v0, 4($s0)	# node->p[1] = result of bit_reverse

	move 	$a0, $s1	# call on bit_reversed node->p[0] w/o LSB set
	jal	count_nodes
	add	$s1, $v0, 1

	lw	$a0, 4($s0)	# call on bit_reversed node->p[1]
	jal	count_nodes
	add	$v0, $s1, $v0

	lw	$ra, 0($sp)	# clean up
	lw	$s0, 4($sp)
	lw	$s1, 8($sp)
	add    	$sp, $sp, 64

count_nodes_end:
	jr	$ra		# and return
























############# KERNEL CODE ###############

.data			# interrupt handler data (separated just for readability)
save:	     .word 0
	     .word 0
non_intrpt_str:   .asciiz "Non-interrupt exception\n"
unhandled_str:    .asciiz "Unhandled interrupt type\n"
something: .asciiz " Clue Received\n"
lebonk: .asciiz " I le Bonk'd\n"

.ktext 0x80000080
interrupt_handler:
	.set noat
	move	$k1, $at		# Save $at
	.set at
	la      $k0, save
	sw	$a0, 0($k0)		# Get some free registers
	sw	$a1, 4($k0)		# by storing them to a global variable

	mfc0 	$k0, $13		# Get Cause register
	srl 	$a0, $k0, 2		
	and 	$a0, $a0, 0xf		# ExcCode field
	bne 	$a0, 0, non_intrpt

interrupt_dispatch:			# Interrupt:
	mfc0 	$k0, $13		# Get Cause register, again
	beq	$k0, $zero, done	# handled all outstanding interrupts

	and 	$a0, $k0, 0x1000	# is there a bonk interrupt?
	bne 	$a0, 0, bonk_interrupt	

  and   $a0, $k0, 0x4000
  bne   $a0, 0, clue_interrupt

					# add dispatch for other interrupt types here.

	li $v0, 4			# Unhandled interrupt types
	la $a0, unhandled_str
	syscall
	b done
	
bonk_interrupt:
	li	$a1, 1
	sw	$a1, bonked($zero)
	sw	$a1, 0xffff0060($zero)		# acknowledge interrupt
	
	li	$t2, 10				
	sw	$t2, 0xffff0010($zero)		# set velocity
	
	li	$t7, -1
	
	lw	$t2, lockedblock($0)
	bne	$t2, $t7, leave_bonk
	
	
	lw	$t2, block($0)
	sw	$t2, lockedblock($0)
	
	
	la   $t5, clue
	sw   $t5, 0xffff007c($0)
	
	
leave_bonk:
	b 	interrupt_dispatch		# see if other interrupts are waiting

clue_interrupt:
  li  $a1, 1
  sw  $a1, 0xffff0068($0) # acknowledge
  sw  $a1, clue_avail($0)

  b   interrupt_dispatch

non_intrpt:				# was some non-interrupt
	li $v0, 4			
	la $a0, non_intrpt_str
	syscall				# print out an error message
	b done

done:
	la      $k0, save
	lw	$a0, 0($k0)		# Restore regsiters
	lw	$a1, 4($k0)		# 
	mfc0 	$k0 $14			# EPC
	.set noat
	move	$at $k1			# Restore $at
	.set at
	rfe				# Return from exception handler
	jr 	$k0
        nop
