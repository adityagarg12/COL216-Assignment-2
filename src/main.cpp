#include "processor.hpp"
#include "noforwarding.hpp"
// #include "forwarding.hpp"

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

    NoForwardingProcessor noForwardProc(instructions);
    // ForwardingProcessor forwardProc(instructions);

    std::cout << "Running No Forwarding Processor:\n";
    noForwardProc.runSimulation(20);

    // std::cout << "\nRunning Forwarding Processor:\n";
    // forwardProc.runSimulation(10);

    return 0;
}


// #include <fstream>  // For file I/O
// #include <sstream>  // For string parsing
// #include <stdexcept> // For error handling
// #include "processor.hpp"
// #include "noforwarding.hpp"

// int main(int argc, char* argv[]) {
//     // Check if the correct number of arguments is provided
//     if (argc != 3) {
//         std::cerr << "Usage: " << argv[0] << " <input_file_path> <cycle_count>\n";
//         std::cerr << "Example: " << argv[0] << " ../inputfiles/filename.txt 25\n";
//         return 1;
//     }

//     // Parse command-line arguments
//     std::string inputFilePath = argv[1];  // Path to the input file
//     int cycleCount;

//     // Convert cycle count from string to integer
//     try {
//         cycleCount = std::stoi(argv[2]);
//         if (cycleCount <= 0) {
//             std::cerr << "Error: Cycle count must be a positive integer.\n";
//             return 1;
//         }
//     } catch (const std::exception& e) {
//         std::cerr << "Error: Invalid cycle count '" << argv[2] << "'. Must be a positive integer.\n";
//         return 1;
//     }

//     // Vector to store the machine code instructions
//     std::vector<uint32_t> instructions;

//     // Open the input file
//     std::ifstream inputFile(inputFilePath);
//     if (!inputFile.is_open()) {
//         std::cerr << "Error: Could not open input file at " << inputFilePath << "\n";
//         return 1;
//     }

//     // Read the file line by line
//     std::string line;
//     while (std::getline(inputFile, line)) {
//         // Skip empty lines
//         if (line.empty()) continue;

//         // Use stringstream to parse the line
//         std::stringstream ss(line);
//         std::string machineCodeStr;

//         // Read the first column (machine code in hex)
//         ss >> machineCodeStr;

//         // Convert the hexadecimal string to uint32_t
//         try {
//             uint32_t machineCode = std::stoul(machineCodeStr, nullptr, 16);
//             instructions.push_back(machineCode);
//         } catch (const std::exception& e) {
//             std::cerr << "Error: Invalid machine code format in line: " << line << "\n";
//             inputFile.close();
//             return 1;
//         }

//         // Ignore the rest of the line (instruction description)
//     }

//     // Close the file
//     inputFile.close();

//     // Check if any instructions were loaded
//     if (instructions.empty()) {
//         std::cerr << "Error: No instructions loaded from the input file.\n";
//         return 1;
//     }

//     // Print the loaded instructions for verification
//     std::cout << "Loaded Instructions (Machine Code):\n";
//     for (const auto& instr : instructions) {
//         std::cout << std::hex << "0x" << instr << "\n";
//     }
//     std::cout << std::dec << "\n"; // Reset to decimal output

//     // Create the processor with the loaded instructions
//     NoForwardingProcessor noForwardProc(instructions);

//     std::cout << "Running No Forwarding Processor for " << cycleCount << " cycles:\n";
//     noForwardProc.runSimulation(cycleCount);

//     return 0;
// }