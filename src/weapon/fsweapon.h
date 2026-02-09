#ifndef FSWEAPON_IS_INCLUDED
#define FSWEAPON_IS_INCLUDED
/* { */

////////////////////////////////////////////////////////////

#include "fsdef.h"
#include "fsrecord.h"
#include "fsinstreading.h"

class FsWeaponRecord
{
public:
	FSWEAPONTYPE type;
	float x,y,z,h,p,b;
	float velocity,lifeRemain;
	int power;
	class FsExistence *firedBy;
	FSWEAPON_CREDIT_OWNER creditOwner;

	float vMax,mobility,radar;
	class FsExistence *target;
};

class FsKillCredit
{
public:
	FsExistence *whoKilled,*whom;
	FSWEAPON_CREDIT_OWNER creditOwner;
	FSWEAPONTYPE byWhatKindOfWeapon;
	YsVec3 where;
	double when;

	int GetAntiAirScore(YsString &msg,const class FsSimulation *sim,const class FsExistence *player) const;
	int GetAntiGroundScore(YsString &msg,const class FsSimulation *sim,const class FsExistence *player) const;
};


////////////////////////////////////////////////////////////


class FsWeaponSmokeTrail
{
public:
	enum
	{
		TIMEPERSEG=100, // ms
		MAXNUMTRAIL=64
	};

	int trailBegin,trailUsed;
	double trailNextRecord;
	YsVec3 trailPos[MAXNUMTRAIL];
	YsAtt3 trailAtt[MAXNUMTRAIL];
	double trailTime[MAXNUMTRAIL];

	YSBOOL used;

	void Initialize(void);

	void MakeVertexArray(
	    class YsGLVertexBuffer vtxBuf[4],class YsGLNormalBuffer nomBuf[4],class YsGLColorBuffer colBuf[4],
	    const YsVec3 &pos,const YsAtt3 &att,FSSMOKETYPE smk,const double &cTime) const;

	void Draw(const YsVec3 &pos,const YsAtt3 &att,YSBOOL transparency,FSSMOKETYPE smk,const double &cTime) const;
	void Add(const double &dt,const double &cTime,const YsVec3 &pos,const YsAtt3 &att);

	void AddToParticleManagerAsFlare(class YsGLParticleManager &partMan,const YsVec3 cPos,const double cTime,YSBOOL includeCurrentPos, FSENVIRONMENT env);
	void AddToParticleManager(class YsGLParticleManager &partMan,const YsVec3 cPos,const double cTime,YSBOOL includeCurrentPos, FSENVIRONMENT env);
};



// Declaration /////////////////////////////////////////////
class FsWeapon
{
public:
	FsWeapon *prev,*next;
	FsWeapon *prevFlare,*nextFlare;
	int bufPos; //Weapon index in buf

	FSWEAPONTYPE type;
	FSWEAPONCATEGORY category;
	FSENTITYLIST inList;
	YsString identify;

	double lifeRemain;   // Life remaining by distance
	double lifeTime;	 // Life remaining by time
	double timeRemain;   // Time remaining after impact (to draw smoke better)
	double timeUnguided; // Duration of unguided flying  2005/02/18 Set in FsWeapon::Fire

	double velocity;
	double drag;
	double weight;
	double range;
	YSBOOL applyGravity;
	double launchSpeed;
	double maxVelocity;
	double accel; //acceleration below maxVelocity
	double decel; //deceleration above maxVelocity

	YsVec3 prv,pos,vec;
	YsVec3 velVec, accelVec;
	YsAtt3 att;
	YsVec3 lastChecked;
	class FsExistence *firedBy;
	FSWEAPON_CREDIT_OWNER creditOwner;

	double mobility,radar; //turn rate, tracking half angle
	double fuseRadius;
	YSBOOL impactFuse;
	int destructivePower;

	class FsExistence *target;
	YSBOOL targetAir;
	YSBOOL targetGnd;
	YSBOOL canRetarget;
	YSBOOL followDecoy;

	FsWeaponSmokeTrail *trail;
	YSBOOL trailSmoke;
	YSBOOL shouldJettison;
	YSBOOL canJettison;
	YSBOOL faceForward; //point this weapon in the direction of travel
	YSBOOL isFuelTank;
	YSBOOL isDecoy;
	


