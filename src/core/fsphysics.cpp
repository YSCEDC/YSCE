#include "fsphysics.h"
#include "fssimulation.h"
#include "fsweapon.h"

FsPhysics::FsPhysics()
{
	sim = nullptr;
	timeStep = 0.0;
}
void FsPhysics::UpdatePhysics(FsSimulation* currentSim)
{
	sim = currentSim;
	timeStep = sim->GetTimeStep();
	//userInput = sim->GetUserInput();
	//cfg = sim->GetConfig();
}

/////////////////// Object to object collisions /////////////////////////
//  Object/object collisions
//Check object collision with line (obj, pos1, pos2, clearance) [for weapon coll calcs]
//Check object collision with object (obj1, obj2)
YSBOOL FsPhysics::CheckLineObjectCollision(YsVec3 start, YsVec3 end, FsExistence* obj, double clearance, YsVec3& collPos)
{
	YsVec3 bbx[2];
	bbx[0] = obj->GetCollisionShellBbx()[0];
	bbx[1] = obj->GetCollisionShellBbx()[1];
	if (CheckLineSegmentBoundingBoxIntersect(bbx, obj->GetMatrix(), start, end, clearance) == YSTRUE)
	{
		YsShellPolygonHandle* hitFace = nullptr;
		FsVisualSrf shell = obj->TransformedCollisionShell();

		if (CheckLineShellIntersect(shell, start, end, clearance, collPos, hitFace) == YSTRUE)
		{
			return YSTRUE;
		}
	}
	return YSFALSE;
}

YSBOOL FsPhysics::CheckObjectObjectCollision(FsExistence* obj1, FsExistence* obj2, double clearance, CollisionEvent* coll)
{
	if (obj1 == obj2)
	{
		return YSFALSE;
	}
	
	YsVec3 collPos;
	YsShellPolygonHandle face1, face2;
	YsVec3 pos1, pos2, bbx1[2], bbx2[2];
	double rad1, rad2;
	YsMatrix4x4 mat1, mat2;

	pos1 = obj1->GetPosition();
	pos2 = obj2->GetPosition();
	rad1 = obj1->GetApproximatedCollideRadius();
	rad2 = obj2->GetApproximatedCollideRadius();

	if (CheckRadiusIntersect(pos1, rad1, pos2, rad2, clearance) == YSFALSE)
	{
		return YSFALSE;
	}
	
	bbx1[0] = obj1->GetCollisionShellBbx()[0];
	bbx1[1] = obj1->GetCollisionShellBbx()[1];
	bbx2[0] = obj2->GetCollisionShellBbx()[0];
	bbx2[1] = obj2->GetCollisionShellBbx()[1];
	mat1 = obj1->GetMatrix();
	mat2 = obj2->GetMatrix();	

	//Check the bounding boxes from their coordinates
	if (Check2BoundingBoxIntersect(bbx1, mat1, bbx2, mat2, clearance) == YSFALSE)
	{
		return YSFALSE;
	}
	
	//Convert bounding boxes to SURF and check intersection
	//FsVisualSrf bbx1Local, bbx1Trans, bbx2Local, bbx2Trans;
	//ConvertBbxToShell(bbx1, mat1, bbx1Local, bbx1Trans);
	//ConvertBbxToShell(bbx2, mat2, bbx2Local, bbx2Trans);
	//if (CheckShellShellIntersect(bbx1Trans, bbx2Trans, clearance, collPos, face1, face2) == YSFALSE)
	//{
	//	return YSFALSE;
	//}
	
	if (obj1->transformedCollShellThisStep != YSTRUE)
	{
		obj1->CommonProp().RemakeMatrix();
		obj1->SetTransformationToCollisionShell(obj1->GetMatrix());
	}
	if (obj2->transformedCollShellThisStep != YSTRUE)
	{
		obj2->CommonProp().RemakeMatrix();
		obj2->SetTransformationToCollisionShell(obj2->GetMatrix());
	}
	
	double vD1, vD2;
	if (CheckShellShellIntersectAndDepth(obj1->TransformedCollisionShell(), obj2->TransformedCollisionShell(), clearance, collPos, face1, face2, vD1, vD2) == YSTRUE)
	{
		//Get normals
		YsVec3 n1, n2;

		n1 = obj1->TransformedCollisionShell().GetNormal(face1);
		n2 = obj2->TransformedCollisionShell().GetNormal(face2);
		
			//Alternate method to get normal just in case
		if (n1 == YsOrigin())
		{
			const YsShellVertexHandle* vt;
			n1 = obj1->TransformedCollisionShell().GetPolygon(face1)->GetNormal();
		}
		if (n2 == YsOrigin())
		{
			const YsShellVertexHandle* vt;
			n2 = obj2->TransformedCollisionShell().GetPolygon(face2)->GetNormal();
		}
		n1.Normalize();
		n2.Normalize();

		//Point normals toward colliding object
		YsMatrix4x4 mat;
		YsVec3 facepos;
		YsAtt3 att;
		int direc;

		mat.Initialize();
		att.SetUpVector(n2.GetArbitraryPerpendicularVector());
		att.SetForwardVector(n2);
		mat.Rotate(att);
		
		facepos = obj1->GetPosition() - collPos;
		mat.MulInverse(facepos, facepos, 1.0);
		direc = YsSign(facepos.z());
		n2 *= direc;

		mat.Initialize();
		att.SetUpVector(n1.GetArbitraryPerpendicularVector());
		att.SetForwardVector(n1);
		mat.Rotate(att);

		facepos = obj2->GetPosition() - collPos;
		mat.MulInverse(facepos, facepos, 1.0);
		direc = YsSign(facepos.z());
		n1 *= direc;

		coll->collPos = collPos;
		coll->objOne = obj1;
		coll->objTwo = obj2;
		coll->keyOne = obj1->SearchKey();
		coll->keyTwo = obj2->SearchKey();
		coll->handleOne = face1;
		coll->handleTwo = face2;
		coll->nomOne = n1;
		coll->nomTwo = n2;
		if (obj1 != nullptr)
		{
			coll->vertDepthOne = abs(vD1);
		}
		if (obj2 != nullptr)
		{
			coll->vertDepthTwo = abs(vD2);
		}
		
		coll->totalPush = coll->nomTwo * coll->vertDepthOne + coll->nomOne * coll->vertDepthTwo;

		return YSTRUE;
	}
	return YSFALSE;
}

/////////////////// Object to terrain collisions /////////////////////////
// Object/terrain collisions
//Check object collision with terrain (obj, ter) [parent function]
	//Check radius groundplane overlap (pos, rad)
	//Check line groundplane overlap (pos, pos) [this is so simple as to not be necessary. If p1 or p2 is below y=0, return true]
	//Check shell overlap with terrain (obj, ter)
	//Check line overlap with terrain (ter, p1, p2)
	//Check point inside terrain (ter, p1) [this is functionally GetElevationAtPoint compared with p1]
	//Check point inside surf terrain [this one is more complicated because terrain surfs may not be solid. Can steal from IsInCloud though]
YSBOOL FsPhysics::CheckObjectGroundPlaneCollision(FsExistence* obj, double clearance, CollisionEvent* coll)
{
	YsVec3 collPos = YsOrigin();
	YsShellPolygonHandle face1, face2;

	YsVec3 objPos, collPosLocal;
	double objRad;
	objPos = obj->GetPosition();
	objRad = obj->GetApproximatedCollideRadius();
	YsVec3 objBbx[2];
	objBbx[0] = obj->GetCollisionShellBbx()[0];
	objBbx[1] = obj->GetCollisionShellBbx()[1];

	if (objPos.y() > objRad + clearance) //too high to intersect
	{
		return YSFALSE;
	}
	else
	{
		if (obj->GetPosition().y() < 0.0)
		{
			YsVec3 plop;
			plop.Set(obj->GetPosition().x(), 0.0, obj->GetPosition().z());
			obj->CommonProp().SetPosition(plop);
			obj->CommonProp().RemakeMatrix();
		}
		
		YsVec3 floorBbx[2];
		floorBbx[0].Set(-9999.0, -9999.0, -9999.0);
		floorBbx[1].Set(9999.0, 0.0, 9999.0);
		
		YsMatrix4x4 floorMat;
		floorMat.Initialize();
		floorMat.Translate(obj->GetPosition().x(), 0.0, obj->GetPosition().z());
		FsVisualSrf floorSrf, bbxLocal, bbxTrans;

		YsVec3 vtx[4];
		YsShellVertexHandle vt[4];
		vtx[0].Set(-9999, 0, -9999); //50000m is too big for YsCheckInsidePolygon3() to correctly resolve
		vtx[1].Set(9999, 0, -9999);
		vtx[2].Set(9999, 0, 9999);
		vtx[3].Set(-9999, 0, 9999);
		for (int c = 0; c < 4; c++)
		{
			vt[c] = floorSrf.AddVertex(vtx[c]);
		}
		
		floorSrf.AddPolygon(4, vt);
		floorSrf.SetMatrix(floorMat);

		//ConvertBbxToShell(objBbx, obj->GetMatrix(), bbxLocal, bbxTrans);

		if (Check2BoundingBoxIntersect(objBbx, obj->GetMatrix(), floorBbx, floorMat, clearance) == YSTRUE)
		{
			if (obj->transformedCollShellThisStep != YSTRUE)
			{
				obj->CommonProp().RemakeMatrix();
				obj->SetTransformationToCollisionShell(obj->GetMatrix());
			}
		
			double depth1, depth2;
			obj->sceCollision.Increment();
			if (CheckShellShellIntersectAndDepth(obj->TransformedCollisionShell(), floorSrf, clearance, collPos, face1, face2, depth1, depth2) == YSTRUE)
			{
				coll->collPos = collPos;
				coll->objOne = obj;
				coll->objTwo = nullptr;
				coll->keyOne = obj->SearchKey();
				coll->keyTwo = -1;
				coll->handleOne = face1;
				coll->handleTwo = nullptr;
				coll->nomOne = obj->TransformedCollisionShell().GetNormal(face1);
				coll->nomTwo.Set(0.0, 1.0, 0.0);
				coll->vertDepthOne = depth1;
				coll->vertDepthTwo = 0.0;
				coll->totalPush = coll->nomTwo * coll->vertDepthOne;

				return YSTRUE;
			}
		}
	}
}

