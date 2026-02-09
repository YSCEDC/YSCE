#ifndef FSRADAR_IS_INCLUDED
#define FSRADAR_IS_INCLUDED
/* { */

#include "fs.h"

class FsHorizontalRadar
{
public:
	void Draw(
	    const class FsSimulation *sim,int x1,int y1,int x2,int y2,const double &rangeInX,const class FsAirplane &withRespectTo,
	    int mode,const double &airAltLimit) const;

	void DrawBasic(
	    const class FsSimulation *sim,int x1,int y1,int x2,int y2,const double &rangeInX,
	    const class FsExistence &withRespectTo,const YsVec3 &pos,const YsAtt3 &att,
	    int mode,const double &airAltLimit) const;

	void DrawCircular(const class FsSimulation* sim, int x, int y, int radius, const double &rangeInX, const class FsAirplane& withRespectTo, int mode, const double& airAltLimit);
private:
	YSBOOL IsInsideCircle(YsVec2 point, YsVec2 circleCenter, int radius);
};


/* } */
#endif
