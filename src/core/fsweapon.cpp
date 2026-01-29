#include <ysclass.h>
#include <ysglparticlemanager.h>

#include <fsairproperty.h>

#include "fs.h"
#include "fsnetutil.h"
#include "graphics/common/fsopengl.h"
#include "fsinstreading.h"
#include "fsweapon.h"


////////////////////////////////////////////////////////////

int FsKillCredit::GetAntiAirScore(YsString &msg,const FsSimulation *,const FsExistence *player) const
{
	msg.Set("");
	if(NULL==whoKilled || NULL==whom)
	{
		return 0;
	}

	YSBOOL playerResponsible=YSFALSE;
	if(creditOwner==FSWEAPON_CREDIT_OWNER_PLAYER)
	{
		playerResponsible=YSTRUE;
	}
	else if(creditOwner==FSWEAPON_CREDIT_OWNER_NOT_RECORDED && player==whoKilled)
	{
		playerResponsible=YSTRUE;
	}

	FsAirplane *victim=(FsAirplane *)whom;
	if(YSTRUE==playerResponsible && FSEX_AIRPLANE==victim->GetType())
	{
		if(whoKilled->iff==whom->iff)
		{
			msg.Set("Killed Friendly Airplane   -100");
			return -100;   // You killed your friendly!
		}
		else
		{
			unsigned score=0;

			msg.Append("Killed ");
			msg.Append(victim->Prop().GetIdentifier());

			if(FSEX_AIRPLANE==whoKilled->GetType())
			{
				const double alt=where.y();
				if(alt<909.1)  // 3300ft or less
				{
					msg.Append("@Low Altitude(10) ");
					score=10;
				}
				else if(alt>3300.0)
				{
					msg.Append("@High Altitude(30) ");
					score=30;
				}
				else
				{
					msg.Append("@Mid Altitude(20) ");
					score=20;
				}
			}
			else
			{
				switch(victim->Prop().GetAirplaneCategory())
				{
				default:
					msg.Append("Non-Combat Airplane(5) ");
					score=5;
					break;
				case FSAC_TRAINER:
					msg.Append("Trainer (10) ");
					score=10;
					break;
				case FSAC_FIGHTER:
				case FSAC_WW2FIGHTER:
					msg.Append("Fighter (20) ");
					score=20;
					break;
				case FSAC_ATTACKER:
				case FSAC_WW2ATTACKER:
				case FSAC_WW2DIVEBOMBER:
					msg.Append("Fighter (30) ");
					score=30;
					break;
				case FSAC_HEAVYBOMBER:
				case FSAC_WW2BOMBER:
					msg.Append("Bomber (40) ");
					score=40;
					break;
				}
			}

			switch(byWhatKindOfWeapon)
			{
			case FSWEAPON_GUN:
				msg.Append(" By GUN (Times 1.5) ");
				score=score*3/2;
				break;
			case FSWEAPON_AGM65:
				msg.Append(" By AGM (Times 2.0) ");
				score=score*2;
				break;
			case FSWEAPON_BOMB:
			case FSWEAPON_BOMB250:
			case FSWEAPON_BOMB500HD:
				msg.Append(" By Bomb (Times 1.2) ");
				score=score*6/5;
				break;
			case FSWEAPON_AIM9:
			case FSWEAPON_AIM9X:
			case FSWEAPON_AIM120:
				msg.Append(" By AAM (Times 1.0) ");
				break;
			case FSWEAPON_ROCKET:
				msg.Append(" By Rocket (Times 2.0) ");
				score=score*2;
				break;
			default:
				msg.Append(" By ? (Times 1.0) ");
				break;
			}

			char sco[256];
			sprintf(sco,"= %d",score);
			msg.Append(sco);

			return score;
		}
	}
	return 0;
}

int FsKillCredit::GetAntiGroundScore(YsString &msg,const FsSimulation *,const FsExistence *player) const
{
	msg.Set("");
	if(NULL==whoKilled || NULL==whom)
	{
		return 0;
	}

	YSBOOL playerResponsible=YSFALSE;
	if(creditOwner==FSWEAPON_CREDIT_OWNER_PLAYER)
	{
		playerResponsible=YSTRUE;
	}
	else if(creditOwner==FSWEAPON_CREDIT_OWNER_NOT_RECORDED && player==whoKilled)
	{
		playerResponsible=YSTRUE;
	}

	const FsGround *victim=(FsGround *)whom;
	if(YSTRUE==playerResponsible && FSEX_GROUND==victim->GetType())
	{
		if(whoKilled->iff==whom->iff)
		{
			msg.Append("Destroyed Friendly Facility    -200");
			return -200;   // You killed your friendly!
		}
		else
		{
			unsigned score=0;

			msg.Append("Destroyed ");
			msg.Append(victim->Prop().GetIdentifier());


			const double rad=whom->GetApproximatedCollideRadius();
			if(rad<20.0)
			{
				msg.Append("(5) ");
				score=5;
			}
			else if(rad<100.0)
			{
				msg.Append("(10) ");
				score=10;
			}
			else
			{
				msg.Append("(20) ");
				score=20;
			}

			switch(byWhatKindOfWeapon)
			{
			case FSWEAPON_GUN:
				msg.Append(" By GUN (Times 1.0) ");
				break;
			case FSWEAPON_AGM65:
				msg.Append(" By AGM (Times 1.2) ");
				score=score*6/5;
				break;
			case FSWEAPON_BOMB:
			case FSWEAPON_BOMB250:
			case FSWEAPON_BOMB500HD:
				msg.Append(" By Bomb (Times 1.5) ");
				score=score*3/2;
				break;
			case FSWEAPON_ROCKET:
				msg.Append(" By Rocket (Times 1.5) ");
				score=score*3/2;
				break;
			case FSWEAPON_AIM9:
			case FSWEAPON_AIM9X:
			case FSWEAPON_AIM120:
				msg.Append(" By AAM (Times 1.0) ");
				score=score*6/5;
				break;

			default:
				msg.Append(" By ? (Times 1.0) ");
				break;
			}

			char sco[256];
			sprintf(sco,"=%d",score);
			msg.Append(sco);

			return score;
		}
	}
	return 0;
}


////////////////////////////////////////////////////////////

void FsWeaponSmokeTrail::Initialize(void)
{
	trailBegin=0;
	trailUsed=0;
	trailNextRecord=double(TIMEPERSEG)/1000.0;
}

void FsWeaponSmokeTrail::MakeVertexArray(
    YsGLVertexBuffer vtxBuf[4],YsGLNormalBuffer nomBuf[4],YsGLColorBuffer colBuf[4],
    const YsVec3 &pos,const YsAtt3 &att,FSSMOKETYPE smk,const double &cTime) const
{
	int a;
	double w;
	YsVec3 v,v1,v2,n,n1,n2,refV1,refV2,prev;
	int iAlpha;

	for(int i=0; i<4; ++i)
	{
		vtxBuf[i].CleanUp();
		nomBuf[i].CleanUp();
		colBuf[i].CleanUp();
	}

	switch(smk)
	{
	default:
		break;
	case FSSMKSOLID:
		refV1.Set(-1.0, 0.0,0.0);
		refV2.Set( 0.0, 1.0,0.0);
		for(a=0; a<4; a++)
		{
			prev=pos;
			if(trailUsed>0 && prev!=trailPos[(trailBegin+trailUsed-1)%MAXNUMTRAIL])
			{
				v1=refV1*0.1;
				v1.SetZ(-1.5);
				v2=refV2*0.1;
				v2.SetZ(-1.5);
				att.Mul(v1,v1);
				v1+=pos;
				att.Mul(v2,v2);
				v2+=pos;

				att.Mul(n1,refV1);
				att.Mul(n2,refV2);

				colBuf[a].Add<float>(1.0f,1.0f,1.0f,0.7f);
				nomBuf[a].Add(n2);
				vtxBuf[a].Add(v2);

				colBuf[a].Add<float>(1.0f,1.0f,1.0f,0.7f);
				nomBuf[a].Add(n1);
				vtxBuf[a].Add(v1);
			}

			iAlpha=MAXNUMTRAIL-1;
			for(int i=trailUsed-1; i>=0; i--)
			{
				iAlpha--;

				const int idx=(trailBegin+i)%MAXNUMTRAIL;

				if(prev==trailPos[idx])
				{
					continue;
				}
				prev=trailPos[(trailBegin+i)%MAXNUMTRAIL];

				float alpha=1.0f-(float)(cTime-trailTime[idx])/(float)((TIMEPERSEG*MAXNUMTRAIL-500)/1000);
				alpha=YsBound(alpha,0.0f,0.7f);

				w=1.0*double(trailUsed-i);
				w=YsSmaller(w,2.5);

				/* Known working set, looks to me an error. >>
				v1=refV1*w; // v1.Set(-w,0.0,0.0);
				v2=refV2*w; // v2.Set( w,0.0,0.0);
				trailAtt[idx].Mul(v1,v1);
				v1+=trailPos[idx];
				trailAtt[idx].Mul(v2,v2);
				v2+=trailPos[idx];

				att.Mul(n1,refV1); // att.Mul(n,YsYVec());
				att.Mul(n2,refV2); */

				trailAtt[idx].Mul(n1,refV1);
				trailAtt[idx].Mul(n2,refV2);
				v1=trailPos[idx]+n1*w;
				v2=trailPos[idx]+n2*w;

				colBuf[a].Add<float>(1.0f,1.0f,1.0f,alpha);
				nomBuf[a].Add(n2);
				vtxBuf[a].Add(v2);

				colBuf[a].Add<float>(1.0f,1.0f,1.0f,alpha);
				nomBuf[a].Add(n1);
				vtxBuf[a].Add(v1);
			}

			double xx,yy;
			xx=refV1.x();
			yy=refV1.y();
			refV1.Set(yy,-xx,0.0);
			xx=refV2.x();
			yy=refV2.y();
			refV2.Set(yy,-xx,0.0);
		}
		break;

	case FSSMKNOODLE:
	case FSSMKCIRCLE:
	case FSSMKTOWEL:
		prev=pos;
		if(trailUsed>0 && prev!=trailPos[(trailBegin+trailUsed-1)%MAXNUMTRAIL])
		{
			v1.Set(-0.1,0.0,-1.5);
			v2.Set( 0.1,0.0,-1.5);
			att.Mul(v1,v1);
			v1+=pos;
			att.Mul(v2,v2);
			v2+=pos;
			att.Mul(n,YsYVec());

			colBuf[0].Add<float>(1.0f,1.0f,1.0f,0.7f);
			nomBuf[0].Add(n);
			vtxBuf[0].Add(v2);

			colBuf[0].Add<float>(1.0f,1.0f,1.0f,0.7f);
			nomBuf[0].Add(n);
			vtxBuf[0].Add(v1);
		}

		iAlpha=MAXNUMTRAIL-1;
		for(int i=trailUsed-1; i>=0; i--)
		{
			iAlpha--;

			const int idx=(trailBegin+i)%MAXNUMTRAIL;

			if(prev==trailPos[idx])
			{
				continue;
			}
			prev=trailPos[(trailBegin+i)%MAXNUMTRAIL];

			float alpha=1.0f-(float)(cTime-trailTime[idx])/(float)((TIMEPERSEG*MAXNUMTRAIL-500)/1000);
			alpha=YsBound(alpha,0.0f,0.7f);

			w=1.0*double(trailUsed-i);
			w=YsSmaller(w,2.5);
			v1.Set(-w,0.0,0.0);
			v2.Set( w,0.0,0.0);
			trailAtt[idx].Mul(v1,v1);
			v1+=trailPos[idx];
			trailAtt[idx].Mul(v2,v2);
			v2+=trailPos[idx];
			att.Mul(n,YsYVec());

			colBuf[0].Add<float>(1.0f,1.0f,1.0f,alpha);
			nomBuf[0].Add(n);
			vtxBuf[0].Add(v2);

			colBuf[0].Add<float>(1.0f,1.0f,1.0f,alpha);
			nomBuf[0].Add(n);
			vtxBuf[0].Add(v1);
		}
		break;
	}
}

// The following two functions are moved to fsweapongl.cpp and fsweaponbi.cpp
// void FsWeaponSmokeTrail::Draw(const YsVec3 &pos,const YsAtt3 &att)
// void FsWeaponSmokeTrail::DrawFlare(const YsVec3 &pos,const YsAtt3 &att)

void FsWeaponSmokeTrail::Add(const double &dt,const double &cTime,const YsVec3 &pos,const YsAtt3 &att)
{
	trailNextRecord-=dt;
	if(trailNextRecord<=0.0)
	{
		trailNextRecord=double(TIMEPERSEG)/1000.0;
		trailPos[(trailBegin+trailUsed)%MAXNUMTRAIL]=pos;
		trailAtt[(trailBegin+trailUsed)%MAXNUMTRAIL]=att;
		trailTime[(trailBegin+trailUsed)%MAXNUMTRAIL]=cTime;
		if(trailUsed<MAXNUMTRAIL)
		{
			trailUsed++;
		}
		else
		{
			trailBegin=(trailBegin+1)%MAXNUMTRAIL;
		}
	}
}

