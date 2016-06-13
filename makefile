#proj 3 makefile

proj3: proj3.c
	gcc -o proj3 proj3.c -pthread

clean:
	rm *.o proj3

tar:
	tar -cf project3.tar proj3.c makefile
