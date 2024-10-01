#!/bin/bash


#
# Build a library for the web page
#
function scan_programs(){
    
for f in src/*.pl1; do 
    cat "$f" | jq -sR '{"'$(basename $f)'":.}'
done

}

function scan_worlds(){
    
for f in wrl/*.wrl; do 
    cat "$f" | jq -sR '{"'$(basename $f)'":.}'
done

}

function both(){
scan_programs | jq -s 'add | {"programs":.}'
scan_worlds | jq -s 'add | {"worlds":.}'
}


both | jq -s 'add'
