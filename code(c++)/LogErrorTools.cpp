#include "LogErrorTools.hpp"
#include "CommonTools.hpp"

using namespace std;

string error_file_name("error_file.txt");
string log_file_name("log_file.txt");

void delErrorFile()
{
	ofstream error_file("./" + error_file_name);
}

void delLogFile()
{
	ofstream log_file("./" + log_file_name);
}

void delOutFile(string file_name)
{
	ofstream out_file("./" + file_name);
}

string createMessage(string message)
{
	message = to_string(getRepeatedNum()) + " " + message;
	return message;
}

bool writeErrorMessage(string message)
{
	ofstream error_file("./" + error_file_name, ios::app);
	if (!error_file)
	{
		cerr << "writeErrorMessage() : Failed to open log file"
			 << "\n";
		return false;
	}
	error_file << createMessage(message) << "\n";
	return true;
}

// mode = default mode
bool errorMessage(string message)
{
	return errorMessage(message, getDefaultRunMode());
}

// mode = option
bool errorMessage(string message, Mode option)
{
	if (option <= getRunMode())
	{
		logMessage(message, option);
		if (getIsOutputErrorMessage2Console())
		{
			cout << createMessage(message) << "\n";
		}
		return writeErrorMessage(message);
	}
	return true;
}

bool writeLogMessage(string message)
{
	ofstream log_file("./" + log_file_name, ios::app);
	if (!log_file)
	{
		cerr << "LogMessage() : Failed to open log file"
			 << "\n";
		return false;
	}
	log_file << createMessage(message) << "\n";
	return true;
}

// mode = default mode
bool logMessage(string message)
{
	return logMessage(message, getDefaultRunMode());
}

bool logMessage(string message, Mode option)
{
	if (option <= getRunMode())
	{
		bool result = writeLogMessage(message);
		if (getIsOutputLogMessage2Console())
		{
			cout << createMessage(message) << "\n";
		}
		return result;
	}
	return true;
}

bool outputData(string file_name, string message)
{
	if (getDefaultRunMode() < getRunMode())
	{
		return true;
	}
	ofstream output_file("./" + file_name, ios::app);
	if (!output_file)
	{
		cerr << "outputData() : Failed to output data file"
			 << " " << file_name << " " << message << "\n";
		return false;
	}
	if (getDefaultRunMode() >= MODE_DEBUG)
	{
		cout << message << "\n";
	}
	output_file << message << "\n";
	return true;
}

bool outputData(string file_name, string message, Mode option)
{
	if (option < getRunMode())
	{
		return true;
	}
	bool result = outputData(file_name, message);
	if (getDefaultRunMode() < MODE_DEBUG && option >= MODE_DEBUG)
	{
		cout << message << "\n";
	}
	return result;
}
