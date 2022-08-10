# Kiwi Fuzzer

Fuzzing or fuzz testing is an automated software testing technique that involves providing invalid, unexpected, or random data as inputs to a computer program. The program is then monitored for exceptions such as crashes, failing built-in code assertions, or potential memory leaks.

## Mutators
Input text is mutated and fed to the mutators. As this is a tool meant for developers, you can script your own mutators and generate an input file to be fed to the program, if a user would like to use more advanced mutators to manipulate the text. We provide 4 kinds of mutators in the root of the project directory, however, any number of advanced mutators are accepted in a language that can generate an input file. 

##  Ponii

At the heart of our project is the ARM Emulator ponii, that makes our fuzzer stand out from any fuzzer out there. By using an ARM emulator, we are effectively able to view things such as value of a register during execution that are impossible, thus giving a programmer an impressive level of control over the entire build process.

## Project Structure
The top folder contains the barebone's of our Fuzzer. The meat of the project lies under the ponii which contains the ARM emulator we have developed.  The bin directory contains script for our dockerfile, while docs contains a tex file of our project proposal. 

## Build
For the fuzzer the build can be completed by:
	`make all` in the root of the project directory.
For ponii:
    `cd ponii/`
    `make all`
