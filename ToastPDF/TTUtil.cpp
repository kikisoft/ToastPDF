#include "stdafx.h"
#include <algorithm> 
#include <functional> 
#include <cctype>
#include <boost/filesystem.hpp>
#include <boost/random/random_device.hpp>
#include <boost/random/uniform_int_distribution.hpp>
#include "TTUtil.h"

TTUtil::TTUtil()
{
	
}

TTUtil::~TTUtil()
{
}

string TTUtil::getModuleFileName()
{
	char path[MAX_PATH];
	::GetModuleFileName(NULL, path, MAX_PATH);
	return std::tstring(path);
}

string TTUtil::currentPath() {
	boost::filesystem::path p(getModuleFileName());
	boost::filesystem::path workdir = p.parent_path();
	boost::filesystem::current_path(workdir.string());
	return workdir.string();
}

string TTUtil::getRandomString() {

	std::tstring name;
	std::tstring chars(
		/*"abcdefghijklmnopqrstuvwxyz"*/
		"1234567890");

	boost::random::random_device rng;
	boost::random::uniform_int_distribution<> index_dist(0, chars.size() - 1);
	for (int i = 0; i < 8; ++i) {
		name += chars[index_dist(rng)];
	}
	return name;
}

string TTUtil::getOutputPostscript() {
	Registry registry;
	if (registry.Open(HKEY_LOCAL_MACHINE, "SYSTEM\\CurrentControlSet\\Control\\Print\\Monitors\\" MONITOR_NAME "\\Ports\\" PORT_NAME) == ERROR_SUCCESS) {
		CString val;
		registry.Read("Postscript", val);
		registry.Close();
		return val.GetString();
	}

	return "";
}

bool TTUtil::silent() {
	Registry registry;
	if (registry.Open(HKEY_LOCAL_MACHINE, "SYSTEM\\CurrentControlSet\\Control\\Print\\Monitors\\" MONITOR_NAME "\\Ports\\" PORT_NAME) == ERROR_SUCCESS) {
		DWORD val;
		registry.Read("Silent", val);
		registry.Close();
		if (val == 1) return true;
	}

	return false;
}

void TTUtil::reset() {
	Registry registry;
	if (registry.Open(HKEY_LOCAL_MACHINE, "SYSTEM\\CurrentControlSet\\Control\\Print\\Monitors\\" MONITOR_NAME "\\Ports\\" PORT_NAME) == ERROR_SUCCESS) {
		registry.Write("Postscript", "");
		registry.Write("Silent", (DWORD)0);
		registry.Close();
	}
}

string TTUtil::getOutputFromSaveDialog(HINSTANCE hInstance) {
	char cPath[MAX_PATH + 1] = {0};
	OPENFILENAME info;

	strcpy(cPath, getDocumentFilenameWithFileExtension(".pdf", true).c_str());

	memset(&info, 0, sizeof(info));
	info.lStructSize = sizeof(info);
	info.hInstance = hInstance;
	info.lpstrFilter = "PDF Files (*.pdf)\0*.pdf\0All Files (*.*)\0*.*\0\0";
	info.lpstrFile = cPath;
	info.nMaxFile = MAX_PATH + 1;
	info.lpstrTitle = "저장할 PDF 파일이름을 입력하세요";
	info.Flags = OFN_ENABLESIZING | OFN_EXPLORER | OFN_NOREADONLYRETURN | OFN_OVERWRITEPROMPT | OFN_PATHMUSTEXIST;
	info.lpstrDefExt = "pdf";

	if (GetSaveFileName(&info)) {
		return string(cPath);
	}
	else {
		return "";
	}
}

string TTUtil::getDocumentFilename(bool removeDocType) {
	CString sDocumentname;

	Registry registry;
	if (registry.Open(HKEY_LOCAL_MACHINE, "SYSTEM\\CurrentControlSet\\Control\\Print\\Monitors\\" MONITOR_NAME "\\Ports\\" PORT_NAME) == ERROR_SUCCESS) {
		registry.Read("Document", sDocumentname);
		registry.Close();

		if (removeDocType) {
			int nFound = sDocumentname.Find(" - ");
			sDocumentname = sDocumentname.Mid(nFound + 3);
		}
	}

	return sDocumentname.GetString();
}

string TTUtil::getDocumentFilenameWithFileExtension(const string& fileExtension, bool removeDocType) {
	string sDocumentname = getDocumentFilename(removeDocType);
	int nFound = sDocumentname.rfind('.');
	if (nFound != -1) {
		sDocumentname = sDocumentname.substr(0, nFound) + fileExtension;
	}
	return sDocumentname;
}

void TTUtil::trim(std::string &s) {
	s.erase(s.begin(), std::find_if(s.begin(), s.end(),
		std::not1(std::ptr_fun<int, int>(std::isspace))));

	s.erase(std::find_if(s.rbegin(), s.rend(),
		std::not1(std::ptr_fun<int, int>(std::isspace))).base(), s.end());
}