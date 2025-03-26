// #include "processor.hpp"
#include "processor_forwarding.hpp"
// #include "noforwarding.hpp"
#include "forwarding.hpp"

int main(int argc, char* argv[]) {
    std::vector<uint32_t> instructions = {
        0x00000293,  // addi x5, x0, 0
        0x00a28333,  // add x6, x5, x10
        0x00030303,  // lb x6, 0(x6)
        0x00030663,   // beq x6, x0, 12
        0x00128293,     //addi x5 x5 1
        0xff1ff06f, //
        0x00028513,
        0x00008067
    };

    std::vector<uint32_t> instructions2 = {
        0x00058283,  // addi x5, x0, 0
        0x00550023,  // add x6, x5, x10
        0x00028863,  // lb x6, 0(x6)
        0x00150513,   // beq x6, x0, 12
        0x00158593,     //addi x5 x5 1
        0xff1ff06f, //
        0xfedff06f,
        0x00008067
    };

    

    // NoForwardingProcessor noForwardProc(instructions);
    ForwardingProcessor forwardProc(instructions);

    // std::cout << "Running No Forwarding Processor:\n";
    // noForwardProc.runSimulation(25);

    std::cout << "\nRunning Forwarding Processor:\n";
    forwardProc.runSimulation(15);

    return 0;

}
