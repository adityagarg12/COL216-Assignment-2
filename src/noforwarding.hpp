
#include "processor.hpp"

class NoForwardingProcessor : public Processor {
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

        int op1 = regFile.read(id_ex.rs1);
        int op2 = id_ex.control.ALUSrc ? id_ex.imm : regFile.read(id_ex.rs2);

        
        
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
        
        }
         
        ex_mem.aluResult = alu.execute(op1, op2, ALUCtrl);
        ex_mem.zero = ex_mem.aluResult == 0;
        if (id_ex.control.Branch) {
            bool taken = (id_ex.funct3 == 0x0 && ex_mem.aluResult == 0) || // BEQ
                         (id_ex.funct3 == 0x1 && ex_mem.aluResult != 0);   // BNE
            if (taken) {
                if_id.pc = id_ex.pc + (id_ex.imm << 1);
                if_id.instruction = 0; // Flush IF/ID with NOP
                stall = false;
                stallIF = false; // Clear stalls
            }
        }
        ex_mem.rd = id_ex.rd;
        ex_mem.rs2 = id_ex.rs2;
        ex_mem.control = id_ex.control; 
        ex_mem.pc = id_ex.pc;
        if (ex_mem.pc/4 + 1>= numInstructions && ex_mem.pc/4 < numInstructions){
        vec[ex_mem.pc/4][cycle]= "EX";
        }
        ex_mem.perform = true; // TO BE CHECKED
    }
};