

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stddef.h>
#include <math.h>

#include <ysclass.h>

#include "fs.h"





const char *FsWeatherCloudLayer::CloudLayerTypeString(int cloudLayerType)
{
	switch(cloudLayerType)
	{
	default:
	case FSCLOUDLAYER_NONE:
		return "CLR";
	case FSCLOUDLAYER_OVERCAST:
		return "OVC";
	};
}

int FsWeatherCloudLayer::CloudLayerTypeFromString(const char str[])
{
	if(strcmp(str,"OVC")==0 || strcmp(str,"ovc")==0)
	{
		return FSCLOUDLAYER_OVERCAST;
	}
	return FSCLOUDLAYER_NONE;
}

FsWeatherCloudLayer FsWeatherCloudLayer::Overcast(double y0,double y1)
{
	this->y0=y0;
	this->y1=y1;
	this->cloudLayerType=FSCLOUDLAYER_OVERCAST;
	return *this;
}




FsWeather::FsWeather()
{
	wind.Set(0.0,0.0,0.0);
	fog=YSTRUE;
	fogVisibility=20000;

	transWind.Set(0.0,0.0,0.0);
	transFogVisibility=20000;
}

FsWeather::~FsWeather()
{
}

void FsWeather::WeatherTransition(const double &dt)
{
	if(wind!=transWind)
	{
		YsVec3 d;
		double l,chg;

		chg=4.0*dt;

		d=transWind-wind;
		l=d.GetLength();
		if(l<chg)
		{
			wind=transWind;
		}
		else
		{
			d/=l;
			wind+=d*chg;
		}
	}

	if(YsEqual(fogVisibility,transFogVisibility)!=YSTRUE)
	{
		double d,chg;

		d=transFogVisibility-fogVisibility;
		chg=10000.0*dt;

		if(fabs(d)<chg)
		{
			fogVisibility=transFogVisibility;
		}
		else
		{
			if(d>0)
			{
				fogVisibility+=chg;
			}
			else
			{
				fogVisibility-=chg;
			}
		}
	}
}

const YsVec3 &FsWeather::GetWind(void) const
{
	return wind;
}

YSRESULT FsWeather::SetWind(const YsVec3 &w)
{
	wind=w;
	transWind=w;
	return YSOK;
}

YSRESULT FsWeather::SetTransWind(const YsVec3 &w)
{
	transWind=w;
	return YSOK;
}

const double &FsWeather::GetFogVisibility(void) const
{
	return fogVisibility;
}

YSRESULT FsWeather::SetFogVisibility(const double &visibility)
{
	fogVisibility=YsBound(visibility,FS_FOG_VISIBILITY_MIN,FS_FOG_VISIBILITY_MAX);
	transFogVisibility=fogVisibility;
	return YSOK;
}

YSRESULT FsWeather::SetTransFogVisibility(const double &vis)
{
	transFogVisibility=vis;
	return YSOK;
}

void FsWeather::SetFog(YSBOOL f)
{
	fog=f;
}

YSBOOL FsWeather::GetFog(void) const
{
	return fog;
}

void FsWeather::SetCloudLayer(YSSIZE_T nLayer,const FsWeatherCloudLayer layer[])
{
	cloudLayer.Set(nLayer,layer);
}

void FsWeather::AddCloudLayer(const FsWeatherCloudLayer &layer)
{
	cloudLayer.Append(layer);
}

void FsWeather::GetCloudLayer(int &nLayer,const FsWeatherCloudLayer *&layer) const
{
	nLayer=(int)cloudLayer.GetN();
	layer=cloudLayer;
}

YSBOOL FsWeather::IsInCloudLayer(const YsVec3 &pos) const
{
	int i;
	forYsArray(i,cloudLayer)
	{
		if(cloudLayer[i].y0<=pos.y() && pos.y()<=cloudLayer[i].y1)
		{
			return YSTRUE;
		}
	}
	return YSFALSE;
}

int FsWeather::GetCloudLayerCount() const
{
	return (int)cloudLayer.GetN();
}
//Day/Night cycle functions


YsColor FsWeather::GetLightColour(const double dayTime) const{
	YsColor lightColour = daylightColour;
	if (IsDuskOrDawn(dayTime) == YSTRUE){
		//If it's dusk or dawn, then we need to fade the colour.
			lightColour.SetRd(ColourInterpolate(daylightColour.Rd(),dawnColour.Rd(),DuskIntensity(dayTime)));
			lightColour.SetGd(ColourInterpolate(daylightColour.Gd(),dawnColour.Gd(),DuskIntensity(dayTime)));
			lightColour.SetBd(ColourInterpolate(daylightColour.Bd(),dawnColour.Bd(),DuskIntensity(dayTime)));
	}
	if (IsDay(dayTime) == YSFALSE){
		lightColour = nightColour;
	}

	return lightColour;
};

YsColor FsWeather::GetLightColour(YsColor skyColour, const double dayTime) const{
	YsColor lightColour = skyColour;
	if (IsDuskOrDawn(dayTime) == YSTRUE){
		//If it's dusk or dawn, then we need to fade the colour.
			lightColour.SetRd(ColourInterpolate(skyColour.Rd(),dawnColour.Rd(),DuskIntensity(dayTime)));
			lightColour.SetGd(ColourInterpolate(skyColour.Gd(),dawnColour.Gd(),DuskIntensity(dayTime)));
			lightColour.SetBd(ColourInterpolate(skyColour.Bd(),dawnColour.Bd(),DuskIntensity(dayTime)));			
		
	}

	return lightColour;

};

