#ifndef NOFORWARDING_HPP
#define NOFORWARDING_HPP

#include "processor.hpp"

class NoForwardingProcessor : public Processor {
public:
    using Processor::Processor;

    void executeStage() override {
        int op1 = regFile.read(id_ex.rs1);
        int op2 = id_ex.control.ALUSrc ? id_ex.imm : regFile.read(id_ex.rs2);

        int ALUCtrl = determineALUControl(id_ex);
        ex_mem.aluResult = alu.execute(op1, op2, ALUCtrl);

        ex_mem.rd = id_ex.rd;
        ex_mem.control = id_ex.control;

        logStallWarnings(op1, op2);
    }

private:
    int determineALUControl(const ID_EX& stage) {
        switch(stage.opcode) {
            case 0x03: return 2;  // Load
            case 0x23: return 2;  // Store
            case 0x63: return 6;  // Branch
            case 0x33:
                if (stage.funct3 == 0x0) {
                    return (stage.funct7 == 0x00) ? 2 : 6;  // ADD/SUB
                }
                if (stage.funct3 == 0x7) return 0;  // AND
                if (stage.funct3 == 0x6) return 1;  // OR
                break;
            default: return 0;
        }
        return 0;
    }

    //------------------------Logging potential stall warnings------------------------
    void logStallWarnings(int op1, int op2) {
        std::cerr << "No Forwarding Potential Stall Warning:\n";
        std::cerr << "Operand 1 Value: " << op1 << "\n";
        std::cerr << "Operand 2 Value: " << op2 << "\n";
    }
};

#endif // NOFORWARDING_HPP