#include "processor.hpp"

class ForwardingProcessor : public Processor {
public:
    using Processor::Processor;

    void executeStage() override {
        int op1 = regFile.read(id_ex.rs1);
        int op2 = id_ex.imm;

        // Forwarding logic
        if (ex_mem.regWrite && ex_mem.rd == id_ex.rs1) op1 = ex_mem.aluResult;
        if (ex_mem.regWrite && ex_mem.rd == id_ex.rs2) op2 = ex_mem.aluResult;

        ex_mem.aluResult = alu.execute(op1, op2, '+');
        ex_mem.rd = id_ex.rd;
        ex_mem.regWrite = true;
    }
};
