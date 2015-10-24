#ifndef _VE_FILE_
#define _VE_FILE_
#include "Prerequisites.h"

struct veFileParam
{

	enum MemeryCreate
	{
		INSTACNE,
		COPY,
	};

	MemeryCreate  create;
};

class veSceneManager;
class VE_EXPORT veFile
{
public:
	veFile();
	~veFile();

	static veFile* instance();
	void* readFile(veSceneManager *sm, const std::string &filePath, const std::string &name, const veFileParam &param = {veFileParam::COPY});
	bool writeFile(veSceneManager *sm, void *data, const std::string &filePath);

	static std::string readFileToBuffer(const std::string &filePath);
	static std::string getFullFilePath(const std::string &filePath);
	bool isSupportFile(const std::string &filePath);
	

private:

	std::string getFileExt(const std::string &filePath);
};

#endif

