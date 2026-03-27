#include "crt.hpp"

_CONST_RETURN char * __cdecl strchr(register const char *s, int c)
{
  do
  {
    if(*s==(char)c)
      return s;
  } while (*s++);
  return 0;
}

_CONST_RETURN_W wchar_t * __cdecl wcschr(register const wchar_t *s, wchar_t c)
{
  do
  {
    if(*s==c)
      return s;
  } while (*s++);
  return 0;
}
