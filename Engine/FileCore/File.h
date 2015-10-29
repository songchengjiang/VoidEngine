#ifndef _VE_FILE_
#define _VE_FILE_
#include "Prerequisites.h"

struct veFileParam
{

	enum MemoryCreate
	{
		INSTACNE,
		COPY,
	};

	MemoryCreate  create;
};

class veSceneManager;
class VE_EXPORT veFile
{
public:
	~veFile();

	static veFile* instance();
	void* readFile(veSceneManager *sm, const std::string &filePath, const std::string &name, const veFileParam &param = {veFileParam::COPY});
	bool writeFile(veSceneManager *sm, void *data, const std::string &filePath);

	virtual std::string readFileToBuffer(const std::string &filePath) = 0;
	virtual std::string getFullFilePath(const std::string &filePath) = 0;
	bool isSupportFile(const std::string &filePath);
	

protected:

	veFile();
	std::string getFileExt(const std::string &filePath);
};

#endif

