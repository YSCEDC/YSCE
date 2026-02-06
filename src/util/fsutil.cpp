#include <stdio.h>
#include <time.h>
#include <string.h>
#include <ysclass.h>
#include <ysport.h>
#include <fsdef.h>
#include "fsutil.h"

////////////////////////////////////////////////////////////

// Numerical Differentiation

// Lagrange Polynomials
//  L0=  (x*x-(x1+x2)*x+x1*x2)/((x0-x1)*(x0-x2)
//  L0'= (2*x-(x1+x2))/((x0-x1)*(x0-x2))
//  L0''= 2/((x0-x1)*(x0-x2))

//  L1=  (x*x-(x0+x2)*x+x0*x2)/((x1-x0)*(x1-x2))
//  L1'= (2*x-(x0+x2))/((x1-x0)*(x1-x2))
//  L1''= 2/((x1-x0)*(x1-x2))

//  L2=  (x*x-(x0+x1)*x+x0*x1)/((x2-x0)*(x2-x1))
//  L2'= (2*x-(x0+x1))/((x2-x0)*(x2-x1))
//  L2''= 2/((x2-x0)*(x2-x1));

double FsNumerical1stDifferentiation
    (const double &x0,const double &y0,
     const double &x1,const double &y1,
     const double &x2,const double &y2,
     const double &x)
{
	return y0*(2.0*x-(x1+x2))/((x0-x1)*(x0-x2))
	      +y1*(2.0*x-(x0+x2))/((x1-x0)*(x1-x2))
	      +y2*(2.0*x-(x0+x1))/((x2-x0)*(x2-x1));
}


double FsNumerical2ndDifferentiation
    (const double &x0,const double &y0,
     const double &x1,const double &y1,
     const double &x2,const double &y2,
     const double &/*x*/)
{
	return y0*2.0/((x0-x1)*(x0-x2))
	      +y1*2.0/((x1-x0)*(x1-x2))
	      +y2*2.0/((x2-x0)*(x2-x1));
}

void FsCompressNumberInString(char str[])
{



	YSSIZE_T n=(int)strlen(str);
	auto lastSeparator=n;
	for(auto i=n; i>=0; i--)
	{
		if(str[i]==0 || str[i]==' ' || str[i]=='\t' || str[i]=='\n' ||
		   str[i]==',' || str[i]=='e' || str[i]=='E')
		{
			lastSeparator=i;
		}
		else if(str[i]=='.')
		{
			// Here between i+1 to lastSeparator-1 are below-decimal digits.
			decltype(n) j,firstNonZero,nDel;

			firstNonZero=i;
			for(j=lastSeparator-1; j>i; j--)
			{
				if(str[j]!='0')
				{
					firstNonZero=j;
					break;
				}
			}
			if(firstNonZero==i)
			{
				nDel=lastSeparator-(i+2);  //  0.000 -> 0.0
			}
			else
			{
				nDel=lastSeparator-(firstNonZero+1);
			}

			if(nDel>0)
			{
				for(j=lastSeparator; str[j]!=0; j++)
				{
					str[j-nDel]=str[j];
				}
				str[j-nDel]=0;
			}

			lastSeparator=i;
		}
	}
}


YSBOOL FsTestKeyWordMatch(const YsString &str,YSSIZE_T ac,const YsString av[])
{
	if(ac==0)
	{
		return YSTRUE;
	}

	for(YSSIZE_T i=0; i<ac; i++)
	{
		YSBOOL match;
		match=YSFALSE;
		for(YSSIZE_T j=0; j<=str.Strlen()-av[i].Strlen(); j++)
		{
			if(strncmp(str+j,av[i],av[i].Strlen())==0)
			{
				match=YSTRUE;
				break;
			}
		}
		if(match!=YSTRUE)
		{
			return YSFALSE;
		}
	}
	return YSTRUE;
}


const char *FsTrueFalseString(YSBOOL tf)
{
	switch(tf)
	{
	case YSTRUE:
		return "TRUE";
	default:
		return "FALSE";
	}
}


void FsGetTodayFileString(YsString &str)
{
	struct tm *tmStr;
	time_t tm;
	char buf[256];

	tm=time(NULL);
	tmStr=localtime(&tm);
	sprintf(buf,"%04d%02d%02d",tmStr->tm_year+1900,tmStr->tm_mon+1,tmStr->tm_mday);

	str.Set(buf);
}

