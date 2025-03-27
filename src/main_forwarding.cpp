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
    // Find the last path separator ('/' or '\')
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

    // Vector to store the machine code instructions
    std::vector<uint32_t> instructions;

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
        std::string machineCodeStr;

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
    }

    // Close the input file
    inputFile.close();

    // Check if any instructions were loaded
    if (instructions.empty()) {
        std::cerr << "Error: No instructions loaded from the input file.\n";
        return 1;
    }

    // Print the loaded instructions (this will go to the output file)
    std::cout << "Loaded Instructions (Machine Code):\n";
    for (const auto& instr : instructions) {
        std::cout << std::hex << "0x" << instr << "\n";
    }
    std::cout << std::dec << "\n";

    // Create the processor with the loaded instructions
    ForwardingProcessor forwardProc(instructions);

    std::cout << "Running Forwarding Processor for " << cycleCount << " cycles:\n";
    forwardProc.runSimulation(cycleCount);

    // Close the redirected stdout
    fclose(stdout);

    return 0;
}