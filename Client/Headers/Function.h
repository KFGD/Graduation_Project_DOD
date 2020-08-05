#pragma once

#include "Typedef.h"

template <typename T>
inline unsigned long SafeAddRef(T& pInstance)
{
	unsigned long dwRefCnt = 0;

	if (nullptr != pInstance)
	{
		dwRefCnt = pInstance->AddRef();
	}

	return dwRefCnt;
}


template <typename T>
inline unsigned long SafeRelease(T& pInstance)
{
	unsigned long dwRefCnt = 0;
	if (nullptr != pInstance)
	{

		dwRefCnt = pInstance->Release();
		if (dwRefCnt == 0)
			pInstance = nullptr;
	}
	return dwRefCnt;
}

template <typename T>
inline void SafeDelete(T& pointer)
{
	if (nullptr != pointer)
	{
		delete pointer;
		pointer = nullptr;
	}
}

template <typename T>
inline void SafeDeleteArray(T& pointer)
{
	if (NULL != pointer)
	{
		delete[] pointer;
		pointer = NULL;
	}
}

inline constexpr _uint HashCode(const char* const str)
{
	return str[0] ? static_cast<_uint>(str[0]) + 0xEDB8832Full * HashCode(str + 1) : 8603;
}
