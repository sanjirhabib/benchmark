# Benchmark

Benchmark for C vs 20 other languages.

#### What does it measure?

- Memory manager performance
- String operation
- Hashmap speed.

These three things in one go. Critical in web application development.

#### The Task
Basically a 4 line task.

- Create a hashmap.
- Add 500,000 items 10 times over.
- Print 2 values to indicate the task was successful.

Score gives equal weight on speed and memory.

[score] = [run time seceonds] x [memory in MB] — The lower the better.


#### What it says :

Initially wrote these tests in 2010, when these were generally assumed :

- lua-jit is the fastest scripting language around. Almost as fast as C.
- Compiled languages like Swift and Go are x10 faster than interpretated languages, like PHP or Ruby
- PHP is the slowest of them all.

None of the above are true anymore.

### How do I run it?

```
git clone --recursive --depth=1 https://github.com/sanjirhabib/benchmark
make clean
make all
make

```
The last make command will start benchmark test.

#### Show me the numbers!

Here are the numbers after running on my old laptop. Ordered by score on the last column, the lower the better

```
./phpmap        Time:  0.65   Memory:   70 mb    Score:  45
./cmap          Time:  1.14   Memory:   55 mb    Score:  63
./sparsemap     Time:  1.82   Memory:   36 mb    Score:  66
./cppmap        Time:  1.28   Memory:   56 mb    Score:  72
./cppmap2       Time:  1.71   Memory:   54 mb    Score:  93
./swiftmap      Time:  1.69   Memory:   57 mb    Score:  96
./densemap      Time:  1.26   Memory:   99 mb    Score:  125
./rustmap       Time:  1.75   Memory:  103 mb    Score:  181
./zigmap        Time:  1.34   Memory:  159 mb    Score:  213
./nimmap        Time:  2.83   Memory:   87 mb    Score:  248
./gomap         Time:  2.37   Memory:  105 mb    Score:  250
./vmap          Time:  2.95   Memory:   87 mb    Score:  258
./perlmap       Time:  2.06   Memory:  132 mb    Score:  272
./pythonmap     Time:  3.72   Memory:   85 mb    Score:  317
./ocamlmap      Time:  3.30   Memory:  111 mb    Score:  367
dotnet          Time:  3.40   Memory:  159 mb    Score:  541
./awkmap        Time:  2.92   Memory:  166 mb    Score:  487
./luamap        Time:  4.50   Memory:  192 mb    Score:  866
java            Time:  2.27   Memory:  420 mb    Score:  953
./bunmap        Time:  3.84   Memory:  335 mb    Score:  1288
./nodemap       Time:  5.01   Memory:  294 mb    Score:  1473
./rubymap       Time: 12.13   Memory:  124 mb    Score:  1508
./tclmap        Time:  9.74   Memory:  206 mb    Score:  2007
./juliamap      Time:  4.49   Memory:  476 mb    Score:  2138
./haskellmap    Time: 14.40   Memory:  443 mb    Score:  6391
./elixirmap     Time:  9.61   Memory:  746 mb    Score:  7172
```

#### Summary
- My minimal hashmap is not slower than C++'s map, undered_map, densemap, or sparsemap.
- glibc's malloc() free() is not slower than manual memory management.
- String manipulation is the main bottleneck.


#### The surprises
Even though I can beat other languages, I can't beat PHP. No idea why or how they are doing it.

I tried other things like caching the hash value like PHP, but nothing helped.

#### What other optimizations did you put in your C code?

There's a list of optimizations that I DIDN'T perform :

- Strings don't cache hash. Like how it's done in PHP
- No small string optimization like in C++ 
- No SIMD AVX from my code, other than what the compiter does internally.
- No pre-memory allocation, or custom memeory manager.
- Memory allocation in increased by 1 every time an element is added. No buffer.
- No excessive use of C macros for inlining. Depended on a lot of one-line-functions instead.
- Values are all boxed. Not pointer passing around.
- A runtime type-inferance is at work making the hashmap flexible for any size of data. Unlike compile time code generation in C++.
- Released all memory before exit. Clean valgrind memcheck.

So basically just a regular C program, without any specific optimization.


#### How do I build the programs?
```
make clean        # to clear revious builds
make all          # to build programs one by one, you need to have the build invironment installed
make              # to run the benchmark test
```

or, for individual benchmark

```
make cpp             # to build the cpp program
bm cppmap            # benchmark cpp
                     # `bm` is a bash file
```

Example : with rust
```
$ make rust
rustc -O rustmap.rs
$ bm rustmap
500000
val.4510000
rustmap  	Time: 2.92 	Memory: 103 mb	Score: 301
$
```

#### Where's the C code?

The benchmark code is in cmap.c

But the library is in clib/ directory


#### I want to improve on some code

Rules :

- Don't pre allocate memory.
- Don't optimize for the benchmark.
- Use strring concat or format() which ever is faster.
- Use unordered map if that's available.

I personally prefer unordered map that stores and enumerate the items in insertion order.
Which is wny I had to write my own map data structure in C.
This is not standard and none of the c++ map library support it.
JavaScript/node didn't support it either in its initial release. But later changed their behaviour on popular demand. Node mow enumerate on insertion order.
