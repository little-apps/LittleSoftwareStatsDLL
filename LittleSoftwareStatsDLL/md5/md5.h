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

class CMD5
{
public:
	CMD5() {
		if(CryptAcquireContext(&m_hCryptProv, NULL, MS_ENHANCED_PROV, PROV_RSA_FULL, CRYPT_NEWKEYSET) == 0){
			if(GetLastError() == NTE_EXISTS){
				CryptAcquireContext(&m_hCryptProv, NULL, MS_ENHANCED_PROV, PROV_RSA_FULL, 0);
			}
		}
	}

	~CMD5() {
		if(m_hCryptProv) 
			CryptReleaseContext(m_hCryptProv, 0);
		m_hCryptProv = NULL;
		free(m_szHash);
	}

	bool Calculate(LPCTSTR szText) {
		DWORD dwLen = sizeof(TCHAR) * _tcslen(szText);
		DWORD dwHashLen;
		DWORD dwHashLenSize = sizeof(DWORD);

		if (CryptCreateHash(m_hCryptProv, CALG_MD5, 0, 0, &m_hHash)) {
			if (CryptHashData(m_hHash, (const BYTE*)szText, dwLen, 0)) {
				if (CryptGetHashParam(m_hHash, HP_HASHSIZE, (BYTE *)&dwHashLen, &dwHashLenSize, 0)) {
					if(m_szHash = (BYTE*)malloc(dwHashLen)) {
						if (CryptGetHashParam(m_hHash, HP_HASHVAL, (BYTE*)m_szHash, &dwHashLen, 0)) {
							CryptDestroyHash(m_hHash);
						}
					}
				}
			}
		}

		return false;
	}

	bool Calculate(const LPBYTE szText, DWORD dwLen) {
		DWORD dwHashLen;
		DWORD dwHashLenSize = sizeof(DWORD);

		if (CryptCreateHash(m_hCryptProv, CALG_MD5, 0, 0, &m_hHash)) {
			if (CryptHashData(m_hHash, (const BYTE*)szText, dwLen, 0)) {
				if (CryptGetHashParam(m_hHash, HP_HASHSIZE, (BYTE *)&dwHashLen, &dwHashLenSize, 0)) {
					if(m_szHash = (BYTE*)malloc(dwHashLen)) {
						if (CryptGetHashParam(m_hHash, HP_HASHVAL, (BYTE*)m_szHash, &dwHashLen, 0)) {
							CryptDestroyHash(m_hHash);
						}
					}
				}
			}
		}

		return false;
	}

	LPBYTE Hash() const {
		LPBYTE szHash = new BYTE[16];

		ZeroMemory(szHash, 16);

		memcpy(szHash, m_szHash, 16);

		return szHash;
	}

	void HexHash(CString &strHash) {
		//LPTSTR szBuf = strHash.GetBuffer(33);

		//ZeroMemory(szBuf, 33);

		for (int i=0; i<16; i++)
			strHash += StringFormat(_T("%02X"), m_szHash[i]);
		//_stprintf_s(szBuf+i*2, 33, _T("%02X"), m_szHash[i]);

		//szBuf[32]=0;

		return;
	}
private:
	BYTE *m_szHash;
	DWORD m_hHash;
	HCRYPTPROV m_hCryptProv;
};
