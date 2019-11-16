Lab - Logger Library
====================

Implement a Logger library in C that will be used for printing the following types of messages.
- `INFO` General information.
- `WARN` Warnings.
- `ERROR` Errors.
- `PANIC` Panics. Non recoverable issues with core dump.

This `logger` library will be used instead of `printf` function calls in all new labs and challenges that we'll be doing in the course.
Below you can see the prototype for each function.

```
int infof(const char *format, ...)
int warnf(const char *format, ...)
int errorf(const char *format, ...)
int panicf(const char *format, ...)
```

General instructions
--------------------
- Use the `logger.c` file for your implementation.
- Update `README.md` with the proper steps for building and running your code.
- Use the `testLogger.c` file for testing all your logger functions.
- Use different font color for each of the log types. For colors references go to: https://www.linuxjournal.com/article/8603
- Don't forget to handle errors properly.
- Coding best practices implementation will be also considered.

How to submit your work
=======================
```
GITHUB_USER=<your_github_user>  make submit
```
More details at: [Classify API](../../classify.md)


## How to build and run
To build first compile the Logger using the flag -c
```
gcc -c logger.c -o logger
```
Then your program
```
gcc -c prog.c -o prog
```
Then compile them together under a new name which will be the final executable
```
gcc prog logger -o main
```
To use it inside the code you get 4 functions of logging and you can decide the
level from which to print with the function setLevel(int).

There are the following logging levels 0=INFO, 1=WARNING, 2=ERROR, 3=PANIC

The level 0 includes all of them, level 1 includes WARNING, ERROR and PANIC,
the same pattern with the next levels.
