#include "crt.hpp"

#ifdef __cplusplus
extern "C" {
#endif
  int _fltused = 0;

  #pragma function(_dclass)
  short __cdecl _dclass(_In_ double _X) {return 0;}
  #pragma function(_ldclass)
  short __cdecl _ldclass(_In_ long double _X) {return 0;}
  #pragma function(_fdclass)
  short __cdecl _fdclass(_In_ float _X) {return 0;}
#ifdef __cplusplus
}
#endif

#ifdef _MSC_VER
#pragma function(ceilf)
#endif
float ceilf(float X)
{
	int Y = (int)X;
	return (float)(Y < X ? Y+1 : Y);
}