double FsWeather::GetLightIntensity(const double dayTime) const{
	double lightIntensity = 1.0;
	if (IsDuskOrDawn(dayTime) == YSTRUE){ //Then it's dusk/dawn
		lightIntensity = DuskIntensity(dayTime);
	}
	if (IsDay(dayTime) == YSFALSE){
		lightIntensity = 0.1;
	}
	return lightIntensity;
};

double FsWeather::ColourInterpolate(const double colour2, const double colour1, const double i) const {
	return colour1 + (colour2 - colour1) * i;
};

YSBOOL FsWeather::IsDay(const double dayTime) const{
	if (dayTime < YsPi/2 || dayTime > 3*YsPi/2){
		return YSTRUE;
	}
	else{
		return YSFALSE;
	}
};

YSBOOL FsWeather::IsDuskOrDawn(const double dayTime) const{
	if (sin(dayTime+YsPi/2)<0.2 && sin(dayTime+YsPi/2)>-0.2){
		return YSTRUE;
	}
	else{
		return YSFALSE;
	}

};

double FsWeather::DuskIntensity(const double dayTime) const{
	//Scale the dusk intensity. Dusk is between sin(dayTime+YsPi/2) = 0.2 and -0.2.
	//return sin(dayTime+YsPi/2)*5+0.5;
	return abs(sin(dayTime+YsPi/2))*5;

};

//Increments the dayTime by deltaTime (in seconds) and the dayLength (in seconds)
//Returns the dayTime - which is the time of day * 2*PI. 
// 0 is mid day, PI is midnight, 2*PI is mid day again.
void FsWeather::GetDayTime(double& daytime, double dt, int dayLength) const{
	int totalSteps = dayLength/dt;
	double step = 2 * YsPi / totalSteps;
	daytime = daytime + step;
	if (daytime > 2 * YsPi){
		daytime = 0;
	}
};

void FsWeather::SetSunPosition(YsVec3& lightPosition, double dayTime) const{
	if (IsDay(dayTime) == YSTRUE){
		lightPosition.SetX(cos(dayTime+YsPi/2));
		lightPosition.SetY(abs(sin(dayTime+YsPi/2)));
	}
	else{
		lightPosition.SetX(-cos(dayTime+YsPi/2));
		lightPosition.SetY(abs(sin(dayTime+YsPi/2)));
	}
};

//Save cloud functions - to review whether these continue to be used with new clouds in future.

YSRESULT FsWeather::Save(FILE *fp) const
{
	fprintf(fp,"WEATHERX\n");

	fprintf(fp,"CONSTWIND %lfm/s %lfm/s %lfm/s\n",wind.x(),wind.y(),wind.z());
	fprintf(fp,"VISIBILIT %lfm\n",fogVisibility);

	int i;
	forYsArray(i,cloudLayer)
	{
		fprintf(fp,"CLOUDLAYER %s %lfm %lfm\n",
		    FsWeatherCloudLayer::CloudLayerTypeString(cloudLayer[i].cloudLayerType),
		    cloudLayer[i].y0,
		    cloudLayer[i].y1);
	}

	fprintf(fp,"ENDWEATHER\n");

	return YSOK;
}

static const char *const weatherCmd[]=
{
	"ENDWEATHER",
	"CONSTWIND",
	"VISIBILIT",
	"CLOUDLAYER",
	NULL
};

YSRESULT FsWeather::Load(FILE *fp)
{
	// Keyword WEATHER is already read by FsWorld
	char buf[256];
	int ac;
	char *av[16];

	while(fgets(buf,256,fp)!=NULL)
	{
		if(YsArguments(&ac,av,16,buf)==YSOK)
		{
			int cmd;
			if(YsCommandNumber(&cmd,av[0],weatherCmd)==YSOK)
			{
				switch(cmd)
				{
				case 0:  // ENDWEATHER
					goto ENDWEATHER;
				case 1:  // CONSTWIND
					if(ac>=4)
					{
						double x,y,z;
						if(FsGetSpeed(x,av[1])==YSOK &&
						   FsGetSpeed(y,av[2])==YSOK &&
						   FsGetSpeed(z,av[3])==YSOK)
						{
							wind.Set(x,y,z);
							transWind.Set(x,y,z);  // 2009/04/28
						}
					}
					break;
				case 2:  // VISIBILIT
					if(ac>=2)
					{
						double l;
						if(FsGetLength(l,av[1])==YSOK)
						{
							fogVisibility=l;
							transFogVisibility=l;  // 2009/04/28
						}
					}
					break;
				case 3: // 	"CLOUDLAYER",
					if(ac>=4)
					{
						FsWeatherCloudLayer lyr;
						lyr.cloudLayerType=FsWeatherCloudLayer::CloudLayerTypeFromString(av[1]);
						FsGetLength(lyr.y0,av[2]);
						FsGetLength(lyr.y1,av[3]);
						cloudLayer.Append(lyr);
					}
					break;
				}
			}
		}
	}

ENDWEATHER:
	return YSOK;
}


