static const int sunnVtx=9;
static const int sunPlg = 8;
static double sunvtx[] = {
0.000,0.000,0.000,
0.000,0.000,2.280,
-1.612,0.000,1.612,
-2.280,0.000,-0.000,
-1.612,0.000,-1.612,
0.000,0.000,-2.280,
1.612,0.000,-1.612,
2.280,0.000,0.000,
1.612,0.000,1.612};

static int sunidx[] = {
    0, 2, 1,
    0, 3, 2,
    0, 4, 3,
    0, 5, 4,
    0, 6, 5,
    0, 7, 6,
    0, 8, 7,
    0, 1, 8
    };

static double sunnorm[]={
    -0.537,0.000,1.297,
    -1.297,0.000,0.537,
    -1.297,0.000,-0.537,
    -0.537,0.000,-1.297,
    0.537,0.000,-1.297,
    1.297,0.000,-0.537,
    1.297,0.000,0.537,
    0.537,0.000,1.297

};

YsArray<YsShellVertexHandle> vtHdList;
YsShellExt shell;
YsShellPolygonHandle plHd;
YsShellVertexHandle vtHd;

for(int i=0; i<sunnVtx; i++)
{
    YsVec3 pos;
    pos.Set(sunvtx[3*i],sunvtx[3*i+1],sunvtx[3*i+2]);

    vtHd=shell.AddVertex(pos);
    vtHdList.Append(vtHd);
}

for(int i=0; i<sunPlg; i++)
{
    YsVec3 plNormal;
    YsShellPolygonHandle plHd;
    YsShellVertexHandle plVtHd[3];
    plNormal.Set(sunnorm[3*i],sunnorm[3*i+1],sunnorm[3*i+2]);
    plVtHd[0] = vtHdList[sunidx[3*i]];
    plVtHd[1] = vtHdList[sunidx[3*i+1]];
    plVtHd[2] = vtHdList[sunidx[3*i+2]];
    plHd = shell.AddPolygon(3,plVtHd);
    shell.SetPolygonNormal(plHd,plNormal);
    YsColor col;
    col.SetIntRGBA(255,255,255,255);
    shell.SetPolygonColor(plHd,col);
};



