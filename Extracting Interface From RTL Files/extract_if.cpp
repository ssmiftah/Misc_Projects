#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <regex>
#include <algorithm>



class port_dets {
public:
    std::string port_name;
    std::string port_type;
    size_t port_size;
    bool port_empty = true;

    void print() {
        std::cout << "Port name: " << port_name << std::endl;
        std::cout << "Port type: " << port_type << std::endl;
        std::cout << "Port size: " << port_size << std::endl;
    }


    port_dets& operator=(const port_dets& other) {                                      // copy assignment
        if (this != &other) {
            port_name = other.port_name;
            port_type = other.port_type;
            port_size = other.port_size;
            port_empty = other.port_empty;
        }
        return *this;
    }

    port_dets() : port_name(""), port_type(""), port_size(0), port_empty(true) {}       // default constructor
};

std::string stripCommentsAndSpaces(const std::string& line) {
    std::string strippedLine = line;
    // Remove comments
    size_t commentPos = strippedLine.find("//");
    if (commentPos != std::string::npos) {
        strippedLine = strippedLine.substr(0, commentPos);                              // Remove everything after the comment
    }
    // Remove trailing spaces
    strippedLine.erase(std::find_if(strippedLine.rbegin(), strippedLine.rend(), [](unsigned char ch) {  // Remove trailing spaces
        return !std::isspace(ch);
    }).base(), strippedLine.end());
    return strippedLine;
}

std::string removeLineBreaks(const std::string& inputText) {

    std::string line;
    std::string content;
    bool firstLine = true;

    std::istringstream inputTextStream(inputText);

    while (std::getline(inputTextStream, line)) {
        std::string strippedLine = stripCommentsAndSpaces(line);
        if (!firstLine) {
            if (content.back() != ';') {
                content += " ";  // Add a space if the previous line did not end with a semicolon
            } else {
                content += "\n";  // Add a newline if the previous line ended with a semicolon
            }
        }
        content += line;
        firstLine = false;
    }

    return content;
}

std::string removeComments(const std::string& inputFilename) {

    std::string output = "";

    std::ifstream inputFile(inputFilename);
    if (!inputFile.is_open()) {
        std::cerr << "Error opening input file: " << inputFilename << std::endl;
        return output;
    }

    std::string content((std::istreambuf_iterator<char>(inputFile)), std::istreambuf_iterator<char>());

    // Remove single-line comments
    content = std::regex_replace(content, std::regex("//.*"), "");

    // Remove multi-line comments
    content = std::regex_replace(content, std::regex("/\\*.*?\\*/", std::regex::extended), "");

    // Remove leading and trailing spaces
    std::istringstream iss(content);
    std::ostringstream oss;
    std::string line;
    while (std::getline(iss, line)) {
        // Remove leading spaces
        line.erase(line.begin(), std::find_if(line.begin(), line.end(), [](unsigned char ch) {
            return !std::isspace(ch);   // Find the first non-space character
        }));
        // Remove trailing spaces
        line.erase(std::find_if(line.rbegin(), line.rend(), [](unsigned char ch) {
            return !std::isspace(ch);   // Find the first non-space character from the end
        }).base(), line.end());
        oss << line << '\n';
    }
    content = oss.str();

    output = content;  // Write the content to the output file

    return output;
}

std::string parseAndAddNewLines(const std::string& inputText) {
    std::string line;
    std::istringstream inputTextStream(inputText);
    std::string output;
    

    /**
     * @brief Defines a regular expression to match various Verilog keywords and constructs.
     * 
     * This regex pattern matches the following Verilog constructs:
     * - `case` statements
     * - `if` statements
     * - `else if` statements
     * - `else` statements
     * - `always` blocks with sensitivity lists
     * - `always_ff` blocks with sensitivity lists
     * - `always_comb` blocks
     * - `begin` keywords
     * - `end` keywords
     * - `endcase` keywords
     */
    std::regex keywordRegex(R"((case\s*\(.*\)|if\s*\(.*\)|else\s*if\s*\(.*\)|else\s*\(.*\)|always\s*@\(.*\)|always_ff\s*@\(.*\)|always_comb\s+|begin\s+|end\s+|endcase\s+))");
    std::smatch match;

    while (std::getline(inputTextStream, line)) {
        std::string modifiedLine;
        size_t startPos = 0;

        std::string tempLine = line.substr(startPos);


        while (std::regex_search(tempLine, match, keywordRegex)) {
            // Find the position of the match in the original line
            size_t matchPos = match.position(0) + startPos;
            
            // Append the substring from startPos to the end of the match, followed by a newline
            modifiedLine += line.substr(startPos, matchPos - startPos + match.length(0)) + "\n";
            
            // Remove leading whitespace characters from modifiedLine
            modifiedLine.erase(modifiedLine.begin(), std::find_if(modifiedLine.begin(), modifiedLine.end(), [](unsigned char ch) {
                return !std::isspace(ch);
            }));
            
            // Update startPos to the position after the current match
            startPos = matchPos + match.length(0);
            
            // Update tempLine to the substring starting from the new startPos
            tempLine = line.substr(startPos);
        }
        
        // Remove leading whitespace characters from the string 'line'
        line.erase(
            line.begin(), // Start erasing from the beginning of the string
            std::find_if(
                line.begin(), // Start searching from the beginning of the string
                line.end(),   // Search until the end of the string
                [](unsigned char ch) { // Lambda function to check each character
                    return !std::isspace(ch); // Return true if the character is not a whitespace
                }
            )
        );

        modifiedLine += line.substr(startPos);                          // Append the remaining part of the line to modifiedLine
        // Remove leading whitespace from the string 'modifiedLine'
        modifiedLine.erase(
            // Define the range to erase: from the beginning of the string to the first non-whitespace character
            modifiedLine.begin(), 
            std::find_if(
                modifiedLine.begin(), 
                modifiedLine.end(), 
                // Lambda function to check if a character is not a whitespace
                [](unsigned char ch) {
                    return !std::isspace(ch);
                }
            )
        );
        output += modifiedLine + "\n";                                  // Append the modified line to the output string
    }

    return output;
}


