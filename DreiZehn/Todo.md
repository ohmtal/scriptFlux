# DreiZehn Todo

## 0.2

- [X] fix Identifier and Keywords can be delimited by a '.' or ':'
- [X] neg numbers and fix '.' somewhere outside a number  
- [X] fix Garbage collection to current scope, using a current scope global env 
It needs to be done in "end" 
- [X] add preprocessor with ';' break to lines
- [X] added multiline statement parser and fixed issues with undetected syntax errors - endless loop
- [X] while or similar while(true) loop

## 0.3 

- add some basic SDL3 bindings ;) Test if i can use my ElfScript macros - but i 
don't have to PoD Types here at the moment. 
- write the starfield demo in DreiZehn
- local / global var is sometime mixed - since we fetch a global it is not found on local scope
but it's a bit unclear when its global and when its local

# future ideas

- change printf errorf to a overwritable class or add a handler 
- make header only 
- move to it's own project
- change to bytecode compiler - really ? : maybe

- add object/structs
    - link propeties
    - add method calls
    