void FsGetTodayTimeString(YsString &str)
{
	time_t tm;
	tm=time(NULL);
	str.Set(ctime(&tm));
}



FsAutoCloseFile::FsAutoCloseFile()
{
	filePtr=NULL;
}

FsAutoCloseFile::~FsAutoCloseFile()
{
	if(filePtr!=NULL)
	{
		fclose(filePtr);
	}
}


FILE *FsAutoCloseFile::Fp()
{
	return filePtr;
}

FILE *FsAutoCloseFile::Fopen(const wchar_t fn[],const char mode[])
{
	if(filePtr==NULL)
	{
		filePtr=YsFileIO::Fopen(fn,mode);
		return filePtr;
	}
	else
	{
		return NULL;
	}
}

void FsAutoCloseFile::Fclose(void)
{
	if(filePtr!=NULL)
	{
		fclose(filePtr);
		filePtr=NULL;
	}
}
////////////////////////////////////////////////////////////

void FsInternationalText::CleanUp(void)
{
	this->languageCode="";
	this->msg=L"";
}
void FsInternationalText::SetText(const char lang[],const wchar_t msg[])
{
	this->languageCode=lang;
	this->msg=msg;
}
YSBOOL FsInternationalText::MatchLanguage(const char lang[]) const
{
	if(0==strcmp(lang,languageCode))
	{
		return YSTRUE;
	}
	return YSFALSE;
}
const char *FsInternationalText::GetLanguageCode(void) const
{
	return languageCode;
}
const wchar_t *FsInternationalText::GetText(void) const
{
	return msg;
}

////////////////////////////////////////////////////////////

unsigned int FsGetDate(void)
{
	time_t t0;
	t0=time(NULL);

	struct tm *t;
	t=localtime(&t0);

	unsigned int d=(t->tm_year+1900)*10000 +(t->tm_mon+1)*100+ t->tm_mday;
	printf("Date %d\n",d);

	return d;
}

////////////////////////////////////////////////////////////

const char *FsRegionIdToString(int id)
{
	switch(id)
	{
	default:
		return "Unknown Region Type";
	case FS_RGNID_RUNWAY:
		return "FS_RGNID_RUNWAY";
	case FS_RGNID_TAXIWAY:
		return "FS_RGNID_TAXIWAY";
	case FS_RGNID_AIRPORT_AREA:
		return "FS_RGNID_AIRPORT_AREA";
	case FS_RGNID_ENEMY_TANK_GENERATOR:
		return "FS_RGNID_ENEMY_TANK_GENERATOR";
	case FS_RGNID_FRIENDLY_TANK_GENERATOR:
		return "FS_RGNID_FRIENDLY_TANK_GENERATOR";
	case FS_RGNID_VIEWPOINT:
		return "FS_RGNID_VIEWPOINT";
	}
}

////////////////////////////////////////////////////////////

double FsGetLevelTurnRadius(const double v,const double bank)
{
	return (v*v)/(FsGravityConst*tan(bank));
}

////////////////////////////////////////////////////////////

static const char *const FsLengthUnit[]=
{
	"IN","FT","KM","CM","M","SM","NM",NULL
};

static const char *const FsAreaUnit[]=
{
	"IN^2","M^2",NULL
};

static const char *const FsWeightUnit[]=
{
	"KG","LB","T","N",NULL
};

static const char *const FsSpeedUnit[]=
{
	"MACH","KM/H","M/S","KT",NULL
};

static const char *const FsAngleUnit[]=
{
	"RAD","DEG",NULL
};
static const char *const FsPowerUnit[]=
{
	"HP","J/S",NULL
};

static int FsGetUnit(const char in[],const char *const unitLst[])
{
	int i;
	char str[256];
	strncpy(str,in,255);
	str[255]=0;

	YsCapitalize(str);
	for(i=0; unitLst[i]!=NULL; i++)
	{

		int ls=(int)strlen(str);
		int lu=(int)strlen(unitLst[i]);
		if(lu<ls && strcmp(str+ls-lu,unitLst[i])==0 &&
		  '0'<=str[ls-lu-1] && str[ls-lu-1]<='9')
		{
			return i;
		}
	}

	static YSBOOL first=YSTRUE;
	if(YSTRUE==first)
	{
		printf("Unkown or Inproper Unit %s\n",in);
		first=YSFALSE;
	}
	return -1;
}

