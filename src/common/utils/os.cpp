#include "pch.h"
#include "os.h"

#pragma region External Dependencies

#include <filesystem>
#include <windows.h>
#include <commdlg.h>
#include <shlobj.h>

#pragma endregion

std::wstring stringToWString(const std::string& _str) {
	int size_needed = MultiByteToWideChar(CP_UTF8, 0, _str.c_str(), -1, nullptr, 0);
	std::wstring wstr(size_needed, 0);
	MultiByteToWideChar(CP_UTF8, 0, _str.c_str(), -1, &wstr[0], size_needed);
	return wstr;
}

std::string wstringToString(const std::wstring& _wstr) {
	int size_needed = WideCharToMultiByte(CP_UTF8, 0, _wstr.c_str(), -1, nullptr, 0, nullptr, nullptr);
	std::string str(size_needed, 0);
	WideCharToMultiByte(CP_UTF8, 0, _wstr.c_str(), -1, &str[0], size_needed, nullptr, nullptr);
	return str;
}

namespace angler::os {
	std::string getFileName(std::string _path, bool _extension) {
		std::string name = std::filesystem::path(_path).filename().string();
		if (!_extension) name = name.substr(0, name.find_last_of('.'));
		return name;
	}

	std::string getExtension(std::string _path, std::string _unknow) {
		std::string extension = std::filesystem::path(_path).extension().string();
		if (extension.empty()) return _unknow;
		return extension.substr(1);
	}

	std::string getAbsolutePath(std::string _path) {
		return std::filesystem::absolute(_path).generic_string();
	}

	std::string getStandardPath(std::string _path) {
		std::replace(_path.begin(), _path.end(), '\\', '/');
		return _path;
	}

	std::string getExecutablePath() {
		wchar_t path[MAX_PATH];
		DWORD length = GetModuleFileName(NULL, path, MAX_PATH);

		if (length == 0) {
			return "";
		}

		int bufferSize = WideCharToMultiByte(CP_UTF8, 0, path, -1, NULL, 0, NULL, NULL);
		if (bufferSize == 0) {
			return "";
		}

		std::string result(bufferSize - 1, '\0');
		WideCharToMultiByte(CP_UTF8, 0, path, -1, &result[0], bufferSize, NULL, NULL);

		result = getStandardPath(result);

		size_t position = result.find_last_of("/");
		if (position != std::string::npos) {
			result = result.substr(0, position);
		}

		return result;
	}

	std::string getFileInDirectory(std::string path, std::string file, char args) {
		std::filesystem::path directoryPath(path);
		for (const auto& entry : std::filesystem::recursive_directory_iterator(directoryPath, std::filesystem::directory_options::skip_permission_denied)) {
			if (std::filesystem::is_regular_file(entry) && entry.path().filename() == file) {
				std::string result = getStandardPath(entry.path().string());
				return getStandardPath(entry.path().string());
			}
		}
		return "";
	}

	std::vector<std::string> getFilesInDirectory(std::string _path, std::string _extension) {
		std::vector<std::string> files;
		std::filesystem::path directoryPath(_path);

		for (const auto& entry : std::filesystem::recursive_directory_iterator(directoryPath, std::filesystem::directory_options::skip_permission_denied)) {
			if (std::filesystem::is_regular_file(entry) && entry.path().extension() == _extension) {
				files.push_back(getStandardPath(entry.path().string()));
			}
		}

		return files;
	}

	bool directoryExists(std::string _path) {
		return std::filesystem::is_directory(_path);
	}

	bool fileExists(std::string _path) {
		return  std::filesystem::exists(_path);
	}

	void createDirectory(std::string _path) {
		std::filesystem::create_directories(_path);
	}

	void copyDirectory(std::string _source, std::string _destination) {
		std::filesystem::copy(_source, _destination, std::filesystem::copy_options::recursive);
	}

	std::string fileExplorer(ExplorerDataType _type, ExplorerDialogType _dialog, std::string _filter) {
		wchar_t filePath[MAX_PATH] = { 0 };

		if (_type == ExplorerDataType::Directory) {
			BROWSEINFO bi = {};
			bi.lpszTitle = L"Select a Directory";
			bi.ulFlags = BIF_RETURNONLYFSDIRS | BIF_NEWDIALOGSTYLE;

			LPITEMIDLIST pidl = SHBrowseForFolder(&bi);
			if (pidl) {
				SHGetPathFromIDListW(pidl, filePath);
				CoTaskMemFree(pidl);
				return wstringToString(filePath);
			}
			return "";
		}

		OPENFILENAMEW ofn = {};
		ofn.lStructSize = sizeof(ofn);
		ofn.hwndOwner = nullptr;
		ofn.lpstrFile = filePath;
		ofn.nMaxFile = MAX_PATH;
		std::wstring wFilter = stringToWString(_filter);
		ofn.lpstrFilter = wFilter.c_str();
		ofn.nFilterIndex = 1;
		ofn.Flags = OFN_PATHMUSTEXIST | ((_dialog == ExplorerDialogType::Open) ? OFN_FILEMUSTEXIST : 0);

		bool success = false;

		if (_dialog == ExplorerDialogType::Open) {
			success = GetOpenFileNameW(&ofn);
		}
		else {
			success = GetSaveFileNameW(&ofn);
		}

		return success ? wstringToString(filePath) : "";
	}
}