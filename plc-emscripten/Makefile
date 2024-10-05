all:
	$(MAKE) -C ../plc
	emcc plc-emscripten.c ../plc/lex.yy.c ../plc/pl-compiler.tab.c -o plc.html -sEXPORTED_FUNCTIONS=_invoke_compile -sEXPORTED_RUNTIME_METHODS=ccall,cwrap
	cp plc.js plc.wasm plc.html  /var/www/html
	cp gui/precursorlanguages.html gui/*.css gui/*.js /var/www/html
	cp gui/gfx /var/www/html -R

html: 
	cp plc.js plc.wasm plc.html  /var/www/html
	cp gui/precursorlanguages.html gui/*.css gui/*.js /var/www/html
	cp gui/gfx /var/www/html -R

dist:
	mkdir dist
	cp plc.js plc.wasm plc.html  dist
	cp gui/precursorlanguages.html gui/*.css gui/*.js dist
	cp gui/gfx dist -R
	tar -czf plc.tgz dist



clean:
	rm -f plc.js plc.wasm plc.html