YSBOOL FsPhysics::CheckObjectTerrainCollision(FsExistence* obj, double clearance, CollisionEvent* coll)
{
	//Need to check the line between obj current pos and prev pos, see if it intersects the tershell
	//Currently with enough speed you can clip through terrain/buildings
	YsVec3 collPos = YsOrigin();
	YsShellPolygonHandle face1, face2;
	
	YsVec3 objPos, collPosLocal;
	double objRad;
	objPos = obj->GetPosition();
	objRad = obj->GetApproximatedCollideRadius();
	YsVec3 objBbx[2];
	objBbx[0] = obj->GetCollisionShellBbx()[0];
	objBbx[1] = obj->GetCollisionShellBbx()[1];
	
	//Check if obj is too high to intersect anything
	if (objPos.y() > sim->highestTerrainHeight + objRad + clearance)
	{
		return YSFALSE;
	}

	//Check terrain items
	YsArray <YsSceneryItem*> candidateList;
	candidateList.CleanUp();
	candidateList.CopyFrom(sim->terrainList);
	YsArray <YsArray <YsElvGridFaceId>> candidateIntersectingFaces;
	YsArray <YsArray<YsVec3>> intersectingTris;
	
	for (int i = 0; i < candidateList.GetN(); i++)
	{
		if (CheckRadiusIntersect(objPos, objRad, candidateList[i]->GetGlobalCenterPosition(), candidateList[i]->GetBbxDiag(), clearance) == YSFALSE)
		{
			candidateList.Delete(i);
			i--;
		}
	}
	if (candidateList.GetN() < 1)
	{
		return YSFALSE;
	}

	FsVisualSrf bbxLoc, bbxTrans, terrLoc, terrTrans;
	ConvertBbxToShell(objBbx, obj->GetMatrix(), bbxLoc, bbxTrans); //This should be cached in the aircraft object eventually
	//Check bounding box overlap with terrain item bbxes
	for (int i = 0; i < candidateList.GetN(); i++)
	{
		YsVec3 terBbx[2];
		YsMatrix4x4 terMat;
		terMat.Translate(candidateList[i]->GetGlobalPosition());
		terMat.Rotate(candidateList[i]->GetGlobalAttitude());
		candidateList[i]->GetBoundingBox(terBbx);

		ConvertBbxToShell(terBbx, terMat, terrLoc, terrTrans);

		//if (Check2BoundingBoxIntersect(objBbx, obj->GetMatrix(), terBbx, terMat, clearance) == YSFALSE) //This function is weirdly slow. It's cheaper to convert to srf and check collision than to run it
		if (CheckPointBoundingBoxIntersect(terBbx, terMat, obj->GetPosition(), obj->GetApproximatedCollideRadius()) == YSFALSE &&
			CheckShellShellIntersect(bbxTrans, terrTrans, 0, collPos, face1, face2) == YSFALSE)
		{
			candidateList.Delete(i);
			i--;
		}
	}
	if (candidateList.GetN() < 1)
	{
		return YSFALSE;
	}

	//Check bounding box overlap with terrain item geometry
	YsVisualSrf *meshLink;
	for (int i = 0; i < candidateList.GetN(); i++)
	{
		if (candidateList[i]->GetObjType() == YsSceneryItem::ELEVATIONGRID)
		{
			if (candidateList[i]->GetElevationGrid()->srfGenerated != YSTRUE)
			{
				candidateList[i]->GetElevationGrid()->ConvertTerToShell();
			}
			if (candidateList[i]->GetElevationGrid()->needTransform == YSTRUE)
			{
				YsMatrix4x4 mat;
				mat.Initialize();
				mat.Rotate(candidateList[i]->GetGlobalAttitude());
				mat.Translate(candidateList[i]->GetGlobalPosition());

				candidateList[i]->GetElevationGrid()->collisionTransformed = candidateList[i]->GetElevationGrid()->collision;
				candidateList[i]->GetElevationGrid()->collisionTransformed.SetMatrix(mat);
				candidateList[i]->GetElevationGrid()->needTransform = YSFALSE;
			}
			terrTrans.CopyFrom(candidateList[i]->GetElevationGrid()->collisionTransformed);
		}
		else if (candidateList[i]->GetObjType() == YsSceneryItem::SHELL)
		{
			terrTrans.CopyFrom(candidateList[i]->GetTerrainShell()->GetTransformedCollisionShell());
		}
		if (CheckShellShellIntersect(bbxTrans, terrTrans, 0, collPos, face1, face2) == YSFALSE)
		{
			candidateList.Delete(i);
			i--;
			
		}
	}
	if (candidateList.GetN() < 1)
	{
		return YSFALSE;
	}

	//Check object geometry collision with terrain geometry:
	if (obj->transformedCollShellThisStep != YSTRUE)
	{
		obj->CommonProp().RemakeMatrix();
		obj->SetTransformationToCollisionShell(obj->GetMatrix());
	}
	for (int i = 0; i < candidateList.GetN(); i++)
	{
		if (candidateList[i]->GetObjType() == YsSceneryItem::ELEVATIONGRID)
		{
			terrTrans.CopyFrom(candidateList[i]->GetElevationGrid()->collisionTransformed);
			meshLink = &candidateList[i]->GetElevationGrid()->collisionTransformed;

		}
		else if (candidateList[i]->GetObjType() == YsSceneryItem::SHELL)
		{
			terrTrans.CopyFrom(candidateList[i]->GetTerrainShell()->GetTransformedCollisionShell());
			meshLink = &candidateList[i]->GetElevationGrid()->collisionTransformed;
		}
		
		double depth1, depth2;
		if (CheckShellShellIntersectAndDepth(obj->TransformedCollisionShell(), terrTrans, 0, collPos, face1, face2, depth1, depth2) == YSTRUE)
		{
			//Get normals
			YsVec3 n1, n2;

			n1 = obj->TransformedCollisionShell().GetNormal(face1);
			n2 = terrTrans.GetNormal(face2);
			n1.Normalize();
			n2.Normalize();

			//Point normals toward colliding object
			YsMatrix4x4 mat;
			YsVec3 facepos;
			YsAtt3 att;
			int direc;

			mat.Initialize();
			att.SetUpVector(n2.GetArbitraryPerpendicularVector());
			att.SetForwardVector(n2);
			mat.Rotate(att);

			facepos = obj->GetPosition() - collPos;
			mat.MulInverse(facepos, facepos, 1.0);
			direc = YsSign(facepos.z());
			n2 *= direc;

			coll->collPos = collPos;
			coll->handleOne = face1;
			coll->handleTwo = face2;
			coll->keyOne = obj->SearchKey();
			coll->keyTwo = -1;
			coll->nomOne = n1;
			coll->nomTwo = n2;
			coll->objOne = obj;
			coll->vertDepthOne = depth1;
			coll->vertDepthTwo = depth2;
			coll->totalPush = coll->nomTwo * coll->vertDepthOne + coll->nomOne * coll->vertDepthTwo;
		}
		else
		{
			candidateList.Delete(i);
			i--;
		}
	}
	if (candidateList.GetN() > 0)
	{
		return YSTRUE;
	}
	return YSFALSE;
}

YSBOOL FsPhysics::CheckBoundingBoxTerrainIntersect(YsVec3 bbx[2], YsMatrix4x4 bbxMat, const YsElevationGrid* grid, YsMatrix4x4 terMat, double clearance, YsArray <YsElvGridFaceId> &intersectors, YsArray <YsArray<YsVec3>>& intersectingTris)
{
	FsVisualSrf bbxLocal, bbxTrans, ter;
	YsVec3 collPos;
	YsShellPolygonHandle discard;
	ConvertBbxToShell(bbx, bbxMat, bbxLocal, bbxTrans);
	ter = ConvertYsVisToFsVis(&grid->owner->collisionTransformed);
	if (CheckShellShellIntersect(bbxTrans, ter, clearance, collPos, discard, discard))
	{
		return YSTRUE;
	}	

	return YSFALSE;
}



