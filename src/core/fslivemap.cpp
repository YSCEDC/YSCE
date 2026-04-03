#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#ifndef __APPLE__
#include <GL/gl.h>
#else
#include <OpenGL/gl.h>
#endif
#include "fslivemap.h"
#include "fssimulation.h"
#include "graphics/common/fsopengl.h"

static YSBOOL ClipLine(int &ax, int &ay, int &bx, int &by, int x1, int y1, int x2, int y2)
{
	enum { INSIDE=0, LEFT=1, RIGHT=2, BOTTOM=4, TOP=8 };
	auto outcode=[&](int x, int y) -> int {
		int code=INSIDE;
		if(x<x1) code|=LEFT;
		else if(x>x2) code|=RIGHT;
		if(y<y1) code|=TOP;
		else if(y>y2) code|=BOTTOM;
		return code;
	};

	int codeA=outcode(ax,ay);
	int codeB=outcode(bx,by);

	for(;;)
	{
		if((codeA|codeB)==0)
			return YSTRUE;
		if((codeA&codeB)!=0)
			return YSFALSE;

		int codeOut=(codeA!=0) ? codeA : codeB;
		int nx=0,ny=0;
		if(codeOut&BOTTOM)
		{
			nx=ax+(int)((double)(bx-ax)*(y2-ay)/(by-ay));
			ny=y2;
		}
		else if(codeOut&TOP)
		{
			nx=ax+(int)((double)(bx-ax)*(y1-ay)/(by-ay));
			ny=y1;
		}
		else if(codeOut&RIGHT)
		{
			ny=ay+(int)((double)(by-ay)*(x2-ax)/(bx-ax));
			nx=x2;
		}
		else if(codeOut&LEFT)
		{
			ny=ay+(int)((double)(by-ay)*(x1-ax)/(bx-ax));
			nx=x1;
		}

		if(codeOut==codeA)
		{
			ax=nx; ay=ny;
			codeA=outcode(ax,ay);
		}
		else
		{
			bx=nx; by=ny;
			codeB=outcode(bx,by);
		}
	}
}

static void DrawClippedLine(int ax, int ay, int bx, int by, int x1, int y1, int x2, int y2, const YsColor &col)
{
	if(ClipLine(ax, ay, bx, by, x1, y1, x2, y2)==YSTRUE)
		FsDrawLine(ax, ay, bx, by, col);
}

static int ClipEdge(int *out, const int *in, int n, int edge, int val, YSBOOL clipLess)
{
	if(n < 1) return 0;
	int cnt = 0;
	for(int i = 0; i < n; i++)
	{
		int cur = i, nxt = (i + 1) % n;
		int cx = in[cur * 2], cy = in[cur * 2 + 1];
		int nx = in[nxt * 2], ny = in[nxt * 2 + 1];
		YSBOOL cIn, nIn;
		int cv, nv;
		if(edge == 0 || edge == 2) { cv = cx; nv = nx; } else { cv = cy; nv = ny; }
		if(clipLess == YSTRUE) { cIn = (cv >= val ? YSTRUE : YSFALSE); nIn = (nv >= val ? YSTRUE : YSFALSE); }
		else { cIn = (cv <= val ? YSTRUE : YSFALSE); nIn = (nv <= val ? YSTRUE : YSFALSE); }

		if(cIn == YSTRUE && nIn == YSTRUE)
		{
			if(cnt < 510) { out[cnt * 2] = nx; out[cnt * 2 + 1] = ny; cnt++; }
		}
		else if(cIn == YSTRUE && nIn == YSFALSE)
		{
			int dx = nx - cx, dy = ny - cy;
			int ix, iy;
			if(edge == 0 || edge == 2) { ix = val; iy = (dx != 0) ? cy + (int)((double)dy * (val - cx) / dx) : cy; }
			else { iy = val; ix = (dy != 0) ? cx + (int)((double)dx * (val - cy) / dy) : cx; }
			if(cnt < 510) { out[cnt * 2] = ix; out[cnt * 2 + 1] = iy; cnt++; }
		}
		else if(cIn == YSFALSE && nIn == YSTRUE)
		{
			int dx = nx - cx, dy = ny - cy;
			int ix, iy;
			if(edge == 0 || edge == 2) { ix = val; iy = (dx != 0) ? cy + (int)((double)dy * (val - cx) / dx) : cy; }
			else { iy = val; ix = (dy != 0) ? cx + (int)((double)dx * (val - cy) / dy) : cx; }
			if(cnt < 510) { out[cnt * 2] = ix; out[cnt * 2 + 1] = iy; cnt++; }
			if(cnt < 510) { out[cnt * 2] = nx; out[cnt * 2 + 1] = ny; cnt++; }
		}
	}
	return cnt;
}

