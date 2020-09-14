#pragma once

typedef		bool					_bool;

typedef		signed char				_char;
typedef		signed char				_byte;
typedef		unsigned char			_ubyte;

typedef		wchar_t					_tchar;


typedef		signed short			_short;
typedef		unsigned short			_ushort;

typedef		signed int				_int;
typedef		unsigned int			_uint;

typedef		signed long				_long;
typedef		unsigned long			_ulong;

typedef		float					_float;
typedef		double					_double;

#ifdef _WIN64
typedef		unsigned __int64		_size_t;
#else
typedef		unsigned int			_size_t;
#endif
typedef		unsigned int			_uniqueId;
typedef		unsigned int			_comCode;

#include <d3dx9math.h>
typedef		D3DXVECTOR3				_vec3;
typedef		D3DXVECTOR2				_vec2;
typedef		D3DXVECTOR4				_vec4;

typedef		D3DXMATRIX				_matrix;
