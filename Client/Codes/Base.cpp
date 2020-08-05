#include "stdafx.h"
#include "Base.h"

Base::Base()
	: mRefCnt(0)
{
}

unsigned long Base::AddRef()
{
	return ++mRefCnt;
}

unsigned long Base::Release()
{
	if (0 == mRefCnt)
	{
		Free();

		delete this;

		return 0;
	}

	return mRefCnt--;
}
