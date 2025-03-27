#include "processor_forwarding.hpp"

class ForwardingProcessor : public Processor {
public:
    using Processor::Processor;

    
// void decodeStage(int cycles,std::vector<std::vector<std::string>> &vec) {

//     if(if_id.nop > 0){
        
//         if(if_id.nop_count == if_id.nop){
//             if_id.nop = 0;
//             if_id.nop_count = 0;
//             if_id = IF_ID{};
//             stallID = false;
//             return;
//         }   
//         if_id.nop_count++;
//     }

//     if (stallID) {
//         // During stall, insert NOP into ID/EX
//         // id_ex = ID_EX{}; // NOP
//         // stallCycles--;
//         // if (stallCycles <= 0) {
//         //     stall = false;
//         //     // Don't clear stallIF yet; it will be cleared when ID is free
//         // }
//         if (stall == false) {
//             stallID = false;
//         }
//         stallIF = true; // Keep IF stalled while ID is occupied
//         if (id_ex.pc/4 < numInstructions){
//         vec[id_ex.pc/4][cycles]="-";
//         }
//         return;
//     }

//     if (id_ex.pc/4 + 1 >= numInstructions) {
//         id_ex = ID_EX{};
//         return; // No more instructions to decode
//     }

//     if (if_id.perform == false) {
//         return;
//     }

//     uint32_t inst = if_id.instruction;
//     uint32_t opcode = inst & 0x7F;
//     DecodedInstruction decodedInst = decodeInstruction(inst);

//     // Hazard detection
//     bool hazard = false;
//     int cycles_to_stall = 0;

//     // // Check ID/EX (instruction in EX stage)
//     // if (id_ex.control.RegWrite && id_ex.rd != 0 &&
//     //     (id_ex.rd == decodedInst.rs1 || id_ex.rd == decodedInst.rs2)) {
//     //     hazard = true;
//     //     cycles_to_stall = 3; // ID/EX → EX → MEM → WB
//     // }
//     // Check EX/MEM
//     // if (ex_mem.control.RegWrite && ex_mem.rd != 0 &&
//     //          (ex_mem.rd == decodedInst.rs1 || ex_mem.rd == decodedInst.rs2) && ex_mem.opcode == 0x03 ) {
//     //     hazard = true;
//     //     cycles_to_stall = 1; // EX/MEM → MEM → WB
//     // }

//     if (ex_mem.control.RegWrite && ex_mem.rd != 0){
//         if ( ex_mem.opcode == 0x03 && decodedInst.opcode == 0x23 ) {
//             if (ex_mem.rd == decodedInst.rs1){
//                 hazard = true;
//                 cycles_to_stall = 1; // EX/MEM → MEM → WB
//             }
             
//         }
//         else if ( (ex_mem.rd == decodedInst.rs1 || ex_mem.rd == decodedInst.rs2) && ex_mem.opcode == 0x03 ) {
//             hazard = true;
//             cycles_to_stall = 1; // EX/MEM → MEM → WB
//         }
         
// }

//     // Check MEM/WB
//     // else if (mem_wb.control.RegWrite && mem_wb.rd != 0 &&
//     //          (mem_wb.rd == decodedInst.rs1 || mem_wb.rd == decodedInst.rs2)) {
//     //     hazard = true;
//     //     cycles_to_stall = 1; // MEM/WB → WB
//     // }

//     if (hazard) {
//         stall = true;
//         stallCycles = cycles_to_stall;
//         // stallID = true;
//         // id_ex = ID_EX{}; // Insert NOP
//         // stallIF = true; // Keep IF stalled while ID is occupied
//         // return;
//     }
//     // id_ex = ID_EX{}; // Clear previous values
//     id_ex.pc = if_id.pc ;
//     if ((id_ex.pc/4 >= 0) && (id_ex.pc/4 < numInstructions)){
//     vec[id_ex.pc/4][cycles] = "ID";
//     }
//     if_id.perform = false;
//     id_ex.perform = true;
//     switch (opcode) {
//         case 0x33:  // R-Type (ADD, SUB, AND, OR)
//             id_ex.instruction = if_id.instruction;
//             id_ex.rs1 = decodedInst.rs1;
//             id_ex.rs2 = decodedInst.rs2;
//             id_ex.rd = decodedInst.rd;
//             id_ex.imm = 0;
//             id_ex.shamt = 0;
//             id_ex.opcode = opcode;
//             id_ex.funct3 = decodedInst.funct3;
//             id_ex.funct7 = decodedInst.funct7;
            
//              // TO BE CHECKED         
//             id_ex.control.RegWrite = 1;
//             id_ex.control.ALUOp = 2;  // ALU operation based on funct3
//             id_ex.control.ALUSrc = 0; // ALU uses register
//             id_ex.control.MemToReg = 0; // Write ALU result to reg
//             id_ex.control.MemRead = 0;
//             id_ex.control.MemWrite = 0;
//             id_ex.control.Branch = 0;

//             break;

//         case 0x13:  // I-Type (ADDI, ANDI, ORI)
//             // printf("I-Type\n");
//             // printf("rs1: %d\n", decodedInst.rs1);
//             // printf("rd: %d\n", decodedInst.rd);
//             // printf("imm: %d\n", decodedInst.imm);
//             id_ex.rs1 = decodedInst.rs1;
//             id_ex.rs2 = 0;
//             id_ex.rd = decodedInst.rd;
//             id_ex.imm = decodedInst.imm;
//             id_ex.opcode = opcode;
//             id_ex.funct3 = decodedInst.funct3;
//             id_ex.funct7 = decodedInst.funct7;
//             if (id_ex.funct3 == 0x1 || id_ex.funct3 == 0x5){

//                 id_ex.imm = decodedInst.shamt;

//             }
//              // TO BE CHECKED
//             id_ex.control.RegWrite = 1;
//             id_ex.control.ALUOp = 2;  
//             id_ex.control.ALUSrc = 1; // ALU uses immediate
//             id_ex.control.MemToReg = 0; // Write ALU result to reg
//             id_ex.control.MemRead = 0;
//             id_ex.control.MemWrite = 0;
//             id_ex.control.Branch = 0;
//             break;

//         case 0x03:  // Load (LW)
//             id_ex.rs1 = decodedInst.rs1;
//             id_ex.rs2 = 0;
//             id_ex.rd = decodedInst.rd;
//             id_ex.imm = decodedInst.imm;
//             id_ex.opcode = opcode;
//             id_ex.funct3 = decodedInst.funct3;
//             id_ex.funct7 = decodedInst.funct7;
//              // TO BE CHECKED
//             id_ex.control.RegWrite = 1;
//             id_ex.control.ALUOp = 0;  // ADD for memory address calculation
//             id_ex.control.ALUSrc = 1;
//             id_ex.control.MemRead = 1;
//             id_ex.control.MemToReg = 1; // Write MEM data to reg
//             id_ex.control.MemWrite = 0;
//             id_ex.control.Branch = 0;

//             break;

//         case 0x23:  // Store (SW)
//             id_ex.rs1 = decodedInst.rs1;
//             id_ex.rs2 = decodedInst.rs2;
//             id_ex.rd = 0;
//             id_ex.imm = decodedInst.imm;     
//             id_ex.opcode = opcode;  
//             id_ex.funct3 = decodedInst.funct3;
//             id_ex.funct7 = decodedInst.funct7;     
//              // TO BE CHECKED
//             id_ex.control.ALUOp = 0;
//             id_ex.control.ALUSrc = 1;
//             id_ex.control.MemWrite = 1;
//             id_ex.control.MemRead = 0;
//             id_ex.control.MemToReg = 0;
//             id_ex.control.RegWrite = 0;
//             id_ex.control.Branch = 0;

//             break;

//         case 0x63:  // Branch (BEQ, BNE)
//             id_ex.rs1 = decodedInst.rs1;
//             id_ex.rs2 = decodedInst.rs2;
//             id_ex.rd = 0;
//             id_ex.imm = decodedInst.imm;     
//             id_ex.opcode = opcode;  
//             id_ex.funct3 = decodedInst.funct3;
//             id_ex.funct7 = decodedInst.funct7;   
//              // TO BE CHECKED
//             id_ex.control.Branch = 1;
//             id_ex.control.ALUOp = 1; // Branch ALU operation
//             id_ex.control.ALUSrc = 0;
//             id_ex.control.MemWrite = 0;
//             id_ex.control.MemRead = 0;
//             id_ex.control.MemToReg = 0;
//             id_ex.control.RegWrite = 0;
//             // id_ex.nop =1;
//             // int zero = alu.execute(regFile.read(id_ex.rs1), regFile.read(id_ex.rs2), 6);
//             // uint32_t shiftedImm = (uint32_t)id_ex.imm << 1;
//             // if (zero == 0) {
//             //     if_id.pc -4 = if_id.pc -4 - 4 + shiftedImm;
//             // }
//             break;
//         case 0x6F:  // Jump (JAL)  
//             id_ex.rd = decodedInst.rd;
//             id_ex.imm = decodedInst.imm;
//             id_ex.rs1 = 0;
//             id_ex.rs2 = 0;
//             id_ex.opcode = opcode;
//              // TO BE CHECKED
//             id_ex.control.RegWrite = 1;
//             id_ex.control.ALUOp = 2;  // ALU operation based on funct3
//             id_ex.control.ALUSrc = 1; // ALU uses immediate
//             id_ex.control.MemToReg = 0; // Write ALU result to reg
//             id_ex.control.MemRead = 0;
//             id_ex.control.MemWrite = 0;
//             id_ex.control.Branch = 0;
            
//             // if_id.nop = 1;
//             id_ex.nop = 1;

//             break;
//         case 0x67: // Jump Register (JALR)
//             id_ex.rs1 = decodedInst.rs1;
//             id_ex.rs2 = 0;
//             id_ex.rd = decodedInst.rd;
//             id_ex.imm = decodedInst.imm;
//             id_ex.opcode = opcode;
//              // TO BE CHECKED
//             id_ex.control.RegWrite = 1;
//             id_ex.control.ALUOp = 2;  // ALU operation based on funct3
//             id_ex.control.ALUSrc = 1; // ALU uses immediate
//             id_ex.control.MemToReg = 0; // Write ALU result to reg
//             id_ex.control.MemRead = 0;
//             id_ex.control.MemWrite = 0;
//             id_ex.control.Branch = 0;

//             // if_id.nop = 1;
//             id_ex.nop = 1;;
//             break;
//             case 0x17: // AUIPC
//             id_ex.rs1 = 0;
//             id_ex.rs2 = 0;
//             id_ex.rd = decodedInst.rd;
//             id_ex.imm = decodedInst.imm;
//             id_ex.opcode = opcode;
//             id_ex.funct3 = decodedInst.funct3;
//             id_ex.funct7 = decodedInst.funct7;
//             id_ex.control.RegWrite = 1;
//             id_ex.control.ALUOp = 2;  // ALU operation based on funct3
//             id_ex.control.ALUSrc = 1; // ALU uses immediate
//             id_ex.control.MemToReg = 0; // Write ALU result to reg
//             id_ex.control.MemRead = 0;
//             id_ex.control.MemWrite = 0;
//             id_ex.control.Branch = 0;
//             break;
//         case 0x37: // LUI
//             id_ex.rs1 = 0;
//             id_ex.rs2 = 0;
//             id_ex.rd = decodedInst.rd;
//             id_ex.imm = decodedInst.imm;
//             id_ex.opcode = opcode;
//             id_ex.funct3 = decodedInst.funct3;
//             id_ex.funct7 = decodedInst.funct7;
//             id_ex.control.RegWrite = 1;
//             id_ex.control.ALUOp = 2;  // ALU operation based on funct3
//             id_ex.control.ALUSrc = 1; // ALU uses immediate
//             id_ex.control.MemToReg = 0; // Write ALU result to reg
//             id_ex.control.MemRead = 0;
//             id_ex.control.MemWrite = 0;
//             id_ex.control.Branch = 0;
//             break;

//         default:
//             // std::cout << "Unknown opcode: " << opcode << "\n";
//             break;
//     }
// }



