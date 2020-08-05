#pragma once
#include "Base.h"

#include "Defines.h"

class Frame;

class FrameManager :
	public Base
{
	DECLARE_SINGLETON(FrameManager)

private:
	explicit FrameManager();
	virtual ~FrameManager() = default;

public:
	_bool	IsPermitCall(const char* const frameTag, const _double timeDelta) { return IsPermitCall(HashCode(frameTag), timeDelta); }
	_bool	IsPermitCall(const _uint frameHash, const _double timeDelta);

public:
	_bool	ReadyFrame(const char* frameTag, const _double callLimit) { return ReadyFrame(HashCode(frameTag), callLimit); }
	_bool	ReadyFrame(const _uint frameHash, const _double callLimit);

private:
	map<const _uint, Frame*>	mFrameGroup;
	typedef map<const _uint, Frame*>	FRAMEGROUP;
	typedef pair<const _uint, Frame*>	FRAMEPAIR;

private:
	Frame*	FindFrame(const _uint frameHash);

public:
	virtual void Free()	override;

};

