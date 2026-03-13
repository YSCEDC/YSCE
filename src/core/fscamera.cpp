#include <fssimplewindow.h>
#include "fscamera.h"
#include "fswindow.h"

static const double PROJ_PLANE_DIST_SCALE = 1.41421356 * 0.75;
static const double frustrumNearDist = 0.1;
static const double frustrumFarDist = 18000.0;

FsViewPort::FsViewPort()
{
	parentObject = NULL;
	lookAtObj = NULL;
	lookFromObj = NULL;
	lookAtPos = YsOrigin();
	lookFromPos = YsOrigin();
	viewMode = FSCOCKPITVIEW;
	nextViewMode = FSCOCKPITVIEW;
	viewHdg = 0.0;
	viewPch = 0.0;

	viewPoint = YsVec3::Origin();
	viewAttitude = YsZeroAtt();
	
	zoomViewMode = 1.0;
	prevZoomViewMode = 1.0;
	isViewPointInCloud = YSFALSE;
	fogVisibility = 0.0;
	centerThisCamera = YSTRUE;
	offsetRadius = 0.0;
	onboardViewId = 0;

	projection = new FsProjection;
	projection->zoomProjection = 1.0;

	prevProjection = projection;
}

void FsViewPort::SetParentObject(FsExistence* obj)
{
	parentObject = obj;
}

FsCamera::FsCamera()
{
	ghostViewSpeed = 0.0;
	timeStep = 0.0;
	viewRefPoint = YsOrigin();
	mainViewPort = new FsViewPort;
	leftViewPort = new FsViewPort;
	rightViewPort = new FsViewPort;
	activeViewPort = mainViewPort;
	zoomUser = 1.0;
	prevZoomUser = 1.0;
	sim = nullptr;
	cfg = nullptr;
	towerViewId = 0;
	towerViewPos = YsOrigin();
	ilsViewId = 0;
	ilsViewPos = YsOrigin();
	viewTargetObj = NULL;
	viewSourceObj = NULL;
	viewTargetPos = YsOrigin();
	viewSourcePos = YsOrigin();
	viewOffSetAngle = YsZeroAtt();
	viewOffsetPos = YsOrigin();
	viewOffsetRadius = 1.0;
	//mainViewPort->SetParentObject(sim->GetPlayerObject());
	//Generate list of ILS and towers here
}

FsCamera::~FsCamera()
{
	delete mainViewPort;
	delete leftViewPort;
	delete rightViewPort;
}

const char* FsCamera::ViewmodeToStr(FSVIEWMODE viewmode)
{
	switch (viewmode)
	{
	case FSCOCKPITVIEW:
		return "FSCOCKPITVIEW";
	case FSOUTSIDEPLAYERPLANE:
		return "FSOUTSIDEPLAYERPLANE";
	case FSFIXEDPOINTPLAYERPLANE:
		return "FSFIXEDPOINTPLAYERPLANE";
	case FSVARIABLEPOINTPLAYERPLANE:
		return "FSVARIABLEPOINTPLAYERPLANE";
	case FSFROMTOPOFPLAYERPLANE:
		return "FSFROMTOPOFPLAYERPLANE";
	case FSANOTHERAIRPLANE:
		return "FSANOTHERAIRPLANE";
	case FSMISSILEVIEW:
		return "FSMISSILEVIEW";
	case FSAIRTOAIRVIEW:
		return "FSAIRTOAIRVIEW";
	case FSAIRFROMAIRVIEW:
		return "FSAIRFROMAIRVIEW";
	case FSPLAYERPLANEFROMSIDE:
		return "FSPLAYERPLANEFROMSIDE";
	case FSCARRIERVIEW:
		return "FSCARRIERVIEW";
	case FSTESTVIEW1:
		return "FSTESTVIEW1";
	case FSTESTVIEW2:
		return "FSTESTVIEW2";
	case FSOUTSIDEPLAYER2:
		return "FSOUTSIDEPLAYER2";
	case FSOUTSIDEPLAYER3:
		return "FSOUTSIDEPLAYER3";
	case FSBOMBINGVIEW:
		return "FSBOMBINGVIEW";
	case FSTOWERVIEW:
		return "FSTOWERVIEW";
	case FSPLAYERTOGNDVIEW:
		return "FSPLAYERTOGNDVIEW";
	case FSGNDTOPLAYERVIEW:
		return "FSGNDTOPLAYERVIEW";
	case FSSPOTPLANEVIEW:
		return "FSSPOTPLANEVIEW";

	case FSMYWEAPONVIEW_OLD:
		return "FSMYWEAPONVIEW_OLD";
	case FSMYWEAPONVIEW_NEW:
		return "FSMYWEAPONVIEW_NEW";
	case FSBACKMIRRORVIEW:
		return "FSBACKMIRRORVIEW";
	case FS45DEGREERIGHTVIEW:
		return "FS45DEGREERIGHTVIEW";
	case FS45DEGREELEFTVIEW:
		return "FS45DEGREELEFTVIEW";
	case FS90DEGREERIGHTVIEW:
		return "FS90DEGREERIGHTVIEW";
	case FS90DEGREELEFTVIEW:
		return "FS90DEGREELEFTVIEW";
	case FSTELESCOPEVIEW:
		return "FSTELESCOPEVIEW";
	case FSLOCKEDTARGETVIEW:
		return "FSLOCKEDTARGETVIEW";
	case FSGHOSTVIEW:
		return "FSGHOSTVIEW";

	case FSAIRTOTOWERVIEW:
		return "FSAIRTOTOWERVIEW";
	case FSAIRTOTOWERVIEWSOLO:
		return "FSAIRTOTOWERVIEWSOLO";
	case FSTOWERVIEW_NOMAGNIFY:
		return "FSTOWERVIEW_NOMAGNIFY";

	case FSVERTICALORBITINGVIEW:
		return "FSVERTICALORBITINGVIEW";
	case FSHORIZONTALORBITINGVIEW:
		return "FSHORIZONTALORBITINGVIEW";
	case FSTURNVIEW:
		return "FSTURNVIEW";

	case FSADDITIONALAIRPLANEVIEW:
		return "FSADDITIONALAIRPLANEVIEW";
	case FSADDITIONALAIRPLANEVIEW_CABIN:
		return "FSADDITIONALAIRPLANEVIEW_CABIN";
	default:
		break;
	}
	return "Unknown_Viewmode";
}
/* static */ FsCamera::FSVIEWMODE FsCamera::StrToViewmode(const char* str)
{
	if (0 == YsString::STRCMP("FSCOCKPITVIEW", str))
	{
		return FSCOCKPITVIEW;
	}
	if (0 == YsString::STRCMP("FSOUTSIDEPLAYERPLANE", str))
	{
		return FSOUTSIDEPLAYERPLANE;
	}
	if (0 == YsString::STRCMP("FSFIXEDPOINTPLAYERPLANE", str))
	{
		return FSFIXEDPOINTPLAYERPLANE;
	}
	if (0 == YsString::STRCMP("FSVARIABLEPOINTPLAYERPLANE", str))
	{
		return FSVARIABLEPOINTPLAYERPLANE;
	}
	if (0 == YsString::STRCMP("FSFROMTOPOFPLAYERPLANE", str))
	{
		return FSFROMTOPOFPLAYERPLANE;
	}
	if (0 == YsString::STRCMP("FSANOTHERAIRPLANE", str))
	{
		return FSANOTHERAIRPLANE;
	}
	if (0 == YsString::STRCMP("FSMISSILEVIEW", str))
	{
		return FSMISSILEVIEW;
	}
	if (0 == YsString::STRCMP("FSAIRTOAIRVIEW", str))
	{
		return FSAIRTOAIRVIEW;
	}
	if (0 == YsString::STRCMP("FSAIRFROMAIRVIEW", str))
	{
		return FSAIRFROMAIRVIEW;
	}
	if (0 == YsString::STRCMP("FSPLAYERPLANEFROMSIDE", str))
	{
		return FSPLAYERPLANEFROMSIDE;
	}
	if (0 == YsString::STRCMP("FSCARRIERVIEW", str))
	{
		return FSCARRIERVIEW;
	}
	if (0 == YsString::STRCMP("FSTESTVIEW1", str))
	{
		return FSTESTVIEW1;
	}
	if (0 == YsString::STRCMP("FSTESTVIEW2", str))
	{
		return FSTESTVIEW2;
	}
	if (0 == YsString::STRCMP("FSOUTSIDEPLAYER2", str))
	{
		return FSOUTSIDEPLAYER2;
	}
	if (0 == YsString::STRCMP("FSOUTSIDEPLAYER3", str))
	{
		return FSOUTSIDEPLAYER3;
	}
	if (0 == YsString::STRCMP("FSBOMBINGVIEW", str))
	{
		return FSBOMBINGVIEW;
	}
	if (0 == YsString::STRCMP("FSTOWERVIEW", str))
	{
		return FSTOWERVIEW;
	}
	if (0 == YsString::STRCMP("FSPLAYERTOGNDVIEW", str))
	{
		return FSPLAYERTOGNDVIEW;
	}
	if (0 == YsString::STRCMP("FSGNDTOPLAYERVIEW", str))
	{
		return FSGNDTOPLAYERVIEW;
	}
	if (0 == YsString::STRCMP("FSSPOTPLANEVIEW", str))
	{
		return FSSPOTPLANEVIEW;
	}

	if (0 == YsString::STRCMP("FSMYWEAPONVIEW_OLD", str))
	{
		return FSMYWEAPONVIEW_OLD;
	}
	if (0 == YsString::STRCMP("FSMYWEAPONVIEW_NEW", str))
	{
		return FSMYWEAPONVIEW_NEW;
	}
	if (0 == YsString::STRCMP("FSBACKMIRRORVIEW", str))
	{
		return FSBACKMIRRORVIEW;
	}
	if (0 == YsString::STRCMP("FS45DEGREERIGHTVIEW", str))
	{
		return FS45DEGREERIGHTVIEW;
	}
	if (0 == YsString::STRCMP("FS45DEGREELEFTVIEW", str))
	{
		return FS45DEGREELEFTVIEW;
	}
	if (0 == YsString::STRCMP("FS90DEGREERIGHTVIEW", str))
	{
		return FS90DEGREERIGHTVIEW;
	}
	if (0 == YsString::STRCMP("FS90DEGREELEFTVIEW", str))
	{
		return FS90DEGREELEFTVIEW;
	}
	if (0 == YsString::STRCMP("FSTELESCOPEVIEW", str))
	{
		return FSTELESCOPEVIEW;
	}
	if (0 == YsString::STRCMP("FSLOCKEDTARGETVIEW", str))
	{
		return FSLOCKEDTARGETVIEW;
	}
	if (0 == YsString::STRCMP("FSGHOSTVIEW", str))
	{
		return FSGHOSTVIEW;
	}

	if (0 == YsString::STRCMP("FSAIRTOTOWERVIEW", str))
	{
		return FSAIRTOTOWERVIEW;
	}
	if (0 == YsString::STRCMP("FSAIRTOTOWERVIEWSOLO", str))
	{
		return FSAIRTOTOWERVIEWSOLO;
	}
	if (0 == YsString::STRCMP("FSTOWERVIEW_NOMAGNIFY", str))
	{
		return FSTOWERVIEW_NOMAGNIFY;
	}

	if (0 == YsString::STRCMP("FSVERTICALORBITINGVIEW", str))
	{
		return FSVERTICALORBITINGVIEW;
	}
	if (0 == YsString::STRCMP("FSHORIZONTALORBITINGVIEW", str))
	{
		return FSHORIZONTALORBITINGVIEW;
	}
	if (0 == YsString::STRCMP("FSTURNVIEW", str))
	{
		return FSTURNVIEW;
	}

	if (0 == YsString::STRCMP("FSADDITIONALAIRPLANEVIEW", str))
	{
		return FSADDITIONALAIRPLANEVIEW;
	}
	if (0 == YsString::STRCMP("FSADDITIONALAIRPLANEVIEW_CABIN", str))
	{
		return FSADDITIONALAIRPLANEVIEW_CABIN;
	}
	return FSCOCKPITVIEW;
}