YSRESULT FsGetJoulePerSecond(double &dat,const char in[])
{
	int uni;
	uni=FsGetUnit(in,FsPowerUnit);
	if(uni>=0)
	{
		switch(uni)
		{
		case 0://"HP"
			dat=atof(in)*740.0;
			break;
		case 1://"J/S"
			dat=atof(in);
			break;
		}
		return YSOK;
	}
	else
	{
		return YSERR;
	}
}

YSRESULT FsGetLength(double &dat,const char in[])
{
	int uni;
	uni=FsGetUnit(in,FsLengthUnit);
	if(uni>=0)
	{
		switch(uni)
		{
		case 0://"IN"
			dat=atof(in)*0.0254;
			break;
		case 1://"FT"
			dat=atof(in)*0.3048;
			break;
		case 2://"KM"
			dat=atof(in)*1000.0;
			break;
		case 3://"CM"
			dat=atof(in)/100.0;
			break;
		case 4://"M"
			dat=atof(in);
			break;
		case 5: // "SM"
			dat=atof(in)*1609.0;
			break;
		case 6: // "NM"
			dat=atof(in)*1852.0;
			break;
		}
		return YSOK;
	}
	else
	{
		return YSERR;
	}
}

YSRESULT FsGetArea(double &dat,const char in[])
{
	int uni;
	uni=FsGetUnit(in,FsAreaUnit);
	if(uni>=0)
	{
		switch(uni)
		{
		case 0://"IN^2"
			dat=atof(in)*0.000645;
			break;
		case 1://"M^2"
			dat=atof(in);
			break;
		}
		return YSOK;
	}
	else
	{
		return YSERR;
	}
}

YSRESULT FsGetWeight(double &dat,const char in[])
{
	int uni;
	uni=FsGetUnit(in,FsWeightUnit);
	if(uni>=0)
	{
		switch(uni)
		{
		case 0://"KG"
			dat=atof(in);
			break;
		case 1://"LB"
			dat=atof(in)*0.453597;
			break;
		case 2://"T"
			dat=atof(in)*1000.0;
			break;
		case 3://"N"
			dat=atof(in)/FsGravityConst;
			break;
		}
		return YSOK;
	}
	else
	{
		return YSERR;
	}
}

YSRESULT FsGetForce(double &dat,const char in[])
{
	int uni;
	uni=FsGetUnit(in,FsWeightUnit);
	if(uni>=0)
	{
		switch(uni)
		{
		case 0://"KG"
			dat=atof(in)*FsGravityConst;
			break;
		case 1://"LB"
			dat=atof(in)*0.453597*FsGravityConst;
			break;
		case 2://"T"
			dat=atof(in)*1000.0*FsGravityConst;
			break;
		case 3://"N"
			dat=atof(in);
			break;
		}
		return YSOK;
	}
	else
	{
		return YSERR;
	}
}

YSRESULT FsGetSpeed(double &dat,const char in[])
{
	int uni;
	uni=FsGetUnit(in,FsSpeedUnit);
	if(uni>=0)
	{
		switch(uni)
		{
		case 0://"MACH"
			dat=atof(in)*340.0;
			break;
		case 1://"KM/H"
			dat=atof(in)*1000.0/3600.0;
			break;
		case 2://"M/S"
			dat=atof(in);
			break;
		case 3://"KT"
			dat=atof(in)*1852.0/3600.0;
			break;
		}
		return YSOK;
	}
	else
	{
		return YSERR;
	}
}

YSRESULT FsGetAngle(double &dat,const char in[])
{
	int uni;
	uni=FsGetUnit(in,FsAngleUnit);
	if(uni>=0)
	{
		switch(uni)
		{
		case 0: //"RAD"
			dat=atof(in);
			break;
		case 1: //"DEG"
			dat=atof(in)*YsPi/180.0;
			break;
		}
		return YSOK;
	}
	else
	{
		return YSERR;
	}
}

