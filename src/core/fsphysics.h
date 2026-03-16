#ifndef FSPHYSICS_IS_INCLUDED
#define FSPHYSICS_IS_INCLUDED

#include <ysclass.h>
#include <ysclass11.h>

#include "fsdef.h"
#include "fsenvironment.h"
#include "fslattice.h"
//#include "fsexistence.h"
#include "fsfield.h"
#include "ysscenery.h"

class FsPhysics
{
public:
	FsPhysics();
	YSBOOL CheckObjectToObjectCollision(FsExistence* obj1, FsExistence* obj2);

	YSBOOL CheckRadiusOverlap(YsVec3 pos1, double rad1, YsVec3 pos2, double rad2, double clearance);
	YSBOOL CheckPointInBoundingBox(YsVec3 bbx[2], YsMatrix4x4 mat, YsVec3 pos, double clearance);
	YSBOOL CheckLineInBoundingBox(YsVec3 bbx[2], YsMatrix4x4 mat, YsVec3 startPos, YsVec3 endPos, double clearance);
	YSBOOL CheckBoundingBoxOverlap(YsVec3 bbx1[2], YsMatrix4x4 mat1, YsVec3 bbx2[2], YsMatrix4x4 mat2, double clearance);
	YSBOOL CheckLineShellCollision(FsVisualSrf shell, YsMatrix4x4 mat, YsVec3 startPos, YsVec3 endPos, double clearance, YsVec3& collPos, YsShellPolygonHandle& plHd);
	YSBOOL CheckShellShellCollision(FsVisualSrf shell1, YsMatrix4x4 mat1, FsVisualSrf shell2, YsMatrix4x4 mat2, double clearance, YsVec3& collPos, YsShellPolygonHandle& plHd1, YsShellPolygonHandle& plHd2);

};




#endif