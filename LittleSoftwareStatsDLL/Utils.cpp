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

#include "includes.h"

extern CString g_strApiUrl;
extern int g_nApiFormat;

void GetMachineHash(CString &strHashHex) {
	CMD5 cMD5;
	BYTE *szHash = new BYTE[48];
	LPBYTE szMachineNameHash;
	LPBYTE szNetworkAddressHash;
	LPBYTE szVolumeIdHash;

	TCHAR szMachineId[100];
	DWORD nMachineIdLen = 100;

	TCHAR szNetworkAddress[13];
	IP_ADAPTER_INFO *pAdapterInfo;
	IP_ADAPTER_INFO *pAdapter;
	ULONG ulOutBufLen = sizeof(IP_ADAPTER_INFO);

	TCHAR szVolumeId[20];
	TCHAR szVolumeName[MAX_PATH];
	TCHAR szFileSystemName[MAX_PATH];
	DWORD dwSerialNumber = 0;
	DWORD dwMaxComponentLen = 0;
	DWORD dwFileSystemFlags = 0;

	ZeroMemory(szHash, 48);
	ZeroMemory(szMachineId, 100);
	ZeroMemory(szVolumeId, 20);
	ZeroMemory(szVolumeName, MAX_PATH);
	ZeroMemory(szFileSystemName, MAX_PATH);
	ZeroMemory(szNetworkAddress, 13);

	GetComputerName(szMachineId, &nMachineIdLen);

	cMD5.Calculate(szMachineId);
	szMachineNameHash = cMD5.Hash();

	pAdapterInfo = new IP_ADAPTER_INFO;
	if (pAdapterInfo == nullptr) {
		TRACE(_T("Error allocating memory needed to call GetAdaptersinfo()"));
	}

	// Make an initial call to GetAdaptersInfo to get the necessary size into the ulOutBufLen variable
	if (GetAdaptersInfo(pAdapterInfo, &ulOutBufLen) == ERROR_BUFFER_OVERFLOW) {
		delete pAdapterInfo;
		pAdapterInfo = reinterpret_cast<IP_ADAPTER_INFO *>(new BYTE[ulOutBufLen]);

		if (pAdapterInfo == nullptr) {
			TRACE(_T("Error allocating memory needed to call GetAdaptersinfo()"));
		}
	}

	if (GetAdaptersInfo(pAdapterInfo, &ulOutBufLen) == NO_ERROR) {
		pAdapter = pAdapterInfo;

		while (pAdapter) {
			if (pAdapter->Type != MIB_IF_TYPE_LOOPBACK) {
				_stprintf_s(szNetworkAddress, 13, _T("%.2X%.2X%.2X%.2X%.2X%.2X"), 
					pAdapter->Address[0], 
					pAdapter->Address[1], 
					pAdapter->Address[2], 
					pAdapter->Address[3], 
					pAdapter->Address[4], 
					pAdapter->Address[5]
				);

				break;
			}

			pAdapter = pAdapter->Next;
		}
	} else {
		TRACE(_T("GetAdaptersInfo() call failed"));
	}

	cMD5.Calculate(szNetworkAddress);
	szNetworkAddressHash = cMD5.Hash();

	if (GetVolumeInformation(
		nullptr,
		szVolumeName,
		sizeof(szVolumeName),
		&dwSerialNumber,
		&dwMaxComponentLen,
		&dwFileSystemFlags,
		szFileSystemName,
		sizeof(szFileSystemName))) {
			_stprintf_s(szVolumeId, 20, _T("%lu"), dwSerialNumber); 
	}

	cMD5.Calculate(szVolumeId);
	szVolumeIdHash = cMD5.Hash();

	// Calculate hash from hashes
	memcpy(szHash, szMachineNameHash, 16);
	memcpy(szHash+16, szNetworkAddressHash, 16);
	memcpy(szHash+32, szVolumeIdHash, 16);

	cMD5.Calculate(szHash, 48);

	//strHashHex.Preallocate(33);
	cMD5.HexHash(strHashHex);

	delete[] szHash;
	delete   pAdapterInfo;

	delete[] szMachineNameHash;
	delete[] szVolumeIdHash;
	delete[] szNetworkAddressHash;

	return;
}

CStringA GenerateId() {
	CStringA strResult;
	char *szId = strResult.GetBuffer(33);

	static const char alphanum[] = "0123456789ABCDEF";

	ZeroMemory(szId, 33);

	srand(static_cast<unsigned int>(time(nullptr)));

    for (int i = 0; i < 32; ++i) {
		szId[i] = alphanum[rand() % (sizeof(alphanum) - 1)];
    }

	strResult.ReleaseBuffer();

	return strResult;
}

