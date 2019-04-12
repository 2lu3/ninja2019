#ifndef LOG_ERROR_TOOLS

#include "CospaceSettings.hpp"

bool errorMessage(std::string message);
bool logMessage(std::string message);
bool errorMessage(std::string message, int option);
bool logMessage(std::string message, int option);
bool outputData(std::string file_name, std::string message);
bool outputData(std::string file_name, std::string message, int option);
void delErrorFile();
void delLogFile();
void delOutFile(std::string file_name);

#endif // !LOG_ERROR_TOOLS
