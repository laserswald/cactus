# 🌵 cactus

_a scheme implementation with spines_

## design goals

Cactus has taken a lot of inspiration from [Picrin](http://github.com/picrin-scheme/picrin) in that it tries to be a minimal and readable Scheme R7RS implementation. However, Cactus attempts to use the data structure libraries that are commonly packaged in the BSD family of OSes. Personally, I've found those libraries to be pretty elegant, so I've scraped together a few of them in one place and added one more that I think needed to be added.

- tree.h : Self-balancing binary trees, cribbed directly from the BSDs.
- queue.h : Linked lists, cribbed from the BSDs as well.
- array.h : Dynamically allocated arrays, cribbed from Nicholas Marriott
- table.h : Internal hash tables, made in house by yours truly.

Other than that, I want this to be clean and easy to read. It's as much of an art project as it is a programming project. 

## installation

First of all, don't use this. It'll prick you often. 

Compilation is done with djb redo, a minimal implementation being built in:

    ./do

You can configure what built-in functions you want in cactus by editing `config.h` and recompiling.

## what is actually supported 

see `config.h`

## todo

- 
