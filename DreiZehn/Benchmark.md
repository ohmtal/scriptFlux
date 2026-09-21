# DreiZehn-Script Benchmark 

I used my Benchmark Script from ElfScript:

- Lua 5.5.1: 1.243u 0.002s 0:01.25 99.2%     0+0k 0+0io 0pf+0w
- Elfscript 0.7c: 1.354u 0.002s 0:01.36 99.2%     0+0k 0+0io 0pf+0w
- python 3: 15.768u 0.005s 0:15.83 99.5%    0+0k 0+0io 0pf+0w. 
- 🐢 Dreizehn 0.1: 26.907u 0.006s 0:27.00 99.6%    0+0k 0+0io 0pf+0w

Not a rocket, but it also does not have a ByteCode Compiler - It runs
the code line by line.

The main application should be to run your own commands as debug helpers 
with basic scripting functions.


## Script on Version 0.1:

```
# Benchmark-Test for DreiZehn Engine
JLOOPS = 25
ILOOPS = 1000000

localX = 0

for j 0 (JLOOPS - 1)
    for i 0 (ILOOPS - 1)
        localX = localX + 1
    end
    print (concat "SUM (++) IS: " localX)

    for i 0 (ILOOPS - 1)
        localX = localX - 1
    end
    print (concat "SUM (--) IS: " localX)

    localX = 66
    print (concat "set Sum to 66 == " (localX * 1))

    for i 0 (ILOOPS - 1)
        localX = localX * (i + 1)
        localX = localX / (i + 1)
    end
    print (concat "SUM (*/ %i+1) IS: " localX)

    # TEST comment
    limit = (ILOOPS / 2) - 1
    ran = 0.0
    for i 0 limit
        ran = random * i
        localX = localX - ran
        localX = localX + ran
    end
    print (concat "last ran" ran)
    print (concat "SUM (rand +-) IS: " localX)
end

localX = localX - 33
print "---------------------"
print "---------------------"
print (concat "Final sum should be 33 == " localX)
print "---------------------"
print "---------------------"
```
