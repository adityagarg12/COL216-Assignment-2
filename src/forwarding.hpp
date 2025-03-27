#include "processor_forwarding.hpp"

class ForwardingProcessor : public Processor {
public:
    using Processor::Processor;

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

        int op1 = id_ex.opcode==0x6F ? id_ex.pc : regFile.read(id_ex.rs1);
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
        }
        else if (id_ex.opcode == 0x67){
            ALUCtrl = 2; // JALR
            ex_mem.jump = (regFile.read(id_ex.rs1) + op2) & ~1;
            if_done = false;
        }
        if(id_ex.opcode != 0x13 && (id_ex.opcode ==0x13 && id_ex.funct3 != 0x1 && id_ex.funct3 != 0x5)){
            ex_mem.aluResult = alu.execute(op1, op2, ALUCtrl);
            if_done = false;
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