/////////////////// Common functions /////////////////////////
// Common utility functions
//Check radius overlap (pos1, rad1, pos2, rad2)
//Check point in bounding box (bbx, mat, pos)
//Check line in bounding box (bbx, mat, p1, p2)
//Check bounding box overlap (bbx1, mat1, bbx2, mat2)
//Check line collision with shell (srf, matrix, p1, p2)
//Check shell overlap (srf, matrix, srf, matrix)
//Convert ter to surf
//Exclude shell faces if vertex is beyond htradius
YSBOOL FsPhysics::CheckRadiusIntersect(YsVec3 pos1, double rad1, YsVec3 pos2, double rad2, double clearance)
{
	YsVec3 dif;
	dif = pos1 - pos2;
	if (dif.GetLength() < rad1 + rad2 + clearance)
	{
		return YSTRUE;
	}
	return YSFALSE;
}

YSBOOL FsPhysics::CheckPointBoundingBoxIntersect(YsVec3 bbx[2], YsMatrix4x4 mat, YsVec3 pos, double clearance)
{
	YsMatrix4x4 localMat = mat;
	localMat.Invert();
	YsVec3 localPos;
	localPos = localMat * pos;
	if (localPos.x() < bbx[0].x() - clearance || localPos.x() > bbx[1].x() + clearance)
	{
		return YSFALSE;
	}
	if (localPos.y() < bbx[0].y() - clearance || localPos.y() > bbx[1].y() + clearance)
	{
		return YSFALSE;
	}
	if (localPos.z() < bbx[0].z() - clearance || localPos.z() > bbx[1].z() + clearance)
	{
		return YSFALSE;
	}
	return YSTRUE;
}

YSBOOL FsPhysics::CheckLineSegmentBoundingBoxIntersect(YsVec3 bbx[2], YsMatrix4x4 mat, YsVec3 startPos, YsVec3 endPos, double clearance)
{
	if (CheckPointBoundingBoxIntersect(bbx, mat, startPos, clearance) == YSTRUE || CheckPointBoundingBoxIntersect(bbx, mat, endPos, clearance) == YSTRUE)
	{
		return YSTRUE;
	}

	YsMatrix4x4 localMat = mat;
	YsVec3 localStart, localEnd, nearest, bbxCen;
	localMat.Invert();

	localStart = localMat * startPos;
	localEnd = localMat * endPos;
	bbxCen = (bbx[0] + bbx[1]) / 2;
	YsGetNearestPointOnLine3(nearest, localStart, localEnd, bbxCen);
	//Reset the matrix before passing it so it doesn't transform nearest
	localMat.Initialize();

	if (CheckPointBoundingBoxIntersect(bbx, localMat, nearest, clearance) == YSTRUE)
	{
		return YSTRUE;
	}
	
	return YSFALSE;
}

YSBOOL FsPhysics::CheckInfiniteLineBoundingBoxIntersect(YsVec3 bbx[2], YsMatrix4x4 mat, YsVec3 origin, YsVec3 direction, double clearance)
{
	YsMatrix4x4 localMat = mat;
	YsVec3 localStart, localEnd, nearest, bbxCen;
	localMat.Invert();

	localStart = localMat * origin;
	localEnd = localMat * (origin + direction);
	bbxCen = (bbx[0] + bbx[1]) / 2;
	YsGetNearestPointOnLine3(nearest, localStart, localEnd, bbxCen);
	//Reset the matrix before passing it so it doesn't transform nearest
	localMat.Initialize();

	if (CheckPointBoundingBoxIntersect(bbx, localMat, nearest, clearance) == YSTRUE)
	{
		return YSTRUE;
	}

	return YSFALSE;
}

YSBOOL FsPhysics::CheckTriangleBoundingBoxIntersect(YsVec3 bbx[2], YsMatrix4x4 mat, YsVec3 tri[3], double clearance)
{
	//This currently expects everything in bbx reference frame
	YsVec3 lineOrig, lineDir;
	YsPlane pln;
	pln.MakePlaneFromTriangle(tri[0],tri[1],tri[2]);

	if (CheckPlaneBoundingBoxIntersect(bbx, mat, pln, clearance, lineOrig, lineDir) == YSTRUE)
	{
		YsVec2 nearest, p1, p2, triCen, tri2[3];
		p1.GetXZ(lineOrig);
		p2.GetXZ(lineOrig + lineDir);
		triCen.GetXZ(YsGetCenter(3, tri));
		for (int i = 0; i < 3; i++)
		{
			tri2[i].GetXZ(tri[i]);
		}
		YsGetNearestPointOnLine2(nearest, p1, p2, triCen);
		
		if (YsCheckInsideTriangle2(nearest, tri2) != YSOUTSIDE)
		{
			return YSTRUE;
		}
	}
	return YSFALSE;
}

YSBOOL FsPhysics::CheckPlaneBoundingBoxIntersect(YsVec3 bbx[2], YsMatrix4x4 mat, YsPlane pln, double clearance, YsVec3& lineOrig, YsVec3& lineDir)
{
	//This currently expects everything in bbx reference frame
	YsVec3 corner[8];
	corner[0].Set(bbx[0].x() - clearance, bbx[0].y() - clearance, bbx[0].z() - clearance);
	corner[1].Set(bbx[1].x() + clearance, bbx[0].y() - clearance, bbx[0].z() - clearance);
	corner[2].Set(bbx[0].x() - clearance, bbx[1].y() + clearance, bbx[0].z() - clearance);
	corner[3].Set(bbx[1].x() + clearance, bbx[1].y() + clearance, bbx[0].z() - clearance);
	corner[4].Set(bbx[0].x() - clearance, bbx[0].y() - clearance, bbx[1].z() + clearance);
	corner[5].Set(bbx[1].x() + clearance, bbx[0].y() - clearance, bbx[1].z() + clearance);
	corner[6].Set(bbx[0].x() - clearance, bbx[1].y() + clearance, bbx[1].z() + clearance);
	corner[7].Set(bbx[1].x() + clearance, bbx[1].y() + clearance, bbx[1].z() + clearance);

	YsPlane bbxPlane[6];
	bbxPlane[0].MakePlaneFromTriangle(corner[4], corner[5], corner[6]); //Front: 4, 5, 6, 7
	bbxPlane[1].MakePlaneFromTriangle(corner[0], corner[1], corner[2]); //Back: 0, 1, 2, 3
	bbxPlane[2].MakePlaneFromTriangle(corner[0], corner[2], corner[4]); //Right: 0, 2, 4, 6
	bbxPlane[3].MakePlaneFromTriangle(corner[1], corner[3], corner[5]); //Left: 1, 3, 5, 7
	bbxPlane[4].MakePlaneFromTriangle(corner[2], corner[3], corner[6]); //Top: 2, 3, 6, 7
	bbxPlane[5].MakePlaneFromTriangle(corner[0], corner[1], corner[4]); //Bottom: 0, 1, 4, 5

	YsMatrix4x4 nulMat;
	nulMat.Initialize();
	for (int i = 0; i < 6; i++)
	{
		if (YsGetTwoPlaneCrossLine(lineOrig, lineDir, bbxPlane[i], pln) == YSOK)
		{
			if (CheckInfiniteLineBoundingBoxIntersect(bbx, nulMat, lineOrig,lineDir,clearance) == YSTRUE)
			{
				return YSTRUE;
			}
		}
	}
	return YSFALSE;
}

YSBOOL FsPhysics::Check2BoundingBoxIntersect(YsVec3 bbx1[2], YsMatrix4x4 mat1, YsVec3 bbx2[2], YsMatrix4x4 mat2, double clearance)
{
	//This uses untransformed bounding box min/max
	YsVec3 corner[8];
	corner[0].Set(bbx2[0].x(), bbx2[0].y(), bbx2[0].z());
	corner[1].Set(bbx2[1].x(), bbx2[0].y(), bbx2[0].z());
	corner[2].Set(bbx2[0].x(), bbx2[1].y(), bbx2[0].z());
	corner[3].Set(bbx2[1].x(), bbx2[1].y(), bbx2[0].z());
	corner[4].Set(bbx2[0].x(), bbx2[0].y(), bbx2[1].z());
	corner[5].Set(bbx2[1].x(), bbx2[0].y(), bbx2[1].z());
	corner[6].Set(bbx2[0].x(), bbx2[1].y(), bbx2[1].z());
	corner[7].Set(bbx2[1].x(), bbx2[1].y(), bbx2[1].z());

	YSBOOL allAbove = YSTRUE, allBelow = YSTRUE, allLeft = YSTRUE, allRight = YSTRUE, allAhead = YSTRUE, allBehind = YSTRUE;
	YsMatrix4x4 invMat = mat1;
	invMat.Invert();

	YsMatrix4x4 tfm = invMat * mat2;

	const double& xMin = bbx1[0].x() - clearance, & xMax = bbx1[1].x() + clearance;
	const double& yMin = bbx1[0].y() - clearance, & yMax = bbx1[1].y() + clearance;
	const double& zMin = bbx1[0].z() - clearance, & zMax = bbx1[1].z() + clearance;

	for (int i = 0; i < 8; i++)
	{
		YsVec3 tst;
		tfm.Mul(tst, corner[i], 1.0);
		if (tst.x() <= xMax)
		{
			allRight = YSFALSE;
		}
		if (xMin <= tst.x())
		{
			allLeft = YSFALSE;
		}

		if (tst.y() <= yMax)
		{
			allAbove = YSFALSE;
		}
		if (yMin <= tst.y())
		{
			allBelow = YSFALSE;
		}

		if (tst.z() <= zMax)
		{
			allBehind = YSFALSE;
		}
		if (zMin <= tst.z())
		{
			allAhead = YSFALSE;
		}
	}

	if (YSTRUE == allAbove ||
		YSTRUE == allBelow ||
		YSTRUE == allLeft ||
		YSTRUE == allRight ||
		YSTRUE == allBehind ||
		YSTRUE == allAhead)
	{
		return YSFALSE;
	}

	return YSTRUE;
}

