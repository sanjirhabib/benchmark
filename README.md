# Benchmark

A benchmark comparing C against 20 other programming languages.

#### What Does It Measure?

- Memory manager performance
- String operations
- Hashmap speed

These three aspects are evaluated simultaneously, as they are critical in web application development

#### The Task

A simple 4-line task:
- Create a hashmap.
- Add 500,000 items 10 times.
- Print 2 values to verify task completion.

The score is calculated as:

```[score] = [run time in seconds] × [memory in MB]```

Lower scores are better, giving equal weight to speed and memory usage.

#### What It Reveals
These tests were initially written in 2010, when the following assumptions were common:
- LuaJIT was the fastest scripting language, nearly as fast as C.
- Compiled languages like Swift and Go were 10x faster than interpreted languages like PHP or Ruby.
- PHP was the slowest of all.

None of these assumptions hold true today.

### How do I run it?

```
git clone --recursive --depth=1 https://github.com/sanjirhabib/benchmark
make clean
make all
make

```
The last make command will start benchmark test.

#### Show me the numbers!

Results from running the benchmark on my old laptop, ordered by score (lower is better):

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

- My minimal hashmap is not slower than C++'s map, unordered_map, densemap, or sparsemap.
- glibc's malloc() and free() are not slower than manual memory management.
- String manipulation is the primary bottleneck.




#### The surprises
Despite outperforming many languages, my implementation couldn't beat PHP. The reason remains unclear, even after attempting optimizations like caching hash values as PHP does.

#### What Optimizations Were Used in the C Code?
The C code deliberately avoids several optimizations:
- No caching of string hashes (unlike PHP).
- No small string optimization (unlike C++).
- No SIMD AVX instructions beyond what the compiler applies.
- No pre-allocation of memory or custom memory manager.
- Memory allocation increases by 1 for each element added, with no buffer.
- No excessive use of C macros for inlining; relies on one-line functions instead.
- Values are boxed, not passed as pointers.
- Runtime type inference enables a flexible hashmap for any data size, unlike C++'s compile-time code generation.
- All memory is released before exit, ensuring a clean Valgrind memcheck.

In essence, this is a straightforward C program without specialized optimizations



#### How do I build the programs?
```
make clean        # Clear previous builds
make all          # Build all programs (requires the build environment to be installed)
make              # Run the benchmark test
```

To build and benchmark an individual program, e.g., C++:


```
make cpp             # to build the cpp program
bm cppmap            # benchmark cpp
                     # `bm` is a bash file
```

Example with Rust:

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

The benchmark code is in ```cmap.c```

The supporting library is in the ```clib/``` directory.


#### How Can I Improve the Code?
Rules for contributions:Do not pre-allocate memory.
- Do not hint to the hashmap about upcoming operations.
- Do not optimize specifically for the benchmark.
- Run on a single processor; do not spawn threads.
- Use string concatenation or format(), whichever is faster.
- Use an unordered map if available.

I prefer an unordered map that stores and enumerates items in insertion order, which is why I wrote my own hashmap. Note that this behavior is non-standard and not supported by C++ map libraries. JavaScript/Node.js initially lacked this feature but later adopted insertion-order enumeration after developer feedback.

#### What's the Design of the Hashmap?

Technically, it's not a traditional hashmap. It uses:
- Two separate vectors for keys and values.
- A third vector to store item indices linked with hashes.
- A load factor of approximately 1.


