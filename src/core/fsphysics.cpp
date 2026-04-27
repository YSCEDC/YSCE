#include "fsphysics.h"
#include "fssimulation.h"
#include "fsweapon.h"

FsPhysics::FsPhysics()
{
}
void FsPhysics::UpdatePhysics(FsSimulation* currentSim)
{
	sim = currentSim;
	//timeStep = sim->GetTimeStep();
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
	if (CheckLineSegmentBoundingBoxIntersection(bbx, obj->GetMatrix(), start, end, clearance) == YSTRUE)
	{
		YsShellPolygonHandle plHd;
		FsVisualSrf shell = obj->TransformedCollisionShell();

		if (CheckLineShellIntersection(shell, start, end, clearance, collPos, plHd) == YSTRUE)
		{
			return YSTRUE;
		}
	}
	return YSFALSE;
}

YSBOOL FsPhysics::CheckObjectObjectCollision(FsExistence* obj1, FsExistence* obj2, double clearance, YsVec3& collPos)
{
	YsVec3 pos1, pos2, bbx1[2], bbx2[2];
	double rad1, rad2;
	YsMatrix4x4 mat1, mat2;

	pos1 = obj1->GetPosition();
	pos2 = obj2->GetPosition();
	rad1 = obj1->GetApproximatedCollideRadius();
	rad2 = obj2->GetApproximatedCollideRadius();

	if (CheckRadiusIntersection(pos1, rad1, pos2, rad2, clearance) == YSFALSE)
	{
		return YSFALSE;
	}
	//printf("Two objects both alike in being within each others' HTRADII\n");

	bbx1[0] = obj1->GetCollisionShellBbx()[0];
	bbx1[1] = obj1->GetCollisionShellBbx()[1];
	bbx2[0] = obj2->GetCollisionShellBbx()[0];
	bbx2[1] = obj2->GetCollisionShellBbx()[1];
	mat1 = obj1->GetMatrix();
	mat2 = obj2->GetMatrix();	

	if (Check2BoundingBoxIntersection(bbx1, mat1, bbx2, mat2, clearance) == YSFALSE)
	{
		return YSFALSE;
	}
	//printf("Bbxes overlap\n");

	obj1->SetTransformationToCollisionShell(obj1->GetMatrix());
	obj2->SetTransformationToCollisionShell(obj2->GetMatrix());
	FsVisualSrf shell1, shell2;
	YsShellPolygonHandle plHd1, plHd2;
	shell1 = obj1->TransformedCollisionShell();
	shell2 = obj2->TransformedCollisionShell();

	if (CheckShellShellIntersection(&shell1, &shell2, clearance, collPos, plHd1, plHd2) == YSTRUE)
	{
		//Transform collPos into each obj's matrix and save in CommonProp to be used for collision physics calcs in a later step
		//Currently it's stored as global coords in FsAirplane airCollision
		return YSTRUE;
	}
	//printf("No shell intersection\n");
	/*
	YSBOOL FsSimulation::CheckMidAir(YsVec3 &collisionPos,FsExistence &ex1,FsExistence &ex2)
	{
	const YsVec3 *p1=&ex1.GetPosition();
	const YsVec3 *p2=&ex2.GetPosition();
	const double r1=ex1.GetApproximatedCollideRadius();
	const double r2=ex2.GetApproximatedCollideRadius();

	if((*p1-*p2).GetSquareLength()<(r1+r2)*(r1+r2))
	{
		YsShellPolygonHandle plHd1,plHd2;
		if(YSTRUE==ex1.MayCollideWith(ex2) && YSTRUE==ex2.MayCollideWith(ex1))
		{
			if(YsCheckShellCollisionEx(collisionPos,plHd1,plHd2,ex1.TransformedCollisionShell().Conv(),ex2.TransformedCollisionShell().Conv())==YSTRUE)
			{
				return YSTRUE;
			}
		}
	}
	return YSFALSE;
}*/
	/*
	FsSimulation::SimComputeAirToObjCollision(void)
	{
	YsArray <FsAirplane *,256> airCandidate;
	YsArray <FsGround *,256> gndCandidate;

	for(FsAirplane *airPtr=NULL; NULL!=(airPtr=FindNextAirplane(airPtr)); )
	{
		airPtr->airCollision.CleanUp();
		airPtr->gndCollision.CleanUp();
	}
	
	int playerId = FsExistence::GetSearchKey(GetPlayerAirplane());

	for(FsAirplane *air1=NULL; NULL!=(air1=FindNextAirplane(air1)); )
	{
		if (world->GetIsNetClient() == YSFALSE || air1->SearchKey() == playerId) //Skip collision check for non-player aircraft as client
		{
			if (air1->IsAlive() == YSTRUE)
			{
				GetLattice().GetAirCollisionCandidate(airCandidate, air1);

				for (int j = 0; j < airCandidate.GetN(); j++)
				{
					FsAirplane* air2 = airCandidate[j];
					if (YSTRUE == cfgPtr->midAirCollision || YSTRUE == air2->Prop().IsRacingCheckPoint())
					{
						if ((air1->SearchKey() < air2->SearchKey() || world->GetIsNetClient() == YSTRUE) &&
							air2->IsAlive() == YSTRUE &&
							(air1->Prop().IsActive() == YSTRUE || air2->Prop().IsActive() == YSTRUE))  // 2005/03/03
						{
							YsVec3 collPos;
							if (CheckMidAir(collPos, *air1, *air2) == YSTRUE)
							{
								air1->airCollision.Increment();
								air1->airCollision.Last().objKey = air2->SearchKey();
								air1->airCollision.Last().pos = collPos;

								air2->airCollision.Increment();
								air2->airCollision.Last().objKey = air1->SearchKey();
								air2->airCollision.Last().pos = collPos;
							}
						}
					}
				}
			}

			if (air1->GetPosition().y() - air1->GetApproximatedCollideRadius() < tallestGroundObjectHeight)
			{
				GetLattice().GetGndCollisionCandidate(gndCandidate, air1);
				for (int j = 0; j < gndCandidate.GetN(); j++)
				{
					FsGround* gnd2 = gndCandidate[j];
					if (YSTRUE == cfgPtr->midAirCollision || YSTRUE == gnd2->Prop().IsRacingCheckPoint())
					{
						if (gnd2->IsAlive() == YSTRUE)
						{
							YsVec3 collPos;
							if (CheckMidAir(collPos, *air1, *gnd2) == YSTRUE)
							{
								air1->gndCollision.Increment();
								air1->gndCollision.Last().objKey = gnd2->SearchKey();
								air1->gndCollision.Last().pos = collPos;

								gnd2->airCollision.Increment();
								gnd2->airCollision.Last().objKey = air1->SearchKey();
								gnd2->airCollision.Last().pos = collPos;
							}
						}
					}
				}
			}
		}
	}
}*/
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
YSBOOL FsPhysics::CheckObjectTerrainCollision(FsExistence* obj, double clearance, YsVec3& collPosGlobal)
{
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

	//Check ground plane intersection
	if (objPos.y() < objRad + clearance)
	{
		YsPlane pln;
		YsVec3 tri[3], disregard[2];
		tri[0].Set(-99999999999.0, 0.0, 999999999999.0);
		tri[1].Set(-999999999999.0, 0.0, -999999999999.0);
		tri[2].Set(999999999999.0, 0.0, -999999999999.0);
		
		for (int q = 0; q < 3; q++)
		{
			obj->GetMatrix().MulInverse(tri[q], tri[q], 1.0);
		}

		pln.MakePlaneFromTriangle(tri[0], tri[1], tri[2]);
		if (CheckPlaneBoundingBoxIntersection(objBbx, obj->GetMatrix(), pln, clearance, disregard[0],disregard[1]) == YSTRUE)
		{	
			FsVisualSrf coll;
			YsShellPolygonHandle plHd;
			coll = obj->UntransformedCollisionShell();
			if (CheckTriangleShellIntersection(&coll, clearance, collPosLocal, tri, plHd) == YSTRUE)
			{
				obj->GetMatrix().Mul(collPosGlobal, collPosLocal, 1.0);
				//Add collPosGlobal to list of collPos
			}
		}

	}

	//Check terrain items
	YsArray <YsSceneryItem*> candidateList;
	candidateList.CleanUp();
	candidateList.CopyFrom(sim->terrainList);
	YsArray <YsArray <YsElvGridFaceId>> candidateIntersectingFaces;
	YsArray <YsArray<YsVec3>> intersectingTris;
	
	for (int i = 0; i < candidateList.GetN(); i++)
	{
		if (CheckRadiusIntersection(objPos, objRad, candidateList[i]->GetGlobalCenterPosition(), candidateList[i]->GetBbxDiag(), clearance) == YSFALSE)
		{
			candidateList.Delete(i);
			i--;
		}
	}
	if (candidateList.GetN() < 1)
	{
		return YSFALSE;
	}
	
	//Check bounding box overlap with terrain item bbxes
	for (int i = 0; i < candidateList.GetN(); i++)
	{
		YsVec3 terBbx[2];
		YsMatrix4x4 terMat;
		terMat.Translate(candidateList[i]->GetGlobalPosition());
		terMat.Rotate(candidateList[i]->GetGlobalAttitude());
		candidateList[i]->GetBoundingBox(terBbx);
		if (Check2BoundingBoxIntersection(objBbx, obj->GetMatrix(), terBbx, terMat, clearance) == YSFALSE)
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
	for (int i = 0; i < candidateList.GetN(); i++)
	{
		if (candidateList[i]->GetObjType() == YsSceneryItem::ELEVATIONGRID)
		{
			/*const YsElevationGrid *gridData;
			YsSceneryElevationGrid* grid;
			grid = candidateList[i]->GetElevationGrid();
			gridData = grid->GetGridData();
			YsVec2i xz = gridData->GetNumBlock();
			YsVec3 p = candidateList[i]->GetGlobalPosition();
			YsAtt3 a = candidateList[i]->GetGlobalAttitude();*/

			YsMatrix4x4 gridMat;
			gridMat.Initialize();
			gridMat.Translate(candidateList[i]->GetGlobalPosition());
			gridMat.Rotate(candidateList[i]->GetGlobalAttitude());
			YsArray <YsElvGridFaceId> intersectors;

			if (CheckBoundingBoxTerrainIntersection(objBbx, obj->GetMatrix(), candidateList[i]->GetElevationGrid()->GetGridData(), gridMat, clearance, intersectors,intersectingTris) == YSFALSE)
			{
				candidateList.Delete(i);
				i--;
			}
			else
			{
				printf("bbx overlap\n");
				candidateIntersectingFaces.Add(intersectors);
			}
		}
		else if (candidateList[i]->GetObjType() == YsSceneryItem::SHELL)
		{
			//Skip handling surf terrain for now and assume true. Figure out how to handle that later
			//Probably easiest to construct a shell in the bbx coords then check shell/shell intersect on it
		}
	}
	if (candidateList.GetN() < 1)
	{
		return YSFALSE;
	}
	

	//Check object geometry collision with terrain geometry:
	for (int i = 0; i < candidateList.GetN(); i++)
	{
		if (candidateList[i]->GetObjType() == YsSceneryItem::ELEVATIONGRID)
		{
			for (int j = 0; j < candidateIntersectingFaces[i].GetN(); j++)
			{
				YsVec3 terTri[3];
				candidateList[i]->GetElevationGrid()->GetGridData()->GetTriangle(terTri, candidateIntersectingFaces[i][j].x, candidateIntersectingFaces[i][j].z, candidateIntersectingFaces[i][j].tri);
				YsMatrix4x4 terMat;
				terMat.Initialize();
				terMat.Translate(candidateList[i]->GetGlobalPosition());
				terMat.Rotate(candidateList[i]->GetGlobalAttitude());

				for (int q = 0; q < 3; q++)
				{
					//printf("Grid-relative %i:%i:%i %f %f %f\n", i,j, q, terTri[q].x(), terTri[q].y(), terTri[q].z());
					terMat.Mul(terTri[q], terTri[q], 1.0);
					//printf("world-relative %i:%i:%i %f %f %f\n", i, j, q, terTri[q].x(), terTri[q].y(), terTri[q].z());
					obj->GetMatrix().MulInverse(terTri[q], terTri[q], 1.0);
					printf("obj-relative %i:%i:%i %f %f %f\n",i, j, q, terTri[q].x(), terTri[q].y(), terTri[q].z());
				}

				YsPlane pln;
				pln.MakePlaneFromTriangle(terTri[0], terTri[1], terTri[2]);
				printf("Testpos %f %f %f | %f\n", obj->GetPosition().x(), obj->GetPosition().y(), obj->GetPosition().z(), pln.GetDistance(YsOrigin()));
				

				FsVisualSrf coll;
				YsShellPolygonHandle plHd;
				coll = obj->UntransformedCollisionShell();
				if (CheckTriangleShellIntersection(&coll, clearance, collPosLocal, terTri, plHd) == YSTRUE)
				{
					obj->GetMatrix().Mul(collPosGlobal, collPosLocal, 1.0);
					//Add collPosGlobal to list of all collPos
					printf("Ter collision global %f %f %f | obj %f %f %f\n", collPosGlobal.x(), collPosGlobal.y(), collPosGlobal.z(), collPosLocal.x(), collPosLocal.y(), collPosLocal.z());
				}
				else
				{
					candidateIntersectingFaces.Delete(i);
					candidateList.Delete(i);
					i--;
				}
			}
		}
		else if (candidateList[i]->GetObjType() == YsSceneryItem::SHELL)
		{
			FsVisualSrf objShell;
			YsShellPolygonHandle plHd1, plHd2;
			obj->SetTransformationToCollisionShell(obj->GetMatrix());
			objShell = obj->TransformedCollisionShell();

			//Terrain surf uses YsVisualSrf but object surf uses FsVisualSurf, so this mess is needed to convert
			FsVisualSrf terShell;			
			terShell.CopyFrom((YsShellExt)candidateList[i]->GetTerrainShell()->GetTransformedCollisionShell().Conv());

			if (CheckShellShellIntersection(&objShell, &terShell, clearance, collPosGlobal, plHd1, plHd2) != YSTRUE)
			{
				//Add collPosGlobal to list of all collPos
				printf("Srf collision %f %f %f\n", collPosGlobal.x(), collPosGlobal.y(), collPosGlobal.z());
			}
			else
			{
				candidateIntersectingFaces.Delete(i);
				candidateList.Delete(i);
				i--;
			}
		}
	}
	if (candidateList.GetN() > 0)
	{
		return YSTRUE;
	}
	return YSFALSE;
}

YSBOOL FsPhysics::CheckBoundingBoxTerrainIntersection(YsVec3 bbx[2], YsMatrix4x4 bbxMat, const YsElevationGrid* grid, YsMatrix4x4 terMat, double clearance, YsArray <YsElvGridFaceId> &intersectors, YsArray <YsArray<YsVec3>>& intersectingTris)
{
	//This currently only handles .ter. Eventually it needs to handle surf terrain as well
	//This doesn't yet do .ter walls correctly
	double groundElev;
	YsVec3 groundNom[8];
	YsVec3 corner[9];
	
	corner[0].Set(bbx[0].x() - clearance, bbx[0].y() - clearance, bbx[0].z() - clearance);
	corner[1].Set(bbx[1].x() + clearance, bbx[0].y() - clearance, bbx[0].z() - clearance);
	corner[2].Set(bbx[0].x() - clearance, bbx[1].y() + clearance, bbx[0].z() - clearance);
	corner[3].Set(bbx[1].x() + clearance, bbx[1].y() + clearance, bbx[0].z() - clearance);
	corner[4].Set(bbx[0].x() - clearance, bbx[0].y() - clearance, bbx[1].z() + clearance);
	corner[5].Set(bbx[1].x() + clearance, bbx[0].y() - clearance, bbx[1].z() + clearance);
	corner[6].Set(bbx[0].x() - clearance, bbx[1].y() + clearance, bbx[1].z() + clearance);
	corner[7].Set(bbx[1].x() + clearance, bbx[1].y() + clearance, bbx[1].z() + clearance);
	corner[8].Set((bbx[0].x() + bbx[1].x()) / 2, (bbx[0].y() + bbx[1].y()) / 2, (bbx[0].z() + bbx[1].z()) / 2);

	YsMatrix4x4 bbxToTer, terMatInv, terToBbx, bbxMatInv;
	terMatInv = terMat;
	terMatInv.Invert();
	bbxToTer = terMatInv * bbxMat;
	bbxMatInv = bbxMat;
	bbxMatInv.Invert();
	terToBbx = bbxMatInv * terMat;
	for (int i = 0; i < 9; i++)
	{
		bbxToTer.Mul(corner[i], corner[i], 1.0);
	}
	
	YsVec2i dim = grid->GetNumBlock();
	double elev;
	YsVec3 faceTri[3];
	int facex, facez, facedir;
	int minX=dim.x(), maxX=0, minZ=dim.y(), maxZ=0;
	YSBOOL checkSidewalls = YSFALSE;
	
	for (int i = 0; i < 9; i++)
	{
		if (grid->GetElevation(elev, facex, facez, facedir, corner[i]) == YSOK)
		{
			if (facex < minX)
			{
				minX = facex;
			}
			if (facex > maxX)
			{
				maxX = facex;
			}
			if (facez < minZ)
			{
				minZ = facez;
			}
			if (facez > maxZ)
			{
				maxZ = facez;
			}

			if (corner[i].y() < elev)
			{
				YsElvGridFaceId cross;
				cross.x = facex;
				cross.z = facez;
				cross.tri = facedir;
				intersectors.Add(cross);
				grid->GetTriangle(faceTri, facex, facez, facedir);
				YsArray <YsVec3> triArray;
				triArray.Add(faceTri[0]);
				triArray.Add(faceTri[1]);
				triArray.Add(faceTri[2]);
				intersectingTris.Add(triArray);
			}
		}
		else
		{
			//checkSidewalls = YSTRUE;
		}
	}

	int dx=0, dz=0;
	for (; minX+dx < maxX; dx++)
	{
	}
	for (; minZ + dz < maxZ; dz++)
	{
	}

	//Check face intersection for all grid faces between bbx corners
	YsVec3 mulTri[3];
	YsMatrix4x4 nulMat;
	nulMat.Initialize();
	for (int i = 0; i < dx; i++)
	{
		for (int j = 0; j < dz; j++)
		{
			//Test first face direction
			grid->GetTriangle(faceTri, minX + i, minZ + j, 0);
			for (int q = 0; q < 3; q++)
			{
				terToBbx.Mul(mulTri[q],faceTri[q],1.0);
			}

			if (CheckTriangleBoundingBoxIntersection(bbx, bbxMat,mulTri, clearance) == YSTRUE)
			{
				YsElvGridFaceId cross;
				cross.x = minX + i;
				cross.z = minZ + j;
				cross.tri = 0;
				intersectors.Add(cross);
				YsArray <YsVec3> triArray;
				triArray.Add(faceTri[0]);
				triArray.Add(faceTri[1]);
				triArray.Add(faceTri[2]);
				intersectingTris.Add(triArray);
			}

			//Test second face direction
			grid->GetTriangle(faceTri, minX + i, minZ + j, 1);
			for (int q = 0; q < 3; q++)
			{
				terToBbx.Mul(mulTri[q], faceTri[q], 1.0);
			}

			if (CheckTriangleBoundingBoxIntersection(bbx, bbxMat, mulTri, clearance) == YSTRUE)
			{
				YsElvGridFaceId cross;
				cross.x = minX + i;
				cross.z = minZ + j;
				cross.tri = 1;
				intersectors.Add(cross);
				YsArray <YsVec3> triArray;
				triArray.Add(faceTri[0]);
				triArray.Add(faceTri[1]);
				triArray.Add(faceTri[2]);
				intersectingTris.Add(triArray);
			}
		}
	}

	//Check sidewalls
	if (minX == 0 && grid->sideWall[3] == YSTRUE) //Left (-X) sidewall
	{
		printf("\nCheck minX sidewall\n============================\n");
		for (int j = 0; j < dz; j++)
		{
			//Get Z column edge nodes. Make two tris with edge nodes and footer position. Test both tris
			YsVec3 bL, bR, tL, tR, triA[3], triB[3];
			grid->GetTriangle(triA, minX, minZ + j, 0);
			grid->GetTriangle(triB, minX, minZ + j, 1);

			//Figure out which of these six are actual edge nodes
			printf("j %i: A0 %f %f %f\n", j, triA[0].x(), triA[0].y(), triA[0].z());
			printf("j %i: A1 %f %f %f\n", j, triA[1].x(), triA[1].y(), triA[1].z());
			printf("j %i: A2 %f %f %f\n", j, triA[2].x(), triA[2].y(), triA[2].z());
			printf("j %i: B0 %f %f %f\n", j, triB[0].x(), triB[0].y(), triB[0].z());
			printf("j %i: B1 %f %f %f\n", j, triB[1].x(), triB[1].y(), triB[1].z());
			printf("j %i: B2 %f %f %f\n", j, triB[2].x(), triB[2].y(), triB[2].z());

			YsVec3 verts[6],cen, leftMost, rightMost, working;
			verts[0] = triA[0];
			verts[1] = triA[1];
			verts[2] = triA[2];
			verts[3] = triB[0];
			verts[4] = triB[1];
			verts[5] = triB[2];
			cen = (verts[0] + verts[1] + verts[2] + verts[3] + verts[4] + verts[5])/6;
			for (int q = 1; q < 6; q++)
			{
				if (verts[q].x() < cen.x())
				{
					if (verts[q].z() < cen.z())
					{
						leftMost = verts[q];
					}
					else
					{
						rightMost = verts[q];
					}
				}
			}

			printf("Left %f %f %f Right %f %f %f\n", leftMost.x(), leftMost.y(), leftMost.z(), rightMost.x(), rightMost.y(), rightMost.z());


			//Test first tri

			if (CheckTriangleBoundingBoxIntersection(bbx, bbxMat, mulTri, clearance) == YSTRUE)
			{
				YsElvGridFaceId cross;
				cross.x = minX + i;
				cross.z = minZ + j;
				cross.tri = 0;
				intersectors.Add(cross);
				YsArray <YsVec3> triArray;
				triArray.Add(faceTri[0]);
				triArray.Add(faceTri[1]);
				triArray.Add(faceTri[2]);
				intersectingTris.Add(triArray);
			}

			//Test second face direction
			grid->GetTriangle(faceTri, minX + i, minZ + j, 1);
			for (int q = 0; q < 3; q++)
			{
				terToBbx.Mul(mulTri[q], faceTri[q], 1.0);
			}

			if (CheckTriangleBoundingBoxIntersection(bbx, bbxMat, mulTri, clearance) == YSTRUE)
			{
				YsElvGridFaceId cross;
				cross.x = minX + i;
				cross.z = minZ + j;
				cross.tri = 1;
				intersectors.Add(cross);
				YsArray <YsVec3> triArray;
				triArray.Add(faceTri[0]);
				triArray.Add(faceTri[1]);
				triArray.Add(faceTri[2]);
				intersectingTris.Add(triArray);
			}
		}

		//////////////////////////////////////////////////
		YsVec3 wallVert[3];
		wallVert[0].Set(grid->bbx[0].x(), grid->bbx[0].y(), grid->bbx[0].z());
		wallVert[1].Set(grid->bbx[0].x(), grid->bbx[0].y(), grid->bbx[1].z());
		wallVert[2].Set(grid->bbx[0].x(), grid->bbx[1].y(), grid->bbx[1].z());
		YsPlane wallPlane;
		terToBbx.Mul(wallVert[0], wallVert[0], 1.0);
		terToBbx.Mul(wallVert[1], wallVert[1], 1.0);
		terToBbx.Mul(wallVert[2], wallVert[2], 1.0);
		wallPlane.MakePlaneFromTriangle(wallVert[0], wallVert[1], wallVert[2]);

		YsVec3 disregard1, disregard2;
		if (CheckPlaneBoundingBoxIntersection(bbx, bbxMat, wallPlane, 0.0, disregard1, disregard2) == YSTRUE)
		{
			printf("Bbx crosses left -X wallPlane\n");
		}
	}
	if (maxX == dim.x() && grid->sideWall[1] == YSTRUE) //Right (+X) sidewall
	{
		YsVec3 wallVert[3];
		wallVert[0].Set(grid->bbx[1].x(), grid->bbx[0].y(), grid->bbx[0].z());
		wallVert[1].Set(grid->bbx[1].x(), grid->bbx[1].y(), grid->bbx[0].z());
		wallVert[2].Set(grid->bbx[1].x(), grid->bbx[1].y(), grid->bbx[1].z());
		YsPlane wallPlane;
		terToBbx.Mul(wallVert[0], wallVert[0], 1.0);
		terToBbx.Mul(wallVert[1], wallVert[1], 1.0);
		terToBbx.Mul(wallVert[2], wallVert[2], 1.0);
		wallPlane.MakePlaneFromTriangle(wallVert[0], wallVert[1], wallVert[2]);

		YsVec3 disregard1, disregard2;
		if (CheckPlaneBoundingBoxIntersection(bbx, bbxMat, wallPlane, 0.0, disregard1, disregard2) == YSTRUE)
		{
			printf("Bbx crosses right +X wallPlane\n");
		}
	}
	if (minZ == 0 && grid->sideWall[0] == YSTRUE) //Bottom (-Z) sidewall
	{
		YsVec3 wallVert[3];
		wallVert[0].Set(grid->bbx[0].x(), grid->bbx[0].y(), grid->bbx[0].z());
		wallVert[1].Set(grid->bbx[1].x(), grid->bbx[0].y(), grid->bbx[0].z());
		wallVert[2].Set(grid->bbx[1].x(), grid->bbx[1].y(), grid->bbx[0].z());
		YsPlane wallPlane;
		terToBbx.Mul(wallVert[0], wallVert[0], 1.0);
		terToBbx.Mul(wallVert[1], wallVert[1], 1.0);
		terToBbx.Mul(wallVert[2], wallVert[2], 1.0);
		wallPlane.MakePlaneFromTriangle(wallVert[0], wallVert[1], wallVert[2]);

		YsVec3 disregard1, disregard2;
		if (CheckPlaneBoundingBoxIntersection(bbx, bbxMat, wallPlane, 0.0, disregard1, disregard2) == YSTRUE)
		{
			printf("Bbx crosses bottom -Z wallPlane\n");
		}
	}
	if (maxZ == dim.y() && grid->sideWall[2] == YSTRUE) //Top (+Z) sidewall
	{
		YsVec3 wallVert[3];
		wallVert[0].Set(grid->bbx[0].x(), grid->bbx[0].y(), grid->bbx[1].z());
		wallVert[1].Set(grid->bbx[1].x(), grid->bbx[0].y(), grid->bbx[1].z());
		wallVert[2].Set(grid->bbx[1].x(), grid->bbx[1].y(), grid->bbx[1].z());
		YsPlane wallPlane;
		terToBbx.Mul(wallVert[0], wallVert[0], 1.0);
		terToBbx.Mul(wallVert[1], wallVert[1], 1.0);
		terToBbx.Mul(wallVert[2], wallVert[2], 1.0);
		wallPlane.MakePlaneFromTriangle(wallVert[0], wallVert[1], wallVert[2]);

		YsVec3 disregard1, disregard2;
		if (CheckPlaneBoundingBoxIntersection(bbx, bbxMat, wallPlane, 0.0, disregard1, disregard2) == YSTRUE)
		{
			printf("Bbx crosses top +Z wallPlane\n");
		}
	}


	if (checkSidewalls == YSTRUE)
	{
			if (grid->sideWall[0] == YSTRUE) //Bottom (-Z) sidewall
			{
				printf("Sidewall -Z exist\n");
				YsVec3 wallVert[3];
				wallVert[0].Set(grid->bbx[0].x(), grid->bbx[0].y(), grid->bbx[0].z());
				wallVert[1].Set(grid->bbx[1].x(), grid->bbx[0].y(), grid->bbx[0].z());
				wallVert[2].Set(grid->bbx[1].x(), grid->bbx[1].y(), grid->bbx[0].z());
				YsPlane wallPlane;
				terToBbx.Mul(wallVert[0], wallVert[0], 1.0);
				terToBbx.Mul(wallVert[1], wallVert[1], 1.0);
				terToBbx.Mul(wallVert[2], wallVert[2], 1.0);
				wallPlane.MakePlaneFromTriangle(wallVert[0], wallVert[1], wallVert[2]);

				YsVec3 disregard1, disregard2;
				if (CheckPlaneBoundingBoxIntersection(bbx, bbxMat, wallPlane, 0.0, disregard1, disregard2) == YSTRUE)
				{
					printf("Bbx crosses bottom -Z wallPlane\n");
				}
			}
			if (grid->sideWall[1] == YSTRUE) //Right (+X) sidewall
			{
				printf("Sidewall +X exist\n");
				YsVec3 wallVert[3];
				wallVert[0].Set(grid->bbx[1].x(), grid->bbx[0].y(), grid->bbx[0].z());
				wallVert[1].Set(grid->bbx[1].x(), grid->bbx[1].y(), grid->bbx[0].z());
				wallVert[2].Set(grid->bbx[1].x(), grid->bbx[1].y(), grid->bbx[1].z());
				YsPlane wallPlane;
				terToBbx.Mul(wallVert[0], wallVert[0], 1.0);
				terToBbx.Mul(wallVert[1], wallVert[1], 1.0);
				terToBbx.Mul(wallVert[2], wallVert[2], 1.0);
				wallPlane.MakePlaneFromTriangle(wallVert[0], wallVert[1], wallVert[2]);

				YsVec3 disregard1, disregard2;
				if (CheckPlaneBoundingBoxIntersection(bbx, bbxMat, wallPlane, 0.0, disregard1, disregard2) == YSTRUE)
				{
					printf("Bbx crosses right +X wallPlane\n");
				}
			}
			if (grid->sideWall[2] == YSTRUE) //Top (+Z) sidewall
			{
				printf("Sidewall +Z exist\n");
				YsVec3 wallVert[3];
				wallVert[0].Set(grid->bbx[0].x(), grid->bbx[0].y(), grid->bbx[1].z());
				wallVert[1].Set(grid->bbx[1].x(), grid->bbx[0].y(), grid->bbx[1].z());
				wallVert[2].Set(grid->bbx[1].x(), grid->bbx[1].y(), grid->bbx[1].z());
				YsPlane wallPlane;
				terToBbx.Mul(wallVert[0], wallVert[0], 1.0);
				terToBbx.Mul(wallVert[1], wallVert[1], 1.0);
				terToBbx.Mul(wallVert[2], wallVert[2], 1.0);
				wallPlane.MakePlaneFromTriangle(wallVert[0], wallVert[1], wallVert[2]);

				YsVec3 disregard1, disregard2;
				if (CheckPlaneBoundingBoxIntersection(bbx, bbxMat, wallPlane, 0.0, disregard1, disregard2) == YSTRUE)
				{
					printf("Bbx crosses top +Z wallPlane\n");
				}
			}
			if (grid->sideWall[3] == YSTRUE) //Left (-X) sidewall
			{
				printf("Sidewall -X exist\n");
				YsVec3 wallVert[3];
				wallVert[0].Set(grid->bbx[0].x(), grid->bbx[0].y(), grid->bbx[0].z());
				wallVert[1].Set(grid->bbx[0].x(), grid->bbx[0].y(), grid->bbx[1].z());
				wallVert[2].Set(grid->bbx[0].x(), grid->bbx[1].y(), grid->bbx[1].z());
				YsPlane wallPlane;
				terToBbx.Mul(wallVert[0], wallVert[0], 1.0);
				terToBbx.Mul(wallVert[1], wallVert[1], 1.0);
				terToBbx.Mul(wallVert[2], wallVert[2], 1.0);
				wallPlane.MakePlaneFromTriangle(wallVert[0], wallVert[1], wallVert[2]);

				YsVec3 disregard1, disregard2;
				if (CheckPlaneBoundingBoxIntersection(bbx, bbxMat, wallPlane, 0.0, disregard1, disregard2) == YSTRUE)
				{
					printf("Bbx crosses left -X wallPlane\n");
				}
			}

	}

	//if (intersectors.GetN() > 0)
	if (intersectingTris.GetN() > 0)
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
YSBOOL FsPhysics::CheckRadiusIntersection(YsVec3 pos1, double rad1, YsVec3 pos2, double rad2, double clearance)
{
	YsVec3 dif;
	dif = pos1 - pos2;
	if (dif.GetLength() < rad1 + rad2 + clearance)
	{
		return YSTRUE;
	}
	return YSFALSE;
}

YSBOOL FsPhysics::CheckPointBoundingBoxIntersection(YsVec3 bbx[2], YsMatrix4x4 mat, YsVec3 pos, double clearance)
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

YSBOOL FsPhysics::CheckLineSegmentBoundingBoxIntersection(YsVec3 bbx[2], YsMatrix4x4 mat, YsVec3 startPos, YsVec3 endPos, double clearance)
{
	if (CheckPointBoundingBoxIntersection(bbx, mat, startPos, clearance) == YSTRUE || CheckPointBoundingBoxIntersection(bbx, mat, endPos, clearance) == YSTRUE)
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

	if (CheckPointBoundingBoxIntersection(bbx, localMat, nearest, clearance) == YSTRUE)
	{
		return YSTRUE;
	}
	
	return YSFALSE;
}

YSBOOL FsPhysics::CheckInfiniteLineBoundingBoxIntersection(YsVec3 bbx[2], YsMatrix4x4 mat, YsVec3 origin, YsVec3 direction, double clearance)
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

	if (CheckPointBoundingBoxIntersection(bbx, localMat, nearest, clearance) == YSTRUE)
	{
		return YSTRUE;
	}

	return YSFALSE;
}

YSBOOL FsPhysics::CheckTriangleBoundingBoxIntersection(YsVec3 bbx[2], YsMatrix4x4 mat, YsVec3 tri[3], double clearance)
{
	//This currently expects everything in bbx reference frame
	YsVec3 lineOrig, lineDir;
	YsPlane pln;
	pln.MakePlaneFromTriangle(tri[0],tri[1],tri[2]);

	if (CheckPlaneBoundingBoxIntersection(bbx, mat, pln, clearance, lineOrig, lineDir) == YSTRUE)
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

YSBOOL FsPhysics::CheckPlaneBoundingBoxIntersection(YsVec3 bbx[2], YsMatrix4x4 mat, YsPlane pln, double clearance, YsVec3& lineOrig, YsVec3& lineDir)
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
			if (CheckInfiniteLineBoundingBoxIntersection(bbx, nulMat, lineOrig,lineDir,clearance) == YSTRUE)
			{
				return YSTRUE;
			}
		}
	}
	return YSFALSE;
}

