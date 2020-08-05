#pragma once

class Base abstract
{
protected:
	explicit Base();
	virtual	~Base() = default;

public:
	unsigned long AddRef();
	unsigned long Release();

private:
	unsigned long mRefCnt;

public:
	virtual void Free() = 0;
};