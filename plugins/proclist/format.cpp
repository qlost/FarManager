#define FMT_THROW(x) {}
#define FMT_ASSERT(condition, message) {}

#include "format.hpp"

WARNING_PUSH(3)

WARNING_DISABLE_GCC("-Wmissing-declarations")

WARNING_DISABLE_CLANG("-Weverything")

#include <format.cc>

WARNING_POP()

extern "C" {
int __isa_available = 5; //__ISA_AVAILABLE_AVX2
int __do_unsigned_char_lconv_initialization = 255;
FILE* __cdecl __acrt_iob_func(unsigned const id) {return 0;}
}
namespace fmt {inline namespace v12 {void assert_fail(const char*, int, const char*) {}}}
namespace std {const char* __cdecl _Syserror_map(int) {return 0;}}
void(__cdecl* std::_Raise_handler)(class stdext::exception const&) {};
void __cdecl std::_Xout_of_range(char const*) {}
void __cdecl std::_Xlength_error(char const*) {}
void __cdecl _lock_file(FILE*) {}
void __cdecl _unlock_file(FILE*) {}
void __cdecl _invoke_watson(wchar_t const*, wchar_t const*, wchar_t const*, unsigned int, uintptr_t) {}
int atexit(void(*)(void)) {return 0;}
int __cdecl _dsign(double _X) {return _X < 0;}
int __cdecl fflush(FILE*) {return 0;}
int __cdecl fputc(int, FILE*) {return 0;}
size_t __cdecl fwrite(void const*, size_t, size_t, FILE*) {return 0;}