void FsWeaponSmokeTrail::AddToParticleManagerAsFlare(class YsGLParticleManager &partMan,const YsVec3 cPos,const double cTime,YSBOOL includeCurrentPos, FSENVIRONMENT env)
{
	int c=0;
	YSSIZE_T i0=trailUsed;
	if(YSTRUE!=includeCurrentPos)
	{
		--i0;
	}
	for(YSSIZE_T i=i0; i>0; i--)
	{
		const int idx0=(trailBegin+i)%MAXNUMTRAIL;
		const int idx1=(trailBegin+i-1)%MAXNUMTRAIL;

		const double t0=(i==trailUsed ? 0.0 : cTime-trailTime[idx0]);
		const double t1=cTime-trailTime[idx1];

		const auto p0=(i==trailUsed ? cPos : trailPos[idx0]);
		const auto p1=trailPos[idx1];

		float sp=0.5f+(float)(t0*4.0);
		if(16.0f<sp)
		{
			sp=16.0f;
		}

		int nDiv=3+(int)((p0 - p1).GetLength() /sp);
		for(int i=0; i<nDiv; ++i)
		{
			const double param=(double)i/(double)nDiv;

			const double t=t0*(1.0-param)+t1*param;
			const auto pos=p0*(1.0-param)+p1*param;
			double dist = (cPos - pos).GetLength();

			YsColor col;
			double r,g,b,a;
			
			YsColor whiteHot, yellowHot, orangeHot, litSmoke, greySmoke, whiteSmoke;
			float tWhite, tYellow, tOrange, tLit, tGrey, tEnd;
			float dWhite, dYellow, dOrange, dLit;

			if (env == FSNIGHT)
			{
				whiteHot.SetFloatRGBA(1.0,0.99,0.93,1.0);
				yellowHot.SetFloatRGBA(0.8,0.74,0.52,0.95);
				orangeHot.SetFloatRGBA(0.8,0.7,0.5,0.9);
				litSmoke.SetFloatRGBA(0.5, 0.5, 0.5,0.8);
				greySmoke.SetFloatRGBA(0.15,0.15,0.15,0.5);
				whiteSmoke.SetFloatRGBA(0.1,0.1,0.1,0.25);
				tWhite = 0.05;
				tYellow = 0.08;
				tOrange = 0.11;
				tLit = 0.5;
				tGrey = 1.0;
				tEnd = 6.0;
				dWhite = 2.0;
				dYellow = 3.0;
				dOrange = 4.0;
				dLit = 15.0;
			}
			else
			{
				whiteHot.SetFloatRGBA(1.0,0.99,0.93,1.0);
				yellowHot.SetFloatRGBA(1.0,0.92,0.65,0.95);
				orangeHot.SetFloatRGBA(0.7,0.63,0.5,0.9);
				litSmoke.SetFloatRGBA(0.65, 0.65, 0.65,0.8);
				greySmoke.SetFloatRGBA(0.75, 0.75, 0.75,0.5);
				whiteSmoke.SetFloatRGBA(0.8, 0.8, 0.8,0.25);
				tWhite = 0.05;
				tYellow = 0.08;
				tOrange = 0.11;
				tLit = 0.5;
				tGrey = 1.0;
				tEnd = 6.0;
				dWhite = 1.0;
				dYellow = 2.0;
				dOrange = 3.0;
				dLit = 5.0;
			}
			//x0 – (x0-x1) * (t-t0)/dt
			if (dist < dWhite)
			{
				r = whiteHot.Rf() - (whiteHot.Rf() - yellowHot.Rf()) * dist / dWhite;
				g = whiteHot.Gf() - (whiteHot.Gf() - yellowHot.Gf()) * dist / dWhite;
				b = whiteHot.Bf() - (whiteHot.Bf() - yellowHot.Bf()) * dist / dWhite;
			}
			else if (dist < dYellow)
			{
				r = yellowHot.Rf() - (yellowHot.Rf() - orangeHot.Rf()) * (dist - dWhite) / (dYellow - dWhite);
				g = yellowHot.Gf() - (yellowHot.Gf() - orangeHot.Gf()) * (dist - dWhite) / (dYellow - dWhite);
				b = yellowHot.Bf() - (yellowHot.Bf() - orangeHot.Bf()) * (dist - dWhite) / (dYellow - dWhite);
			}
			else if (dist < dOrange)
			{
				r = orangeHot.Rf() - (orangeHot.Rf() - litSmoke.Rf()) * (dist - dYellow) / (dOrange - dYellow);
				g = orangeHot.Gf() - (orangeHot.Gf() - litSmoke.Gf()) * (dist - dYellow) / (dOrange - dYellow);
				b = orangeHot.Bf() - (orangeHot.Bf() - litSmoke.Bf()) * (dist - dYellow) / (dOrange - dYellow);
			}
			else if (dist < dLit)
			{
				r = litSmoke.Rf() - (litSmoke.Rf() - greySmoke.Rf()) * (dist - dOrange) / (dLit - dOrange);
				g = litSmoke.Gf() - (litSmoke.Gf() - greySmoke.Gf()) * (dist - dOrange) / (dLit - dOrange);
				b = litSmoke.Bf() - (litSmoke.Bf() - greySmoke.Bf()) * (dist - dOrange) / (dLit - dOrange);
			}
			else if (t < tLit)
			{
				r = greySmoke.Rf();
				g = greySmoke.Gf();
				b = greySmoke.Bf();
			}
			else if (t < tGrey)
			{
				r = greySmoke.Rf() - (greySmoke.Rf() - whiteSmoke.Rf()) * (t - tLit) / (tGrey - tLit);
				g = greySmoke.Gf() - (greySmoke.Gf() - whiteSmoke.Gf()) * (t - tLit) / (tGrey - tLit);
				b = greySmoke.Bf() - (greySmoke.Bf() - whiteSmoke.Bf()) * (t - tLit) / (tGrey - tLit);
			}
			else if (t < tEnd)
			{
				r = whiteSmoke.Rf();
				g = whiteSmoke.Gf();
				b = whiteSmoke.Bf();
			}
			else
			{
				r = 1.0;
				g = 0.0;
				b = 0.0;
			}

			int exp = 3;
			int offset = 0.005;
			a = YsBound((1/pow(tEnd,exp-1)*pow(tEnd-t,exp))/tEnd + offset, 0.0, 1.0);

			float sz;// = 0.75f + (float)(t * 4.0);
			sz = 0.75 + (31 * (t / tEnd));
			if (32.0f < sz)
			{
				sz = 32.0f;
			}

			col.SetDoubleRGBA(r, g, b, a);

			float s=(float)((i+idx0)&7)*0.125;
			partMan.Add(pos,col,sz,s,0);
		}
		c++;
	}
}

void FsWeaponSmokeTrail::AddToParticleManager(class YsGLParticleManager &partMan,const YsVec3 cPos,const double cTime,YSBOOL includeCurrentPos, FSENVIRONMENT env)
{
	int c=0;
	YSSIZE_T i0=trailUsed;
	if(YSTRUE!=includeCurrentPos)
	{
		--i0;
	}
	for(YSSIZE_T i=i0; i>0; i--)
	{
		const int idx0=(trailBegin+i)%MAXNUMTRAIL;
		const int idx1=(trailBegin+i-1)%MAXNUMTRAIL;

		const double t0=(i==trailUsed ? 0.0 : cTime-trailTime[idx0]);
		const double t1=cTime-trailTime[idx1];

		const auto p0=(i==trailUsed ? cPos : trailPos[idx0]);
		const auto p1=trailPos[idx1];

		float sz=0.5f+(float)(t0*20.0);
		if(40.0f<sz)
		{
			sz=40.0f;
		}

		int nDiv=3+(int)((p0-p1).GetLength()/sz);
		for (int i = 0; i < nDiv; ++i)
		{
			const double param = (double)i / (double)nDiv;

			const double t = t0 * (1.0 - param) + t1 * param;
			const auto pos = p0 * (1.0 - param) + p1 * param;

			double dist = (cPos - pos).GetLength();
			YsColor col;
			double r, g, b, a;

			// Color
			// t=0   -> Red
			// t=0.1 -> Red
			// t=0.3 -> White
			// Alpha
			// t<5   -> 1.0
			// t=8   -> 0.0
			/*
			r=0.7;
			g=0.7;
			b=0.7;

			a=1.0f-t/(double)((TIMEPERSEG*MAXNUMTRAIL-500)/1000);
			a=YsBound(a,0.0,0.7);
			*/
			YsColor whiteHot, yellowHot, orangeHot, litSmoke, greySmoke, whiteSmoke;
			float tWhite, tYellow, tOrange, tLit, tGrey, tEnd;
			float dClear, dWhite, dYellow, dOrange, dLit;

			if (env == FSNIGHT)
			{
				whiteHot.SetFloatRGBA(1.0, 0.99, 0.93, 1.0);
				yellowHot.SetFloatRGBA(0.8, 0.74, 0.52, 0.95);
				orangeHot.SetFloatRGBA(0.8, 0.7, 0.5, 0.9);
				litSmoke.SetFloatRGBA(0.5, 0.5, 0.5, 0.8);
				greySmoke.SetFloatRGBA(0.15, 0.15, 0.15, 0.5);
				whiteSmoke.SetFloatRGBA(0.1, 0.1, 0.1, 0.25);
				tWhite = 0.05;
				tYellow = 0.08;
				tOrange = 0.11;
				tLit = 0.5;
				tGrey = 1.0;
				tEnd = 6.0;
				dWhite = 2.0;
				dYellow = 3.0;
				dOrange = 4.0;
				dLit = 15.0;
			}
			else
			{
				whiteHot.SetFloatRGBA(1.0, 0.99, 0.93, 1.0);
				yellowHot.SetFloatRGBA(1.0, 0.92, 0.65, 0.95);
				orangeHot.SetFloatRGBA(0.7, 0.63, 0.5, 0.9);
				litSmoke.SetFloatRGBA(0.65, 0.65, 0.65, 0.8);
				greySmoke.SetFloatRGBA(0.75, 0.75, 0.75, 0.5);
				whiteSmoke.SetFloatRGBA(0.8, 0.8, 0.8, 0.25);
				tWhite = 0.05;
				tYellow = 0.08;
				tOrange = 0.11;
				tLit = 0.5;
				tGrey = 1.0;
				tEnd = 6.0;
				dWhite = 1.0;
				dYellow = 2.0;
				dOrange = 3.0;
				dLit = 5.0;
			}
			//x0 – (x0-x1) * (t-t0)/dt
			if (dist < dWhite)
			{
				r = whiteHot.Rf() - (whiteHot.Rf() - yellowHot.Rf()) * dist / dWhite;
				g = whiteHot.Gf() - (whiteHot.Gf() - yellowHot.Gf()) * dist / dWhite;
				b = whiteHot.Bf() - (whiteHot.Bf() - yellowHot.Bf()) * dist / dWhite;
			}
			else if (dist < dYellow)
			{
				r = yellowHot.Rf() - (yellowHot.Rf() - orangeHot.Rf()) * (dist - dWhite) / (dYellow - dWhite);
				g = yellowHot.Gf() - (yellowHot.Gf() - orangeHot.Gf()) * (dist - dWhite) / (dYellow - dWhite);
				b = yellowHot.Bf() - (yellowHot.Bf() - orangeHot.Bf()) * (dist - dWhite) / (dYellow - dWhite);
			}
			else if (dist < dOrange)
			{
				r = orangeHot.Rf() - (orangeHot.Rf() - litSmoke.Rf()) * (dist - dYellow) / (dOrange - dYellow);
				g = orangeHot.Gf() - (orangeHot.Gf() - litSmoke.Gf()) * (dist - dYellow) / (dOrange - dYellow);
				b = orangeHot.Bf() - (orangeHot.Bf() - litSmoke.Bf()) * (dist - dYellow) / (dOrange - dYellow);
			}
			else if (dist < dLit)
			{
				r = litSmoke.Rf() - (litSmoke.Rf() - greySmoke.Rf()) * (dist - dOrange) / (dLit - dOrange);
				g = litSmoke.Gf() - (litSmoke.Gf() - greySmoke.Gf()) * (dist - dOrange) / (dLit - dOrange);
				b = litSmoke.Bf() - (litSmoke.Bf() - greySmoke.Bf()) * (dist - dOrange) / (dLit - dOrange);
			}
			else if (t < tLit)
			{
				r = greySmoke.Rf();
				g = greySmoke.Gf();
				b = greySmoke.Bf();
			}
			else if (t < tGrey)
			{
				r = greySmoke.Rf() - (greySmoke.Rf() - whiteSmoke.Rf()) * (t - tLit) / (tGrey - tLit);
				g = greySmoke.Gf() - (greySmoke.Gf() - whiteSmoke.Gf()) * (t - tLit) / (tGrey - tLit);
				b = greySmoke.Bf() - (greySmoke.Bf() - whiteSmoke.Bf()) * (t - tLit) / (tGrey - tLit);
			}
			else if (t < tEnd)
			{
				r = whiteSmoke.Rf();
				g = whiteSmoke.Gf();
				b = whiteSmoke.Bf();
			}
			else
			{
				r = 1.0;
				g = 0.0;
				b = 0.0;
			}

			int exp = 3;
			int offset = 0.005;
			a = YsBound((1 / pow(tEnd, exp - 1) * pow(tEnd - t, exp)) / tEnd + offset, 0.0, 1.0);

			col.SetDoubleRGBA(r,g,b,a);

			float sz=0.5f+(float)(t*20.0);
			if(32.0f<sz)
			{
				sz=32.0f;
			}

			float s=(float)((i+idx0)&7)*0.125;
			partMan.Add(pos,col,sz,s,0);
		}
		c++;
	}
}

// Implementation //////////////////////////////////////////
FsVisualDnm FsWeapon::aim9=nullptr;
FsVisualDnm FsWeapon::agm65=nullptr;
FsVisualDnm FsWeapon::bomb=nullptr;
FsVisualDnm FsWeapon::rocket=nullptr;
FsVisualDnm FsWeapon::aim9s=nullptr;
FsVisualDnm FsWeapon::agm65s=nullptr;
FsVisualDnm FsWeapon::rockets=nullptr;
FsVisualDnm FsWeapon::aim_coarse=nullptr;
FsVisualDnm FsWeapon::agm_coarse=nullptr;
FsVisualDnm FsWeapon::bomb_coarse=nullptr;
FsVisualDnm FsWeapon::rocket_coarse=nullptr;

FsVisualDnm FsWeapon::aim120=nullptr;
FsVisualDnm FsWeapon::aim120s=nullptr;
FsVisualDnm FsWeapon::aim120_coarse=nullptr;
FsVisualDnm FsWeapon::bomb250=nullptr;
FsVisualDnm FsWeapon::bomb250s=nullptr;
FsVisualDnm FsWeapon::bomb250_coarse=nullptr;
FsVisualDnm FsWeapon::bomb500hd=nullptr;
FsVisualDnm FsWeapon::bomb500hds=nullptr;
FsVisualDnm FsWeapon::bomb500hd_coarse=nullptr;

FsVisualDnm FsWeapon::aim9x=nullptr;
FsVisualDnm FsWeapon::aim9xs=nullptr;
FsVisualDnm FsWeapon::aim9x_coarse=nullptr;

FsVisualDnm FsWeapon::flarePod=nullptr;
FsVisualDnm FsWeapon::flare = nullptr;
FsVisualDnm FsWeapon::flare_coarse = nullptr;
FsVisualDnm FsWeapon::fuelTank=nullptr;


const double FsTrailTimePerSegment=0.1;


FsWeapon::FsWeapon()
{
	lifeRemain=0.0;
	timeRemain=0.0;
	timeUnguided=0.0;
	velocity=0.0;
	pos.Set(0.0,0.0,0.0);
	vec.Set(0.0,0.0,0.0);
	firedBy=NULL;
	creditOwner=FSWEAPON_CREDIT_OWNER_NOT_RECORDED;

	prev=NULL;
	next=NULL;
	prevFlare=NULL;
	nextFlare=NULL;

	trail=NULL;
	shouldJettison = YSFALSE;
}

void FsWeapon::Reset()
{
	type = FSWEAPON_NULL;
	category = FSWEAPONCAT_NULL;
	inList = FSENTITYLIST_SYSTEM;
	identify = "";
	prev = NULL;
	next = NULL;

	prv = YsOrigin();
	pos = YsOrigin();
	lastChecked = YsOrigin();
	vec = YsOrigin();
	velVec = YsOrigin();
	velocity = 0.0;
	accelVec = YsOrigin();
	att.Set(0.0,0.0,0.0);
	lifeRemain = 0.0;
	lifeTime = 0.0;
	timeRemain = 0.0;
	timeUnguided = 0.0;

	launchSpeed = 0.0;
	maxVelocity = 0.0;
	accel = 0.0;
	decel = 0.0;
	drag = 0.0;
	weight = 0.0;

	mobility = 0.0;
	radar = 0.0;
	destructivePower = 0.0;
	fuseRadius = 0.0;
	impactFuse = YSFALSE;
	target = NULL;
	canRetarget = YSFALSE;
	targetAir = YSFALSE;
	targetGnd = YSFALSE;
	followDecoy = YSFALSE;

	firedBy = NULL;
	creditOwner = FSWEAPON_CREDIT_OWNER_NOT_RECORDED;
	canJettison = YSFALSE;
	isFuelTank = YSFALSE;
	isDecoy = YSFALSE;
	trailSmoke = YSFALSE;
	applyGravity = YSFALSE;
	faceForward = YSFALSE;
	trail = NULL;
}

FsWeapon::FsWeaponPerformance FsWeapon::GetWeaponPerformanceByType(FSWEAPONTYPE &wep)
{
	switch (wep)
	{
	default:
	case FSWEAPON_NULL:
		return nullWepPerf;
	case FSWEAPON_GUN:
		return gunPerf;
	case FSWEAPON_AIM9:
		return aim9Perf;
	case FSWEAPON_AIM9X:
		return aim9xPerf;
	case FSWEAPON_AIM120:
		return aim120Perf;
	case FSWEAPON_AGM65:
		return agm65Perf;
	case FSWEAPON_ROCKET:
		return rocketPerf;
	case FSWEAPON_BOMB250:
		return b250Perf;
	case FSWEAPON_BOMB:
		return b500Perf;
	case FSWEAPON_BOMB500HD:
		return b500hdPerf;
	case FSWEAPON_FLARE:
	case FSWEAPON_FLARE_INTERNAL:
		return flarePerf;
	case FSWEAPON_FLAREPOD:
		return flarepodPerf;
	case FSWEAPON_FUELTANK:
		return fueltankPerf;	
	case FSWEAPON_SMOKE:
		return smokePerf;
	}
}

