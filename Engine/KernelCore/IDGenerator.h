#ifndef _VE_ID_GENERATOR_
#define _VE_ID_GENERATOR_

class IDGenerator
{
public:
	~IDGenerator() {}

	static unsigned int generateUniqueID() {
		static unsigned int id = 0;
		return id++;
	}

protected:
	IDGenerator() {}
};

#endif
