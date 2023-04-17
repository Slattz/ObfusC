# ObfusC - Implementation of a Compiler Plugin for C++ Obfuscation
ObfusC entails creating a LLVM Compiler Plugin (written in C++) with the main purpose of providing a seamless and easy to use C/C++ obfuscation for the user. 

The user is given the option to specify what functionality to obfuscate through the use of code attributes. 

ObfusC integrates directly into LLVM by working at the Intermediate Representation (IR) level of the compiler, which is used to translate high-level languages such as C and C++, to target architectures such as x86 and ARM and AArch64.

# Features

## Mixed Boolean Arithmetic

This involves the use of a combination of standard arithmetic operations, such as addition and subtraction, with logical operations, such as AND, NOT, and OR.

![Alt text](images/MBA.png?raw=true)

---

## Bogus Control Flow

This alters a function’s control flow by inserting conditional jumps that point either into the original basic code block or to a fake basic code block.

![Alt text](images/BCF.png?raw=true)

---

## Instruction Substitution

This simply replaces standard binary operators with functionally equivalent but more complicated sequences of instructions. 

![Alt text](images/iSub.png?raw=true)

---

## Control Flow Flattening

This involves the transformation of a program's control flow, the sequence of instructions that determines how a program executes, into a more complex and less transparent form. This is accomplished through techniques such as loop unrolling.

![Alt text](images/iSub.png?raw=true)

---

# Download
Latest Release: [Here](https://github.com/Slattz/ObfusC/releases/latest)

# Building on Linux

## Dependencies

* CMake 3.13.4+
* Ninja
* LLVM
* LLVM Dev Headers

## Building
```
git clone --recursive https://github.com/Slattz/ObfusC
cd ObfusC
```

Next:
```bash
mkdir build && cd build
cmake ../ -GNinja -DCMAKE_C_COMPILER:FILEPATH=/usr/bin/clang-15 -DCMAKE_CXX_COMPILER:FILEPATH=/usr/bin/clang++-15
```