void FsCamera::UpdateCameras(FsSimulation *currentSim)
{
	sim = currentSim;
	timeStep = sim->GetTimeStep();
	userInput = sim->GetUserInput();
	cfg = sim->GetConfig();

	for (int i = 0; i < ilsList.GetN(); i++)
	{
		ilsList[i].Update();
	}
	//ViewingControl()

}

void FsCamera::SelectViewPort(int port)
{
	switch (port)
	{
	default:
	case 0:
		activeViewPort = mainViewPort;
		break;
	case 1:
		activeViewPort = leftViewPort;
		break;
	case 2:
		activeViewPort = rightViewPort;
		break;
	}
}

void FsCamera::PrepareViewPort(int port)
{
	switch (port)
	{
	default:
	case 0:
		activeViewPort = mainViewPort;
		break;
	case 1:
		activeViewPort = leftViewPort;
		break;
	case 2:
		activeViewPort = rightViewPort;
		break;
	}
	//if (activeViewPort->parentObject == NULL)
	{
		activeViewPort->parentObject = sim->GetPlayerObject();
	}
	CalculateProjection(*activeViewPort);
	UpdateViewPort(*activeViewPort, VIEW_UNSPECIFIED);
	AutoViewChange(activeViewPort->viewMode);
}

void FsCamera::UpdateViewPort(FsViewPort &viewPort, FSVIEWMODE next)
{
	if (next != VIEW_UNSPECIFIED)
	{
		viewPort.nextViewMode = next;
	}
	FsAirplane *playerPlane = sim->GetPlayerAirplane();

	viewPort.viewMode=viewPort.nextViewMode;  // by Default
	viewPort.zoomViewMode=1.0;       // by Default
	viewPort.viewHdg=userInput.viewHdg;  // by Default
	viewPort.viewPch=userInput.viewPch;  // by Default

	if(viewPort.viewMode!=FSGHOSTVIEW)
	{
		if (playerPlane != NULL)
		{
			DecideViewMode(viewPort, playerPlane);
		}
		else if (sim->CheckNoExtAirView() != YSTRUE)
		{
			FsAirplane* air;
			air = NULL;
			while ((air = sim->FindNextAirplane(air)) != NULL)
			{
				if (air != NULL)
				{
					DecideViewMode(viewPort, air);
				}
			}
		}
		else
		{
			viewPort.viewPoint.Set(0.0, 10.0, 0.0);
			viewPort.viewAttitude.Set(0.0, 0.0, 0.0);
		}
	}

	viewPort.viewMat.Initialize();
	viewPort.viewMat.RotateXY(-viewPort.viewAttitude.b());
	viewPort.viewMat.RotateZY(-viewPort.viewAttitude.p());
	viewPort.viewMat.RotateXZ(-viewPort.viewAttitude.h());
	viewPort.viewMat.Translate(-viewPort.viewPoint);

	ApplyViewPortEnvironment(&viewPort, FsGetMainWindowDrawingAreaSize());
}

void FsCamera::ApplyViewPortEnvironment(FsViewPort* viewPort, YsVec2i drawingAreaSize)
{
	FsViewPort& vp = *viewPort;

	viewPort->isViewPointInCloud = sim->CheckIsInCloud(viewPort->viewPoint);

	if (viewPort->isViewPointInCloud != YSTRUE)
	{
		viewPort->fogVisibility = sim->GetFogVis();
	}
	else
	{
		viewPort->fogVisibility = 100.0;
	}

	//Temporary call back to original function to draw texture shadowmap
	sim->SimCalculateShadowMap(vp, drawingAreaSize);
}