YSRESULT FsGetBool(YSBOOL &dat,const char in[])
{
	char str[256];
	strcpy(str,in);
	YsCapitalize(str);
	if(strncmp(str,"TRUE",4)==0)
	{
		dat=YSTRUE;
		return YSOK;
	}
	else if(strncmp(str,"FALSE",5)==0)
	{
		dat=YSFALSE;
		return YSOK;
	}
	else
	{
		return YSERR;
	}
}

YSRESULT FsGetNonDimensional(double &dat,const char in[])
{
	dat=atof(in);
	return YSOK;
}

YSRESULT FsGetVec3(YsVec3 &vec,YSSIZE_T ac,const char * const av[])
{
	if(ac>=3)
	{
		double x,y,z;
		if(FsGetLength(x,av[0])==YSOK &&
		   FsGetLength(y,av[1])==YSOK &&
		   FsGetLength(z,av[2])==YSOK)
		{
			vec.Set(x,y,z);
			return YSOK;
		}
	}
	return YSERR;
}

YSRESULT FsGetVec3(YsVec3 &vec,YSSIZE_T ac,const YsString av[])
{
	if(ac>=3)
	{
		double x,y,z;
		if(FsGetLength(x,av[0])==YSOK &&
		   FsGetLength(y,av[1])==YSOK &&
		   FsGetLength(z,av[2])==YSOK)
		{
			vec.Set(x,y,z);
			return YSOK;
		}
	}
	return YSERR;
}

YSRESULT FsGetAtt3(YsAtt3 &att,YSSIZE_T ac,const char * const av[])
{
	if(ac>=3)
	{
		double h,p,b;
		if(FsGetAngle(h,av[0])==YSOK &&
		   FsGetAngle(p,av[1])==YSOK &&
		   FsGetAngle(b,av[2])==YSOK)
		{
			att.Set(h,p,b);
			return YSOK;
		}
	}
	return YSERR;
}

YSRESULT FsGetAtt3(YsAtt3 &att,YSSIZE_T ac,const YsString av[])
{
	if(ac>=3)
	{
		double h,p,b;
		if(FsGetAngle(h,av[0])==YSOK &&
		   FsGetAngle(p,av[1])==YSOK &&
		   FsGetAngle(b,av[2])==YSOK)
		{
			att.Set(h,p,b);
			return YSOK;
		}
	}
	return YSERR;
}

int FsGetRandomBetween(int min, int max) 
{
	return min + rand() / (RAND_MAX / (max - min + 1) + 1);
}

double FsGetRandomBetween(double min, double max)
{
	double random = ((double)rand()) / (double)RAND_MAX;
	double range = max - min;
	return (random * range) + min;
}

////////////////////////////////////////////////////////////

StockCheck::StockCheck()
{
}