YSBOOL FsPhysics::CheckLineShellIntersect(FsVisualSrf shell, YsVec3 startPos, YsVec3 endPos, double clearance, YsVec3& collPos, YsShellPolygonHandle* face)
{
	//This uses TransformedCollisionShell and is in global matrix
	YsShell::PolygonHandle* plHd = &shell.ShootRayH(collPos, startPos, endPos - startPos);

	if (plHd != NULL && YsCheckInBetween3(collPos, endPos, startPos) == YSTRUE)
	{
		face = plHd;
		return YSTRUE;
	}
	return YSFALSE;
}

YSBOOL FsPhysics::CheckTriangleShellIntersect(FsVisualSrf* shell1, double clearance, YsVec3& collPosLocal, YsVec3 tri[3], YsShellPolygonHandle* face)
{
	YsPlane pln;
	pln.MakePlaneFromTriangle(tri[0], tri[1], tri[2]);

	YsVec3 intersection;
	YsArray <YsVec3> collisions;
	shell1->EnableSearch();

	for (auto edge : shell1->AllEdge())
	{
		YsShell::VertexHandle start, end;
		shell1->GetEdge(start, end, edge);
		YsVec3 startPos, endPos;

		shell1->GetVertexPosition(startPos, start);
		shell1->GetVertexPosition(endPos, end);

		if (pln.GetIntersection(intersection, startPos, endPos - startPos))
		{
			if (YsCheckInsideTriangle3(intersection, tri) != YSOUTSIDE && YsCheckInBetween3(intersection, startPos, endPos) == YSTRUE)
			{
				collisions.Add(intersection);
			}
		}
	}

	if (collisions.GetN() > 0)
	{
		YsVec3 net = YsOrigin();
		for (int i = 0; i < collisions.GetN(); i++)
		{
			net += collisions[i];
		}
		collPosLocal = net/collisions.GetN();

		return YSTRUE;
	}
	return YSFALSE;
}

YSBOOL FsPhysics::CheckShellShellIntersect(const FsVisualSrf& shell1, const FsVisualSrf& shell2,
	double clearance, YsVec3& collPos, YsShellPolygonHandle& face1, YsShellPolygonHandle& face2)
{
	//This uses TransformedCollisionShell and is in global matrix
	//YsShellCollisionEx returns the first found intersection point of:
	//  An edge of obj1-poly1 intersect obj2-poly1
	//  An edge of obj2-poly1 intersect obj1-poly1
	//  An edge of obj1-poly2 intersect obj2-poly1 etc
	//This means the intersection point could be on either side of the true midpoint between objects
	
	YsShellExt::ConstConverter conv1, conv2;
	conv1 = shell1.Conv();
	conv2 = shell2.Conv();


	//20260804 Pulled from ysshell.cpp YsCheckShellCollisionEx() and modified
	//Eventually these changes should go back to ysshell.cpp, but it's in ysce_public and I am lazy
	
	//For some reason this is only returning one collision per wizard using coll shell, but bbx shell calcs four collisions per wizard
	YsShellPolygonHandle plHd1, plHd2;
	int nv1, nv2;
	YsArray <YsVec3, 256> v1, v2;
	YsCollisionOfPolygon wizard;

	collPos = YsOrigin();
	face1 = NULL;
	face2 = NULL;

	YsArray <YsVec3> points;
	YsVec3 hold;
	points.CleanUp();

	plHd1 = NULL;
	while ((plHd1 = shell1.FindNextPolygon(plHd1)) != NULL)
	{
		nv1 = shell1.GetNumVertexOfPolygon(plHd1);
		shell1.GetVertexListOfPolygon(v1, plHd1);
		wizard.SetPolygon1(nv1, v1);
		YsVec3 v;
		shell1.GetCenterOfPolygon(v, plHd1);
	
		plHd2 = NULL;
		while ((plHd2 = shell2.FindNextPolygon(plHd2)) != NULL)
		{
			shell2.GetCenterOfPolygon(v, plHd2);
	
			nv2 = shell2.GetNumVertexOfPolygon(plHd2);
			shell2.GetVertexListOfPolygon(v2, plHd2);
			wizard.SetPolygon2(nv2, v2);

			if (wizard.CheckCollision(hold) == YSTRUE)
			{
				face1 = plHd1;
				face2 = plHd2;
				points.Add(hold);
				collPos = hold;
			}
		}
	}
	
	if (points.GetN() > 0)
	{
		return YSTRUE;
	}
	return YSFALSE;
}

YSBOOL FsPhysics::CheckShellShellIntersectAndDepth(const FsVisualSrf& shell1, const FsVisualSrf& shell2, double clearance, YsVec3& collPosGlobal, YsShellPolygonHandle& face1, YsShellPolygonHandle& face2, double& depth1, double& depth2)
{
	YsShellPolygonHandle hold1, hold2, plHd1, plHd2;
	hold1 = plHd1;
	if (CheckShellShellIntersect(shell1, shell2, clearance, collPosGlobal, plHd1, plHd2) != YSTRUE)
	{
		return YSFALSE;
	}

	const YsShell::VertexHandle* vtHd1, * vtHd2;
	const YsShellVertex* vtx;
	YsArray <YsVec3> verts1, verts2, vertTrans;
	const YsShellPolygon* plg1 = nullptr;
	const YsShellPolygon* plg2 = nullptr;

	if (hold1 != plHd1) //If equal, plHd1 hasn't been updated by CheckShellShellItc
	{
		plg1 = shell1.GetPolygon(plHd1);

		if (plg1 != nullptr)
		{
			vtHd1 = plg1->GetVertexArray();
			vtx = shell1.GetVertex(vtHd1[0]);
			
			for (int i = 0; i < plg1->GetNumVertex(); i++)
			{
				verts1.Increment();
				verts1.Last() = shell1.GetVertexPosition(vtHd1[i]);
			}
		}
	}

	if (hold2 != plHd2)
	{
		plg2 = shell2.GetPolygon(plHd2);
		if (plg2 != nullptr)
		{
			vtHd2 = plg2->GetVertexArray();
			vtx = shell2.GetVertex(vtHd2[0]);
			
			for (int i = 0; i < plg2->GetNumVertex(); i++)
			{
				verts2.Increment();
				verts2.Last() = shell2.GetVertexPosition(vtHd2[i]);
			}
		}
	}

	if (plg1 != nullptr && plg2 != nullptr)
	{
		YsMatrix4x4 mat;
		YsAtt3 att;
		double farPos, farNeg;

		mat.Initialize();
		att.SetUpVector(shell2.GetNormal(plHd2)); //shell2.GetNormal() returns in global matrix. plg2->GetNormal() returns in local matrix
		att.SetForwardVector(shell2.GetNormal(plHd2).GetArbitraryPerpendicularVector());
		mat.Rotate(att);
		vertTrans.CleanUp();
		vertTrans.Increment();
		mat.Mul(vertTrans.Last(), verts1[0] - collPosGlobal, 1.0);
		farPos = vertTrans.Last().y();
		farNeg = vertTrans.Last().y();
		
		for (int i = 1; i < verts1.GetN(); i++)
		{
			vertTrans.Increment();
			mat.Mul(vertTrans[i], verts1[i] - collPosGlobal, 1.0);
			
			if (YsCheckInsidePolygon3(vertTrans.Last(), verts2.GetN(), verts2))
			{
				farPos = YsGreater(vertTrans.Last().y(), farPos);
				farNeg = YsSmaller(vertTrans.Last().y(), farNeg);				
			}
		}
		depth1 = abs(farPos) > abs(farNeg) ? abs(farNeg) : abs(farPos);
		
		mat.Initialize();
		att.SetUpVector(shell1.GetNormal(plHd1));
		att.SetForwardVector(shell1.GetNormal(plHd1).GetArbitraryPerpendicularVector());
		mat.Rotate(att);
		vertTrans.CleanUp();
		vertTrans.Increment();
		mat.Mul(vertTrans.Last(), verts2[0] - collPosGlobal, 1.0);
		farPos = vertTrans.Last().y();
		farNeg = vertTrans.Last().y();
		for (int i = 1; i < verts2.GetN(); i++)
		{
			mat.Mul(vertTrans.Last(), verts2[i] - collPosGlobal, 1.0);
			
			if (YsCheckInsidePolygon3(vertTrans.Last(), verts1.GetN(), verts1))
			{
				farPos = YsGreater(vertTrans.Last().y(), farPos);
				farNeg = YsSmaller(vertTrans.Last().y(), farNeg);
			}
		}
		depth2 = abs(farPos) > abs(farNeg) ? abs(farNeg) : abs(farPos);
		YsVec3 p1, p2, pt;
		p1 = depth1 * shell2.GetNormal(plHd2);
		p2 = depth2 * shell1.GetNormal(plHd1);
		pt = p1 + p2;
	}
	face1 = plHd1;
	face2 = plHd2;
	return YSTRUE;
}

