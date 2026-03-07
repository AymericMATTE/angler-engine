#pragma once

#pragma region External Dependencies

#include <string>
#include <vector>

#pragma endregion

/// <summary>
/// Converts a narrow string (UTF-8) to a wide string (UTF-16).
/// </summary>
/// <param name="_str">(std::string) : The narrow string to convert.</param>
/// <returns>(std::wstring) : The resulting wide string.</returns>
std::wstring stringToWString(const std::string& _str);
	
/// <summary>
/// Converts a wide string (UTF-16) to a narrow string (UTF-8).
/// </summary>
/// <param name="_wstr">(std::wstring) : The wide string to convert.</param>
/// <returns>(std::string) : The resulting narrow string.</returns>
std::string wstringToString(const std::wstring& _wstr);

namespace angler::os {
	/// <summary>
	/// Retrieves the name of a file from a given path.
	/// </summary>
	/// <param name="_path">(std::string) : The path to the file.</param>
	/// <param name="_extension">(bool) : Whether to include the file extension in the returned name.</param>
	/// <returns>(std::string) : The name of the file, including the extension if specified.</returns>
	std::string getFileName(std::string _path, bool _extension = false);

	/// <summary>
	/// Retrieves the extension of a file from a given path or filename.
	/// </summary>
	/// <param name="_path">(std::string) : The path or name of the file.</param>
	/// <param name="_unknow">(std::string) : The default value to return if the extension is not found.</param>
	/// <returns>(std::string) : The file extension.</returns>
	std::string getExtension(std::string _path, std::string _unknow = "");

	/// <summary>
	/// Converts a relative path to an absolute path.
	/// </summary>
	/// <param name="_path">(std::string) : The input relative path.</param>
	/// <returns>(std::string) : The resulting absolute path.</returns>
	std::string getAbsolutePath(std::string _path);

	/// <summary>
	/// Replaces all backslashes with forward slashes in the given path.
	/// </summary>
	/// <param name="path">(std::string) : The path to be standardized.</param>
	/// <returns>(std::string) : The standardized path.</returns>
	std::string getStandardPath(std::string _path);

	/// <summary>
	/// Retrieves the path of the currently running build executable.
	/// </summary>
	/// <returns>(std::string) : The path of the executable.</returns>
	std::string getExecutablePath();

	/// <summary>
	/// Recursively searches for a file within a directory and its subdirectories.
	/// </summary>
	/// <param name="path">(std::string) : The path of the root directory to start the search.</param>
	/// <param name="file">(std::string) : The name of the file to search for.</param>
	/// <param name="args">(char) : The file access mode to use when opening the file.</param>
	/// <returns>(std::string) : The full path to the file if found; otherwise, an empty string.</returns>
	std::string getFileInDirectory(std::string path, std::string file, char args = 'r');

	/// <summary>
	/// Searches for all files with a specific extension within a directory and its subdirectories.
	/// </summary>
	/// <param name="_path">(std::string) : The path to the root directory.</param>
	/// <param name="_extension">(std::string) : The file extension to search for.</param>
	/// <returns>(std::vector) : A list of all files with the specified extension.</returns>
	std::vector<std::string> getFilesInDirectory(std::string _path, std::string _extension = ".png");

	/// <summary>
	/// Checks if the specified directory exists at the given path.
	/// </summary>
	/// <param name="_path">(std::string) : The path to the directory.</param>
	/// <returns>(bool) : True if the directory exists, otherwise false.</returns>
	bool directoryExists(std::string _path);

	/// <summary>
	/// Checks if the specified file exists at the given path.
	/// </summary>
	/// <param name="_path">(std::string) : The path to the file.</param>
	/// <returns>(bool) : True if the file exists, otherwise false.</returns>
	bool fileExists(std::string _path);

	/// <summary>
	/// Create a directory at the specified path.
	/// </summary>
	/// <param name="_path">(std::string) : The path to the directory.</param>
	void createDirectory(std::string _path);

	/// <summary>
	/// Copies a directory, its subdirectories, and its contents to another directory.
	/// </summary>
	/// <param name="_source">(std::string) : Path to the source directory.</param>
	/// <param name="_destination">(std::string) : Path to the destination directory.</param>
	void copyDirectory(std::string _source, std::string _destination);

	#define EXPLORER_FILTER_IMAGES "Image Files\0*.jpg;*.jpeg;*.png;*.bmp;*.gif;*.tiff;*.webp\0"
	#define EXPLORER_FILTER_TEXT "Text Files\0*.txt;*.csv;*.log;*.ini;*.json;*.xml\0"
	#define EXPLORER_FILTER_SOUND "Sound Files\0*.mp3;*.wav;*.ogg;*.flac;*.aac;*.m4a\0"
	#define EXPLORER_FILTER_FONTS "Font Files\0*.ttf;*.otf;*.woff;*.woff2\0"
	enum class ExplorerDataType {
		File,
		Directory
	};
	enum class ExplorerDialogType {
		Open,
		Save
	};
	std::string fileExplorer(ExplorerDataType _type = ExplorerDataType::File, ExplorerDialogType _dialog = ExplorerDialogType::Open, std::string _filter = "All Files\0*.*\0");
}