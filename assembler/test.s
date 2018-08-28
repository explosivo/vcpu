; this is a test source file
; for the vcpu assembler

jmp start

testImmd:
addi r3, r0, 95;100
subi r3, r3, 20;80
divi r3, r3, 8;10
muli r3, r3, 10;100
jmp end

testMove:
set r3, 8
mul r4, r3, r3
mov r5, r4
jmp testImmd

start:
set r0, 5
set r1, 10
add r2, r0, r1; this is another comment
mul r2, r2, r0
sub r2, r2, r1
mul r2, r2, r0
div r2, r2, r0
jmp testMove
end:
