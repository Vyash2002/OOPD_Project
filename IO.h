#ifndef IO_H
#define IO_H

#include "basicIO.h"

// We will use the global `io` declared in basicIO.h
// extern basicIO io;  // already declared in basicIO.h

class IO {
public:
    IO() {}

    // Read an integer with a prompt
    int readInt(const char* prompt) {
        io.outputstring(prompt);
        return io.inputint();
    }

    // Print text without newline
    void print(const char* text) {
        io.outputstring(text);
    }

    // Print text followed by newline
    void println(const char* text) {
        io.outputstring(text);
        io.terminate();      // prints '\n'
    }

    // Print "label" + integer + newline
    void printInt(const char* label, int value) {
        io.outputstring(label);
        io.outputint(value);
        io.terminate();      // newline
    }

    // If you want just an int + newline (no label)
    void printlnInt(int value) {
        io.outputint(value);
        io.terminate();
    }
};

#endif // IO_H
