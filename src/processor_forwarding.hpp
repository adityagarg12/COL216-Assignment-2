#ifndef PROCESSOR_FORWARDING_HPP
#define PROCESSOR_FORWARDING_HPP

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

// ****************** PIPELINE REGISTERS ******************

// struct PC {
//     int pc = 0;
// };


struct IF_ID {
    uint32_t instruction = 0;
    int pc = 0;
    bool perform = false;
    bool jump = false;
    int nop = 0;
    int nop_count = 0;

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
    bool perform = false;
    int nop = 0;
    int nop_count = 0;

};

struct EX_MEM {
    int aluResult = 0;
    int rd = 0;
    int rs2 = 0;
    int pc = 0;
    bool zero = false;
    uint32_t opcode = 0;
    ControlSignals control;
    bool perform = false;
    uint64_t jump = -1;
};

struct MEM_WB {
    int memData = 0;
    int aluResult = 0;
    int rd = 0;
    int pc = 0;
    bool regWrite = false;
    ControlSignals control;
    bool perform = false;
};
//********************************************* */
int NumInstruction(std::vector<uint32_t> instructions){
    int count = 0;
    for (int i = 0; i < instructions.size(); i++){
        if (instructions[i] != 0){
            count++;
        }
    }
    return count;
}


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

    bool stall = false; // Stall flag
    int stallCycles = 0; // Number of cycles to stall
    IF_ID stalled_if_id; // Stalled instruction
    bool stallIF = false;
    bool stallID = false;
    int numInstructions = 0;

    bool start = true;
    bool if_done = false;
    
public:
    Processor(std::vector<uint32_t> instructions) { memory = instructions;
        numInstructions = NumInstruction(instructions);
    }

    // Virtual functions to be implemented differently for no-forwarding and forwarding
    virtual void executeStage(int cycles,std::vector<std::vector<std::string>> &vec) = 0;

    // Common functions
    void fetchStage(int cycles,std::vector<std::vector<std::string>> &vec);
    void decodeStage(int cycles,std::vector<std::vector<std::string>> &vec);
    void memoryStage(int cycles,std::vector<std::vector<std::string>> &vec);
    void writeBackStage(int cycles,std::vector<std::vector<std::string>> &vec);
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





// ****************** FUNCTION DEFINITIONS ******************

void Processor::fetchStage(int cycles, std::vector<std::vector<std::string>> &vec) {
    if (if_done == true) {
        return;
    }
    if (stallIF){
        if (stallID == false) {          
            stallIF = false;
        }
        if(if_id.pc/4  < numInstructions){
        vec[if_id.pc/4][cycles] = "- ";
        }
        return;
    }
    if (start == false ){
        if(if_id.jump == true){
            if_id.jump = false;
            if(if_id.pc/4 >= numInstructions){
                return ; //exception to be raised
            }
        }
        else if ((if_id.pc/4 + 1)  < numInstructions) if_id.pc +=4;
        else {
            if_id = IF_ID{};
            if_done = true;
            return;        
        }
    }
    if_id.instruction = memory[if_id.pc / 4];
    if(if_id.pc/4 < numInstructions){
    vec[if_id.pc/4][cycles] = "IF";
    }
    start = false;
    if_id.perform = true;
    if(ex_mem.jump != -1){
        if_id.pc = ex_mem.jump;
        ex_mem.jump = -1;
        if_id.jump = true;
    }
    // if_id.pc += 4;
}

