# benchmark
Benchmark Numbers
Performed 

### "What did you measure?"

Basically gc performance. Plus a little bit of hash table and string operation.
Every program bellow creates a hash map sized 500,000 and populates it x10 time to check GC performance and total memory consumption.

#### What it says :

Previously everyone thought lua-jit is the fastest scripting language around. Almost as fast as C. That's hardly the case now. Node the blazing fast lang is now the slowest and too much memory hog.

Compiled languages like Swift and Go aren't x10 faster than scripting languages, as it had been previously.

PHP isn't slow any more. And python, the de facto language for AI/ML, isn't faster than the others.

Follows the numbers for a simple run

```
$ make | grep Time
./gomap: Time: 2.16, Memory: 147 mb
./phpmap: Time: 1.27, Memory: 83 mb
./swiftmap: Time: 2.45, Memory: 55 mb
./nodemap: Time: 6.57, Memory: 374 mb
./pythonmap: Time: 3.80, Memory: 90 mb
./luamap: Time: 5.27, Memory: 144 mb
./rustmap: Time: 6.11, Memory: 103 mb
./rubymap: Time: 8.53, Memory: 132 mb
java: Time: 4.09, Memory: 354 mb
dotnet: Time: 3.32, Memory: 161 mb
```