const char *const StockCheck::sojiAirList[] =
{
	"[SOJI]A-4_SKYHAWK",
	"[SOJI]A-6_INTRUDER",
	"[SOJI]A6M5_ZERO",
	"[SOJI]A-10A_THUNDERBOLT2",
	"[SOJI]AIRBUS300",
	"[SOJI]AIRBUS320",
	"[SOJI]AH-1_COBRA",
	"[SOJI]AH-64_APACHE",
	"[SOJI]ALPHAJET",
	"[SOJI]AMX",
	"[SOJI]AV-8B_HARRIER",
	"[SOJI]B-2_SPIRIT",
	"[SOJI]B-29_SUPERFORTRESS",
	"[SOJI]B-52_STRATOFORTRESS",
	"[SOJI]B737",
	"[SOJI]B747",
	"[SOJI]B767",
	"[SOJI]B777",
	"[SOJI]C-130_HERCULES",
	"[SOJI]CESSNA_172R",
	"[SOJI]CONCORDE",
	"[SOJI]DRAKEN",
	"[SOJI]E-2C_HAWKEYE",
	"[SOJI]EA-6B_PROWLER",
	"[SOJI]DIAMOND_ECLIPSE",
	"[SOJI]EUROFIGHTER_TYPHOON",
	"[SOJI]F-1",
	"[SOJI]F-2",
	"[SOJI]F-4E_PHANTOM",
	"[SOJI]F-4EJ_PHANTOM",
	"[SOJI]F-5_TIGER2",
	"[SOJI]F-14_TOMCAT",
	"[SOJI]F-15C_EAGLE",
	"[SOJI]F-15D_EAGLE",
	"[SOJI]F-15DJ_EAGLE",
	"[SOJI]F-15DJ_EAGLE_CHAIRO",
	"[SOJI]F-15DJ_EAGLE_KURO",
	"[SOJI]F-15DJ_EAGLE_MADARA",
	"[SOJI]F-15DJ_EAGLE_MIDORI",
	"[SOJI]F-15DJ_EAGLE_NAKAAO",
	"[SOJI]F-15DJ_EAGLE_SOTOAO",
	"[SOJI]F-15DJ_EAGLE_SOTOMIDORI",
	"[SOJI]F-15J_EAGLE",
	"[SOJI]F-16C_FIGHTINGFALCON",
	"[SOJI]F-16C_THUNDERBIRDS",
	"[SOJI]F-18C_HORNET",
	"[SOJI]F-18C_BLUEANGELS",
	"[SOJI]F-18E_SUPERHORNET",
	"[SOJI]F-22_RAPTOR",
	"[SOJI]F-86_BLUEIMPULSE",
	"[SOJI]F-100D_SUPERSABRE",
	"[SOJI]F-104_STARFIGHTER",
	"[SOJI]F-117A_NIGHTHAWK",
	"[SOJI]FOCKE_WULF_FW190",
	"[SOJI]G4M_TYPE1BOMBER",
	"[SOJI]GRIPEN",
	"[SOJI]HAWK",
	"[SOJI]HAWK_REDARROWS",
	"[SOJI]HURRICANE",
	"[SOJI]KFIR",
	"[SOJI]LANCASTER",
	"[SOJI]MIG-15_FAGOT",
	"[SOJI]MIG-21_FISHBED",
	"[SOJI]MIG-23_FLOGGER",
	"[SOJI]MIG-29_FULCRUM",
	"[SOJI]MIRAGE2000",
	"[SOJI]MU-2S",
	"[SOJI]P-51_MUSTANG",
	"[SOJI]RAFALE",
	"[SOJI]PIPER_ARCHER(WASHIN-AIR_MARKING)",
	"[SOJI]S-3_VIKING",
	"[SOJI]SPITFIRE",
	"[SOJI]SU-22_FITTER",
	"[SOJI]SU-24_FENCER",
	"[SOJI]SU-25_FROGFOOT",
	"[SOJI]SU-27_FLANKER",
	"[SOJI]T-2_BLUEIMPULSE",
	"[SOJI]T-4_BLUEIMPULSE",
	"[SOJI]T-400",
	"[SOJI]TORNADO",
	"[SOJI]TU-160_BLACKJACK",
	"[SOJI]U-125",
	"[SOJI]U-125A",
	"[SOJI]UH-60_BLACKHAWK",
	"[SOJI]UH-60J_BLACKHAWK",
	"[SOJI]V-107",
	"[SOJI]VIGGEN",
	"[SOJI]YS-11",
	NULL
};

