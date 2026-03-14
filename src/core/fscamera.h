#ifndef FSCAMERA_IS_INCLUDED
#define FSCAMERA_IS_INCLUDED

#include <ysclass.h>
#include <ysclass11.h>
#include "fsdef.h"
#include "fscontrol.h"
#include "fsopengl.h"
#include "fsexistence.h"

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

		VIEW_ONBOARD,
		VIEW_ONBOARD_WEAPONVIEW,
		VIEW_ONBOARD_TURRET,
		VIEW_ONBOARD_TO_OBJECT,
		VIEW_FLYBY,
		VIEW_EXTERNAL,
		VIEW_WEAPON_FIXED,
		VIEW_WEAPON_EXTERNAL,
		VIEW_WEAPON_TO_TARGET,
		VIEW_WEAPON_FROM_TARGET,
		VIEW_OBJECT_FROM_CAMERA,
		VIEW_OBJECT_FROM_POINT,
		VIEW_OBJECT_TO_POINT,
		VIEW_OBJECT_FROM_CHASE,
		VIEW_LOCKEDTARGET,
		VIEW_GHOST_GLOBAL,
		VIEW_GHOST_LOCAL,
		VIEW_FORMATION_EXTERNAL,
		VIEW_FORMATION_FROM_POINT,
		VIEW_FORMATION_TO_POINT,
		VIEW_CONSTANTROTATION,
		VIEW_CINEMATIC,
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

	enum FSZOOMMODE
	{
		ZOOM_STEPPED,
		ZOOM_SMOOTH,
		ZOOM_TOSIZE,
		ZOOM_CINEMATIC
	};

	class GroundCameraList
	{
	public:
		FsExistence* obj;
		YSBOOL active;
		YsVec3 pos;
		int searchKey;
		double zoomMult;

		void Add(FsExistence* ob, int key, double zoom)
		{
			obj = ob;
			active = obj->IsAlive();
			pos = obj->GetPosition();
			searchKey = key;
			zoomMult = zoom;
		}

		void Update()
		{
			active = obj->IsAlive();
			pos = obj->GetPosition();
		}
	};

	class TowerCameraList
	{
	public:
		YsVec3 pos;
		int refKey;
		double zoomMult;

		void Add(YsVec3 p, int key, double zoom)
		{
			pos = p;
			refKey = key;
			zoomMult = zoom;
		}
	};

	FSVIEWMODE viewMode;
	FSVIEWMODE nextViewMode;
	double viewHdg, viewPch;
	YsVec3 viewPoint;
	YsAtt3 viewAttitude;
	YsMatrix4x4 viewMat;
	FsProjection* projection;
	FsProjection* prevProjection;
	YSBOOL centerThisCamera;
	YSBOOL isViewPointInCloud;
	double fogVisibility;

	double zoomViewMode; //viewMagFix
	double prevZoomViewMode;
	//double zoomThisPort;
	//double prevZoomThisPort; //For subwindow-only view zoom, eg. targetting pod
	double offsetRadius; //viewTargetDist

	FsExistence* parentObject;
	FsExistence* lookAtObj;
	FsExistence* lookFromObj;
	YsVec3 lookAtPos;
	YsVec3 lookFromPos;
	int currentViewId;

	enum
	{
		NUM_SHADOW_MAP = 3
	};
	YsMatrix4x4 shadowProjMat[NUM_SHADOW_MAP];
	YsMatrix4x4 shadowViewMat[NUM_SHADOW_MAP];

	FsViewPort();
	void SetParentObject(FsExistence* obj);
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

	YsArray <GroundCameraList, 2, int> ilsList;
	YsArray <TowerCameraList> towerList;
	double ghostViewSpeed;
	double timeStep;
	//YsVec3 viewRefPoint;
	//YsVec3 towerViewPos;
	int towerViewId;
	YsVec3 ilsViewPos;
	int ilsViewId;
	//FsExistence* viewTargetAir;
	//FsExistence* viewSourceAir;
	//FsExistence* viewTargetGnd;
	//FsExistence* viewSourceGnd;
	//FsExistence* viewTargetObj;
	//FsExistence* viewSourceObj;
	//YsVec3 viewTargetPos, viewSourcePos;
	YsAtt3 viewOffsetAngle;
	YsVec3 viewOffsetPos;
	double viewOffsetRadius;
	//int currentViewKey;

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
	void SelectViewPort(int port);
	void PrepareViewPort(int port);
	void UpdateViewPort(FsViewPort& viewPort, FSVIEWMODE next);
	void DecideViewSourceAndTarget(FsViewPort& viewPort);
	void ApplyViewPortEnvironment(FsViewPort* viewPort, YsVec2i drawingAreaSize);
	void DecideViewMode(FsViewPort& viewPort,  FsAirplane* playerPlane);
	void AutoViewChange(FSVIEWMODE viewMode);

	void CalculateOnboardView(FsViewPort& viewPort, FsExistence* focus, int viewIdx, YsVec3 posOffset, YsAtt3 attOffset, YsBool3 locked);
	void CalculateFlybyView(FsViewPort& viewPort, double range, double resetRange);
	void CalculateExternalView(FsViewPort& viewPort, double radius, YsAtt3 neutral, YsAtt3 offset, YsBool3 local, YsBool3 locked, YSBOOL delay);
	void CalculateWeaponView(FsViewPort& viewPort, FsWeapon* wep, YSBOOL direction);
	void CalculateChaseView(FsViewPort& viewPort, FsExistence* lookAt);
	void CalculateObjectToObjectView(FsViewPort& viewPort);
	void CalculateCameraToObjectView(FsViewPort& viewPort);
	void CalculatePointToObjectView(FsViewPort& viewPort);
	void CalculateObjectToPointView(FsViewPort& viewPort);
	void CalculatePointToPointView(FsViewPort& viewPort, YsVec3 from, YsVec3 to);
	void CalculateConstantRotationView(FsViewPort& viewPort, YsAtt3 rot);
	void CalculateCinematicView();
	void CalculateGhostView(FsSimulation* sim, const double dt);

	void UpdateViewpointAccordingToPlayerAirplane(const double& distance, YSBOOL reset);

	void SelectNextView(FSBUTTONFUNCTION fnc, FSUSERCONTROL userControl);
	
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



	//For future update:
	//Decide camera subject, camera operator (object, entity, region, etc) (subject and operator can be the same)
	//Decide camera offset from operator and target offset from subject
	//Mark if the camera is internal to subject
	//Send position and velocities of subject and operator, as well as any subject/operator IDs to the camera function
	//Camera function remains completely agnostic to anything it's shooting

};

#endif