void FsCamera::DecideViewMode(FsViewPort &viewPort, FsAirplane *playerPlane)
{
	viewPort.lookAtObj = viewTargetObj;
	viewPort.lookFromObj = viewSourceObj;
	viewPort.lookFromPos = viewSourcePos;
	viewPort.lookAtPos = viewTargetPos;
	double relDist;
	YsAtt3 relAtt;
	sim->GetRelView(relDist, relAtt);

	switch(viewPort.viewMode)
	{
	///////////////////////////////// Onboard
	case FSCOCKPITVIEW:
	case FSADDITIONALAIRPLANEVIEW:
	case FSADDITIONALAIRPLANEVIEW_CABIN:
	case FSBACKMIRRORVIEW:
	case FS45DEGREERIGHTVIEW:
	case FS45DEGREELEFTVIEW:
	case FS90DEGREERIGHTVIEW:
	case FS90DEGREELEFTVIEW:
	case FSVIEWUP:
	case FSVIEWDOWN:
	case FSBOMBINGVIEW:
		if (viewPort.parentObject != nullptr && (viewPort.parentObject->CommonProp().IsActive() == YSTRUE))
		{
			YsBool3 lock;
			lock.Set(YSFALSE, YSFALSE, YSFALSE);
			YsAtt3 dir = YsZeroAtt();
			if (viewPort.onboardViewId == 0)
			{
				dir = ((FsAirplane*)viewPort.parentObject)->Prop().GetNeutralHeadDirection();
			}

			switch (viewPort.viewMode)
			{
			case FSBACKMIRRORVIEW:
				dir.Set(YsPi, 0.0, 0.0);
				lock.Set(YSTRUE, YSTRUE, YSTRUE);
				break;
			case FS45DEGREERIGHTVIEW:
				dir.Set(-YsPi / 4.0, 0.0, 0.0);
				lock.Set(YSTRUE, YSTRUE, YSTRUE);
				break;
			case FS45DEGREELEFTVIEW:
				dir.Set(YsPi / 4.0, 0.0, 0.0);
				lock.Set(YSTRUE, YSTRUE, YSTRUE);
				break;
			case FS90DEGREERIGHTVIEW:
				dir.Set(-YsPi / 2.0, 0.0, 0.0);
				lock.Set(YSTRUE, YSTRUE, YSTRUE);
				break;
			case FS90DEGREELEFTVIEW:
				dir.Set(YsPi / 2.0, 0.0, 0.0);
				lock.Set(YSTRUE, YSTRUE, YSTRUE);
				break;
			case FSVIEWUP:
				dir.Set(0.0, YsPi / 2.0, 0.0);
				lock.Set(YSTRUE, YSTRUE, YSTRUE);
				break;
			case FSVIEWDOWN:
				dir.Set(0.0, -YsPi / 2.0, 0.0);
				lock.Set(YSTRUE, YSTRUE, YSTRUE);
				break;
			}
			
			CalculateOnboardView(viewPort, viewPort.parentObject, viewPort.onboardViewId, YsOrigin(), dir, lock);
		}
		break;
	///////////////////////////////// Flyby
	case FSOUTSIDEPLAYERPLANE:
		{
			viewPort.viewPoint=viewPort.parentObject->GetPosition();

			auto dist= viewPort.parentObject->GetApproximatedCollideRadius()*2.0;

			YsVec3 tmp;
			tmp=viewRefPoint-viewPort.viewPoint;
			tmp.Normalize();
			tmp*=dist;
			viewPort.viewPoint+=tmp;
			viewPort.viewAttitude.SetForwardVector(-tmp);
			viewPort.offsetRadius =dist;
		}
		break;
	case FSFIXEDPOINTPLAYERPLANE:
		{
			YsVec3 tmp;
			tmp=(viewPort.parentObject->GetPosition())-viewRefPoint;

			viewPort.viewPoint=viewRefPoint+tmp*5.0/6.0;
			viewPort.viewAttitude.SetForwardVector(tmp);
		}
		break;
	case FSVARIABLEPOINTPLAYERPLANE:
		{
			YsVec3 tmp;
			tmp= viewPort.parentObject->CommonProp().GetPosition();
			viewPort.viewPoint=viewRefPoint;
			tmp=tmp-viewPort.viewPoint;
			viewPort.viewAttitude.SetForwardVector(tmp);
		}
		break;
	///////////////////////////////// External
	case FSFROMTOPOFPLAYERPLANE:
		if (viewPort.lookAtObj->IsActive() == YSTRUE)
		{
			YsBool3 local, lock;
			local.Set(YSTRUE, YSFALSE, YSFALSE);
			lock.Set(YSTRUE, YSTRUE, YSTRUE);
			YsAtt3 dir;
			dir.Set(YsPi/2, YsDegToRad(-72), 0.0);
			CalculateExternalView(viewPort, viewPort.parentObject->GetApproximatedCollideRadius() * 2.0 * viewOffsetRadius, dir, viewOffSetAngle, local, lock, YSFALSE);
		}
		break;
	case FSPLAYERPLANEFROMSIDE:
		if (viewPort.lookAtObj->IsActive() == YSTRUE)
		{
			YsBool3 local, lock;
			local.Set(YSTRUE, YSFALSE, YSFALSE);
			lock.Set(YSTRUE, YSTRUE, YSTRUE);
			YsAtt3 dir;
			dir.Set(YsPi / 2.0, 0.0, 0.0);
			CalculateExternalView(viewPort, viewPort.parentObject->GetApproximatedCollideRadius() * 2.0 * viewOffsetRadius, dir, viewOffSetAngle, local, lock, YSFALSE);
		}
		break;
	case FSOUTSIDEPLAYER2:
		if (viewPort.lookAtObj->IsActive() == YSTRUE)
		{
			YsBool3 local, lock;
			local.Set(YSFALSE, YSFALSE, YSFALSE);
			lock.Set(YSFALSE, YSFALSE, YSFALSE);
			YsAtt3 dir;
			dir.Set(0.0, YsDegToRad(-15), 0.0);
			CalculateExternalView(viewPort, relDist * viewPort.parentObject->GetApproximatedCollideRadius()*viewOffsetRadius, dir, viewOffSetAngle, local, lock, YSFALSE);
		}
		break;
	case FSOUTSIDEPLAYER3:
		if (viewPort.lookAtObj->IsActive() == YSTRUE)
		{
			YsBool3 local, lock;
			local.Set(YSTRUE, YSTRUE, YSTRUE);
			lock.Set(YSFALSE, YSFALSE, YSFALSE);
			YsAtt3 dir;
			dir.Set(0.0, YsDegToRad(-15), 0.0);

			CalculateExternalView(viewPort, relDist * viewPort.parentObject->GetApproximatedCollideRadius()* viewOffsetRadius, dir, viewOffSetAngle, local, lock, cfg->externalCameraDelay);
		}
		break;
	/////////////////////////////////// Weapon
	case FSMISSILEVIEW:
		{
			YsVec3 &pos=viewPort.viewPoint;
			YsAtt3 &att=viewPort.viewAttitude;
			if(sim->FindFirstGuidedMissile(pos,att)==YSOK && sim->CheckNoExtAirView() != YSTRUE)
			{
				YsVec3 off;
				att.SetB(0.0);
				off.Set(0.0,2.0,-10.0);
				att.Mul(off,off);
				pos=pos+off;
			}
			else
			{
				UpdateViewPort(viewPort,FSCOCKPITVIEW);
			}
		}
		break;
	case FSMYWEAPONVIEW_OLD:
	case FSMYWEAPONVIEW_NEW:
		{
			YSRESULT res;
			res=YSERR;
			if(viewPort.viewMode ==FSMYWEAPONVIEW_OLD)
			{
				res=sim->FindOldestMissileOfOwner(viewPort.viewPoint,viewPort.viewAttitude,playerPlane);
			}
			else if(viewPort.viewMode ==FSMYWEAPONVIEW_NEW)
			{
				res= sim->FindNewestMissileOfOwner(viewPort.viewPoint,viewPort.viewAttitude,playerPlane);
			}

			if(res==YSOK)
			{
				YsVec3 off;
				viewPort.viewAttitude.SetB(0.0);
				off.Set(0.0,2.0,-10.0);
				viewPort.viewAttitude.Mul(off,off);
				viewPort.viewPoint+=off;
			}
			else if(sim->CheckNoExtAirView()==YSTRUE)
			{
				UpdateViewPort(viewPort,FSCOCKPITVIEW);
			}
			else
			{
				UpdateViewPort(viewPort,FSLOCKEDTARGETVIEW);
			}
		}
		break;
	case FSLOCKEDTARGETVIEW:
		if(viewPort.parentObject !=NULL)
		{
			const FsExistence *trg;
			YsVec3 ccip;
			viewPort.zoomViewMode = 40.0;

			if((((FsAirplane*)viewPort.parentObject)->Prop().GetSelectedWeaponPerformance().targetAir == YSTRUE &&
				sim->FindAirplane(((FsAirplane*)viewPort.parentObject)->Prop().GetAirTargetKey()) != NULL) ||
				(((FsAirplane*)viewPort.parentObject)->Prop().GetSelectedWeaponPerformance().targetGnd == YSTRUE &&
					sim->FindGround(((FsAirplane*)viewPort.parentObject)->Prop().GetGroundTargetKey()) != NULL))
			{
				if(((FsAirplane*)viewPort.parentObject)->Prop().GetSelectedWeaponPerformance().targetAir != YSTRUE)
				{
					trg=sim->FindGround(((FsAirplane*)viewPort.parentObject)->Prop().GetGroundTargetKey());
				}
				else //Prioritize air targets if weapon can target both
				{
					trg=sim->FindAirplane(((FsAirplane*)viewPort.parentObject)->Prop().GetAirTargetKey());
				}

				if(trg!=NULL)
				{
					double offsetRadius =trg->GetApproximatedCollideRadius()*1.5;

					YsVec3 ev,uv;
					ev=trg->GetPosition()-viewPort.parentObject->GetPosition();
					ev.Normalize();
					uv=viewPort.parentObject->GetAttitude().GetUpVector();
					viewPort.viewPoint=trg->GetPosition()-ev* offsetRadius;
					viewPort.viewAttitude.SetTwoVector(ev,uv);
					viewPort.offsetRadius = offsetRadius;
					return;
				}
			}
			else if (((FsAirplane*)viewPort.parentObject)->Prop().GetSelectedWeaponPerformance().category == FSWEAPONCAT_FREEFALL &&
				((FsAirplane*)viewPort.parentObject)->Prop().ComputeEstimatedBombLandingPosition(ccip, sim->GetWeather()) == YSOK)
			{
				double offsetRadius = viewPort.parentObject->GetApproximatedCollideRadius();
				YsVec3 viewPos, viewDirec, cameraOffset, bombPos;
				YsAtt3 viewAtt;

				((FsAirplane*)viewPort.parentObject)->Prop().FindNextWeaponSlot(((FsAirplane*)viewPort.parentObject)->Prop().GetSelectedWeaponType(),bombPos);
				bombPos = viewPort.parentObject->GetMatrix() * bombPos;
				
				viewDirec = ccip - bombPos;
				viewAtt.SetForwardVector(viewDirec);
				viewAtt.SetUpVector(viewPort.parentObject->GetAttitude().GetUpVector());

				cameraOffset.Set(0.0, 0.0, offsetRadius);
				viewAtt.Mul(cameraOffset, cameraOffset);
				
				viewPort.viewPoint = bombPos + cameraOffset;
				viewPort.viewAttitude = viewAtt;
				viewPort.offsetRadius = offsetRadius;
				return;
			}
			else if(((FsAirplane*)viewPort.parentObject)->Prop().GetSelectedWeaponType()==FSWEAPON_GUN)
			{
				if(((FsAirplane*)viewPort.parentObject)->Prop().GetHasPilotControlledTurret()==YSTRUE)
				{
					YsVec3 dir;
					((FsAirplane*)viewPort.parentObject)->Prop().GetFirstPilotControlledTurretPosition(viewPort.viewPoint);
					((FsAirplane*)viewPort.parentObject)->Prop().GetFirstPilotControlledTurretDirection(dir);

					viewPort.viewAttitude.SetTwoVector(dir, viewPort.parentObject->GetAttitude().GetUpVector());
					return;
				}
				else
				{
					const FsAirplane *target;
					YsVec3 aim;
					if(sim->PassGunAim(target,aim)==YSOK)
					{
						double offsetRadius = viewPort.parentObject->GetApproximatedCollideRadius();

						YsVec3 ev;
						ev=aim- viewPort.parentObject->GetPosition();
						ev.Normalize();
						viewPort.viewPoint= viewPort.parentObject->GetPosition()+ev* offsetRadius;
						viewPort.viewAttitude.SetTwoVector(ev, viewPort.parentObject->GetAttitude().GetUpVector());
						viewPort.offsetRadius = offsetRadius;
						return;
					}
					else
					{
						YsVec3 gunPos, gunAim, cameraOffset;
						YsAtt3 gunDir;
						double offsetRadius = viewPort.parentObject->GetApproximatedCollideRadius();
						cameraOffset.Set(0.0, 0.0, offsetRadius);

						((FsAirplane*)viewPort.parentObject)->Prop().GetGunPosition(gunPos, gunAim);
						gunPos = viewPort.parentObject->GetMatrix() * gunPos;
						gunAim.Normalize();
						viewPort.parentObject->GetAttitude().Mul(gunAim,gunAim);
						gunDir.SetTwoVector(gunAim, viewPort.parentObject->GetAttitude().GetUpVector());

						viewPort.viewAttitude = gunDir;
						viewPort.viewPoint = gunPos + gunAim* offsetRadius;
						viewPort.offsetRadius = offsetRadius;
						return;
					}
				}
			}

			double offsetRadius = viewPort.parentObject->GetApproximatedCollideRadius();

			YsVec3 ev, vVec;
			YsAtt3 vAtt;
			viewPort.parentObject->CommonProp().GetVelocity(vVec);
			vAtt.SetForwardVector(vVec);
			viewPort.viewPoint= viewPort.parentObject->GetPosition();
			viewPort.viewAttitude=vAtt;
			ev=viewPort.viewAttitude.GetForwardVector();
			viewPort.viewPoint+=ev* offsetRadius;
			viewPort.offsetRadius = offsetRadius;
		}
		break;
	/////////////////// Object to object
	case FSANOTHERAIRPLANE:
	case FSAIRTOAIRVIEW:
	case FSAIRFROMAIRVIEW:
	case FSPLAYERTOGNDVIEW:
	case FSGNDTOPLAYERVIEW:
		if (sim->CheckNoExtAirView() != YSTRUE && viewPort.lookAtObj != NULL && viewPort.lookFromObj != NULL)
		{
			CalculateObjectToObjectView(viewPort);
		}
		break;
	///////////////////////// Point to object
	case FSCARRIERVIEW:
		if (viewPort.lookFromObj != NULL && viewPort.lookAtObj != NULL)
		{
			CalculateCameraToObjectView(viewPort);
		}
		break;
	case FSTOWERVIEW:
	case FSTOWERVIEW_NOMAGNIFY:
		if (viewPort.lookAtObj != NULL)
		{
			CalculatePointToObjectView(viewPort);
		}
		break;
	case FSAIRTOTOWERVIEW:
	case FSAIRTOTOWERVIEWSOLO:
		if (viewPort.lookFromObj != NULL)
		{
			CalculateObjectToPointView(viewPort);
		}
		break;
	////////////////////////////////// Other
	case FSSPOTPLANEVIEW:
		if(viewPort.lookAtObj !=NULL)
		{
			YsVec3 dir,fomCen;
			double dist= relDist* viewPort.lookAtObj->GetApproximatedCollideRadius();
			dir.Set(0,0,dist);

			relAtt.Mul(dir,dir);

			viewPort.viewPoint= viewPort.lookAtObj->GetPosition()-dir;
			viewPort.viewAttitude=relAtt;
			viewPort.offsetRadius =dist;
		}
		else
		{
			UpdateViewPort(viewPort,FSCOCKPITVIEW);
		}
		break;
	case FSVERTICALORBITINGVIEW:
		UpdateViewPort(viewPort,FSOUTSIDEPLAYER2);
		viewPort.viewAttitude.SetB(0.0);
		break;
	case FSHORIZONTALORBITINGVIEW:
		UpdateViewPort(viewPort,FSOUTSIDEPLAYER2);
		break;
	case FSTURNVIEW:
		UpdateViewPort(viewPort,FSOUTSIDEPLAYER2);
		break;
	}
}

