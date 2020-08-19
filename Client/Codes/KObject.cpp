#include "stdafx.h"
#include "KObject.h"


KObject::KObject(const Info& info)
	: mInfo(info)
{
}

const char * KObject::GetObjectTypeName() const
{
	switch (mInfo.Objecttype) 
	{
	case Type::Player:
		return "Player";

	case Type::Bot:
		return "Bot";

	default:
		return "";
	}
}

KObject * KObject::Create(const Info & objectInfo)
{
	return new KObject(objectInfo);
}

void KObject::Free()
{
}