	// ***s (eg. aim9s, agm65s) stands for silent.
	static class FsVisualDnm aim9,aim9s,aim_coarse;
	static class FsVisualDnm aim9x,aim9xs,aim9x_coarse;
	static class FsVisualDnm agm65,agm65s,agm_coarse;
	static class FsVisualDnm bomb,bomb_coarse;
	static class FsVisualDnm rocket,rockets,rocket_coarse;

	static class FsVisualDnm aim120,aim120s,aim120_coarse;  // 2004/01/22
	static class FsVisualDnm bomb250,bomb250s,bomb250_coarse;  // 2004/01/22
	static class FsVisualDnm bomb500hd,bomb500hds,bomb500hd_coarse;  // 2004/01/22

	static class FsVisualDnm flarePod, flare, flare_coarse;
	static class FsVisualDnm fuelTank;

	static FsAmmunitionIndication::WEAPONTYPE WeaponTypeToWeaponIndicationType(FSWEAPONTYPE wpnType);

	static void DrawVisual
	   (FSWEAPONTYPE type,YSBOOL coarse,
	   const YsMatrix4x4 &viewTfm,const YsMatrix4x4 &projTfm,
	   const YsVec3 &pos,const YsAtt3 &att,unsigned int drawFlag);
	static void DrawShadow
	   (FSWEAPONTYPE type,YSBOOL coarse,
	   const YsMatrix4x4 &viewTfm,const YsMatrix4x4 &projTfm,
	   const YsVec3 &pos,const YsAtt3 &att,const YsMatrix4x4 &projPlnTfm);

