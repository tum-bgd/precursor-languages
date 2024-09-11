all:
	emcc testapi.c ../plc/lex.yy.c ../plc/pl-compiler.tab.c -o testapi.html -sEXPORTED_FUNCTIONS=_get_it,_twice -sEXPORTED_RUNTIME_METHODS=ccall,cwrap

clean:
	rm -f testapi.js testapi.wasm testapi.html
