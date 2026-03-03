#include <fssimplewindow.h>
#include "fscamera.h"
#include "fswindow.h"

static const double PROJ_PLANE_DIST_SCALE = 1.41421356 * 0.75;
static const double frustrumNearDist = 0.1;
static const double frustrumFarDist = 18000.0;

FsViewPort::FsViewPort()
{
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
	cockpitViewId = 0;

	projection = new FsProjection;
	projection->zoomProjection = 1.0;

	prevProjection = projection;
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

	//ViewingControl()

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
	viewPort.centerThisCamera = YSTRUE; //default to centered, overwrite where required

	if(viewPort.viewMode!=FSGHOSTVIEW)
	{
		if (playerPlane != NULL)
		{
			DecideViewMode(viewPort, viewPort.viewMode, playerPlane);
		}
		else if (sim->CheckNoExtAirView() != YSTRUE)
		{
			FsAirplane* air;
			air = NULL;
			while ((air = sim->FindNextAirplane(air)) != NULL)
			{
				if (air != NULL)
				{
					DecideViewMode(viewPort, viewPort.viewMode, air);
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

void FsCamera::DecideViewMode(FsViewPort &viewPort,FSVIEWMODE mode, FsAirplane *playerPlane)
{
	const FsAirplane* focus1 = sim->GetFocusAir();
	const FsAirplane* focus2 = sim->GetFocusAir2();
	const FsGround* focusGnd = sim->GetFocusGnd();
	switch(mode)
	{
	case FSCOCKPITVIEW:
		if(playerPlane->Prop().IsActive()==YSTRUE || playerPlane->Prop().IsAlive()==YSFALSE)
		{
			viewPort.centerThisCamera = cfg->centerCameraPerspective;
			YsVec3 cock;
			YsMatrix4x4 mat;
			playerPlane->Prop().GetCockpitPosition(cock);

			mat.Translate(playerPlane->GetPosition());
			mat.Rotate(playerPlane->GetAttitude());

			viewPort.viewPoint=mat*cock;

			viewPort.viewAttitude=playerPlane->GetAttitude();

			const YsAtt3 &neutAtt=playerPlane->Prop().GetNeutralHeadDirection();

			viewPort.viewAttitude.YawLeft(neutAtt.h());
			viewPort.viewAttitude.NoseUp(neutAtt.p());
			viewPort.viewAttitude.SetB(viewPort.viewAttitude.b()+neutAtt.b());

			viewPort.viewAttitude.YawLeft(userInput.viewHdg);
			viewPort.viewAttitude.NoseUp(userInput.viewPch);
		}
		else
		{
			UpdateViewPort(viewPort,FSFIXEDPOINTPLAYERPLANE);
		}
		break;
	case FSBOMBINGVIEW:
		if(playerPlane->Prop().IsActive()==YSTRUE || playerPlane->Prop().IsAlive()==YSFALSE)
		{
			YsVec3 cock;
			YsMatrix4x4 mat;
			playerPlane->Prop().GetCockpitPosition(cock);

			mat.Translate(playerPlane->GetPosition());
			mat.Rotate(playerPlane->GetAttitude());

			viewPort.viewPoint=mat*cock;

			viewPort.viewAttitude=playerPlane->GetAttitude();
			viewPort.viewAttitude.NoseUp(-YsDegToRad(60.0));
		}
		else
		{
			UpdateViewPort(viewPort,FSFIXEDPOINTPLAYERPLANE);
		}
		break;
	case FSOUTSIDEPLAYERPLANE:
		{
			viewPort.viewPoint=playerPlane->GetPosition();

			auto dist=playerPlane->GetApproximatedCollideRadius()*2.0;

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
			tmp=(playerPlane->GetPosition())-viewRefPoint;

			viewPort.viewPoint=viewRefPoint+tmp*5.0/6.0;
			viewPort.viewAttitude.SetForwardVector(tmp);
		}
		break;
	case FSVARIABLEPOINTPLAYERPLANE:
		{
			YsVec3 tmp;
			tmp=playerPlane->Prop().GetPosition();
			viewPort.viewPoint=viewRefPoint;
			tmp=tmp-viewPort.viewPoint;
			viewPort.viewAttitude.SetForwardVector(tmp);
		}
		break;
	case FSFROMTOPOFPLAYERPLANE:
		viewPort.viewPoint=playerPlane->Prop().GetPosition();
		viewPort.viewPoint.Set(viewPort.viewPoint.x(),viewPort.viewPoint.y()+30.0,viewPort.viewPoint.z()-10.0);
		viewPort.viewAttitude.Set(0.0,YsDegToRad(-72),0.0);
		break;
	case FSPLAYERPLANEFROMSIDE:
		{
			const double offsetRadius =playerPlane->Prop().GetOutsideRadius()*1.5;

			YsVec3 offset;
			viewPort.viewPoint=playerPlane->Prop().GetPosition();
			viewPort.viewAttitude=playerPlane->Prop().GetAttitude();
			viewPort.viewAttitude.Set(viewPort.viewAttitude.h()+YsPi/2.0,0.0,0.0);
			offset.Set(0.0,0.0,-offsetRadius);
			offset.RotateXZ(viewPort.viewAttitude.h());
			viewPort.viewPoint+=offset;

			viewPort.offsetRadius = offsetRadius;
		}
		break;
	case FSANOTHERAIRPLANE:
		if(sim->CheckNoExtAirView()!=YSTRUE)  // 2006/06/11
		{
			if(focus1!=NULL && focus1 !=playerPlane && focus1->IsAlive()==YSTRUE)
			{
				const YsVec3 &p1= focus1->GetPosition();
				const YsAtt3 &a1= focus1->GetAttitude();

				const YsVec3 &p2=playerPlane->GetPosition();
				// const YsAtt3 &a2=playerPlane->GetAttitude();

				const double radius= focus1->GetApproximatedCollideRadius();

				const YsVec3 viewDir=p2-p1;

				viewPort.viewAttitude.SetForwardVector(viewDir);
				viewPort.viewAttitude.SetB(a1.b());

				if(YSTRUE== focus1->Prop().IsOnGround() && radius*sin(viewPort.viewAttitude.p())> focus1->Prop().GetGroundStandingHeight()/2.0)
				{
					const double p=asin((focus1->Prop().GetGroundStandingHeight()/2.0)/radius);
					viewPort.viewAttitude.SetP(p);
				}

				YsVec3 off(1.0,0.6,-3.0);
				off*=radius;
				viewPort.viewAttitude.Mul(off,off); // off=att.GetMatrix()*off;

				viewPort.offsetRadius =off.GetLength();

				viewPort.viewPoint=p1+off;

				if(viewPort.viewPoint.y()< focus1->Prop().GetGroundElevation()+0.5)
				{
					viewPort.viewPoint.SetY(focus1->Prop().GetGroundElevation()+0.5);
				}

				return;
			}
		}
		// If no other airplane is found,
		UpdateViewPort(viewPort,FSCOCKPITVIEW);  // Actual viewmode will be automatically set
		break;
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
			if(mode==FSMYWEAPONVIEW_OLD)
			{
				res=sim->FindOldestMissileOfOwner(viewPort.viewPoint,viewPort.viewAttitude,playerPlane);
			}
			else if(mode==FSMYWEAPONVIEW_NEW)
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
		if(playerPlane!=NULL)
		{
			const FsExistence *trg;
			YsVec3 ccip;
			viewPort.zoomViewMode = 40.0;

			if((playerPlane->Prop().GetSelectedWeaponPerformance().targetAir == YSTRUE &&
				sim->FindAirplane(playerPlane->Prop().GetAirTargetKey()) != NULL) ||
				(playerPlane->Prop().GetSelectedWeaponPerformance().targetGnd == YSTRUE &&
					sim->FindGround(playerPlane->Prop().GetGroundTargetKey()) != NULL))
			{
				if(playerPlane->Prop().GetSelectedWeaponPerformance().targetAir != YSTRUE)
				{
					trg=sim->FindGround(playerPlane->Prop().GetGroundTargetKey());
				}
				else //Prioritize air targets if weapon can target both
				{
					trg=sim->FindAirplane(playerPlane->Prop().GetAirTargetKey());
				}

				if(trg!=NULL)
				{
					double offsetRadius =trg->GetApproximatedCollideRadius()*1.5;

					YsVec3 ev,uv;
					ev=trg->GetPosition()-playerPlane->GetPosition();
					ev.Normalize();
					uv=playerPlane->GetAttitude().GetUpVector();
					viewPort.viewPoint=trg->GetPosition()-ev* offsetRadius;
					viewPort.viewAttitude.SetTwoVector(ev,uv);
					viewPort.offsetRadius = offsetRadius;
					return;
				}
			}
			else if (playerPlane->Prop().GetSelectedWeaponPerformance().category == FSWEAPONCAT_FREEFALL &&
				playerPlane->Prop().ComputeEstimatedBombLandingPosition(ccip, sim->GetWeather()) == YSOK)
			{
				double offsetRadius = playerPlane->GetApproximatedCollideRadius();
				YsVec3 viewPos, viewDirec, cameraOffset, bombPos;
				YsAtt3 viewAtt;

				playerPlane->Prop().FindNextWeaponSlot(playerPlane->Prop().GetSelectedWeaponType(),bombPos);
				bombPos = playerPlane->Prop().GetMatrix() * bombPos;
				
				viewDirec = ccip - bombPos;
				viewAtt.SetForwardVector(viewDirec);
				viewAtt.SetUpVector(playerPlane->Prop().GetAttitude().GetUpVector());

				cameraOffset.Set(0.0, 0.0, offsetRadius);
				viewAtt.Mul(cameraOffset, cameraOffset);
				
				viewPort.viewPoint = bombPos + cameraOffset;
				viewPort.viewAttitude = viewAtt;
				viewPort.offsetRadius = offsetRadius;
				return;
			}
			else if(playerPlane->Prop().GetSelectedWeaponType()==FSWEAPON_GUN)
			{
				if(playerPlane->Prop().GetHasPilotControlledTurret()==YSTRUE)
				{
					YsVec3 dir;
					playerPlane->Prop().GetFirstPilotControlledTurretPosition(viewPort.viewPoint);
					playerPlane->Prop().GetFirstPilotControlledTurretDirection(dir);

					viewPort.viewAttitude.SetTwoVector(dir,playerPlane->GetAttitude().GetUpVector());
					return;
				}
				else
				{
					const FsAirplane *target;
					YsVec3 aim;
					if(sim->PassGunAim(target,aim)==YSOK)
					{
						double offsetRadius =playerPlane->GetApproximatedCollideRadius();

						YsVec3 ev;
						ev=aim-playerPlane->Prop().GetPosition();
						ev.Normalize();
						viewPort.viewPoint=playerPlane->Prop().GetPosition()+ev* offsetRadius;
						viewPort.viewAttitude.SetTwoVector(ev,playerPlane->GetAttitude().GetUpVector());
						viewPort.offsetRadius = offsetRadius;
						return;
					}
					else
					{
						YsVec3 gunPos, gunAim, cameraOffset;
						YsAtt3 gunDir;
						double offsetRadius = playerPlane->GetApproximatedCollideRadius();
						cameraOffset.Set(0.0, 0.0, offsetRadius);

						playerPlane->Prop().GetGunPosition(gunPos, gunAim);
						gunPos = playerPlane->Prop().GetMatrix() * gunPos;
						gunAim.Normalize();
						playerPlane->Prop().GetAttitude().Mul(gunAim,gunAim);
						gunDir.SetTwoVector(gunAim,playerPlane->Prop().GetAttitude().GetUpVector());

						viewPort.viewAttitude = gunDir;
						viewPort.viewPoint = gunPos + gunAim* offsetRadius;
						viewPort.offsetRadius = offsetRadius;
						return;
					}
				}
			}

			double offsetRadius =playerPlane->GetApproximatedCollideRadius();

			YsVec3 ev, vVec;
			YsAtt3 vAtt;
			playerPlane->Prop().GetVelocity(vVec);
			vAtt.SetForwardVector(vVec);
			viewPort.viewPoint=playerPlane->Prop().GetPosition();
			viewPort.viewAttitude=vAtt;
			ev=viewPort.viewAttitude.GetForwardVector();
			viewPort.viewPoint+=ev* offsetRadius;
			viewPort.offsetRadius = offsetRadius;
		}
		break;
	case FSAIRTOAIRVIEW:
	case FSAIRFROMAIRVIEW:
		if(sim->CheckNoExtAirView()!=YSTRUE)  // 2006/06/11
		{
			const FsAirplane *from=sim->GetFocusAir();
			const FsAirplane *to=sim->GetFocusAir2();

			if(focus1 !=NULL && focus2 !=NULL)
			{
				YsVec3 off;
				const YsVec3 *p1,*p2;
				const YsAtt3 *a1,*a2;
				YsVec3 upv;

				p1=&focus1->GetPosition();
				a1=&focus1->GetAttitude();

				p2=&focus2->GetPosition();
				a2=&focus2->GetAttitude();

				if(focus1->Prop().IsActive()==YSTRUE)
				{
					upv=a1->GetUpVector();
				}
				else
				{
					upv=a2->GetUpVector();
				}

				if(mode==FSAIRTOAIRVIEW)
				{
					viewPort.viewAttitude.SetTwoVector(*p2-*p1,upv);
				}
				else if(mode==FSAIRFROMAIRVIEW)
				{
					viewPort.viewAttitude.SetTwoVector(*p1-*p2,upv);
				}

				off.Set(1.0,0.6,-3.0);
				off*= focus1->GetApproximatedCollideRadius();
				viewPort.viewAttitude.Mul(off,off);  // off=att.GetMatrix()*off;
				viewPort.offsetRadius =off.GetLength();

				viewPort.viewPoint=*p1+off;
				if(viewPort.viewPoint.y()<1.0)
				{
					viewPort.viewPoint.SetY(1.0);
				}
			}
		}
		else
		{
			UpdateViewPort(viewPort,FSCOCKPITVIEW);
		}
		break;
	case FSCARRIERVIEW:
		if(NULL!=focusGnd && NULL != focus1)
		{
			if(focusGnd !=NULL && focusGnd->IsAlive()==YSTRUE)
			{
				YsVec3 off;
				YsMatrix4x4 mat;
				mat.Initialize();
				mat.Translate(focusGnd->GetPosition());
				mat.Rotate(focusGnd->GetAttitude());

				if(focusGnd->Prop().GetAircraftCarrierProperty()!=NULL)
				{
					off= focusGnd->Prop().GetAircraftCarrierProperty()->GetBridgePos();
				}
				else
				{
					off.Set(25.0,52.0,-28.0);
				}

				YsVec3 ofstAir= focus1->GetLookAtOffset();
				focus1->GetAttitude().Mul(ofstAir,ofstAir);

				viewPort.viewPoint=ofstAir+mat*off;
				viewPort.offsetRadius =ofstAir.GetLength();

				viewPort.zoomViewMode =2.0;

				viewPort.viewAttitude.SetForwardVector(focus1->GetPosition()-viewPort.viewPoint);
			}
			else
			{
				UpdateViewPort(viewPort,FSCOCKPITVIEW);
			}
		}
		break;
	case FSTOWERVIEW:
	case FSTOWERVIEW_NOMAGNIFY:
		{
			if(sim->GetFocusAir()!=NULL)
			{
				YsVec3 dir, towerPos;
				towerPos = sim->GetTowerPos();
				dir= sim->GetFocusAir()->GetPosition()-towerPos;
				dir.Normalize();
				viewPort.viewAttitude.SetForwardVector(dir);
				viewPort.viewPoint= towerPos;

				if(mode==FSTOWERVIEW)
				{
					viewPort.zoomViewMode =8.0;
				}
			}
			else
			{
				UpdateViewPort(viewPort,FSCOCKPITVIEW);
			}
		}
		break;
	case FSAIRTOTOWERVIEW:
	case FSAIRTOTOWERVIEWSOLO:
		if(focus1!=NULL)
		{
			double r;
			YsVec3 dir, towerPos;
			towerPos = sim->GetTowerPos();
			dir= towerPos -focus1->GetPosition();
			dir.Normalize();

			r=focus1->GetRadiusFromCollision();

			if(mode==FSAIRTOTOWERVIEW)
			{
				r*=3.0;
			}
			else
			{
				r*=1.5;
			}

			YsVec3 ofst=focus1->GetLookAtOffset();
			focus1->GetAttitude().Mul(ofst,ofst);

			viewPort.viewPoint=focus1->GetPosition()+ofst-dir*r;
			if(viewPort.viewPoint.y()< towerPos.y())
			{
				viewPort.viewPoint.SetY(towerPos.y());
			}
			viewPort.offsetRadius =(ofst-dir*r).GetLength();

			dir=focus1->GetPosition()-viewPort.viewPoint;

			viewPort.viewAttitude.SetForwardVector(dir);
			viewPort.viewAttitude.SetB(0.0);
		}
		else
		{
			UpdateViewPort(viewPort,FSCOCKPITVIEW);
		}
		break;
	case FSPLAYERTOGNDVIEW:
		if(focusGnd!=NULL)
		{
			YsVec3 gnd=focusGnd->GetCollisionShellCenter();

			focusGnd->GetAttitude().Mul(gnd,gnd);
			gnd+=focusGnd->GetPosition();

			YsVec3 vec=gnd-playerPlane->GetPosition();
			vec.Normalize();

			const double rad=playerPlane->Prop().GetOutsideRadius()*2.0;
			vec*=rad;
			vec.SubY(rad*0.5);
			if(vec.y()>=0.0)
			{
				vec.SetY(0.0);
			}

			viewPort.viewAttitude.SetForwardVector(vec);
			viewPort.viewAttitude.SetB(0.0);
			viewPort.offsetRadius =rad;

			viewPort.viewPoint=playerPlane->GetPosition()-vec;
		}
		else
		{
			UpdateViewPort(viewPort,FSCOCKPITVIEW);
		}
		break;
	case FSGNDTOPLAYERVIEW:
		if(focusGnd!=NULL)
		{
			YsVec3 gnd=focusGnd->GetCollisionShellCenter();

			focusGnd->GetAttitude().Mul(gnd,gnd);
			gnd+=focusGnd->GetPosition();

			YsVec3 vec=playerPlane->GetPosition()-gnd;
			vec.Normalize();

			const double rad=playerPlane->Prop().GetOutsideRadius()*2.0;
			vec*=rad;
			vec.SubY(rad*0.5);
			if(vec.y()>=0.0)
			{
				vec.SetY(0.0);
			}

			viewPort.viewAttitude.SetForwardVector(vec);
			viewPort.viewAttitude.SetB(0.0);
			viewPort.offsetRadius =rad;

			viewPort.viewPoint=playerPlane->GetPosition()-vec;
		}
		else
		{
			UpdateViewPort(viewPort,FSCOCKPITVIEW);
		}
		break;
	case FSSPOTPLANEVIEW:
		if(focus1!=NULL)
		{
			YsVec3 dir,fomCen;
			double relDist;
			YsAtt3 relAtt;
			sim->GetRelView(relDist, relAtt);
			double dist= relDist*focus1->GetApproximatedCollideRadius();
			dir.Set(0,0,dist);

			relAtt.Mul(dir,dir);

			viewPort.viewPoint=focus1->GetPosition()-dir;
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
	case FSOUTSIDEPLAYER2:
	{
		const FsExistence* toLookAt = focus1;
		double relDist;
		YsAtt3 relAtt;
		sim->GetRelView(relDist,relAtt);
		if (NULL != toLookAt)
		{
			YsVec3 dir, fomCen;
			double dist;
			dist = relDist * toLookAt->GetApproximatedCollideRadius();
			dir.Set(0, 0, dist);

			relAtt.Mul(dir, dir);

			YsVec3 ofst = toLookAt->GetLookAtOffset();
			toLookAt->GetAttitude().Mul(ofst, ofst);

			viewPort.viewPoint = toLookAt->GetPosition() + ofst - dir;
			viewPort.viewAttitude = relAtt;
			viewPort.offsetRadius = dist;
		}
	}
	break;
	case FSOUTSIDEPLAYER3:
	{
		const FsExistence* toLookAt = focus1;
		if (NULL != toLookAt)
		{
			YsVec3 ev[2], uv[2], newEv, newUv;
			YsVec3 dir, fomCen;
			double dist, relDist;
			YsAtt3 relAtt;
			sim->GetRelView(relDist, relAtt);
			dist = relDist * toLookAt->GetApproximatedCollideRadius();
			dir.Set(0, 0, dist);

			YsAtt3 airAtt;
			if (cfg->externalCameraDelay != YSTRUE || toLookAt->GetAttitudeFromRecord(airAtt, sim->CurrentTime() - 0.8) != YSOK)
			{
				airAtt = toLookAt->GetAttitude();
			}
			airAtt.Mul(newEv, relAtt.GetForwardVector());
			airAtt.Mul(newUv, relAtt.GetUpVector());
			viewPort.viewAttitude.SetTwoVector(newEv, newUv);
			viewPort.viewAttitude.Mul(dir, dir);
			viewPort.offsetRadius = dist;

			YsVec3 ofst = toLookAt->GetLookAtOffset();
			toLookAt->GetAttitude().Mul(ofst, ofst);

			viewPort.viewPoint = toLookAt->GetPosition() + ofst - dir;
		}
	}
	break;
	case FSADDITIONALAIRPLANEVIEW:
		if (playerPlane != NULL)
		{
			viewPort.centerThisCamera = cfg->centerCameraPerspective;
			const FsAdditionalViewpoint* vp;
			vp = playerPlane->GetAdditionalView(viewPort.cockpitViewId);
			if (vp != NULL)
			{
				switch (vp->vpType)
				{
				default:
				case FS_ADVW_INSIDE:
					viewPort.viewMode = FSADDITIONALAIRPLANEVIEW;
					break;
				case FS_ADVW_OUTSIDE:
					viewPort.viewMode = FSOUTSIDEPLAYER2;
					break;
				case FS_ADVW_CABIN:
					viewPort.viewMode = FSADDITIONALAIRPLANEVIEW_CABIN;
					break;
				}

				YsVec3 ev, uv;
				playerPlane->GetMatrix().Mul(viewPort.viewPoint, vp->pos, 1.0);
				playerPlane->GetMatrix().Mul(ev, vp->att.GetForwardVector(), 0.0);
				playerPlane->GetMatrix().Mul(uv, vp->att.GetUpVector(), 0.0);
				viewPort.viewAttitude.SetTwoVector(ev, uv);

				viewPort.viewAttitude.YawLeft(userInput.viewHdg);
				viewPort.viewAttitude.NoseUp(userInput.viewPch);
			}
			else
			{
				UpdateViewPort(viewPort, FSCOCKPITVIEW);
			}
		}
		break;
	case FSBACKMIRRORVIEW:
	case FS45DEGREERIGHTVIEW:
	case FS45DEGREELEFTVIEW:
	case FS90DEGREERIGHTVIEW:
	case FS90DEGREELEFTVIEW:
	case FSVIEWUP:
	case FSVIEWDOWN:
		switch (mode)
		{
		case FSBACKMIRRORVIEW:
			viewPort.viewHdg = YsPi;
			viewPort.viewPch = 0.0;
			break;
		case FS45DEGREERIGHTVIEW:
			viewPort.viewHdg = -YsPi / 4.0;
			viewPort.viewPch = 0.0;
			break;
		case FS45DEGREELEFTVIEW:
			viewPort.viewHdg = YsPi / 4.0;
			viewPort.viewPch = 0.0;
			break;
		case FS90DEGREERIGHTVIEW:
			viewPort.viewHdg = -YsPi / 2.0;
			viewPort.viewPch = 0.0;
			break;
		case FS90DEGREELEFTVIEW:
			viewPort.viewHdg = YsPi / 2.0;
			viewPort.viewPch = 0.0;
			break;
		case FSVIEWUP:
			viewPort.viewHdg = 0.0;
			viewPort.viewPch = YsPi / 2.0;
			break;
		case FSVIEWDOWN:
			viewPort.viewHdg = 0.0;
			viewPort.viewPch = -YsPi / 2.0;
			break;
		}

		if (playerPlane->IsActive() == YSTRUE || playerPlane->IsAlive() == YSFALSE)
		{
			YsVec3 cock = playerPlane->GetCockpitPosition();

			YsMatrix4x4 mat;
			mat.Translate(playerPlane->GetPosition());
			mat.Rotate(playerPlane->GetAttitude());

			viewPort.viewPoint = mat * cock;

			viewPort.viewAttitude = playerPlane->GetAttitude();
			viewPort.viewAttitude.YawLeft(viewPort.viewHdg);
			viewPort.viewAttitude.NoseUp(viewPort.viewPch);

			viewPort.viewMode = FSCOCKPITVIEW;
		}
		else
		{
			UpdateViewPort(viewPort, FSCOCKPITVIEW);
		}
		break;
	}
}

void FsCamera::AutoViewChange(FSVIEWMODE viewMode)
{
	const FsAirplane* focusAir = sim->GetFocusAir();
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
			auto playerPlane=sim->GetPlayerAirplane();
			if(nullptr!=playerPlane)
			{
				YsVec3 tmp=playerPlane->Prop().GetPosition();

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
				const FsAirplane *air=focusAir;
				if(air==NULL || air==sim->GetPlayerAirplane() || air->IsAlive()!=YSTRUE)
				{
					focusAir=sim->FindNextAirplane(focusAir);
					if(focusAir==NULL)
					{
						focusAir=sim->FindNextAirplane(focusAir);
					}
				}
				else
				{
					break;
				}
			}
		}
		break;
	case FSTOWERVIEW:
	case FSTOWERVIEW_NOMAGNIFY:
	case FSAIRTOTOWERVIEW:
	case FSAIRTOTOWERVIEWSOLO:
		if(focusAir==NULL)
		{
			focusAir=sim->FindNextAirplane(NULL);
		}
		break;
	case FSSPOTPLANEVIEW:
		if(focusAir!=NULL)
		{
			YsVec3 dir;
			double dist;
			dist= relDist *focusAir->GetApproximatedCollideRadius();
			dir.Set(0,0,dist);

			YsVec3 ev1,ev2;
			ev1=focusAir->GetAttitude().GetForwardVector();
			ev2=relAtt.GetForwardVector();

			relAtt.Mul(dir,dir); // dir=relViewAtt.GetMatrix()*dir;
			relAtt.SetB((ev1*ev2)*focusAir->GetAttitude().b()/2.0);
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

void FsCamera::UpdateViewpointAccordingToPlayerAirplane(const double &distance,YSBOOL reset)
{
	const YsAtt3 *att;
	const YsVec3 *pos;
	FsAirplane *playerPlane;

	playerPlane=sim->GetPlayerAirplane();
	if(playerPlane!=NULL)
	{
		pos=&playerPlane->GetPosition();

		if(reset==YSTRUE || (*pos-viewRefPoint).GetSquareLength()>=distance*distance)
		{
			YsVec3 offset;
			att=&playerPlane->GetAttitude();
			offset.Set(20.0,5.0,distance/4.0);
			att->Mul(offset,offset);
			viewRefPoint=*pos+offset;
			if(viewRefPoint.y()<5.0)
			{
				viewRefPoint.Set(viewRefPoint.x(),5.0,viewRefPoint.z());
			}
		}
	}
}

void FsCamera::ProcessGhostView(FsSimulation* sim, const double dt)
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

/*
void FsCamera::ViewingControl(FSBUTTONFUNCTION fnc, FSUSERCONTROL userControl)
{
	const int dir = (YSTRUE != FsGetKeyState(FSKEY_SHIFT) ? 1 : -1);

	switch (fnc)
	{
	case FSBTF_COCKPITVIEW:
	{
		const FsExistence* playerObj = GetPlayerObject();
		if (playerObj != NULL)
		{
			YsArray <ViewModeAndIndex> viewModeAndIndex;
			viewModeAndIndex.Increment();
			viewModeAndIndex.Last().Set(FSCOCKPITVIEW, 0);

			if (playerObj->GetType() == FSEX_AIRPLANE)
			{
				const FsAirplane* air = (const FsAirplane*)playerObj;
				for (int idx = 0; idx < air->Prop().GetNumAdditionalView(); ++idx)
				{
					viewModeAndIndex.Increment();
					viewModeAndIndex.Last().Set(FSADDITIONALAIRPLANEVIEW, idx);
				}
				if (air->Prop().FreeFallBombIsLoaded() > 0)
				{
					viewModeAndIndex.Increment();
					viewModeAndIndex.Last().Set(FSBOMBINGVIEW, 0);
				}
			}
			else if (playerObj->GetType() == FSEX_GROUND)
			{
				const FsGround* gnd = (const FsGround*)playerObj;
				for (int idx = 0; idx < gnd->Prop().GetNumAdditionalView(); ++idx)
				{
					viewModeAndIndex.Increment();
					viewModeAndIndex.Last().Set(FSADDITIONALAIRPLANEVIEW, idx);
				}
			}

			FSVIEWMODE nextViewMode = FSCOCKPITVIEW;
			int nextAdditionalAirplaneViewId = 0;
			const int curIndex = (mainWindowViewmode == FSADDITIONALAIRPLANEVIEW ? mainWindowAdditionalAirplaneViewId : 0);
			for (int idx = 0; idx < viewModeAndIndex.GetN(); ++idx)
			{
				if (mainWindowViewmode == viewModeAndIndex[idx].viewmode &&
					curIndex == viewModeAndIndex[idx].refIndex)
				{
					nextViewMode = viewModeAndIndex.GetCyclic(idx + dir).viewmode;
					nextAdditionalAirplaneViewId = viewModeAndIndex.GetCyclic(idx + dir).refIndex;
					break;
				}
			}
			mainWindowViewmode = nextViewMode;
			mainWindowAdditionalAirplaneViewId = nextAdditionalAirplaneViewId;
		}
	}
	break;
	case FSBTF_OUTSIDEPLAYERVIEW:
		if (GetPlayerAirplane() != NULL)
		{
			if (mainWindowViewmode == FSOUTSIDEPLAYERPLANE)
			{
				mainWindowViewmode = FSFIXEDPOINTPLAYERPLANE;
				UpdateViewpointAccordingToPlayerAirplane(500.0, YSTRUE);
			}
			else if (mainWindowViewmode == FSFIXEDPOINTPLAYERPLANE)
			{
				mainWindowViewmode = FSVARIABLEPOINTPLAYERPLANE;
				UpdateViewpointAccordingToPlayerAirplane(500.0, YSTRUE);
			}
			else if (mainWindowViewmode == FSVARIABLEPOINTPLAYERPLANE)
			{
				mainWindowViewmode = FSFROMTOPOFPLAYERPLANE;
				UpdateViewpointAccordingToPlayerAirplane(500.0, YSTRUE);
			}
			else if (mainWindowViewmode == FSFROMTOPOFPLAYERPLANE)
			{
				mainWindowViewmode = FSPLAYERPLANEFROMSIDE;
			}
			else
			{
				mainWindowViewmode = FSOUTSIDEPLAYERPLANE;
			}
		}
		break;
	case FSBTF_COMPUTERAIRPLANEVIEW:
		if (0 <= dir)
		{
			if (mainWindowViewmode == FSANOTHERAIRPLANE)
			{
				int i, nAir;

				nAir = GetNumAirplane();
				focusAir = FindNextAirplane(focusAir);
				for (i = 0; i < nAir; ++i)
				{
					if (focusAir == NULL || focusAir == GetPlayerObject() || focusAir->IsAlive() != YSTRUE)
					{
						focusAir = FindNextAirplane(focusAir);
					}
					else
					{
						break;
					}
				}
			}
			else
			{
				mainWindowViewmode = FSANOTHERAIRPLANE;
				focusAir = FindNextAirplane(NULL);
			}
		}
		else
		{
			if (mainWindowViewmode == FSANOTHERAIRPLANE)
			{
				int i, nAir;

				nAir = GetNumAirplane();
				focusAir = FindPrevAirplane(focusAir);
				for (i = 0; i < nAir; ++i)
				{
					if (focusAir == NULL || focusAir == GetPlayerObject() || focusAir->IsAlive() != YSTRUE)
					{
						focusAir = FindPrevAirplane(focusAir);
					}
					else
					{
						break;
					}
				}
			}
			else
			{
				mainWindowViewmode = FSANOTHERAIRPLANE;
				focusAir = FindPrevAirplane(NULL);
			}
		}
		break;
	case FSBTF_WEAPONVIEW:
	{
		mainWindowViewmode = FSMISSILEVIEW;
	}
	break;
	case FSBTF_CHANGEAIRPLANE:
		if (EveryAirplaneIsRecordedAirplane() == YSTRUE || userControl == FSUSC_VIEWCONTROLONLY)
		{
			if (CheckNoExtAirView() != YSTRUE)  // 2006/07/19, corrected 2006/08/25
			{
				int i, nAir;
				FsAirplane* next;

				nAir = GetNumAirplane();
				next = GetPlayerAirplane();
				for (i = 0; i < nAir; i++)
				{
					next = FindNextAirplane(next);
					if (next == NULL)
					{
						next = FindNextAirplane(next);
					}
					if (next->IsAlive() == YSTRUE)
					{
						SetPlayerAirplane(next, YSFALSE);
						break;
					}
				}
			}
			else
			{
				AddTimedMessage("The server does not allow third airplane view.");
			}
		}
		break;
	case FSBTF_ILSVIEW:
	case FSBTF_CONTROLTOWERVIEW:
	{
		YsArray <ViewModeAndIndexAndPosition> view;
		if (FSBTF_ILSVIEW == fnc)
		{
			view = MakeAvailableILSView();
		}
		else
		{
			view = MakeAvailableTowerView();
		}
		if (0 < view.GetN())
		{
			if (NULL == focusAir || (FSCARRIERVIEW != mainWindowViewmode && FSTOWERVIEW != mainWindowViewmode))
			{
				focusAir = GetPlayerAirplane();
				if (NULL == focusAir || YSTRUE != focusAir->IsAlive())
				{
					focusAir = FindFirstAliveAirplane(focusAir);
				}
			}
			if (NULL != focusAir)
			{
				YsArray <double> dist(view.GetN(), NULL);
				for (int idx = 0; idx < view.GetN(); ++idx)
				{
					dist[idx] = (view[idx].pos - focusAir->GetPosition()).GetSquareLength();
				}
				YsQuickSort(dist.GetN(), dist.GetEditableArray(), view.GetEditableArray());

				FSVIEWMODE curMode = mainWindowViewmode;
				int curIndex = 0;
				if (FSCARRIERVIEW == mainWindowViewmode && NULL != focusGnd)
				{
					curIndex = (int)focusGnd->SearchKey();
				}
				else if (FSTOWERVIEW == mainWindowViewmode)
				{
					curIndex = towerViewId;
				}

				YSSIZE_T nextIndex = 0;
				for (YSSIZE_T idx = 0; idx < view.GetN(); ++idx)
				{
					if (curMode == view[idx].viewmode && curIndex == view[idx].refIndex)
					{
						nextIndex = (idx + view.GetN() + dir) % view.GetN();
						break;
					}
				}

				auto& nextView = view[nextIndex];
				if (FSCARRIERVIEW == nextView.viewmode)
				{
					focusGnd = FindGround((YSHASHKEY)nextView.refIndex);
					if (NULL != focusGnd)
					{
						mainWindowViewmode = FSCARRIERVIEW;
					}
				}
				else if (FSTOWERVIEW == nextView.viewmode)
				{
					mainWindowViewmode = FSTOWERVIEW;
					towerViewId = nextView.refIndex;
					towerViewPos = nextView.pos;
				}
			}
		}
	}
	break;
	case FSBTF_OUTSIDEPLAYERVIEW2:
		if (mainWindowViewmode != FSOUTSIDEPLAYER2)
		{
			relViewAtt.SetB(0.0);
			focusAir = GetPlayerAirplane();
			relViewDist = 2.0;
			mainWindowViewmode = FSOUTSIDEPLAYER2;
		}
		else
		{
			relViewDist *= 2;
			if (relViewDist > 8.0) { relViewDist = 1.0; }
		}
		break;
	case FSBTF_OUTSIDEPLAYERVIEW3:
		if (mainWindowViewmode != FSOUTSIDEPLAYER3)
		{
			relViewAtt.SetB(0.0);
			focusAir = GetPlayerAirplane();
			relViewDist = 2.0;
			mainWindowViewmode = FSOUTSIDEPLAYER3;
		}
		else
		{
			relViewDist *= 2;
			if (relViewDist > 8.0) { relViewDist = 1.0; }
		}

		break;
	case FSBTF_GHOSTVIEW:
	{
		if (mainWindowViewmode != FSGHOSTVIEW)
		{
			mainWindowViewmode = FSGHOSTVIEW;
			ghostViewSpeed = 0.0;
		}
	}
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
	{
		const YSBOOL includePlayer = YSTRUE;
		auto targetAirCandidate = MakeAvailableViewTargetAirplane(includePlayer);
		if (0 < targetAirCandidate.GetN())
		{
			const int dir = (YSTRUE != FsGetKeyState(FSKEY_SHIFT) ? 1 : -1);

			auto nextFocusAir = targetAirCandidate[0];
			for (int idx = 0; idx < targetAirCandidate.GetN(); ++idx)
			{
				if (targetAirCandidate[idx] == focusAir)
				{
					nextFocusAir = targetAirCandidate.GetCyclic(idx + dir);
					break;
				}
			}
			focusAir = nextFocusAir;
		}
	}
	break;
	}
}*/

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
	const FsAirplane *playerPlane;
	playerPlane = sim->GetPlayerAirplane();

	FsGetDrawingAreaSize(wid,hei);
	YsVec2i drawingArea(wid, hei);

	if(viewPort.centerThisCamera == YSFALSE && NULL != playerPlane)
	{
		const YsVec2 scrnCen = playerPlane->Prop().GetScreenCenter();
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