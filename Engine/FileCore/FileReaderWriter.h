#ifndef _VE_FILE_READER_WRITER_
#define _VE_FILE_READER_WRITER_
#include "Prerequisites.h"
#include "KernelCore/Registrar.h"
#include "File.h"

class VE_EXPORT veFileReaderWriter
{
public:
	veFileReaderWriter();
	virtual ~veFileReaderWriter();

	virtual void* readFile(veSceneManager *sm, const std::string &filePath, const std::string &name, const veFileParam &param) = 0;
	virtual bool writeFile(veSceneManager *sm, void *data, const std::string &filePath) = 0;
};

typedef veRegistrar<std::string, veFileReaderWriter> FileReaderWriterRegistrar;
#define VE_READERWRITER_REG(EXT, CLASS) static veRegistrarAgent<std::string, veFileReaderWriter, CLASS> g_regAgent_##CLASS(EXT)

#endif