void Processor::decodeStage(int cycles,std::vector<std::vector<std::string>> &vec) {

    if(if_id.nop > 0){
        
        if(if_id.nop_count == if_id.nop){
            if_id.nop = 0;
            if_id.nop_count = 0;
            if_id = IF_ID{};
            stallID = false;
            return;
        }   
        if_id.nop_count++;
    }

    if (stallID) {
        // During stall, insert NOP into ID/EX
        // id_ex = ID_EX{}; // NOP
        // stallCycles--;
        // if (stallCycles <= 0) {
        //     stall = false;
        //     // Don't clear stallIF yet; it will be cleared when ID is free
        // }
        if (stall == false) {
            stallID = false;
        }
        stallIF = true; // Keep IF stalled while ID is occupied
        if (id_ex.pc/4 < numInstructions){
        vec[id_ex.pc/4][cycles]="- ";
        }
        return;
    }

    if (id_ex.pc/4 + 1 >= numInstructions) {
        id_ex = ID_EX{};
        return; // No more instructions to decode
    }

    if (if_id.perform == false) {
        return;
    }

    uint32_t inst = if_id.instruction;
    uint32_t opcode = inst & 0x7F;
    DecodedInstruction decodedInst = decodeInstruction(inst);

    // Hazard detection
    bool hazard = false;
    int cycles_to_stall = 0;

    // // Check ID/EX (instruction in EX stage)
    // if (id_ex.control.RegWrite && id_ex.rd != 0 &&
    //     (id_ex.rd == decodedInst.rs1 || id_ex.rd == decodedInst.rs2)) {
    //     hazard = true;
    //     cycles_to_stall = 3; // ID/EX → EX → MEM → WB
    // }
    // Check EX/MEM
    // if (ex_mem.control.RegWrite && ex_mem.rd != 0 &&
    //          (ex_mem.rd == decodedInst.rs1 || ex_mem.rd == decodedInst.rs2) && ex_mem.opcode == 0x03 ) {
    //     hazard = true;
    //     cycles_to_stall = 1; // EX/MEM → MEM → WB
    // }
    
    if (ex_mem.control.RegWrite && ex_mem.rd != 0){
        if ( ex_mem.opcode == 0x03 && decodedInst.opcode == 0x23 ) {
            if (ex_mem.rd == decodedInst.rs1){
                hazard = true;
                cycles_to_stall = 1; // EX/MEM → MEM → WB
            }
             
        }
        else if ( (ex_mem.rd == decodedInst.rs1 || ex_mem.rd == decodedInst.rs2) && ex_mem.opcode == 0x03 ) {
            hazard = true;
            cycles_to_stall = 1; // EX/MEM → MEM → WB
        }
         
}

    // Check MEM/WB
    // else if (mem_wb.control.RegWrite && mem_wb.rd != 0 &&
    //          (mem_wb.rd == decodedInst.rs1 || mem_wb.rd == decodedInst.rs2)) {
    //     hazard = true;
    //     cycles_to_stall = 1; // MEM/WB → WB
    // }

    if (hazard) {
        stall = true;
        stallCycles = cycles_to_stall;
        // stallID = true;
        // id_ex = ID_EX{}; // Insert NOP
        // stallIF = true; // Keep IF stalled while ID is occupied
        // return;
    }
    // id_ex = ID_EX{}; // Clear previous values
    id_ex.pc = if_id.pc ;
    if ((id_ex.pc/4 >= 0) && (id_ex.pc/4 < numInstructions)){
    vec[id_ex.pc/4][cycles] = "ID";
    }
    if_id.perform = false;
    id_ex.perform = true;
    switch (opcode) {
        case 0x33:  // R-Type (ADD, SUB, AND, OR)
            id_ex.rs1 = decodedInst.rs1;
            id_ex.rs2 = decodedInst.rs2;
            id_ex.rd = decodedInst.rd;
            id_ex.imm = 0;
            id_ex.opcode = opcode;
            id_ex.funct3 = decodedInst.funct3;
            id_ex.funct7 = decodedInst.funct7;
            
             // TO BE CHECKED         
            id_ex.control.RegWrite = 1;
            id_ex.control.ALUOp = 2;  // ALU operation based on funct3
            id_ex.control.ALUSrc = 0; // ALU uses register
            id_ex.control.MemToReg = 0; // Write ALU result to reg
            id_ex.control.MemRead = 0;
            id_ex.control.MemWrite = 0;
            id_ex.control.Branch = 0;

            break;

        case 0x13:  // I-Type (ADDI, ANDI, ORI)
            // printf("I-Type\n");
            // printf("rs1: %d\n", decodedInst.rs1);
            // printf("rd: %d\n", decodedInst.rd);
            // printf("imm: %d\n", decodedInst.imm);
            id_ex.rs1 = decodedInst.rs1;
            id_ex.rd = decodedInst.rd;
            id_ex.imm = decodedInst.imm;
            id_ex.opcode = opcode;
            id_ex.funct3 = decodedInst.funct3;
            id_ex.funct7 = decodedInst.funct7;
             // TO BE CHECKED
            id_ex.control.RegWrite = 1;
            id_ex.control.ALUOp = 2;  
            id_ex.control.ALUSrc = 1; // ALU uses immediate
            id_ex.control.MemToReg = 0; // Write ALU result to reg
            id_ex.control.MemRead = 0;
            id_ex.control.MemWrite = 0;
            id_ex.control.Branch = 0;
            break;

        case 0x03:  // Load (LW)
            id_ex.rs1 = decodedInst.rs1;
            id_ex.rd = decodedInst.rd;
            id_ex.imm = decodedInst.imm;
            id_ex.opcode = opcode;
            id_ex.funct3 = decodedInst.funct3;
            id_ex.funct7 = decodedInst.funct7;
             // TO BE CHECKED
            id_ex.control.RegWrite = 1;
            id_ex.control.ALUOp = 0;  // ADD for memory address calculation
            id_ex.control.ALUSrc = 1;
            id_ex.control.MemRead = 1;
            id_ex.control.MemToReg = 1; // Write MEM data to reg
            id_ex.control.MemWrite = 0;
            id_ex.control.Branch = 0;

            break;

        case 0x23:  // Store (SW)
            id_ex.rs1 = decodedInst.rs1;
            id_ex.rs2 = decodedInst.rs2;
            id_ex.imm = decodedInst.imm;     
            id_ex.opcode = opcode;  
            id_ex.funct3 = decodedInst.funct3;
            id_ex.funct7 = decodedInst.funct7;     
             // TO BE CHECKED
            id_ex.control.ALUOp = 0;
            id_ex.control.ALUSrc = 1;
            id_ex.control.MemWrite = 1;
            id_ex.control.MemRead = 0;
            id_ex.control.MemToReg = 0;
            id_ex.control.RegWrite = 0;
            id_ex.control.Branch = 0;

            break;

        case 0x63:  // Branch (BEQ, BNE)
            id_ex.rs1 = decodedInst.rs1;
            id_ex.rs2 = decodedInst.rs2;
            id_ex.imm = decodedInst.imm;     
            id_ex.opcode = opcode;  
            id_ex.funct3 = decodedInst.funct3;
            id_ex.funct7 = decodedInst.funct7;   
             // TO BE CHECKED
            id_ex.control.Branch = 1;
            id_ex.control.ALUOp = 1; // Branch ALU operation
            id_ex.control.ALUSrc = 0;
            id_ex.control.MemWrite = 0;
            id_ex.control.MemRead = 0;
            id_ex.control.MemToReg = 0;
            id_ex.control.RegWrite = 0;
            // int zero = alu.execute(regFile.read(id_ex.rs1), regFile.read(id_ex.rs2), 6);
            // uint64_t shiftedImm = (uint64_t)id_ex.imm << 1;
            // if (zero == 0) {
            //     if_id.pc -4 = if_id.pc -4 - 4 + shiftedImm;
            // }
            break;
        case 0x6F:  // Jump (JAL)  
            id_ex.rd = decodedInst.rd;
            id_ex.imm = decodedInst.imm;

            id_ex.opcode = opcode;
             // TO BE CHECKED
            id_ex.control.RegWrite = 1;
            id_ex.control.ALUOp = 2;  // ALU operation based on funct3
            id_ex.control.ALUSrc = 1; // ALU uses immediate
            id_ex.control.MemToReg = 0; // Write ALU result to reg
            id_ex.control.MemRead = 0;
            id_ex.control.MemWrite = 0;
            id_ex.control.Branch = 0;
            
            if_id.nop = 1;
            id_ex.nop = 1;

            break;
        case 0x67: // Jump Register (JALR)
            id_ex.rs1 = decodedInst.rs1;
            id_ex.rd = decodedInst.rd;
            id_ex.imm = decodedInst.imm;
            id_ex.opcode = opcode;
             // TO BE CHECKED
            id_ex.control.RegWrite = 1;
            id_ex.control.ALUOp = 2;  // ALU operation based on funct3
            id_ex.control.ALUSrc = 1; // ALU uses immediate
            id_ex.control.MemToReg = 0; // Write ALU result to reg
            id_ex.control.MemRead = 0;
            id_ex.control.MemWrite = 0;
            id_ex.control.Branch = 0;

            if_id.nop = 1;
            id_ex.nop = 1;;
            break;

        default:
            std::cout << "Unknown opcode: " << opcode << "\n";
            break;
    }
}


