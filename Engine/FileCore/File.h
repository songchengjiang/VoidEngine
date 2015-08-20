#ifndef _VE_FILE_
#define _VE_FILE_
#include "Prerequisites.h"

class VE_EXPORT veFile
{
public:
	veFile();
	~veFile();

	static veFile* instance();
	void* readFile(const std::string &filePath);
	bool writeFile(void *data, const std::string &filePath);

private:

	std::string getFileExt(const std::string &filePath);
};

#endif

