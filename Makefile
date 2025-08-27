.PHONY: bm

bm:
	@bm ./phpmap
	@bm ./cmap
	@bm ./sparsemap
	@bm ./cppmap
	@bm ./cppmap2
	@bm ./swiftmap
	@bm ./densemap
	@bm ./rustmap
	@bm ./zigmap
	@bm ./nimmap
	@bm ./gomap
	@bm ./vmap
	@bm ./perlmap
	@bm ./pythonmap
	@bm ./ocamlmap
	@bm dotnet dotnetmap/bin/Release/net6.0/dotnetmap.dll
	@bm ./awkmap
	@bm ./luamap
	@bm java Javamap
	@bm ./bunmap
	@bm ./nodemap
	@bm ./rubymap
	@bm ./tclmap
	@bm ./juliamap
	@bm ./haskellmap
	@bm ./elixirmap
	#@bm Rscript rmap.R

all: c v zig go swift rust dotnet haskell nim dense sparse cpp cpp2 ocaml java

clean:
	rm -f cmap
	rm -f vmap
	rm -f zigmap
	rm -f gomap
	rm -f swiftmap
	rm -f rustmap
	rm -f haskellmap
	rm -f nimmap
	rm -f densemap
	rm -f sparsemap
	rm -f cppmap
	rm -f cpp2map
	rm -f ocamlmap
	cd dotnetmap && make clean
	#rm -f javamap

c:
	cd clib && make clean
	make -C clib
	gcc -g -O2 -o cmap cmap.c -lm -Lclib -lcvar -Iclib
v:
	v vmap.v
zig:
	zig build-exe zigmap.zig -OReleaseFast
	rm zigmap.o
go:
	go build gomap.go
swift:
	swiftc -O swiftmap.swift
rust:
	rustc -O rustmap.rs
java:
	javac Javamap.java
dotnet:
	make -C dotnetmap
haskell:
	ghc -O2 haskellmap.hs
	rm -f haskellmap.hi
	rm -f haskellmap.o
nim:
	nim c --verbosity:0 -d:release nimmap.nim
dense:
	g++ -std=c++11 densemap.cpp -o densemap -O2
sparse:
	g++ -std=c++11 sparsemap.cpp -o sparsemap -O2
cpp:
	g++ -std=c++11 cppmap.cpp -o cppmap -O2
cpp2:
	g++ -std=c++11 cppmap2.cpp -o cppmap2 -O2
ocaml:
	ocamlopt -O2 -o ocamlmap ocamlmap.ml
	rm -f ocamlmap.cm?
	rm -f ocamlmap.o