void FsCamera::AutoViewChange(FSVIEWMODE viewMode)
{
	double relDist;
	YsAtt3 relAtt;
	sim->GetRelView(relDist, relAtt);
	switch(viewMode)
	{
	default:
		break;
	case FSFIXEDPOINTPLAYERPLANE:
		UpdateViewpointAccordingToPlayerAirplane(2000.0,YSFALSE);
		break;
	case FSOUTSIDEPLAYERPLANE:
		UpdateViewpointAccordingToPlayerAirplane(2000.0,YSFALSE);
		break;
	case FSVARIABLEPOINTPLAYERPLANE:
		{
			if(nullptr!=activeViewPort->lookAtObj)
			{
				YsVec3 tmp= activeViewPort->lookAtObj->CommonProp().GetPosition();

				if((tmp-viewRefPoint).GetSquareLength()>=300.0*300.0)
				{
					YsVec3 mov;
					mov=viewRefPoint-tmp;
					mov.Normalize();
					mov=mov*300.0;
					viewRefPoint=tmp+mov;
				}
			}
		}
		break;
	case FSANOTHERAIRPLANE:
		if(sim->CheckNoExtAirView()!=YSTRUE)  // 2006/06/11
		{
			for(int i=0; i< sim->GetNumAirplane(); i++)
			{
				if(viewTargetObj==NULL || viewTargetObj ==sim->GetPlayerAirplane() || viewTargetObj->IsAlive()!=YSTRUE)
				{
					viewTargetObj =sim->FindNextAirplane((const FsAirplane*)viewTargetObj);
					if(viewTargetObj ==NULL)
					{
						viewTargetObj =sim->FindNextAirplane((const FsAirplane*)viewTargetObj);
					}
				}
				else
				{
					break;
				}
			}
		}
		break;
	case FSCARRIERVIEW:
	case FSTOWERVIEW:
	case FSTOWERVIEW_NOMAGNIFY:
	case FSAIRTOTOWERVIEW:
	case FSAIRTOTOWERVIEWSOLO:
		if(viewTargetObj ==NULL)
		{
			viewTargetObj =sim->FindNextAirplane(NULL);
		}
		break;
	case FSSPOTPLANEVIEW:
		if(viewTargetObj !=NULL)
		{
			YsVec3 dir;
			double dist;
			dist= relDist * viewTargetObj->GetApproximatedCollideRadius();
			dir.Set(0,0,dist);

			YsVec3 ev1,ev2;
			ev1= viewTargetObj->GetAttitude().GetForwardVector();
			ev2=relAtt.GetForwardVector();

			relAtt.Mul(dir,dir); // dir=relViewAtt.GetMatrix()*dir;
			relAtt.SetB((ev1*ev2)* viewTargetObj->GetAttitude().b()/2.0);
		}
		break;
	case FSVERTICALORBITINGVIEW:
		relAtt.NoseUp(-timeStep*YsPi/12.0);
		break;
	case FSHORIZONTALORBITINGVIEW:
		relAtt.YawLeft(timeStep*YsPi/12.0);
		break;
	case FSTURNVIEW:
		relAtt.SetH(relAtt.h()+timeStep*YsPi/12.0);
		break;
	}
}

void FsCamera::CalculateOnboardView(FsViewPort& viewPort, FsExistence* focus, int viewIdx, YsVec3 posOffset, YsAtt3 attOffset, YsBool3 locked)
{
	if (viewPort.parentObject != NULL)
	{
		if (viewIdx < viewPort.parentObject->CommonProp().GetNumOnboardViewpoint())
		{
			YsVec3 pos;
			YsAtt3 att;

			const FsOnboardViewpoint* onboard;
			onboard = viewPort.parentObject->CommonProp().GetOnboardViewpoint(viewIdx);

			pos = onboard->pos + posOffset;
			att = onboard->att;
			att.AddH(attOffset.h());
			att.AddP(attOffset.p());
			att.AddB(attOffset.b());
			viewPort.viewMode = FSCOCKPITVIEW;

			YsMatrix4x4 mat = viewPort.parentObject->GetMatrix();

			viewPort.viewPoint = mat * pos;

			viewPort.viewAttitude = viewPort.parentObject->GetAttitude();
			viewPort.viewAttitude.YawLeft(att.h());
			viewPort.viewAttitude.NoseUp(att.p());
			viewPort.viewAttitude.SetB(viewPort.viewAttitude.b() + att.b());

			if (locked.x != YSTRUE)
			{
				viewPort.viewAttitude.YawLeft(userInput.viewHdg);
			}
			if (locked.y != YSTRUE)
			{
				viewPort.viewAttitude.NoseUp(userInput.viewPch);
			}
		}
		else if (viewIdx == 99) //Temporary hardcode for bombing view. Make better weapon seeker view handling later
		{
			viewPort.viewMode = FSBOMBINGVIEW;
			YsVec3 cock;
			YsMatrix4x4 mat;
			cock = viewPort.parentObject->GetCockpitPosition();

			mat.Translate(viewPort.parentObject->GetPosition());
			mat.Rotate(viewPort.parentObject->GetAttitude());

			viewPort.viewPoint = mat * cock;

			viewPort.viewAttitude = viewPort.parentObject->GetAttitude();
			viewPort.viewAttitude.NoseUp(-YsDegToRad(60.0));
		}
		else
		{
			viewPort.onboardViewId = 0;
		}

	}
}

void FsCamera::CalculateFlybyView(FsViewPort& viewPort, double range, double resetRange)
{
}

void FsCamera::CalculateExternalView(FsViewPort& viewPort, double radius, YsAtt3 neutral, YsAtt3 offset, YsBool3 local, YsBool3 locked, YSBOOL delay)
{
	YsAtt3 airAtt, pointing;
	YsVec3 pos;

	if (delay != YSTRUE || viewPort.parentObject->GetAttitudeFromRecord(airAtt, sim->CurrentTime() - 0.1) != YSOK)
	{
		airAtt = viewPort.parentObject->GetAttitude();
	}
	
	pointing.Set(airAtt.h() * local.x, airAtt.p() * local.y, airAtt.b() * local.z);
	pointing.YawLeft(neutral.h() + offset.h() * locked.InvX());
	pointing.NoseUp(neutral.p() + offset.p() * locked.InvY());
	pointing.AddB(neutral.b() + offset.b() * locked.InvZ());

	pos = pointing.GetForwardVector() * radius;
	

	viewPort.viewPoint = viewPort.parentObject->CommonProp().GetPosition() - pos;
	viewPort.viewAttitude = pointing;
}

void FsCamera::CalculateWeaponView(FsViewPort& viewPort, FsWeapon* wep, YSBOOL direction)
{
}

void FsCamera::CalculateChaseView(FsViewPort& viewPort, FsExistence* lookAt)
{
}