CString StringFormat(const TCHAR *fmt, ...) {
	CString strResult;
    LPTSTR szBuffer;
    int sz;
    va_list args;

    va_start( args, fmt );

    sz = _vsctprintf( fmt, args ) + 1;

	szBuffer = strResult.GetBuffer(sz);

	sz = _vsntprintf_s(szBuffer, sz, _TRUNCATE, fmt, args);

    strResult.ReleaseBuffer( sz );

    va_end( args );

    return strResult;
}

CStringW Enquoute(const CStringW &s) {
	if (s.IsEmpty())
		return CStringW(_T("\"\""));

	int i;
	int nLen = s.GetLength();
	CStringW str;

	str =  _T('"');

	for (i = 0; i < nLen; i++) {
		wchar_t c = s[i];

		if ((c == '\\') || (c == '"') || (c == '>')) 
		{
			str += _T("\\");
			str += c;
		} 
		else if (c == '\b')
			str += _T("\\b");
		else if (c == '\t')
			str += _T("\\t");
		else if (c == '\n')
			str += _T("\\n");
		else if (c == '\f')
			str += _T("\\f");
		else if (c == '\r')
			str += _T("\\r");
		else 
		{
			if (c < _T(' '))
				str += StringFormat(_T("\\u%0.4X"), c);
			else
				str += c;
		}
	}

	str += _T('"');

	return str;
}

LPSTR ConvertUTF16ToUTF8(LPCWSTR pszTextUTF16) {
	// Special case of NULL or empty input string

	if ( (pszTextUTF16 == nullptr) || (*pszTextUTF16 == _T('\0')) )
		// Return empty string
		return "";

	// Consider WCHAR's count corresponding to total input string length,
	// including end-of-string (L'\0') character.

	size_t cchUTF16;
	cchUTF16 = wcslen(pszTextUTF16);

	if (cchUTF16 <= 0)
		return "";

	// Consider also terminating \0
	++cchUTF16;

	// WC_ERR_INVALID_CHARS flag is set to fail if invalid input character is encountered.
	// This flag is supported on Windows Vista and later.
	// Don't use it on Windows XP and previous.
	OSVERSIONINFOEX osvi;
	DWORD dwConversionFlags;

	ZeroMemory(&osvi, sizeof(osvi));

	osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);
	if (!GetVersionEx(reinterpret_cast<OSVERSIONINFO*>(&osvi)))
		// Failed to get OS version
		return "";

	if (osvi.dwMajorVersion >= 6 && osvi.dwMinorVersion >= 0)
		dwConversionFlags = WC_ERR_INVALID_CHARS;
	else
		dwConversionFlags = 0;

	// Get size of destination UTF-8 buffer, in CHAR's (= bytes)

	int cbUTF8 = ::WideCharToMultiByte(
		CP_UTF8,                // convert to UTF-8
		dwConversionFlags,      // specify conversion behavior
		pszTextUTF16,           // source UTF-16 string
		static_cast<int>( cchUTF16 ),   // total source string length, in WCHAR's, including end-of-string \0
		nullptr,                   // unused - no conversion required in this step
		0,                      // request buffer size
		nullptr, nullptr              // unused
		);

	if (cbUTF8 == 0)
		return "";

	// Allocate destination buffer for UTF-8 string
	int cchUTF8 = cbUTF8; // sizeof(CHAR) = 1 byte
	LPSTR szUTF8 = new CHAR[cchUTF8];

	ZeroMemory(szUTF8, cchUTF8);

	// Do the conversion from UTF-16 to UTF-8
	int result = ::WideCharToMultiByte(
		CP_UTF8,                // convert to UTF-8
		dwConversionFlags,      // specify conversion behavior
		pszTextUTF16,           // source UTF-16 string
		static_cast<int>( cchUTF16 ),   // total source string length, in WCHAR's, including end-of-string \0
		szUTF8,                 // destination buffer
		cbUTF8,                 // destination buffer size, in bytes
		nullptr, nullptr              // unused
		); 

	if (result == 0) 
		return "";

	// Return resulting UTF-8 string
	return szUTF8;
}

