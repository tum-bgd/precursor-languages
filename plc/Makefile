all:
	lex pl-compiler.l
	yacc pl-compiler.y
	gcc -o plc y.tab.c -ll -ly
clean:
	rm -f lex.yy.c
	rm -f y.tab.c
	rm -f plc
