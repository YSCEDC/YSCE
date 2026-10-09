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
	double timeStep;

	class CollisionEvent
	{
	public:
		//For now, a collision is definitionally between two objects, otherwise it gets complicated
		CollisionEvent();

		YSHASHKEY keyOne; //object SearchKey. If ground plane, -1
		YSHASHKEY keyTwo;
		FsExistence* objOne;
		FsExistence* objTwo;
		YsShellPolygonHandle handleOne;
		YsShellPolygonHandle handleTwo;
		YsVec3 nomOne; //Face normal
		YsVec3 nomTwo;
		double vertDepthOne; //How far through collision plane 2 is the nearest vertex of poly1?
		double vertDepthTwo;

		YsVec3 totalPush;
		double pushOne; //Proportion of totalPush allocated to obj1. 0 < x < 1
		double pushTwo;

		YsVec3 collPos; //Position of collision in global coordinates
	};

	YSBOOL CheckLineObjectCollision(YsVec3 start, YsVec3 end, FsExistence* obj, double clearance, YsVec3& collPosGlobal);
	YSBOOL CheckObjectObjectCollision(FsExistence* obj1, FsExistence* obj2, double clearance, CollisionEvent* coll);
	
	YSBOOL CheckObjectGroundPlaneCollision(FsExistence* obj, double clearance, CollisionEvent* coll);
	YSBOOL CheckObjectTerrainCollision(FsExistence* obj, double clearance, CollisionEvent* coll);
	//Check if object bounding box intersects terrain item geometry
	YSBOOL CheckBoundingBoxTerrainIntersect(YsVec3 bbx[2], YsMatrix4x4 bbxMat, const YsElevationGrid* grid, YsMatrix4x4 terMat, double clearance, YsArray <YsElvGridFaceId> &intersectors,YsArray <YsArray<YsVec3>>& intersectingTris);

	YSBOOL CheckRadiusIntersect(YsVec3 firstpos, double firstrad, YsVec3 secondpos, double secondrad, double clearance);
	YSBOOL CheckPointBoundingBoxIntersect(YsVec3 bbx[2], YsMatrix4x4 mat, YsVec3 pos, double clearance);
	YSBOOL CheckLineSegmentBoundingBoxIntersect(YsVec3 bbx[2], YsMatrix4x4 mat, YsVec3 startPos, YsVec3 endPos, double clearance);
	YSBOOL CheckInfiniteLineBoundingBoxIntersect(YsVec3 bbx[2], YsMatrix4x4 mat, YsVec3 origin, YsVec3 direction, double clearance);

	//Bounding box and triangle verts should be in the bounding box reference frame (mat)
	YSBOOL CheckTriangleBoundingBoxIntersect(YsVec3 bbx[2], YsMatrix4x4 mat, YsVec3 tri[3], double clearance);

	//Bounding box and plane should be in the bounding box reference frame (mat)
	YSBOOL CheckPlaneBoundingBoxIntersect(YsVec3 bbx[2], YsMatrix4x4 mat, YsPlane pln, double clearance, YsVec3 &lineOrig, YsVec3 &lineDir); 
	

	YSBOOL Check2BoundingBoxIntersect(YsVec3 bbx1[2], YsMatrix4x4 mat1, YsVec3 bbx2[2], YsMatrix4x4 mat2, double clearance);
	YSBOOL CheckLineShellIntersect(FsVisualSrf shell, YsVec3 startPos, YsVec3 endPos, double clearance, YsVec3& collPos, YsShellPolygonHandle* face);
	YSBOOL CheckTriangleShellIntersect(FsVisualSrf* shell1, double clearance, YsVec3& collPosLocal, YsVec3 tri[3], YsShellPolygonHandle* face);
	YSBOOL CheckShellShellIntersect(const FsVisualSrf& shell1, const FsVisualSrf& shell2, double clearance, YsVec3& collPosGlobal, YsShellPolygonHandle& face1, YsShellPolygonHandle& face2);
	YSBOOL CheckShellShellIntersectAndDepth(const FsVisualSrf& shell1, const FsVisualSrf& shell2, double clearance, YsVec3& collPosGlobal, YsShellPolygonHandle& face1, YsShellPolygonHandle& face2, double& depth1, double& depth2);
	YSRESULT CheckIntersectionDepth(const FsVisualSrf& shell1, const FsVisualSrf& shell2, double clearance, YsVec3& collPosGlobal, YsShellPolygonHandle* face1, YsShellPolygonHandle* face2, double& depth1, double& depth2);
	YSBOOL CheckShellShellIntersection(const FsVisualSrf& shell1, const FsVisualSrf& shell2, double clearance);


	YSRESULT ConvertBbxToShell(YsVec3 bbx[2], YsMatrix4x4 transform, FsVisualSrf& localShell, FsVisualSrf& transformedShell);
	//YSRESULT ConvertTerToShell(YsElevationGrid& grid, FsVisualSrf& shell);
	YsVisualSrf ConvertFsVisToYsVis(FsVisualSrf* in);
	FsVisualSrf ConvertYsVisToFsVis(YsVisualSrf* in);

	//Output push vector
	YSRESULT PushObjectsFromCollision(CollisionEvent* coll, YsVec3 out[2]);
	int DoDamageByDeltaV(YsVec3 deltaV);
	
	YsVec3 ConvertAngularToLinearVelocity(YsAtt3 rot, YsVec3 pos);
	void ConvertLinearToAngularVelocity(YsAtt3& rot, YsVec3 pos, YsVec3 vel, YsVec3 bbx[2], YsVec3& deltaV);

};




#endif