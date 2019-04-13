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
		cerr << "writeErrorMessage() : Failed to open log file" << endl;
		return false;
	}
	error_file << createMessage(message) << endl;
	return true;
}

// mode = default mode
bool errorMessage(string message)
{
	if (getDefaultRunMode() <= getRunMode())
	{
		logMessage(message);
		bool result = writeErrorMessage(message);
		// log message will do it
		// if (MODE_DEBUG <= getRunMode())
		// {
		// 	cout << createMessage(message) << endl;
		// }
		return result;
	}
	return true;
}

// mode = option
bool errorMessage(string message, Mode option)
{
	if (option <= getRunMode())
	{
		logMessage(message, option);
		bool result = writeErrorMessage(message);
		// logMessage will do it
		// if (MODE_DEBUG <= getRunMode())
		// {
		// 	cout << createMessage(message) << endl;
		// }
		return result;
	}
	return true;
}

bool writeLogMessage(string message)
{
	ofstream log_file("./" + log_file_name, ios::app);
	if (!log_file)
	{
		cerr << "LogMessage() : Failed to open log file" << endl;
		return false;
	}
	log_file << createMessage(message) << endl;
	return true;
}

// mode = default mode
bool logMessage(string message)
{
	if (getDefaultRunMode() <= getRunMode())
	{
		bool result = writeLogMessage(message);
		if (MODE_DEBUG <= getRunMode())
		{
			cout << createMessage(message) << endl;
		}
		return result;
	}
	return true;
}

bool logMessage(string message, Mode option)
{
	if (option <= getRunMode())
	{
		bool result = writeLogMessage(message);
		if (MODE_DEBUG <= getRunMode())
		{
			cout << createMessage(message) << endl;
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
				 << " " << file_name << " " << message << endl;
		return false;
	}
	if (getDefaultRunMode() >= MODE_DEBUG)
	{
		cout << message << endl;
	}
	output_file << message << endl;
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
		cout << message << endl;
	}
	return true;
}