void Processor::memoryStage(int cycles,std::vector<std::vector<std::string>> &vec) {
    mem_wb = MEM_WB(); // Clear previous values
    if (ex_mem.perform == false) {
        return;
    }

    if (ex_mem.control.MemRead) {
        mem_wb.memData = memory[ex_mem.aluResult / 4];
    }
    if (ex_mem.control.MemWrite) {
        memory[ex_mem.aluResult / 4] = regFile.read(ex_mem.rd);
    }

    mem_wb.aluResult = ex_mem.aluResult;
    mem_wb.rd = ex_mem.rd;
    mem_wb.pc = ex_mem.pc;
    mem_wb.control = ex_mem.control;
    if (mem_wb.pc/4 < numInstructions){
    vec[mem_wb.pc/4][cycles]="MEM";
    }
    mem_wb.perform = true;
    ex_mem = EX_MEM();
    ex_mem.perform = false; // TO BE CHECKED
}

void Processor::writeBackStage(int cycles,std::vector<std::vector<std::string>> &vec) {
    if(mem_wb.perform == false){
        return;
    }   

    if (mem_wb.regWrite) {
        if (mem_wb.control.MemToReg) { 
            regFile.write(mem_wb.rd, mem_wb.memData);
        }
        else {
            regFile.write(mem_wb.rd, mem_wb.aluResult);
        }
    }
    if (((mem_wb.pc)/4)< numInstructions){
    vec[(mem_wb.pc)/4][cycles] = "WB";
    }
    mem_wb = MEM_WB();
    mem_wb.perform = false;

    
}

