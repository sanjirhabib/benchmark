# Benchmark

Benchmark numbers measured for a bunch of porgramming language implementations.

#### "So, what did you measure?"

- GC performance
- string operation
- hashmap speed.

These three things in one go. Critical in web application development.

#### Process
This basically is 4 line code.
- Create a hashmap.
- Add 500,000 items 10 times over.
- Measure speed and memory consumption.

Score is based on giving equal weight on both speed and memory consuption.

score = run time seceonds x memory in MB. The lower the better.



#### What it says :

I initially wrote these tests in the 2010s, when it was thought.

- lua-jit is the fastest scripting language around. Almost as fast as C.
- Compiled languages like Swift and Go are x10 faster than scripting languages, like PHP or Ruby
- PHP is the slowest of them all.

None of the above are true anymore.

#### How do I ron it?

```
git clone --recursive --depth=1 https://github.com/sanjirhabib/benchmark
make clean
make all
make

```
The last make command will run the benchmark test.

#### Show me the numbers!

Here's the numbers from my old computer. Ordered by score, lowest (best) to highest (worse)

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
- My amature coded hashmap is not slower than C++'s map, undered_map, densemap, or sparsemap.
- glibc's malloc() free() is not slower than manual memory management whatever I can write.
- String manipulation with pointer + length is speedy enough.


#### The surprises
I can beat the other languages, but not PHP. No idea why or how they are doing it.

I tried caching the hash value like PHP, but that didn't help either.

#### What other optimizations did you put in your C coee?
None that I can tell you of.

- Strings don't cache the hash. Like how it's done in PHP
- No small string optimization like in C++ 
- No SIMD AVX from my code, other than what the compiter does internally.
- No pre-memory allocation, or custom memeory manager.
- Memory is allocated by 1 every time an element is added. No buffer.
- No excessive use of C macros for inlining. Using one line functions rather than macros.
- Everything is boxed. No excessive pointer passing.
- A runtime type inferance is at work so that the hashmap is flexible for all types of data. Not only strings or int.

So basically just a regular C program, without optimization.
