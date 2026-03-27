#include "crt.hpp"

_CONST_RETURN char * __cdecl strrchr(const char *string, int ch)
{
  const char *start = string;

  while (*string++)
    ;

  while (--string != start && *string != (char)ch)
    ;

  if (*string == (char)ch)
    return string;

  return NULL;
}

_CONST_RETURN_W wchar_t * __cdecl wcsrchr(const wchar_t *string, wchar_t ch)
{
  const wchar_t *start = string;

  while (*string++)
    ;

  while (--string != start && *string != ch)
    ;

  if (*string == ch)
    return string;

  return NULL;
}
