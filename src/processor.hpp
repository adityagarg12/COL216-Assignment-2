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
    int execute(int op1, int op2, int ALUCtrl) {
        if (ALUCtrl == 0) return op1 & op2;  // AND
        if (ALUCtrl == 1) return op1 | op2;  // OR
        if (ALUCtrl == 2) return op1 + op2;  // ADD
        if (ALUCtrl == 6) return op1 - op2;  // SUB
        return 0;  // Default return value
    }
};

// ****************** PIPELINE REGISTERS ******************

// struct PC {
//     int pc = 0;
// };


struct IF_ID {
    uint32_t instruction = 0;
    int pc = 0;
};

struct ID_EX {
    uint32_t instruction = 0;
    int pc = 0;
    int rs1 = 0, rs2 = 0, rd = 0;
    int64_t imm = 0;
    uint32_t opcode = 0;
    uint32_t funct3 = 0;
    uint32_t funct7 = 0;
    ControlSignals control;
};

struct EX_MEM {
    int aluResult = 0;
    int rd = 0;
    
    ControlSignals control;
};

struct MEM_WB {
    int memData = 0;
    int aluResult = 0;
    int rd = 0;
    bool regWrite = false;
    ControlSignals control;
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

struct DecodedInstruction {
    uint32_t opcode;
    uint32_t rd;
    uint32_t funct3;
    uint32_t rs1;
    uint32_t rs2;
    uint32_t funct7;
    int64_t imm;  // Signed immediate
};

DecodedInstruction decodeRType(uint32_t instruction) {
    DecodedInstruction decoded;
    decoded.opcode = instruction & 0x7F;
    decoded.rd = (instruction >> 7) & 0x1F;
    decoded.funct3 = (instruction >> 12) & 0x07;
    decoded.rs1 = (instruction >> 15) & 0x1F;
    decoded.rs2 = (instruction >> 20) & 0x1F;
    decoded.funct7 = (instruction >> 25) & 0x7F;
    return decoded;
}

DecodedInstruction decodeIType(uint32_t instruction) {
    DecodedInstruction decoded;
    decoded.opcode = instruction & 0x7F;
    decoded.rd = (instruction >> 7) & 0x1F;
    decoded.funct3 = (instruction >> 12) & 0x07;
    decoded.rs1 = (instruction >> 15) & 0x1F;
    // Sign-extend 12-bit immediate
    decoded.imm = ((int64_t)(instruction & 0xFFF00000)) >> 20;
    return decoded;
}

DecodedInstruction decodeSType(uint32_t instruction) {
    DecodedInstruction decoded;
    decoded.opcode = instruction & 0x7F;
    decoded.funct3 = (instruction >> 12) & 0x07;
    decoded.rs1 = (instruction >> 15) & 0x1F;
    decoded.rs2 = (instruction >> 20) & 0x1F;
    // Combine imm[11:5] and imm[4:0]
    decoded.imm = ((int64_t)(instruction & 0xFE000000) >> 20) | ((instruction >> 7) & 0x1F);
    return decoded;
}

DecodedInstruction decodeBType(uint32_t instruction) {
    DecodedInstruction decoded;
    decoded.opcode = instruction & 0x7F;
    decoded.funct3 = (instruction >> 12) & 0x07;
    decoded.rs1 = (instruction >> 15) & 0x1F;
    decoded.rs2 = (instruction >> 20) & 0x1F;
    // Sign-extend immediate and arrange bits properly
    decoded.imm = (((int64_t)(instruction >> 31) & 1) << 12) |  // imm[12]
                  (((int64_t)(instruction >> 25) & 0x3F) << 5) | // imm[10:5]
                  (((int64_t)(instruction >> 8) & 0xF) << 1) |   // imm[4:1]
                  (((int64_t)(instruction >> 7) & 1) << 11);     // imm[11]
    // Sign-extend the immediate value
    decoded.imm = (decoded.imm << 51) >> 51;
    return decoded;
}

DecodedInstruction decodeJType(uint32_t instruction) {
    DecodedInstruction decoded;
    decoded.opcode = instruction & 0x7F;
    decoded.rd = (instruction >> 7) & 0x1F;
    // Sign-extend and reorder immediate bits
    decoded.imm = (((instruction >> 31) & 1) << 20) |
                  (((instruction >> 21) & 0x3FF) << 1) |
                  (((instruction >> 20) & 1) << 11) |
                  (((instruction >> 12) & 0xFF) << 12);
    return decoded;
}

DecodedInstruction decodeInstruction(uint32_t instruction) {
    uint32_t opcode = instruction & 0x7F;
    
    if (opcode == 0x33) return decodeRType(instruction); // R-Type
    if (opcode == 0x13 || opcode == 0x03) return decodeIType(instruction); // I-Type (ADDI, LOAD)
    if (opcode == 0x23) return decodeSType(instruction); // S-Type (Store)
    if (opcode == 0x63) return decodeBType(instruction); // B-Type (Branch)
    if (opcode == 0x6F) return decodeJType(instruction); // J-Type (JAL)

    return {}; // Return empty struct if unknown instruction
}


// ****************** CONTROL SIGNALS ******************

struct ControlSignals {
    bool RegWrite = 0;
    int ALUOp = 0; // ALU Operation type
    bool ALUSrc = 0;
    bool MemRead = 0;
    bool MemWrite = 0;
    bool MemToReg = 0;
    bool Branch = 0;
};



// ****************** FUNCTION DEFINITIONS ******************

void Processor::fetchStage() {
    if_id.instruction = memory[if_id.pc / 4];
    if_id.pc += 4;
}

void Processor::decodeStage() {
    uint32_t inst = if_id.instruction;
    uint32_t opcode = inst & 0x7F;
    uint32_t funct3 = (inst >> 12) & 0x07;
    
    id_ex.control = {}; // Reset control signals
    struct DecodedInstruction decodedInst = decodeInstruction(inst);

    switch (opcode) {
        case 0x33:  // R-Type (ADD, SUB, AND, OR)
            id_ex.rs1 = decodedInst.rs1;
            id_ex.rs2 = decodedInst.rs2;
            id_ex.rd = decodedInst.rd;
            id_ex.imm = 0;
            id_ex.opcode = opcode;
            id_ex.funct3 = decodedInst.funct3;
            id_ex.funct7 = decodedInst.funct7;
            id_ex.pc = if_id.pc; // TO BE CHECKED         
            id_ex.control.RegWrite = 1;
            id_ex.control.ALUOp = 2;  // ALU operation based on funct3
            id_ex.control.ALUSrc = 0; // ALU uses register
            break;

        case 0x13:  // I-Type (ADDI, ANDI, ORI)

            id_ex.rs1 = decodedInst.rs1;
            id_ex.rd = decodedInst.rd;
            id_ex.imm = decodedInst.imm;
            id_ex.opcode = opcode;
            id_ex.funct3 = decodedInst.funct3;
            id_ex.funct7 = decodedInst.funct7;
            id_ex.pc = if_id.pc; // TO BE CHECKED
            id_ex.control.RegWrite = 1;
            id_ex.control.ALUOp = 2;  
            id_ex.control.ALUSrc = 1; // ALU uses immediate
            break;

        case 0x03:  // Load (LW)
            id_ex.rs1 = decodedInst.rs1;
            id_ex.rd = decodedInst.rd;
            id_ex.imm = decodedInst.imm;
            id_ex.opcode = opcode;
            id_ex.funct3 = decodedInst.funct3;
            id_ex.funct7 = decodedInst.funct7;
            id_ex.pc = if_id.pc; // TO BE CHECKED
            id_ex.control.RegWrite = 1;
            id_ex.control.ALUOp = 0;  // ADD for memory address calculation
            id_ex.control.ALUSrc = 1;
            id_ex.control.MemRead = 1;
            id_ex.control.MemToReg = 1; // Write MEM data to reg
            break;

        case 0x23:  // Store (SW)
            id_ex.rs1 = decodedInst.rs1;
            id_ex.rs2 = decodedInst.rs2;
            id_ex.imm = decodedInst.imm;     
            id_ex.opcode = opcode;  
            id_ex.funct3 = decodedInst.funct3;
            id_ex.funct7 = decodedInst.funct7;     
            id_ex.pc = if_id.pc; // TO BE CHECKED
            id_ex.control.ALUOp = 0;
            id_ex.control.ALUSrc = 1;
            id_ex.control.MemWrite = 1;
            break;

        case 0x63:  // Branch (BEQ, BNE)
            id_ex.rs1 = decodedInst.rs1;
            id_ex.rs2 = decodedInst.rs2;
            id_ex.imm = decodedInst.imm;     
            id_ex.opcode = opcode;  
            id_ex.funct3 = decodedInst.funct3;
            id_ex.funct7 = decodedInst.funct7;   
            id_ex.pc = if_id.pc; // TO BE CHECKED
            id_ex.control.Branch = 1;
            id_ex.control.ALUOp = 1; // Branch ALU operation
            id_ex.control.ALUSrc = 0;
            int zero = alu.execute(regFile.read(id_ex.rs1), regFile.read(id_ex.rs2), 6);
            uint64_t shiftedImm = (uint64_t)id_ex.imm << 1;
            if (zero == 0) {
                if_id.pc = if_id.pc - 4 + shiftedImm;
            }


            break;

        default:
            std::cout << "Unknown opcode: " << opcode << "\n";
            break;
    }
}


void Processor::memoryStage() {
    if (ex_mem.control.MemRead) {
        mem_wb.memData = memory[ex_mem.aluResult / 4];
    }
    if (ex_mem.control.MemWrite) {
        memory[ex_mem.aluResult / 4] = regFile.read(ex_mem.rd);
    }

    mem_wb.aluResult = ex_mem.aluResult;
    mem_wb.rd = ex_mem.rd;
    mem_wb.control = ex_mem.control;
}

void Processor::writeBackStage() {
    if (mem_wb.regWrite) {
        if (mem_wb.control.MemToReg) { // TO BE CHANGED TO CONTROL SIGNAL 
            regFile.write(mem_wb.rd, mem_wb.memData);
        }
        else {
            regFile.write(mem_wb.rd, mem_wb.aluResult);
        }
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