DWORD WINAPI SendPost(LPVOID lpParam) {
	HINTERNET hInt = nullptr;
	HINTERNET hConn = nullptr;
	HINTERNET hReq = nullptr;
	int flags;
	DWORD dwSize, dwFlags;
	LPSTR szHdr;

	CStringW *strData = reinterpret_cast<CStringW*>(lpParam);

	// Must be set to application/x-www-form-urlencoded otherwise server won't read POST data
	szHdr = ConvertUTF16ToUTF8(L"Content-Type: application/x-www-form-urlencoded");

	strData->Insert(0, L"data=");

	OutputDebugStringW(strData->GetString());

	try {
		LPSTR szData = ConvertUTF16ToUTF8(strData->GetString());

		if (strcmp(szData, "") == 0) // Error converting to UTF8
			return FALSE;

		hInt = InternetOpen(API_USERAGENT, INTERNET_OPEN_TYPE_PRECONFIG, nullptr, nullptr, 0);

		if (hInt == nullptr)
			return FALSE;

		// Set HTTP request timeout
		if (API_TIMEOUT > 0) {
			int nPostTimeout = API_TIMEOUT;
			InternetSetOption(hInt, INTERNET_OPTION_CONNECT_TIMEOUT, &nPostTimeout, sizeof(nPostTimeout));
			InternetSetOption(hInt, INTERNET_OPTION_SEND_TIMEOUT, &nPostTimeout, sizeof(nPostTimeout));
			InternetSetOption(hInt, INTERNET_OPTION_RECEIVE_TIMEOUT, &nPostTimeout, sizeof(nPostTimeout));
		}

		// Crack URI
		URL_COMPONENTS urlComp;

		ZeroMemory(&urlComp, sizeof(urlComp));
		urlComp.dwStructSize = sizeof(urlComp);

		// Set required component lengths to non-zero so that they are cracked.
		urlComp.dwHostNameLength = static_cast<DWORD>(-1);
		urlComp.dwSchemeLength = static_cast<DWORD>(-1);
		urlComp.dwUrlPathLength = static_cast<DWORD>(-1);
		urlComp.dwExtraInfoLength = static_cast<DWORD>(-1);

		if (!InternetCrackUrl(g_strApiUrl.GetString(), static_cast<DWORD>(g_strApiUrl.GetLength()), 0, &urlComp)) {
			InternetCloseHandle(hInt);
			return FALSE;
		}

		CString strHostname(urlComp.lpszHostName, urlComp.dwHostNameLength);

		hConn = InternetConnect(hInt, strHostname, urlComp.nPort, nullptr, nullptr, INTERNET_SERVICE_HTTP, 0, 1);

		if (hConn == nullptr) {
			InternetCloseHandle(hInt);
			return FALSE;
		}

		if (urlComp.nScheme == INTERNET_SCHEME_HTTPS)
			flags = INTERNET_FLAG_NO_UI | INTERNET_FLAG_SECURE | INTERNET_FLAG_IGNORE_CERT_CN_INVALID | INTERNET_FLAG_IGNORE_CERT_DATE_INVALID;
		else
			flags = INTERNET_FLAG_NO_UI;

		hReq = HttpOpenRequest(hConn, _T("POST"), urlComp.lpszUrlPath, nullptr, nullptr, nullptr, flags, 1);

		if (hReq == nullptr) {
			InternetCloseHandle(hInt);
			InternetCloseHandle(hConn);

			return FALSE;
		}

		if (urlComp.nScheme == INTERNET_SCHEME_HTTPS) {
			dwSize = sizeof(dwFlags);
			if (InternetQueryOption(hReq, INTERNET_OPTION_SECURITY_FLAGS, &dwFlags, &dwSize)) {
				dwFlags = dwFlags | SECURITY_FLAG_IGNORE_UNKNOWN_CA;
				if (!InternetSetOption(hReq, INTERNET_OPTION_SECURITY_FLAGS, &dwFlags, sizeof(dwSize))) {
					InternetCloseHandle(hInt);
					InternetCloseHandle(hConn);
					InternetCloseHandle(hReq);

					return FALSE;
				}
			} else {
				InternetCloseHandle(hInt);
				InternetCloseHandle(hConn);
				InternetCloseHandle(hReq);

				return FALSE; //InternetQueryOption failed
			}
		}

		if (HttpSendRequestA(hReq, szHdr, strlen(szHdr), szData, strlen(szData))) {
			if (API_READRESPONSE == TRUE) {
				// Read server Response
				DWORD dwTotalRead = 0, dwBytesRead = READBUFFERSIZE;
				CHAR szBuff[READBUFFERSIZE];

				ZeroMemory(szBuff, READBUFFERSIZE);

				while (dwBytesRead > 0) {
					if (!InternetReadFile(hReq, szBuff, READBUFFERSIZE, &dwBytesRead)) {
						InternetCloseHandle(hInt);
						InternetCloseHandle(hConn);
						InternetCloseHandle(hReq);

						return FALSE;
					}

					if (dwBytesRead > 0 && dwBytesRead <= READBUFFERSIZE) {
						dwTotalRead += dwBytesRead;

						/* Do what you want with the response here */

						InternetCloseHandle(hInt);
						InternetCloseHandle(hConn);
						InternetCloseHandle(hReq);

						return TRUE;
					}
				}
			}
		} else {
			int err = GetLastError();

			InternetCloseHandle(hInt);
			InternetCloseHandle(hConn);
			InternetCloseHandle(hReq);

			return FALSE;
		}
	} catch(...) {
		if (hInt != INVALID_HANDLE_VALUE)
			InternetCloseHandle(hInt);
		if (hConn != INVALID_HANDLE_VALUE)
			InternetCloseHandle(hConn);
		if (hReq != INVALID_HANDLE_VALUE)
			InternetCloseHandle(hReq);

		return FALSE;
	};

	InternetCloseHandle(hInt);
	InternetCloseHandle(hConn);
	InternetCloseHandle(hReq);

	return TRUE;
}