/*static*/ FsAmmunitionIndication::WEAPONTYPE FsWeapon::WeaponTypeToWeaponIndicationType(FSWEAPONTYPE wpnType)
{
	switch(wpnType)
	{
	default:
		return FsAmmunitionIndication::WPNTYPE_NULL;
	case FSWEAPON_GUN:
		return FsAmmunitionIndication::WPNTYPE_GUN;
	case FSWEAPON_AIM9:
		return FsAmmunitionIndication::WPNTYPE_AIM9;
	case FSWEAPON_AGM65:
		return FsAmmunitionIndication::WPNTYPE_AGM65;
	case FSWEAPON_BOMB:
		return FsAmmunitionIndication::WPNTYPE_BOMB;
	case FSWEAPON_ROCKET:
		return FsAmmunitionIndication::WPNTYPE_ROCKET;
	case FSWEAPON_FLARE:
	case FSWEAPON_FLARE_INTERNAL:
		return FsAmmunitionIndication::WPNTYPE_FLARE;
	case FSWEAPON_AIM120:
		return FsAmmunitionIndication::WPNTYPE_AIM120;
	case FSWEAPON_BOMB250:
		return FsAmmunitionIndication::WPNTYPE_BOMB250;
	case FSWEAPON_SMOKE:
		return FsAmmunitionIndication::WPNTYPE_SMOKE;
	case FSWEAPON_BOMB500HD:
		return FsAmmunitionIndication::WPNTYPE_BOMB500HD;
	case FSWEAPON_AIM9X:
		return FsAmmunitionIndication::WPNTYPE_AIM9X;
	case FSWEAPON_FUELTANK:
		return FsAmmunitionIndication::WPNTYPE_FUELTANK;
	}
}

/* static */void FsWeapon::DrawVisual
	   (FSWEAPONTYPE wpnType,YSBOOL coarse,
	   const YsMatrix4x4 &viewTfm,const YsMatrix4x4 &projTfm,
	   const YsVec3 &pos,const YsAtt3 &att,unsigned int /*drawFlag*/)
{
	FsVisualDnm *vis=nullptr;
	switch(wpnType)
	{
	default:
		break;
	case FSWEAPON_AIM120:
		vis=(coarse!=YSTRUE ? &aim120s : &aim120_coarse);
		break;
	case FSWEAPON_AIM9:
		vis=(coarse!=YSTRUE ? &aim9s : &aim_coarse);
		break;
	case FSWEAPON_AIM9X:
		vis=(coarse!=YSTRUE ? &aim9xs : &aim9x_coarse);
		break;
	case FSWEAPON_AGM65:
		vis=(coarse!=YSTRUE ? &agm65s : &agm_coarse);
		break;
	case FSWEAPON_BOMB:
		vis=(coarse!=YSTRUE ? &bomb : &bomb_coarse);
		break;
	case FSWEAPON_BOMB250:
		vis=(coarse!=YSTRUE ? &bomb250 : &bomb250_coarse);
		break;
	case FSWEAPON_BOMB500HD:
		vis=(coarse!=YSTRUE ? &bomb500hds : &bomb500hd_coarse);
		break;
	case FSWEAPON_ROCKET:
		vis=(coarse!=YSTRUE ? &rockets : &rocket_coarse);
		break;
	case FSWEAPON_FLAREPOD:
		vis=&flarePod;
		break;
	case FSWEAPON_FLARE:
		vis = (coarse != YSTRUE ? &flare : &flare_coarse);
		break;
	case FSWEAPON_FUELTANK:
		vis=&fuelTank;
		break;
	}

	if(vis!=nullptr)
	{
		vis->Draw(viewTfm,projTfm,pos,att,FSVISUAL_DRAWALL);
	}
}
/* static */ void FsWeapon::DrawShadow(
    FSWEAPONTYPE wpnType,YSBOOL coarse,
    const YsMatrix4x4 &viewTfm,const YsMatrix4x4 &projTfm,
    const YsVec3 &pos,const YsAtt3 &att,const YsMatrix4x4 &projPlnTfm)
{
	FsVisualDnm *vis=nullptr;
	switch(wpnType)
	{
	default:
		break;
	case FSWEAPON_AIM120:
		vis=(coarse!=YSTRUE ? &aim120s : &aim120_coarse);
		break;
	case FSWEAPON_AIM9:
		vis=(coarse!=YSTRUE ? &aim9s : &aim_coarse);
		break;
	case FSWEAPON_AIM9X:
		vis=(coarse!=YSTRUE ? &aim9xs : &aim9x_coarse);
		break;
	case FSWEAPON_AGM65:
		vis=(coarse!=YSTRUE ? &agm65s : &agm_coarse);
		break;
	case FSWEAPON_BOMB:
		vis=(coarse!=YSTRUE ? &bomb : &bomb_coarse);
		break;
	case FSWEAPON_BOMB250:
		vis=(coarse!=YSTRUE ? &bomb250 : &bomb250_coarse);
		break;
	case FSWEAPON_BOMB500HD:
		vis=(coarse!=YSTRUE ? &bomb500hds : &bomb500hd_coarse);
		break;
	case FSWEAPON_ROCKET:
		vis=(coarse!=YSTRUE ? &rockets : &rocket_coarse);
		break;
	case FSWEAPON_FLAREPOD:
		vis=&flarePod;
		break;
	case FSWEAPON_FLARE: //No shadow for flares for now
		break;
	case FSWEAPON_FUELTANK:
		vis=&fuelTank;
		break;
	}

	if(vis!=nullptr)
	{
		vis->DrawShadow(viewTfm,projTfm,pos,att,projPlnTfm);
	}
}

void FsWeapon::LaunchWeapon(
	FsWeaponPerformance &data,
	const YsVec3 &cpos,
	const YsAtt3 &catt,
	const double &vInit,
	class FsExistence *owner,
	FSWEAPON_CREDIT_OWNER creditOwnerIn,
	class FsExistence *targ,
	FsWeaponSmokeTrail *trl)
{
	if (owner != NULL && owner->GetType() == FSEX_AIRPLANE)
	{
		FsAirplane* ownerAirplane = (FsAirplane*)owner;
		shouldJettison = ownerAirplane->Prop().GetShouldJettisonWeapon();
	}
	else
	{
		shouldJettison = YSFALSE;
	}
	
	type = data.type;
	category = data.category;
	inList = data.inList;
	identify = data.identify;
	prv = cpos;
	pos = cpos;
	lastChecked = cpos;
	att = catt;
	lifeRemain = data.flightRange;
	lifeTime = data.flightTime;
	timeRemain = 2.0 * double(FsWeaponSmokeTrail::TIMEPERSEG * FsWeaponSmokeTrail::MAXNUMTRAIL) / 1000.0;
	// ^^^ 2.0 : take margine. because the trail buffer may not be updated regulary by TIMEPERSEG

	timeUnguided = data.trackDelay;
	
	launchSpeed = data.launchSpeed;
	maxVelocity = data.maxSpeed;
	accel = data.accel;
	decel = data.decel;
	drag = data.flyingDrag;
	weight = data.weight;
	if (weight == 0)
	{
		weight = 1.0;
	}
	
	mobility = data.turnRate;
	radar = data.trackAngle;
	destructivePower = data.power;
	fuseRadius = data.fuseRadius;
	impactFuse = data.impactFuse;
	target = targ;
	canRetarget = data.canRetarget;
	targetAir = data.targetAir;
	targetGnd = data.targetGnd;
	followDecoy = data.followDecoy;

	firedBy = owner;
	creditOwner = creditOwnerIn;
	canJettison = data.canJettison;
	isFuelTank = data.fueltank;
	isDecoy = data.isDecoy;
	trailSmoke = data.trailSmoke;
	applyGravity = data.gravity;
	faceForward = data.faceForward;

	trail = trl;

	vec.Set(0.0, 0.0, vInit + data.launchSpeed);
	catt.Mul(vec, vec); // vec=a.GetMatrix()*vec;
	velocity = vec.GetLength();

	if (trl != NULL)
	{
		trl->Initialize();
	}
}

void FsWeapon::ThrowDebris(const double &/*ctime*/,const YsVec3 &p,const YsVec3 &v,const double &l)
{
	type=FSWEAPON_DEBRIS;
	prv=p;
	pos=p;
	vec=v;
	lastChecked=p;
	firedBy=NULL;
	creditOwner=FSWEAPON_CREDIT_OWNER_NON_PLAYER;
	velocity=v.GetLength();
	lifeRemain=l;
	timeRemain=0.1;
	target=NULL;
	trail=NULL;
	destructivePower=0;
	att=YsZeroAtt();
}

void FsWeapon::Move(const double &dt,const double &cTime,const FsWeather &weather,const FsWeapon *flareList)
{
	accelVec.Set(0.0,0.0,0.0);
	if (lifeRemain > 0.0)
	{
		YSBOOL controlled = YSFALSE;
		if (canJettison == YSTRUE && shouldJettison == YSTRUE)
		{
			accelVec += FsGravityVec;
			att.SetForwardVector(vec);
		}
		else
		{
			//Pointing
			if (target != NULL && timeUnguided < YsTolerance)
			{
				controlled = YSTRUE;
				YsVec3 tpos;
				YsMatrix4x4 mat;

				mat.Translate(pos);
				mat.Rotate(att);
				mat.Invert();

				tpos = mat * (target->GetPosition());
				if (followDecoy == YSTRUE)
				{
					const FsWeapon* flare;
					YsVec3 flarePos;
					double flareZ;
					YSBOOL fooled;
					fooled = YSFALSE;
					flareZ = lifeRemain;
					for (flare = flareList; flare != NULL; flare = flare->nextFlare)
					{
						flarePos = mat * flare->pos;
						if (flarePos.z() > 0.0 && flarePos.z() < flareZ &&
							atan2(flarePos.x() * flarePos.x() + flarePos.y() * flarePos.y(), flarePos.z()) < radar)
						{
							fooled = YSTRUE;
							tpos = flarePos;
							flareZ = flarePos.z();
						}
					}
				}

				double r;
				r = atan2(sqrt(tpos.x() * tpos.x() + tpos.y() * tpos.y()), tpos.z());
				if (r < radar || (canRetarget == YSTRUE && YSTRUE == IsOwnerStillHaveTarget()))
				{
					double maxMovement;
					maxMovement = mobility * dt;

					double yaw, pit;
					yaw = atan2(-tpos.x(), tpos.z());
					pit = atan2(tpos.y(), tpos.z());

					yaw = YsBound(yaw, -maxMovement, maxMovement);
					pit = YsBound(pit, -maxMovement, maxMovement);
					att.YawLeft(yaw);
					att.NoseUp(pit);
				}
				else if (tpos.z() < -300.0)
				{
					target = NULL;
				}
			}
			
			//Forces
			accelVec += FsGravityVec * (int)applyGravity;
			
			const double D = 0.5 * drag * velocity * velocity * FsGetAirDensity(pos.y());
			const double a = D / weight;
			YsVec3 dVec = -vec;
			YsVec3 thrust;
			if (YSOK == dVec.Normalize())
			{
				accelVec += a * dVec;
			}
			
			if (velocity < maxVelocity)
			{
				thrust.Set(0.0,0.0,accel);
			}
			else if (velocity > maxVelocity)
			{
				thrust.Set(0.0, 0.0, -decel);
			}
			att.Mul(thrust,thrust);
			accelVec += thrust;

			if (timeUnguided > 0.0)
			{
				timeUnguided -= dt;
			}
		}
		
		vec = vec + accelVec * dt;
		velocity = vec.GetLength();
		if (faceForward == YSTRUE && controlled != YSTRUE) //Make attitude follow vVector
		{
			att.SetForwardVector(vec);
		}
		else //Make vVector follow attitude
		{
			vec.Set(0.0, 0.0, velocity);
			att.Mul(vec, vec);
		}
		prv = pos;
		pos = pos + vec * dt;
		pos += weather.GetWind() * dt;
		
		lifeRemain = lifeRemain - velocity * dt;
		lifeTime -= dt;
		if (lifeRemain <= YsTolerance)
		{
			lifeRemain = 0.0;
		}
		
	}
	
	else if(timeRemain>0.0)
	{
		timeRemain-=dt;
		if(timeRemain<=YsTolerance)
		{
			timeRemain=0.0;
		}
	}
	
	if(trailSmoke == YSTRUE)
	{
		if (shouldJettison == YSTRUE)
		{
			if (trail != nullptr)
			{
				trail->used = YSTRUE;
				trail = nullptr;
			}
		}
		else if(nullptr!=trail && lifeRemain>0.0)
		{
			trail->Add(dt,cTime,pos,att);
		}
	}
}

YSBOOL FsWeapon::Bounce(YsVec3 &normal, YsVec3 &intersect, YsVec3 &outVel, YsVec3 &posOffset, double maxAngle, double eneryReturn)
{
	return YSFALSE;
}

YSBOOL FsWeapon::IsOwnerStillHaveTarget(void)
{
	YSHASHKEY ownerAirTargetKey=YSNULLHASHKEY;
	if(NULL!=firedBy)
	{
		if(FSEX_AIRPLANE==firedBy->GetType())
		{
			ownerAirTargetKey=((FsAirplane *)firedBy)->Prop().GetAirTargetKey();
		}
		else if(FSEX_GROUND==firedBy->GetType())
		{
			ownerAirTargetKey=FsExistence::GetSearchKey(((FsGround *)firedBy)->Prop().GetAirTarget());
		}
	}

	if(NULL!=firedBy && NULL!=target && ownerAirTargetKey==FsExistence::GetSearchKey(target))
	{
		return YSTRUE;
	}
	else
	{
		return YSFALSE;
	}
}

void FsWeapon::HitGround(
    FsWeaponHolder *callback,
    const double &ctime,
    const FsField &fld,
    FsExplosionHolder *explode,
    FsSimulation *sim,
    YsList <FsKillCredit> *&killCredit)
{
	if(lifeRemain>0.0)
	{
		double elv;
		const YsSceneryItem *evg;
		int collType; // 1:Ground  2:Shell
		YSSCNAREATYPE areaType;
		YsVec3 shellItsc;
		YsVec3 cen, nom, crs;
		YsPlane pln;

		evg=fld.GetFieldElevation(elv,pos.x(),pos.z());

		collType=0;
		areaType=YSSCNAREA_NOAREA;
		if(pos.y()<=elv+YsTolerance)
		{
			collType=1;
			areaType=sim->GetAreaType(pos);
			
		}
		else if(YSTRUE==fld.GetFieldShellCollision(shellItsc,prv,pos))
		{
			collType=2;
			pos = shellItsc;
		}

		if(collType!=0)
		{
			if (shouldJettison == YSTRUE)
			{
				lifeRemain = 0.0;
				timeRemain = 0.0;
				explode = NULL;
			}
			if(explode!=NULL)
			{
				static int step=0;
				switch(category)
				{
				case FSWEAPONCAT_BULLET:
					if((step&3)==0)
					{
						// Adjust for elevation grid.
						// Do this intersection-calculation only for guns.
						// The location calculated by the intersection-calculation may
						// be far away from the actual exploding location.  For example,
						// when the bomb hits side wall of the elevation grid.
						// This error may cause unexpected damage to airplanes or
						// grounds.
						if(collType==1)
						{
							cen = pos;
							cen.SetY(elv);

							fld.GetFieldElevationAndNormal(elv, nom, pos.x(), pos.z());
							pln.Set(cen, nom);
							if (pln.GetIntersection(crs, pos, pos - prv) == YSOK)
							{
								pos = crs;
							}
						}

						if (collType == 1 && areaType == YSSCNAREA_WATER)
						{
							if (impactFuse == YSTRUE)
							{
								ExplodeBombInWater(callback, ctime, pos, destructivePower, explode, sim, killCredit);
							}
							else
							{
								explode->WaterPlume(ctime, pos, destructivePower, destructivePower / 2, destructivePower * 3, NULL, YSFALSE);
							}
						}
						else
						{
							if (impactFuse == YSTRUE)
							{
								ExplodeBomb(callback, ctime, pos, destructivePower * 5 / 2, explode, sim, killCredit);
							}
							else
							{
								explode->Explode(ctime, pos, destructivePower, 0.0, destructivePower * 2, YSTRUE, NULL, YSFALSE);
							}
						}
						lifeRemain = 0.0;
						timeRemain = 0.0;
					}
					step=(step+1)&255;
					break;
				case FSWEAPONCAT_COUNTERMEASURE:
					if (collType == 1)
					{
						pos.SetY(elv);
					}

					if (areaType == YSSCNAREA_WATER)
					{
						explode->WaterPlume(ctime, pos, 3.0, 1.0, 10.0, NULL, YSFALSE);
						lifeRemain = 0.0;
					}
					else
					{

						YsVec3 nom, offset, bounceVel;
						double maxBounceAngle, eReturn;
						eReturn = 0.75;
						maxBounceAngle = YsPi / 2;
						
						cen = pos;
						cen.SetY(elv);

						fld.GetFieldElevationAndNormal(elv, nom, pos.x(), pos.z());
						pln.Set(cen, nom);
						if (pln.GetIntersection(crs, pos, pos - prv) == YSOK)
						{
							pos = crs;
						}

						if (Bounce(nom, crs, bounceVel, offset, maxBounceAngle, eReturn) == YSTRUE)
						{
							pos += offset;
							velVec = bounceVel;
							velocity = velVec.GetLength();
							if (velocity < 10.0)
							{
								lifeRemain = 0.0;
							}
						}
						else
						{
							lifeRemain = 0.0;
						}
					}
					break;
				case FSWEAPONCAT_SYSTEM:
					lifeRemain = 0.0;
					timeRemain = 0.0;
					break;
				default:
					if (collType == 1)
					{
						pos.SetY(elv);
					}

					if(areaType==YSSCNAREA_WATER)
					{
						if (impactFuse == YSTRUE)
						{
							ExplodeBombInWater(callback, ctime, pos, destructivePower * 5/10, explode, sim, killCredit);
						}
						else
						{
							explode->WaterPlume(ctime, pos, destructivePower/3, destructivePower / 5, destructivePower, NULL, YSFALSE);
						}
					}
					else
					{
						if (impactFuse == YSTRUE)
						{
							ExplodeBomb(callback, ctime, pos, destructivePower * 5/2, explode, sim, killCredit);
						}
						else
						{
							explode->Explode(ctime, pos, destructivePower/3, 0.0, destructivePower, YSTRUE, NULL, YSFALSE);
						}
					}
					lifeRemain = 0.0;
					break;
				}
			}
		}
	}
}

