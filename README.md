# htop 

This project practices object oriented programming in C++, using the [htop](https://htop.dev/) process viewer for a Linux file system as the project goal.
My contributions are marked under TODO's.


## Classes

- Interestingly, LinuxParser was implemented as a namespace. There's no object level data/need for multiple instantiations, so it makes sense
to give it some more implicit structure that way. The constant string [paths](./include/linux_parser.h) are very interesting to read, as they are how system data is stored in memory on a Linux OS.
- The project is basically reading from text files with process and memory utilization information stored and organized by the Linux OS.
There are decisions such as whether to read once then cache the result in the object instance or to have a pass-through function
where each time it is called the file is read again. There is an instance of an overloaded less than operator to 
compare one process to the process with the method called, allowing the sort function to sort processes by X metric.
- I wrote a bit on why the project authors made a parsing decision [here](./src/linux_parser.cpp#L22).
- There were some bonus extensions that I implemented.
- The authors originally used this attribute specifier to suppress compiler warnings about unused methods: `[[maybe_unused]](since C++17)`,
which was interesting to see.
Example usage: `long LinuxParser::UpTime(int pid[[maybe_unused]]) { return 0; }`

## Resources

I quite liked these links shared by Udacity for understanding the files:
- [/proc/meminfo file](https://www.thegeekdiary.com/understanding-proc-meminfo-file-analyzing-memory-utilization-in-linux/)
- how the htop author calculated the memory meter numbers
[Stack Overflow post](https://stackoverflow.com/questions/41224738/how-to-calculate-system-memory-usage-from-proc-meminfo-like-htop/41251290#41251290)

## ncurses
[ncurses](https://www.gnu.org/software/ncurses/) is a library that facilitates text-based graphical output in the terminal. This project relies on ncurses for display output.

The original htop uses ncurses too.

Install ncurses within your own Linux environment using `sudo apt install libncurses5-dev libncursesw5-dev`.

## Make
This project uses [Make](https://www.gnu.org/software/make/). The Makefile has four targets:
* `build` compiles the source code and generates an executable
* `format` applies [ClangFormat](https://clang.llvm.org/docs/ClangFormat.html) to style the source code
* `debug` compiles the source code and generates an executable, including debugging symbols
* `clean` deletes the `build/` directory, including all of the build artifacts

If you look inside the Makefile, it calls CMake then calls Make again. The authors mentioned they
wrote it using CMake in case the student wants to extend this to a Windows filesystem.

## How to Run

1. `make build` to build the project.

3. `./build/monitor` to run the generated executable.

