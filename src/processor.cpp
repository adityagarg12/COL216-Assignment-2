#include <iostream>
#include <vector>



class RegisterFile {
    private:
        int registers[32] = {0};  // 32 Registers initialized to 0
    public:
        int read(int reg) { return registers[reg]; }
        void write(int reg, int value) { if (reg != 0) registers[reg] = value; } // x0 is always 0
    };

    
struct IF_ID {
    uint32_t instruction;
    int pc;
};

struct ID_EX {
    uint32_t instruction;
    int pc, rs1, rs2, rd;
    int imm;
    int controlSignals;
};

struct EX_MEM {
    int aluResult;
    int rd;
    bool memRead, memWrite, regWrite;
};

struct MEM_WB {
    int memData;
    int aluResult;
    int rd;
    bool regWrite;
};

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

class Processor {
    private:
        RegisterFile regFile;
        ALU alu;
        std::vector<uint32_t> memory;  // Instruction memory
        IF_ID if_id;
        ID_EX id_ex;
        EX_MEM ex_mem;
        MEM_WB mem_wb;
    
    public:
        Processor(std::vector<uint32_t> instructions) {
            memory = instructions;
        }
    
        void fetchStage() {
            if_id.instruction = memory[if_id.pc / 4];
            if_id.pc += 4;
        }
    
        void decodeStage() {
            uint32_t inst = if_id.instruction;
            id_ex.rs1 = (inst >> 15) & 0x1F;
            id_ex.rs2 = (inst >> 20) & 0x1F;
            id_ex.rd  = (inst >> 7) & 0x1F;
            id_ex.imm = (inst >> 20); // Simplified for immediate values
        }
    
        void executeStage() {
            ex_mem.aluResult = alu.execute(id_ex.rs1, id_ex.imm, '+'); // Example: ADDI instruction
        }
    
        void memoryStage() { /* Handle memory operations if required */ }
    
        void writeBackStage() { regFile.write(mem_wb.rd, mem_wb.aluResult); }
    
        void runSimulation(int cycles) {
            for (int i = 0; i < cycles; i++) {
                writeBackStage();
                memoryStage();
                executeStage();
                decodeStage();
                fetchStage();
            }
        }
    };
    