void FsCamera::CalculateObjectToObjectView(FsViewPort& viewPort)
{
	//if (sim->CheckNoExtAirView() != YSTRUE)  // 2006/06/11
		//{
		//	if(viewSourceAir !=NULL && viewSourceAir !=parent && viewSourceAir->IsAlive()==YSTRUE)
		//	{
		//		const YsVec3 &p1= viewSourceAir->GetPosition();
		//		const YsAtt3 &a1= viewSourceAir->GetAttitude();

		//		const YsVec3 &p2= viewPort.lookAtObj->GetPosition();
		//		// const YsAtt3 &a2=playerPlane->GetAttitude();

		//		const double radius= viewSourceAir->GetApproximatedCollideRadius();

		//		const YsVec3 viewDir=p2-p1;

		//		FsAirplane* source = (FsAirplane*)viewSourceAir;

		//		viewPort.viewAttitude.SetForwardVector(viewDir);
		//		viewPort.viewAttitude.SetB(a1.b());
		//		
		//		if(YSTRUE== source->Prop().IsOnGround() && radius*sin(viewPort.viewAttitude.p())> source->Prop().GetGroundStandingHeight()/2.0)
		//		{
		//			const double p=asin((source->Prop().GetGroundStandingHeight()/2.0)/radius);
		//			viewPort.viewAttitude.SetP(p);
		//		}

		//		YsVec3 off(1.0,0.6,-3.0);
		//		off*=radius;
		//		viewPort.viewAttitude.Mul(off,off); // off=att.GetMatrix()*off;

		//		viewPort.offsetRadius =off.GetLength();

		//		viewPort.viewPoint=p1+off;

		//		if(viewPort.viewPoint.y()< source->Prop().GetGroundElevation()+0.5)
		//		{
		//			viewPort.viewPoint.SetY(source->Prop().GetGroundElevation()+0.5);
		//		}

		//		return;
		//	}
		//}
		//// If no other airplane is found,
		//UpdateViewPort(viewPort,FSCOCKPITVIEW);  // Actual viewmode will be automatically set
		

	//if (sim->CheckNoExtAirView() != YSTRUE)  // 2006/06/11
		//{
		//	//const FsAirplane *from=sim->GetFocusAir();
		//	//const FsAirplane *to=sim->GetFocusAir2();

		//	if(viewPort.lookAtObj !=NULL && viewSourceAir !=NULL)
		//	{
		//		YsVec3 off;
		//		const YsVec3 *p1,*p2;
		//		const YsAtt3 *a1,*a2;
		//		YsVec3 upv;

		//		p1=&viewPort.lookAtObj->GetPosition();
		//		a1=&viewPort.lookAtObj->GetAttitude();

		//		p2=&viewSourceAir->GetPosition();
		//		a2=&viewSourceAir->GetAttitude();

		//		if(viewPort.lookAtObj->CommonProp().IsActive()==YSTRUE)
		//		{
		//			upv=a1->GetUpVector();
		//		}
		//		else
		//		{
		//			upv=a2->GetUpVector();
		//		}

		//		if(viewPort.viewMode ==FSAIRTOAIRVIEW)
		//		{
		//			viewPort.viewAttitude.SetTwoVector(*p2-*p1,upv);
		//		}
		//		else if(viewPort.viewMode ==FSAIRFROMAIRVIEW)
		//		{
		//			viewPort.viewAttitude.SetTwoVector(*p1-*p2,upv);
		//		}

		//		off.Set(1.0,0.6,-3.0);
		//		off*= viewPort.lookAtObj->GetApproximatedCollideRadius();
		//		viewPort.viewAttitude.Mul(off,off);  // off=att.GetMatrix()*off;
		//		viewPort.offsetRadius =off.GetLength();

		//		viewPort.viewPoint=*p1+off;
		//		if(viewPort.viewPoint.y()<1.0)
		//		{
		//			viewPort.viewPoint.SetY(1.0);
		//		}
		//	}
		//}
		//else
		//{
		//	UpdateViewPort(viewPort,FSCOCKPITVIEW);
		//}
	/*if(viewPort.lookAtObj !=NULL)
		{
			YsVec3 gnd= viewPort.lookAtObj->GetCollisionShellCenter();

			viewPort.lookAtObj->GetAttitude().Mul(gnd,gnd);
			gnd+= viewPort.lookAtObj->GetPosition();

			YsVec3 vec=gnd-viewSourceAir->GetPosition();
			vec.Normalize();

			const double rad= viewSourceAir->CommonProp().GetOutsideRadius()*2.0;
			vec*=rad;
			vec.SubY(rad*0.5);
			if(vec.y()>=0.0)
			{
				vec.SetY(0.0);
			}

			viewPort.viewAttitude.SetForwardVector(vec);
			viewPort.viewAttitude.SetB(0.0);
			viewPort.offsetRadius =rad;

			viewPort.viewPoint= viewSourceAir->GetPosition()-vec;
		}
		else
		{
			UpdateViewPort(viewPort,FSCOCKPITVIEW);
		}*/

	//if (viewSourceGnd != NULL)
	//{
	//	YsVec3 gnd = viewSourceGnd->GetCollisionShellCenter();

	//	viewSourceGnd->GetAttitude().Mul(gnd, gnd);
	//	gnd += viewSourceGnd->GetPosition();

	//	YsVec3 vec = viewPort.lookAtObj->GetPosition() - gnd;
	//	vec.Normalize();

	//	const double rad = viewPort.lookAtObj->CommonProp().GetOutsideRadius() * 2.0;
	//	vec *= rad;
	//	vec.SubY(rad * 0.5);
	//	if (vec.y() >= 0.0)
	//	{
	//		vec.SetY(0.0);
	//	}

	//	viewPort.viewAttitude.SetForwardVector(vec);
	//	viewPort.viewAttitude.SetB(0.0);
	//	viewPort.offsetRadius = rad;

	//	viewPort.viewPoint = viewPort.lookAtObj->GetPosition() - vec;
	//}
	//else
	//{
	//	UpdateViewPort(viewPort, FSCOCKPITVIEW);
	//}
}

void FsCamera::CalculateCameraToObjectView(FsViewPort& viewPort)
{
	/*if(NULL!=viewSourceGnd && NULL != viewPort.lookAtObj)
		{
			if(viewSourceGnd !=NULL && viewSourceGnd->IsAlive()==YSTRUE)
			{
				YsVec3 off;
				YsMatrix4x4 mat;
				mat.Initialize();
				mat.Translate(viewSourceGnd->GetPosition());
				mat.Rotate(viewSourceGnd->GetAttitude());
				FsGround* source = (FsGround*)viewSourceGnd;
				if(source->Prop().GetAircraftCarrierProperty()!=NULL)
				{
					off= source->Prop().GetAircraftCarrierProperty()->GetBridgePos();
				}
				else
				{
					off.Set(25.0,52.0,-28.0);
				}

				YsVec3 ofstAir= viewPort.lookAtObj->GetLookAtOffset();
				viewPort.lookAtObj->GetAttitude().Mul(ofstAir,ofstAir);

				viewSourcePos = mat * off;
				viewTargetPos = viewPort.lookAtObj->GetPosition() + ofstAir;



				viewPort.viewAttitude.SetForwardVector(viewTargetPos-viewSourcePos);
			}
			else
			{
				UpdateViewPort(viewPort,FSCOCKPITVIEW);
			}
		}*/
}

void FsCamera::CalculatePointToObjectView(FsViewPort& viewPort)
{
	/*{
			if(viewPort.lookAtObj !=NULL)
			{
				YsVec3 dir;
				dir= viewPort.lookAtObj->GetPosition()-viewSourcePos;
				dir.Normalize();
				viewPort.viewAttitude.SetForwardVector(dir);
				viewPort.viewPoint=viewSourcePos;

				if(viewPort.viewMode ==FSTOWERVIEW)
				{
					viewPort.zoomViewMode =8.0;
				}
			}
			else
			{
				UpdateViewPort(viewPort,FSCOCKPITVIEW);
			}
		}*/
}

void FsCamera::CalculateObjectToPointView(FsViewPort& viewPort)
{
	/*if(viewSourceAir!=NULL)
		{
			double r;
			YsVec3 dir;
			dir= viewTargetPos- viewSourceAir->GetPosition();
			dir.Normalize();

			r= viewSourceAir->GetRadiusFromCollision();

			if(viewPort.viewMode ==FSAIRTOTOWERVIEW)
			{
				r*=3.0;
			}
			else
			{
				r*=1.5;
			}

			YsVec3 ofst= viewSourceAir->GetLookAtOffset();
			viewSourceAir->GetAttitude().Mul(ofst,ofst);

			viewPort.viewPoint= viewSourceAir->GetPosition()+ofst-dir*r;
			if(viewPort.viewPoint.y()< towerViewPos.y())
			{
				viewPort.viewPoint.SetY(towerViewPos.y());
			}
			viewPort.offsetRadius =(ofst-dir*r).GetLength();

			dir= viewSourceAir->GetPosition()-viewPort.viewPoint;

			viewPort.viewAttitude.SetForwardVector(dir);
			viewPort.viewAttitude.SetB(0.0);
		}
		else
		{
			UpdateViewPort(viewPort,FSCOCKPITVIEW);
		}*/
}

void FsCamera::CalculatePointToPointView(FsViewPort& viewPort, YsVec3 from, YsVec3 to)
{
}

void FsCamera::CalculateConstantRotationView(FsViewPort& viewPort, YsAtt3 rot)
{

}

void FsCamera::CalculateGhostView(FsSimulation* sim, const double dt)
{
	FsFlightControl *userInput = &sim->GetUserInput();
	auto &viewPoint= mainViewPort->viewPoint;
	auto &viewAttitude= mainViewPort->viewAttitude;

	const double accel=120.0;
	double vp,vh,vb,vy,desigSpd;
	vp=userInput->ctlElevator*(YsPi/2.0)*dt;
	vb=userInput->ctlAileron*(YsPi)*dt;
	vh=(sin(viewAttitude.b())/5.0*dt)*YsAbs(cos(viewAttitude.p()));
	//vy=userInput.ctlRudder*dt;
	double speedMult = userInput->ctlThrottle;

	viewAttitude.SetP(YsBound(viewAttitude.p()+vp,-YsPi/2,YsPi/2));
	viewAttitude.SetH(viewAttitude.h()+vb);  // vb~=Aileron.  Let aileron control heading.
	if(viewAttitude.p()<-YsPi/2)
	{
		viewAttitude.SetP(-YsPi/2);
	}
	else if(viewAttitude.p()>YsPi/2)
	{
		viewAttitude.SetP(YsPi/2);
	}
	viewAttitude.SetB(viewAttitude.b()/2.0);

	if(userInput->ctlFireWeaponButton==YSTRUE)
	{
		desigSpd=350.0*speedMult+5;
	}
	else if(userInput->ctlCycleWeaponButton==YSTRUE)
	{
		desigSpd=-350.0*speedMult+5;
	}
	else
	{
		desigSpd=0.0;
	}

	if(ghostViewSpeed<desigSpd)
	{
		ghostViewSpeed+=accel*dt;
		if(ghostViewSpeed>desigSpd)
		{
			ghostViewSpeed=desigSpd;
		}
	}
	else
	{
		ghostViewSpeed-=accel*dt;
		if(ghostViewSpeed<desigSpd)
		{
			ghostViewSpeed=desigSpd;
		}
	}

	YsVec3 displacement;
	displacement=viewAttitude.GetForwardVector()*ghostViewSpeed*dt;
	viewPoint+=displacement;

	double elv;
	elv=sim->GetFieldElevation(viewPoint.x(),viewPoint.z());
	if(viewPoint.y()<elv+0.25)
	{
		viewPoint.SetY(elv+0.25);
	}
}