static void DrawClippedPolygon(int n, int *plg, int x1, int y1, int x2, int y2, const YsColor &col)
{
	int tmpA[1024], tmpB[1024];
	int cn = n;
	if(cn > 510) cn = 510;
	memcpy(tmpA, plg, cn * 2 * sizeof(int));
	cn = ClipEdge(tmpB, tmpA, cn, 0, x1, YSTRUE);
	cn = ClipEdge(tmpA, tmpB, cn, 2, x2, YSFALSE);
	cn = ClipEdge(tmpB, tmpA, cn, 1, y1, YSTRUE);
	cn = ClipEdge(tmpA, tmpB, cn, 3, y2, YSFALSE);
	if(cn >= 3)
		FsDrawPolygon(cn, tmpA, col);
}

static void DrawMapElement(const Ys2DDrawingElement &elem, const YsMatrix4x4 &tfm,
	double cx, double cy, double mag, double px, double pz,
	int x1, int y1, int x2, int y2)
{
	const YsArray<YsVec2> &pts = elem.GetPointList();
	if(pts.GetN() < 3)
		return;

	YSBOOL anyVisible = YSFALSE;
	for(YSSIZE_T k = 0; k < pts.GetN(); k++)
	{
		YsVec3 wp;
		tfm.Mul(wp, YsVec3(pts[k].x(), 0.0, pts[k].y()), 1.0);
		int sx = (int)(cx + (wp.x() - px) * mag);
		int sy = (int)(cy - (wp.z() - pz) * mag);
		if(sx >= x1 && sx <= x2 && sy >= y1 && sy <= y2)
		{
			anyVisible = YSTRUE;
			break;
		}
	}
	if(anyVisible != YSTRUE)
		return;

	YsColor col = elem.GetColor();
	Ys2DDrawingElement::OBJTYPE type = elem.GetElemType();

	if(type == Ys2DDrawingElement::POLYGON)
	{
		int n = (int)pts.GetN();
		int plg[512];
		int kept = 0;
		for(int k = 0; k < n && kept < 255; k++)
		{
			YsVec3 wp;
			tfm.Mul(wp, YsVec3(pts[k].x(), 0.0, pts[k].y()), 1.0);
			int sx = (int)(cx + (wp.x() - px) * mag);
			int sy = (int)(cy - (wp.z() - pz) * mag);
			if(kept > 0 && k < n - 1 &&
			   abs(sx - plg[(kept-1)*2]) < 4 && abs(sy - plg[(kept-1)*2+1]) < 4)
				continue;
			plg[kept*2] = sx;
			plg[kept*2+1] = sy;
			kept++;
		}
		if(kept >= 3)
			DrawClippedPolygon(kept, plg, x1, y1, x2, y2, col);
	}
	else if(type == Ys2DDrawingElement::TRIANGLES)
	{
		for(YSSIZE_T k = 0; k + 2 < pts.GetN(); k += 3)
		{
			int plg[6];
			for(int j = 0; j < 3; j++)
			{
				YsVec3 wp;
				tfm.Mul(wp, YsVec3(pts[k + j].x(), 0.0, pts[k + j].y()), 1.0);
				plg[j * 2] = (int)(cx + (wp.x() - px) * mag);
				plg[j * 2 + 1] = (int)(cy - (wp.z() - pz) * mag);
			}
			if(abs(plg[2]-plg[0]) < 4 && abs(plg[3]-plg[1]) < 4 &&
			   abs(plg[4]-plg[0]) < 4 && abs(plg[5]-plg[1]) < 4)
				continue;
			DrawClippedPolygon(3, plg, x1, y1, x2, y2, col);
		}
	}
	else if(type == Ys2DDrawingElement::QUADS)
	{
		for(YSSIZE_T k = 0; k + 3 < pts.GetN(); k += 4)
		{
			int plg[8];
			for(int j = 0; j < 4; j++)
			{
				YsVec3 wp;
				tfm.Mul(wp, YsVec3(pts[k + j].x(), 0.0, pts[k + j].y()), 1.0);
				plg[j * 2] = (int)(cx + (wp.x() - px) * mag);
				plg[j * 2 + 1] = (int)(cy - (wp.z() - pz) * mag);
			}
			if(abs(plg[2]-plg[0]) < 4 && abs(plg[3]-plg[1]) < 4 &&
			   abs(plg[4]-plg[0]) < 4 && abs(plg[5]-plg[1]) < 4 &&
			   abs(plg[6]-plg[0]) < 4 && abs(plg[7]-plg[1]) < 4)
				continue;
			DrawClippedPolygon(4, plg, x1, y1, x2, y2, col);
		}
	}
	else if(type == Ys2DDrawingElement::QUADSTRIP)
	{
		for(YSSIZE_T k = 0; k + 3 < pts.GetN(); k += 2)
		{
			int plg[8];
			YsVec3 wp;
			tfm.Mul(wp, YsVec3(pts[k].x(), 0.0, pts[k].y()), 1.0);
			plg[0] = (int)(cx + (wp.x() - px) * mag); plg[1] = (int)(cy - (wp.z() - pz) * mag);
			tfm.Mul(wp, YsVec3(pts[k + 1].x(), 0.0, pts[k + 1].y()), 1.0);
			plg[2] = (int)(cx + (wp.x() - px) * mag); plg[3] = (int)(cy - (wp.z() - pz) * mag);
			tfm.Mul(wp, YsVec3(pts[k + 3].x(), 0.0, pts[k + 3].y()), 1.0);
			plg[4] = (int)(cx + (wp.x() - px) * mag); plg[5] = (int)(cy - (wp.z() - pz) * mag);
			tfm.Mul(wp, YsVec3(pts[k + 2].x(), 0.0, pts[k + 2].y()), 1.0);
			plg[6] = (int)(cx + (wp.x() - px) * mag); plg[7] = (int)(cy - (wp.z() - pz) * mag);
			if(abs(plg[2]-plg[0]) < 4 && abs(plg[3]-plg[1]) < 4 &&
			   abs(plg[4]-plg[0]) < 4 && abs(plg[5]-plg[1]) < 4 &&
			   abs(plg[6]-plg[0]) < 4 && abs(plg[7]-plg[1]) < 4)
				continue;
			DrawClippedPolygon(4, plg, x1, y1, x2, y2, col);
		}
	}
}