	class FsWeaponPerformance { //20260121 code cleaning update
	public:
		FSWEAPONTYPE type; //weapon type (GUN AIM9 AIM9X AIM120 AGM RKT B250 B500 B500HD FUEL FLR)
		FSWEAPONCATEGORY category;
		FSENTITYLIST inList;
		YsString identify;
		double launchSpeed; //launch speed relative to aircraft (to fix guns)
		double maxSpeed; //max speed (powered)
		double flightRange; //range
		double flightTime;
		int power; //damage power
		YSBOOL targetAir; //aam
		YSBOOL targetGnd; //agm
		YSBOOL canRetarget; //a-aam loopback
		double trackRange; //how far away does lock symbology show?
		double trackDelay; //post-launch lock on delay
		double trackAngle; //seeker cone half-angle
		double turnRate; //turn rate
		double fuseRadius; //proximity fuse
		YSBOOL impactFuse;
		YSBOOL trailSmoke; //smoke trail
		double weight; //weight
		YSBOOL fueltank; //this weaponn contais fuel
		YSBOOL isDecoy; //flares etc
		YSBOOL gravity; //affected by gravity
		YSBOOL faceForward; //points in direction of travel or no
		double flyingDrag; //deployed cD
		double accel; //powered acceleration rate
		double decel; //deceleration above maxSpeed
		YSBOOL followDecoy; //decoyed by flare
		YSBOOL canJettison; //can jett
	};
						//	    {    type,               category,                     list,                ident,    vint, vmax, range, time, power, air,   gnd,  retarget, lock, delay, angle, rate, proxfuse, impact, smoke,  weight, fuel,    decoy,   grav,   forward,  cd,  accel, decel, trackFlr, canJett
	FsWeaponPerformance gunPerf{     FSWEAPON_GUN,       FSWEAPONCAT_BULLET,           FSENTITYLIST_BULLET, "GUN",     1700, 1700, 3000,  1.76, 1,  YSFALSE, YSFALSE, YSFALSE, 3000,  0.0, 0.0,   0.0,   0.0,  YSFALSE, YSFALSE, 0.0,   YSFALSE, YSFALSE, YSTRUE,  YSTRUE,  0.0,   0.0,  20.0, YSFALSE, YSFALSE};
	FsWeaponPerformance aim9Perf{    FSWEAPON_AIM9,      FSWEAPONCAT_ANTIAIRMISSILE,   FSENTITYLIST_WEAPON, "AIM9",    0,    1020, 5000,  4.9,  12, YSTRUE,  YSFALSE, YSFALSE, 5000,  0.0, 0.524, 1.571, 18.0, YSFALSE, YSTRUE,  90.0,  YSFALSE, YSFALSE, YSFALSE, YSTRUE,  0.0,   50.0, 20.0, YSTRUE,  YSTRUE};
	FsWeaponPerformance aim9xPerf{   FSWEAPON_AIM9X,     FSWEAPONCAT_ANTIAIRMISSILE,   FSENTITYLIST_WEAPON, "AIM9X",   0,    1020, 5000,  4.9,  12, YSTRUE,  YSFALSE, YSTRUE,  5000,  0.5, 0.524, 1.396, 23.0, YSFALSE, YSTRUE,  90.0,  YSFALSE, YSFALSE, YSFALSE, YSTRUE,  0.0,   50.0, 20.0, YSTRUE,  YSTRUE};
	FsWeaponPerformance aim120Perf{  FSWEAPON_AIM120,    FSWEAPONCAT_ANTIAIRMISSILE,   FSENTITYLIST_WEAPON, "AIM120",  0,    1360, 30000, 22.0, 12, YSTRUE,  YSFALSE, YSTRUE,  30000, 3.0, 0.524, 0.96,  25.0, YSFALSE, YSTRUE,  150.0, YSFALSE, YSFALSE, YSFALSE, YSTRUE,  0.0,   50.0, 20.0, YSTRUE,  YSTRUE};
	FsWeaponPerformance agm65Perf{   FSWEAPON_AGM65,     FSWEAPONCAT_ANTIGROUNDMISSILE,FSENTITYLIST_WEAPON, "AGM65",   0,    340,  5000,  14.7, 12, YSFALSE, YSTRUE,  YSFALSE, 99000, 1.6, 0.349, 1.571, 0.0,  YSFALSE, YSTRUE,  300.0, YSFALSE, YSFALSE, YSFALSE, YSTRUE,  0.0,   50.0, 20.0, YSFALSE, YSTRUE};
	FsWeaponPerformance rocketPerf{  FSWEAPON_ROCKET,    FSWEAPONCAT_UNGUIDEDMISSILE,  FSENTITYLIST_WEAPON, "ROCKET",  0,    800,  10000, 12.5, 10, YSFALSE, YSFALSE, YSFALSE, 0.0,   0.0, 0.0,   0.0,   0.0,  YSTRUE,  YSFALSE, 10.0,  YSFALSE, YSFALSE, YSFALSE, YSTRUE,  0.0,   50.0, 20.0, YSFALSE, YSTRUE};
	FsWeaponPerformance b250Perf{    FSWEAPON_BOMB250,   FSWEAPONCAT_FREEFALL,         FSENTITYLIST_WEAPON, "B250",    0,    340,  99999, 999,  35, YSFALSE, YSFALSE, YSFALSE, 0.0,   0.0, 0.0,   0.0,   0.0,  YSTRUE,  YSFALSE, 120.0, YSFALSE, YSFALSE, YSTRUE,  YSTRUE,  0.0,   0.0,  0.0,  YSFALSE, YSTRUE};
	FsWeaponPerformance b500Perf{    FSWEAPON_BOMB,      FSWEAPONCAT_FREEFALL,         FSENTITYLIST_WEAPON, "B500",    0,    340,  99999, 999,  50, YSFALSE, YSFALSE, YSFALSE, 0.0,   0.0, 0.0,   0.0,   0.0,  YSTRUE,  YSFALSE, 250.0, YSFALSE, YSFALSE, YSTRUE,  YSTRUE,  0.0,   0.0,  0.0,  YSFALSE, YSTRUE};
	FsWeaponPerformance b500hdPerf{  FSWEAPON_BOMB500HD, FSWEAPONCAT_FREEFALL,         FSENTITYLIST_WEAPON, "B500HD",  0,    340,  99999, 999,  35, YSFALSE, YSFALSE, YSFALSE, 0.0,   0.0, 0.0,   0.0,   0.0,  YSTRUE,  YSFALSE, 250.0, YSFALSE, YSFALSE, YSTRUE,  YSTRUE,  0.8,   0.0,  0.0,  YSFALSE, YSTRUE};
	FsWeaponPerformance flarePerf{   FSWEAPON_FLARE,     FSWEAPONCAT_COUNTERMEASURE,   FSENTITYLIST_DECOY,  "FLARE",   0,    120,  1000,  10.0, 0,  YSFALSE, YSFALSE, YSFALSE, 0.0,   0.0, 0.0,   0.0,   0.0,  YSFALSE, YSTRUE,  1.0,   YSFALSE, YSTRUE,  YSTRUE,  YSTRUE,  0.002, 0.0,  0.0,  YSFALSE, YSFALSE};
	FsWeaponPerformance flarepodPerf{FSWEAPON_FLAREPOD,  FSWEAPONCAT_UTILITY,          FSENTITYLIST_WEAPON, "FLAREPOD",0,    340,  99999, 999,  0,  YSFALSE, YSFALSE, YSFALSE, 0.0,   0.0, 0.0,   0.0,   0.0,  YSFALSE, YSFALSE, 150.0, YSFALSE, YSFALSE, YSTRUE,  YSTRUE,  0.0,   0.0,  0.0,  YSFALSE, YSTRUE};
	FsWeaponPerformance fueltankPerf{FSWEAPON_FUELTANK,  FSWEAPONCAT_UTILITY,          FSENTITYLIST_WEAPON, "FUELTANK",0,    340,  99999, 999,  1,  YSFALSE, YSFALSE, YSFALSE, 0.0,   0.0, 0.0,   0.0,   0.0,  YSTRUE,  YSFALSE, 150.0, YSTRUE,  YSFALSE, YSTRUE,  YSTRUE,  0.0,   0.0,  0.0,  YSFALSE, YSTRUE};
	FsWeaponPerformance smokePerf{   FSWEAPON_SMOKE,     FSWEAPONCAT_SYSTEM,           FSENTITYLIST_SYSTEM, "SMOKE",   0,    1,    99999, 999,  0,  YSFALSE, YSFALSE, YSFALSE, 0.0,   0.0, 0.0,   0.0,   0.0,  YSFALSE, YSTRUE,  0.0,   YSFALSE, YSFALSE, YSFALSE, YSFALSE, 0.0,   0.0,  0.0,  YSFALSE, YSFALSE};
	FsWeaponPerformance debrisPerf{  FSWEAPON_DEBRIS,    FSWEAPONCAT_SYSTEM,           FSENTITYLIST_SYSTEM, "DEBRIS",  0,    1,    60.0,  999,  0,  YSFALSE, YSFALSE, YSFALSE, 0.0,   0.0, 0.0,   0.0,   0.0,  YSFALSE, YSFALSE, 0.0,   YSFALSE, YSFALSE, YSFALSE, YSFALSE, 0.0,   0.0,  0.0,  YSFALSE, YSFALSE };
	FsWeaponPerformance nullWepPerf{ FSWEAPON_NULL,      FSWEAPONCAT_SYSTEM,           FSENTITYLIST_SYSTEM, "NULL",    0,    1,    99999, 999,  0,  YSFALSE, YSFALSE, YSFALSE, 0.0,   0.0, 0.0,   0.0,   0.0,  YSFALSE, YSFALSE, 0.0,   YSFALSE, YSFALSE, YSFALSE, YSFALSE, 0.0,   0.0,  0.0,  YSFALSE, YSFALSE};

