﻿/*
platform.security.cpp

*/
/*
Copyright © 2010 Far Group
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

// BUGBUG
#include "platform.headers.hpp"

// Self:
#include "platform.security.hpp"

// Internal:
#include "log.hpp"

// Platform:
#include "platform.hpp"
#include "platform.concurrency.hpp"

// Common:
#include "common/string_utils.hpp"

// External:

//----------------------------------------------------------------------------

namespace
{
	const auto& lookup_privilege_value(const wchar_t* Name)
	{
		static unordered_string_map<std::optional<LUID>> s_Cache;
		static os::critical_section s_CS;

		SCOPED_ACTION(std::scoped_lock)(s_CS);

		const auto [Iterator, IsEmplaced] = s_Cache.try_emplace(Name);

		auto& [MapKey, MapValue] = *Iterator;

		if (IsEmplaced)
		{
			LUID Luid;
			if (LookupPrivilegeValue(nullptr, MapKey.c_str(), &Luid))
				MapValue = Luid;
			else
				LOGWARNING(L"LookupPrivilegeValue({}): {}"sv, MapKey, os::last_error());
		}

		return MapValue;
	}
}

static bool operator==(const LUID& a, const LUID& b)
{
	return a.LowPart == b.LowPart && a.HighPart == b.HighPart;
}

namespace os::security
{
	void detail::sid_deleter::operator()(PSID Sid) const noexcept
	{
		FreeSid(Sid);
	}

	sid_ptr make_sid(PSID_IDENTIFIER_AUTHORITY IdentifierAuthority, BYTE SubAuthorityCount, DWORD SubAuthority0, DWORD SubAuthority1, DWORD SubAuthority2, DWORD SubAuthority3, DWORD SubAuthority4, DWORD SubAuthority5, DWORD SubAuthority6, DWORD SubAuthority7)
	{
		PSID Sid;
		return sid_ptr(AllocateAndInitializeSid(IdentifierAuthority, SubAuthorityCount, SubAuthority0, SubAuthority1, SubAuthority2, SubAuthority3, SubAuthority4, SubAuthority5, SubAuthority6, SubAuthority7, &Sid)? Sid : nullptr);
	}

	bool is_admin()
	{
		static const auto Result = []
		{
			// Vista+
			TOKEN_ELEVATION Elevation;
			DWORD ReturnLength;
			if (GetTokenInformation(GetCurrentProcessToken(), TokenElevation, &Elevation, sizeof(Elevation), &ReturnLength))
				return Elevation.TokenIsElevated != 0;

			// Old method
			SID_IDENTIFIER_AUTHORITY NtAuthority = SECURITY_NT_AUTHORITY;
			const auto AdministratorsGroup = make_sid(&NtAuthority, 2, SECURITY_BUILTIN_DOMAIN_RID, DOMAIN_ALIAS_RID_ADMINS);
			if (!AdministratorsGroup)
				return false;

			BOOL IsMember;
			return CheckTokenMembership(nullptr, AdministratorsGroup.get(), &IsMember) && IsMember;
		}();

		return Result;
	}

	TOKEN_ELEVATION_TYPE elevation_type()
	{
		TOKEN_ELEVATION_TYPE ElevationType;
		DWORD ReturnLength;
		return GetTokenInformation(GetCurrentProcessToken(), TokenElevationType, &ElevationType, sizeof(ElevationType), &ReturnLength)?
			ElevationType :
			TokenElevationTypeDefault;
	}

	handle open_current_process_token(DWORD const DesiredAccess)
	{
		HANDLE Handle;
		if (!OpenProcessToken(GetCurrentProcess(), DesiredAccess, &Handle))
		{
			LOGWARNING(L"open_current_process_token({}): {}"sv, DesiredAccess, last_error());
			return {};
		}

		return handle(Handle);
	}

	privilege::privilege(std::span<const wchar_t* const> const Names)
	{
		if (Names.empty())
			return;

		const block_ptr<TOKEN_PRIVILEGES> NewState(sizeof(TOKEN_PRIVILEGES) + sizeof(LUID_AND_ATTRIBUTES) * (Names.size() - 1));
		NewState->PrivilegeCount = 0;

		std::vector<size_t> NameIndices;
		NameIndices.reserve(Names.size());

		for (const auto& i: Names)
		{
			const auto& Luid = lookup_privilege_value(i);
			if (!Luid)
				continue;

			NewState->Privileges[NewState->PrivilegeCount++] = { *Luid, SE_PRIVILEGE_ENABLED };
			NameIndices.emplace_back(&i - Names.data());
		}

		const auto Token = open_current_process_token(TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY);
		if (!Token)
			return;

		DWORD ReturnLength;
		m_SavedState.reset(NewState.size());
		m_Changed = AdjustTokenPrivileges(Token.native_handle(), FALSE, NewState.data(), static_cast<DWORD>(m_SavedState.size()), m_SavedState.data(), &ReturnLength) && m_SavedState->PrivilegeCount;
		const auto LastError = last_error();
		if (LastError.Win32Error == ERROR_SUCCESS)
			return;

		LOGWARNING(L"AdjustTokenPrivileges(): {}"sv, LastError);

		if (LastError.Win32Error != ERROR_NOT_ALL_ASSIGNED)
			return;

		std::span const
			Privileges(NewState->Privileges, NewState->PrivilegeCount),
			Changed(m_SavedState->Privileges, m_SavedState->PrivilegeCount);

		for (const auto& i: Privileges)
		{
			if (std::ranges::find(Changed, i.Luid, &LUID_AND_ATTRIBUTES::Luid) == Changed.end())
			{
				LOGWARNING(L"{} not enabled"sv, Names[NameIndices[&i - Privileges.data()]]);
			}
		}
	}

	privilege::~privilege()
	{
		if (!m_Changed)
			return;

		const auto Token = open_current_process_token(TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY);
		if (!Token)
			return;

		SCOPED_ACTION(os::last_error_guard);

		AdjustTokenPrivileges(Token.native_handle(), FALSE, m_SavedState.data(), 0, {}, {});
		if (const auto LastError = last_error(); LastError.Win32Error != ERROR_SUCCESS)
			LOGWARNING(L"AdjustTokenPrivileges(): {}"sv, LastError);
	}

	bool privilege::check(std::span<const wchar_t* const> const Names)
	{
		const auto Token = open_current_process_token(TOKEN_QUERY);
		if (!Token)
			return false;

		const block_ptr<PRIVILEGE_SET> PrivilegeSet(sizeof(PRIVILEGE_SET) + sizeof(LUID_AND_ATTRIBUTES) * (Names.size() - 1));
		PrivilegeSet->PrivilegeCount = 0;
		PrivilegeSet->Control = PRIVILEGE_SET_ALL_NECESSARY;

		for (const auto& i: Names)
		{
			const auto& Luid = lookup_privilege_value(i);
			if (!Luid)
				return false;

			PrivilegeSet->Privilege[PrivilegeSet->PrivilegeCount++] = { *Luid };
		}

		BOOL Result;
		if (!PrivilegeCheck(Token.native_handle(), PrivilegeSet.data(), &Result))
		{
			LOGWARNING(L"PrivilegeCheck(): {}"sv, os::last_error());
			return false;
		}

		return Result != FALSE;
	}
}