YSBOOL FsPhysics::CheckShellShellIntersection(const FsVisualSrf& shell1, const FsVisualSrf& shell2, double clearance)
{
	YsVec3 unusedPos;
	YsShellPolygonHandle unusedHandle;
	double unusedDouble;
	return CheckShellShellIntersectAndDepth(shell1, shell2, clearance, unusedPos, unusedHandle, unusedHandle, unusedDouble, unusedDouble);
}

YSRESULT FsPhysics::ConvertBbxToShell(YsVec3 bbx[2], YsMatrix4x4 transform, FsVisualSrf& localShell, FsVisualSrf& transformedShell)
{
	YsVec3 corner[8];
	corner[0].Set(bbx[0].x(), bbx[0].y(), bbx[0].z());
	corner[1].Set(bbx[1].x(), bbx[0].y(), bbx[0].z());
	corner[2].Set(bbx[0].x(), bbx[1].y(), bbx[0].z());
	corner[3].Set(bbx[1].x(), bbx[1].y(), bbx[0].z());
	corner[4].Set(bbx[0].x(), bbx[0].y(), bbx[1].z());
	corner[5].Set(bbx[1].x(), bbx[0].y(), bbx[1].z());
	corner[6].Set(bbx[0].x(), bbx[1].y(), bbx[1].z());
	corner[7].Set(bbx[1].x(), bbx[1].y(), bbx[1].z());

	YsShellVertexHandle verts[4];
	YsShellPolygonHandle hdl;
	YsVec3 nom;
	//Front: 4, 5, 6, 7
	verts[0].CopyFrom(localShell.AddVertex(corner[4]));
	verts[1].CopyFrom(localShell.AddVertex(corner[5]));
	verts[2].CopyFrom(localShell.AddVertex(corner[6]));
	verts[3].CopyFrom(localShell.AddVertex(corner[7]));
	hdl = localShell.AddPolygon(4,verts);
	nom.Set(0.0, 0.0, 1.0);
	localShell.SetPolygonNormal(hdl, nom);

	//Back: 0, 1, 2, 3
	verts[0].CopyFrom(localShell.AddVertex(corner[0]));
	verts[1].CopyFrom(localShell.AddVertex(corner[1]));
	verts[2].CopyFrom(localShell.AddVertex(corner[2]));
	verts[3].CopyFrom(localShell.AddVertex(corner[3]));
	hdl = localShell.AddPolygon(4, verts);
	nom.Set(0.0, 0.0, -1.0);
	localShell.SetPolygonNormal(hdl, nom);
	
	//Right: 0, 2, 4, 6
	verts[0].CopyFrom(localShell.AddVertex(corner[0]));
	verts[1].CopyFrom(localShell.AddVertex(corner[2]));
	verts[2].CopyFrom(localShell.AddVertex(corner[4]));
	verts[3].CopyFrom(localShell.AddVertex(corner[6]));
	hdl = localShell.AddPolygon(4, verts);
	nom.Set(1.0, 0.0, 0.0);
	localShell.SetPolygonNormal(hdl, nom);
	
	//Left: 1, 3, 5, 7
	verts[0].CopyFrom(localShell.AddVertex(corner[1]));
	verts[1].CopyFrom(localShell.AddVertex(corner[3]));
	verts[2].CopyFrom(localShell.AddVertex(corner[5]));
	verts[3].CopyFrom(localShell.AddVertex(corner[7]));
	hdl = localShell.AddPolygon(4, verts);
	nom.Set(-1.0, 0.0, 0.0);
	localShell.SetPolygonNormal(hdl, nom);
	
	//Top: 2, 3, 6, 7
	verts[0].CopyFrom(localShell.AddVertex(corner[2]));
	verts[1].CopyFrom(localShell.AddVertex(corner[3]));
	verts[2].CopyFrom(localShell.AddVertex(corner[6]));
	verts[3].CopyFrom(localShell.AddVertex(corner[7]));
	hdl = localShell.AddPolygon(4, verts);
	nom.Set(0.0, 1.0, 0.0);
	localShell.SetPolygonNormal(hdl, nom);
	
	//Bottom: 0, 1, 4, 5
	verts[0].CopyFrom(localShell.AddVertex(corner[0]));
	verts[1].CopyFrom(localShell.AddVertex(corner[1]));
	verts[2].CopyFrom(localShell.AddVertex(corner[4]));
	verts[3].CopyFrom(localShell.AddVertex(corner[5]));
	hdl = localShell.AddPolygon(4, verts);
	nom.Set(0.0, -1.0, 0.0);
	localShell.SetPolygonNormal(hdl, nom);

	// Transformed //////////////////////////////////////////////////

	for (int i = 0; i < 8; i++)
	{
		transform.Mul(corner[i], corner[i], 1.0);
	}

	//Front: 4, 5, 6, 7
	verts[0].CopyFrom(transformedShell.AddVertex(corner[4]));
	verts[1].CopyFrom(transformedShell.AddVertex(corner[5]));
	verts[2].CopyFrom(transformedShell.AddVertex(corner[6]));
	verts[3].CopyFrom(transformedShell.AddVertex(corner[7]));
	transformedShell.AddPolygon(4, verts);

	//Back: 0, 1, 2, 3
	verts[0].CopyFrom(transformedShell.AddVertex(corner[0]));
	verts[1].CopyFrom(transformedShell.AddVertex(corner[1]));
	verts[2].CopyFrom(transformedShell.AddVertex(corner[2]));
	verts[3].CopyFrom(transformedShell.AddVertex(corner[3]));
	transformedShell.AddPolygon(4, verts);

	//Right: 0, 2, 4, 6
	verts[0].CopyFrom(transformedShell.AddVertex(corner[0]));
	verts[1].CopyFrom(transformedShell.AddVertex(corner[2]));
	verts[2].CopyFrom(transformedShell.AddVertex(corner[4]));
	verts[3].CopyFrom(transformedShell.AddVertex(corner[6]));
	transformedShell.AddPolygon(4, verts);

	//Left: 1, 3, 5, 7
	verts[0].CopyFrom(transformedShell.AddVertex(corner[1]));
	verts[1].CopyFrom(transformedShell.AddVertex(corner[3]));
	verts[2].CopyFrom(transformedShell.AddVertex(corner[5]));
	verts[3].CopyFrom(transformedShell.AddVertex(corner[7]));
	transformedShell.AddPolygon(4, verts);

	//Top: 2, 3, 6, 7
	verts[0].CopyFrom(transformedShell.AddVertex(corner[2]));
	verts[1].CopyFrom(transformedShell.AddVertex(corner[3]));
	verts[2].CopyFrom(transformedShell.AddVertex(corner[6]));
	verts[3].CopyFrom(transformedShell.AddVertex(corner[7]));
	transformedShell.AddPolygon(4, verts);

	//Bottom: 0, 1, 4, 5
	verts[0].CopyFrom(transformedShell.AddVertex(corner[0]));
	verts[1].CopyFrom(transformedShell.AddVertex(corner[1]));
	verts[2].CopyFrom(transformedShell.AddVertex(corner[4]));
	verts[3].CopyFrom(transformedShell.AddVertex(corner[5]));
	transformedShell.AddPolygon(4, verts);

	return YSOK;
}

YsVisualSrf FsPhysics::ConvertFsVisToYsVis(FsVisualSrf* in)
{
	YsVisualSrf result;
	result.CopyFrom(*in);

	return result;
}

FsVisualSrf FsPhysics::ConvertYsVisToFsVis(YsVisualSrf* in)
{
	FsVisualSrf result;
	result.CopyFrom(*in);

	return result;
}

