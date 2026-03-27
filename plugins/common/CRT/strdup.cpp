#include "crt.hpp"
#pragma warning (disable : 4005)
#include <windows.h>

char * __cdecl strdup(const char *block)
{
  char *result = (char *)malloc((lstrlenA(block)+1));
  if (!result)
    return NULL;
  lstrcpyA(result,block);
  return result;
}

wchar_t * __cdecl wcsdup(const wchar_t *block)
{
  wchar_t *result = (wchar_t *)malloc((lstrlenW(block)+1)*sizeof(wchar_t));
  if (!result)
    return NULL;
  lstrcpyW(result,block);
  return result;
}
