#ifndef FORWARDING_HPP
#define FORWARDING_HPP

#include "processor.hpp"

class ForwardingProcessor : public Processor {
private:
    enum ForwardingUnit {
        NO_FORWARD,
        FROM_EX_MEM,
        FROM_MEM_WB
    };

    int getForwardedValue(int rs, ForwardingUnit& forwardUnit) {
        forwardUnit = NO_FORWARD;
        return 0;  // Placeholder for actual forwarding logic
    }

public:
    using Processor::Processor;

    void executeStage() override {
        ForwardingUnit forwardUnit1 = NO_FORWARD, forwardUnit2 = NO_FORWARD;

        int op1 = getForwardedValue(id_ex.rs1, forwardUnit1);
        int op2 = id_ex.control.ALUSrc ? id_ex.imm : getForwardedValue(id_ex.rs2, forwardUnit2);

        int ALUCtrl = determineALUControl(id_ex);
        ex_mem.aluResult = alu.execute(op1, op2, ALUCtrl);

        ex_mem.rd = id_ex.rd;
        ex_mem.control = id_ex.control;

        logForwardingDetails(forwardUnit1, forwardUnit2);
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

    //------------------------Logging forwarding details------------------------
    void logForwardingDetails(ForwardingUnit unit1, ForwardingUnit unit2) {
        std::cerr << "Forwarding Details:\n";
        std::cerr << "RS1 Forwarding: " << (unit1 != NO_FORWARD ? "Enabled" : "Disabled") << "\n";
        std::cerr << "RS2 Forwarding: " << (unit2 != NO_FORWARD ? "Enabled" : "Disabled") << "\n";
    }
};

#endif // FORWARDING_HPP