all:
	$(MAKE) -C ../plc
	emcc plc-emscripten.c ../plc/lex.yy.c ../plc/pl-compiler.tab.c -o plc.html -sEXPORTED_FUNCTIONS=_invoke_compile -sEXPORTED_RUNTIME_METHODS=ccall,cwrap
	cp plc.js plc.wasm plc.html  /var/www/html
	cp gui/precursorlanguages.html gui/*.css gui/*.js /var/www/html
	cp gui/gfx /var/www/html -R

clean:
	rm -f plc.js plc.wasm plc.html
