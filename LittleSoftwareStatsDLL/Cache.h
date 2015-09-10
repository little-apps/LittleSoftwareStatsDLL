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

extern int g_nApiFormat;

class Cache {
public:
	Cache(CString strAppId) {
		LPTSTR szTempPath = new TCHAR[MAX_PATH];
		CString strFile;

		m_strApplicationId = strAppId;

		if (!GetTempPath(MAX_PATH, szTempPath)) {
			delete[] szTempPath;
			return;
		}

		if (g_nApiFormat == FORMAT_JSON)
			strFile = StringFormat(_T("%s\\%s.json.tmp"), szTempPath, m_strApplicationId);
		else
			strFile = StringFormat(_T("%s\\%s.xml.tmp"), szTempPath, m_strApplicationId);

		delete[] szTempPath;

		m_cFile.Open(strFile, CFile::modeCreate | CFile::modeNoTruncate | CFile::modeReadWrite);

		return;
	}

	~Cache() {
		if (m_cFile != INVALID_HANDLE_VALUE)
			m_cFile.Close();
	}

	BOOL DataExists() {
		if (m_cFile == INVALID_HANDLE_VALUE)
			return FALSE;

		return (m_cFile.GetLength() > 0);
	}

	CStringW GetData() {
		CStringW strData;

		if (m_cFile == INVALID_HANDLE_VALUE)
			return L"";

		// Go to beginning of file
		m_cFile.SeekToBegin();

		// Read file
		UINT nBytes = static_cast<UINT>(m_cFile.GetLength()) + 1;
		int nChars = nBytes / sizeof(WCHAR);
		nBytes = m_cFile.Read(strData.GetBuffer(nChars), nBytes);
		strData.ReleaseBuffer(nChars);

		// Decode from Base64
		CStringW strDecodedData = base64_decode(strData);

		return strDecodedData;
	}

	CStringW GetPostData(CStringW strData) {
		CStringW strOutput;

		if (!DataExists()) {
			if (g_nApiFormat == FORMAT_JSON) {
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

			if (g_nApiFormat == FORMAT_JSON) {
				strOutput = L"[";
				strOutput += strData;
			} else {
				strOutput = L"<?xml version=\"1.0\"?><data>";
				strOutput += strData;
			}

			int nPos = 0;
			CStringW strLine = strCachedData.Tokenize(L"\n", nPos).Trim();
			while (strLine != L"") {
				if (g_nApiFormat == FORMAT_JSON) {
					strOutput += L",";
					strOutput += strLine;
				} else {
					strOutput += strLine;
				}

				strLine = strCachedData.Tokenize(L"\n", nPos).Trim();
			}

			if (g_nApiFormat == FORMAT_JSON) {
				strOutput += L"]";
			} else {
				strOutput += L"</data>";
			}
		}

		strOutput.Replace(L"&", L"%26");
		return strOutput;
	}

	BOOL SaveData(CStringW strData) {
		if (m_cFile == INVALID_HANDLE_VALUE)
			return FALSE;

		CStringW strOutput = strData + L"\n";

		if (DataExists()) {
			CStringW strOldData = GetData();
			strOutput += strOldData;
		}

		CString strBase64Output = base64_encode(strOutput);

		// Clear File
		Clear();

		TRY {
			// Write file (double string length since were using unsigned char)
			m_cFile.Write(strBase64Output, strBase64Output.GetLength() * 2);

			// Set hidden permission
			::SetFileAttributes(m_cFile.GetFilePath(), FILE_ATTRIBUTE_HIDDEN);
		} CATCH (CFileException, e) {

		}
		END_CATCH

		return TRUE;
	}

	void Clear() {
		if (DataExists()) {
			TRY {
				m_cFile.SetLength(0);
				m_cFile.Flush();
			} CATCH(CFileException, e) {

			}
			END_CATCH
		}
	}
private:
	CFile m_cFile;
	BOOL m_bFileExists;
	CString m_strApplicationId;
};