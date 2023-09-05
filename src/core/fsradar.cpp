#include "fsradar.h"
#include "graphics/common/fsopengl.h"



////////////////////////////////////////////////////////////

// Ground -> CROSS
// Air -> RECT
// Friendly -> White
// Hostile -> Green

void FsHorizontalRadar::Draw(
	 const FsSimulation *sim,int x1,int y1,int x2,int y2,const double &rangeInX,const FsAirplane &withRespectTo,
	 int mode,const double &airAltLimit) const
	// mode 0:air to air    1: air to gnd    2:bombing
{
	const double mag=double(YsAbs(x2-x1))/(rangeInX*1852); // Conversion from meters to miles
	const int mkSize=3;


	YsVec2 w1,w2,wc;
	w1.Set(x1,y1);
	w2.Set(x2,y2);
	if(mode==0)
	{
		wc=(w1+w2)/2.0;
	}
	else
	{
		wc.Set((w1.x()+w2.x())/2.0,(w1.y()+w2.y()*3.0)/4.0);
	}


	YsAtt3 attH;
	attH=withRespectTo.GetAttitude();
	attH.SetP(0.0);
	attH.SetB(0.0);


	YsMatrix4x4 ref;
	ref.Translate(withRespectTo.GetPosition());
	ref.Rotate(attH);

	DrawBasic(sim,x1,y1,x2,y2,rangeInX,withRespectTo,withRespectTo.GetPosition(),withRespectTo.GetAttitude(),mode,airAltLimit);

	switch(mode)
	{
	case 0:
	case 1:
		{
			double radar,dy,dx;
			YsVec2 wr;
			if(mode==0)
			{
				radar=withRespectTo.Prop().GetAAMRadarAngle();
			}
			else
			{
				radar=withRespectTo.Prop().GetAGMRadarAngle();
			}
			dy=double(YsAbs(y2-y1))/2.0;
			dx=dy*sin(radar);
			wr.Set(wc.x()-dx,w1.y());
			const int x =(int)wc.x();
			const int y =(int)wc.y();
			int xx=(int)wr.x();
			int yy=(int)wr.y();
			FsDrawLine(x - 10,y - 10,xx,yy,YsGreen());
			wr.Set(wc.x()+dx,w1.y());
			xx=(int)wr.x();
			yy=(int)wr.y();
			FsDrawLine(x + 10,y - 10,xx,yy,YsGreen());
		}
		break;
	case 2:
		{
			YsVec3 bomb;
			if(withRespectTo.Prop().ComputeEstimatedBombLandingPosition(bomb,sim->GetWeather())==YSOK)
			{
				YsVec2 prj;

				ref.MulInverse(bomb,bomb,1.0);
				bomb*=mag;
				prj.Set(bomb.x(),-bomb.z());
				prj+=wc;

				const int x=(int)prj.x();
				const int y=(int)prj.y();
				if(YsCheckInsideBoundingBox2(prj,w1,w2)==YSTRUE)
				{
					FsDrawDiamond(x,y,mkSize,YsGreen(),YSFALSE);
				}

				FsDrawLine((int)wc.x()-3,y1,(int)wc.x()-3,y2,YsGreen());
				FsDrawLine((int)wc.x()+3,y1,(int)wc.x()+3,y2,YsGreen());
			}
		}
		break;
	}
}

