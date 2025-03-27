### README.md

# RISC-V Pipeline Simulator with Forwarding and No-Forwarding Implementations

This project implements a RISC-V pipeline simulator with two variants: a **No-Forwarding Processor** (`NoForwardingProcessor`) and a **Forwarding Processor** (`ForwardingProcessor`). The simulator models a 5-stage pipeline (IF, ID, EX, MEM, WB) and supports a subset of RISC-V instructions, including arithmetic (e.g., `ADD`, `SUB`, `AND`, `OR`), immediate operations (e.g., `ADDI`, `ANDI`, `ORI`), load/store (`LB`, `SW`), branches (`BEQ`, `BNE`, `BLT`, `BGE`), and jumps (`JAL`, `JALR`, `AUIPC`, `LUI`). The simulator reads instructions from an input file, simulates the pipeline for a specified number of cycles, and outputs the pipeline execution diagram to a file.

## Design Decisions
### 1. **Processor**
- **Register File**: It creates an array of 32 registers and initialises all of them to be zero. It defines to funcitons read and write for reading and writing operations.
- **ALU Unit**: It does appropriate operations based on ALUCtrl signals .
- It initialises all pipeline registers, stall bools, etc. and copies all instructions in a memory vector.
### 2. **Pipeline Structure**
- **5-Stage Pipeline**: The simulator implements a classic 5-stage RISC-V pipeline: Instruction Fetch (IF), Instruction Decode (ID), Execute (EX), Memory Access (MEM), and Write-Back (WB). This structure is standard for RISC-V and allows for clear separation of concerns in each stage.
- **Pipeline Registers**: Each stage is separated by pipeline registers (`IF_ID`, `ID_EX`, `EX_MEM`, `MEM_WB`) to hold intermediate data (e.g., PC, instruction, operands, control signals). This ensures proper data propagation between stages.
- **Control Signals**: A `ControlSignals` struct is used to manage control signals (e.g., `RegWrite`, `ALUSrc`, `MemRead`, `MemWrite`, `MemToReg`, `Branch`) for each instruction, set during the ID stage and propagated through the pipeline.

### 3. **Instruction Decoding**
- **Instruction Types**: The simulator supports RISC-V instruction types: R-type (e.g., `ADD`), I-type (e.g., `ADDI`, `LB`, `JALR`), S-type (e.g., `SW`), B-type (e.g., `BEQ`), J-type (e.g., `JAL`), and U-type (e.g., `AUIPC`, `LUI`).
- **Decoding Functions**: Separate decoding functions (`decodeRType`, `decodeIType`, etc.) are implemented for each instruction type to extract fields like `opcode`, `rd`, `rs1`, `rs2`, `funct3`, `funct7`, `imm`, and `shamt`. This modular approach simplifies handling different instruction formats.
- **Immediate Sign-Extension**: Immediate values are sign-extended appropriately to 32-bit for each instruction type (e.g., 12-bit for I-type, 13-bit for B-type, 21-bit for J-type) to ensure correct arithmetic operations.

### 4. **Pipeline Execution**
- **Stage Functions**: Each pipeline stage is implemented as a separate method (`fetchStage`, `decodeStage`, `executeStage`, `memoryStage`, `writeBackStage`) in the `Processor` base class, with `executeStage` overridden in `NoForwardingProcessor` and `ForwardingProcessor` to handle their specific behaviors.
- **Pipeline Diagram**: A 2D vector (`std::vector<std::vector<std::string>> vec`) tracks the pipeline stage of each instruction across cycles. The `printPipelineDiagram` function outputs the diagram in the format `instruction;stage0;stage1;stage2;...`, where stages are `IF`, `ID`, `EX`, `MEM`, `WB`, " " (not yet reached), or `"-"` (passed or stalled).
- **Cycle Simulation**: The `runSimulation` method iterates over the specified number of cycles, executing each stage in reverse order (WB → MEM → EX → ID → IF) to ensure proper data propagation.

### 5. **No-Forwarding vs. Forwarding**
- **No-Forwarding Processor**:
  - **Stalls for Data Hazards**: Detects data hazards in the `decodeStage` by checking if the destination register (`rd`) of instructions in EX/MEM or MEM/WB matches the source registers (`rs1`, `rs2`) of the current instruction. If a hazard is detected, the pipeline stalls for the required number of cycles (2 for EX/MEM, 1 for MEM/WB).
  - **Stall Implementation**: Uses `stall`, `stallCycles`, `stallIF`, and `stallID` flags to manage stalls. During a stall, the IF and ID stages are paused, and NOPs are inserted into the pipeline.
- **Forwarding Processor**:
  - **Data Forwarding**: Implements forwarding to resolve data hazards without stalling in most cases. In the `executeStage`, it forwards values from EX/MEM (`ex_mem.aluResult`) or MEM/WB (`mem_wb.memData`) to the operands (`op1`, `op2`) if the destination register matches the source registers.
  - **Stalls for Load-Use Hazards**: Stalls only for load-use hazards (e.g., `lb` followed by an instruction using the loaded value). For example, if an `lb` in EX/MEM writes to `rd`, and the next instruction in ID/EX uses `rd` as `rs1` or `rs2`, a 1-cycle stall is inserted.
  - **Forwarding Logic**: Checks for forwarding from EX/MEM and MEM/WB stages, prioritizing the most recent value (EX/MEM over MEM/WB). Special handling is added for load instructions (`opcode == 0x03`) and store instructions (`opcode == 0x23`).

### 6. **Control Flow Handling**
- **Branches and Jumps**: The `executeStage` handles branches (`BEQ`, `BNE`, `BLT`, `BGE`) and jumps (`JAL`, `JALR`). Branch conditions are evaluated using the ALU result (`ex_mem.aluResult`), and the branch target (`ex_mem.branchTarget`) or jump target (`ex_mem.jump`) is set if the condition is met.
- **Pipeline Flush**: After a branch or jump is taken, the `fetchStage` updates the PC to the target address (`ex_mem.branchTarget` or `ex_mem.jump`) and clears the `jump` and `branchTarget` fields. A 1-cycle NOP is inserted into the pipeline (via `id_ex.nop`) to account for the branch delay slot.
- **Branch Prediction**: The simulator assumes a simple "not-taken" prediction, meaning it continues fetching the next instruction until the branch is resolved in the EX stage. If the branch is taken, the incorrect instructions are flushed by updating the PC.

## Build System
- **Makefile**: A `Makefile` is provided to build two executables: `noforward` (for `NoForwardingProcessor`) and `forward` (for `ForwardingProcessor`). It uses `g++` with C++11 standard and includes debugging flags (`-g`) and warnings (`-Wall`).

## Sources Consulted
1. **RISC-V Specification**
2. **Computer Organization and Design: The Hardware/Software Interface (RISC-V Edition)**
3. **ChatGPT (Grok AI) for debugging assistance**


