#include "stdafx.h"
#include <shellapi.h>
#include <errno.h>
#include <stdio.h>
#include <io.h>
#include <fcntl.h>
#include <iostream>
#include <string>
#include <cassert>
#include <boost/asio.hpp>
#include <boost/filesystem.hpp>
#include <boost/process/windows.hpp>
#include <boost/thread.hpp>

using namespace std;


#define BUFSIZE		4096

t_start_callback TTPDFWorker::_start_callback = 0;
t_work_status_callback TTPDFWorker::_status_callback = 0;
t_progress_callback TTPDFWorker::_progress_callback = 0;
t_finish_callback TTPDFWorker::_finish_callback = 0;

TTPDFWorker::TTPDFWorker()
{
	_process = 0;
}

TTPDFWorker::~TTPDFWorker()
{
}

int TTPDFWorker::_getPageCount(const string& input) {
	DWORD val = 0;
	Registry registry;
	if (registry.Open(HKEY_LOCAL_MACHINE, "SYSTEM\\CurrentControlSet\\Control\\Print\\Monitors\\" MONITOR_NAME "\\Ports\\" PORT_NAME) == ERROR_SUCCESS) {
		registry.Read("Pages", val);
		registry.Close();
	}
	return val;
}

bool TTPDFWorker::stdin2ps(const string& ps) {

	HANDLE hPSStdInFile;
	HANDLE hOutFile;

	char chBuf[BUFSIZE];
	DWORD dwRead, dwWritten;
	BOOL bSuccess = FALSE;

	hPSStdInFile = GetStdHandle(STD_INPUT_HANDLE);
	SetFilePointer(hPSStdInFile, 0, NULL, FILE_BEGIN);

	hOutFile = CreateFile(
		ps.c_str(),
		GENERIC_READ | GENERIC_WRITE,
		0,
		NULL,
		CREATE_ALWAYS,
		FILE_ATTRIBUTE_NORMAL,
		NULL
	);

	if (hOutFile == INVALID_HANDLE_VALUE) {
		return false;
	}

	for (;;)
	{
		bSuccess = ReadFile(hPSStdInFile, chBuf, BUFSIZE, &dwRead, NULL);
		if (!bSuccess || dwRead == 0) break;

		bSuccess = WriteFile(hOutFile, chBuf,
			dwRead, &dwWritten, NULL);
		if (!bSuccess) break;
	}

	CloseHandle(hOutFile);
	
	return true;
}

void TTPDFWorker::setCallbacks(t_start_callback start, t_work_status_callback _status, t_progress_callback progress, t_finish_callback finish) {
	_start_callback = start;

	_status_callback = _status;

	_progress_callback = progress;

	_finish_callback = finish;
}

void TTPDFWorker::ps2pdf(const string& input, const string& output) {

	int totalpage = _getPageCount(input);
	
	if (!_start_callback.empty()) {
		boost::filesystem::path input_path(output);
		_start_callback(TTUtil::getDocumentFilename(), input_path.filename().string(), input_path.parent_path().string(), totalpage);
	}
	
	string app_dir = TTUtil::currentPath();

	string command;

	command = "\"" + app_dir + "\\tools\\gs9.16\\bin\\gswin32c.exe" + "\"";
	command += " -q -dNOPAUSE -dBATCH -dSAFER -sDEVICE=pdfwrite";
	command += " -sOutputFile=\"" + output + "\"";
	command += " -IC=\"" + app_dir + "\\tools\\gs9.16\\font;" + app_dir + "\\tools\\gs9.16\\lib;" + "\"";
	command += " -c.setpdfwrite";
	command += " -f \"" + input + "\"";

	ipstream pipe_stream;
	child c(command, std_out > pipe_stream, boost::process::windows::hide);

	_process = &c;

	std::string line, page;

	int nFound;
	int currentpage = 0, lastpage = 0;

	fprintf(stdout, "ps2pdf|ing|0/%d\n", totalpage);
	fflush(stdout);

	while (pipe_stream && std::getline(pipe_stream, line) && !line.empty()) {
		nFound = line.find("Page: ");
		if (nFound != -1) {
			page = line.substr(nFound + 6, line.find("]") - (nFound + 6));
			TTUtil::trim(page);
			
			currentpage = ::_ttoi(page.c_str());
			if (currentpage > lastpage) {

				fprintf(stdout, "ps2pdf|ing|%d/%d\n", currentpage, totalpage);
				fflush(stdout);

				if (!_progress_callback.empty()) {
					_progress_callback(::_ttoi(page.c_str()), totalpage);
				}
			}
			lastpage = currentpage;
		}
	}

	fprintf(stdout, "ps2pdf|done\n");
	fflush(stdout);

	c.wait();

	if (!_finish_callback.empty()) {
		_finish_callback();
	}
	
	Registry registry;
	if (registry.Open(HKEY_LOCAL_MACHINE, "SYSTEM\\CurrentControlSet\\Control\\Print\\Monitors\\" MONITOR_NAME "\\Ports\\" PORT_NAME) == ERROR_SUCCESS) {
		registry.Write("Silent", (DWORD)0);
		registry.Write("Postscript", "");
		registry.Write("Document", "");
		registry.Close();
	}

	_process = 0;
}

void TTPDFWorker::stop() {
	if (_process) {
		_canceled = true;
		_process->terminate();
	}
}