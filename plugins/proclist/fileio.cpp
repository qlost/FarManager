#include "Proclist.hpp"

int __do_unsigned_char_lconv_initialization = 255;

int atexit(void(*func)(void))
{
	return 0;
}

_ACRTIMP __declspec(noreturn) void __cdecl _invalid_parameter_noinfo_noreturn(void) {}

size_t WriteToFile(HANDLE File, const std::wstring_view& Str)
{
	DWORD Written;
	if (!WriteFile(File, Str.data(), static_cast<DWORD>(Str.size() * sizeof(wchar_t)), &Written, {}))
		return 0;

	return Written / sizeof(wchar_t);
}

size_t WriteToFile(HANDLE File, wchar_t Char)
{
	// BUGBUG this is mental
	return WriteToFile(File, std::wstring_view{ &Char, 1 });
}