static void DrawTerrainRecursive(const YsScenery *scn, const YsMatrix4x4 &parentTfm,
	double cx, double cy, double mag, double px, double pz,
	int x1, int y1, int x2, int y2)
{
	YsMatrix4x4 tfm = parentTfm;
	tfm.Translate(scn->GetPosition());
	tfm.RotateXZ(scn->GetAttitude().h());
	tfm.RotateZY(scn->GetAttitude().p());
	tfm.RotateXY(scn->GetAttitude().b());

	const YsListItem<YsScenery2DDrawing> *mapItem = NULL;
	while((mapItem = scn->FindNextMap(mapItem)) != NULL)
	{
		YsMatrix4x4 mapTfm = tfm;
		mapTfm.Translate(mapItem->dat.GetPosition());
		mapTfm.RotateXZ(mapItem->dat.GetAttitude().h());
		mapTfm.RotateZY(mapItem->dat.GetAttitude().p());
		mapTfm.RotateXY(mapItem->dat.GetAttitude().b());

		const Ys2DDrawing &drawing = mapItem->dat.GetDrawing();
		const YsListItem<Ys2DDrawingElement> *elemItem = NULL;
		while((elemItem = drawing.FindNextElem(elemItem)) != NULL)
		{
			Ys2DDrawingElement::OBJTYPE type = elemItem->dat.GetElemType();
			if(type != Ys2DDrawingElement::POLYGON &&
			   type != Ys2DDrawingElement::TRIANGLES &&
			   type != Ys2DDrawingElement::QUADS &&
			   type != Ys2DDrawingElement::QUADSTRIP)
				continue;
			DrawMapElement(elemItem->dat, mapTfm, cx, cy, mag, px, pz, x1, y1, x2, y2);
		}
	}

	const YsListItem<YsScenery> *childScn = NULL;
	while((childScn = scn->FindNextChildScenery(childScn)) != NULL)
	{
		DrawTerrainRecursive(&childScn->dat, tfm, cx, cy, mag, px, pz, x1, y1, x2, y2);
	}
}

