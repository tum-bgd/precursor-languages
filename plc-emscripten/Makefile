all:
	$(MAKE) -C ../plc
	emcc testapi.c ../plc/lex.yy.c ../plc/pl-compiler.tab.c -o testapi.html -sEXPORTED_FUNCTIONS=_invoke_compile -sEXPORTED_RUNTIME_METHODS=ccall,cwrap
	cp testapi.js testapi.wasm testapi.html test.html /var/www/html
	cp gui/precursorlanguages.html gui/*.css gui/*.js /var/www/html
	cp gui/gfx /var/www/html -R

clean:
	rm -f testapi.js testapi.wasm testapi.html
