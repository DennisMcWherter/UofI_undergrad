## you get to write this one from scratch.
## we'll only be testing its behavior, not poking your code directly

.text
main:
  add   $t0, $0, 0x0a       # $t0 = 10 ; 11 blocks, 0-A

iterate_blocks:
  blt   $t0, $0, done
  sw    $t0, 0xffff0070($0)    # Which block to read
  b     turn_right

check_y:
  lw    $t1, 0xffff0074($0)    # Current y position
  lw    $t2, 0xffff0024($0)    # Spimbot current y
  sub   $t3, $t2, $t1          # $t3 = $t2 - $t1
  blt   $t3, $0, turn_up
  bgt   $t3, $0, turn_down

push_block:
  jal   stop
  add   $t1, $0, 180           
  sw    $t1, 0xffff0014($0)    # Turn left
  add   $t1, $0, 1
  sw    $t1, 0xffff0018($0)
  jal go

push:
  lw    $t1, 0xffff0070($0)    # Current x position
  lw    $t2, 0xffff0020($0)    # Spimbot current x
  sub   $t3, $t2, $t1          # $t3 = $t2 - $t1
  ble   $t3, $0, end_iterate
  j push

end_iterate:
  sub   $t0, $t0, 0x01         # $t0--
  j iterate_blocks

turn_right:
  jal   stop
  add   $t2, $0, 0x00          # $t2 = 0
  sw    $t2, 0xffff0014($0)    # Turn right
  or    $t2, $t2, 0x01         # $t2 |= 1
  sw    $t2, 0xffff0018($0)
  jal   go
  lw    $t1, 0xffff0070($0)    # Current x
  add   $t4, $t1, 15

move_right:
  lw    $t2, 0xffff0020($0)    # Spimbot current x
  sub   $t3, $t4, $t2          # $t3 = $t4 - $t1
  ble   $t3, $0, check_y
  j move_right

turn_up: 
  jal   stop
  add   $t4, $0, 90            # $t4 = 90
  sw    $t4, 0xffff0014($0)    # Turn up
  add   $t4, $0, 1
  sw    $t4, 0xffff0018($0)
  jal   go
  lw    $t1, 0xffff0074($0)    # Current y

move_up:
  lw    $t2, 0xffff0024($0)    # Spimbot current y
  sub   $t3, $t2, $t1          # $t3 = $t2 - $t1
  beq   $t3, $0, push_block
  bgt   $t3, $0, turn_down
  j move_up

turn_down:
  jal   stop
  add   $t4, $0, 270           # $t4 = 270
  sw    $t4, 0xffff0014($0)    # Turn down
  add   $t4, $0, 1
  sw    $t4, 0xffff0018($0)
  jal   go
  lw    $t1, 0xffff0074($0)    # Current y

move_down:
  lw    $t2, 0xffff0024($0)    # Spimbot current y
  sub   $t3, $t2, $t1          # $t3 = $t2 - $t1
  beq   $t3, $0, push_block
  blt   $t3, $0, turn_up
  j move_down

stop:
  sub   $sp, $sp, 4
  sw    $t0, 0($sp)
  add   $t0, $0, 0x00
  sw    $t0, 0xffff0010($0)
  lw    $t0, 0($sp)
  add   $sp, $sp, 4
  jr  $ra

go:
  sub   $sp, $sp, 4
  sw    $t0, 0($sp)
  add   $t0, $0, 0x0a
  sw    $t0, 0xffff0010($0)
  lw    $t0, 0($sp)
  add   $sp, $sp, 4
  jr  $ra

done:
  jr  $ra