void FsHorizontalRadar::DrawBasic(
    const class FsSimulation *sim,int x1,int y1,int x2,int y2,const double &rangeInX,
    const FsExistence &withRespectTo,const YsVec3 &pos,const YsAtt3 &att,
    int mode,const double &airAltLimit) const
{
	const double mag=double(YsAbs(x2-x1))/(rangeInX*1852); // Conversion from meters to n miles
	int x,y,xx,yy;
	YsAtt3 attH; //radar heading
	YsMatrix4x4 ref; //4x4 transform matrix of aircraft
	YsVec2 w1,w2,wc; //w1: upper left coordinate, wc: center coordinate, w2: lower right coordinate
	YsColor col;
	const int mkSize=3;

	//draw radar bounding box
	FsDrawRect(x1,y1,x2,y2,YsGreen(),YSFALSE);
	printf("x1,y1,x2,y2: %d,%d,%d,%d\n", x1, y1, x2, y2);
	//draw radar range string
	char str[256];
	sprintf(str,"%d NM",int(rangeInX));
	FsDrawString(x1+8,y1+24,str,YsGreen());

	//get heading
	attH=att;
	attH.SetP(0.0);
	attH.SetB(0.0);

	//init 4x4 transform matrix
	ref.Translate(pos);
	ref.Rotate(attH);

	//set w1 and w2
	w1.Set(x1,y1);
	w2.Set(x2,y2);

	//set wc based on mode (center of screen or bottom-quarter-center)
	if(mode==0)
	{
		wc=(w1+w2)/2.0;
	}
	else
	{
		wc.Set((w1.x()+w2.x())/2.0,(w1.y()+w2.y()*3.0)/4.0);
	}

	//draw aircraft indicator centered at wc
	x=(int)wc.x();
	y=(int)wc.y();
	FsDrawLine(x - 6, y - 5, x + 6, y - 5,YsGreen());
	FsDrawLine(x  ,y-8,x  ,y+8,YsGreen());
	FsDrawLine(x - 4, y + 5, x + 4, y + 5, YsGreen());

	//draw indicators for ground objects (cross)
	const FsGround *gnd;
	gnd=NULL;
	while((gnd=sim->FindNextGround(gnd))!=NULL)
	{
		if(gnd->IsAlive()==YSTRUE && gnd->Prop().IsNonGameObject()!=YSTRUE)
		{
			YsVec3 pos;
			YsVec2 prj;

			ref.MulInverse(pos,gnd->GetPosition(),1.0);

			pos*=mag;

			prj.Set(pos.x(),-pos.z());
			prj+=wc;

			if(YsCheckInsideBoundingBox2(prj,w1,w2)==YSTRUE)
			{
				if(withRespectTo.iff==gnd->iff)
				{
					col=YsWhite();
				}
				else
				{
					col=YsGreen();
				}

				FsDrawX((int)prj.x(),(int)prj.y(),mkSize,col);
			}
		}
	}

	//draw aircraft indicators
	const FsAirplane *air;
	air=NULL;
	while((air=sim->FindNextAirplane(air))!=NULL)
	{
		if(air!=&withRespectTo && air->IsAlive()==YSTRUE)
		{
			double altLimit;
			altLimit=airAltLimit+1000.0*(1.0-air->Prop().GetRadarCrossSection());
			if(altLimit<air->GetPosition().y())
			{

				YsVec3 pos,vel;
				YsVec2 prj1,prj2;

				air->Prop().GetVelocity(vel);

				//get relative position and velocity
				ref.MulInverse(pos,air->GetPosition(),1.0);
				ref.MulInverse(vel,vel,0.0);

				//scale velocity by 12
				if(vel.Normalize()==YSOK)
				{
					vel*=12.0;
				}

				//scale position to radar scale
				pos*=mag;
				vel+=pos;

				//invert Z for proper Y position in radar coordinate system
				prj1.Set(pos.x(),-pos.z());
				prj2.Set(vel.x(),-vel.z());

				prj1+=wc;
				prj2+=wc;

				if(YsCheckInsideBoundingBox2(prj1,w1,w2)==YSTRUE)
				{
					if(air->Prop().IsActive()!=YSTRUE)
					{
						col=YsBlack();
					}
					else if(withRespectTo.iff==air->iff)
					{
						col=YsWhite();
					}
					else
					{
						col=YsGreen();
					}

					//draw rectangle for each plane
					x=(int)prj1.x();
					y=(int)prj1.y();
					FsDrawDiamond(x, y, 5, col, YSFALSE);

					//draw heading line for each plane (normalized and scaled velocity vector)
					if(YsCheckInsideBoundingBox2(prj2,w1,w2)==YSTRUE)
					{
						xx=(int)prj2.x();
						yy=(int)prj2.y();
						FsDrawLine(x,y,xx,yy,col);
					}

					//if the plane is locked onto us, draw a yellow line to indicate
					if (air->Prop().GetAirTargetKey() == FsExistence::GetSearchKey(&withRespectTo))
					{
						FsDrawLine((int)wc.x(), (int)wc.y(), (int)prj1.x(), (int)prj1.y(), YsYellow());
					}
				}
			}
		}
	}

	const FsWeapon *wpn;
	wpn=NULL;
	while((wpn=sim->FindNextActiveWeapon(wpn))!=NULL)
	{
		if(wpn->lifeRemain>YsTolerance && wpn->timeRemain>YsTolerance)
		{
			if(wpn->type==FSWEAPON_AIM9 || wpn->type==FSWEAPON_AIM120 || wpn->type == FSWEAPON_AIM9X)
			{
				col=YsRed();
			}
			else if(wpn->type==FSWEAPON_AGM65 || wpn->type==FSWEAPON_ROCKET)
			{
				col=YsYellow();
			}
			else
			{
				continue;
			}

			YsVec3 pos;
			YsVec2 prj;

			ref.MulInverse(pos,wpn->pos,1.0);

			pos*=mag;

			prj.Set(pos.x(),-pos.z());

			prj+=wc;

			if(YsCheckInsideBoundingBox2(prj,w1,w2)==YSTRUE)
			{
				//FsDrawPoint2Pix((int)prj.x(),(int)prj.y(),col);
				FsDrawCircle((int)prj.x(), (int)prj.y(), mkSize, col, YSTRUE);
			}
		}
	}
}

