	.data
bonked:	.word		0
clue_avail: .word 0
clue_req: .word -1
clue: .space 4096
sc: .asciiz "Clue\n"

	.text
main:	
  # Enable bonk, clue interrupts
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

  # Check clue stuff
  lw  $t5, clue_avail($0)
  beq $t5, 0, get_clue
  mov $s0, $a0
  lw  $a0, clue($0)
  jal count_nodes
  mov $a0, $s0
  sw  $0, clue_req($0)

get_clue:
  lw  $t5, clue_req($0)
  bne $t5, -1, skip_clue
  la  $t5, clue
  sw  $t5, 0xffff007c($0)
  sw  $t0, clue_req($0)

skip_clue:
	add 	$a0, $a0, 15		# try to over shoot it

# GET ALIGNED IN +X DIRECTION	
	sw	$zero, 0xffff0014($zero) # turn to drive in +x
	li	$t0, 1
	sw	$t0, 0xffff0018($zero)


x_pos:
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

#try count
## unsigned LSB_set(unsigned in); 
LSB_set:
ori	$v0, $a0, 1		# out = in | 0x00000001
jr	$ra			# return out

## unsigned LSB_clear(unsigned in); 
LSB_clear:
li	$t0, -2			# -2 = 0xfffffffe
and	$v0, $a0, $t0		# out = in & 0xfffffffe
jr	$ra			# return out

## unsigned bit_reverse(unsigned in);
bit_reverse:
move	$t0, $a0		# reversed = in
li	$t1, 31			# num_bits = 31
srl	$a0, $a0, 1		# for(in >>= 1;;) {

reverse_loop:
beqz	$t1, reverse_loop_done	# for(;num_bits;)
sll	$t0, $t0, 1		# reversed <<= 1
andi	$t2, $a0, 1		# in & 0x00000001
or	$t0, $t0, $t2		# reversed |= in & 0x00000001
addi	$t1, $t1, -1		# num_bits--
srl	$a0, $a0, 1		# for(;;in >>= 1)
j	reverse_loop		# }

reverse_loop_done:
move	$v0, $t0		
jr	$ra			# return reversed

## unsigned count_nodes(node_t *node);
count_nodes:
addi	$sp, $sp, -16
sw	$a0, 0($sp)			# sp = node
sw	$s0, 4($sp)			# sp + 4 = saved s0
sw	$ra, 8($sp)			# sp + 8 = saved ra
sw  $t0, 12($sp)

lw	$a0, 0($a0)			# node->p[0]
jal	LSB_set				# LSB_set(node->p[0])
lw	$a0, 0($sp)			# node
lw	$t0, 0($a0)			# node->p[0]
bne	$t0, $v0, count_nodes_endif	# if (. == .)
li	$v0, 0				# return 0
j	count_nodes_end

count_nodes_endif:
sw	$v0, 0($a0)			# node->p[0] = LSB_set(node->p[0])

lw	$a0, 0($a0)			# node->p[0]
jal	LSB_clear			# LSB_clear(node->p[0])

move	$a0, $v0
jal	count_nodes
move	$s0, $v0
lw	$a0, 0($sp)

lw	$a0, 4($a0)			# node->p[1]
jal	LSB_clear			# LSB_clear(node->p[0])

move	$a0, $v0
jal	count_nodes

addi	$v0, $v0, 1
add	$v0, $v0, $s0

count_nodes_end:
lw	$s0, 4($sp)
lw	$ra, 8($sp)
lw

lw  $t0, clue_req
sw  $t0, 0xffff0070($0)
sw   $v0, 0xffff0090($0) # Unlock block
li  $t0, -1
sw  $t0, clue_req

lw  $t0, 12($sp)
addi	$sp, $sp, 16
jr	$ra


.data			# interrupt handler data (separated just for readability)
save:	     .word 0
	     .word 0
non_intrpt_str:   .asciiz "Non-interrupt exception\n"
unhandled_str:    .asciiz "Unhandled interrupt type\n"

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

  and   $a0, $k0, 0x4000  # any clue interrupts?
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

	b 	interrupt_dispatch		# see if other interrupts are waiting

clue_interrupt:
  li  $a1, 1
  sw  $a1, clue_avail($0)
  sw  $a1, 0xffff0068($0)

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