    void executeStage(int cycle,std::vector<std::vector<std::string>> &vec) override {

        


        if (stall) {
            // ex_mem = EX_MEM{}; // NOP
            stallCycles--;
            if (stallCycles <= 0) {
                stall = false;
                
            }
            stallID = true; // Keep ID stalled while EX is occupied
            return;
            

        }

        if (id_ex.nop > 0) {
            
            if (id_ex.nop_count == id_ex.nop) {
                id_ex.nop = 0;
                id_ex.nop_count = 0;
                id_ex = ID_EX{};
                stall = false;
                return;
            }
            id_ex.nop_count++;
        }

        if(ex_mem.pc/4 + 1 >= numInstructions){
            ex_mem = EX_MEM{};
            return;
        }

        if(id_ex.perform == false){
            return;
        }

        int op1 = (id_ex.opcode==0x6F || id_ex.opcode==0x17) ? id_ex.pc : regFile.read(id_ex.rs1);
        int op2 = id_ex.control.ALUSrc ? id_ex.imm : regFile.read(id_ex.rs2);

        if (ex_mem.control.RegWrite && ex_mem.rd == id_ex.rs1) op1 = ex_mem.aluResult;
        if (ex_mem.control.RegWrite && ex_mem.rd == id_ex.rs2) op2 = ex_mem.aluResult;
        if (ex_mem.opcode == 0x03 && ex_mem.control.RegWrite && ex_mem.rd == id_ex.rs1) op1 = mem_wb.memData;
        if (ex_mem.opcode == 0x03 && ex_mem.control.RegWrite && ex_mem.rd == id_ex.rs2) op2 = mem_wb.memData;
        if (ex_mem.opcode == 0x03 && id_ex.opcode == 0x23 && ex_mem.control.RegWrite && ex_mem.rd == id_ex.rs2) op2 = mem_wb.memData;
        
        int ALUCtrl = 0;
        if (id_ex.opcode == 0x03) {  
            ALUCtrl = 2;  // Load
        }
        else if (id_ex.opcode == 0x23) {
            ALUCtrl = 2;  // Store
        }
        else if (id_ex.opcode == 0x63) {
            ALUCtrl = 6;  // Branch
        }
        else if (id_ex.opcode == 0x33) {
            if (id_ex.funct3 == 0x0) {
                if (id_ex.funct7 == 0x00) {
                    ALUCtrl = 2;  // ADD
                }
                else {
                    ALUCtrl = 6;  // SUB
                }
            }    
            else if (id_ex.funct3 == 0x7) {
                ALUCtrl = 0;  // AND
            }
            else if (id_ex.funct3 == 0x6) {
                ALUCtrl = 1;  // OR
            }
            else if (id_ex.opcode == 0x13){
                if (id_ex.funct3 == 0x1) { // SLLI
                    // Perform shift left logical: rd = rs1 << shamt
                    ex_mem.aluResult = (uint32_t)regFile.read(id_ex.rs1) << id_ex.imm;
                    // regFile.write(id_ex.rd,(uint32_t)regFile.read(id_ex.rs1) << id_ex.imm) ;
                }
                else if (id_ex.funct3 == 0x5) {
                    if (id_ex.funct7 == 0x20) { // SRAI
                        // Perform shift right arithmetic: rd = rs1 >> shamt (sign-preserving)
                        ex_mem.aluResult = (int32_t)regFile.read(id_ex.rs1) >> id_ex.imm;
                        // regFile.write(id_ex.rd,(int32_t)regFile.read(id_ex.rs1) >> id_ex.imm) ;
                    } else { // SRLI (funct7 = 0x00)
                        // Perform shift right logical: rd = rs1 >> shamt (zero-fill)
                        ex_mem.aluResult = (uint32_t)regFile.read(id_ex.rs1) >> id_ex.imm;
                        // regFile.write(id_ex.rd,(uint32_t)regFile.read(id_ex.rs1) >> id_ex.imm) ;
                    }
                }
                else if (id_ex.funct3 == 0x0) {
                    // Perform add immediate: rd = rs1 + imm
                    ALUCtrl = 2;
                }
    
                else if (id_ex.funct3 == 0x7) {
                    // Perform and immediate: rd = rs1 & imm
                    ALUCtrl = 0;
                    // regFile.write(id_ex.rd,regFile.read(id_ex.rs1) & id_ex.imm) ;
                }
                else if (id_ex.funct3 == 0x6) {
                    // Perform or immediate: rd = rs1 | imm
                    ALUCtrl = 1;
                    // regFile.write(id_ex.rd,regFile.read(id_ex.rs1) | id_ex.imm) ;
                }
    
            }
        
        }
        else if (id_ex.opcode == 0x6F){
            ALUCtrl = 2; // JAL 
            ex_mem.jump = id_ex.pc + id_ex.imm;
            if_done = false;
        }
        else if (id_ex.opcode == 0x67){
            ALUCtrl = 2; // JALR
            ex_mem.jump = (regFile.read(id_ex.rs1) + op2) & ~1;
            if_done = false;
        }
        else if (id_ex.opcode == 0x17 || id_ex.opcode == 0x37){

            ALUCtrl = 2; // AUIPC

        }
        if(id_ex.opcode != 0x13 && (id_ex.opcode ==0x13 && id_ex.funct3 != 0x1 && id_ex.funct3 != 0x5)){
            ex_mem.aluResult = alu.execute(op1, op2, ALUCtrl);
            
        }
        // ex_mem.aluResult = alu.execute(op1, op2, ALUCtrl);
        if (id_ex.opcode == 0x67 || id_ex.opcode == 0x6F){
            ex_mem.aluResult = id_ex.pc + 4;
        }
        ex_mem.zero = ex_mem.aluResult == 0;
        if (id_ex.control.Branch) {
            bool taken = (id_ex.funct3 == 0x0 && ex_mem.aluResult == 0) || // BEQ
                         (id_ex.funct3 == 0x1 && ex_mem.aluResult != 0) || // BNE 
                         (id_ex.funct3 == 0x5 && ex_mem.aluResult >= 0 ) || //BGE
                         (id_ex.funct3 == 0x4 && ex_mem.aluResult < 0 )  //BLT
                         ;   
            if (taken) {
                ex_mem.branchTarget = id_ex.pc + (id_ex.imm );
                if_done = false;
                // if_id.instruction = 0; // Flush IF/ID with NOP
                // stall = false;
                // stallIF = false; // Clear stalls
            }
        }
        ex_mem.rd = id_ex.rd;
        ex_mem.rs2 = id_ex.rs2;
        ex_mem.opcode = id_ex.opcode;
        ex_mem.control = id_ex.control; 
        ex_mem.pc = id_ex.pc;
        if (ex_mem.pc/4 >= 0 && ex_mem.pc/4 < numInstructions){
        vec[ex_mem.pc/4][cycle]= "EX";
        }
        id_ex.perform = false;
        ex_mem.perform = true; // TO BE CHECKED
    }
};