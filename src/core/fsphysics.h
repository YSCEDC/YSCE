#ifndef FSPHYSICS_IS_INCLUDED
#define FSPHYSICS_IS_INCLUDED

#include <ysclass.h>
#include <ysclass11.h>

#include "fsdef.h"
//#include "fsenvironment.h"
//#include "fslattice.h"
#include "fsexistence.h"
//#include "fssimulation.h"
//#include "fsfield.h"
#include "ysscenery.h"

class FsPhysics
{
public:
	FsSimulation* sim;
	FsPhysics();
	void UpdatePhysics(FsSimulation* currentSim);

	YSBOOL CheckLineObjectCollision(YsVec3 start, YsVec3 end, FsExistence* obj, double clearance, YsVec3& collPosGlobal);
	YSBOOL CheckObjectObjectCollision(FsExistence* obj1, FsExistence* obj2, double clearance, YsVec3& collPosGlobal);
	
	YSBOOL CheckObjectTerrainCollision(FsExistence* obj, double clearance, YsVec3& collPosGlobal);
	//Check if object bounding box intersects terrain item geometry
	YSBOOL CheckBoundingBoxTerrainIntersection(YsVec3 bbx[2], YsMatrix4x4 bbxMat, const YsElevationGrid* grid, YsMatrix4x4 terMat, double clearance, YsArray <YsElvGridFaceId> &intersectors,YsArray <YsArray<YsVec3>>& intersectingTris);

	YSBOOL CheckRadiusIntersection(YsVec3 firstpos, double firstrad, YsVec3 secondpos, double secondrad, double clearance);
	YSBOOL CheckPointBoundingBoxIntersection(YsVec3 bbx[2], YsMatrix4x4 mat, YsVec3 pos, double clearance);
	YSBOOL CheckLineSegmentBoundingBoxIntersection(YsVec3 bbx[2], YsMatrix4x4 mat, YsVec3 startPos, YsVec3 endPos, double clearance);
	YSBOOL CheckInfiniteLineBoundingBoxIntersection(YsVec3 bbx[2], YsMatrix4x4 mat, YsVec3 origin, YsVec3 direction, double clearance);

	//Bounding box and triangle verts should be in the bounding box reference frame (mat)
	YSBOOL CheckTriangleBoundingBoxIntersection(YsVec3 bbx[2], YsMatrix4x4 mat, YsVec3 tri[3], double clearance);

	//Bounding box and plane should be in the bounding box reference frame (mat)
	YSBOOL CheckPlaneBoundingBoxIntersection(YsVec3 bbx[2], YsMatrix4x4 mat, YsPlane pln, double clearance, YsVec3 &lineOrig, YsVec3 &lineDir); 
	

	YSBOOL Check2BoundingBoxIntersection(YsVec3 bbx1[2], YsMatrix4x4 mat1, YsVec3 bbx2[2], YsMatrix4x4 mat2, double clearance);
	YSBOOL CheckLineShellIntersection(FsVisualSrf shell, YsVec3 startPos, YsVec3 endPos, double clearance, YsVec3& collPos, YsShellPolygonHandle& plHd);
	YSBOOL CheckTriangleShellIntersection(FsVisualSrf* shell1, double clearance, YsVec3& collPosLocal, YsVec3 tri[3], YsShellPolygonHandle& plHd);
	YSBOOL CheckShellShellIntersection(FsVisualSrf* shell1, FsVisualSrf* shell2, double clearance, YsVec3& collPosGlobal, YsShellPolygonHandle& plHd1, YsShellPolygonHandle& plHd2);
};




#endif