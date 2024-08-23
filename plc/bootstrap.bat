@echo off
echo Generating Bootstrap
echo Flexer
winbison\win_flex.exe pl-compiler.l 
echo The YACC or Bison
winbison\win_bison.exe pl-compiler.y
echo Now, you can:
echo mkdir build // cd build // cmake .. // cmake --build --target=Release .
echo /* For Windows, we need an empty unistd.h*/ > unistd.h