const char *const StockCheck::sojiGndList[] =
{
	"[SOJI]AAA",
	"[SOJI]ALERT",
	"[SOJI]ASPAM",
	"[SOJI]ATSUGI_TOWER",
	"[SOJI]BIGBEN",
	"[SOJI]BOX1000X1000X1000",
	"[SOJI]BRIDGE1",
	"[SOJI]BRIDGE2",
	"[SOJI]BRIDGE3",
	"[SOJI]BUILD01",
	"[SOJI]BUILD10W",
	"[SOJI]BUILD11",
	"[SOJI]BUILD15",
	"[SOJI]BUILD30",
	"[SOJI]BUILD60",
	"[SOJI]BUILD90",
	"[SOJI]BURKE_DESTROYER",
	"[SOJI]BUS_1",
	"[SOJI]AIRCRAFTCARRIER",
	"[SOJI]CASTLE",
	"[SOJI]CAVE_BASE",
	"[SOJI]CAVE_BASE_COLL",
	"[SOJI]CIELING1",
	"[SOJI]CONTAINERSHIP_L",
	"[SOJI]ELEPHANT",
	"[SOJI]ELEVATED_RUNWAY_1000X60",
	"[SOJI]FIXED_CHECKPOINT_500FT",
	"[SOJI]FLOATINGBASE1",
	"[SOJI]FLOATINGBASE2",
	"[SOJI]FUEL_TRUCK",
	"[SOJI]FUEL_TRUCK(W)",
	"[SOJI]GATEGUARD_CONCORDE",
	"[SOJI]GEPARD",
	"[SOJI]GROUNDTARGET1",
	"[SOJI]GROUNDTARGET2",
	"[SOJI]HANGAR1",
	"[SOJI]HANGAR2",
	"[SOJI]HANGAR3",
	"[SOJI]HANGER",
	"[SOJI]HANGER1",
	"[SOJI]HANGER4",
	"[SOJI]HG01",
	"[SOJI]HG02",
	"[SOJI]HG03",
	"[SOJI]HG04",
	"[SOJI]HG05",
	"[SOJI]HG06",
	"[SOJI]HG07",
	"[SOJI]ILS",
	"[SOJI]HMS_INVINCIBLE",
	"[SOJI]ISLAND_BASE",
	"[SOJI]JEEP",
	"[SOJI]TOWER_OF_LONDON",
	"[SOJI]M1A1ABRAMS",
	"[SOJI]M60PATTON",
	"[SOJI]NDB",
	"[SOJI]PALACE",
	"[SOJI]PAPI_LEFT",
	"[SOJI]PAPI_RIGHT",
	"[SOJI]PIRATE",
	"[SOJI]PLANT",
	"[SOJI]POWER_PLANT",
	"[SOJI]RADAR",
	"[SOJI]RADARBASE",
	"[SOJI]NUCLEAR_REACTOR",
	"[SOJI]SAM",
	"[SOJI]SFC",
	"[SOJI]SFC_ALPHA",
	"[SOJI]SFC_CLUB",
	"[SOJI]SFC_GAMMA",
	"[SOJI]SFC_LAMBDA",
	"[SOJI]SFC_MEDIA",
	"[SOJI]SFC_O1",
	"[SOJI]SFC_O2",
	"[SOJI]SFC_OMEGA",
	"[SOJI]SFC_SIGMA",
	"[SOJI]SFC_TAU",
	"[SOJI]SHOP",
	"[SOJI]SHOPPING1",
	"[SOJI]SHOPPING2",
	"[SOJI]STATION",
	"[SOJI]T-64",
	"[SOJI]T-80",
	"[SOJI]TERMINAL",
	"[SOJI]TERMINAL_SHORT",
	"[SOJI]TEST",
	"[SOJI]TOWER",
	"[SOJI]TOWER1",
	"[SOJI]TOWER2",
	"[SOJI]TOWER3",
	"[SOJI]TRACKER",
	"[SOJI]TREE1",
	"[SOJI]TREE2",
	"[SOJI]TREES_200M_10M",
	"[SOJI]TREES_400M_20M",
	"[SOJI]TRUCK",
	"[SOJI]TUNNEL1",
	"[SOJI]TUNNEL2",
	"[SOJI]TUNNEL3",
	"[SOJI]TUNNEL4",
	"[SOJI]TUNNEL5",
	"[SOJI]UDALOY_DESTROYER",
	"[SOJI]VASI",
	"[SOJI]VOR",
	"[SOJI]VORDME",
	"[SOJI]WALL1",
	"[SOJI]WINDMILLBASE",
	"[SOJI]WINDMILLBLADE",
	NULL
};

const char *const StockCheck::sojiSceList[] =
{
	"[SOJI]RACING_VALLEY",
	"[SOJI]RACING_DESERT",
	"[SOJI]ATSUGI_AIRBASE",
	"[SOJI]CRESCENT_ISLAND",
	"[SOJI]HAWAII",
	"[SOJI]ISLAND_GOURD",
	"[SOJI]MATSUSHIMA_AIRBASE",
	"[SOJI]NAHA_AIRPORT",
	"[SOJI]NEWTA_AIRBASE",
	"[SOJI]PACIFIC_ATOLL",
	"[SOJI]SMALL_MAP",
	"[SOJI]TOHOKU",
	"[SOJI]AOMORI",
	"[SOJI]NORTH_KYUSYU",
	"[SOJI]AIRSTRIKE_CHALLENGE",
	"[SOJI]HEATHROW",
	"[SOJI]SLAPSTICK",
	"[SOJI]OCEAN",
	NULL
};

const char *const StockCheck::ysceAirList[] =
{
	"[YSCE]BK-117",
	"[YSCE]CONCORDE",
	"[YSCE]F-16A_FIGHTINGFALCON",
	"[YSCE]CESSNA_172",
	"[YSCE]B737",
	NULL
};