// Base64 Encoding/decoding
static const CStringW base64_chars = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

static inline bool is_base64(WCHAR c) {
	return (iswalnum(c) || (c == L'+') || (c == L'/'));
}

CStringW base64_encode(CStringW const string_to_encode) {
	return base64_encode(reinterpret_cast<const WCHAR*>(string_to_encode.GetString()), string_to_encode.GetLength());
}

CStringW base64_encode(WCHAR const* bytes_to_encode, unsigned int in_len) {
	CStringW ret;
	int i = 0;
	int j = 0;
	WCHAR char_array_3[3];
	WCHAR char_array_4[4];

	while (in_len--) {
		char_array_3[i++] = *(bytes_to_encode++);
		if (i == 3) {
			char_array_4[0] = (char_array_3[0] & 0xfc) >> 2;
			char_array_4[1] = ((char_array_3[0] & 0x03) << 4) + ((char_array_3[1] & 0xf0) >> 4);
			char_array_4[2] = ((char_array_3[1] & 0x0f) << 2) + ((char_array_3[2] & 0xc0) >> 6);
			char_array_4[3] = char_array_3[2] & 0x3f;

			for(i = 0; (i <4) ; i++)
				ret += base64_chars[char_array_4[i]];
			i = 0;
		}
	}

	if (i) {
		for(j = i; j < 3; j++)
			char_array_3[j] = '\0';

		char_array_4[0] = (char_array_3[0] & 0xfc) >> 2;
		char_array_4[1] = ((char_array_3[0] & 0x03) << 4) + ((char_array_3[1] & 0xf0) >> 4);
		char_array_4[2] = ((char_array_3[1] & 0x0f) << 2) + ((char_array_3[2] & 0xc0) >> 6);
		char_array_4[3] = char_array_3[2] & 0x3f;

		for (j = 0; (j < i + 1); j++)
			ret += base64_chars[char_array_4[j]];

		while((i++ < 3))
			ret += '=';

	}

	return ret;

}

CStringW base64_decode(CStringW& encoded_string) {
	int in_len = encoded_string.GetLength();
	int i = 0;
	int j = 0;
	int in_ = 0;
	WCHAR char_array_4[4], char_array_3[3];
	CStringW ret;

	while (in_len-- && ( encoded_string[in_] != '=') && is_base64(encoded_string[in_])) {
		char_array_4[i++] = encoded_string[in_];
		in_++;
		if (i == 4) {
			for (i = 0; i <4; i++)
				char_array_4[i] = base64_chars.Find(char_array_4[i]);

			char_array_3[0] = (char_array_4[0] << 2) + ((char_array_4[1] & 0x30) >> 4);
			char_array_3[1] = ((char_array_4[1] & 0xf) << 4) + ((char_array_4[2] & 0x3c) >> 2);
			char_array_3[2] = ((char_array_4[2] & 0x3) << 6) + char_array_4[3];

			for (i = 0; (i < 3); i++)
				ret += char_array_3[i];
			i = 0;
		}
	}

	if (i) {
		for (j = i; j <4; j++)
			char_array_4[j] = 0;

		for (j = 0; j <4; j++)
			char_array_4[j] = base64_chars.Find(char_array_4[j]);

		char_array_3[0] = (char_array_4[0] << 2) + ((char_array_4[1] & 0x30) >> 4);
		char_array_3[1] = ((char_array_4[1] & 0xf) << 4) + ((char_array_4[2] & 0x3c) >> 2);
		char_array_3[2] = ((char_array_4[2] & 0x3) << 6) + char_array_4[3];

		for (j = 0; (j < i - 1); j++) ret += char_array_3[j];
	}

	return ret;
}