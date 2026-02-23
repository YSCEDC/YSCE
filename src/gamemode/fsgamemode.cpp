#include "fssimulation.h"
#include "fsgamemode.h"



FsSimExtensionBase::LowerLeftAndBitmap::LowerLeftAndBitmap(const LowerLeftAndBitmap &incoming)
{
	this->pos=incoming.pos;
	this->bmp=incoming.bmp;
}

FsSimExtensionBase::LowerLeftAndBitmap::LowerLeftAndBitmap(LowerLeftAndBitmap &&incoming)
{
	this->pos=incoming.pos;
	this->bmp.MoveFrom(incoming.bmp);
}

FsSimExtensionBase::LowerLeftAndBitmap &FsSimExtensionBase::LowerLeftAndBitmap::operator=(const LowerLeftAndBitmap &incoming)
{
	this->pos=incoming.pos;
	this->bmp=incoming.bmp;
	return *this;
}

FsSimExtensionBase::LowerLeftAndBitmap &FsSimExtensionBase::LowerLeftAndBitmap::operator=(LowerLeftAndBitmap &&incoming)
{
	this->pos=incoming.pos;
	this->bmp.MoveFrom(incoming.bmp);
	return *this;
}

const char *FsSimExtensionBase::FirstCommand(void) const
{
	return "EXTENSIO ";
}

////////////////////////////////////////////////////////////


#include "fsgamemode_racing.h"
#include "fsgamemode_endurance.h"
#include "fsgamemode_intercept.h"
#include "fsgamemode_closeairsupport.h"
#include "fsgamemode_groundtoair.h"



void FsSimExtensionRegistry::RegisterKnownExtension(void)
{
	RegisterExtension(FsSimExtension_RacingMode::Ident(),FsSimExtension_RacingMode::Generator());
	RegisterExtension(FsSimExtension_EnduranceMode::Ident(),FsSimExtension_EnduranceMode::Generator());
	RegisterExtension(FsSimExtension_InterceptMission::Ident(),FsSimExtension_InterceptMission::Generator());
	RegisterExtension(FsSimExtension_CloseAirSupport::Ident(),FsSimExtension_CloseAirSupport::Generator());
	RegisterExtension(FsSimExtension_GroundToAir::Ident(),FsSimExtension_GroundToAir::Generator());
}

void FsSimExtensionRegistry::RegisterExtension(const char ident[],std::function <std::shared_ptr <FsSimExtensionBase>(void)> generator)
{
	IdentAndGenerator reg;
	reg.ident=ident;
	reg.generator=generator;
	extensionRegistry.push_back(reg);
}

std::shared_ptr <FsSimExtensionBase> FsSimExtensionRegistry::Create(const char ident[])
{
	for(auto &reg : extensionRegistry)
	{
		if(0==reg.ident.Strcmp(ident))
		{
			return reg.generator();
		}
	}
	return nullptr;
}