YSRESULT FsPhysics::PushObjectsFromCollision(CollisionEvent* coll, YsVec3 out[2])
{
	//Currently only works with a single face in a collmesh because of how Soji's collision detection works	
	//This also doesn't properly handle two one-face wings meeting at their leading edges
	//Have to factor in relative velocity rather than normal for the push direction
	//And also have a small settling nudge in normal direction so overlapping faces smoothly deconflict over multiple steps

	double cF = 0.05; //coefficient of friction for collision
	double cR = 0.01; //coefficient of restitution; how bouncy is the collision?
	double hardness = 1.0; //damage scaler, apply to dV after setting velocity
	double roughness = 0.0; //Add random bounce to vel and att

	if (coll->objTwo == nullptr) //objOne hit terrain
	{
		YSSCNAREATYPE areaType = sim->GetAreaType(coll->objOne->GetPosition());


		//Calculate something that vaguely resembles friction
		const FsField* fld;
		fld = sim->GetField();

		if (areaType == YSSCNAREA_WATER)
		{ //Unpaved water
			cF = 0.2;
			cR = 0.0;
			hardness = -1.0;
			roughness = 0.2;
		}
		else
		{ //Unpaved land
			cF = 0.5;
			cR = 0.01;
			hardness = 0.75;
			roughness = 0.25;
		}

		YsArray <const YsSceneryRectRegion*, 16> rgn;
		if (fld->GetFieldRegion(rgn, coll->objOne->GetPosition().x(), coll->objOne->GetPosition().z()) == YSOK)
		{
			int i;
			for (i = 0; i < rgn.GetN(); i++)
			{
				if (sim->IsSafeTerrainRegionId(rgn[i]->GetId()) == YSTRUE)
				{
					//Paved, whether LAND or WATER
					cF = 0.1;
					cR = 0.05;
					hardness = 1.0;
					roughness = 0.01;
					break;
				}
			}
		}
	}

	//////////////////////////////////////////////////////////////////////

	YsMatrix4x4 mat;
	YsAtt3 att, rotVel1, rotVel2;
	YsVec3 initVel, faceVel, bounceVel, outVel;
	YsVec3 deltaV1 = YsOrigin();
	YsVec3 deltaV2 = YsOrigin();
	double bounce;
	YsVec3 inVel1, inVel2, pointVel1, pointVel2, netVel1, netVel2, totalVel;
	YsVec3 locPos1, locPos2;
	YsVec3 roughnessVel;
	YsAtt3 roughnessAtt;

	if (coll->objOne != nullptr)
	{
		coll->objOne->CommonProp().GetVelocity(inVel1);

		if (coll->objOne->IsAirplane() == YSTRUE)
		{
			FsAirplane* air = (FsAirplane*)coll->objOne;
			air->Prop().GetRotationSpeed(rotVel1);
		}
		else
		{
			FsGround* gnd = (FsGround*)coll->objOne;
			rotVel1 = YsZeroAtt();
			rotVel1.SetH(gnd->Prop().GetRotation()); //This only returns rotation around vertical axis
			//Need a new function to output: staRotation, chConstHeadingRotation, chConstPitchRotation, chConstBankRotation
		}

		//Get collpos tangential velocity
		coll->objOne->GetMatrix().MulInverse(locPos1, coll->collPos, 1.0);
		pointVel1 = ConvertAngularToLinearVelocity(rotVel1, locPos1);

		//translate out of local coords
		mat.Initialize();
		mat.Rotate(coll->objOne->GetAttitude());
		mat.Mul(pointVel1, pointVel1, 1.0);
	}
	else
	{
		inVel1 = YsOrigin();
		rotVel1 = YsZeroAtt();
		pointVel1 = YsOrigin();
	}

	if (coll->objTwo != nullptr)
	{
		coll->objTwo->CommonProp().GetVelocity(inVel2);
		if (coll->objTwo->IsAirplane() == YSTRUE)
		{
			FsAirplane* air = (FsAirplane*)coll->objTwo;
			air->Prop().GetRotationSpeed(rotVel2);
		}
		else
		{
			FsGround* gnd = (FsGround*)coll->objTwo;
			rotVel2 = YsZeroAtt();
			rotVel2.SetH(gnd->Prop().GetRotation());
		}

		YsVec3 locPos;
		coll->objTwo->GetMatrix().MulInverse(locPos, coll->collPos, 1.0);
		pointVel2 = ConvertAngularToLinearVelocity(rotVel2, locPos);

		mat.Initialize();
		mat.Rotate(coll->objTwo->GetAttitude());
		mat.Mul(pointVel2, pointVel2, 1.0);
	}
	else
	{
		inVel2 = YsOrigin();
		rotVel2 = YsZeroAtt();
		pointVel2 = YsOrigin();
	}

	netVel1 = inVel1 + pointVel1;
	netVel2 = inVel2 + pointVel2;

	if (coll->objOne != nullptr)
	{
		mat.Initialize();
		att.SetUpVector(coll->nomTwo.GetArbitraryPerpendicularVector());
		att.SetForwardVector(coll->nomTwo);
		mat.Rotate(att);
		
		//When aircraft is over carrier deck, GetVelocity returns velocity relative to carrier for some reason
		if (coll->objOne->CommonProp().IsOnCarrier() == YSTRUE)
		{
			totalVel = netVel1 - pointVel2;
		}
		else
		{
			totalVel = netVel1 - netVel2;
		}
		mat.MulInverse(faceVel, totalVel, 1.0);

		//If collPos is moving away from the face, don't bounce
		//Normal+ is toward obj, set when coll data is written
		if (faceVel.z() < 0.0)
		{
			//Calculate push
			YsVec3 push;
			push = coll->nomTwo * coll->vertDepthOne * coll->pushOne;

			//Compute rotation
			//Use closing speed (totalVel) in obj1 coordinates:
			YsVec3 localVel;
			YsMatrix4x4 forwardMat;
			forwardMat.Initialize();
			forwardMat.Rotate(coll->objOne->GetAttitude());
			forwardMat.MulInverse(localVel, totalVel, 1.0);
			localVel *= -1; //Switch from object velocity to point velocity

			YsVec3 dV, bbx[2];
			YsAtt3 torque;
			coll->objOne->GetMatrix().MulInverse(locPos1, coll->collPos, 1.0);
			
			const YsVec3* aaa = coll->objOne->GetCollisionShellBbx();
			bbx[0] = aaa[0];
			bbx[1] = aaa[1];
			
			//Janky scaling to account for friction torque rather than impact torque
			localVel *= YsSmaller(abs(faceVel.z()) / 10,1.0);

			ConvertLinearToAngularVelocity(torque, locPos1, localVel, bbx, dV);
			
			torque.SetH(torque.h() * 0.95); //fake some energy loss
			torque.SetP(torque.p() * 0.95);
			torque.SetB(torque.b() * 0.95);
			
			//Push position
			coll->objOne->CommonProp().SetPosition(coll->objOne->CommonProp().GetPosition() + push);
			coll->objOne->CommonProp().RemakeMatrix();

			//Push attitude
			//Once multi-point collisions exist, need to settle attitude so all points are on their colliding faces
			
			//Apply rotation velocity
			if (coll->objOne->IsAirplane() == YSTRUE)
			{
				FsAirplane* air = (FsAirplane*)coll->objOne;
				YsAtt3 old = rotVel1;
				//Attenuate angular velocity for eventual static stability when sitting on surface
				old.SetH(old.h() * 0.99);
				old.SetP(old.p() * 0.99);
				old.SetB(old.b() * 0.99);

				roughnessAtt.SetH((((double)(rand() % 10000) / 5000.0) - 1) * roughness);
				roughnessAtt.SetP((((double)(rand() % 10000) / 5000.0) - 1) * roughness);
				roughnessAtt.SetB((((double)(rand() % 10000) / 5000.0) - 1) * roughness);
				torque.SetH(torque.h() * (1 + roughnessAtt.h()));
				torque.SetP(torque.p() * (1 + roughnessAtt.p()));
				torque.SetB(torque.b() * (1 + roughnessAtt.b()));

				old.NoseUp(torque.p()); //Why does att3 + att3 syntax not exist?
				old.YawLeft(torque.h());
				old.AddB(torque.b());
				air->Prop().SetRotationSpeed(old);
			}
			else
			{
				//No rotation for you right now
			}

			//Bounce velocity
			forwardMat.Mul(dV, dV, 1.0); //dV in global
			YsVec3 dVface;
			mat.MulInverse(dVface, dV, 1.0); //dV in facemat
			
			bounce = dVface.z() * (1 - coll->pushOne - cR * coll->pushOne);

			bounceVel.SetZ((faceVel.z() + bounce)* -1);

			if (hardness < 0.0) //hit water. Scale hardness with normal velocity
			{
				hardness = YsSmaller(bounceVel.z(), 10.0) / 10.0;
			}

			//Friction force
			//Because we're using velocity instead of force, friction force is very low when sitting on a surface
			//You gotta crank those numbers up. Those are rookie numbers
			double slow = YsGreater(cF * (bounce - dVface.z()), cF);
			bounceVel.SetX(YsSmaller(slow, abs(dVface.x())) * YsSign(dVface.x()));
			bounceVel.SetY(YsSmaller(slow, abs(dVface.y())) * YsSign(dVface.y()));

			roughnessVel.SetX((((double)(rand() % 10000) / 5000.0) - 1) * roughness);
			roughnessVel.SetY((((double)(rand() % 10000) / 5000.0) - 1) * roughness);
			roughnessVel.SetZ((((double)(rand() % 10000) / 20000.0) - 0.1) * roughness); // -0.1 to +0.4 * roughness

			bounceVel.MulX(1 + roughnessVel.x());
			bounceVel.MulY(1 + roughnessVel.y());
			bounceVel.MulZ(1 + roughnessVel.z());

			mat.Mul(bounceVel, bounceVel, 1.0);

			outVel = inVel1;
			outVel.AddX(YsSmaller(abs(inVel1.x()), abs(bounceVel.x())) * YsSignInv(totalVel.x()));
			outVel.AddY(YsSmaller(abs(inVel1.y()), abs(bounceVel.y())) * YsSignInv(totalVel.y()));
			outVel.AddZ(YsSmaller(abs(inVel1.z()), abs(bounceVel.z())) * YsSignInv(totalVel.z()));		
			coll->objOne->CommonProp().SetVelocity(outVel);
			deltaV1 = bounceVel * hardness;
		}
	}

	if (coll->objTwo != nullptr)
	{
		mat.Initialize();
		att.SetUpVector(coll->nomOne.GetArbitraryPerpendicularVector());
		att.SetForwardVector(coll->nomOne);
		mat.Rotate(att);

		//When aircraft is over carrier deck, GetVelocity returns velocity relative to carrier for some reason
		if (coll->objTwo->CommonProp().IsOnCarrier() == YSTRUE)
		{
			totalVel = netVel2 - pointVel1;
		}
		else
		{
			totalVel = netVel2 - netVel1;
		}
		mat.MulInverse(faceVel, totalVel, 1.0);

		//If collPos is moving away from the face, don't bounce
		//Normal+ is toward obj, set when coll data is written
		if (faceVel.z() < 0.0)
		{
			//Calculate push
			YsVec3 push;
			push = coll->nomOne * coll->vertDepthTwo * coll->pushTwo;

			//Compute rotation
			//Use closing speed (totalVel) in obj1 coordinates:
			YsVec3 localVel;
			YsMatrix4x4 forwardMat;
			forwardMat.Initialize();
			forwardMat.Rotate(coll->objTwo->GetAttitude());
			forwardMat.MulInverse(localVel, totalVel, 1.0);
			localVel *= -1; //Switch from object velocity to point velocity

			YsVec3 dV, bbx[2];
			YsAtt3 torque;
			coll->objTwo->GetMatrix().MulInverse(locPos2, coll->collPos, 1.0);

			const YsVec3* aaa = coll->objTwo->GetCollisionShellBbx();
			bbx[0] = aaa[0];
			bbx[1] = aaa[1];

			//Janky scaling to account for friction torque rather than impact torque
			localVel *= YsSmaller(abs(faceVel.z()) / 10, 1.0);

			ConvertLinearToAngularVelocity(torque, locPos2, localVel, bbx, dV);

			torque.SetH(torque.h() * 0.95); //fake some energy loss
			torque.SetP(torque.p() * 0.95);
			torque.SetB(torque.b() * 0.95);

			//Push position
			coll->objTwo->CommonProp().SetPosition(coll->objTwo->CommonProp().GetPosition() + push);
			coll->objTwo->CommonProp().RemakeMatrix();

			//Push attitude
			//Once multi-point collisions exist, need to settle attitude so all points are on their colliding faces

			//Apply rotation velocity
			if (coll->objTwo->IsAirplane() == YSTRUE)
			{
				FsAirplane* air = (FsAirplane*)coll->objTwo;
				YsAtt3 old = rotVel2;
				//Attenuate angular velocity for eventual static stability when sitting on surface
				old.SetH(old.h() * 0.99);
				old.SetP(old.p() * 0.99);
				old.SetB(old.b() * 0.99);

				roughnessAtt.SetH((((double)(rand() % 10000) / 5000.0) - 1) * roughness);
				roughnessAtt.SetP((((double)(rand() % 10000) / 5000.0) - 1) * roughness);
				roughnessAtt.SetB((((double)(rand() % 10000) / 5000.0) - 1) * roughness);
				torque.SetH(torque.h() * (1 + roughnessAtt.h()));
				torque.SetP(torque.p() * (1 + roughnessAtt.p()));
				torque.SetB(torque.b() * (1 + roughnessAtt.b()));

				old.NoseUp(torque.p()); //Why does att3 + att3 syntax not exist?
				old.YawLeft(torque.h());
				old.AddB(torque.b());
				air->Prop().SetRotationSpeed(old);
			}
			else
			{
				//No rotation for you right now
			}

			//Bounce velocity
			forwardMat.Mul(dV, dV, 1.0); //dV in global
			YsVec3 dVface;
			mat.MulInverse(dVface, dV, 1.0); //dV in facemat

			bounce = dVface.z() * (1 - coll->pushTwo - cR * coll->pushTwo);

			bounceVel.SetZ((faceVel.z() + bounce) * -1);

			if (hardness < 0.0) //hit water. Scale hardness with normal velocity
			{
				hardness = YsSmaller(bounceVel.z(), 10.0) / 10.0;
			}

			//Friction force
			//Because we're using velocity instead of force, friction force is very low when sitting on a surface
			//You gotta crank those numbers up. Those are rookie numbers
			double slow = YsGreater(cF * (bounce - dVface.z()), cF);
			bounceVel.SetX(YsSmaller(slow, abs(dVface.x())) * YsSign(dVface.x()));
			bounceVel.SetY(YsSmaller(slow, abs(dVface.y())) * YsSign(dVface.y()));

			roughnessVel.SetX((((double)(rand() % 10000) / 5000.0) - 1) * roughness);
			roughnessVel.SetY((((double)(rand() % 10000) / 5000.0) - 1) * roughness);
			roughnessVel.SetZ((((double)(rand() % 10000) / 20000.0) - 0.1) * roughness); // -0.1 to +0.4 * roughness

			bounceVel.MulX(1 + roughnessVel.x());
			bounceVel.MulY(1 + roughnessVel.y());
			bounceVel.MulZ(1 + roughnessVel.z());

			mat.Mul(bounceVel, bounceVel, 1.0);

			outVel = inVel2;
			outVel.AddX(YsSmaller(abs(inVel2.x()), abs(bounceVel.x())) * YsSignInv(totalVel.x()));
			outVel.AddY(YsSmaller(abs(inVel2.y()), abs(bounceVel.y())) * YsSignInv(totalVel.y()));
			outVel.AddZ(YsSmaller(abs(inVel2.z()), abs(bounceVel.z())) * YsSignInv(totalVel.z()));
			coll->objTwo->CommonProp().SetVelocity(outVel);
			deltaV2 = bounceVel * hardness;
		}
	}
	
	out[0] = deltaV1;
	out[1] = deltaV2;

	//Soji's rotate-until-gear-on-deck code
	/*YsVec3 nose, left, right;
	YsVec3 nosevc, leftvc, rightvc;
	YsVec3 nosert, leftrt, rightrt;
	YsVec3 tail, lateral, ev, uv;
	double dy;

	nose = staMatrix * chWheel;
	left = staMatrix * chMainGearL;
	right = staMatrix * chMainGearR;

	nosevc.Set(0.0, chWheel.y(), 0.0);
	leftvc.Set(0.0, chMainGearL.y(), 0.0);
	rightvc.Set(0.0, chMainGearR.y(), 0.0);
	nosevc = staMatrix * nosevc;
	leftvc = staMatrix * leftvc;
	rightvc = staMatrix * rightvc;

	const double gDistLeft = left.y() - staFootAtMainGearL.y();
	const double gDistRight = right.y() - staFootAtMainGearR.y();
	const double gDistWheel = nose.y() - staFootAtWheel.y();

	if (YsTolerance < gDistWheel &&
		YsTolerance < gDistLeft &&
		YsTolerance < gDistRight)
	{
		gDistance = YsSmaller(gDistWheel, gDistLeft);
		gDistance = YsSmaller(gDistance, gDistRight);
		return YSFALSE;
	}
	else
	{
		YSBOOL diggingIn = YSFALSE;
		// Digging In?
		{
			const YsVec3 v1 = staFootAtMainGearR - staFootAtWheel;
			const YsVec3 v2 = staFootAtMainGearL - staFootAtWheel;

			YsVec3 gndNom = v1 ^ v2;
			if (gndNom.y() < 0.0)
			{
				gndNom = -gndNom;
			}
			gndNom.Normalize();

			YsVec3 vNom;
			GetVelocity(vNom);
			vNom.Normalize();

			if (cos(YsPi * 91.0 / 180.0) > vNom * gndNom)
			{
				diggingIn = YSTRUE;
			}
		}



		gDistance = 0.0;

		//Phase 1 If airspeed is faster than chManSpeed1, allow airplane to be lifted a little bit.
		double deepest, airspeed;
		deepest = YsSmaller(gDistWheel, gDistLeft);
		deepest = YsSmaller(deepest, gDistRight);
		deepest = -deepest;
		airspeed = GetVelocity();

		if (YSTRUE != diggingIn)
		{
			if (airspeed > chManSpeed2)
			{
				staPosition.SetY(staPosition.y() + deepest);
				return YSTRUE;
			}
			else if (airspeed > chManSpeed1)
			{
				double dy;
				dy = deepest * (airspeed - chManSpeed1) / (chManSpeed2 - chManSpeed1);
				staPosition.SetY(staPosition.y() + dy);
			}
		}


		//Phase 2 Adjust attitude
		nose = chWheel;  // Force symmetric
		nose.SetX((chMainGearL.x() + chMainGearR.x()) / 2.0);
		nose = staMatrix * nose;

		nose.Set(nose.x(), YsGreater(nose.y(), staFootAtWheel.y()), nose.z());
		left.Set(left.x(), YsGreater(left.y(), staFootAtMainGearL.y()), left.z());
		right.Set(right.x(), YsGreater(right.y(), staFootAtMainGearR.y()), right.z());

		nosert = nose - nosevc;
		leftrt = left - leftvc;
		rightrt = right - rightvc;

		tail = (leftrt + rightrt) / 2.0;

		if (chWheel.z() > chMainGearL.z())
		{
			ev = nosert - tail;
		}
		else
		{
			ev = tail - nosert;
		}

		YsVec3 vSave;
		staInverseMatrix.Mul(vSave, staVelocity, 0.0);

		lateral = leftrt - rightrt;
		uv = lateral ^ ev;

		//Smoothly blend between pitch settling and flying by weight on wheels
		YsVec3 evCurr, uvCurr, evOut, uvOut;
		evCurr = staAttitude.GetForwardVector();
		uvCurr = staAttitude.GetUpVector();

		evOut = GetWeightOnWheels() * ev + (1 - GetWeightOnWheels()) * evCurr;

		staAttitude.SetTwoVector(ev, uv);

		RemakeMatrix();

		staMatrix.Mul(staVelocity, vSave, 0.0);



		// Phase 3 Adjust Position
		nose = staMatrix * chWheel;
		left = staMatrix * chMainGearL;
		right = staMatrix * chMainGearR;

		dy = 0.0;
		if (nose.y() < staFootAtWheel.y())
		{
			dy = YsGreater(staFootAtWheel.y() - nose.y(), dy);
		}
		if (left.y() < staFootAtMainGearL.y())
		{
			dy = YsGreater(staFootAtMainGearL.y() - left.y(), dy);
		}
		if (right.y() < staFootAtMainGearR.y())
		{
			dy = YsGreater(staFootAtMainGearR.y() - right.y(), dy);
		}
		staPosition.SetY(staPosition.y() + dy);
		RemakeMatrix();
		*/
	return YSOK;
}

