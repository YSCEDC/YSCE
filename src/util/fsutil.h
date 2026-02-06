#ifndef FSUTIL_IS_INCLUDED
#define FSUTIL_IS_INCLUDED
/* { */

#include "fsdef.h"

YSBOOL FsTestKeyWordMatch(const YsString &str,YSSIZE_T ac,const YsString av[]);

double FsNumerical1stDifferentiation(const double &t1,const double &v1,const double &t2,const double &v2,const double &t3,const double &v3,const double &t);
double FsNumerical2ndDifferentiation(const double &t1,const double &v1,const double &t2,const double &v2,const double &t3,const double &v3,const double &t);

int FsGetRandomBetween(int min, int max);
double FsGetRandomBetween(double min, double max);

double FsGetLevelTurnRadius(const double v,const double bank);


void FsCompressNumberInString(char str[]);


const char *FsTrueFalseString(YSBOOL tf);


void FsGetTodayFileString(YsString &str);
void FsGetTodayTimeString(YsString &str);


class FsAutoCloseFile
{
public:
	FsAutoCloseFile();
	~FsAutoCloseFile();

	FILE *Fp();
	FILE *Fopen(const wchar_t fn[],const char mode[]);
	void Fclose(void);

protected:
	FILE *filePtr;
};


class FsInternationalText
{
private:
	YsString languageCode;
	YsWString msg;
public:
	void CleanUp(void);
	void SetText(const char lang[],const wchar_t msg[]);
	YSBOOL MatchLanguage(const char lang[]) const;
	const char *GetLanguageCode(void) const;
	const wchar_t *GetText(void) const;
};


unsigned int FsGetDate(void);

const char *FsRegionIdToString(int id);

YSRESULT FsGetJoulePerSecond(double &dat,const char in[]);
YSRESULT FsGetLength(double &dat,const char in[]);
YSRESULT FsGetArea(double &dat,const char in[]);
YSRESULT FsGetWeight(double &dat,const char in[]);
YSRESULT FsGetForce(double &dat,const char in[]);
YSRESULT FsGetSpeed(double &dat,const char in[]);
YSRESULT FsGetAngle(double &dat,const char in[]);
YSRESULT FsGetBool(YSBOOL &dat,const char in[]);
YSRESULT FsGetNonDimensional(double &dat,const char in[]);
YSRESULT FsGetVec3(YsVec3 &vec,YSSIZE_T ac,const char * const av[]);
YSRESULT FsGetVec3(YsVec3 &vec,YSSIZE_T ac,const YsString av[]);
YSRESULT FsGetAtt3(YsAtt3 &att,YSSIZE_T ac,const char * const av[]);
YSRESULT FsGetAtt3(YsAtt3 &att,YSSIZE_T ac,const YsString av[]);
YSRESULT FsGetWeaponOfChoice(FSWEAPONTYPE &woc,const char str[]);
YSRESULT FsGetJoulePerSecond(double &power,const char in[]);

class StockCheck
{
private:
public:
	StockCheck();
	int nSojiAir = 87; //zero indexed
	int nSojiGnd = 107;
	int nSojiSce = 17;
	int nYsceAir = 4;
	int nYsceGnd = 4;
	int nYsceSce = 2;
	static const char *const sojiAirList[];
	static const char *const sojiGndList[];
	static const char *const sojiSceList[];
	static const char *const ysceAirList[];
	static const char *const ysceGndList[];
	static const char *const ysceSceList[];
	YSBOOL FsCheckIfSojiStock(const char in[]);
	int FsCheckIfSojiAir(const char in[]);
	int FsCheckIfSojiGnd(const char in[]);
	int FsCheckIfSojiSce(const char in[]);
	YSBOOL FsAddTagIfSojiAir(const char in[], YsString &out);
	YSBOOL FsAddTagIfSojiGnd(const char in[], YsString &out);
	YSBOOL FsAddTagIfSojiSce(const char in[], YsString &out);
	YSBOOL FsRemoveTagIfSojiAir(const char in[], YsString &out);
	YSBOOL FsRemoveTagIfSojiGnd(const char in[], YsString &out);
	YSBOOL FsRemoveTagIfSojiSce(const char in[], YsString &out);
	int FsFindNextStockAir(int idx, YsString &out);
	int FsFindNextStockGnd(int idx, YsString &out);
};
/* } */
#endif
