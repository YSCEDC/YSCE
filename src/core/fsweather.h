#ifndef FSWEATHER_IS_INCLUDED
#define FSWEATHER_IS_INCLUDED
/* { */

#include "fsdef.h"

// FS_FOG_VISIBILITY_MAX, FS_FOG_VISIBILITY_MIN, FSCLOUDLAYER_NONE, and FSCLOUDLAYER_OVERCAST are moved to fsdef.h


class FsWeatherCloudLayer
{
public:
	int cloudLayerType;
	double y0,y1;

	static const char *CloudLayerTypeString(int cloudLayerType);
	static int CloudLayerTypeFromString(const char str[]);
	FsWeatherCloudLayer Overcast(double y0,double y1); // Returns an overcast 
};



class FsWeather
{
protected:
	YsVec3 wind;
	YSBOOL fog;
	double fogVisibility;

	YsVec3 transWind;
	double transFogVisibility;

	YsArray <FsWeatherCloudLayer> cloudLayer;

	YsColor daylightColour = YsColor(1.0,1.0,1.0); //Because it is spelled colour. Not color... uncultured swines.
	YsColor nightColour = YsColor(0.1,0.1,0.3);
	YsColor dawnColour = YsColor(1,0.5,0.0);

public:
	FsWeather();
	~FsWeather();

	void WeatherTransition(const double &dt);

	const YsVec3 &GetWind(void) const;
	YSRESULT SetWind(const YsVec3 &wind);
	YSRESULT SetTransWind(const YsVec3 &wind);

	const double &GetFogVisibility(void) const;
	YSRESULT SetFogVisibility(const double &visibility);
	YSRESULT SetTransFogVisibility(const double &visibility);

	void SetFog(YSBOOL f);
	void SetTransFog(YSBOOL f);
	YSBOOL GetFog(void) const;

	void SetCloudLayer(YSSIZE_T nLayer,const FsWeatherCloudLayer layer[]);
	void AddCloudLayer(const FsWeatherCloudLayer &layer);
	void GetCloudLayer(int &nLayer,const FsWeatherCloudLayer *&layer) const;
	int GetCloudLayerCount() const;
	YSBOOL IsInCloudLayer(const YsVec3 &pos) const;

	YSRESULT Save(FILE *fp) const;
	YSRESULT Load(FILE *fp);

	void DrawCloudLayer(const YsVec3 &cameraPos) const;

	//Day/Night cycle functions

	YsColor GetLightColour(const double dayTime) const;
	YsColor GetLightColour(YsColor skyColour, const double dayTime) const;
	double GetLightIntensity(const double dayTime) const;
	double ColourInterpolate(const double colour1, const double colour2, const double i) const;
	YSBOOL IsDay(const double dayTime) const;
	YSBOOL IsDuskOrDawn(const double dayTime) const;
	double DuskIntensity(const double dayTime) const;
	void GetDayTime(double& daytime, double dt, int dayLength) const;
	void SetSunPosition(YsVec3& lightPosition, double dayTime) const;
};


/* } */
#endif