void FsLiveMap::Draw(const FsSimulation *sim, int x1, int y1, int x2, int y2, double range) const
{
	static GLuint cacheTexId = 0;
	static double cachedPx = 0, cachedPz = 0, cachedRange = 0;
	static int cachedW = 0, cachedH = 0;
	static int frameCounter = 0;
	frameCounter++;

	const FsField *fld = sim->GetField();
	if(fld == NULL)
		return;

	YsVec3 playerPos = YsOrigin();
	double playerHdg = 0.0;
	const FsAirplane *playerPlane = sim->GetPlayerAirplane();
	const FsGround *playerGround = sim->GetPlayerGround();
	if(playerPlane != NULL)
	{
		playerPos = playerPlane->GetPosition();
		playerHdg = -playerPlane->GetAttitude().h();
	}
	else if(playerGround != NULL)
	{
		playerPos = playerGround->GetPosition();
		playerHdg = -playerGround->GetAttitude().h();
	}
	else
	{
		return;
	}

	double viewRange = range;
	if(viewRange <= 0.0)
	{
		YsVec3 bbMin, bbMax;
		fld->GetBoundingBox(bbMin, bbMax);
		double fieldSizeX = bbMax.x() - bbMin.x();
		double fieldSizeZ = bbMax.z() - bbMin.z();
		viewRange = YsGreater(fieldSizeX, fieldSizeZ) * 0.65;
		if(viewRange < 5000.0)
			viewRange = 5000.0;
	}

	int mapW = x2 - x1;
	int mapH = y2 - y1;
	int mapSize = YsAbs(mapW);
	double mag = (double)mapSize / (viewRange * 2.0);

	double cx = (x1 + x2) / 2.0;
	double cy = (y1 + y2) / 2.0;
	YsVec2 w1(x1, y1), w2(x2, y2);

	YSBOOL dirty = YSFALSE;
	if(cacheTexId == 0 || cachedRange != viewRange || mapW != cachedW || mapH != cachedH)
		dirty = YSTRUE;
	else if(frameCounter % 3 == 0 &&
	        (playerPos.x() != cachedPx || playerPos.z() != cachedPz))
		dirty = YSTRUE;

	if(dirty == YSTRUE)
	{
		FsDrawRect(x1, y1, x2, y2, YsColor(0, 0, 40), YSTRUE);
		const YsScenery *scn = fld->GetFieldPtr();
		if(scn != NULL)
			DrawTerrainRecursive(scn, YsIdentity4x4(),
				cx, cy, mag, playerPos.x(), playerPos.z(), x1, y1, x2, y2);

		if(cacheTexId == 0)
		{
			glGenTextures(1, &cacheTexId);
			glBindTexture(GL_TEXTURE_2D, cacheTexId);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		}
		else
		{
			glBindTexture(GL_TEXTURE_2D, cacheTexId);
		}
		if(mapW != cachedW || mapH != cachedH)
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, mapW, mapH, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);

		GLint vp[4];
		glGetIntegerv(GL_VIEWPORT, vp);
		glCopyTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, x1, vp[3] - y2, mapW, mapH);
		glBindTexture(GL_TEXTURE_2D, 0);

		cachedPx = playerPos.x();
		cachedPz = playerPos.z();
		cachedRange = viewRange;
		cachedW = mapW;
		cachedH = mapH;
	}
	else
	{
		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, cacheTexId);
		glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
		glBegin(GL_QUADS);
		glTexCoord2f(0.0f, 1.0f); glVertex2i(x1, y1);
		glTexCoord2f(1.0f, 1.0f); glVertex2i(x2, y1);
		glTexCoord2f(1.0f, 0.0f); glVertex2i(x2, y2);
		glTexCoord2f(0.0f, 0.0f); glVertex2i(x1, y2);
		glEnd();
		glBindTexture(GL_TEXTURE_2D, 0);
		glDisable(GL_TEXTURE_2D);
	}

	FsDrawRect(x1, y1, x2, y2, YsCyan(), YSFALSE);

	YsArray<const YsSceneryRectRegion *, 64> rgnList;
	if(fld->SearchFieldRegionById(rgnList, FS_RGNID_AIRPORT_AREA) == YSOK)
	{
		for(YSSIZE_T i = 0; i < rgnList.GetN(); i++)
		{
			YsVec3 rect[4];
			if(fld->GetFieldRegionRect(rect, rgnList[i]) == YSOK)
			{
				for(int j = 0; j < 4; j++)
				{
					int next = (j + 1) % 4;
					int sx1 = (int)(cx + (rect[j].x() - playerPos.x()) * mag);
					int sy1 = (int)(cy - (rect[j].z() - playerPos.z()) * mag);
					int sx2 = (int)(cx + (rect[next].x() - playerPos.x()) * mag);
					int sy2 = (int)(cy - (rect[next].z() - playerPos.z()) * mag);
					DrawClippedLine(sx1, sy1, sx2, sy2, x1, y1, x2, y2, YsColor(60, 60, 80));
				}
			}
		}
	}

	if(fld->SearchFieldRegionById(rgnList, FS_RGNID_RUNWAY) == YSOK)
	{
		for(YSSIZE_T i = 0; i < rgnList.GetN(); i++)
		{
			YsVec3 rect[4];
			if(fld->GetFieldRegionRect(rect, rgnList[i]) == YSOK)
			{
				for(int j = 0; j < 4; j++)
				{
					int next = (j + 1) % 4;
					int sx1 = (int)(cx + (rect[j].x() - playerPos.x()) * mag);
					int sy1 = (int)(cy - (rect[j].z() - playerPos.z()) * mag);
					int sx2 = (int)(cx + (rect[next].x() - playerPos.x()) * mag);
					int sy2 = (int)(cy - (rect[next].z() - playerPos.z()) * mag);
					DrawClippedLine(sx1, sy1, sx2, sy2, x1, y1, x2, y2, YsWhite());
				}
			}
		}
	}

	for(int i = 0; i < sim->GetNumVOR(); i++)
	{
		const FsGround *vor = sim->GetVOR(i);
		if(vor == NULL)
			continue;
		int sx = (int)(cx + (vor->GetPosition().x() - playerPos.x()) * mag);
		int sy = (int)(cy - (vor->GetPosition().z() - playerPos.z()) * mag);
		YsVec2 sp(sx, sy);
		if(YsCheckInsideBoundingBox2(sp, w1, w2) != YSTRUE)
			continue;
		FsDrawCircle(sx, sy, 5, YsCyan(), YSFALSE);
		FsDrawPoint(sx, sy, YsCyan());
	}

	for(int i = 0; i < sim->GetNumNDB(); i++)
	{
		const FsGround *ndb = sim->GetNDB(i);
		if(ndb == NULL)
			continue;
		int sx = (int)(cx + (ndb->GetPosition().x() - playerPos.x()) * mag);
		int sy = (int)(cy - (ndb->GetPosition().z() - playerPos.z()) * mag);
		YsVec2 sp(sx, sy);
		if(YsCheckInsideBoundingBox2(sp, w1, w2) != YSTRUE)
			continue;
		FsDrawDiamond(sx, sy, 4, YsMagenta(), YSFALSE);
	}

	for(int i = 0; i < sim->GetNumILSFacility(); i++)
	{
		FsGround *ils = sim->GetILS(i);
		if(ils == NULL)
			continue;
		int sx = (int)(cx + (ils->GetPosition().x() - playerPos.x()) * mag);
		int sy = (int)(cy - (ils->GetPosition().z() - playerPos.z()) * mag);
		YsVec2 sp(sx, sy);
		if(YsCheckInsideBoundingBox2(sp, w1, w2) != YSTRUE)
			continue;
		FsDrawX(sx, sy, 3, YsYellow());
	}

	double arrowLen = 8.0;
	double tipDx = sin(playerHdg) * arrowLen;
	double tipDy = -cos(playerHdg) * arrowLen;
	double tailAng = YsPi * 5.0 / 6.0;
	double tailLen = arrowLen * 0.6;
	double lDx = sin(playerHdg - tailAng) * tailLen;
	double lDy = -cos(playerHdg - tailAng) * tailLen;
	double rDx = sin(playerHdg + tailAng) * tailLen;
	double rDy = -cos(playerHdg + tailAng) * tailLen;

	int tx = (int)(cx + tipDx), ty = (int)(cy + tipDy);
	int lx = (int)(cx + lDx), ly = (int)(cy + lDy);
	int rx = (int)(cx + rDx), ry = (int)(cy + rDy);

	FsDrawLine(tx, ty, lx, ly, YsGreen());
	FsDrawLine(tx, ty, rx, ry, YsGreen());
	FsDrawLine(lx, ly, rx, ry, YsGreen());

	int nax = (int)cx;
	int nay1 = y1 + 4;
	int nay2 = y1 + 14;
	FsDrawLine(nax, nay2, nax, nay1, YsCyan());
	FsDrawLine(nax, nay1, nax - 3, nay1 + 4, YsCyan());
	FsDrawLine(nax, nay1, nax + 3, nay1 + 4, YsCyan());
	FsDrawString(nax + 6, nay2, "N", YsCyan());

	double rangeInMiles = viewRange / 1609.34;
	char str[64];
	sprintf(str, "%.0fmi", rangeInMiles);
	FsDrawString(x1 + 4, y2 - 4, str, YsCyan());
}
