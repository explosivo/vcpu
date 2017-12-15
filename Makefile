default:
	gcc -c *.c
	gcc *.o -o vcpu
	
clean:
	rm -rf *.o vcpu