port_dets port_det_extract(std::string inputText){
    std::regex port_format(R"((input|output|inout)\s+(\blogic|reg|bit\b)?\s+(\[(\d+):(\d+)\])?\s+(\w+)\s*(\[(\d+):(\d+)\])?)");
    std::smatch match;
    port_dets port;

    if(std::regex_search(inputText, match, port_format)){
        port.port_empty = false;            // port is not empty
        port.port_type = match.str(1);      // port type - matching group 1
        port.port_name = match.str(6);      // port name - matching group 6
        if (match.str(3) != ""){            // if port size is specified
            port.port_size = std::abs(std::stoi(match.str(4)) - std::stoi(match.str(5)) + 1);
        } else {
            port.port_size = 1;
        }
    }
    return port;
}


void print_port_det_vector(std::vector<port_dets> port){
    std::cout << "ports in this module are: " << std::endl;
    for (size_t i = 0; i < port.size(); i++){
        port[i].print();
        std::cout << std::endl;
    }
}


std::vector<port_dets> extract_IF(const std::string& inputText) {
    std::vector<port_dets> module_ports;
    std::string line;
    std::istringstream inputTextStream(inputText);
    
    std::regex moduleRegex(R"(module\s+(\w+)\s*\((.*)\);)");
    std::smatch match;

    while (std::getline(inputTextStream, line)) {                   // Read each line from the input stream
    if (std::regex_search(line, match, moduleRegex)){               // Check if the line matches the module regex pattern
        std::cout << "Module name: " << match.str(1) << std::endl;  // Print the module name (first capture group)
        int startPos = 0; // Initialize the starting position for port extraction
        std::string ports = match.str(2); // Get the ports string (second capture group)
        port_dets temp; // Temporary variable to hold port details

        // Loop through the ports string to extract individual port definitions
        while (ports.find(",", startPos) != std::string::npos) {
            // Extract the substring representing a single port definition
            std::string port = ports.substr(startPos, ports.find(",", startPos) - startPos);
            // Update startPos to the position after the current comma
            startPos = ports.find(",", startPos) + 1;
            // Extract port details from the port definition string
            temp = port_det_extract(port);
            // If the port is not empty, add it to the module_ports vector
            if(!temp.port_empty) module_ports.push_back(temp);
        }

        // Extract the last port definition (after the last comma)
        temp = port_det_extract(ports.substr(startPos, ports.length() - startPos));
        if(!temp.port_empty) // If the port is not empty, add it to the module_ports vector
            module_ports.push_back(temp);
    }     
}

    return module_ports;
}


int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <input_file> [output_file]" << std::endl;
        return 1;
    }

    std::string inputFilename = argv[1];
    std::string outputFilename = (argc > 2) ? argv[2] : "output.v"; // Default output file name
    std::string Comment_removed;

    Comment_removed = removeComments(inputFilename);
    // std::cout << "Comments removed and content written to " << Comment_removed << std::endl;

    std::string NewLinesAdded = removeLineBreaks(Comment_removed);
    // std::cout << NewLinesAdded << std::endl;

    std::string formattedText = parseAndAddNewLines(NewLinesAdded);

    // Open the output file for writing
    std::ofstream outputFile(outputFilename);
    if (!outputFile.is_open()) {
        // If the file cannot be opened, print an error message and return with an error code
        std::cerr << "Error opening output file: " << outputFilename << std::endl;
        return 1;
    }
    // Write the formatted text to the output file
    outputFile << formattedText << std::endl;
    // Close the output file
    outputFile.close();

    print_port_det_vector(extract_IF(formattedText));

    return 0;
}