	FsWeapon();
	
	FsWeaponPerformance GetWeaponPerformanceByType(FSWEAPONTYPE &wep);

	void Reset(void);
	void LaunchWeapon(
		FsWeaponPerformance &data,
		const YsVec3 &cpos,
		const YsAtt3 & catt,
		const double &vInit,
		class FsExistence *owner,
		FSWEAPON_CREDIT_OWNER creditOwnerIn,
		class FsExistence *target,
		FsWeaponSmokeTrail *trail); //20260121 code cleaning update

	// Debris
	void ThrowDebris(const double &ctime,const YsVec3 &pos,const YsVec3 &vec,const double &l);


	void Move(const double &dt,const double &cTime,const class FsWeather &weather,const FsWeapon *flareList);
	YSBOOL Bounce(YsVec3 &normal, YsVec3 &intersect, YsVec3 &outVel, YsVec3 &posOffset, double maxAngle, double energyReturn);
	YSBOOL IsOwnerStillHaveTarget(void);
	void HitGround
	    (class FsWeaponHolder *callback,
	     const double &ctime,
	     const class FsField &fld,
	     class FsExplosionHolder *explosion,
	     class FsSimulation *sim,
	     YsList <FsKillCredit> *&killCredit);
	YSBOOL HitObject
	    (class FsWeaponHolder *callback,
	     const double &ctime,
	     class FsExistence &obj,
	     class FsExplosionHolder *explosion,
	     class FsSimulation *sim,
	     YsList <FsKillCredit> *&killCredit);
	void ExplodeBomb
	    (class FsWeaponHolder *callback,
	     const double &ctime,
	     const YsVec3 &pos,
	     const double &rad,
	     class FsExplosionHolder *explosion,
	     class FsSimulation *sim,
	     YsList <FsKillCredit> *&killCredit);
	void ExplodeBombInWater
	    (class FsWeaponHolder *callback,
	     const double &ctime,
	     const YsVec3 &pos,
	     const double &rad,
	     class FsExplosionHolder *explosion,
	     class FsSimulation *sim,
	     YsList <FsKillCredit> *&killCredit);
	void Draw(
	    YSBOOL coarse,const YsMatrix4x4 &viewMat,const YsMatrix4x4 &projMat,
	    YSBOOL transparency,FSSMOKETYPE smk,const double &cTime,unsigned int drawFlag) const;