//////////////////////////////////////////////////
// Post-collision
//Calculate impact forces [determine safe impact]
//Apply impact forces to obj
//Calculate crash

/////////////////////////////////////////////////
// Forces
//Calculate lift
//Calculate drag
//Calculate lateral force

/////////////////////////////////////////////////
/////////////////////////////////////////////////
//Full checks:
		//FsSimulation::CheckMidAir
		//FsSimulation::SimComputeAirToObjCollision
		//FsSimulation::SimProcessCollisionAndTerrain
			// ^ includes gear to deck check

//HTRADIUS overlap check:
		//FsSimulation::MayCollide(const YsVec3& objPos, const double objRad
		//FsSimulation::MayCollideWithAir(const YsVec3 &objPos,const double objRad
		//FsSimulation::MayCollideGround(const YsVec3 &objPos,const double objRad

//Bbox overlap check:
		//FsSimulation::MayCollide(const YsVec3 &objPos,const YsAtt3 &objAtt
		//FsSimulation::MayCollideWithAir(const YsVec3 &objPos,const YsAtt3 &objAtt
		//FsSimulation::MayCollideWithGround(const YsVec3 &objPos,const YsAtt3 &objAtt
		//FsExistence::MayCollideWith
		//FsSimulation::SimTestCollision
			//This is a spawn-safety check for network STPs

