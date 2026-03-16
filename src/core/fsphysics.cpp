#include "fsphysics.h"

FsPhysics::FsPhysics()
{
}

////////////////////////////////////////////
//  Object/object collisions
//Check object collision with object (obj1, obj2) [parent function that calls all the other steps]
	
YSBOOL FsPhysics::CheckObjectToObjectCollision(FsExistence* obj1, FsExistence* obj2)
{
	
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
}

// Object/terrain collisions
//Check object collision with terrain (obj, ter) [parent function]
	//Check radius groundplane overlap (pos, rad)
	//Check line groundplane overlap (pos, pos) [this is so simple as to not be necessary. If p1 or p2 is below y=0, return true]
	//Check shell overlap with terrain (obj, ter)
	//Check line overlap with terrain (ter, p1, p2)
	//Check point inside terrain (ter, p1) [this is functionally GetElevationAtPoint compared with p1]
	//Check point inside surf terrain [this one is more complicated because terrain surfs may not be solid. Can steal from IsInCloud though]


// Common utility functions
//Check radius overlap (pos1, rad1, pos2, rad2)
//Check point in bounding box (bbx, mat, pos)
//Check line in bounding box (bbx, mat, p1, p2)
//Check bounding box overlap (bbx1, mat1, bbx2, mat2)
//Check line collision with shell (srf, matrix, p1, p2)
//Check shell overlap (srf, matrix, srf, matrix)

//Convert ter to surf
//Exclude shell faces if vertex is beyond htradius
YSBOOL FsPhysics::CheckRadiusOverlap(YsVec3 pos1, double rad1, YsVec3 pos2, double rad2, double clearance)
{
	YsVec3 dif;
	dif = pos1 - pos2;
	if (dif.GetLength() < rad1 + rad2)
	{
		return YSTRUE;
	}
	return YSFALSE;
}

YSBOOL FsPhysics::CheckPointInBoundingBox(YsVec3 bbx[2], YsMatrix4x4 mat, YsVec3 pos, double clearance)
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

YSBOOL FsPhysics::CheckLineInBoundingBox(YsVec3 bbx[2], YsMatrix4x4 mat, YsVec3 startPos, YsVec3 endPos, double clearance)
{
	if (CheckPointInBoundingBox(bbx, mat, startPos, clearance) == YSTRUE || CheckPointInBoundingBox(bbx, mat, endPos, clearance) == YSTRUE)
	{
		return YSTRUE;
	}

	YSBOOL allAbove = YSTRUE, allBelow = YSTRUE, allLeft = YSTRUE, allRight = YSTRUE, allAhead = YSTRUE, allBehind = YSTRUE;
	YsMatrix4x4 localMat = mat;
	YsVec3 localStart, localEnd;
	localMat.Invert();
	localStart = localMat * startPos;
	localEnd = localMat * endPos;
	



	const double& xMin = bbx[0].x() - clearance, & xMax = bbx[1].x() + clearance;
	const double& yMin = bbx[0].y() - clearance, & yMax = bbx[1].y() + clearance;
	const double& zMin = bbx[0].z() - clearance, & zMax = bbx[1].z() + clearance;

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

YSBOOL FsPhysics::CheckBoundingBoxOverlap(YsVec3 bbx1[2], YsMatrix4x4 mat1, YsVec3 bbx2[2], YsMatrix4x4 mat2, double clearance)
{
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
	mat1.Invert();
	YsMatrix4x4 tfm = mat1 * mat2;

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

YSBOOL FsPhysics::CheckLineShellCollision(FsVisualSrf shell, YsMatrix4x4 mat, YsVec3 startPos, YsVec3 endPos, double clearance, YsVec3& collPos, YsShellPolygonHandle& plHd)
{
}

YSBOOL FsPhysics::CheckShellShellCollision(FsVisualSrf shell1, YsMatrix4x4 mat1, FsVisualSrf shell2, YsMatrix4x4 mat2, double clearance, YsVec3& collPos, YsShellPolygonHandle& plHd1, YsShellPolygonHandle& plHd2)
{
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