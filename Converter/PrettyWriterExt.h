#ifndef _PRETTY_WRITER_EXT_
#define _PRETTY_WRITER_EXT_
#include <string>
#include <rapidjson/include/prettywriter.h>

template<typename OutputStream, typename SourceEncoding = rapidjson::UTF8<>, typename TargetEncoding = rapidjson::UTF8<>, typename StackAllocator = rapidjson::CrtAllocator>
class PrettyWriterExt : public rapidjson::PrettyWriter<OutputStream, SourceEncoding, TargetEncoding, StackAllocator> {
public:
	typedef rapidjson::PrettyWriter<OutputStream, SourceEncoding, TargetEncoding, StackAllocator> Base;
	typedef typename Base::Ch Ch;

	//! Constructor
	/*! \param os Output stream.
	\param allocator User supplied allocator. If it is null, it will create a private one.
	\param levelDepth Initial capacity of stack.
	*/
	PrettyWriterExt(OutputStream& os, StackAllocator* allocator = 0, size_t levelDepth = Base::kDefaultLevelDepth) :
		rapidjson::PrettyWriter<OutputStream, SourceEncoding, TargetEncoding, StackAllocator>(os, allocator,levelDepth){}

    bool Float(float f)       { rapidjson::PrettyWriter<OutputStream, SourceEncoding, TargetEncoding, StackAllocator>::Prefix(rapidjson::kNumberType); return WriteFloat(f); }
	bool Double(double f) { rapidjson::PrettyWriter<OutputStream, SourceEncoding, TargetEncoding, StackAllocator>::Prefix(rapidjson::kNumberType); return rapidjson::PrettyWriter<OutputStream, SourceEncoding, TargetEncoding, StackAllocator>::WriteDouble(f); }
	bool Uint(unsigned ui)   { rapidjson::PrettyWriter<OutputStream, SourceEncoding, TargetEncoding, StackAllocator>::Prefix(rapidjson::kNumberType); return rapidjson::PrettyWriter<OutputStream, SourceEncoding, TargetEncoding, StackAllocator>::WriteUint(ui); }

	void WriteIndent() { Base::WriteIndent(); };

	bool WriteFloat(float f) {
		char str[32];
		sprintf(str, "%f", f);
		unsigned int sz = strlen(str);
		for (unsigned int i = 0; i < sz; ++i)
			rapidjson::PrettyWriter<OutputStream, SourceEncoding, TargetEncoding, StackAllocator>::os_->Put(str[i]);
		return true;
	}
};

#endif