YSBOOL FsPhysics::Check2BoundingBoxIntersection(YsVec3 bbx1[2], YsMatrix4x4 mat1, YsVec3 bbx2[2], YsMatrix4x4 mat2, double clearance)
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

YSBOOL FsPhysics::CheckLineShellIntersection(FsVisualSrf shell, YsVec3 startPos, YsVec3 endPos, double clearance, YsVec3& collPos, YsShellPolygonHandle& plHd)
{
	//This uses TransformedCollisionShell and is in global matrix
	plHd = shell.ShootRayH(collPos, startPos, endPos - startPos);

	if (plHd != NULL && YsCheckInBetween3(collPos, endPos, startPos) == YSTRUE)
	{
		return YSTRUE;
	}
	return YSFALSE;
}

YSBOOL FsPhysics::CheckTriangleShellIntersection(FsVisualSrf* shell1, double clearance, YsVec3& collPosLocal, YsVec3 tri[3], YsShellPolygonHandle& plHd)
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

YSBOOL FsPhysics::CheckShellShellIntersection(FsVisualSrf* shell1, FsVisualSrf* shell2, double clearance, YsVec3& collPos, YsShellPolygonHandle& plHd1, YsShellPolygonHandle& plHd2)
{
	//This uses TransformedCollisionShell and is in global matrix
	YsShellExt::Converter conv1, conv2;

	conv1 = shell1->Conv();
	conv2 = shell2->Conv();

	if (YsCheckShellCollisionEx(collPos, plHd1, plHd2, conv1, conv2) == YSTRUE)
	{
		return YSTRUE;
	}

	return YSFALSE;
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