void FsCamera::UpdateViewpointAccordingToPlayerAirplane(const double& distance, YSBOOL reset)
{
	const YsAtt3* att;
	const YsVec3* pos;

	if (activeViewPort->lookAtObj != NULL)
	{
		pos = &activeViewPort->lookAtObj->GetPosition();

		if (reset == YSTRUE || (*pos - viewRefPoint).GetSquareLength() >= distance * distance)
		{
			YsVec3 offset;
			att = &activeViewPort->lookAtObj->GetAttitude();
			offset.Set(20.0, 5.0, distance / 4.0);
			att->Mul(offset, offset);
			viewRefPoint = *pos + offset;
			if (viewRefPoint.y() < 5.0)
			{
				viewRefPoint.Set(viewRefPoint.x(), 5.0, viewRefPoint.z());
			}
		}
	}
}

void FsCamera::SelectNextView(FSBUTTONFUNCTION fnc, FSUSERCONTROL userControl)
{
	printf("ViewingControl | ");
	const int dir = (YSTRUE != FsGetKeyState(FSKEY_SHIFT) ? 1 : -1);
	double viewDist;
	YsAtt3 viewAtt;
	sim->GetRelView(viewDist, viewAtt);
	YsArray <ViewModeAndIndexAndPosition> fillerA, fillerB;
	switch (fnc)
	{
	case FSBTF_LOOKLEFT: //This is unsmooth because each function call is discrete. To smooth it, need to update view vector in FsSimulation
		viewOffSetAngle.AddH(-0.05);
		break;
	case FSBTF_LOOKRIGHT:
		viewOffSetAngle.AddH(0.05);
		break;
	case FSBTF_LOOKUP:
		viewOffSetAngle.AddP(-0.05);
		break;
	case FSBTF_LOOKDOWN:
		viewOffSetAngle.AddP(0.05);
		break;
	case FSBTF_LOOKFORWARD:
		viewOffsetRadius /= 1.05;
		break;
	case FSBTF_LOOKBACK:
		viewOffsetRadius *= 1.05;
		break;
	case FSBTF_COCKPITVIEW:
	{
		printf("Cockpit");
		viewOffSetAngle = YsZeroAtt();
		viewOffsetRadius = 1.0;
		FsExistence* playerObj = sim->GetPlayerObject();
		activeViewPort->parentObject = playerObj;
		
		if (playerObj != NULL)
		{
			YSBOOL bombLoaded = ((FsAirplane*)activeViewPort->parentObject)->Prop().FreeFallBombIsLoaded() > 0 ? YSTRUE : YSFALSE;
			if (activeViewPort->onboardViewId < activeViewPort->parentObject->CommonProp().GetNumOnboardViewpoint()-1 &&
				(activeViewPort->viewMode == FSCOCKPITVIEW ||
				 activeViewPort->viewMode == FSADDITIONALAIRPLANEVIEW ||
				 activeViewPort->viewMode == FSADDITIONALAIRPLANEVIEW_CABIN ||
				 activeViewPort->viewMode == FSBOMBINGVIEW))
			{
				activeViewPort->onboardViewId++;
			}
			else if (activeViewPort->viewMode != FSCOCKPITVIEW && activeViewPort->viewMode != FSBOMBINGVIEW)
			{
				activeViewPort->onboardViewId = 0;
			}
			else
			{
				if (bombLoaded == YSTRUE && activeViewPort->onboardViewId != 99) //This is janky and needs to be updated
				{
					activeViewPort->onboardViewId = 99;
				}
				else
				{
					activeViewPort->onboardViewId = 0;
				}
			}

			activeViewPort->nextViewMode = FSCOCKPITVIEW;
			viewSourceObj = NULL;
			viewTargetObj = NULL;
		}
	}
	break;
	case FSBTF_OUTSIDEPLAYERVIEW:
		printf("OutsidePlayer");
		viewOffSetAngle = YsZeroAtt();
		viewOffsetRadius = 1.0;
		if (activeViewPort->parentObject != NULL)
		{
			if (activeViewPort->viewMode == FSOUTSIDEPLAYERPLANE)
			{
				activeViewPort->nextViewMode = FSFIXEDPOINTPLAYERPLANE;
				UpdateViewpointAccordingToPlayerAirplane(500.0, YSTRUE);
			}
			else if (activeViewPort->viewMode == FSFIXEDPOINTPLAYERPLANE)
			{
				activeViewPort->nextViewMode = FSVARIABLEPOINTPLAYERPLANE;
				UpdateViewpointAccordingToPlayerAirplane(500.0, YSTRUE);
			}
			else if (activeViewPort->viewMode == FSVARIABLEPOINTPLAYERPLANE)
			{
				activeViewPort->nextViewMode = FSFROMTOPOFPLAYERPLANE;
				UpdateViewpointAccordingToPlayerAirplane(500.0, YSTRUE);
			}
			else if (activeViewPort->viewMode == FSFROMTOPOFPLAYERPLANE)
			{
				activeViewPort->nextViewMode = FSPLAYERPLANEFROMSIDE;
			}
			else
			{
				activeViewPort->nextViewMode = FSOUTSIDEPLAYERPLANE;
			}
			viewTargetObj = activeViewPort->parentObject;
			viewSourceObj = NULL;
		}
		break;
	case FSBTF_COMPUTERAIRPLANEVIEW:
		printf("ComputerAirplane");
		if (0 <= dir)
		{
			if (mainViewPort->viewMode == FSANOTHERAIRPLANE)
			{
				int i, nAir;

				nAir = sim->GetNumAirplane();
				viewSourceObj = sim->FindNextAirplane((const FsAirplane* )viewSourceObj);
				for (i = 0; i < nAir; ++i)
				{
					if (viewSourceObj == NULL || viewSourceObj == sim->GetPlayerObject() || viewSourceObj->IsAlive() != YSTRUE)
					{
						viewSourceObj = sim->FindNextAirplane((const FsAirplane* )viewSourceObj);
					}
					else
					{
						break;
					}
				}
			}
			else
			{
				mainViewPort->nextViewMode = FSANOTHERAIRPLANE;
				viewSourceObj = sim->FindNextAirplane(NULL);
			}
		}
		else
		{
			if (mainViewPort->viewMode == FSANOTHERAIRPLANE)
			{
				int i, nAir;

				nAir = sim->GetNumAirplane();
				viewSourceObj = sim->FindPrevAirplane((const FsAirplane* )viewSourceObj);
				for (i = 0; i < nAir; ++i)
				{
					if (viewSourceObj == NULL || viewSourceObj == sim->GetPlayerObject() || viewSourceObj->IsAlive() != YSTRUE)
					{
						viewSourceObj = sim->FindPrevAirplane((const FsAirplane* )viewSourceObj);
					}
					else
					{
						break;
					}
				}
			}
			else
			{
				mainViewPort->nextViewMode = FSANOTHERAIRPLANE;
				viewSourceObj = sim->FindPrevAirplane(NULL);
			}
		}
		viewTargetObj = sim->GetPlayerObject();
		break;
	case FSBTF_WEAPONVIEW:
	{
		mainViewPort->nextViewMode = FSMISSILEVIEW;
	}
	break;
	case FSBTF_CHANGEAIRPLANE:
		printf("ChangeAirplane");
		if (sim->EveryAirplaneIsRecordedAirplane() == YSTRUE || userControl == FSUSC_VIEWCONTROLONLY)
		{
			if (sim->CheckNoExtAirView() != YSTRUE)  // 2006/07/19, corrected 2006/08/25
			{
				activeViewPort->onboardViewId = 0;
				int i, nAir;
				FsAirplane* next;

				nAir = sim->GetNumAirplane();
				next = sim->GetPlayerAirplane();
				for (i = 0; i < nAir; i++)
				{
					next = sim->FindNextAirplane(next);
					if (next == NULL)
					{
						next = sim->FindNextAirplane(next);
					}
					if (next->IsAlive() == YSTRUE)
					{
						activeViewPort->SetParentObject(next);
						activeViewPort->lookAtObj = next;
						sim->SetPlayerAirplane(next, YSFALSE);
						break;
					}
				}
			}
			else
			{
				sim->AddTimedMessage("The server does not allow third airplane view.");
			}
		}
		break;
	case FSBTF_ILSVIEW:
		printf("ILS");
		sim->GetTowerILSArrays(fillerA, fillerB);
		if (viewTargetObj != NULL)
		{
			//Remove dead objects
			YsArray <GroundCameraList> activeList = ilsList;
			for (int i = 0; i < ilsList.GetN(); i++)
			{
				int rem = 0;
				if (activeList[i-rem].active == YSFALSE)
				{
					printf("Remove dead\n");
					activeList.Delete(i - rem);
					rem++;
				}
			}

			//Reorder ilsList by distance
			YsArray <double> dist(activeList.GetN(), NULL);
			int distIdx = 0;
			for (int idx = 0; idx < activeList.GetN(); ++idx)
			{
				dist[idx] = (activeList[idx].pos - viewTargetObj->GetPosition()).GetSquareLength();
			}
			YsQuickSort(dist.GetN(), dist.GetEditableArray(), activeList.GetEditableArray());

			if (mainViewPort->viewMode == FSCARRIERVIEW && viewSourceObj != NULL)
			{
				printf("Next ILS\n");
				YSSIZE_T nextIndex = 0;
				for (YSSIZE_T idx = 0; idx < activeList.GetN(); ++idx)
				{
					if (viewSourceObj->SearchKey() == activeList[idx].searchKey)
					{
						nextIndex = (idx + activeList.GetN() + dir) % activeList.GetN();
						break;
					}
				}
				viewSourceObj = activeList[nextIndex].obj;
				currentViewKey = activeList[nextIndex].searchKey;
			}
			else
			{
				printf("First ILS\n");
				viewSourceObj = activeList[0].obj;
				currentViewKey = activeList[0].searchKey;
			}

			mainViewPort->nextViewMode = FSCARRIERVIEW;
			viewSourcePos = viewSourceObj->GetPosition();
			viewTargetObj = sim->GetPlayerObject();
		}
		break;
	case FSBTF_CONTROLTOWERVIEW:
		printf("Tower");
		sim->GetTowerILSArrays(fillerA, fillerB);
		viewTargetObj = sim->GetPlayerObject();
		if (viewTargetObj != NULL)
		{
			YsArray <TowerCameraList> activeList = towerList;
			YsArray <double> dist(activeList.GetN(), NULL);
			for (int idx = 0; idx < activeList.GetN(); ++idx)
			{
				dist[idx] = (activeList[idx].pos - viewTargetObj->GetPosition()).GetSquareLength();
			}
			YsQuickSort(dist.GetN(), dist.GetEditableArray(), activeList.GetEditableArray());

			if (mainViewPort->viewMode == FSTOWERVIEW || mainViewPort->viewMode == FSTOWERVIEW_NOMAGNIFY)
			{
				YSSIZE_T nextIndex = 0;
				for (YSSIZE_T idx = 0; idx < activeList.GetN(); ++idx)
				{
					if (currentViewKey == activeList[idx].refKey)
					{
						nextIndex = (idx + activeList.GetN() + dir) % activeList.GetN();
						break;
					}
				}
				viewSourcePos = activeList[nextIndex].pos;
				currentViewKey = activeList[nextIndex].refKey;
			}
			else
			{
				viewSourcePos = activeList[0].pos;
				currentViewKey = activeList[0].refKey;
			}
		}
		mainViewPort->nextViewMode = FSTOWERVIEW;
		viewSourceObj = NULL;

		break;
	case FSBTF_OUTSIDEPLAYERVIEW2:
		printf("Outside2");
		viewOffSetAngle = YsZeroAtt();
		viewOffsetRadius = 1.0;
		if (mainViewPort->viewMode != FSOUTSIDEPLAYER2)
		{
			viewAtt.SetB(0.0);
			viewDist = 2.0;
			mainViewPort->nextViewMode = FSOUTSIDEPLAYER2;
		}
		else
		{
			viewDist *= 2;
			if (viewDist > 8.0) { viewDist = 1.0; }
		}
		viewTargetObj = sim->GetPlayerAirplane();
		viewSourceObj = NULL;
		break;
	case FSBTF_OUTSIDEPLAYERVIEW3:
		printf("Outside3");
		viewOffSetAngle = YsZeroAtt();
		viewOffsetRadius = 1.0;
		if (mainViewPort->viewMode != FSOUTSIDEPLAYER3)
		{
			viewAtt.SetB(0.0);
			viewDist = 2.0;
			mainViewPort->nextViewMode = FSOUTSIDEPLAYER3;
		}
		else
		{
			viewDist *= 2;
			if (viewDist > 8.0) { viewDist = 1.0; }
		}
		viewTargetObj = sim->GetPlayerAirplane();
		viewSourceObj = NULL;
		break;
	case FSBTF_GHOSTVIEW:
		printf("Ghost");
		if (mainViewPort->viewMode != FSGHOSTVIEW)
		{
			mainViewPort->nextViewMode = FSGHOSTVIEW;
			ghostViewSpeed = 0.0;
		}
		viewTargetObj = NULL;
		viewSourceObj = NULL;
		break;
	case FSBTF_VIEWZOOM:
		if (zoomUser < 12.0)
		{
			zoomUser *= 1.1;
		}
		break;
	case FSBTF_VIEWMOOZ:
		if (zoomUser > 1.0)
		{
			zoomUser /= 1.1;
		}
		break;
	case FSBTF_SWITCHVIEWTARGET:
		printf("SwitchTarget");
		const YSBOOL includePlayer = YSTRUE;
		auto targetAirCandidate = sim->GetAllAirplaneList();
		if (0 < targetAirCandidate.GetN())
		{
			const int dir = (YSTRUE != FsGetKeyState(FSKEY_SHIFT) ? 1 : -1);

			auto nextFocusAir = targetAirCandidate[0];
			for (int idx = 0; idx < targetAirCandidate.GetN(); ++idx)
			{
				if (targetAirCandidate[idx] == viewTargetObj)
				{
					nextFocusAir = targetAirCandidate.GetCyclic(idx + dir);
					break;
				}
			}
			mainViewPort->SetParentObject(sim->FindAirplane(nextFocusAir->SearchKey()));
			viewTargetObj = sim->FindAirplane(nextFocusAir->SearchKey());
		}
		break;
	}
	printf("\n");
}