void printPipelineDiagram(const std::vector<std::vector<std::string>> &vec) {
    std::cout << "Pipeline Execution:\n";
    // Print cycle numbers on top
    
    for (size_t i = 0; i < vec[0].size(); ++i) {
        std::cout <<" "<< i << " ";
    }
    std::cout << "\n";
    // Iterate over each instruction (row-wise)
    for (const auto &row : vec) {
        // Iterate over each cycle (column-wise)
        for (const auto &stage : row) {
            std::cout << stage << " ";  // Print each stage in a cycle
        }
        std::cout << "\n";  // New line for next instruction
    }
}



void Processor::runSimulation(int cycles) {

    std::vector<std::vector<std::string>> vec(numInstructions, std::vector<std::string>(cycles, "  "));

    for (int i = 0; i < cycles; i++) {
        std::cout << "Cycle " << i << ":\n";
        std::cout << "IF: PC=" << if_id.pc << ", Instr=" << std::hex << if_id.instruction << "\n";
        std::cout << "ID: Instr=" << id_ex.instruction << ", rs1=" << id_ex.rs1 << ", rs2=" << id_ex.rs2 << ",rd=" << id_ex.rd << "\n";
        std::cout << "EX: ALUResult=" << ex_mem.aluResult << "\n";
        std::cout << "MEM: MemData=" << mem_wb.memData << "\n";
        std::cout << "WB: rd=" << mem_wb.rd << ", value=" << (mem_wb.control.MemToReg ? mem_wb.memData : mem_wb.aluResult) << "\n";
        if (stall) std::cout << "Stall: " << stallCycles << " cycles remaining\n";
        std::cout << "\n";
       

        writeBackStage(i,vec);
        memoryStage(i,vec);
        executeStage(i,vec);
        decodeStage(i,vec);

        fetchStage(i,vec);

       
    }

    printPipelineDiagram(vec);

}

#endif // PROCESSOR_HPP
