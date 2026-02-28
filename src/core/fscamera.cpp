#include "fscamera.h"
#include "fswindow.h"

ActualViewMode::ActualViewMode()
{
	actualViewMode = FSCOCKPITVIEW;
	actualViewHdg = 0.0;
	actualViewPch = 0.0;

	viewPoint = YsVec3::Origin();
	viewAttitude = YsZeroAtt();

	viewMagFix = 1.0;
	isViewPointInCloud = YSFALSE;
	fogVisibility = 0.0;
	centerThisCamera = YSTRUE;
	viewTargetDist = 0.0;
}

FsCamera::FsCamera()
{
	ghostViewSpeed = 0.0;
	timeStep = 0.0;
	viewRefPoint = YsOrigin();
	currentExCamera = 0;
	mainViewMode = new ActualViewMode;
	subViewModeL = new ActualViewMode;
	subViewModeR = new ActualViewMode;
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

void FsCamera::DecideAllViewPoint(FsSimulation *currentSim, const double dt, FSVIEWMODE nextViewMode)
{
	sim = currentSim;
	timeStep = dt;
	userInput = sim->GetUserInput();
	cfg = sim->GetConfig();
	ActualViewMode* vm = sim->GetActualViewMode();
	ActualViewMode* sw[2];
	sim->GetSubwindowViewModes(sw);
	
	DecideViewpointAndCheckIsInCloud(vm,nextViewMode,FsGetMainWindowDrawingAreaSize());
	//AutoViewChange(vm->actualViewMode,dt);
	sim->AutoViewChangeCallback(nextViewMode,dt);
	for(int i=0; i<FsMaxNumSubWindow; i++)
	{
		if(FsIsSubWindowOpen(i)==YSTRUE)
		{
			DecideViewpointAndCheckIsInCloud(sw[i], nextViewMode, FsGetSubWindowDrawingAreaSize());
		}
	}
}

void FsCamera::DecideViewpointAndCheckIsInCloud(ActualViewMode* actualViewMode,FSVIEWMODE nextViewMode,YsVec2i drawingAreaSize)
{
	ActualViewMode& vm = *actualViewMode;
	DecideViewpoint(vm, nextViewMode);

	actualViewMode->viewMat.Initialize();
	actualViewMode->viewMat.RotateXY(-actualViewMode->viewAttitude.b());
	actualViewMode->viewMat.RotateZY(-actualViewMode->viewAttitude.p());
	actualViewMode->viewMat.RotateXZ(-actualViewMode->viewAttitude.h());
	actualViewMode->viewMat.Translate(-actualViewMode->viewPoint);

	actualViewMode->isViewPointInCloud=sim->CheckIsInCloud(actualViewMode->viewPoint);

	if(actualViewMode->isViewPointInCloud!=YSTRUE)
	{
		actualViewMode->fogVisibility= sim->GetFogVis();
	}
	else
	{
		actualViewMode->fogVisibility=100.0;
	}

	//Call back to original function to draw texture shadowmap
	sim->SimCalculateShadowMap(vm, nextViewMode, drawingAreaSize);
}

void FsCamera::DecideViewpoint(ActualViewMode &actualViewMode,FSVIEWMODE mode) const
{
	const FsAirplane *playerPlane = sim->GetPlayerAirplane();

	actualViewMode.actualViewMode=mode;  // by Default
	actualViewMode.viewMagFix=1.0;       // by Default
	actualViewMode.actualViewHdg=userInput.viewHdg;  // by Default
	actualViewMode.actualViewPch=userInput.viewPch;  // by Default
	actualViewMode.centerThisCamera = YSTRUE; //default to centered, overwrite where required

	if(mode==FSGHOSTVIEW)
	{
		return;
	}

	if (playerPlane != NULL)
	{
		DecideViewpoint_Air(actualViewMode, mode, playerPlane);
		return;
	}
	else if (sim->CheckNoExtAirView() != YSTRUE)
	{
		FsAirplane* air;
		air = NULL;
		while ((air = sim->FindNextAirplane(air)) != NULL)
		{
			if (air != NULL)
			{
				DecideViewpoint_Air(actualViewMode, mode, air);
				return;
			}

		}
	}
	else
	{
		actualViewMode.viewPoint.Set(0.0, 10.0, 0.0);
		actualViewMode.viewAttitude.Set(0.0, 0.0, 0.0);
		return;
	}
}

void FsCamera::DecideViewpoint_Air(ActualViewMode &actualViewMode,FSVIEWMODE mode,const FsAirplane *playerPlane) const
{
	const FsAirplane* focus1 = sim->GetFocusAir();
	const FsAirplane* focus2 = sim->GetFocusAir2();
	const FsGround* focusGnd = sim->GetFocusGnd();
	switch(mode)
	{
	case FSCOCKPITVIEW:
		if(playerPlane->Prop().IsActive()==YSTRUE || playerPlane->Prop().IsAlive()==YSFALSE)
		{
			actualViewMode.centerThisCamera = cfg->centerCameraPerspective;
			YsVec3 cock;
			YsMatrix4x4 mat;
			playerPlane->Prop().GetCockpitPosition(cock);

			mat.Translate(playerPlane->GetPosition());
			mat.Rotate(playerPlane->GetAttitude());

			actualViewMode.viewPoint=mat*cock;

			actualViewMode.viewAttitude=playerPlane->GetAttitude();

			const YsAtt3 &neutAtt=playerPlane->Prop().GetNeutralHeadDirection();

			actualViewMode.viewAttitude.YawLeft(neutAtt.h());
			actualViewMode.viewAttitude.NoseUp(neutAtt.p());
			actualViewMode.viewAttitude.SetB(actualViewMode.viewAttitude.b()+neutAtt.b());

			actualViewMode.viewAttitude.YawLeft(userInput.viewHdg);
			actualViewMode.viewAttitude.NoseUp(userInput.viewPch);
		}
		else
		{
			if(playerPlane->Prop().GetFlightState()==FSOVERRUN)
			{
				DecideViewpoint(actualViewMode,FSFROMTOPOFPLAYERPLANE);
			}
			else
			{
				DecideViewpoint(actualViewMode,FSFIXEDPOINTPLAYERPLANE);
			}
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

			actualViewMode.viewPoint=mat*cock;

			actualViewMode.viewAttitude=playerPlane->GetAttitude();
			actualViewMode.viewAttitude.NoseUp(-YsDegToRad(60.0));
		}
		else
		{
			if(playerPlane->Prop().GetFlightState()==FSOVERRUN)
			{
				DecideViewpoint(actualViewMode,FSFROMTOPOFPLAYERPLANE);
			}
			else
			{
				DecideViewpoint(actualViewMode,FSFIXEDPOINTPLAYERPLANE);
			}
		}
		break;
	case FSOUTSIDEPLAYERPLANE:
		{
			actualViewMode.viewPoint=playerPlane->GetPosition();

			auto dist=playerPlane->GetApproximatedCollideRadius()*2.0;

			YsVec3 tmp;
			tmp=viewRefPoint-actualViewMode.viewPoint;
			tmp.Normalize();
			tmp*=dist;
			actualViewMode.viewPoint+=tmp;
			actualViewMode.viewAttitude.SetForwardVector(-tmp);
			actualViewMode.viewTargetDist=dist;
		}
		break;
	case FSFIXEDPOINTPLAYERPLANE:
		{
			YsVec3 tmp;
			tmp=(playerPlane->GetPosition())-viewRefPoint;

			actualViewMode.viewPoint=viewRefPoint+tmp*5.0/6.0;
			actualViewMode.viewAttitude.SetForwardVector(tmp);
		}
		break;
	case FSVARIABLEPOINTPLAYERPLANE:
		{
			YsVec3 tmp;
			tmp=playerPlane->Prop().GetPosition();
			actualViewMode.viewPoint=viewRefPoint;
			tmp=tmp-actualViewMode.viewPoint;
			actualViewMode.viewAttitude.SetForwardVector(tmp);
		}
		break;
	case FSFROMTOPOFPLAYERPLANE:
		actualViewMode.viewPoint=playerPlane->Prop().GetPosition();
		actualViewMode.viewPoint.Set(actualViewMode.viewPoint.x(),actualViewMode.viewPoint.y()+30.0,actualViewMode.viewPoint.z()-10.0);
		actualViewMode.viewAttitude.Set(0.0,YsDegToRad(-72),0.0);
		break;
	case FSPLAYERPLANEFROMSIDE:
		{
			const double viewTargetDist=playerPlane->Prop().GetOutsideRadius()*1.5;

			YsVec3 offset;
			actualViewMode.viewPoint=playerPlane->Prop().GetPosition();
			actualViewMode.viewAttitude=playerPlane->Prop().GetAttitude();
			actualViewMode.viewAttitude.Set(actualViewMode.viewAttitude.h()+YsPi/2.0,0.0,0.0);
			offset.Set(0.0,0.0,-viewTargetDist);
			offset.RotateXZ(actualViewMode.viewAttitude.h());
			actualViewMode.viewPoint+=offset;

			actualViewMode.viewTargetDist=viewTargetDist;
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

				actualViewMode.viewAttitude.SetForwardVector(viewDir);
				actualViewMode.viewAttitude.SetB(a1.b());

				if(YSTRUE== focus1->Prop().IsOnGround() && radius*sin(actualViewMode.viewAttitude.p())> focus1->Prop().GetGroundStandingHeight()/2.0)
				{
					const double p=asin((focus1->Prop().GetGroundStandingHeight()/2.0)/radius);
					actualViewMode.viewAttitude.SetP(p);
				}

				YsVec3 off(1.0,0.6,-3.0);
				off*=radius;
				actualViewMode.viewAttitude.Mul(off,off); // off=att.GetMatrix()*off;

				actualViewMode.viewTargetDist=off.GetLength();

				actualViewMode.viewPoint=p1+off;

				if(actualViewMode.viewPoint.y()< focus1->Prop().GetGroundElevation()+0.5)
				{
					actualViewMode.viewPoint.SetY(focus1->Prop().GetGroundElevation()+0.5);
				}

				return;
			}
		}
		// If no other airplane is found,
		DecideViewpoint(actualViewMode,FSCOCKPITVIEW);  // Actual viewmode will be automatically set
		break;
	case FSMISSILEVIEW:
		{
			YsVec3 &pos=actualViewMode.viewPoint;
			YsAtt3 &att=actualViewMode.viewAttitude;
			if(sim->FindFirstGuidedMissile(pos,att)==YSOK)
			{
				YsVec3 off;
				att.SetB(0.0);
				off.Set(0.0,2.0,-10.0);
				att.Mul(off,off);
				pos=pos+off;
			}
			else if(sim->CheckNoExtAirView()==YSTRUE)  // 2006/06/11
			{
				DecideViewpoint(actualViewMode,FSCOCKPITVIEW);
			}
			else
			{
				DecideViewpoint(actualViewMode,FSCOCKPITVIEW);
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
				res=sim->FindOldestMissileOfOwner(actualViewMode.viewPoint,actualViewMode.viewAttitude,playerPlane);
			}
			else if(mode==FSMYWEAPONVIEW_NEW)
			{
				res= sim->FindNewestMissileOfOwner(actualViewMode.viewPoint,actualViewMode.viewAttitude,playerPlane);
			}

			if(res==YSOK)
			{
				YsVec3 off;
				actualViewMode.viewAttitude.SetB(0.0);
				off.Set(0.0,2.0,-10.0);
				actualViewMode.viewAttitude.Mul(off,off);
				actualViewMode.viewPoint+=off;
			}
			else if(sim->CheckNoExtAirView()==YSTRUE)
			{
				DecideViewpoint(actualViewMode,FSCOCKPITVIEW);
			}
			else
			{
				DecideViewpoint(actualViewMode,FSLOCKEDTARGETVIEW);
			}
		}
		break;
	case FSLOCKEDTARGETVIEW:
		if(playerPlane!=NULL)
		{
			const FsExistence *trg;
			if(playerPlane->Prop().GetSelectedWeaponPerformance().targetAir == YSTRUE ||
				playerPlane->Prop().GetSelectedWeaponPerformance().targetGnd == YSTRUE)
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
					double viewTargetDist=trg->GetApproximatedCollideRadius()*1.5;

					YsVec3 ev,uv;
					ev=trg->GetPosition()-playerPlane->GetPosition();
					ev.Normalize();
					uv=playerPlane->GetAttitude().GetUpVector();
					actualViewMode.viewPoint=trg->GetPosition()-ev*viewTargetDist;
					actualViewMode.viewAttitude.SetTwoVector(ev,uv);
					return;
				}
			}
			else if(playerPlane->Prop().GetSelectedWeaponType()==FSWEAPON_GUN)
			{
				if(playerPlane->Prop().GetHasPilotControlledTurret()==YSTRUE)
				{
					YsVec3 dir;
					playerPlane->Prop().GetFirstPilotControlledTurretPosition(actualViewMode.viewPoint);
					playerPlane->Prop().GetFirstPilotControlledTurretDirection(dir);

					actualViewMode.viewAttitude.SetTwoVector(dir,playerPlane->GetAttitude().GetUpVector());
					actualViewMode.viewMagFix=40.0;
					return;
				}
				else
				{
					const FsAirplane *target;
					YsVec3 aim;
					if(sim->PassGunAim(target,aim)==YSOK)
					{
						double viewTargetDist=playerPlane->GetApproximatedCollideRadius();

						YsVec3 ev;
						ev=aim-playerPlane->Prop().GetPosition();
						ev.Normalize();
						actualViewMode.viewPoint=playerPlane->Prop().GetPosition()+ev*viewTargetDist;
						actualViewMode.viewAttitude.SetTwoVector(ev,playerPlane->GetAttitude().GetUpVector());
						actualViewMode.viewMagFix=40.0;
						actualViewMode.viewTargetDist=viewTargetDist;
						return;
					}
				}
			}

			double viewTargetDist=playerPlane->GetApproximatedCollideRadius();

			YsVec3 ev, vVec;
			YsAtt3 vAtt;
			playerPlane->Prop().GetVelocity(vVec);
			vAtt.SetForwardVector(vVec);
			actualViewMode.viewPoint=playerPlane->Prop().GetPosition();
			actualViewMode.viewAttitude=vAtt;
			ev=actualViewMode.viewAttitude.GetForwardVector();
			actualViewMode.viewPoint+=ev*viewTargetDist;
			actualViewMode.viewMagFix=40.0;
			actualViewMode.viewTargetDist=viewTargetDist;
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
					actualViewMode.viewAttitude.SetTwoVector(*p2-*p1,upv);
				}
				else if(mode==FSAIRFROMAIRVIEW)
				{
					actualViewMode.viewAttitude.SetTwoVector(*p1-*p2,upv);
				}

				off.Set(1.0,0.6,-3.0);
				off*= focus1->GetApproximatedCollideRadius();
				actualViewMode.viewAttitude.Mul(off,off);  // off=att.GetMatrix()*off;
				actualViewMode.viewTargetDist=off.GetLength();

				actualViewMode.viewPoint=*p1+off;
				if(actualViewMode.viewPoint.y()<1.0)
				{
					actualViewMode.viewPoint.SetY(1.0);
				}
			}
		}
		else
		{
			DecideViewpoint(actualViewMode,FSCOCKPITVIEW);
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

				actualViewMode.viewPoint=ofstAir+mat*off;
				actualViewMode.viewTargetDist=ofstAir.GetLength();

				actualViewMode.viewMagFix=2.0;

				actualViewMode.viewAttitude.SetForwardVector(focus1->GetPosition()-actualViewMode.viewPoint);
			}
			else
			{
				DecideViewpoint(actualViewMode,FSCOCKPITVIEW);
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
				actualViewMode.viewAttitude.SetForwardVector(dir);
				actualViewMode.viewPoint= towerPos;

				if(mode==FSTOWERVIEW)
				{
					actualViewMode.viewMagFix=8.0;
				}
			}
			else
			{
				DecideViewpoint(actualViewMode,FSCOCKPITVIEW);
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

			actualViewMode.viewPoint=focus1->GetPosition()+ofst-dir*r;
			if(actualViewMode.viewPoint.y()< towerPos.y())
			{
				actualViewMode.viewPoint.SetY(towerPos.y());
			}
			actualViewMode.viewTargetDist=(ofst-dir*r).GetLength();

			dir=focus1->GetPosition()-actualViewMode.viewPoint;

			actualViewMode.viewAttitude.SetForwardVector(dir);
			actualViewMode.viewAttitude.SetB(0.0);

			actualViewMode.viewMagFix=1.0;
		}
		else
		{
			DecideViewpoint(actualViewMode,FSCOCKPITVIEW);
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

			actualViewMode.viewAttitude.SetForwardVector(vec);
			actualViewMode.viewAttitude.SetB(0.0);
			actualViewMode.viewTargetDist=rad;

			actualViewMode.viewPoint=playerPlane->GetPosition()-vec;
		}
		else
		{
			DecideViewpoint(actualViewMode,FSCOCKPITVIEW);
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

			actualViewMode.viewAttitude.SetForwardVector(vec);
			actualViewMode.viewAttitude.SetB(0.0);
			actualViewMode.viewTargetDist=rad;

			actualViewMode.viewPoint=playerPlane->GetPosition()-vec;
		}
		else
		{
			DecideViewpoint(actualViewMode,FSCOCKPITVIEW);
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

			actualViewMode.viewPoint=focus1->GetPosition()-dir;
			actualViewMode.viewAttitude=relAtt;
			actualViewMode.viewTargetDist=dist;
		}
		else
		{
			DecideViewpoint(actualViewMode,FSCOCKPITVIEW);
		}
		break;
	case FSVERTICALORBITINGVIEW:
		DecideViewpoint(actualViewMode,FSOUTSIDEPLAYER2);
		actualViewMode.viewAttitude.SetB(0.0);
		break;
	case FSHORIZONTALORBITINGVIEW:
		DecideViewpoint(actualViewMode,FSOUTSIDEPLAYER2);
		break;
	case FSTURNVIEW:
		DecideViewpoint(actualViewMode,FSOUTSIDEPLAYER2);
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

			actualViewMode.viewPoint = toLookAt->GetPosition() + ofst - dir;
			actualViewMode.viewAttitude = relAtt;
			actualViewMode.viewTargetDist = dist;
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
			actualViewMode.viewAttitude.SetTwoVector(newEv, newUv);
			actualViewMode.viewAttitude.Mul(dir, dir);
			actualViewMode.viewTargetDist = dist;

			YsVec3 ofst = toLookAt->GetLookAtOffset();
			toLookAt->GetAttitude().Mul(ofst, ofst);

			actualViewMode.viewPoint = toLookAt->GetPosition() + ofst - dir;
		}
	}
	break;
	case FSADDITIONALAIRPLANEVIEW:
		if (playerPlane != NULL)
		{
			actualViewMode.centerThisCamera = cfg->centerCameraPerspective;
			const FsAdditionalViewpoint* vp;
			vp = playerPlane->GetAdditionalView(currentExCamera);
			if (vp != NULL)
			{
				switch (vp->vpType)
				{
				default:
				case FS_ADVW_INSIDE:
					actualViewMode.actualViewMode = FSADDITIONALAIRPLANEVIEW;
					break;
				case FS_ADVW_OUTSIDE:
					actualViewMode.actualViewMode = FSOUTSIDEPLAYER2;
					break;
				case FS_ADVW_CABIN:
					actualViewMode.actualViewMode = FSADDITIONALAIRPLANEVIEW_CABIN;
					break;
				}

				YsVec3 ev, uv;
				playerPlane->GetMatrix().Mul(actualViewMode.viewPoint, vp->pos, 1.0);
				playerPlane->GetMatrix().Mul(ev, vp->att.GetForwardVector(), 0.0);
				playerPlane->GetMatrix().Mul(uv, vp->att.GetUpVector(), 0.0);
				actualViewMode.viewAttitude.SetTwoVector(ev, uv);

				actualViewMode.viewAttitude.YawLeft(userInput.viewHdg);
				actualViewMode.viewAttitude.NoseUp(userInput.viewPch);
			}
			else
			{
				DecideViewpoint(actualViewMode, FSCOCKPITVIEW);
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
			actualViewMode.actualViewHdg = YsPi;
			actualViewMode.actualViewPch = 0.0;
			break;
		case FS45DEGREERIGHTVIEW:
			actualViewMode.actualViewHdg = -YsPi / 4.0;
			actualViewMode.actualViewPch = 0.0;
			break;
		case FS45DEGREELEFTVIEW:
			actualViewMode.actualViewHdg = YsPi / 4.0;
			actualViewMode.actualViewPch = 0.0;
			break;
		case FS90DEGREERIGHTVIEW:
			actualViewMode.actualViewHdg = -YsPi / 2.0;
			actualViewMode.actualViewPch = 0.0;
			break;
		case FS90DEGREELEFTVIEW:
			actualViewMode.actualViewHdg = YsPi / 2.0;
			actualViewMode.actualViewPch = 0.0;
			break;
		case FSVIEWUP:
			actualViewMode.actualViewPch = YsPi / 2.0;
			actualViewMode.actualViewHdg = 0.0;
			break;
		case FSVIEWDOWN:
			actualViewMode.actualViewPch = -YsPi / 2.0;
			actualViewMode.actualViewHdg = 0.0;
			break;
		}

		if (playerPlane->IsActive() == YSTRUE || playerPlane->IsAlive() == YSFALSE)
		{
			YsVec3 cock = playerPlane->GetCockpitPosition();

			YsMatrix4x4 mat;
			mat.Translate(playerPlane->GetPosition());
			mat.Rotate(playerPlane->GetAttitude());

			actualViewMode.viewPoint = mat * cock;

			actualViewMode.viewAttitude = playerPlane->GetAttitude();
			actualViewMode.viewAttitude.YawLeft(actualViewMode.actualViewHdg);
			actualViewMode.viewAttitude.NoseUp(actualViewMode.actualViewPch);

			actualViewMode.actualViewMode = FSCOCKPITVIEW;
		}
		else
		{
			DecideViewpoint(actualViewMode, FSCOCKPITVIEW);
		}
		break;
	}
}
/*
void FsSimulation::SimDecideViewpoint_Gnd(ActualViewMode &actualViewMode,FSVIEWMODE mode,const FsGround *playerGround) const
{
	switch(mode)
	{
	default:
		actualViewMode.actualViewMode=FSCOCKPITVIEW;
		// Fall down to FSCOCKPITVIEW
	case FSCOCKPITVIEW:
		if(playerGround->Prop().IsActive()==YSTRUE || playerGround->Prop().IsAlive()==YSFALSE)
		{
			const YsVec3 cock=playerGround->Prop().GetUserViewPoint();

			actualViewMode.viewPoint=playerGround->GetMatrix()*cock;
			actualViewMode.viewAttitude=playerGround->GetAttitude();
			actualViewMode.centerThisCamera = cfgPtr->centerCameraPerspective;

			const YsAtt3 &neutAtt=YsZeroAtt(); // Will be added.

			actualViewMode.viewAttitude.YawLeft(neutAtt.h());
			actualViewMode.viewAttitude.NoseUp(neutAtt.p());
			actualViewMode.viewAttitude.SetB(actualViewMode.viewAttitude.b()+neutAtt.b());

			switch(playerGround->Prop().GetSelectedWeaponType())
			{
			default:
			case FSWEAPON_NULL:
				break;
			case FSWEAPON_GUN:
				//Was it correct? att.YawLeft(playerGround->Prop().GetAaaAim().h());
				//att.NoseUp(playerGround->Prop().GetAaaAim().p());
				actualViewMode.viewAttitude = playerGround->Prop().GetAaaAim();
				break;
			case FSWEAPON_AIM9:
			case FSWEAPON_AGM65:
				//Was it correct? att.YawLeft(playerGround->Prop().GetSamAim().h());
				//att.NoseUp(playerGround->Prop().GetSamAim().p());
				actualViewMode.viewAttitude = playerGround->Prop().GetSamAim();
				break;
			}

			actualViewMode.viewAttitude.YawLeft(userInput.viewHdg);
			actualViewMode.viewAttitude.NoseUp(userInput.viewPch);
		}
		else
		{
			// Outside view to let the user that the user is killed.
		}
		break;
	case FSTOWERVIEW:
	case FSTOWERVIEW_NOMAGNIFY:
		if (playerGround != NULL)
		{
			YsVec3 dir = playerGround->GetPosition() - towerViewPos;
			dir.Normalize();
			actualViewMode.viewAttitude.SetForwardVector(dir);

			YsVec3 ofst = playerGround->GetLookAtOffset();
			playerGround->GetAttitude().Mul(ofst, ofst);

			actualViewMode.viewPoint = towerViewPos;

			if (mode == FSTOWERVIEW)
			{
				actualViewMode.viewMagFix = 8.0;
			}
		}
		else
		{
			SimDecideViewpoint(actualViewMode, FSCOCKPITVIEW);
		}
		break;
	case FSBACKMIRRORVIEW:
	case FS45DEGREERIGHTVIEW:
	case FS45DEGREELEFTVIEW:
	case FS90DEGREERIGHTVIEW:
	case FS90DEGREELEFTVIEW:
	case FSVIEWUP:
	case FSVIEWDOWN:
	case FSADDITIONALAIRPLANEVIEW:
	case FSOUTSIDEPLAYER2:
	case FSOUTSIDEPLAYER3:
		SimDecideViewpoint_Common(actualViewMode, mode);
		break;
	case FSLOCKEDTARGETVIEW:
		if (playerGround != NULL)
		{
			const YsVec3 cock = playerGround->Prop().GetUserViewPoint();

			actualViewMode.viewPoint = playerGround->GetMatrix() * cock;
			actualViewMode.viewAttitude = playerGround->Prop().GetAttitude();

			const FsExistence* trg;
			if (playerGround->GetSelectedWeaponPerformance().targetAir == YSTRUE ||
				playerGround->GetSelectedWeaponPerformance().targetGnd == YSTRUE)
			{
				actualViewMode.viewAttitude = playerGround->Prop().GetSamAim();

				if (playerGround->Prop().GetSelectedWeaponPerformance().targetGnd == YSTRUE)
				{
					trg = playerGround->Prop().GetGroundTarget();
				}
				else
				{
					trg = playerGround->Prop().GetAirTarget();
				}

				if (trg != NULL)
				{
					YsVec3 ev, uv;
					ev = trg->GetPosition() - playerGround->GetPosition();
					ev.Normalize();
					uv = playerGround->GetAttitude().GetUpVector();
					actualViewMode.viewPoint = trg->GetPosition() - ev * trg->GetApproximatedCollideRadius() * 1.5;
					actualViewMode.viewAttitude.SetTwoVector(ev, uv);
					return;
				}
			}
			else if (playerGround->Prop().GetSelectedWeaponPerformance().category == FSWEAPONCAT_BULLET)
			{
				actualViewMode.viewAttitude = playerGround->Prop().GetAaaAim();

				if (playerGround->Prop().GetHasPilotControlledTurret() == YSTRUE)
				{
					YsVec3 dir;
					playerGround->Prop().GetFirstPilotControlledTurretPosition(actualViewMode.viewPoint);
					playerGround->Prop().GetFirstPilotControlledTurretDirection(dir);

					actualViewMode.viewAttitude.SetTwoVector(dir, playerGround->GetAttitude().GetUpVector());
					actualViewMode.viewMagFix = 40.0;
					return;
				}
				else
				{
					//FsAirplane *target;
					//YsVec3 aim;
					//if(SimCalculateGunAim(target,aim)==YSOK)
					//{
					//	YsVec3 ev;
					//	ev=aim-playerGround->Prop().GetPosition();
					//	ev.Normalize();
					//	pos=playerGround->Prop().GetPosition()+ev*playerGround->GetApproximatedCollideRadius();
					//	att.SetTwoVector(ev,playerGround->GetAttitude().GetUpVector());
					//	viewMagFix=40.0;
					//	return;
					//}
					actualViewMode.viewAttitude = playerGround->Prop().GetAaaAim();
					actualViewMode.viewMagFix = 40.0;
					return;
				}
			}

			YsVec3 ev;
			ev = actualViewMode.viewAttitude.GetForwardVector();
			actualViewMode.viewPoint += ev * playerGround->GetApproximatedCollideRadius();
			actualViewMode.viewMagFix = 40.0;
		}
		break;
	}
}

void FsSimulation::SimDecideViewpoint_Common(ActualViewMode& actualViewMode, FSVIEWMODE mode) const
{
	const FsExistence* playerObj = GetPlayerObject();
	switch (mode)
	{
	case FSOUTSIDEPLAYER2:
	{
		const FsExistence* toLookAt = NULL;
		if (NULL != playerObj && FSEX_GROUND == playerObj->GetType())
		{
			toLookAt = playerObj;
		}
		else
		{
			toLookAt = focusAir;
		}
		if (NULL != toLookAt)
		{
			YsVec3 dir, fomCen;
			double dist;
			dist = relViewDist * toLookAt->GetApproximatedCollideRadius();
			dir.Set(0, 0, dist);

			relViewAtt.Mul(dir, dir);

			YsVec3 ofst = toLookAt->GetLookAtOffset();
			toLookAt->GetAttitude().Mul(ofst, ofst);

			actualViewMode.viewPoint = toLookAt->GetPosition() + ofst - dir;
			actualViewMode.viewAttitude = relViewAtt;
			actualViewMode.viewTargetDist = dist;
		}
	}
	break;
	case FSOUTSIDEPLAYER3:
	{
		const FsExistence* toLookAt = NULL;
		if (NULL != playerObj && FSEX_GROUND == playerObj->GetType())
		{
			toLookAt = playerObj;
		}
		else
		{
			toLookAt = focusAir;
		}
		if (NULL != toLookAt)
		{
			YsVec3 ev[2], uv[2], newEv, newUv;
			YsVec3 dir, fomCen;
			double dist;
			dist = relViewDist * toLookAt->GetApproximatedCollideRadius();
			dir.Set(0, 0, dist);

			YsAtt3 airAtt;
			if (cfgPtr->externalCameraDelay != YSTRUE || toLookAt->GetAttitudeFromRecord(airAtt, currentTime - 0.8) != YSOK)
			{
				airAtt = toLookAt->GetAttitude();
			}
			airAtt.Mul(newEv, relViewAtt.GetForwardVector());
			airAtt.Mul(newUv, relViewAtt.GetUpVector());
			actualViewMode.viewAttitude.SetTwoVector(newEv, newUv);
			actualViewMode.viewAttitude.Mul(dir, dir);
			actualViewMode.viewTargetDist = dist;

			YsVec3 ofst = toLookAt->GetLookAtOffset();
			toLookAt->GetAttitude().Mul(ofst, ofst);

			actualViewMode.viewPoint = toLookAt->GetPosition() + ofst - dir;
		}
	}
	break;
	case FSADDITIONALAIRPLANEVIEW:
		if (playerObj != NULL)
		{
			actualViewMode.centerThisCamera = cfgPtr->centerCameraPerspective;
			const FsAdditionalViewpoint* vp;
			vp = playerObj->GetAdditionalView(mainWindowAdditionalAirplaneViewId);
			if (vp != NULL)
			{
				switch (vp->vpType)
				{
				default:
				case FS_ADVW_INSIDE:
					actualViewMode.actualViewMode = FSADDITIONALAIRPLANEVIEW;
					break;
				case FS_ADVW_OUTSIDE:
					actualViewMode.actualViewMode = FSOUTSIDEPLAYER2;
					break;
				case FS_ADVW_CABIN:
					actualViewMode.actualViewMode = FSADDITIONALAIRPLANEVIEW_CABIN;
					break;
				}

				YsVec3 ev, uv;
				playerObj->GetMatrix().Mul(actualViewMode.viewPoint, vp->pos, 1.0);
				playerObj->GetMatrix().Mul(ev, vp->att.GetForwardVector(), 0.0);
				playerObj->GetMatrix().Mul(uv, vp->att.GetUpVector(), 0.0);
				actualViewMode.viewAttitude.SetTwoVector(ev, uv);

				actualViewMode.viewAttitude.YawLeft(userInput.viewHdg);
				actualViewMode.viewAttitude.NoseUp(userInput.viewPch);
			}
			else
			{
				SimDecideViewpoint(actualViewMode, FSCOCKPITVIEW);
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
			actualViewMode.actualViewHdg = YsPi;
			actualViewMode.actualViewPch = 0.0;
			break;
		case FS45DEGREERIGHTVIEW:
			actualViewMode.actualViewHdg = -YsPi / 4.0;
			actualViewMode.actualViewPch = 0.0;
			break;
		case FS45DEGREELEFTVIEW:
			actualViewMode.actualViewHdg = YsPi / 4.0;
			actualViewMode.actualViewPch = 0.0;
			break;
		case FS90DEGREERIGHTVIEW:
			actualViewMode.actualViewHdg = -YsPi / 2.0;
			actualViewMode.actualViewPch = 0.0;
			break;
		case FS90DEGREELEFTVIEW:
			actualViewMode.actualViewHdg = YsPi / 2.0;
			actualViewMode.actualViewPch = 0.0;
			break;
		case FSVIEWUP:
			actualViewMode.actualViewPch = YsPi / 2.0;
			actualViewMode.actualViewHdg = 0.0;
			break;
		case FSVIEWDOWN:
			actualViewMode.actualViewPch = -YsPi / 2.0;
			actualViewMode.actualViewHdg = 0.0;
			break;
		}

		if (playerObj->IsActive() == YSTRUE || playerObj->IsAlive() == YSFALSE)
		{
			YsVec3 cock = playerObj->GetCockpitPosition();

			YsMatrix4x4 mat;
			mat.Translate(playerObj->GetPosition());
			mat.Rotate(playerObj->GetAttitude());

			actualViewMode.viewPoint = mat * cock;

			actualViewMode.viewAttitude = playerObj->GetAttitude();
			actualViewMode.viewAttitude.YawLeft(actualViewMode.actualViewHdg);
			actualViewMode.viewAttitude.NoseUp(actualViewMode.actualViewPch);

			actualViewMode.actualViewMode = FSCOCKPITVIEW;
		}
		else
		{
			SimDecideViewpoint(actualViewMode, FSCOCKPITVIEW);
		}
		break;
	}
}
*/

/*
void FsSimulation::SimAutoViewChange(FSVIEWMODE mainWindowViewMode,const double dt)
{
	switch(mainWindowViewMode)
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
			auto playerPlane=GetPlayerAirplane();
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
		if(CheckNoExtAirView()!=YSTRUE)  // 2006/06/11
		{
			for(int i=0; i<GetNumAirplane(); i++)
			{
				const FsAirplane *air=focusAir;
				if(air==NULL || air==GetPlayerAirplane() || air->IsAlive()!=YSTRUE)
				{
					focusAir=FindNextAirplane(focusAir);
					if(focusAir==NULL)
					{
						focusAir=FindNextAirplane(focusAir);
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
			focusAir=FindNextAirplane(NULL);
		}
		break;
	case FSSPOTPLANEVIEW:
		if(focusAir!=NULL)
		{
			YsVec3 dir;
			double dist;
			dist=relViewDist*focusAir->GetApproximatedCollideRadius();
			dir.Set(0,0,dist);

			YsVec3 ev1,ev2;
			ev1=focusAir->GetAttitude().GetForwardVector();
			ev2=relViewAtt.GetForwardVector();

			relViewAtt.Mul(dir,dir); // dir=relViewAtt.GetMatrix()*dir;
			relViewAtt.SetB((ev1*ev2)*focusAir->GetAttitude().b()/2.0);
		}
		break;
	case FSVERTICALORBITINGVIEW:
		relViewAtt.NoseUp(-dt*YsPi/12.0);
		break;
	case FSHORIZONTALORBITINGVIEW:
		relViewAtt.YawLeft(dt*YsPi/12.0);
		break;
	case FSTURNVIEW:
		relViewAtt.SetH(relViewAtt.h()+dt*YsPi/12.0);
		break;
	}
}*/

