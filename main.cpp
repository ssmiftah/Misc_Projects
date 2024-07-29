#include <iostream>
#include <fstream>

int main() {
    std::string input;
    int totalNumCharacters;
    std::string outputTxt;
    int remainingNumCharacters;

    // Get user input for total number of characters
    std::cout << "Enter the total number of characters: ";
    std::cin >> totalNumCharacters;
    

    // Get user input
    std::cout << "Enter a string: ";
    std::cin.ignore();  // Ignore the newline character in the input buffer
    std::getline(std::cin, input);

    // Remove spaces from the start and end of the input string
    input.erase(0, input.find_first_not_of(" "));
    input.erase(input.find_last_not_of(" ") + 1);

    // Count the number of characters in the input string
    int numCharacters = input.size();
    remainingNumCharacters = (totalNumCharacters - numCharacters - 4)/2;
    std::cout << "Remaining number of characters: " << remainingNumCharacters << std::endl;

    // Create the output string
    outputTxt = "% ";
    if (remainingNumCharacters%2 == 0) {
        outputTxt += std::string(remainingNumCharacters, '=') + " " + input + " " + std::string(remainingNumCharacters, '=');
    } else
    {
        outputTxt += std::string(remainingNumCharacters, '=') + " " + input + " " + std::string(remainingNumCharacters + 1, '=');
    }

    // Print the number of characters
    std::cout << "Number of characters in the input: " << numCharacters << std::endl;

    // Create and open the output file
    std::ofstream outputFile("output.txt");

    // Check if the file was opened successfully
    if (!outputFile) {
        std::cerr << "Failed to open the output file." << std::endl;
        return 1;
    }

    // Write the Output Text to the file
    outputFile << outputTxt;

    // Close the file
    outputFile.close();

    std::cout << "Input stored in output.txt" << std::endl;

    return 0;
}