void FsCamera::UpdateProjections(void)
{
	CalculateProjection(*mainViewPort);

	if (FsIsSubWindowOpen(0) == YSTRUE)
	{
		CalculateProjection(*leftViewPort);
	}
	else if (FsIsSubWindowOpen(1) == YSTRUE)
	{
		CalculateProjection(*rightViewPort);
	}
}

void FsCamera::CalculateProjection(FsViewPort& viewPort)
{
	int wid, hei;

	if (viewPort.nextViewMode == FSCOCKPITVIEW && cfg->centerCameraPerspective == YSFALSE)
	{
		viewPort.centerThisCamera = YSFALSE;
	}
	else
	{
		viewPort.centerThisCamera = YSTRUE;
	}

	FsGetDrawingAreaSize(wid,hei);
	YsVec2i drawingArea(wid, hei);

	if(viewPort.centerThisCamera == YSFALSE && viewPort.parentObject != NULL)
	{
		const YsVec2 scrnCen = ((FsAirplane*)viewPort.parentObject)->Prop().GetScreenCenter();
		viewPort.projection->cx = (int)((double)wid * (1.0 + scrnCen.x()) / 2.0);
		viewPort.projection->cy = (int)((double)hei * (1.0 - scrnCen.y()) / 2.0);
	}
	else
	{
		viewPort.projection->cx = wid / 2;
		viewPort.projection->cy = hei / 2;
	}

	if (drawingArea != viewPort.prevProjection->viewportDim || zoomUser != prevZoomUser || viewPort.zoomViewMode != viewPort.prevZoomViewMode)
	{
		viewPort.prevProjection = viewPort.projection;

		prevZoomUser = zoomUser;
		viewPort.projection->zoomProjection = viewPort.zoomViewMode;
		viewPort.prevZoomViewMode = viewPort.zoomViewMode;

		viewPort.projection->fovInPixels = YsGreater(wid / 2, hei / 2);

		viewPort.projection->prjMode = YsProjectionTransformation::PERSPECTIVE;
		viewPort.projection->prjPlnDist = (double)hei / (PROJ_PLANE_DIST_SCALE) * (viewPort.zoomViewMode * zoomUser / 1.8);
		viewPort.projection->tanFov = (double)viewPort.projection->fovInPixels / viewPort.projection->prjPlnDist;
		viewPort.projection->tanFovSecondary = (double)YsSmaller(wid / 2, hei / 2) / viewPort.projection->prjPlnDist;
		viewPort.projection->fov = atan(viewPort.projection->tanFov);
		viewPort.projection->fovSecondary = atan(viewPort.projection->tanFovSecondary);
		viewPort.projection->viewportDim.Set(wid, hei);

		viewPort.projection->nearz = frustrumNearDist;
		viewPort.projection->farz = frustrumFarDist;

		viewPort.projection->UncacheMatrix();
	}
}

void FsCamera::GetStandardProjection(class FsProjection& prj)
{
	int wid, hei, fovInPixel;
	FsGetWindowSize(wid, hei);

	prj.prjMode = YsProjectionTransformation::PERSPECTIVE;

	prj.cx = wid / 2;
	prj.cy = hei / 2;
	fovInPixel = YsGreater(prj.cx, prj.cy);

	prj.prjPlnDist = (double)wid / (double)1.41421356;
	prj.tanFov = (double)fovInPixel / prj.prjPlnDist;
	prj.viewportDim.Set(wid, hei);

	prj.nearz = 0.1;
	prj.farz = 20000.0;

	prj.zoomProjection = 1.0;

	prj.UncacheMatrix();
}

bool FsCamera::IsObjectVisible(const FsSimulation* sim, FsExistence* obj, FsViewPort& viewPort)
{
	obj->isOnScreen = YSFALSE;
	obj->isSubpixel = YSTRUE;

	//calculate object position in player's view
	YsVec3 objPosInCamSpace = viewPort.viewMat * obj->GetPosition();

	//load visual bounding box corners
	YsVec3 boxMin, boxMax;
	obj->vis.GetBoundingBox(boxMin, boxMax);

	YsVec3 empty(0.0, 0.0, 0.0);
	if (boxMin == empty && boxMax == empty) //Blockplanes fail DNM bbox check. Simple hack to force drawing
	{
		obj->isOnScreen = YSTRUE;
		obj->isSubpixel = YSFALSE;
		return true;
	}

	//calculate span of bounding box
	double boundingBoxDiag = 1.0 * ((boxMin - boxMax).GetLength());

	//distance from object to camera (magnitude of obj position vector in camera space)
	double objDistToCam = objPosInCamSpace.GetLength();

	//compute obj size on screen
	double apparentRadInPixels = boundingBoxDiag * viewPort.projection->prjPlnDist / objDistToCam;

	//if object is within bounding box span length of cam, draw it regardless of viewport visibility
	//(angular culling method below sometimes fails for extreme angles at close distances to camera)
	if (objDistToCam < boundingBoxDiag)
	{
		obj->isOnScreen = YSTRUE;
		obj->isSubpixel = YSFALSE;
		return true;
	}

	//don't perform FOV check if obj too small to see
	if (apparentRadInPixels < 1.0)
	{
		obj->isOnScreen = YSFALSE;
		obj->isSubpixel = YSTRUE;
		return false;
	}

	// compute object's apparent angular radius:
	// (angle between bounding box span and cam axis at object's Z distance)
	//             .
	//            /|
	//           / |
	//          /  |
	//         /   | boundingBoxDiag
	//        /    |
	//       /x    |
	//  cam /------+---> cam axis (+Z)
	//      |      |
	//     objPosInCamSpace.z()
	//
	// angular offset (x): x = atan2(boundingBoxDiag, abs(objPosInCamSpace.z()))
	double objAngularRad = atan2(YsGreatestOf((boxMax).GetLength(), (boxMin).GetLength(),0.00), objDistToCam); //Must check both directions in case of offset center i.e. towers

	//compute view angles from camera axis
	//      +X/+Y  . objPosInCamSpace
	//      ^     /|
	//      |    / |
	//      |   /  |
	//      |  /   |
	//      | /    |
	//      |/a    | horizontal/vertical view angle (in XZ/YZ plane): a = atan2(objPosInCamSpace.x/y(), objPosInCamSpace.z())
	// cam /------+---> cam axis (+Z)
	//
	double objHorizViewAngle = atan2(objPosInCamSpace.x(), objPosInCamSpace.z());
	double objVertViewAngle = atan2(objPosInCamSpace.y(), objPosInCamSpace.z());

	//determine FOV angles based on portrait or landscape aspect ratio
	double horizFovAngle = viewPort.projection->viewportDim.x() >= viewPort.projection->viewportDim.y() ? viewPort.projection->fov : viewPort.projection->fovSecondary;
	double vertFovAngle = viewPort.projection->viewportDim.x() >= viewPort.projection->viewportDim.y() ? viewPort.projection->fovSecondary : viewPort.projection->fov;

	//check if the object is within horizontal and vertical FOV +/- angular rad
	//centerCameraPerspective check accounts for additional 1/6th FOV at top edge in cockpit view
	//temporary fudge factor 0.9 on negative vertical to bandaid tall tower edge cases
	bool objIsInFov = objHorizViewAngle >= -horizFovAngle - objAngularRad && objHorizViewAngle <= horizFovAngle + objAngularRad &&
		0.9*objVertViewAngle >= -vertFovAngle - objAngularRad && objVertViewAngle <= vertFovAngle * (1.67 - 0.67 * cfg->centerCameraPerspective) + objAngularRad;

	obj->isSubpixel = YSFALSE;
	if (objIsInFov == true) { obj->isOnScreen = YSTRUE; }

	return objIsInFov;
}


