RISC-V Processor Pipeline Simulator
Project Overview
This project is a detailed simulation of a RISC-V processor pipeline, demonstrating the execution of machine code instructions through a five-stage pipeline with and without data forwarding.
Project Structure
Key Files

processor_forwarding.hpp: Core processor simulation infrastructure

Base Processor class
Pipeline stage implementations (Fetch, Decode, Execute, Memory, Writeback)
Instruction decoding utilities
Register file and ALU implementations


noforwarding.hpp: No-forwarding processor implementation

Extends base Processor class
Implements execute stage without data forwarding
Handles pipeline stalls due to data hazards


forwarding.hpp: Forwarding processor implementation

Extends base Processor class
Implements data forwarding techniques
Reduces pipeline stalls


main_forwarding.cpp: Forwarding processor simulation entry point

Handles command-line argument parsing
Reads machine code from input files
Configures and runs forwarding processor simulation


main.cpp: Non-forwarding processor simulation entry point

Similar to main_forwarding.cpp
Runs processor simulation without forwarding


Makefile: Build configuration

Compile instructions for different processor variants
Manages build targets and compilation flags



Pipeline Stages
The simulator implements a classic five-stage RISC-V pipeline:

Instruction Fetch (IF):

Retrieves next instruction from memory
Increments program counter


Instruction Decode (ID):

Decodes instruction
Extracts register operands
Generates control signals


Execute (EX):

Performs ALU operations
Calculates memory addresses
Handles branch predictions


Memory (MEM):

Performs memory read/write operations
Handles load and store instructions


Write Back (WB):

Writes results back to register file
Completes instruction execution



Instruction Types Supported

R-Type (Arithmetic and Logical Operations)
I-Type (Immediate Instructions, Loads)
S-Type (Store Instructions)
B-Type (Branch Instructions)
J-Type (Jump Instructions)

Simulation Features
Processor Variants

No Forwarding Processor

Basic pipeline implementation
Stalls on data hazards
Demonstrates pipeline bubbles and performance limitations


Forwarding Processor

Implements data forwarding to minimize pipeline stalls
Handles complex instruction dependencies
Provides detailed cycle-by-cycle execution tracing



Hazard Handling

Stall mechanisms for data hazards
Basic forwarding logic to reduce pipeline stalls
Detailed hazard detection across pipeline stages

Visualization

Generates a pipeline diagram showing instruction progression
Prints detailed information for each cycle
Tracks instruction flow through different pipeline stages

Simulation Output

Detailed cycle-by-cycle processor state
Pipeline execution diagram
Register and memory state information

Components Overview
Register File

Manages 32 processor registers
Supports read and write operations
Implements x0 register as hardwired zero

Arithmetic Logic Unit (ALU)

Supports basic operations: ADD, SUB, AND, OR
Executes arithmetic and logical instructions
Core computational component of the processor

Author
Aditya Garg
Vatsal Malav