YSBOOL FsWeapon::HitObject(
    FsWeaponHolder *callback,
    const double &ctime,
    FsExistence &obj,
    FsExplosionHolder *explosion,
    FsSimulation *sim,
    YsList <FsKillCredit> *&killCredit)
{
	if(lifeRemain>0.0 && firedBy!=&obj)
	{
		const auto &coll=obj.TransformedCollisionShell();

		const YsVec3 *tpos;
		tpos=&obj.GetPosition();

		double sqDist1,sqDist2,sqDist3,sqLastMovedDist,rad, sqfuserad;

		rad=obj.GetApproximatedCollideRadius();

		sqDist1=(pos-(*tpos)).GetSquareLength();
		sqDist2=(lastChecked-(*tpos)).GetSquareLength();
		sqfuserad = (fuseRadius + rad) * (fuseRadius + rad);

		// Proximity tube >>
		if(fuseRadius > 0.0 && (sqDist1 <= sqfuserad || sqDist2 <= sqfuserad) &&
			shouldJettison != YSTRUE && target != NULL && timeUnguided < YsTolerance)
		{
			YsVec3 np;
			YsGetNearestPointOnLine3(np,pos,lastChecked,*tpos);
			sqDist3=(np-(*tpos)).GetSquareLength();

			if(YsCheckInBetween3(np,pos,lastChecked)==YSTRUE && sqDist3<fuseRadius*fuseRadius)
			{
				double l,dmg;
				l=sqrt(sqDist3);

				dmg=1.0+(double)destructivePower*(fuseRadius-l)/fuseRadius;

				YSBOOL killed;
				callback->GiveDamage(killed,obj,(int)dmg,FSDIEDOF_MISSILE,*this);
				callback->ThrowMultiDebris(5,ctime,pos,obj.GetAttitude(),60.0);
				if(killed==YSTRUE)
				{
					sim->KillCallBack(obj,*tpos);
					AddKillCredit(killCredit,&obj,ctime);
				}

				explosion->Explode(ctime,pos,20.0,5.0,range,YSTRUE,NULL,YSTRUE);

				lifeRemain=0.0;
				target=NULL;

				return YSTRUE;
			}
		}
		// Proximity tube <<

		sqLastMovedDist=(lastChecked-pos).GetSquareLength();
		if(sqDist1<=sqLastMovedDist+rad*rad ||
		   sqDist2<=sqLastMovedDist+rad*rad)
		{
			YsVec3 np;
			YsGetNearestPointOnLine3(np,pos,lastChecked,*tpos);
			sqDist3=(np-(*tpos)).GetSquareLength();
			YsShellPolygonHandle plHd;
			YsVec3 intersect;

			switch (category)
			{
			case FSWEAPONCAT_BULLET:
				if(sqDist1>rad*rad && sqDist2>rad*rad && sqDist3>rad*rad)
				{
					return YSFALSE;
				}
				
				plHd=coll.ShootRayH(intersect,lastChecked,pos-lastChecked);

				if(plHd!=NULL && YsCheckInBetween3(intersect,pos,lastChecked)==YSTRUE)
				{
					YSBOOL killed;
					lifeRemain=0.0;

					if(callback->GiveDamage(killed,obj,destructivePower,FSDIEDOF_GUN,*this)==YSTRUE &&
					   explosion!=NULL)
					{
						callback->ThrowRandomDebris(ctime,intersect,obj.GetAttitude(),60.0);
						if(killed==YSTRUE)
						{
							callback->ThrowMultiDebris(5,ctime,pos,obj.GetAttitude(),60.0);
							sim->KillCallBack(obj,*tpos);
							AddKillCredit(killCredit,&obj,ctime);
						}
						else
						{
							explosion->Explode(ctime,intersect,destructivePower/3,0.0,destructivePower,YSTRUE,NULL,YSTRUE);
						}
					}

					return YSTRUE;
				}
				break;			
			case FSWEAPONCAT_COUNTERMEASURE:
			
				if (sqDist1>rad*rad && sqDist2>rad*rad && sqDist3>rad*rad)
				{
					return YSFALSE;
				}
				
				plHd = coll.ShootRayH(intersect, lastChecked, pos - lastChecked);
				
				if (plHd != NULL && YsCheckInBetween3(intersect, pos, lastChecked) == YSTRUE)
				{
					YsVec3 nom, offset, bounceVel;
					double maxBounceAngle, eReturn;
					eReturn = 0.75;
					maxBounceAngle = YsPi / 2;
					coll.GetNormal(nom,plHd);

					if (Bounce(nom, intersect, bounceVel, offset, maxBounceAngle, eReturn) == YSTRUE)
					{
						pos += offset;
						velVec = bounceVel;
					}
					else
					{
						lifeRemain = 0.0;
					}
				}
				break;
			case FSWEAPONCAT_SYSTEM:
				lifeRemain = 0.0;
				timeRemain = 0.0;
				break;
			default:
				if (sqDist1>rad *rad && sqDist2>rad*rad && sqDist3>rad*rad)
				{
					return YSFALSE;
				}

				plHd = coll.ShootRayH(intersect,lastChecked,pos-lastChecked);

				if (plHd != NULL && YsCheckInBetween3(intersect,pos,lastChecked) == YSTRUE)
				{
					YSBOOL killed;
					lifeRemain = 0.0;

					if (shouldJettison == YSTRUE)
					{
						//Do no damage. How much could 250kg at 300kt hurt anyway?
						timeRemain = 0.0;
						target = NULL;
						return YSTRUE;
					}

					FSDIEDOF diedof;
					if (category == FSWEAPONCAT_FREEFALL || category == FSWEAPONCAT_GUIDEDFREEFALL || category == FSWEAPONCAT_UTILITY)
					{
						diedof = FSDIEDOF_BOMB;
					}
					else
					{
						diedof = FSDIEDOF_MISSILE;
					}

					if (callback->GiveDamage(killed, obj, destructivePower, diedof, *this) == YSTRUE &&
						explosion != NULL)
					{
						if (killed == YSTRUE)
						{
							callback->ThrowMultiDebris(5, ctime, pos, obj.GetAttitude(), 60.0);
							sim->KillCallBack(obj, *tpos);
							AddKillCredit(killCredit, &obj, ctime);
						}
						else
						{
							explosion->Explode(ctime, intersect, 5.0, 3.0, 3.0, YSTRUE, NULL, YSTRUE);
						}
					}

					if (impactFuse == YSTRUE)
					{
						ExplodeBomb(callback, ctime, pos, destructivePower * 5 / 2, explosion, sim, killCredit);
					}
					return YSTRUE;
				}
				break;
			}
		}
	}
	return YSFALSE;
}

void FsWeapon::ExplodeBomb
    (FsWeaponHolder *callback,
     const double &ctime,
     const YsVec3 &pos,
     const double &rad,
     FsExplosionHolder *explosion,
     FsSimulation *sim,
     YsList <FsKillCredit> *&killCredit)
{
	double sqDist,dist;
	double dmg;
	FsAirplane *air;
	FsGround *gnd;

	explosion->Explode(ctime,pos,10.0,0.0,rad+5.0,YSTRUE,NULL,YSFALSE);

	FsSoundSetOneTime(FSSND_ONETIME_BLAST);

	air=NULL;
	while((air=sim->FindNextAirplane(air))!=NULL)
	{
		if(air->Prop().IsAlive()==YSTRUE)
		{
			const YsVec3 &objPos=air->GetPosition();
			sqDist=(objPos-pos).GetSquareLength();
			if(sqDist<rad*rad)
			{
				YSBOOL killed;
				dist=sqrt(sqDist);
				dmg=double(destructivePower)*(rad-dist)/rad;
				callback->GiveDamage(killed,*air,int(dmg),FSDIEDOF_BOMB,*this);
				if(killed==YSTRUE)
				{
					AddKillCredit(killCredit,air,ctime);
					callback->ThrowMultiDebris(5,ctime,objPos,air->GetAttitude(),60.0);
					sim->KillCallBack(*air,objPos);
				}
			}
		}
	}

	gnd=NULL;
	while((gnd=sim->FindNextGround(gnd))!=NULL)
	{
		if(gnd->Prop().IsAlive()==YSTRUE)
		{
			const YsVec3 &objPos=gnd->GetPosition();
			sqDist=(objPos-pos).GetSquareLength();
			if(sqDist<rad*rad)
			{
				YSBOOL killed;
				dist=sqrt(sqDist);
				dmg=double(destructivePower)*(rad-dist)/rad;
				callback->GiveDamage(killed,*gnd,int(dmg),FSDIEDOF_BOMB,*this);
				if(killed==YSTRUE)
				{
					AddKillCredit(killCredit,gnd,ctime);
					callback->ThrowMultiDebris(5,ctime,objPos,gnd->GetAttitude(),60.0);
					sim->KillCallBack(*gnd,objPos);
				}
			}
		}
	}
}

void FsWeapon::ExplodeBombInWater(
    FsWeaponHolder *callback,
    const double &ctime,
    const YsVec3 &pos,
    const double &rad,  // B500->50/2  B250->35/2  RKT->10
    FsExplosionHolder *explosion,
    FsSimulation *sim,
    YsList <FsKillCredit> *&killCredit)
{
	double sqDist,dist;
	YsVec3 objPos;
	double dmg;
	FsAirplane *air;
	FsGround *gnd;

	explosion->WaterPlume(ctime,pos,10.0,rad,rad*10.0,NULL,YSFALSE);

	FsSoundSetOneTime(FSSND_ONETIME_BLAST);

	air=NULL;
	while((air=sim->FindNextAirplane(air))!=NULL)
	{
		if(air->Prop().IsAlive()==YSTRUE)
		{
			objPos=air->GetPosition();
			sqDist=(objPos-pos).GetSquareLength();
			if(sqDist<rad*rad)
			{
				YSBOOL killed;
				dist=sqrt(sqDist);
				dmg=double(destructivePower)*(rad-dist)/rad;
				callback->GiveDamage(killed,*air,int(dmg),FSDIEDOF_BOMB,*this);
				if(killed==YSTRUE)
				{
					AddKillCredit(killCredit,air,ctime);
				}
			}
		}
	}

	gnd=NULL;
	while((gnd=sim->FindNextGround(gnd))!=NULL)
	{
		if(gnd->Prop().IsAlive()==YSTRUE)
		{
			objPos=gnd->GetPosition();
			sqDist=(objPos-pos).GetSquareLength();
			if(sqDist<rad*rad)
			{
				YSBOOL killed;
				dist=sqrt(sqDist);
				dmg=double(destructivePower)*(rad-dist)/rad;
				callback->GiveDamage(killed,*gnd,int(dmg),FSDIEDOF_BOMB,*this);
				if(killed==YSTRUE)
				{
					AddKillCredit(killCredit,gnd,ctime);
				}
			}
		}
	}
}

// The following function is moved to fsweapongl.cpp and fsweaponbi.cpp
// void FsWeapon::Draw(void)

YSRESULT FsWeapon::AddKillCredit(YsList <FsKillCredit> *&killCredit,FsExistence *whoIsKilled,const double &when) const
{
	if(whoIsKilled!=NULL && whoIsKilled->isPlayingRecord!=YSTRUE)
	{
		if(FSEX_GROUND==whoIsKilled->GetType())
		{
			FsGround *gnd;
			gnd=(FsGround *)whoIsKilled;
			if(gnd->Prop().IsNonGameObject()==YSTRUE)  // 2005/02/05 Non-game object check
			{
				return YSOK;
			}
		}

		YsList <FsKillCredit> *neo;
		neo=new YsList <FsKillCredit>;
		neo->dat.whoKilled=firedBy;
		neo->dat.creditOwner=creditOwner;
		neo->dat.whom=whoIsKilled;
		neo->dat.byWhatKindOfWeapon=type;
		neo->dat.where=pos;
		neo->dat.when=when;

		killCredit=killCredit->Append(neo);
	}
	return YSOK;
}

void FsWeapon::AddToParticleManager(class YsGLParticleManager &partMan,const double cTime, FSENVIRONMENT env) const
{
	if(nullptr!=trail)
	{
		YSBOOL includeCurrentPos=(0.0<lifeRemain ? YSTRUE : YSFALSE);
		if(category == FSWEAPONCAT_COUNTERMEASURE)
		{
			trail->AddToParticleManagerAsFlare(partMan,pos,cTime,includeCurrentPos, env);
		}
		else
		{
			trail->AddToParticleManager(partMan,pos,cTime,includeCurrentPos, env);
		}
	}
}

// Implementation //////////////////////////////////////////
FsWeaponHolder::FsWeaponHolder(FsSimulation *simPtr) : sim(simPtr)
{
	LoadMissilePattern();

	toPlay=NULL;
	toSave=new FsRecord <FsWeaponRecord>;

	freeList = NULL;
	weaponList=NULL;
	decoyList = NULL;
	bulletList = NULL;
	systemList = NULL;

	killCredit=NULL;

	netServer=NULL;
	netClient=NULL;

	Clear();
}

FsWeaponHolder::~FsWeaponHolder()
{
	if(toSave!=NULL)
	{
		delete toSave;
	}
	if(toPlay!=NULL)
	{
		delete toPlay;
	}
	killCredit->DeleteList();
}