/*
void FsMakeBlackOutPolygon(YsGLVertexBuffer2D &vtxBuf,YsGLColorBuffer &colBuf,const double G)
{
	vtxBuf.CleanUp();
	colBuf.CleanUp();

	const double plusGLimit=9.0f;
	const double minusGLimit=-5.0f;

	if(G>plusGLimit || G<minusGLimit)
	{
		int cenX,cenY,rad;
		double blackness,r,g,b;

		if(G>0.0)
		{
			blackness=(G-plusGLimit)/6.0f;
			r=0.0f;
			g=0.0f;
			b=0.0f;
		}
		else
		{
			blackness=(minusGLimit-G)/6.0f;
			r=1.0f;
			g=0.0f;
			b=0.0f;
		}


		int wid,hei;
		FsGetWindowSize(wid,hei);

		cenX=wid/2;
		cenY=hei/2;
		rad=(int)sqrt((double)(wid*wid+hei*hei));



		double circle[]=
		{
			 1.0       , 0.0,
			 1.4142/2.0, 1.4142/2.0,
			 0.0       , 1.0,
			-1.4142/2.0, 1.4142/2.0,
			-1.0       , 0.0,
			-1.4142/2.0,-1.4142/2.0,
			 0.0       ,-1.0,
			 1.4142/2.0,-1.4142/2.0,
			 1.0       , 0.0
		};

		for(int i=0; i<5; i++)  // i==0:Inner circle    smaller i->lighter black
		{
			double alpha[2]=
			{
				blackness*YsSqr((double)(1+ i   /5.0)),
				blackness*YsSqr((double)(1+(i+1)/5.0))
			};
			const double radius[2]=
			{
				(double)(i*rad/5),
				(double)((i+1)*rad/5)
			};

			alpha[0]=YsBound(alpha[0],0.0,1.0);
			alpha[1]=YsBound(alpha[1],0.0,1.0);

			for(int j=0; j<8; j++)
			{
				const YsVec2 quad[4]=
				{
					YsVec2(cenX+circle[ j   *2]*radius[0],cenY+circle[ j   *2+1]*radius[0]),
					YsVec2(cenX+circle[(j+1)*2]*radius[0],cenY+circle[(j+1)*2+1]*radius[0]),
					YsVec2(cenX+circle[(j+1)*2]*radius[1],cenY+circle[(j+1)*2+1]*radius[1]),
					YsVec2(cenX+circle[ j   *2]*radius[1],cenY+circle[ j   *2+1]*radius[1])
				};

				colBuf.Add(r,g,b,alpha[0]);
				vtxBuf.Add(quad[0]);
				colBuf.Add(r,g,b,alpha[0]);
				vtxBuf.Add(quad[1]);
				colBuf.Add(r,g,b,alpha[1]);
				vtxBuf.Add(quad[2]);

				colBuf.Add(r,g,b,alpha[1]);
				vtxBuf.Add(quad[2]);
				colBuf.Add(r,g,b,alpha[1]);
				vtxBuf.Add(quad[3]);
				colBuf.Add(r,g,b,alpha[0]);
				vtxBuf.Add(quad[0]);
			}
		}
	}
}
*/

/*
void FsSimulation::SimDrawBlackoutGl(const viewPort &viewPort) const
{
	const FsAirplane *playerPlane;

	playerPlane=GetPlayerAirplane();
	int i,j;
	const double plusGLimit=9.0;
	const double minusGLimit=-5.0;

	if(cfgPtr->blackOut==YSTRUE &&
	   viewPort.viewMode==FSCOCKPITVIEW &&
	   playerPlane!=NULL &&
	   playerPlane->IsAlive()==YSTRUE &&
	   (playerPlane->Prop().GetG()>plusGLimit ||
		playerPlane->Prop().GetG()<minusGLimit))
	{
		int vp[4],cenX,cenY,rad;
		double blackness,r,g,b;

		glDisable(GL_LIGHTING);

		if(playerPlane->Prop().GetG()>0.0)
		{
			blackness=(playerPlane->Prop().GetG()-plusGLimit)/6.0;
			r=0.0;
			g=0.0;
			b=0.0;
		}
		else
		{
			blackness=(minusGLimit-playerPlane->Prop().GetG())/6.0;
			r=1.0;
			g=0.0;
			b=0.0;
		}

		glGetIntegerv(GL_VIEWPORT,vp);
		cenX=vp[2]/2;
		cenY=vp[3]/2;
		rad=(int)sqrt((double)(vp[2]*vp[2]+vp[3]*vp[3]));

		glMatrixMode(GL_PROJECTION);
		glPushMatrix();
		glLoadIdentity();
		glOrtho(0.0,double(vp[2]),double(vp[3]),0.0,-1.0,1.0);
		glMatrixMode(GL_MODELVIEW);
		glPushMatrix();
		glLoadIdentity();

		glDisable(GL_DEPTH_TEST);
		glDepthMask(0);
		//glEnable(GL_BLEND);
		//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		double circle[]=
		{
			 1.0       , 0.0,
			 1.4142/2.0, 1.4142/2.0,
			 0.0       , 1.0,
			-1.4142/2.0, 1.4142/2.0,
			-1.0       , 0.0,
			-1.4142/2.0,-1.4142/2.0,
			 0.0       ,-1.0,
			 1.4142/2.0,-1.4142/2.0,
			 1.0       , 0.0
		};

		glBegin(GL_QUADS);
		for(i=0; i<5; i++)  // i==0:Inner circle    smaller i->lighter black
		{
			double alpha[2];
			double radius[2];
			alpha[0]=blackness*YsSqr((double)(1+ i   /5.0));
			alpha[1]=blackness*YsSqr((double)(1+(i+1)/5.0));
			radius[0]=(double)(i*rad/5);
			radius[1]=(double)((i+1)*rad/5);

			alpha[0]=YsBound(alpha[0],0.0,1.0);
			alpha[1]=YsBound(alpha[1],0.0,1.0);

			for(j=0; j<8; j++)
			{
				glColor4d(r,g,b,alpha[0]);
				glVertex2d(cenX+circle[ j   *2]*radius[0],cenY+circle[ j   *2+1]*radius[0]);
				glVertex2d(cenX+circle[(j+1)*2]*radius[0],cenY+circle[(j+1)*2+1]*radius[0]);
				glColor4d(r,g,b,alpha[1]);
				glVertex2d(cenX+circle[(j+1)*2]*radius[1],cenY+circle[(j+1)*2+1]*radius[1]);
				glVertex2d(cenX+circle[ j   *2]*radius[1],cenY+circle[ j   *2+1]*radius[1]);
			}
		}
		glEnd();

		//glDisable(GL_BLEND);
		glDepthMask(~0);
		glEnable(GL_DEPTH_TEST);

		glMatrixMode(GL_PROJECTION);
		glPopMatrix();
		glMatrixMode(GL_MODELVIEW);
		glPopMatrix();

		YsString str;
		str.Printf("%.1lfG",playerPlane->Prop().GetG());

		int wid,hei;
		FsGetWindowSize(wid,hei);
		FsDrawString(wid/2-20,hei/2,str,YsRed());
	}
}
*/

/*
void FsSimulation::SimDrawBlackoutGl2(const viewPort &viewPort) const
{
	const FsAirplane *playerPlane=GetPlayerAirplane();
	const GLfloat plusGLimit=9.0f;
	const GLfloat minusGLimit=-5.0f;

	if(cfgPtr->blackOut==YSTRUE &&
	   viewPort.viewMode==FSCOCKPITVIEW &&
	   playerPlane!=NULL &&
	   playerPlane->IsAlive()==YSTRUE &&
	   (playerPlane->Prop().GetG()>plusGLimit ||
		playerPlane->Prop().GetG()<minusGLimit))
	{
		YsGLVertexBuffer2D vtxBuf;
		YsGLColorBuffer colBuf;
		FsMakeBlackOutPolygon(vtxBuf,colBuf,playerPlane->Prop().GetG());

		glDisable(GL_DEPTH_TEST);
		glDepthMask(GL_FALSE);

		YsGLSL2DRenderer renderer;
		renderer.DrawVtxCol(YsGL::TRIANGLES,vtxBuf.GetN(),vtxBuf,colBuf);

		YsString str;
		str.Printf("%.1lfG",playerPlane->Prop().GetG());

		int wid,hei;
		FsGetWindowSize(wid,hei);
		FsDrawString(wid/2-20,hei/2,str,YsRed());

		glDepthMask(GL_TRUE);
		glEnable(GL_DEPTH_TEST);
	}
}
*/