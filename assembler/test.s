; this is a test source file
; for the vcpu assembler

jmp start

other:
set r3, 8
mul r4, r3, r3
jmp end

start:
set r0, 5
set r1, 10
add r2, r0, r1; this is another comment
mul r2, r2, r0
sub r2, r2, r1
mul r2, r2, r0
div r2, r2, r0
jmp other
end:
