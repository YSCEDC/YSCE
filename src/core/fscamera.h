#ifndef FSCAMERA_IS_INCLUDED
#define FSCAMERA_IS_INCLUDED

#include "ysclass.h"
#include "ysclass11.h"
#include "fsdef.h"
#include "fscontrol.h"
#include "fsopengl.h"

class ActualViewMode
{
public:
	enum FSVIEWMODE
	{
		FSCOCKPITVIEW,
		FSOUTSIDEPLAYERPLANE,
		FSFIXEDPOINTPLAYERPLANE,
		FSVARIABLEPOINTPLAYERPLANE,
		FSFROMTOPOFPLAYERPLANE,
		FSANOTHERAIRPLANE,
		FSMISSILEVIEW,
		FSAIRTOAIRVIEW,
		FSAIRFROMAIRVIEW,
		FSPLAYERPLANEFROMSIDE,
		FSCARRIERVIEW,
		FSTESTVIEW1,
		FSTESTVIEW2,
		FSOUTSIDEPLAYER2,
		FSOUTSIDEPLAYER3,
		FSBOMBINGVIEW,
		FSTOWERVIEW,
		FSPLAYERTOGNDVIEW,
		FSGNDTOPLAYERVIEW,
		FSSPOTPLANEVIEW,

		FSMYWEAPONVIEW_OLD,  // For sub window
		FSMYWEAPONVIEW_NEW,
		FSBACKMIRRORVIEW,
		FS45DEGREERIGHTVIEW,
		FS45DEGREELEFTVIEW,
		FS90DEGREERIGHTVIEW,
		FS90DEGREELEFTVIEW,
		FSTELESCOPEVIEW,
		FSLOCKEDTARGETVIEW,
		FSGHOSTVIEW,

		FSAIRTOTOWERVIEW,
		FSAIRTOTOWERVIEWSOLO,
		FSTOWERVIEW_NOMAGNIFY,

		FSVERTICALORBITINGVIEW,         // 2005/06/07
		FSHORIZONTALORBITINGVIEW,       // 2005/06/07
		FSTURNVIEW,                     // 2005/06/07

		FSADDITIONALAIRPLANEVIEW,       // 2006/07/19 For additional view in cockpit
		FSADDITIONALAIRPLANEVIEW_CABIN,  // 2011/02/01 For additional view in cabin

		FSVIEWUP,
		FSVIEWDOWN 					//Added 01/10/2023 - for subwindow view up and down
	};

	class ViewModeAndIndex
	{
	public:
		FSVIEWMODE viewmode;
		int refIndex;

		void Set(FSVIEWMODE m, int r)
		{
			viewmode = m;
			refIndex = r;
		}
	};
	class ViewModeAndIndexAndPosition : public ViewModeAndIndex
	{
	public:
		YsVec3 pos;

		void Set(FSVIEWMODE m, int r, const YsVec3& p)
		{
			viewmode = m;
			refIndex = r;
			pos = p;
		}
	};

	FSVIEWMODE viewMode;
	FSVIEWMODE nextViewMode;
	double viewHdg, viewPch;
	YsVec3 viewPoint;
	YsAtt3 viewAttitude;
	YsMatrix4x4 viewMat;
	double viewMagFix;
	double prevViewMagFix;
	YSBOOL isViewPointInCloud;
	double fogVisibility;
	YSBOOL centerThisCamera;
	FsProjection projection;
	FsProjection prevProjection;
	double viewTargetDist;
	int cockpitViewId;

	enum
	{
		NUM_SHADOW_MAP = 3
	};
	YsMatrix4x4 shadowProjMat[NUM_SHADOW_MAP];
	YsMatrix4x4 shadowViewMat[NUM_SHADOW_MAP];

	ActualViewMode();
};

#include "fssimulation.h"
#include "fsconfig.h"

class FsSimulation;
//class FsFlightConfig;

class FsCamera : public ActualViewMode
{
public:
	FsCamera();
	double ghostViewSpeed;
	double timeStep;
	YsVec3 viewRefPoint;
	ActualViewMode *mainViewMode;
	ActualViewMode *subViewModeL;
	ActualViewMode *subViewModeR;

	FsSimulation* sim;
	FsFlightControl userInput;
	class FsFlightConfig* cfg;

	static const char* ViewmodeToStr(FSVIEWMODE viewmode);
	static FSVIEWMODE StrToViewmode(const char* str);

	void ProcessGhostView(FsSimulation *sim, const double dt);

	void DecideAllViewPoint(FsSimulation *sim, double dt);
	void DecideViewpointAndCheckIsInCloud(ActualViewMode* actualViewMode, FSVIEWMODE nextViewMode, YsVec2i drawingAreaSize);
	void DecideViewpoint(ActualViewMode& actualViewMode, FSVIEWMODE viewmode) const;
	void DecideViewpoint_Air(ActualViewMode& actualViewMode, FSVIEWMODE viewmode,  FsAirplane* playerPlane) const;
	void AutoViewChange(FSVIEWMODE viewMode);
	void UpdateViewpointAccordingToPlayerAirplane(const double& distance, YSBOOL reset);

	/*void ViewingControl(FSBUTTONFUNCTION fnc, FSUSERCONTROL userControl);
	YsArray <ViewModeAndIndexAndPosition> MakeAvailableILSView(void) const;
	YsArray <ViewModeAndIndexAndPosition> MakeAvailableTowerView(void) const;
	YsArray <const FsAirplane*> MakeAvailableViewTargetAirplane(YSBOOL includePlayer) const;

	void SimAutoViewChange(FSVIEWMODE mainWindowViewMode, const double dt);
	void SimDecideViewpointAndCheckIsInCloud(ActualViewMode& actualViewMode, FSVIEWMODE viewmode, YsVec2i drawingAreaSize);
	void SimDecideViewpoint(ActualViewMode& actualViewMode, FSVIEWMODE viewmode) const;
	void SimDecideViewpoint_Air(ActualViewMode& actualViewMode, FSVIEWMODE viewmode, const FsAirplane* playerPlane) const;
	void SimDecideViewpoint_Gnd(ActualViewMode& actualViewMode, FSVIEWMODE viewmode, const FsGround* playerGround) const;
	void SimDecideViewpoint_Common(ActualViewMode& actualViewMode, FSVIEWMODE viewmode) const;
	YSBOOL CheckNoExtAirView(void) const;

	void GetProjection(class FsProjection& prj, const ActualViewMode& actualViewMode);
	void SetSubWindowViewMode(int windowId, FSVIEWMODE viewMode);

	static void GetStandardProjection(class FsProjection& prj);

	void UpdateViewpointAccordingToPlayerAirplane(const double& distance, YSBOOL reset);*/

};

#endif
