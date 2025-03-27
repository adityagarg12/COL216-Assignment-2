// // #include "processor.hpp"
// #include "processor_forwarding.hpp"
// // #include "noforwarding.hpp"
// #include "forwarding.hpp"

// int main(int argc, char* argv[]) {
//     std::vector<uint32_t> instructions = {
//         0x00000293,  // addi x5, x0, 0
//         0x00a28333,  // add x6, x5, x10
//         0x00030303,  // lb x6, 0(x6)
//         0x00030663,   // beq x6, x0, 12
//         0x00128293,     //addi x5 x5 1
//         0xff1ff06f, //
//         0x00028513,
//         0x00008067
//     };

//     std::vector<uint32_t> instructions2 = {
//         0x00058283,  // addi x5, x0, 0
//         0x00550023,  // add x6, x5, x10
//         0x00028863,  // lb x6, 0(x6)
//         0x00150513,   // beq x6, x0, 12
//         0x00158593,     //addi x5 x5 1
//         0xff1ff06f, //
//         0xfedff06f,
//         0x00008067
//     };

//     std::vector<uint32_t> instructions3 = {
//         0x00000293,        
//     0x02c2d063       ,
// 0x00558333       ,
// 0x00030303        ,
// 0x00030a63        ,
// 0x005503b3        ,
// 0x00638023       ,
// 0x00128293       ,
// 0xfe5ff06f        ,
// 0x00c2da63        ,
// 0x00550333       ,
// 0x00030023       ,
// 0x00128293        ,
// 0xff1ff06f        ,
// 0x00008067        
// };

    

//     // NoForwardingProcessor noForwardProc(instructions);
//     ForwardingProcessor forwardProc(instructions3);

//     // std::cout << "Running No Forwarding Processor:\n";
//     // noForwardProc.runSimulation(25);

//     std::cout << "\nRunning Forwarding Processor:\n";
//     forwardProc.runSimulation(20);

//     return 0;

// }

#include <fstream>
#include <sstream>
#include <stdexcept>
#include "processor_forwarding.hpp"
#include "forwarding.hpp"

int main(int argc, char* argv[]) {
    // Check if the correct number of arguments is provided
    if (argc != 3) {
        std::cerr << "Usage: " << argv[0] << " <input_file_path> <cycle_count>\n";
        std::cerr << "Example: " << argv[0] << " ../inputfiles/filename.txt 25\n";
        return 1;
    }

    // Parse command-line arguments
    std::string inputFilePath = argv[1];  // Path to the input file
    int cycleCount;

    // Convert cycle count from string to integer
    try {
        cycleCount = std::stoi(argv[2]);
        if (cycleCount <= 0) {
            std::cerr << "Error: Cycle count must be a positive integer.\n";
            return 1;
        }
    } catch (const std::exception& e) {
        std::cerr << "Error: Invalid cycle count '" << argv[2] << "'. Must be a positive integer.\n";
        return 1;
    }

    // Extract the base filename using std::string operations
    size_t lastSlash = inputFilePath.find_last_of("/\\");
    std::string filename = (lastSlash == std::string::npos) ? inputFilePath : inputFilePath.substr(lastSlash + 1);

    // Remove the extension (e.g., ".txt")
    size_t lastDot = filename.find_last_of('.');
    std::string baseFilename = (lastDot == std::string::npos) ? filename : filename.substr(0, lastDot);

    // Construct the output file path
    std::string outputFilePath = "../outputfiles/" + baseFilename + "_forward_out.txt";

    // Redirect stdout to the output file
    if (!freopen(outputFilePath.c_str(), "w", stdout)) {
        std::cerr << "Error: Could not redirect stdout to " << outputFilePath << "\n";
        return 1;
    }

    // Vectors to store the machine code instructions and assembly instructions
    std::vector<uint32_t> instructions;
    std::vector<std::string> assemblyInstructions;

    // Open the input file
    std::ifstream inputFile(inputFilePath);
    if (!inputFile.is_open()) {
        std::cerr << "Error: Could not open input file at " << inputFilePath << "\n";
        return 1;
    }

    // Read the file line by line
    std::string line;
    while (std::getline(inputFile, line)) {
        // Skip empty lines
        if (line.empty()) continue;

        // Use stringstream to parse the line
        std::stringstream ss(line);
        std::string machineCodeStr, assemblyInstr;

        // Read the first column (machine code in hex)
        ss >> machineCodeStr;

        // Convert the hexadecimal string to uint32_t
        try {
            uint32_t machineCode = std::stoul(machineCodeStr, nullptr, 16);
            instructions.push_back(machineCode);
        } catch (const std::exception& e) {
            std::cerr << "Error: Invalid machine code format in line: " << line << "\n";
            inputFile.close();
            return 1;
        }

        // Read the rest of the line as the assembly instruction
        std::getline(ss, assemblyInstr);
        // Trim leading whitespace from the assembly instruction
        size_t firstNonSpace = assemblyInstr.find_first_not_of(" \t");
        if (firstNonSpace != std::string::npos) {
            assemblyInstr = assemblyInstr.substr(firstNonSpace);
        } else {
            assemblyInstr = ""; // Handle case where the rest of the line is just whitespace
        }

        // Store the assembly instruction in the vector
        assemblyInstructions.push_back(assemblyInstr);
    }

    // Close the input file
    inputFile.close();

    // Check if any instructions were loaded
    if (instructions.empty()) {
        std::cerr << "Error: No instructions loaded from the input file.\n";
        return 1;
    }

    // Ensure the assembly instructions vector matches the machine code vector
    if (instructions.size() != assemblyInstructions.size()) {
        std::cerr << "Error: Mismatch between machine code and assembly instructions.\n";
        return 1;
    }

    // Print the loaded instructions (this will go to the output file)
    // std::cout << "Loaded Instructions:\n";
    // for (size_t i = 0; i < instructions.size(); ++i) {
    //     std::cout << std::hex << "0x" << instructions[i] << "  " << assemblyInstructions[i] << "\n";
    // }
    // std::cout << std::dec << "\n";

    // Create the processor with the loaded instructions and assembly instructions
    ForwardingProcessor ForwardProc(instructions);

    // std::cout << "Running No Forwarding Processor for " << cycleCount << " cycles:\n";
    ForwardProc.runSimulation(cycleCount, assemblyInstructions);

    // Close the redirected stdout
    fclose(stdout);

    return 0;
}