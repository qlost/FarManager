﻿#ifndef SQLITE_HPP_83511762_721C_4CB4_A7E3_C98B3605D2E2
#define SQLITE_HPP_83511762_721C_4CB4_A7E3_C98B3605D2E2
#pragma once

/*
sqlite.hpp

sqlite wrapper

*/
/*
Copyright © 2011 Far Group
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions
are met:
1. Redistributions of source code must retain the above copyright
   notice, this list of conditions and the following disclaimer.
2. Redistributions in binary form must reproduce the above copyright
   notice, this list of conditions and the following disclaimer in the
   documentation and/or other materials provided with the distribution.
3. The name of the authors may not be used to endorse or promote products
   derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

// Internal:

// Platform:
#include "platform.hpp"

// Common:
#include "common/compiler.hpp"

// External:

#include "sqlite.config.h"

//----------------------------------------------------------------------------

namespace sqlite
{
WARNING_PUSH(3)
WARNING_DISABLE_GCC("-Wold-style-cast")
WARNING_DISABLE_GCC("-Wzero-as-null-pointer-constant")

WARNING_DISABLE_CLANG("-Wold-style-cast")
WARNING_DISABLE_CLANG("-Wzero-as-null-pointer-constant")

#include "thirdparty/sqlite/sqlite3.h"

	static const auto static_destructor = SQLITE_STATIC;
	static const auto transient_destructor = SQLITE_TRANSIENT;

WARNING_POP()
}

#endif // SQLITE_HPP_83511762_721C_4CB4_A7E3_C98B3605D2E2