YSRESULT FsWeaponHolder::LoadMissilePattern(void)
{
	if(FsWeapon::aim9==nullptr)
	{
		FsWeapon::aim9.Load(L"misc/aim9.srf");
		if(FsWeapon::aim9==nullptr)
		{
			fsStderr.Printf("Cannot read AIM-9 pattern\n");
			return YSERR;
		}
	}

	if(FsWeapon::aim9s==nullptr)
	{
		FsWeapon::aim9s.Load(L"misc/aim9s.srf");
		if(FsWeapon::aim9s==nullptr)
		{
			fsStderr.Printf("Cannot read AIM-9(silent) pattern\n");
			return YSERR;
		}
	}

	if(FsWeapon::agm65==nullptr)
	{
		FsWeapon::agm65.Load(L"misc/agm65.srf");
		if(FsWeapon::agm65==nullptr)
		{
			fsStderr.Printf("Cannot read AGM-65 pattern.\n");
			return YSERR;
		}
	}

	if(FsWeapon::agm65s==nullptr)
	{
		FsWeapon::agm65s.Load(L"misc/agm65s.srf");
		if(FsWeapon::agm65s==nullptr)
		{
			fsStderr.Printf("Cannot read AGM-65(silent) pattern.\n");
			return YSERR;
		}
	}

	if(FsWeapon::bomb==nullptr)
	{
		FsWeapon::bomb.Load(L"misc/bomb.srf");
		if(FsWeapon::bomb==nullptr)
		{
			fsStderr.Printf("Cannot read BOMB pattern.\n");
			return YSERR;
		}
	}

	if(FsWeapon::rocket==nullptr)
	{
		FsWeapon::rocket.Load(L"misc/rocket.srf");
		if(FsWeapon::rocket==nullptr)
		{
			fsStderr.Printf("Cannot read ROCKET pattern.\n");
			return YSERR;
		}
	}

	if(FsWeapon::rockets==nullptr)
	{
		FsWeapon::rockets.Load(L"misc/rockets.srf");
		if(FsWeapon::rockets==nullptr)
		{
			fsStderr.Printf("Cannot read ROCKET(silent) pattern.\n");
			return YSERR;
		}
	}

	if(FsWeapon::aim_coarse==nullptr)
	{
		FsWeapon::aim_coarse.Load(L"misc/aim_coarse.srf");
		if(FsWeapon::aim_coarse==nullptr)
		{
			fsStderr.Printf("Cannot read AIM(coarse) pattern.\n");
			return YSERR;
		}
	}

	if(FsWeapon::agm_coarse==nullptr)
	{
		FsWeapon::agm_coarse.Load(L"misc/agm_coarse.srf");
		if(FsWeapon::agm_coarse==nullptr)
		{
			fsStderr.Printf("Cannot read AGM(coarse) pattern.\n");
			return YSERR;
		}
	}

	if(FsWeapon::bomb_coarse==nullptr)
	{
		FsWeapon::bomb_coarse.Load(L"misc/bomb_coarse.srf");
		if(FsWeapon::bomb_coarse==nullptr)
		{
			fsStderr.Printf("Cannot read BOMB(coarse) pattern.\n");
			return YSERR;
		}
	}

	if(FsWeapon::rocket_coarse==nullptr)
	{
		FsWeapon::rocket_coarse.Load(L"misc/rocket_coarse.srf");
		if(FsWeapon::rocket_coarse==nullptr)
		{
			fsStderr.Printf("Cannot read ROCKET(coarse) pattern.\n");
			return YSERR;
		}
	}




	if(FsWeapon::aim120==nullptr)
	{
		FsWeapon::aim120.Load(L"misc/aim120.srf");
		if(FsWeapon::aim120==nullptr)
		{
			fsStderr.Printf("Cannot read AIM120 pattern.\n");
			return YSERR;
		}
	}
	if(FsWeapon::aim120s==nullptr)
	{
		FsWeapon::aim120s.Load(L"misc/aim120s.srf");
		if(FsWeapon::aim120s==nullptr)
		{
			fsStderr.Printf("Cannot read AIM120 (silent) pattern.\n");
			return YSERR;
		}
	}
	if(FsWeapon::aim120_coarse==nullptr)
	{
		FsWeapon::aim120_coarse.Load(L"misc/aim120_coarse.srf");
		if(FsWeapon::aim120_coarse==nullptr)
		{
			fsStderr.Printf("Cannot read AIM120 (coarse) pattern.\n");
			return YSERR;
		}
	}


	if(FsWeapon::bomb250==nullptr)
	{
		FsWeapon::bomb250.Load(L"misc/bomb250.srf");
		if(FsWeapon::bomb250==nullptr)
		{
			fsStderr.Printf("Cannot read BOMB250 pattern.\n");
			return YSERR;
		}
	}

	if(FsWeapon::bomb250_coarse==nullptr)
	{
		FsWeapon::bomb250_coarse.Load(L"misc/bomb250_coarse.srf");
		if(FsWeapon::bomb250_coarse==nullptr)
		{
			fsStderr.Printf("Cannot read BOMB250(coarse) pattern.\n");
			return YSERR;
		}
	}


	if(nullptr==FsWeapon::bomb500hd)
	{
		FsWeapon::bomb500hd.Load(L"misc/bomb500hd.srf");
		if(nullptr==FsWeapon::bomb500hd)
		{
			fsStderr.Printf("Cannot read BOMB500HD pattern.\n");
			return YSERR;
		}
	}

	if(nullptr==FsWeapon::bomb500hds)
	{
		FsWeapon::bomb500hds.Load(L"misc/bomb500hds.srf");
		if(nullptr==FsWeapon::bomb500hds)
		{
			fsStderr.Printf("Cannot read BOMB500HD (silent) pattern.\n");
			return YSERR;
		}
	}

	if(nullptr==FsWeapon::bomb500hd_coarse)
	{
		FsWeapon::bomb500hd_coarse.Load(L"misc/bomb500hd_coarse.srf");
		if(nullptr==FsWeapon::bomb500hd_coarse)
		{
			fsStderr.Printf("Cannot read BOMB500HD (coarse) pattern.\n");
			return YSERR;
		}
	}


	if(nullptr==FsWeapon::aim9x)
	{
		FsWeapon::aim9x.Load(L"misc/aim9x.srf");
		if(nullptr==FsWeapon::aim9x)
		{
			fsStderr.Printf("Cannot read AIM9X pattern.\n");
			return YSERR;
		}
	}

	if(nullptr==FsWeapon::aim9xs)
	{
		FsWeapon::aim9xs.Load(L"misc/aim9xs.srf");
		if(nullptr==FsWeapon::aim9xs)
		{
			fsStderr.Printf("Cannot read AIM9X (silent) pattern.\n");
			return YSERR;
		}
	}

	if(nullptr==FsWeapon::aim9x_coarse)
	{
		FsWeapon::aim9x_coarse.Load(L"misc/aim9x_coarse.srf");
		if(nullptr==FsWeapon::aim9x_coarse)
		{
			fsStderr.Printf("Cannot read AIM9X (coarse) pattern.\n");
			return YSERR;
		}
	}

	if(nullptr==FsWeapon::flarePod)
	{
		FsWeapon::flarePod.Load(L"misc/flarepod.srf");
		if(nullptr==FsWeapon::flarePod)
		{
			fsStderr.Printf("Cannot read Flare Pod pattern.\n");
			return YSERR;
		}
	}

	if (nullptr == FsWeapon::flare)
	{
		FsWeapon::flare.Load(L"misc/flare.srf");
		if (nullptr == FsWeapon::flare)
		{
			fsStderr.Printf("Cannot read Flare pattern.\n");
			return YSERR;
		}
	}

	if (nullptr == FsWeapon::flare_coarse)
	{
		FsWeapon::flare_coarse.Load(L"misc/flare_coarse.srf");
		if (nullptr == FsWeapon::flare)
		{
			fsStderr.Printf("Cannot read Flare (coarse) pattern.\n");
			return YSERR;
		}
	}

	if(nullptr==FsWeapon::fuelTank)
	{
		FsWeapon::fuelTank.Load(L"misc/fueltank.srf");
		if(nullptr==FsWeapon::fuelTank)
		{
			fsStderr.Printf("Cannot read fuel tank pattern.\n");
			return YSERR;
		}
	}


	return YSOK;
}

/* static */ void FsWeaponHolder::FreeMissilePattern(void)
{
	if(FsWeapon::aim9!=nullptr)
	{
		FsWeapon::aim9.CleanUp();
	}

	if(FsWeapon::aim9s!=nullptr)
	{
		FsWeapon::aim9s.CleanUp();
	}

	if(FsWeapon::agm65!=nullptr)
	{
		FsWeapon::agm65.CleanUp();
	}

	if(FsWeapon::agm65s!=nullptr)
	{
		FsWeapon::agm65s.CleanUp();
	}

	if(FsWeapon::bomb!=nullptr)
	{
		FsWeapon::bomb.CleanUp();
	}

	if(FsWeapon::rocket!=nullptr)
	{
		FsWeapon::rocket.CleanUp();
	}

	if(FsWeapon::rockets!=nullptr)
	{
		FsWeapon::rockets.CleanUp();
	}

	if(FsWeapon::aim_coarse!=nullptr)
	{
		FsWeapon::aim_coarse.CleanUp();
	}

	if(FsWeapon::agm_coarse!=nullptr)
	{
		FsWeapon::agm_coarse.CleanUp();
	}

	if(FsWeapon::bomb_coarse!=nullptr)
	{
		FsWeapon::bomb_coarse.CleanUp();
	}

	if(FsWeapon::rocket_coarse!=nullptr)
	{
		FsWeapon::rocket_coarse.CleanUp();
	}




	if(FsWeapon::aim120!=nullptr)
	{
		FsWeapon::aim120.CleanUp();
	}
	if(FsWeapon::aim120s!=nullptr)
	{
		FsWeapon::aim120s.CleanUp();
	}
	if(FsWeapon::aim120_coarse!=nullptr)
	{
		FsWeapon::aim120_coarse.CleanUp();
	}


	if(FsWeapon::bomb250!=nullptr)
	{
		FsWeapon::bomb250.CleanUp();
	}

	if(FsWeapon::bomb250_coarse!=nullptr)
	{
		FsWeapon::bomb250_coarse.CleanUp();
	}


	if(nullptr!=FsWeapon::bomb500hd)
	{
		FsWeapon::bomb500hd.CleanUp();
	}

	if(nullptr!=FsWeapon::bomb500hds)
	{
		FsWeapon::bomb500hds.CleanUp();
	}

	if(nullptr!=FsWeapon::bomb500hd_coarse)
	{
		FsWeapon::bomb500hd_coarse.CleanUp();
	}


	if(nullptr!=FsWeapon::aim9x)
	{
		FsWeapon::aim9x.CleanUp();
	}

	if(nullptr!=FsWeapon::aim9xs)
	{
		FsWeapon::aim9xs.CleanUp();
	}

	if(nullptr!=FsWeapon::aim9x_coarse)
	{
		FsWeapon::aim9x_coarse.CleanUp();
	}

	if(nullptr!=FsWeapon::flarePod)
	{
		FsWeapon::flarePod.CleanUp();
	}

	if (nullptr != FsWeapon::flare)
	{
		FsWeapon::flare.CleanUp();
	}

	if (nullptr != FsWeapon::flare_coarse)
	{
		FsWeapon::flare_coarse.CleanUp();
	}

	if(nullptr!=FsWeapon::fuelTank)
	{
		FsWeapon::fuelTank.CleanUp();
	}
}

YSRESULT FsWeaponHolder::Save(FILE *fp,FsSimulation *sim,int wpnPosPrecision,int wpnAttPrecision)
{
	if(toPlay!=NULL)
	{
		int i,nRec;
		char format[256];

		nRec=toPlay->GetNumRecord();
		fprintf(fp,"BULRECOR\n");

		fprintf(fp,"VERSION 4\n");
			//  Version 3 Added rocket
			//  Version 4 Added credit owner in both launch and kill records

		fprintf(fp,"NUMRECO %d\n",nRec);

		sprintf(format,"%s %s%d%s %s%d%s %s%d%s %s%d%s %s%d%s %s%d%s\n",
		    "%g %d",
		    "%.",wpnPosPrecision,"f",
            "%.",wpnPosPrecision,"f",
            "%.",wpnPosPrecision,"f",
            "%.",wpnAttPrecision,"f",
            "%.",wpnAttPrecision,"f",
            "%.",wpnAttPrecision,"f");


		for(i=0; i<nRec; i++)
		{
			int ownerId;
			double t;

			const FsWeaponRecord *rec=toPlay->GetElement(t,i);

			fprintf(fp,format, //  "%g %d %.2f %.2f %.2f %.2f %.2f %.2f\n",
			    t,rec->type,rec->x,rec->y,rec->z,rec->h,rec->p,rec->b);
			fprintf(fp,"%.2f %.2f %d ",
			    rec->velocity,rec->lifeRemain,rec->power);

			if((ownerId=sim->GetAirplaneIdFromHandle(rec->firedBy))>=0)
			{
				fprintf(fp,"A%d ",ownerId);
			}
			else if((ownerId=sim->GetGroundIdFromHandle(rec->firedBy))>=0)
			{
				fprintf(fp,"G%d ",ownerId);
			}
			else
			{
				fprintf(fp,"N ");
			}

			switch(rec->creditOwner)
			{
			default:
			case FSWEAPON_CREDIT_OWNER_NOT_RECORDED:
				fprintf(fp,"U\n");  // Unknown
				break;
			case FSWEAPON_CREDIT_OWNER_PLAYER:
				fprintf(fp,"P\n");  // Player
				break;
			case FSWEAPON_CREDIT_OWNER_NON_PLAYER:
				fprintf(fp,"N\n");  // Non-Player
				break;
			}

			if(rec->type==FSWEAPON_AIM9 || rec->type==FSWEAPON_AIM9X || rec->type==FSWEAPON_AIM120)
			{
				int targetId;
				targetId=sim->GetAirplaneIdFromHandle(rec->target);
				fprintf(fp,"%.2f %.2f %.2f %d\n",
				    rec->vMax,rec->mobility,rec->radar,targetId);
			}
			else if(rec->type==FSWEAPON_AGM65)
			{
				int targetId;
				targetId=-1;
				targetId=sim->GetGroundIdFromHandle(rec->target);
				fprintf(fp,"%.2f %.2f %.2f %d\n",
				    rec->vMax,rec->mobility,rec->radar,targetId);
			}
			else if(rec->type==FSWEAPON_ROCKET)
			{
				fprintf(fp,"%.2f\n",rec->vMax);
			}
		}

		if(killCredit->GetNumObject()>0)
		{
			YsList <FsKillCredit> *kcSeeker;
			fprintf(fp,"KILLCREDIT 1 %d\n",killCredit->GetNumObject());  // 1 : Version 1
			for(kcSeeker=killCredit; kcSeeker!=NULL; kcSeeker=kcSeeker->Next())
			{
				int whoKilled,whom;

				fprintf(fp,"%d ",kcSeeker->dat.byWhatKindOfWeapon);

				if((whoKilled=sim->GetAirplaneIdFromHandle(kcSeeker->dat.whoKilled))>=0)
				{
					fprintf(fp,"A%d ",whoKilled);
				}
				else if((whoKilled=sim->GetGroundIdFromHandle(kcSeeker->dat.whoKilled))>=0)
				{
					fprintf(fp,"G%d ",whoKilled);
				}
				else
				{
					fprintf(fp,"N ");
				}

				if((whom=sim->GetAirplaneIdFromHandle(kcSeeker->dat.whom))>=0)
				{
					fprintf(fp,"A%d ",whom);
				}
				else if((whom=sim->GetGroundIdFromHandle(kcSeeker->dat.whom))>=0)
				{
					fprintf(fp,"G%d ",whom);
				}
				else
				{
					fprintf(fp,"N ");
				}

				switch(kcSeeker->dat.creditOwner)
				{
				default:
				case FSWEAPON_CREDIT_OWNER_NOT_RECORDED:
					fprintf(fp,"U ");  // Unknown
					break;
				case FSWEAPON_CREDIT_OWNER_PLAYER:
					fprintf(fp,"P ");  // Player
					break;
				case FSWEAPON_CREDIT_OWNER_NON_PLAYER:
					fprintf(fp,"N ");  // Non-Player
					break;
				}

				fprintf(fp,"%.1lf %.1lf %.1lf %.1lf\n",
				    kcSeeker->dat.where.x(),
				    kcSeeker->dat.where.y(),
				    kcSeeker->dat.where.z(),
				    kcSeeker->dat.when);
			}
		}

		fprintf(fp,"ENDRECO\n");
	}
	return YSOK;
}

