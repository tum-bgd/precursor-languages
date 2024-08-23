all:
	flex pl-compiler.l
	bison  -d pl-compiler.y
	gcc -g -o plc lex.yy.c pl-compiler.tab.c -ll -ly
clean:
	rm -f *.o
	rm -f lex.yy.c
	rm -f y.tab.c
	rm -f plc
