#ifndef FILEMANAGER_HPP
# define FILEMANAGER_HPP

# include <string> // std::string.
# include <fstream> // std::ifstream.
# include <sstream> // std::stringstream.
# include <sys/stat.h> // stat().
# include <unistd.h>   // access(), F_OK, R_OK.
# include <vector> // std::vector.
# include <dirent.h> // opendir(), closedir().

class FileManager
{
	private:
		FileManager();
		~FileManager();
		FileManager(const FileManager& ref);
		FileManager& operator=(const FileManager& ref);
	public:
		static std::string		readFile(const std::string& path);
		static bool			isFileExist(const std::string& path);
		static bool			isDirectory(const std::string& path);
		static bool			isReadable(const std::string& path);
		static long			getFileSize(const std::string& path);
		static std::vector<std::string> getDirFiles(const std::string& path);
};

#endif
