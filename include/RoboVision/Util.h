#ifndef UTIL_h
#define UTIL_h

class Util {
	public:
		Util();
		~Util();
		static void debugPrint(const char* methodName, const char* message);
		static void errorPrint(const char* methodName, const char* message);
		
		// File System operations:
		static int fileCount(char* dirPath);
		static std::vector<std::string> getFileNames(char* dirPath);
		static int isFile( char* name);

	private:
};

#endif