void FsHorizontalRadar::DrawCircular(const FsSimulation* sim, int x, int y, int radius, const double& rangeInX, const FsAirplane& withRespectTo, int mode, const double& airAltLimit)
{
	const int mkSize = 3;

	YsAtt3 attH;
	attH = withRespectTo.GetAttitude();
	attH.SetP(0.0);
	attH.SetB(0.0);

	YsMatrix4x4 ref;
	ref.Translate(withRespectTo.GetPosition());
	ref.Rotate(attH);

	YsVec2 wc;
	if (mode == 0)
	{
		wc.Set((double)x, (double)y);
	}
	else
	{
		wc.Set((double)x, (double)y + double(radius) / 2.0);
	}

	//draw radar range string
	char str[256];
	sprintf(str, "%d NM", int(rangeInX));
	FsDrawString(x - radius * 0.9, y - radius * 0.9, str, YsGreen());

	//draw radar circle
	FsDrawCircle(x, y, radius, YsGreen(), YSFALSE);
	const double mag = double(radius) * 2.0 / (rangeInX * 1852); // Conversion from meters to n miles

	//draw aircraft indicator centered at wc
	int centerX = (int)wc.x();
	int centerY = (int)wc.y();
	FsDrawLine(centerX - 6, centerY - 5, centerX + 6, centerY - 5, YsGreen());
	FsDrawLine(centerX, centerY - 8, centerX, centerY + 8, YsGreen());
	FsDrawLine(centerX - 4, centerY + 5, centerX + 4, centerY + 5, YsGreen());


	switch (mode)
	{
		case 0:
		case 1:
		{
			double radarLineStartOffset = 0.05;
			double radar;
			double lineLength;
			if (mode == 0)
			{
				radar = withRespectTo.Prop().GetAAMRadarAngle();
				lineLength = radius;
			}
			else
			{
				radar = withRespectTo.Prop().GetAGMRadarAngle();
				lineLength = (double)radius * (1.5 - radarLineStartOffset);
			}

			//draw radar lines
			FsDrawLine(centerX + lineLength * radarLineStartOffset * sin(radar), centerY - lineLength * radarLineStartOffset * cos(radar), centerX + lineLength * sin(radar), centerY - lineLength * cos(radar), YsGreen());
			FsDrawLine(centerX - lineLength * radarLineStartOffset * sin(radar), centerY - lineLength * radarLineStartOffset * cos(radar), centerX - lineLength * sin(radar), centerY - lineLength * cos(radar), YsGreen());

		}
		break;
		case 2:
		{
			YsVec3 bomb;
			if (withRespectTo.Prop().ComputeEstimatedBombLandingPosition(bomb, sim->GetWeather()) == YSOK)
			{
				YsVec2 prj;

				ref.MulInverse(bomb, bomb, 1.0);
				bomb *= mag;
				prj.Set(bomb.x(), -bomb.z());
				prj += wc;

				//draw bomb estimated impact point
				if (IsInsideCircle(prj, YsVec2(x, y), radius) == YSTRUE)
				{
					FsDrawCircle((int)prj.x(), (int)prj.y(), mkSize * 2, YsGreen(), YSFALSE);
				}

				FsDrawLine((int)wc.x(), (int)wc.y(), (int)prj.x(), (int)prj.y(), YsGreen());
			}
		}
		break;
	}

	//draw indicators for ground objects (cross)
	const FsGround* gnd;
	gnd = NULL;
	while ((gnd = sim->FindNextGround(gnd)) != NULL)
	{
		if (gnd->IsAlive() == YSTRUE && gnd->Prop().IsNonGameObject() != YSTRUE)
		{
			YsVec3 pos;
			YsVec2 prj;

			ref.MulInverse(pos, gnd->GetPosition(), 1.0);

			pos *= mag;

			prj.Set(pos.x(), -pos.z());
			prj += wc;

			YsColor col;
			if (IsInsideCircle(prj, YsVec2(x, y), radius) == YSTRUE)
			{
				if (withRespectTo.iff == gnd->iff)
				{
					col = YsWhite();
				}
				else
				{
					col = YsGreen();
				}

				FsDrawX((int)prj.x(), (int)prj.y(), mkSize, col);
			}
		}
	}

	//draw aircraft indicators
	const FsAirplane* air;
	air = NULL;
	while ((air = sim->FindNextAirplane(air)) != NULL)
	{
		if (air != &withRespectTo && air->IsAlive() == YSTRUE)
		{
			double altLimit;
			altLimit = airAltLimit + 1000.0 * (1.0 - air->Prop().GetRadarCrossSection());
			if (altLimit < air->GetPosition().y())
			{

				YsVec3 pos, vel;
				YsVec2 prj1, prj2;

				air->Prop().GetVelocity(vel);

				//get relative position and velocity
				ref.MulInverse(pos, air->GetPosition(), 1.0);
				ref.MulInverse(vel, vel, 0.0);

				//scale velocity by 12
				if (vel.Normalize() == YSOK)
				{
					vel *= 12.0;
				}

				//scale position to radar scale
				pos *= mag;
				vel += pos;

				//invert Z for proper Y position in radar coordinate system
				prj1.Set(pos.x(), -pos.z());
				prj2.Set(vel.x(), -vel.z());

				prj1 += wc;
				prj2 += wc;

				YsColor col;
				if (IsInsideCircle(prj1, YsVec2(x, y), radius) == YSTRUE)
				{
					if (air->Prop().IsActive() != YSTRUE)
					{
						col = YsBlack();
					}
					else if (withRespectTo.iff == air->iff)
					{
						col = YsWhite();
					}
					else
					{
						col = YsGreen();
					}

					//draw rectangle for each plane
					FsDrawDiamond((int)prj1.x(), (int)prj1.y(), 5, col, YSFALSE);

					//draw heading line for each plane (normalized and scaled velocity vector)
					if (IsInsideCircle(prj2, YsVec2(x, y), radius) == YSTRUE)
					{
						FsDrawLine((int)prj1.x(), (int)prj1.y(), (int)prj2.x(), (int)prj2.y(), col);
					}

					//if the plane is locked onto us, draw a yellow line to indicate
					if (air->Prop().GetAirTargetKey() == FsExistence::GetSearchKey(&withRespectTo))
					{
						FsDrawLine((int)wc.x(), (int)wc.y(), (int)prj1.x(), (int)prj1.y(), YsYellow());
					}

					//if we're locked onto this plane, draw a green line to indicate
					if (withRespectTo.Prop().GetAirTargetKey() == FsExistence::GetSearchKey(air))
					{
						FsDrawLine((int)wc.x(), (int)wc.y(), (int)prj1.x(), (int)prj1.y(), YsGreen());
					}
				}
			}
		}
	}

	//draw active weapons
	const FsWeapon* wpn;
	wpn = NULL;
	while ((wpn = sim->FindNextActiveWeapon(wpn)) != NULL)
	{
		YsColor col;
		if (wpn->lifeRemain > YsTolerance && wpn->timeRemain > YsTolerance)
		{
			if (wpn->type == FSWEAPON_AIM9 || wpn->type == FSWEAPON_AIM120 || wpn->type == FSWEAPON_AIM9X)
			{
				col = YsRed();
			}
			else if (wpn->type == FSWEAPON_AGM65 || wpn->type == FSWEAPON_ROCKET)
			{
				col = YsYellow();
			}
			else
			{
				continue;
			}

			YsVec3 pos;
			YsVec2 prj;

			ref.MulInverse(pos, wpn->pos, 1.0);

			pos *= mag;

			prj.Set(pos.x(), -pos.z());

			prj += wc;

			if (IsInsideCircle(prj, YsVec2(x, y), radius) == YSTRUE)
			{
				FsDrawCircle((int)prj.x(), (int)prj.y(), mkSize, col, YSTRUE);
			}
		}
	}

}

YSBOOL FsHorizontalRadar::IsInsideCircle(YsVec2 point, YsVec2 circleCenter, int radius)
{
	point -= circleCenter;
	if (point.GetSquareLength() > radius * radius)
	{
		return YSFALSE;
	}
	return YSTRUE;
}
