#ifndef FSCAMERA_IS_INCLUDED
#define FSCAMERA_IS_INCLUDED

#include <ysclass.h>
#include <ysclass11.h>
#include "fsdef.h"
#include "fscontrol.h"
#include "fsopengl.h"

class FsViewPort
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
		FSVIEWDOWN, 					//Added 01/10/2023 - for subwindow view up and down
		VIEW_UNSPECIFIED
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
	double zoomViewMode; //viewMagFix
	double prevZoomViewMode;
	//double zoomThisPort;
	//double prevZoomThisPort; //For subwindow-only view zoom, eg. targetting pod
	YSBOOL isViewPointInCloud;
	double fogVisibility;
	YSBOOL centerThisCamera;
	FsProjection* projection;
	FsProjection* prevProjection;
	double offsetRadius; //viewTargetDist
	int cockpitViewId;

	enum
	{
		NUM_SHADOW_MAP = 3
	};
	YsMatrix4x4 shadowProjMat[NUM_SHADOW_MAP];
	YsMatrix4x4 shadowViewMat[NUM_SHADOW_MAP];

	FsViewPort();
};

#include "fssimulation.h"
#include "fsconfig.h"

class FsSimulation;
//class FsFlightConfig;

class FsCamera : public FsViewPort
{
public:
	FsCamera();
	~FsCamera();

	double ghostViewSpeed;
	double timeStep;
	YsVec3 viewRefPoint;
	double zoomUser; //viewMagUser
	double prevZoomUser;
	FsViewPort* activeViewPort;
	FsViewPort* mainViewPort;
	FsViewPort* leftViewPort;
	FsViewPort* rightViewPort;

	FsSimulation* sim;
	FsFlightControl userInput;
	class FsFlightConfig* cfg;

	static const char* ViewmodeToStr(FSVIEWMODE viewmode);
	static FSVIEWMODE StrToViewmode(const char* str);

	void UpdateCameras(FsSimulation *sim);
	void PrepareViewPort(int port);
	void UpdateViewPort(FsViewPort& viewPort, FSVIEWMODE next);
	void ApplyViewPortEnvironment(FsViewPort* viewPort, YsVec2i drawingAreaSize);
	void DecideViewMode(FsViewPort& viewPort, FSVIEWMODE viewmode,  FsAirplane* playerPlane);
	void AutoViewChange(FSVIEWMODE viewMode);
	void UpdateViewpointAccordingToPlayerAirplane(const double& distance, YSBOOL reset);

	void ProcessGhostView(FsSimulation* sim, const double dt);
	//void ViewingControl()
	
	void UpdateProjections(void);
	void CalculateProjection(FsViewPort& viewPort);
	void GetStandardProjection(class FsProjection& prj);

	bool IsObjectVisible(const FsSimulation* sim, FsExistence* obj, FsViewPort& viewPort);

	/*void ViewingControl(FSBUTTONFUNCTION fnc, FSUSERCONTROL userControl);
	YsArray <ViewModeAndIndexAndPosition> MakeAvailableILSView(void) const;
	YsArray <ViewModeAndIndexAndPosition> MakeAvailableTowerView(void) const;
	YsArray <const FsAirplane*> MakeAvailableViewTargetAirplane(YSBOOL includePlayer) const;

	void FsMakeBlackOutPolygon(YsGLVertexBuffer2D &vtxBuf,YsGLColorBuffer &colBuf,const double G)
	void SimDrawBlackoutGl(const viewPort &viewPort) const
	void SimDrawBlackoutGl2(const viewPort &viewPort) const
	*/

};

#endif
