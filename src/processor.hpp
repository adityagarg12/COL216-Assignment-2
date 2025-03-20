#ifndef PROCESSOR_HPP
#define PROCESSOR_HPP

#include <iostream>
#include <vector>
#include <cstdint>

// Number of registers in RISC-V
#define NUM_REGISTERS 32

// ****************** REGISTER FILE ******************
class RegisterFile {
private:
    int registers[NUM_REGISTERS] = {0};  // 32 registers initialized to 0
public:
    int read(int reg) { return registers[reg]; }
    void write(int reg, int value) { if (reg != 0) registers[reg] = value; } // x0 is always 0
};

// ****************** ALU ******************
class ALU {
public:
    int execute(int op1, int op2, char operation) {
        switch (operation) {
            case '+': return op1 + op2;
            case '-': return op1 - op2;
            case '&': return op1 & op2;
            case '|': return op1 | op2;
            default: return 0;
        }
    }
};

// ****************** PIPELINE REGISTERS ******************
struct IF_ID {
    uint32_t instruction = 0;
    int pc = 0;
};

struct ID_EX {
    uint32_t instruction = 0;
    int pc = 0;
    int rs1 = 0, rs2 = 0, rd = 0;
    int imm = 0;
    int controlSignals = 0;
};

struct EX_MEM {
    int aluResult = 0;
    int rd = 0;
    bool memRead = false, memWrite = false, regWrite = false;
};

struct MEM_WB {
    int memData = 0;
    int aluResult = 0;
    int rd = 0;
    bool regWrite = false;
};

// ****************** PROCESSOR BASE CLASS ******************
class Processor {
protected:
    RegisterFile regFile;
    ALU alu;
    std::vector<uint32_t> memory;  // Instruction memory

    // Pipeline registers
    IF_ID if_id;
    ID_EX id_ex;
    EX_MEM ex_mem;
    MEM_WB mem_wb;

public:
    Processor(std::vector<uint32_t> instructions) { memory = instructions; }

    // Virtual functions to be implemented differently for no-forwarding and forwarding
    virtual void executeStage() = 0;

    // Common functions
    void fetchStage();
    void decodeStage();
    void memoryStage();
    void writeBackStage();
    void runSimulation(int cycles);
};

// ****************** FUNCTION DEFINITIONS ******************

void Processor::fetchStage() {
    if_id.instruction = memory[if_id.pc / 4];
    if_id.pc += 4;
}

void Processor::decodeStage() {
    uint32_t inst = if_id.instruction;
    id_ex.rs1 = (inst >> 15) & 0x1F;
    id_ex.rs2 = (inst >> 20) & 0x1F;
    id_ex.rd  = (inst >> 7) & 0x1F;
    id_ex.imm = (inst >> 20); // Simplified for immediate values
}

void Processor::memoryStage() {
    if (ex_mem.memRead) {
        mem_wb.memData = memory[ex_mem.aluResult / 4];
    }
    mem_wb.aluResult = ex_mem.aluResult;
    mem_wb.rd = ex_mem.rd;
    mem_wb.regWrite = ex_mem.regWrite;
}

void Processor::writeBackStage() {
    if (mem_wb.regWrite) {
        regFile.write(mem_wb.rd, mem_wb.aluResult);
    }
}

void Processor::runSimulation(int cycles) {
    for (int i = 0; i < cycles; i++) {
        writeBackStage();
        memoryStage();
        executeStage();
        decodeStage();
        fetchStage();
    }
}

#endif // PROCESSOR_HPP