	void AddToParticleManager(class YsGLParticleManager &partMan,const double cTime, FSENVIRONMENT env) const;

protected:
	YSRESULT AddKillCredit(YsList <FsKillCredit> *&killCredit,FsExistence *whoIsKilled,const double &when) const;
};



// Declaration /////////////////////////////////////////////
class FsWeaponHolder
{
protected:
	FsSimulation *sim;
	YsArray <YsVec3,64> bulletCalibrator;

public:
	FsWeaponHolder(FsSimulation *simPtr);
	~FsWeaponHolder();

	static YSRESULT LoadMissilePattern(void);
	static void FreeMissilePattern(void);

	enum
	{
		NumBulletBuffer=32768,
		NumSmokeTrailBuffer=32
	};

	YSRESULT Save(FILE *fp,class FsSimulation *sim,int wpnPosPrecision,int wpnAttPrecision);
	YSRESULT Load(FILE *fp,class FsSimulation *sim);

	void Clear(void);
	void MoveToActiveList(FsWeapon *wep);
	void MoveToWeaponList(FsWeapon *wep);
	void MoveToBulletList(FsWeapon *wep);
	void MoveToDecoyList(FsWeapon *wep);
	void MoveToSystemList(FsWeapon *wep);
	void MoveToFreeList(FsWeapon *wep);

	void ClearBulletCalibrator(void);
	void CalculateBulletCalibrator(const FsExistence *target);


	YSBOOL GiveDamage(YSBOOL &killed,FsExistence &obj,int power,enum FSDIEDOF diedOf,FsWeapon &wpn);

	YSRESULT RipOffEarlyPartOfRecord(void);

	YSRESULT DeleteRecordForResumeFlight(class FsAirplane *shotBy,const double &startTime);

	YSRESULT RefreshOrdinanceByWeaponRecord(const double &currentTime);

	int LaunchWeapon(
		FsWeapon::FsWeaponPerformance &data,
		const double &ctime,
		YsVec3 &pos,
		YsAtt3 &att,
		const YsVec3 &iniVelocity,
		class FsExistence *owner,
		unsigned int airTarget,
		unsigned int gndTarget,
		YSBOOL recordIt,
		YSBOOL transmit);

	// Firing Gun
	int Fire
	    (const double &ctime,
	     YsVec3 &pos,
	     YsAtt3 &att,
	     double v,
	     double l,
	     int destructivePower,
	     class FsExistence *owner,
	     YSBOOL recordIt,YSBOOL transmit);

	// Firing Missile
	int Fire
	    (const double &ctime,
	     FSWEAPONTYPE missileType,
	     YsVec3 &pos,YsAtt3 &att,
	     double v,double vmax,double l,double mobility,double radar,
	     int destructivePower,
	     class FsExistence *owner,unsigned int targetKey,
	     YSBOOL recordIt,YSBOOL transmit, YSBOOL jettison=YSFALSE);

	// Bomb away!
	int Bomb
	   (const double &ctime,
	    FSWEAPONTYPE bombType,
	    const YsVec3 &pos,const YsAtt3 &att,const YsVec3 &iniVelocity,
	    const double &vMax,
	    int destructivePower,
	    class FsExistence *owner,
	    YSBOOL recordIt,YSBOOL transmit);

	// Dispense Flare
	int DispenseFlare
	   (const double &ctime,
	    const YsVec3 &pos,const YsVec3 &vel,const double &vMax,const double &l,
	    class FsExistence *owner,YSBOOL recordIt,YSBOOL transmit);

