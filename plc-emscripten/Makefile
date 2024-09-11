all:
	emcc testapi.c -o testapi.html -sEXPORTED_FUNCTIONS=_get_it,_twice -sEXPORTED_RUNTIME_METHODS=ccall,cwrap

clean:
	rm -f testapi.js testapi.wasm testapi.html