YSRESULT FsWeaponHolder::Load(FILE *fp,FsSimulation *sim)
{
	if(toSave!=NULL)
	{
		delete toSave;
		toSave=NULL;
	}
	if(toPlay!=NULL)
	{
		delete toPlay;
		toPlay=NULL;
	}
	killCredit->DeleteList();
	killCredit=NULL;

	toPlay=new FsRecord <FsWeaponRecord>;

	FsWeaponRecord rec;
	char buf[256];
	int version;
	version=-1;
	while(fgets(buf,256,fp)!=NULL)
	{
		if(strncmp(buf,"VERSION 1",9)==0)
		{
			version=1;
			continue;
		}
		else if(strncmp(buf,"VERSION 2",9)==0)
		{
			version=2;
			continue;
		}
		else if(strncmp(buf,"VERSION 3",9)==0)
		{
			version=3;
			continue;
		}
		else if(0==strncmp(buf,"VERSION 4",9))
		{
			version=4;
			continue;
		}
		else if(strncmp(buf,"NUMRECO",7)==0)
		{
			int i,nRec;
			nRec=atoi(buf+8);

			if(version>=4)
			{
				for(i=0; i<nRec; i++)
				{
					int type;
					char str[256],creditOwnerId;
					double t;

					fgets(buf,256,fp);
					sscanf(buf,"%lf%d%f%f%f%f%f%f",
					    &t,&type,&rec.x,&rec.y,&rec.z,&rec.h,&rec.p,&rec.b);

					rec.type=(FSWEAPONTYPE)type;

					fgets(buf,256,fp);
					sscanf(buf,"%f%f%d%s%c",&rec.velocity,&rec.lifeRemain,&rec.power,str,&creditOwnerId);

					rec.firedBy=FindObjectByAxxGxxN(str,sim);
					switch(creditOwnerId)
					{
					default:
					case 'U':
						rec.creditOwner=FSWEAPON_CREDIT_OWNER_NOT_RECORDED;
						break;
					case 'P':
						rec.creditOwner=FSWEAPON_CREDIT_OWNER_PLAYER;
						break;
					case 'N':
						rec.creditOwner=FSWEAPON_CREDIT_OWNER_NON_PLAYER;
						break;
					}

					rec.target=NULL;
					if(rec.type==FSWEAPON_GUN)
					{
						rec.vMax=rec.velocity;
						rec.mobility=0.0F;
						rec.target=NULL;
					}
					else if(rec.type==FSWEAPON_AIM9 || rec.type==FSWEAPON_AIM9X || rec.type==FSWEAPON_AIM120)
					{
						int id;
						fgets(buf,256,fp);
						sscanf(buf,"%f%f%f%d",
						    &rec.vMax,&rec.mobility,&rec.radar,&id);
						rec.target=sim->GetAirplaneById(id);
					}
					else if(rec.type==FSWEAPON_AGM65)
					{
						int id;
						fgets(buf,256,fp);
						sscanf(buf,"%f%f%f%d",
						    &rec.vMax,&rec.mobility,&rec.radar,&id);
						rec.target=sim->GetGroundById(id);
					}
					else if(rec.type==FSWEAPON_ROCKET)
					{
						fgets(buf,256,fp);
						sscanf(buf,"%f",&rec.vMax);
						rec.target=NULL;
						rec.mobility=0.0;
					}

					toPlay->AddElement(rec,t);
				}
			}
			else if(version>=2)
			{
				for(i=0; i<nRec; i++)
				{
					int type;
					char str[256];
					double t;

					fgets(buf,256,fp);
					sscanf(buf,"%lf%d%f%f%f%f%f%f",
					    &t,&type,&rec.x,&rec.y,&rec.z,&rec.h,&rec.p,&rec.b);

					rec.type=(FSWEAPONTYPE)type;

					fgets(buf,256,fp);
					sscanf(buf,"%f%f%d%s",&rec.velocity,&rec.lifeRemain,&rec.power,str);

					rec.firedBy=FindObjectByAxxGxxN(str,sim);
					rec.creditOwner=FSWEAPON_CREDIT_OWNER_NOT_RECORDED;

					rec.target=NULL;
					if(rec.type==FSWEAPON_GUN)
					{
						rec.vMax=rec.velocity;
						rec.mobility=0.0F;
						rec.target=NULL;
					}
					else if(rec.type==FSWEAPON_AIM9 || rec.type==FSWEAPON_AIM9X || rec.type==FSWEAPON_AIM120)
					{
						int id;
						fgets(buf,256,fp);
						sscanf(buf,"%f%f%f%d",
						    &rec.vMax,&rec.mobility,&rec.radar,&id);
						rec.target=sim->GetAirplaneById(id);
					}
					else if(rec.type==FSWEAPON_AGM65)
					{
						int id;
						fgets(buf,256,fp);
						sscanf(buf,"%f%f%f%d",
						    &rec.vMax,&rec.mobility,&rec.radar,&id);
						rec.target=sim->GetGroundById(id);
					}
					else if(rec.type==FSWEAPON_ROCKET)
					{
						fgets(buf,256,fp);
						sscanf(buf,"%f",&rec.vMax);
						rec.target=NULL;
						rec.mobility=0.0;
					}

					toPlay->AddElement(rec,t);
				}
			}
			else if(version==1)
			{
				for(i=0; i<nRec; i++)
				{
					int type;
					char str[256];
					FSWEAPONTYPE typeList[]=
					{
						FSWEAPON_GUN,
						FSWEAPON_AIM9,
						FSWEAPON_AGM65
					};
					double t;

					fgets(buf,256,fp);
					sscanf(buf,"%lf%d%f%f%f%f%f%f",
					    &t,&type,&rec.x,&rec.y,&rec.z,&rec.h,&rec.p,&rec.b);

					rec.type=typeList[type];
					if(rec.type==FSWEAPON_GUN)
					{
						rec.power=1;
					}
					else
					{
						rec.power=12;
					}

					fgets(buf,256,fp);
					sscanf(buf,"%f%f%s",
					    &rec.velocity,&rec.lifeRemain,str);

					rec.firedBy=FindObjectByAxxGxxN(str,sim);
					rec.creditOwner=FSWEAPON_CREDIT_OWNER_NOT_RECORDED;

					rec.target=NULL;
					if(rec.type==FSWEAPON_GUN)
					{
						rec.vMax=rec.velocity;
						rec.mobility=0.0F;
						rec.target=NULL;
					}
					else if(rec.type==FSWEAPON_AIM9)
					{
						int id;
						fgets(buf,256,fp);
						sscanf(buf,"%f%f%f%d",
						    &rec.vMax,&rec.mobility,&rec.radar,&id);
						rec.target=sim->GetAirplaneById(id);
					}
					else if(rec.type==FSWEAPON_AGM65)
					{
						int id;
						fgets(buf,256,fp);
						sscanf(buf,"%f%f%f%d",
						    &rec.vMax,&rec.mobility,&rec.radar,&id);
						//rec.target=sim->GetGroundObject(id);
					}

					toPlay->AddElement(rec,t);
				}
			}
		}
		else if(strncmp(buf,"KILLCREDIT",10)==0)
		{
			// 0  1  2  3  4  5  6  7  8  9 10 11 12  13
			// K  I  L  L  C  R  E  D  I  T  _  1  _  (#ofCredit)
			if(buf[11]=='1')
			{
				int i,nCredit;
				nCredit=atoi(buf+13);
				for(i=0; i<nCredit; i++)
				{
					char buf[256];
					char obj1[256],obj2[256],creditOwnerId;
					int type;
					double x,y,z,when;
					YsList <FsKillCredit> *neo;

					if(fgets(buf,256,fp)!=NULL)
					{
						if(4<=version)
						{
							sscanf(buf,"%d%s%s%c%lf%lf%lf%lf",&type,obj1,obj2,&creditOwnerId,&x,&y,&z,&when);
							neo=new YsList <FsKillCredit>;
							neo->dat.whoKilled=FindObjectByAxxGxxN(obj1,sim);
							neo->dat.whom=FindObjectByAxxGxxN(obj2,sim);

							switch(creditOwnerId)
							{
							default:
							case 'U':
								neo->dat.creditOwner=FSWEAPON_CREDIT_OWNER_NOT_RECORDED;
								break;
							case 'P':
								neo->dat.creditOwner=FSWEAPON_CREDIT_OWNER_PLAYER;
								break;
							case 'N':
								neo->dat.creditOwner=FSWEAPON_CREDIT_OWNER_NON_PLAYER;
								break;
							}

							neo->dat.byWhatKindOfWeapon=(FSWEAPONTYPE)type;
							neo->dat.where.Set(x,y,z);
							neo->dat.when=when;
						}
						else
						{
							sscanf(buf,"%d%s%s%lf%lf%lf%lf",&type,obj1,obj2,&x,&y,&z,&when);
							neo=new YsList <FsKillCredit>;
							neo->dat.whoKilled=FindObjectByAxxGxxN(obj1,sim);
							neo->dat.creditOwner=FSWEAPON_CREDIT_OWNER_NOT_RECORDED;
							neo->dat.whom=FindObjectByAxxGxxN(obj2,sim);
							neo->dat.byWhatKindOfWeapon=(FSWEAPONTYPE)type;
							neo->dat.where.Set(x,y,z);
							neo->dat.when=when;
						}
						killCredit=killCredit->Append(neo);
					}
					else
					{
						fsStderr.Printf("Unexpected End Of File while Reading kill credits.\n");
						return YSERR;
					}
				}
			}
			else
			{
				fsStderr.Printf("Incorrect Version Number.\n");
				fsStderr.Printf("Please download newest version and try again.\n");
				return YSERR;
			}
		}
		else if(strncmp(buf,"ENDRECO",7)==0)
		{
			break;
		}
		else
		{
			fsStderr.Printf("Unrecognized Bullet Record\n");
			return YSERR;
		}
	}

	return YSOK;
}

void FsWeaponHolder::Clear(void)
{
	int i;
	for(i=0; i<NumBulletBuffer; i++)
	{
		buf[i].lifeRemain=0.0;
		buf[i].bufPos = i;
		if(i==0)
		{
			buf[i].prev=NULL;
			buf[i].next=&buf[i+1];
		}
		else if(i==NumBulletBuffer-1)
		{
			buf[i].prev=&buf[i-1];
			buf[i].next=NULL;
		}
		else
		{
			buf[i].prev=&buf[i-1];
			buf[i].next=&buf[i+1];
		}
	}

	for(i=0; i<NumSmokeTrailBuffer; i++)
	{
		trl[i].used=YSFALSE;
	}

	weaponList = NULL;
	decoyList = NULL;
	bulletList = NULL;
	systemList = NULL;
	freeList=&buf[0];

	bulletCalibrator.Clear();
}

void FsWeaponHolder::MoveToActiveList(FsWeapon *wep)
{
	if (wep != NULL)
	{
		switch (wep->inList)
		{
		default:
		case FSENTITYLIST_WEAPON:
			MoveToWeaponList(wep);
			break;
		case FSENTITYLIST_DECOY:
			MoveToDecoyList(wep);
			break;
		case FSENTITYLIST_BULLET:
			MoveToBulletList(wep);
			break;
		case FSENTITYLIST_SYSTEM:
			MoveToSystemList(wep);
			break;
		}
	}
	else
	{
		printf("Tried to make NULL weapon active\n");
	}
}

void FsWeaponHolder::MoveToWeaponList(FsWeapon *wep)
{
	//Convention (for now 20260127 until we change to a better list system)
	//next is index i+1, prev is i-1. Next is always an older weapon
	//weaponList, decoyList, bulletList, systemList, freeList always point to the newest in the list

	//Patch hole in the previous list
	if (wep->next != NULL)
	{
		wep->next->prev = wep->prev;
	}
	if (wep->prev != NULL)
	{
		wep->prev->next = wep->next;
	}
	else
	{
		freeList = wep->next;
	}

	//Push the new list down one
	if (weaponList != NULL)
	{
		weaponList->prev = wep;
	}
	wep->next = weaponList;
	wep->prev = NULL;
	weaponList = wep;
}

void FsWeaponHolder::MoveToDecoyList(FsWeapon *wep)
{
	if (wep->next != NULL)
	{
		wep->next->prev = wep->prev;
	}
	if (wep->prev != NULL)
	{
		wep->prev->next = wep->next;
	}
	else
	{
		freeList = wep->next;
	}

	if (decoyList != NULL)
	{
		decoyList->prev = wep;
	}
	wep->next = decoyList;
	wep->prev = NULL;
	decoyList = wep;
}

void FsWeaponHolder::MoveToBulletList(FsWeapon* wep)
{
	if (wep->next != NULL)
	{
		wep->next->prev = wep->prev;
	}
	if (wep->prev != NULL)
	{
		wep->prev->next = wep->next;
	}
	else
	{
		freeList = wep->next;
	}

	if (bulletList != NULL)
	{
		bulletList->prev = wep;
	}
	wep->next = bulletList;
	wep->prev = NULL;
	bulletList = wep;
}

void FsWeaponHolder::MoveToSystemList(FsWeapon* wep)
{
	if (wep->next != NULL)
	{
		wep->next->prev = wep->prev;
	}
	if (wep->prev != NULL)
	{
		wep->prev->next = wep->next;
	}
	else
	{
		freeList = wep->next;
	}

	if (systemList != NULL)
	{
		systemList->prev = wep;
	}
	wep->next = systemList;
	wep->prev = NULL;
	systemList = wep;
}

void FsWeaponHolder::MoveToFreeList(FsWeapon *wep)
{
	FsWeapon *next, *prev;
	prev = wep->prev;
	next = wep->next;

	if (wep->next != NULL)
	{
		wep->next->prev = wep->prev;
	}
	if (wep->prev != NULL)
	{
		wep->prev->next = wep->next;
	}

	if (freeList != NULL)
	{
		freeList->prev = wep;
	}
	wep->next = freeList;
	wep->prev = NULL;
	freeList = wep;

	if (prev == NULL)
	{
		switch (wep->inList)
		{
		default:
		case FSENTITYLIST_WEAPON:
			weaponList = next;
			break;
		case FSENTITYLIST_DECOY:
			decoyList = next;
			break;
		case FSENTITYLIST_BULLET:
			bulletList = next;
			break;
		case FSENTITYLIST_SYSTEM:
			systemList = next;
			break;
		}
	}
}

void FsWeaponHolder::ClearBulletCalibrator(void)
{
	bulletCalibrator.Clear();
}

void FsWeaponHolder::CalculateBulletCalibrator(const FsExistence *target)
{
	ClearBulletCalibrator();
	for(FsWeapon *seeker = FindNextActiveBullet(NULL); seeker != NULL; seeker = FindNextActiveBullet(seeker))
	{
		const YsVec3 &prv=seeker->prv;
		const YsVec3 &pos=seeker->pos;

		YsVec3 nearPos;
		if(YSTRUE==YsCheckInBetween3(target->GetPosition(),pos,prv) &&
			YSOK==YsGetNearestPointOnLine3(nearPos,prv,pos,target->GetPosition()))
		{
			bulletCalibrator.Append(nearPos);
		}
	}
}

YSBOOL FsWeaponHolder::GiveDamage(YSBOOL &killed,FsExistence &obj,int destPower,FSDIEDOF diedOf,FsWeapon &wpn)
{
	if(&obj!=NULL && obj.GetDamage(killed,destPower,diedOf)==YSTRUE)
	{
		if(YSTRUE==killed && NULL!=netServer)
		{
			netServer->ReportKill(&obj,wpn.firedBy,wpn.type);
		}

		// Network Transmission
		if(wpn.firedBy!=NULL && wpn.firedBy->netType==FSNET_LOCAL)
		{
			if(NULL!=netServer)
			{
				netServer->BroadcastGetDamage(&obj,wpn.firedBy,destPower,diedOf);
			}
			if(NULL!=netClient)
			{
				netClient->SendGetDamage(&obj,wpn.firedBy,destPower,diedOf,wpn.type);
			}
		}
		else if(wpn.firedBy==NULL)  // Let's assume it's server's local.
		{
			if(netServer!=NULL)
			{
				netServer->BroadcastGetDamage(&obj,wpn.firedBy,destPower,diedOf);
			}
		}
		return YSTRUE;
	}
	else
	{
		return YSFALSE;
	}
}

YSRESULT FsWeaponHolder::RipOffEarlyPartOfRecord(void)
{
	if(toSave!=NULL)
	{
		toSave->RipOffEarlyPartOfRecord();
	}
	return YSOK;
}

YSRESULT FsWeaponHolder::DeleteRecordForResumeFlight(FsAirplane *shotBy,const double &startTime)
{
	int i,nRec;
	double t;

	FsRecord <FsWeaponRecord> *neo;

	neo=new FsRecord <FsWeaponRecord>;

	if(neo!=NULL)
	{
		nRec=toPlay->GetNumRecord();
		for(i=0; i<nRec; i++)
		{
			FsWeaponRecord *elem;
			elem=toPlay->GetElement(t,i);
			if(elem->firedBy==shotBy && startTime<=t)
			{
			}
			else
			{
				neo->AddElement(*elem,t);
			}
		}

		if(nRec!=neo->GetNumRecord())
		{
			delete toPlay;
			toPlay=neo;
		}
		else
		{
			delete neo;
		}
	}

	return YSOK;
}

