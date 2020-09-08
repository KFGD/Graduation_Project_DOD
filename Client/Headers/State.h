#pragma once

#include "Base.h"
#include "Defines.h"

template<typename T>
class State :
	public Base
{
protected:
	explicit State(T* exectuer)
		: mExecuter(exectuer) {}
	virtual ~State() {}

public:
	virtual void		Start()	= 0;
	virtual	void		Update(const _double timeDelta) = 0;
	virtual	void		LateUpdate(const _double timeDelta) = 0;

	virtual bool		CanStop() const = 0;
	virtual void		Stop() {}

protected:
	T*		GetExecuter() { return mExecuter; }

private:
	T*		mExecuter = nullptr;

public:
	virtual void	Free() override {}
};

