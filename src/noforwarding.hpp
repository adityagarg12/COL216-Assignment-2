#include "processor.hpp"

class NoForwardingProcessor : public Processor {
public:
    using Processor::Processor;  // Inherit constructor

    void executeStage() override {
        ex_mem.aluResult = alu.execute(id_ex.rs1, id_ex.imm, '+'); // Example: ADDI instruction
        ex_mem.rd = id_ex.rd;
        ex_mem.regWrite = true;
    }
};