//FsSimulation::SimCheckTailStrike
		//FsExistence::TestTailStrike checks plane intersection for non-srfs
			//Plane intersection can falsely report tailstrike if flying over ridge

//Post-impact:
		//fsexistence FsAirplane::Bounce and FsGround::Bounce
			// ^ is prototype collision physics used for ground/ground collisions
		//FsAirplane::HitGround
			//determine safe impact
		//FsAirplane::Crash
			//set post-crash attitude, queue explosions
		//FsAirplaneProperty::BouncePitchByTailStrike

//Forces
	//FsAirplaneProperty::CalculateForce
	//FsAirplaneProperty::CalculateGroundDescelerateDistanceAndTime
	//FsAirplaneProperty::CalculateForceByBrake
	//FsAirplaneProperty::CalculateGravitationalForce
	//FsAirplaneProperty::AdjustGravitationalForceForSlope
	//FsAirplaneProperty::GetConvergentThrust //fsairplaneproperty
	//FsAirplaneProperty::CalculatePropellerThrust //fsairplaneproperty
	//FsAirplaneProperty::GetThrustWeightRatio
	//FsAirplaneProperty::GetAcceleration
	//FsAirplaneProperty::GetG(void)
	//FsAirplaneProperty::GetLateralForce
	//FsAirplaneProperty::GetLift
	//FsAirplaneProperty::GetDrag
	//FsAirplaneProperty::GetThrust //fsairplaneproperty
	//FsAirplaneProperty::GetConvergentThrust //fsairplaneproperty


//Rotation
	//FsAirplaneProperty::CalculateRotationalAcceleration
	//FsAirplaneProperty::CalculateRotation
	//FsAirplaneProperty::CalculateMaxTurnRateOnGround
	//FsAirplaneProperty::CalculateTurnRatiusOnGround
	//FsAirplaneProperty::GetRotationSpeed


//FsAirplaneProperty::CalculateTranslation
//FsAirplaneProperty::CalculateGround
//FsAirplaneProperty::CalculateFuelConsumption
//FsAirplaneProperty::CalculateStall
//FsAirplaneProperty::CalculateWeather
//FsAirplaneProperty::CalculateWindDrift


//FsAirplaneProperty::ComputeAOAForRequiredG
//FsAirplaneProperty::ComputeEstimatedBombLandingPosition
//FsAirplaneProperty::AutoComputeTailStrikePitchAngle //fsairplaneproperty
//FsAirplaneProperty::CalculateCurrentState

int FsPhysics::DoDamageByDeltaV(YsVec3 deltaV)
{
	if (timeStep == 0.0)
	{
		return 0;
	}
	int damage;
	double len , frac, noDamageSpeed;
	len = deltaV.GetLength() * 0.1;
	damage = (int)floor(len);
	frac = (double)len - damage;
	noDamageSpeed = 0.1; //0.1m/s, 2kt

	if (frac > noDamageSpeed && FsGetRandomBetween(0.0, 1.0) * frac > 0.5)
	{
		damage += 1;
	}
	return damage;
}

YsVec3 FsPhysics::ConvertAngularToLinearVelocity(YsAtt3 rot, YsVec3 pos)
{
	YsVec3 vel = YsOrigin();
	double h, p, b;
	h = tan(rot.h());
	vel.AddX(pos.z() * h * -1);
	vel.AddZ(pos.x() * h);

	p = tan(rot.p());
	vel.AddY(pos.z() * p);
	vel.AddZ(pos.y() * p * -1);

	b = tan(rot.b());
	vel.AddX(pos.y() * b * -1);
	vel.AddY(pos.x() * b);
	
	return vel;
}

void FsPhysics::ConvertLinearToAngularVelocity(YsAtt3& rot, YsVec3 pos, YsVec3 vel, YsVec3 bbx[2], YsVec3& deltaV)
{
	YsVec3 limit;
	YsAtt3 torque = YsZeroAtt();
	//For a given push position
	//Scale translation vs rotation ratio as a function of distance to edge of bbx
	YsVec3 ratio; //Ratio high means more rotation. Low is more translation
	limit.SetX(YsGreatestOf(abs(bbx[0].x()), abs(bbx[1].x()), -1.0));
	limit.SetY(YsGreatestOf(abs(bbx[0].y()), abs(bbx[1].y()), -1.0));
	limit.SetZ(YsGreatestOf(abs(bbx[0].z()), abs(bbx[1].z()), -1.0));

	ratio.SetX(abs(pos.x() / limit.x()));
	ratio.SetY(abs(pos.y() / limit.y()));
	ratio.SetZ(abs(pos.z() / limit.z()));
	
	//Get velocity normal to lever arm
	//Make matrix with forward vector along line of origin to pos
	YsMatrix4x4 mat;
	mat.Initialize();
	YsAtt3 m;
	m.SetForwardVector(pos);
	m.SetUpVector(pos.GetArbitraryPerpendicularVector());
	mat.Rotate(m);

	//Rotate velocity into matrix. Take x and y as tangentials. Z is radial
	YsVec3 v, tangential, radial;
	mat.MulInverse(v, vel, 1.0);
	tangential = v;
	tangential.SetZ(0.0);
	radial = v;
	radial.SetX(0.0);
	radial.SetY(0.0);

	//Translate tangential and radial back to obj matrix (global mat here)
	mat.Mul(tangential, tangential, 1.0);
	mat.Mul(radial, radial, 1.0);

	//Distribute force between rotation and movement
	deltaV = radial;
	deltaV.AddX(tangential.x() * (1 - ratio.x()));
	deltaV.AddY(tangential.y() * (1 - ratio.y()));
	deltaV.AddZ(tangential.z() * (1 - ratio.z()));
	
	//Maximum rates
	double yawX, yawZ, pitchY, pitchZ, rollX, rollY;
	yawX =   atan(abs(tangential.z()) / limit.x());
	yawZ =   atan(abs(tangential.x()) / limit.z());
	pitchY = atan(abs(tangential.z()) / limit.y());
	pitchZ = atan(abs(tangential.y()) / limit.z());
	rollX =  atan(abs(tangential.y()) / limit.x());
	rollY =  atan(abs(tangential.x()) / limit.y());

	//Scale rates and set direction. Positive is pitch up, yaw left, roll left
	yawX *=   YsSign(pos.x()) * YsSign(tangential.z()) * ratio.x();
	yawZ *=   YsSign(pos.z()) * YsSign(tangential.x()) * ratio.z() * -1;
	pitchY *= YsSign(pos.y()) * YsSign(tangential.z()) * ratio.y() * -1;
	pitchZ *= YsSign(pos.z()) * YsSign(tangential.y()) * ratio.z();
	rollX *=  YsSign(pos.x()) * YsSign(tangential.y()) * ratio.x();
	rollY *=  YsSign(pos.y()) * YsSign(tangential.x()) * ratio.y() * -1;
	
	torque.SetH(yawX + yawZ);
	torque.SetP(pitchY + pitchZ);
	torque.SetB(rollX + rollY);

	rot = torque;
}

FsPhysics::CollisionEvent::CollisionEvent()
{
	keyOne = -1;
	keyTwo = -1;
	objOne = nullptr;
	objTwo = nullptr;
	nomOne = YsOrigin();
	nomTwo = YsOrigin();
	vertDepthOne = 0;
	vertDepthTwo = 0;
	collPos = YsOrigin();
	pushOne = 0;
	pushTwo = 0;
}