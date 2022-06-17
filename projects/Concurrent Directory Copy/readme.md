# Concurrent Directory Copy

**Status** » Finished  

**Language** » C++  

**Date** » 21 October 2020  

*This program was made as an university assignment for a 4th year subject, Concurrent Programming.*  

---

The aim of this project was to gain experience on concurrency building a program that was able to mimic the `cp` command. It performs `cp`'s basic functionality, but is also able to dictate the thread count of the workload and select between two types of copy.

### The differenty copy types

- Default. Performs individual `cp` call for each file.  
- Binary Copy. Copies the files using `C++` file stream.  

### Threads usage

- Default. Uses 4 threads.
- Using `-s` argument. Uses only one thread. synchronous copy.
- Using `-c` argument. Uses all available threads


### Build

The choose between copy types is made at the compile time.
- Default `g++ -O3 -o cDirCopy cDirCopy.cpp -lpthread`
- Binary Copy  `g++ -O3 -DbinCopy -o cDirCopy cDirCopy.cpp -lpthread`

### Performance 

The following testing was performed on an old computer. Relevant specs: 
> CPU: AMD FX-6300  
> RAM: 16GB DDR3 1600MHz  
> Storage: SSD Kingston A400 120GB  
> OS: Ubuntu Server 18.04.5 LTS (no GUI)  

The test was copy a payload formed by a some pictures on nested folders. By sequential I mean it was used the `-s` argument that implies the usage of an only thread. Concurrent, `-c`, meant the usage of all 6 threads of the CPU on that system.

||Sequential|Concurrent|
|-|-|-|
|Default|0.2670490s|0.0924644s|
|Binary|0.0337678s|0.0372321s|


### Conclusion

This code is just an example of pthread usage and file handling. It was never meant to solve a problem. The binary copy code I found on *StackOverflow* is pretty fast. I should test it again with a bigger payload.