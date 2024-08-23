cmake_minimum_required(VERSION 3.20)
project(PrecursorLanguage)

add_executable(plc lex.yy.c pl-compiler.tab.c)
target_include_directories(plc PUBLIC .)
