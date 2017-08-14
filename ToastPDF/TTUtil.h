#pragma once

#include <string>

using namespace std;

class TTUtil
{
public:
	TTUtil();
	~TTUtil();

	static string currentPath();

	static string getModuleFileName();

	static string getRandomString();

	static string getOutputPostscript();

	static bool silent();

	static void reset();

	static string getOutputFromSaveDialog(HINSTANCE hInstance);
	
	static string getDocumentFilename(bool removeDocType = false);

	static string getDocumentFilenameWithFileExtension(const string& fileExtension, bool removeDocType = false);
	
	static void trim(std::string &s);
};

