pico.exe: pico.o getline.o parsing.o builtin_CMD.o
	gcc pico.o getline.o parsing.o builtin_CMD.o -o pico.exe
pico.o: pico.c
	gcc -c pico.c -o pico.o
getline.o: getline.c
	gcc -c getline.c -o getline.o
parsing.o: parsing.c
	gcc -c parsing.c -o parsing.o
builtin_CMD.o: builtin_CMD.c
	gcc -c builtin_CMD.c -o builtin_CMD.o
clean:
	-rm pico.o getline.o parsing.o builtin_CMD.o pico.exe
