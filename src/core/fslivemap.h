#ifndef FSLIVEMAP_IS_INCLUDED
#define FSLIVEMAP_IS_INCLUDED
/* { */

#include "fs.h"

class FsLiveMap
{
public:
	void Draw(const class FsSimulation *sim, int x1, int y1, int x2, int y2, double range) const;
};

/* } */
#endif