YSRESULT FsWeaponHolder::RefreshOrdinanceByWeaponRecord(const double &currentTime)
{
	if(toPlay!=NULL)
	{
		int i,n;
		double t;
		FsWeaponRecord *rec;

		t=0.0;
		n=toPlay->GetNumRecord();
		for(i=0; i<n; i++)
		{
			rec=toPlay->GetElement(t,i);
			if(currentTime<t)
			{
				break;
			}

			if(rec!=NULL &&
			   rec->firedBy!=NULL &&
			   rec->firedBy->IsAirplane()==YSTRUE &&
			   rec->type!=FSWEAPON_GUN)
			{
				FsAirplane *air;
				air=(FsAirplane *)rec->firedBy;
				air->Prop().FireMissileByRecord(rec->type);
			}
		}
		return YSOK;
	}
	else
	{
		return YSERR;
	}
}

int FsWeaponHolder::LaunchWeapon(
	FsWeapon::FsWeaponPerformance &data,
	const double &ctime,
	YsVec3 &pos,
	YsAtt3 &att,
	const YsVec3 &iniVelocity,
	class FsExistence *owner,
	unsigned int airTarget,
	unsigned int gndTarget,
	YSBOOL recordIt,
	YSBOOL transmit)
{
	FsExistence* target = NULL;
	if (gndTarget != NULL && gndTarget != YSNULLHASHKEY)
	{
		target = sim->FindGround(gndTarget);
	}
	if (airTarget != NULL && airTarget != YSNULLHASHKEY) //Air targets get priority for weapons that track both air and gnd
	{
		target = sim->FindAirplane(airTarget);
	}
	
	FsWeaponSmokeTrail *trail;
	trail = NULL;
	if (freeList != NULL)
	{
		if (data.trailSmoke == YSTRUE)
		{
			
			int i;
			for (i = 0; i < NumSmokeTrailBuffer; i++)
			{
				if (trl[i].used != YSTRUE)
				{
					trl[i].used = YSTRUE;
					trail = &trl[i];
					break;
				}
			}
		}

		FsWeapon *toShoot = freeList;
		FSWEAPON_CREDIT_OWNER creditOwner = (sim->GetPlayerObject() == owner ? FSWEAPON_CREDIT_OWNER_PLAYER : FSWEAPON_CREDIT_OWNER_NON_PLAYER);
		if (owner == NULL)
		{
			creditOwner = FSWEAPON_CREDIT_OWNER_SYSTEM;
		}

		toShoot->LaunchWeapon(data, pos, att, iniVelocity.GetLength(), owner, creditOwner, target, trail);

		MoveToActiveList(toShoot);

		if ((recordIt == YSTRUE && toSave != NULL) ||
			(transmit == YSTRUE && (netServer != NULL || netClient != NULL)) &&
			toShoot->category != FSWEAPONCAT_SYSTEM)
		{
			FsWeaponRecord rec;
			rec.type = data.type;
			rec.x = float(pos.x());
			rec.y = float(pos.y());
			rec.z = float(pos.z());
			rec.h = float(att.h());
			rec.p = float(att.p());
			rec.b = float(att.b());
			rec.velocity = float(iniVelocity.GetLength());
			rec.lifeRemain = float(data.flightRange);
			rec.power = data.power;
			rec.firedBy = owner;
			rec.creditOwner = creditOwner;

			rec.vMax = float(data.maxSpeed);
			rec.mobility = float(data.turnRate);
			rec.radar = float(data.trackAngle);
			rec.target = target;

			if (recordIt == YSTRUE && toSave != NULL)
			{
				toSave->AddElement(rec, ctime);
			}
			if (transmit == YSTRUE && netServer != NULL)
			{
				netServer->BroadcastMissileLaunch(rec);
			}
			if (transmit == YSTRUE && netClient != NULL)
			{
				netClient->SendMissileLaunch(rec);
			}
		}
		return (int)(toShoot - buf);
	}
	return -1;
}

//Fire(), Bomb(), DispenseFlare() still have to exist or the libraries throw a fit
int FsWeaponHolder::Fire
    (const double &ctime,
     YsVec3 &pos,
     YsAtt3 &att,
     double v,
     double l,
     int destructivePower,
     FsExistence *owner,
     YSBOOL recordIt,YSBOOL /*transmit*/) 
{
	int res;
	FsWeapon temp;
	FsWeapon::FsWeaponPerformance perf;
	perf = temp.gunPerf;
	perf.flightRange = l;
	perf.power = destructivePower;
	YsVec3 tempVel;
	tempVel.Set(0.0, 0.0, v);
	att.Mul(tempVel, tempVel);
	res = LaunchWeapon(perf, ctime, pos, att, tempVel, owner, NULL, NULL, recordIt, YSFALSE);
	return res;
}

int FsWeaponHolder::Fire
    (const double &ctime,
     FSWEAPONTYPE missileType,
     YsVec3 &pos,YsAtt3 &att,
     double v,double vmax,double l,double mobility,double radar,
     int destructivePower,
     FsExistence *owner,unsigned int targetKey,
     YSBOOL recordIt,YSBOOL transmit, YSBOOL jettison)
{
	int res;
	FsWeapon temp;
	FsWeapon::FsWeaponPerformance perf;
	perf = temp.GetWeaponPerformanceByType(missileType);
	perf.flightRange = l;
	perf.power = destructivePower;
	perf.turnRate = mobility;
	perf.trackAngle = radar;
	perf.maxSpeed = vmax;
	YsVec3 tempVel;
	tempVel.Set(0.0, 0.0, v);
	att.Mul(tempVel, tempVel);
	res = LaunchWeapon(perf, ctime, pos, att, tempVel, owner, targetKey, targetKey, recordIt, YSTRUE);
	return res;
}

int FsWeaponHolder::Bomb(
    const double &ctime,
    FSWEAPONTYPE bombType,
    const YsVec3 &pos,const YsAtt3 &/*att*/,const YsVec3 &iniVelocity,
    const double &vMax,
    int destructivePower,
    class FsExistence *owner,
    YSBOOL recordIt,YSBOOL transmit)
{
	int res;
	FsWeapon temp;
	FsWeapon::FsWeaponPerformance perf;
	perf = temp.GetWeaponPerformanceByType(bombType);
	perf.power = destructivePower;
	perf.maxSpeed = vMax;
	YsAtt3 tempAtt;
	YsVec3 tempPos = pos;
	tempAtt.SetForwardVector(iniVelocity);
	res = LaunchWeapon(perf, ctime, tempPos, tempAtt, iniVelocity, owner, NULL, NULL, recordIt, YSTRUE);
	return res;
}

int FsWeaponHolder::DispenseFlare(
    const double &ctime,
    const YsVec3 &pos,const YsVec3 &vel,const double &vMax,const double &l,
    class FsExistence *owner,YSBOOL recordIt,YSBOOL transmit)
{
	int res;
	FsWeapon temp;
	FsWeapon::FsWeaponPerformance perf;
	perf = temp.flarePerf;
	perf.flightRange = l;
	YsAtt3 tempAtt;
	tempAtt.SetForwardVector(vel);
	YsVec3 tempPos = pos;
	res = LaunchWeapon(perf, ctime, tempPos, tempAtt, vel, owner, NULL, NULL, recordIt, YSFALSE);
	return res;
}

int FsWeaponHolder::ThrowDebris(const double &ctime,const YsVec3 &pos,const YsVec3 &vec,const double &l)
{
	if(freeList!=NULL)
	{
		FsWeapon *toShoot;
		FsWeapon::FsWeaponPerformance perf;
		YsVec3 tempVel, tempPos;
		YsAtt3 tempAtt;
		int res;
		toShoot=freeList;

		perf = toShoot->debrisPerf;
		perf.flightRange = l;
		tempPos = pos;
		tempVel = vec;
		tempAtt.Set(0.0,0.0,0.0);
		
		res = LaunchWeapon(perf, ctime, tempPos, tempAtt, tempVel, NULL, NULL, NULL, YSFALSE, YSFALSE);
		return res;
	}
	return -1;
}

void FsWeaponHolder::ThrowRandomDebris(const double &ctime,const YsVec3 &pos,const YsAtt3 &att,const double &l)
{
	YsVec3 debrisVec;
	debrisVec.Set(rand()&2047-1024,rand()&1023,rand()&2047-1024);
	debrisVec/=16.0;
	att.Mul(debrisVec,debrisVec);
	ThrowDebris(ctime,pos,debrisVec,l);
}

void FsWeaponHolder::ThrowMultiDebris(int n,const double &ctime,const YsVec3 &pos,const YsAtt3 &att,const double &l)
{
	while(n>0)
	{
		ThrowRandomDebris(ctime,pos,att,l);
		n--;
	}
}

YSBOOL FsWeaponHolder::IsLockedOn(const FsExistence *ex) const
{
	FsWeapon *seeker;
	for(seeker = FindNextActiveWeapon(NULL); seeker != NULL; seeker = FindNextActiveWeapon(seeker))
	{
		if(seeker->lifeRemain>YsTolerance && seeker->target==ex) // Need to check lifeRemain since smoke may remain for a few seconds after the missile runs out of its range.
		{
			return YSTRUE;
		}
	}
	return YSFALSE;
}

YSBOOL FsWeaponHolder::IsLockedOn(FSWEAPONTYPE &wpnType,YsVec3 &wpnPos,const FsExistence *ex) const
{
	FsWeapon *seeker;

	for(seeker = FindNextActiveWeapon(NULL); seeker != NULL; seeker = FindNextActiveWeapon(seeker))
	{
		if(seeker->lifeRemain>YsTolerance && seeker->target==ex) // 2014/10/30 lifeRemain check was missing and has been added.
		{
			wpnType=seeker->type;
			wpnPos=seeker->pos;
			return YSTRUE;
		}
	}
	return YSFALSE;
}

//returns a reference to the weapon currently pursuing ex, or NULL if none was found 
FsWeapon* FsWeaponHolder::GetLockedOn(const FsExistence* ex) const
{
	FsWeapon* seeker;
	for (seeker = FindNextActiveWeapon(NULL); seeker != NULL; seeker = FindNextActiveWeapon(seeker))
	{
		if (seeker->lifeRemain > YsTolerance && seeker->target == ex)
		{
			return seeker;
		}
	}
	return NULL;
}


YSRESULT FsWeaponHolder::FindFirstMissilePositionThatIsReallyGuided(YsVec3 &vec,YsAtt3 &att) const
{
	double maxLifeRemain;
	YSRESULT res;
	FsWeapon *seeker;

	maxLifeRemain=0.0;
	res=YSERR;
	
	for(seeker = FindNextActiveWeapon(NULL); seeker != NULL; seeker = FindNextActiveWeapon(seeker))
	{
		if(seeker->lifeRemain>maxLifeRemain)
		{
			if (((seeker->targetAir == YSTRUE || seeker->targetGnd == YSTRUE) && seeker->target != NULL) || seeker->category == FSWEAPONCAT_FREEFALL)
			{
				vec=seeker->pos;
				att=seeker->att;
				maxLifeRemain=seeker->lifeRemain;
				res=YSOK;
			}
		}
	}
	return res;
}

YSRESULT FsWeaponHolder::FindOldestMissilePosition(YsVec3 &vec,YsAtt3 &att,const FsExistence *fired) const
{
	YSRESULT res;
	FsWeapon *seeker;

	res=YSERR;
	for(seeker = FindNextActiveWeapon(NULL); seeker != NULL; seeker = FindNextActiveWeapon(seeker))
	{
		if(seeker->firedBy==fired && seeker->lifeRemain>YsTolerance)
		{
			if (((seeker->targetAir == YSTRUE || seeker->targetGnd == YSTRUE) && seeker->target != NULL) || seeker->category == FSWEAPONCAT_FREEFALL)
			{
				vec=seeker->pos;
				att=seeker->att;
				res=YSOK;
			}
		}
	}
	return res;
}

YSRESULT FsWeaponHolder::FindNewestMissilePosition(YsVec3 &vec,YsAtt3 &att,const FsExistence *fired) const
{
	FsWeapon *seeker;
	for(seeker = FindNextActiveWeapon(NULL); seeker != NULL; seeker = FindNextActiveWeapon(seeker))  // First one in the active list is the newest one.
	{
		if(seeker->firedBy==fired && seeker->lifeRemain>YsTolerance)
		{
			if (((seeker->targetAir == YSTRUE || seeker->targetGnd == YSTRUE) && seeker->target != NULL) || seeker->category == FSWEAPONCAT_FREEFALL)
			{
				vec=seeker->pos;
				att=seeker->att;
				return YSOK;
			}
		}
	}
	return YSERR;
}

FsWeapon *FsWeaponHolder::FindNextActiveEntity(const FsWeapon *wpn) const
{
	FsWeapon* result = NULL;
	const FsWeapon* search = wpn;
	FSENTITYLIST lst = FSENTITYLIST_WEAPON;

	if (weaponList == NULL && decoyList == NULL && bulletList == NULL && systemList == NULL)
	{
		result == NULL;
	}
	else
	{
		if (wpn != NULL)
		{
			lst = wpn->inList;
		}
		
		switch (lst)
		{
		default:
		case FSENTITYLIST_WEAPON:
			result = FindNextActiveWeapon(search);
			if (result != NULL)
			{
				break;
			}
			search = NULL;
		case FSENTITYLIST_DECOY:
			result = FindNextActiveDecoy(search);
			if (result != NULL)
			{
				break;
			}
			search = NULL;
		case FSENTITYLIST_BULLET:
			result = FindNextActiveBullet(search);
			if (result != NULL)
			{
				break;
			}
			search = NULL;
		case FSENTITYLIST_SYSTEM:
			result = FindNextActiveSystem(search);
			break;
		}
	}
	
	return result;
}

FsWeapon *FsWeaponHolder::FindNextActiveWeapon(const FsWeapon *wpn) const
{
	FsWeapon *result = NULL;
	if (wpn == NULL)
	{
		result = weaponList;
	}
	else if (wpn->inList != FSENTITYLIST_WEAPON)
	{
		result = NULL;
	}
	else
	{
		result = wpn->next;
	}

	return result;
}

FsWeapon *FsWeaponHolder::FindNextActiveDecoy(const FsWeapon *wpn) const
{
	FsWeapon *result = NULL;
	if (wpn == NULL)
	{
		result = decoyList;
	}
	else if (wpn->inList != FSENTITYLIST_DECOY)
	{
		result = NULL;
	}
	else
	{
		result = wpn->next;
	}

	return result;
}

FsWeapon *FsWeaponHolder::FindNextActiveBullet(const FsWeapon *wpn) const
{
	FsWeapon *result = NULL;
	if (wpn == NULL)
	{
		result = bulletList;
	}
	else if (wpn->inList != FSENTITYLIST_BULLET)
	{
		result = NULL;
	}
	else
	{
		result = wpn->next;
	}

	return result;
}

FsWeapon *FsWeaponHolder::FindNextActiveSystem(const FsWeapon *wpn) const
{
	FsWeapon *result = NULL;
	if (wpn == NULL)
	{
		result = systemList;
	}
	else if (wpn->inList != FSENTITYLIST_SYSTEM)
	{
		result = NULL;
	}
	else
	{
		result = wpn->next;
	}

	return result;
}

const FsWeapon *FsWeaponHolder::GetWeapon(int id) const
{
	if(0<=id && id<NumBulletBuffer)
	{
		return &buf[id];
	}
	else
	{
		return NULL;
	}
}

void FsWeaponHolder::ObjectIsDeleted(FsExistence *obj) const
{
	FsWeapon *wpn;

	for(wpn = FindNextActiveEntity(NULL); wpn != NULL; wpn = FindNextActiveEntity(wpn))
	{
		if(wpn->firedBy==obj)
		{
			wpn->firedBy=NULL;
		}
		if(wpn->target==obj)
		{
			wpn->target=NULL;
		}
	}
}

void FsWeaponHolder::Move(const double &dt,const double &cTime,const FsWeather &weather)
{
	FsWeapon *seeker,*nxt, *prv = NULL;

	for (seeker = FindNextActiveEntity(NULL); seeker != NULL; seeker = FindNextActiveEntity(nxt))
	{
		seeker->Move(dt, cTime, weather, decoyList);
		
		if (seeker->lifeRemain <= YsTolerance && seeker->timeRemain <= YsTolerance)
		{
			MoveToFreeList(seeker);
			nxt = prv;
		}
		else
		{
			prv = seeker;
			nxt = seeker;
		}
	}
}

