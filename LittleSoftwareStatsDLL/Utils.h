/*
 * Little Software Stats - C++ DLL Library
 * Copyright (C) 2008 Little Apps (http://www.little-apps.org)
 * 
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#pragma once

void GetMachineHash(CString &strHashHex);
CString StringFormat(const TCHAR *fmt, ...);
CStringW Enquoute(const CStringW &s);
CStringA GenerateId(void);
LPSTR ConvertUTF16ToUTF8(LPCWSTR pszTextUTF16);
CStringW base64_encode(CStringW const string_to_encode);
CStringW base64_encode(WCHAR const* bytes_to_encode, unsigned int len);
CStringW base64_decode(CStringW& s);
DWORD WINAPI SendPost(LPVOID lpParam);