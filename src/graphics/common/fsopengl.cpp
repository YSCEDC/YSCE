#include <ysviewcontrol.h>
#include <ysbitmap.h>
#include <yssystemfont.h>
#include "fsopengl.h"
#include "fsfontrenderer.h"

FsProjection::FsProjection()
{
	//Initialize projection for a 1280x720px image
	prjMode = YsProjectionTransformation::PERSPECTIVE;
	matrixCached=YSFALSE;
	fovInPixels = 640;
	fov = 71.7;
	fovSecondary = 59.5;
	tanFov = 3.02;
	tanFovSecondary = 1.7;
	prjPlnDist = 212.1;
	nearz = 0.1;
	farz = 18000.0;
	viewportDim.Set(1280,720);

	cx = 640;
	cy = 360;

	zoomProjection = 1.0;
}

void FsProjection::CacheMatrix(void) const
{
	matrixCached=YSTRUE;

	double aspect=viewportDim.xd()/viewportDim.yd();

	YsProjectionTransformation prjTfm;
	prjTfm.SetProjectionMode(prjMode);
	prjTfm.SetAspectRatio(aspect);
	prjTfm.SetFOVY(atan(tanFov));
	prjTfm.SetNearFar(nearz,farz);
	prjTfm.SetZoom(1.0);

	projMatCache=prjTfm.GetProjectionMatrix();
}

void FsProjection::UncacheMatrix(void) const
{
	matrixCached=YSFALSE;
}

const YsMatrix4x4 &FsProjection::GetMatrix(void) const
{
	if(YSTRUE!=matrixCached)
	{
		CacheMatrix();
	}
	return projMatCache;
}

////////////////////////////////////////////////////////////

void FsDrawString(int x,int y,const wchar_t str[],YsColor col)
{
	YsBitmap bmp;
	YsColor transparent=col;
	transparent.SetAi(0);
	fsUnicodeRenderer.RenderString(bmp,str,col,transparent);
	FsDrawBmp(bmp,x,y);
}

