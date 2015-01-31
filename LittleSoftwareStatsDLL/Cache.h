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

extern int nApiFormat;

class Cache {
public:
	Cache(CString strAppId) {
		LPTSTR szTempPath = new TCHAR[MAX_PATH];
		CString strFile;

		strApplicationId = strAppId;

		if (!GetTempPath(MAX_PATH, szTempPath)) {
			delete[] szTempPath;
			return;
		}

		if (nApiFormat == FORMAT_JSON)
			strFile = StringFormat(_T("%s\\%s.json.tmp"), szTempPath, strApplicationId);
		else
			strFile = StringFormat(_T("%s\\%s.xml.tmp"), szTempPath, strApplicationId);

		delete[] szTempPath;

		cFile.Open(strFile, CFile::modeCreate|CFile::modeNoTruncate|CFile::modeReadWrite);

		return;
	}

	~Cache() {
		if (cFile != INVALID_HANDLE_VALUE)
			cFile.Close();
	}

	BOOL DataExists() {
		if (cFile == INVALID_HANDLE_VALUE)
			return FALSE;

		return (cFile.GetLength() > 0);
	}

	CStringW GetData() {
		CStringW strData;

		if (cFile == INVALID_HANDLE_VALUE)
			return L"";

		// Go to beginning of file
		cFile.SeekToBegin();

		// Read file
		UINT nBytes = (UINT)cFile.GetLength() + 1;
		int nChars = nBytes / sizeof(WCHAR);
		nBytes = cFile.Read(strData.GetBuffer(nChars), nBytes);
		strData.ReleaseBuffer(nChars);

		// Decode from Base64
		CStringW strDecodedData = base64_decode(strData);

		return strDecodedData;
	}

	CStringW GetPostData(CStringW strData) {
		CStringW strOutput;

		if (!DataExists()) {
			if (nApiFormat == FORMAT_JSON) {
				strOutput = L"[";
				strOutput += strData;
				strOutput += L"]";
			} else {
				strOutput = L"<?xml version=\"1.0\"?><data>";
				strOutput += strData;
				strOutput += L"</data>";
			} 
		} else {
			CStringW strCachedData = GetData();

			if (nApiFormat == FORMAT_JSON) {
				strOutput = L"[";
				strOutput += strData;
			} else {
				strOutput = L"<?xml version=\"1.0\"?><data>";
				strOutput += strData;
			}

			int nPos = 0;
			CStringW strLine = strCachedData.Tokenize(L"\n", nPos).Trim();
			while (strLine != L"") {
				if (nApiFormat == FORMAT_JSON) {
					strOutput += L",";
					strOutput += strLine;
				} else {
					strOutput += strLine;
				}

				strLine = strCachedData.Tokenize(L"\n", nPos).Trim();
			}

			if (nApiFormat == FORMAT_JSON) {
				strOutput += L"]";
			} else {
				strOutput += L"</data>";
			}
		}

		strOutput.Replace(L"&", L"%26");
		return strOutput;
	}

	BOOL SaveData(CStringW strData) {
		if (cFile == INVALID_HANDLE_VALUE)
			return FALSE;

		CStringW strOutput = strData + L"\n";

		if (DataExists()) {
			CStringW strOldData = GetData();
			strOutput += strOldData;
		}

		CStringW strBase64Output = base64_encode(strOutput);

		// Clear File
		Clear();

		// Write file (double string length since were using unsigned char)
		cFile.Write(strBase64Output, strBase64Output.GetLength()*2);

		// Set hidden permission
		LPCWSTR szFileName = cFile.GetFilePath();
		::SetFileAttributesW(szFileName, FILE_ATTRIBUTE_HIDDEN);

		return TRUE;
	}

	void Clear() {
		if (DataExists()) {
			cFile.SetLength(0);
			cFile.Flush();
		}
	}
private:
	CFile cFile;
	BOOL bFileExists;
	CString strApplicationId;
};