	// Debris
	int ThrowDebris(const double &ctime,const YsVec3 &pos,const YsVec3 &vec,const double &l);
	void ThrowRandomDebris(const double &ctime,const YsVec3 &pos,const YsAtt3 &att,const double &l);
	void ThrowMultiDebris(int n,const double &ctime,const YsVec3 &pos,const YsAtt3 &att,const double &l);

	YSBOOL IsLockedOn(const FsExistence *ex) const;
	YSBOOL IsLockedOn(FSWEAPONTYPE &wpnType,YsVec3 &wpnPos,const FsExistence *ex) const;
	FsWeapon* GetLockedOn(const FsExistence* ex) const;

	YSRESULT FindFirstMissilePositionThatIsReallyGuided(YsVec3 &vec,YsAtt3 &att) const;
	YSRESULT FindOldestMissilePosition(YsVec3 &vec,YsAtt3 &att,const FsExistence *fired) const;
	YSRESULT FindNewestMissilePosition(YsVec3 &vec,YsAtt3 &att,const FsExistence *fired) const;

	FsWeapon *FindNextActiveEntity(const FsWeapon *wpn) const;
	FsWeapon *FindNextActiveWeapon(const FsWeapon *wpn) const;
	FsWeapon *FindNextActiveDecoy(const FsWeapon *wpn) const;
	FsWeapon *FindNextActiveBullet(const FsWeapon *wpn) const;
	FsWeapon *FindNextActiveSystem(const FsWeapon *wpn) const;
	const FsWeapon *GetWeapon(int id) const;
	void ObjectIsDeleted(FsExistence *obj) const;

	void Move(const double &dt,const double &cTime,const class FsWeather &weather);
	void HitSomething(const double &ctime, const class FsField &field, class FsExplosionHolder *exp, class FsSimulation *sim, const double &tallestGroundObjectHeight);
	void HitGround(
	    const double &ctime,const class FsField &field,class FsExplosionHolder *xp,class FsSimulation *sim);
	void HitObject(
	    const double &ctime,FsExplosionHolder *explo,class FsSimulation *sim,const double &tallestGroundObjectHeight);

	void AddToParticleManager(class YsGLParticleManager &partMan,const double cTime) const;

	void BeginDraw(void) const;
	void Draw(
	    YSBOOL coarse,const YsMatrix4x4 &viewMat,const YsMatrix4x4 &projMat,
	    YSBOOL transparency,FSSMOKETYPE smk,const double &cTime,unsigned int drawFlag) const;
	void EndDraw(void) const;

	void DrawGunCalibrator(void) const;

	void CollectRecord(void);
	void PlayRecord(const double &t1,const double &t2);
	void LaunchRecord(const FsWeaponRecord &rec,const double &t,YSBOOL recordIt,YSBOOL transmit);

	FsWeapon buf[NumBulletBuffer];
	FsWeaponSmokeTrail trl[NumSmokeTrailBuffer];

	FsWeapon *weaponList, *bulletList, *decoyList, *systemList, *freeList;

	FsRecord <FsWeaponRecord> *toPlay;
	FsRecord <FsWeaponRecord> *toSave;

	YsList <FsKillCredit> *killCredit;

protected:
	FsExistence *FindObjectByAxxGxxN(const char identifier[],const class FsSimulation *sim);


// For network play
protected:
	class FsSocketServer *netServer;
	class FsSocketClient *netClient;
public:
	void SetNetServer(class FsSocketServer *svr);
	void SetNetClient(class FsSocketClient *cli);
};


unsigned FsEncodeWeaponRecord(unsigned char dat[],FsSimulation *sim,const FsWeaponRecord &rec);
YSRESULT FsDecodeWeaponRecord
    (FsWeaponRecord &rec,
     int &firedBy,YSBOOL &firedByAirplane,int &firedAt,YSBOOL &firedAtAirplane,
     unsigned char dat[],FsSimulation *sim);


int FsGetDefaultWeaponLoadingUnit(FSWEAPONTYPE wpnType);
int FsGetDefaultSubUnitPerLoadingUnit(FSWEAPONTYPE wpnType);
FSWEAPONTYPE FsGetWeaponTypeByString(const char str[]);
const char *FsGetWeaponString(FSWEAPONTYPE wpnType);

/* } */
#endif
