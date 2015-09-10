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

class WindowsHardware {
public:
	WindowsHardware(void) {
		m_nMemTotal = 0L;
		m_nMemFree = 0L;
		m_nDiskTotal = 0L;
		m_nDiskFree = 0L;
		m_nCPUCores = 0;
		m_nCPUArch = 0;
		m_nCPUFreq = 0;

		GetScreenRes();
		GetMemory();
		GetDiskSpace();
		GetCPUInfo();
	}

	virtual ~WindowsHardware(void) { }

	CString	m_strScreenRes;
	ULONG	m_nMemTotal;
	ULONG	m_nMemFree;
	ULONG	m_nDiskTotal;
	ULONG	m_nDiskFree;
	CString m_strCPUName;
	CString m_strCPUManufacturer;
	int		m_nCPUCores;
	int		m_nCPUArch;
	int		m_nCPUFreq;

private:
	void GetScreenRes() {
		RECT rc;
		GetWindowRect(GetDesktopWindow(), &rc);

		m_strScreenRes = StringFormat(_T("%dx%d"), rc.right, rc.bottom);
	}

	void GetMemory() {
		MEMORYSTATUSEX statex;
		statex.dwLength = sizeof(statex);

		if (GlobalMemoryStatusEx(&statex)) {
			m_nMemTotal = static_cast<ULONG>(statex.ullTotalPhys / 1024 / 1024);
			m_nMemFree = static_cast<ULONG>(statex.ullAvailPhys / 1024 / 1024);
		}

		return;
	}

	void GetDiskSpace() {
		ULONGLONG nFreeBytes, nTotalBytes;

		m_nDiskTotal = 0;
		m_nDiskFree = 0;

		DWORD dwSize = GetLogicalDriveStrings(0, nullptr);
		LPTSTR pszDrives = new TCHAR[dwSize + 2];
		GetLogicalDriveStrings(dwSize + 2,pszDrives);

		LPCTSTR pszDrv = pszDrives;
		while (*pszDrv) {
			if (GetDriveType(pszDrv) == DRIVE_FIXED) {
				nTotalBytes = 0;
				nFreeBytes = 0;

				GetDiskFreeSpaceEx(pszDrv, nullptr, reinterpret_cast<PULARGE_INTEGER>(&nTotalBytes), reinterpret_cast<PULARGE_INTEGER>(&nFreeBytes));

				m_nDiskTotal += static_cast<ULONG>(nTotalBytes / 1024 / 1024);
				m_nDiskFree += static_cast<ULONG>(nFreeBytes / 1024 / 1024);
			}

			pszDrv += _tcslen (pszDrv) + 1;
		}

		delete[] pszDrives;
	}

	void GetCPUInfo() {
		HKEY hKey;
		DWORD dwSize;
		int nPos = -1;

		if (RegOpenKeyEx(HKEY_LOCAL_MACHINE, _T("HARDWARE\\DESCRIPTION\\System\\CentralProcessor\\0"), 0, KEY_READ, &hKey) == ERROR_SUCCESS) {
			// Get CPU name
			LPTSTR szCPUName = new TCHAR[100];
			ZeroMemory(szCPUName, 100);

			if (RegQueryValueEx(hKey, _T("ProcessorNameString"), nullptr, nullptr, reinterpret_cast<LPBYTE>(szCPUName), &dwSize) == ERROR_SUCCESS) {
				m_strCPUName = szCPUName;
				m_strCPUName.Replace(_T("(TM)"), _T(""));
				m_strCPUName.Replace(_T("(R)"), _T(""));
				m_strCPUName.Remove(_T(' '));
			} else {
				m_strCPUName = _T("Unknown");
			}

			// Get CPU manufacturer
			LPTSTR szCPUManufacturer = new TCHAR[100];
			ZeroMemory(szCPUManufacturer, 100);

			if (RegQueryValueEx(hKey, _T("VendorIdentifier"), nullptr, nullptr, reinterpret_cast<LPBYTE>(szCPUManufacturer), &dwSize) == ERROR_SUCCESS) {
				m_strCPUManufacturer = szCPUManufacturer;
			} else {
				m_strCPUManufacturer = _T("Unknown");
			}

			// Get CPU Frequency
			DWORD dwCPUFreq = 0;
			if (RegQueryValueEx(hKey, _T("~MHz"), nullptr, nullptr, reinterpret_cast<LPBYTE>(&dwCPUFreq), &dwSize) == ERROR_SUCCESS)
				m_nCPUFreq = dwCPUFreq;

			// Free buffer
			delete[] szCPUName;
			delete[] szCPUManufacturer;
		} else {
			m_strCPUManufacturer = _T("Unknown");
			m_strCPUName = _T("Unknown");
		}

		SYSTEM_INFO siSysInfo;
		GetNativeSystemInfo(&siSysInfo);

		if (siSysInfo.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_AMD64)
			m_nCPUArch = 64;
		else
			m_nCPUArch = 32;

		m_nCPUCores = siSysInfo.dwNumberOfProcessors;

		RegCloseKey(hKey);

		return;
	}

	
};
