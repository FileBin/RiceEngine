#pragma once

namespace Game {
	#define _DELETE(p)		{ if(p){delete (p); (p)=nullptr;} }
	#define _DELETE_ARRAY(p){ if(p){delete[] (p); (p)=nullptr;} }
	#define _RELEASE(p)		{ if(p){(p)->Release(); (p)=nullptr;} }
	#define _CLOSE(p)		{ if(p){(p)->Close(); delete (p); (p)=nullptr;} }
	#define QUERY_INTERFACE(_obj, _interface) reinterpret_cast<_interface&>(_obj)

#define sign(f) (signbit(f) ? 1 : -1)
}