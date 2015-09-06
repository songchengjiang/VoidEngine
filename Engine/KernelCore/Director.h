#ifndef _VE_DIRECTOR_
#define _VE_DIRECTOR_
#include "Prerequisites.h"
#include "VE_Ptr.h"
#include "Visualiser.h"

class VE_EXPORT veDirector
{
public:
	~veDirector();

	static veDirector* instance();
	veVisualiser* createVisualiser(int w, int h, const std::string &title);

	bool run();
	void stop();

private:

	veDirector();

private:
	
	typedef std::vector< VE_Ptr<veVisualiser> > VisualiserList;

	VisualiserList _visualiserList;
	bool           _isRunning;
};

#endif
