	.data
bonked: .word 0
	
  .text
main:
	li	$t0, 10				
	sw	$t0, 0xffff0010($zero)		# set velocity

  # Enable interrupts
  li  $t2, 0x1001
  mtc0  $t2, $12

	li	$t3, 0

m_loop:	
 	li	$t0, 10
 	sub	$t0, $t0, $t3
	sw	$t0, 0xffff0070($zero)
	lw	$a0, 0xffff0070($zero)	# block x and y location
	lw	$a1, 0xffff0074($zero)

	blt	$a0, 140, next		# already on my side
	bgt	$a0, 280, next 		# not reachable
  lw  $t4, bonked
  bne $t4, $0, next

	add 	$a0, $a0, 15		# try to over shoot it

# GET ALIGNED IN +X DIRECTION	
	sw	$zero, 0xffff0014($zero) # turn to drive in +x
	li	$t0, 1
	sw	$t0, 0xffff0018($zero)

x_pos:
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
  lw  $t4, bonked
  bne $t4, $0, next
	lw	$t2, 0xffff0020($zero)	# get my X position
	bgt	$t2, 140, still_pushing	# keep going until I pass block

next:
  sw  $0, bonked
	add	$t3, $t3, 1
	blt	$t3, 11, m_loop		# if block_num < 11
	li	$t3, 0			# otherwise start at block 0 again
	j	m_loop

	##  infinite outer loop

.kdata
chunkIH:  .space 8
non_interrupt: .asciiz "Non-interrupt exception caught"
unhandled_interrupt: .asciiz "Unhandled interrupt exception"

.ktext 0x80000080
interrupt_handler:
  .set noat
  move  $k1, $at      # Save $at register
  .set at
  la    $k0, chunkIH  # Load address to the register we need
  sw    $a0, 0($k0)
  sw    $v0, 4($k0)

  mfc0  $k0, $13      # Cause register
  srl   $k0, $k0, 2
  and   $k0, $k0, 0xf
  bne   $k0, $0, exception_handler

interrupt_dispatch:
  mfc0  $k0, $13      # Cause register
  beq   $k0, $0, done # Everything handled

  and   $a0, $k0, 0x1000 # Check for bonk interrupt
  bne   $a0, $0, bonk_interrupt

  li    $v0, 4
  la    $a0, unhandled_interrupt
  syscall

  j done # Unhandled interrupts just finish

bonk_interrupt:
  li    $a0, 1
  sw    $a0, bonked           # We interrupted

  # Turn spimbot around!
  sw    $a0, 0xffff0018($0)
  li    $a0, -90
  sw    $a0, 0xffff0014($0)
  li    $a0, 10
  sw    $a0, 0xffff0010($0)


  sw    $0, 0xffff0060($0)    # Acknowledge interrupt

  j interrupt_dispatch

exception_handler:
  li    $v0, 4
  la    $a0, non_interrupt
  syscall
  j done

done:
  la    $k0, chunkIH
  lw    $v0, 4($k0)
  lw    $a0, 0($k0)
  mfc0  $k0, $14

  .set noat
  move  $at, $k1
  .set at

  rfe
  jr  $k0
  nop