/*
void FsSimulation::UpdateViewpointAccordingToPlayerAirplane(const double &distance,YSBOOL reset)
{
	const YsAtt3 *att;
	const YsVec3 *pos;
	FsAirplane *playerPlane;

	playerPlane=GetPlayerAirplane();
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
*/


void FsCamera::ProcessGhostView(FsSimulation* sim, const double dt, ActualViewMode* viewMode)
{
	FsFlightControl *userInput = &sim->GetUserInput();
	mainViewMode = viewMode;
	auto &viewPoint= mainViewMode->viewPoint;
	auto &viewAttitude= mainViewMode->viewAttitude;

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
void FsSimulation::GetProjection(FsProjection &prj,const ActualViewMode &actualViewMode)
{
	int wid, hei;
	const FsAirplane *playerPlane;
	playerPlane = GetPlayerAirplane();

	FsGetDrawingAreaSize(wid,hei);
	YsVec2i drawingArea(wid, hei);

	if (FsIsMainWindowActive() == YSFALSE)
	{
		if (FsIsSubWindowActive(0) == YSTRUE)
		{
			lastProjection = &lastProjSubWindow1;
		}
		else if (FsIsSubWindowActive(1) == YSTRUE)
		{
			lastProjection = &lastProjSubWindow2;
		}
	}
	else
	{
		lastProjection = &lastProjMainWindow;
	}

	if(actualViewMode.centerThisCamera == YSFALSE && NULL != playerPlane)
	{
		const YsVec2 scrnCen = playerPlane->Prop().GetScreenCenter();
		prj.cx = (int)((double)wid * (1.0 + scrnCen.x()) / 2.0);
		prj.cy = (int)((double)hei * (1.0 - scrnCen.y()) / 2.0);
	}
	else
	{
		prj.cx = wid / 2;
		prj.cy = hei / 2;
	}

	if (drawingArea != lastProjection->viewportDim || viewMagUser != lastViewMagUser || actualViewMode.viewMagFix != lastProjection->viewMagFix)
	{
		lastViewMagUser = viewMagUser;
		prj.viewMagFix = actualViewMode.viewMagFix;

		prj.fovInPixels = YsGreater(wid / 2, hei / 2);  // 2010/07/05 It was ...,prj.cx,prj.cy);

		prj.prjMode = YsProjectionTransformation::PERSPECTIVE;
		prj.prjPlnDist = (double)hei / (PROJ_PLANE_DIST_SCALE);  // 2010/07/05 Fix vertical fov (double)wid/(double)1.41421356;
		prj.prjPlnDist *= (actualViewMode.viewMagFix * viewMagUser / 1.8);
		prj.tanFov = (double)prj.fovInPixels / prj.prjPlnDist;
		prj.tanFovSecondary = (double)YsSmaller(wid / 2, hei / 2) / prj.prjPlnDist;
		prj.fov = atan(prj.tanFov);
		prj.fovSecondary = atan(prj.tanFovSecondary);
		prj.viewportDim.Set(wid, hei);

		prj.nearz = 0.1;
		prj.farz = 18000.0;

		prj.UncacheMatrix();

		*lastProjection = prj;
	}
	else
	{
		prj.fovInPixels = lastProjection->fovInPixels;
		prj.prjMode = lastProjection->prjMode;
		prj.prjPlnDist = lastProjection->prjPlnDist;
		prj.tanFov = lastProjection->tanFov;
		prj.tanFovSecondary = lastProjection->tanFovSecondary;
		prj.fov = lastProjection->fov;
		prj.fovSecondary = lastProjection->fovSecondary;
		prj.viewportDim.Set(wid, hei);
		prj.nearz = lastProjection->nearz;
		prj.farz = lastProjection->farz;
		prj.viewMagFix = lastProjection->viewMagFix;
	}

}
*/

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
		if (viewMagUser < 12.0)
		{
			viewMagUser *= 1.1;
		}
		break;
	case FSBTF_VIEWMOOZ:
		if (viewMagUser > 1.0)
		{
			viewMagUser /= 1.1;
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