const char *const StockCheck::ysceGndList[] =
{
	"[YSCE]TOWER",
	"[YSCE]HANGAR",
	"[YSCE]TERMINAL",
	"[YSCE]ILS",
	"[YSCE]VOR",
	NULL
};

const char *const StockCheck::ysceSceList[] =
{
	"[YSCE]HAWAII",
	"[YSCE]NORTH_KYUSYU",
	"[YSCE]SLAPSTICK",
	NULL
};

YSBOOL StockCheck::FsCheckIfSojiStock(const char in[])
{
	if (FsCheckIfSojiAir(in) == YSTRUE)
	{
		return YSTRUE;
	}
	else if (FsCheckIfSojiGnd(in) == YSTRUE)
	{
		return YSTRUE;
	}
	else if (FsCheckIfSojiSce(in) == YSTRUE)
	{
		return YSTRUE;
	}
	return YSFALSE;
}

int StockCheck::FsCheckIfSojiAir(const char in[])
{
	YsString str;
	for (int i = 0; sojiAirList[i] != NULL; i++)
	{
		str = sojiAirList[i];
		str=str.Subset(6, 256);
		if (strcmp(str, in) == 0)
		{
			return i;
		}
	}
	return -1;
}

int StockCheck::FsCheckIfSojiGnd(const char in[])
{
	YsString str;
	for (int i = 0; sojiGndList[i] != NULL; i++)
	{
		str = sojiGndList[i];
		str = str.Subset(6, 256);
		if (strcmp(str, in) == 0)
		{
			return i;
		}
	}
	return -1;
}

int StockCheck::FsCheckIfSojiSce(const char in[])
{
	YsString str;
	for (int i = 0; sojiSceList[i] != NULL; i++)
	{
		str = sojiSceList[i];
		str = str.Subset(6, 256);
		if (strcmp(str, in) == 0)
		{
			return i;
		}
	}
	return -1;
}

YSBOOL StockCheck::FsAddTagIfSojiAir(const char in[],YsString &out)
{
	int i = FsCheckIfSojiAir(in);
	if (i >= 0)
	{
		out = sojiAirList[i];
		return YSTRUE;
	}
	return YSFALSE;
}

YSBOOL StockCheck::FsAddTagIfSojiGnd(const char in[], YsString &out)
{
	int i = FsCheckIfSojiGnd(in);
	if (i >= 0)
	{
		out = sojiGndList[i];
		return YSTRUE;
	}
	return YSFALSE;
}

YSBOOL StockCheck::FsAddTagIfSojiSce(const char in[], YsString &out)
{
	int i = FsCheckIfSojiSce(in);
	if (i >= 0)
	{
		out = sojiSceList[i];
		return YSTRUE;
	}
	return YSFALSE;
}

YSBOOL StockCheck::FsRemoveTagIfSojiAir(const char in[], YsString &out)
{
	YsString str = in;
	str = str.Subset(6, 256);
	if (FsCheckIfSojiAir(str) >= 0)
	{
		out = str;
		return YSTRUE;
	}
	return YSFALSE;
}

YSBOOL StockCheck::FsRemoveTagIfSojiGnd(const char in[], YsString &out)
{
	YsString str = in;
	str = str.Subset(6, 256);
	if (FsCheckIfSojiGnd(str) >= 0)
	{
		out = str;
		return YSTRUE;
	}
	return YSFALSE;
}

YSBOOL StockCheck::FsRemoveTagIfSojiSce(const char in[], YsString &out)
{
	YsString str = in;
	str = str.Subset(6, 256);
	if (FsCheckIfSojiSce(str) >= 0)
	{
		out = str;
		return YSTRUE;
	}
	return YSFALSE;
}

int StockCheck::FsFindNextStockAir(int idx, YsString &out)
{
	int res = -1;
	if (idx >= 0)
	{
		res = idx + 1;
		if (res <= nSojiAir)
		{
			out = sojiAirList[res];
		}
		else
		{
			out = ysceAirList[res - nSojiAir];
		}
	}
	else if (idx > nSojiAir + nYsceAir)
	{
		res = -1;
	}
	else
	{
		out = sojiAirList[0];
		res = 0;
	}
	return res;
}