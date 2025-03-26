#include "processor.hpp"
#include "noforwarding.hpp"
#include "forwarding.hpp"

int main(int argc, char* argv[]) {
    std::vector<uint32_t> instructions = {
        0x00000293,  // addi x5, x0, 0
        0x00a28333,  // add x6, x5, x10
        0x00030303,  // lb x6, 0(x6)
        0x00030663   // beq x6, x0, 12
    };

    NoForwardingProcessor noForwardProc(instructions);
    // ForwardingProcessor forwardProc(instructions);

    std::cout << "Running No Forwarding Processor:\n";
    noForwardProc.runSimulation(15);

    // std::cout << "\nRunning Forwarding Processor:\n";
    // forwardProc.runSimulation(10);

    return 0;
    
}
