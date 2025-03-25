#include "processor.hpp"

class ForwardingProcessor : public Processor {
public:
    using Processor::Processor;

    void executeStage() override {
        int op1 = regFile.read(id_ex.rs1);
        int op2 = id_ex.control.ALUSrc ? id_ex.imm : regFile.read(id_ex.rs2);

        
        // Forwarding logic
        if (ex_mem.control.RegWrite && ex_mem.rd == id_ex.rs1) op1 = ex_mem.aluResult;
        if (ex_mem.control.RegWrite && ex_mem.rd == id_ex.rs2) op2 = ex_mem.aluResult;

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
        if (ex_mem.zero == 1 && id_ex.control.Branch == 1) {
            if_id.pc = id_ex.pc - 4 + (id_ex.imm << 1);
            
            // ex_mem = EX_MEM();
        }
    //    ex_mem.zero = ex_mem.aluResult == 0;
        ex_mem.rd = id_ex.rd;
        ex_mem.rs2 = id_ex.rs2;
        ex_mem.control = id_ex.control; // Pass control signals forward

    }
};