void FsWeaponHolder::HitSomething(const double &ctime, const class FsField &field,
	class FsExplosionHolder *exp, class FsSimulation *sim, const double &tallestGroundObjectHeight) //20260127
{
	int nAir, nGnd;
	int list = 0;
	FsAirplane *air;
	FsGround *gnd;
	FsWeapon *seeker;

	nAir = sim->GetNumAirplane();
	nGnd = sim->GetNumGround();

	for (seeker = FindNextActiveEntity(NULL); seeker != NULL; seeker = FindNextActiveEntity(seeker))
	{
		YsArray <FsAirplane*, 256> airCandidate;
		YsArray <FsGround*, 256> gndCandidate;
		sim->GetLattice().GetAirCollisionCandidate(airCandidate, seeker->pos, seeker->lastChecked);
		sim->GetLattice().GetGndCollisionCandidate(gndCandidate, seeker->pos, seeker->lastChecked);

		//Hit vehicle
		for (int j = 0; j < airCandidate.GetN(); j++)
		{
			air = airCandidate[j];
			if (air->Prop().IsAlive() == YSTRUE)
			{
				if (seeker->HitObject(this, ctime, *air, exp, sim, killCredit) == YSTRUE)
				{
					if (air->Prop().IsActive() == YSTRUE)
					{
						if (air == sim->GetPlayerAirplane())
						{
							FsSoundSetOneTime(FSSND_ONETIME_DAMAGE);
						}
						else
						{
							FsSoundSetOneTime(FSSND_ONETIME_HIT);
						}
					}
					else
					{
						FsSoundSetOneTime(FSSND_ONETIME_BLAST);
					}
				}
			}
		}

		//Hit object
		if ((seeker->pos.y() < tallestGroundObjectHeight || seeker->prv.y() < tallestGroundObjectHeight)
			&& seeker->lifeRemain > 0.0)
		{
			for (int j = 0; j < gndCandidate.GetN(); j++)
			{
				gnd = gndCandidate[j];
				if (gnd->Prop().IsAlive() == YSTRUE)
				{
					if (seeker->HitObject(this, ctime, *gnd, exp, sim, killCredit) == YSTRUE)
					{
						if (gnd->Prop().IsAlive() == YSTRUE)
						{
							FsSoundSetOneTime(FSSND_ONETIME_HIT);
						}
						else
						{
							FsSoundSetOneTime(FSSND_ONETIME_BLAST);
						}
					}

				}
			}
		}

		//Hit ground
		seeker->HitGround(this, ctime, field, exp, sim, killCredit);

		seeker->lastChecked = seeker->pos;
		//if (seeker->lifeRemain <= YsTolerance && seeker->timeRemain <= YsTolerance)
		//{
		//	MoveToFreeList(seeker);
		//}
	}
}

void FsWeaponHolder::Draw(
    YSBOOL coarse,const YsMatrix4x4 &viewMat,const YsMatrix4x4 &projMat,
    YSBOOL transparency,FSSMOKETYPE smk,const double &cTime,unsigned int drawFlag) const
{
	FsWeapon *seeker;

	for (seeker = FindNextActiveEntity(NULL); seeker != NULL; seeker = FindNextActiveEntity(seeker))
	{
		if (seeker->lifeRemain > YsTolerance)
		{
			seeker->Draw(coarse, viewMat, projMat, transparency, smk, cTime, drawFlag);
		}
	}
}

void FsWeaponHolder::CollectRecord(void)
{
	FsRecord <FsWeaponRecord> *neo;

	neo=new FsRecord <FsWeaponRecord>;
	//fsConsole.Printf("  A\n");
	if(neo!=NULL)
	{
		int nToSave,nToPlay;
		nToSave=(toSave!=NULL ? toSave->GetNumRecord() : 0);
		nToPlay=(toPlay!=NULL ? toPlay->GetNumRecord() : 0);

		int i,j;
		i=0;
		j=0;
		//fsConsole.Printf("  B\n");
		while(i<nToSave || j<nToPlay)
		{
			FsWeaponRecord *s,*p;
			double st=0.0,pt=0.0;

			s=(toSave!=NULL ? toSave->GetElement(st,i) : NULL);
			p=(toPlay!=NULL ? toPlay->GetElement(pt,j) : NULL);

			if(s!=NULL && p==NULL)
			{
				neo->AddElement(*s,st);
				i++;
			}
			else if(s==NULL && p!=NULL)
			{
				neo->AddElement(*p,pt);
				j++;
			}
			else if(st<pt)
			{
				neo->AddElement(*s,st);
				i++;
			}
			else
			{
				neo->AddElement(*p,pt);
				j++;
			}

			if(p==NULL && s==NULL)
			{
				fsStderr.Printf("Something Unexpected Happens.\n");
				break;
			}
		}
		//fsConsole.Printf("  C\n");

		if(toSave!=NULL)
		{
			delete toSave;
			toSave=NULL;
		}
		if(toPlay!=NULL)
		{
			delete toPlay;
			toPlay=NULL;
		}

		//fsConsole.Printf("  D\n");
		toSave=new FsRecord <FsWeaponRecord>;;
		toPlay=neo;
	}
	//fsConsole.Printf("  E\n");
}

void FsWeaponHolder::PlayRecord(const double &t1,const double &dt)
{
	YSSIZE_T i,i1,i2;
	double t2,t;
	t2=t1+dt;

	if(toPlay!=NULL)
	{
		if(toPlay->GetIndexByTime(i1,i2,t1,t2)==YSOK)
		{
			for(i=i1; i<=i2; i++)
			{
				FsWeaponRecord *rec;
				rec=toPlay->GetElement(t,i);

				if(rec!=NULL)
				{
					LaunchRecord(*rec,t,YSFALSE,YSFALSE);   // No Record, No Transmit
				}
			}
		}
	}
}

void FsWeaponHolder::LaunchRecord(const FsWeaponRecord &rec,const double &t,YSBOOL recordIt,YSBOOL transmit)
{
	YsVec3 pos;
	YsAtt3 att;
	pos.Set(rec.x,rec.y,rec.z);
	att.Set(rec.h,rec.p,rec.b);

	if(rec.type==FSWEAPON_GUN)
	{
		Fire
		    (t,
		     pos,
		     att,
		     rec.velocity,
		     rec.lifeRemain,
		     rec.power,
		     rec.firedBy,
		     recordIt,
		     transmit);
	}
	else if(rec.type==FSWEAPON_FLARE)
	{
		YsVec3 vel;
		vel=att.GetForwardVector();
		vel*=double(rec.velocity);
		DispenseFlare
		    (t,
		     pos,
		     vel,
		     rec.vMax,
		     rec.lifeRemain,
		     rec.firedBy,
		     recordIt,
		     transmit);
	}
	else
	{
		YSHASHKEY targetKey=(NULL!=rec.target ? FsExistence::GetSearchKey(rec.target) : YSNULLHASHKEY);
		Fire
		    (t,
		     rec.type,
		     pos,
		     att,
		     rec.velocity,
		     rec.vMax,
		     rec.lifeRemain,
		     rec.mobility,
		     rec.radar,
		     rec.power,
		     rec.firedBy,
		     targetKey,
		     recordIt,
		     transmit);

		if(rec.firedBy!=NULL && rec.firedBy->IsAirplane()==YSTRUE)
		{
			FsAirplane *air;
			air=(FsAirplane *)rec.firedBy;
			air->Prop().FireMissileByRecord(rec.type);
		}
	}
}

FsExistence *FsWeaponHolder::FindObjectByAxxGxxN(const char str[],const class FsSimulation *sim)
{
	if(str[0]=='A')
	{
		return sim->GetAirplaneById(atoi(str+1));
	}
	if(str[0]=='G')
	{
		return sim->GetGroundById(atoi(str+1));
	}
	else
	{
		return NULL;
	}
}


unsigned FsEncodeWeaponRecord(unsigned char dat[],FsSimulation *,const FsWeaponRecord &rec)
{
	unsigned char *ptr;
	ptr=dat;

	FsPushInt(ptr,FSNETCMD_MISSILELAUNCH);
	FsPushShort(ptr,(short)rec.type);
	FsPushFloat(ptr,rec.x);
	FsPushFloat(ptr,rec.y);
	FsPushFloat(ptr,rec.z);
	FsPushFloat(ptr,rec.h);
	FsPushFloat(ptr,rec.p);
	FsPushFloat(ptr,rec.b);
	FsPushFloat(ptr,rec.velocity);
	FsPushFloat(ptr,rec.lifeRemain);
	FsPushShort(ptr,(short)rec.power);

	int idOnSvr,type;

	idOnSvr=-1;
	type=0;
	if(rec.firedBy!=NULL)
	{
		if(rec.firedBy->GetType()==FSEX_AIRPLANE)
		{
			type=0;  // Airplane
			idOnSvr=FsExistence::GetSearchKey(rec.firedBy);
		}
		else if(rec.firedBy->GetType()==FSEX_GROUND)
		{
			type=1;
			idOnSvr=FsExistence::GetSearchKey(rec.firedBy);
		}
		else
		{
			type=0;
			idOnSvr=-1;
		}
	}
	FsPushInt(ptr,type);   // Airplane
	FsPushInt(ptr,idOnSvr);  // NULL


	if(rec.type==FSWEAPON_AGM65 ||
	   rec.type==FSWEAPON_AIM9 ||
	   rec.type==FSWEAPON_AIM9X ||
	   rec.type==FSWEAPON_AIM120 || // 2005/03/03
	   rec.type==FSWEAPON_ROCKET)
	{
		FsPushFloat(ptr,rec.vMax);
		FsPushFloat(ptr,rec.mobility);
		FsPushFloat(ptr,rec.radar);

		idOnSvr=-1;
		type=0;
		if(rec.target!=NULL)
		{
			if(rec.target->GetType()==FSEX_AIRPLANE)
			{
				type=0;
				idOnSvr=FsExistence::GetSearchKey(rec.target);
			}
			else if(rec.target->GetType()==FSEX_GROUND)
			{
				type=1;
				idOnSvr=FsExistence::GetSearchKey(rec.target);
			}
			else
			{
				idOnSvr=-1;
				type=0;
			}
		}
		FsPushInt(ptr,type);   // Airplane
		FsPushInt(ptr,idOnSvr);  // NULL
	}
	else if(rec.type==FSWEAPON_FLARE)
	{
		FsPushFloat(ptr,rec.vMax);
	}

	return (unsigned int)(ptr-dat);
}

YSRESULT FsDecodeWeaponRecord
    (FsWeaponRecord &rec,
     int &firedBy,YSBOOL &firedByAirplane,int &firedAt,YSBOOL &firedAtAirplane,
     unsigned char dat[],FsSimulation *)
{
	const unsigned char *ptr=dat;

	FsPopInt(ptr);
	rec.type=(FSWEAPONTYPE)FsPopShort(ptr);
	rec.x=FsPopFloat(ptr);
	rec.y=FsPopFloat(ptr);
	rec.z=FsPopFloat(ptr);
	rec.h=FsPopFloat(ptr);
	rec.p=FsPopFloat(ptr);
	rec.b=FsPopFloat(ptr);
	rec.velocity=FsPopFloat(ptr);
	rec.lifeRemain=FsPopFloat(ptr);
	rec.power=FsPopShort(ptr);

	firedByAirplane=(FsPopInt(ptr)==0 ? YSTRUE : YSFALSE);
	firedBy=FsPopInt(ptr);

	if(rec.type==FSWEAPON_AGM65 ||
	   rec.type==FSWEAPON_AIM9 ||
	   rec.type==FSWEAPON_AIM9X ||
	   rec.type==FSWEAPON_AIM120 || // 2005/03/03
	   rec.type==FSWEAPON_ROCKET)
	{
		rec.vMax=FsPopFloat(ptr);
		rec.mobility=FsPopFloat(ptr);
		rec.radar=FsPopFloat(ptr);

		firedAtAirplane=(FsPopInt(ptr)==0 ? YSTRUE : YSFALSE);
		firedAt=FsPopInt(ptr);
	}
	else if(rec.type==FSWEAPON_FLARE)
	{
		rec.vMax=FsPopFloat(ptr);
	}

	return YSOK;
}

void FsWeaponHolder::SetNetServer(class FsSocketServer *svr)
{
	netServer=svr;
}

void FsWeaponHolder::SetNetClient(class FsSocketClient *cli)
{
	netClient=cli;
}


int FsGetDefaultWeaponLoadingUnit(FSWEAPONTYPE wpnType)
{
	switch(wpnType)
	{
	default:
		return 1;
	case FSWEAPON_ROCKET:
		return 19;
	case FSWEAPON_FLARE:
		return 20;
	}
}

int FsGetDefaultSubUnitPerLoadingUnit(FSWEAPONTYPE wpnType)
{
	switch(wpnType)
	{
	default:
		return 1;
	case FSWEAPON_FUELTANK:
		return 800;
	}
}

FSWEAPONTYPE FsGetWeaponTypeByString(const char strIn[])
{
	YsString str(strIn);
	int i;
	for(i=0; i<str.Strlen(); i++)
	{
		if(str[i]=='*' || str[i]=='&' || str[i]=='|' || str[i]=='@')
		{
			str.SetLength(i);
			break;
		}
	}

	if(strcmp(str,"AIM9")==0)
	{
		return FSWEAPON_AIM9;
	}
	if(strcmp(str,"AGM65")==0)
	{
		return FSWEAPON_AGM65;
	}
	if(strcmp(str,"B500")==0)
	{
		return FSWEAPON_BOMB;
	}
	if(strcmp(str,"RKT")==0)
	{
		return FSWEAPON_ROCKET;
	}
	if(strcmp(str,"FLR")==0)
	{
		return FSWEAPON_FLARE;
	}
	if(strcmp(str,"AIM120")==0)
	{
		return FSWEAPON_AIM120;
	}
	if(strcmp(str,"B250")==0)
	{
		return FSWEAPON_BOMB250;
	}
	if(strcmp(str,"GUN")==0)
	{
		return FSWEAPON_GUN;
	}
	if(strcmp(str,"SMK")==0)
	{
		return FSWEAPON_SMOKE;
	}
	if(strcmp(str,"B500HD")==0)
	{
		return FSWEAPON_BOMB500HD;
	}
	if(strcmp(str,"AIM9X")==0)
	{
		return FSWEAPON_AIM9X;
	}
	if(strcmp(str,"IFLR")==0)
	{
		return FSWEAPON_FLARE_INTERNAL;
	}
	if(strcmp(str,"FUEL")==0)
	{
		return FSWEAPON_FUELTANK;
	}

	return FSWEAPON_NULL;
}

const char *FsGetWeaponString(FSWEAPONTYPE wpnType)
{
	switch(wpnType)
	{
	case FSWEAPON_GUN:
		return "GUN";
	case FSWEAPON_AIM9:
		return "AIM9";
	case FSWEAPON_AGM65:
		return "AGM65";
	case FSWEAPON_BOMB:
		return "B500";
	case FSWEAPON_ROCKET:
		return "RKT";
	case FSWEAPON_FLARE:
		return "FLR";
	case FSWEAPON_AIM120:
		return "AIM120";
	case FSWEAPON_BOMB250:
		return "B250";
	case FSWEAPON_SMOKE:
		return "SMK";
	case FSWEAPON_BOMB500HD:
		return "B500HD";
	case FSWEAPON_AIM9X:
		return "AIM9X";
	case FSWEAPON_FLARE_INTERNAL:
		return "IFLR";
	case FSWEAPON_FUELTANK:
		return "FUEL";
	default:
		return "NULL";
	}
}

void FsWeaponHolder::AddToParticleManager(class YsGLParticleManager &partMan,const double cTime) const
{
	FSENVIRONMENT env = sim->GetEnvironment();

	for(auto seeker=weaponList; seeker!=NULL; seeker=seeker->next)
	{
		seeker->AddToParticleManager(partMan,cTime, env);
	}
	for (auto seeker = decoyList; seeker != NULL; seeker = seeker->next)
	{
		seeker->AddToParticleManager(partMan, cTime, env);
	}
}
