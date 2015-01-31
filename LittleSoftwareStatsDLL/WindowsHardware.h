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
		GetScreenRes();
		GetMemory();
		GetDiskSpace();
		GetCPUInfo();
	}

	~WindowsHardware(void) { }

	CString strScreenRes;
	ULONG nMemTotal, nMemFree;
	ULONG nDiskTotal, nDiskFree;
	CString strCPUName, strCPUManufacturer;
	int nCPUCores, nCPUArch, nCPUFreq;

private:
	void GetScreenRes() {
		RECT rc;
		GetWindowRect(GetDesktopWindow(), &rc);

		this->strScreenRes = StringFormat(_T("%dx%d"), rc.right, rc.bottom);
	}

	void GetMemory() {
		MEMORYSTATUSEX statex;
		statex.dwLength = sizeof(statex);

		if (GlobalMemoryStatusEx(&statex)) {
			this->nMemTotal = static_cast<ULONG>(statex.ullTotalPhys / 1024 / 1024);
			this->nMemFree = static_cast<ULONG>(statex.ullAvailPhys / 1024 / 1024);
		}

		return;
	}

	void GetDiskSpace() {
		ULONGLONG nFreeBytes, nTotalBytes;

		this->nDiskTotal = 0;
		this->nDiskFree = 0;

		DWORD dwSize = GetLogicalDriveStrings(0, NULL);
		LPTSTR pszDrives = new TCHAR[dwSize + 2];
		GetLogicalDriveStrings(dwSize + 2,pszDrives);

		LPCTSTR pszDrv = pszDrives;
		while (*pszDrv) {
			if (GetDriveType(pszDrv) == DRIVE_FIXED) {
				nTotalBytes = 0;
				nFreeBytes = 0;

				GetDiskFreeSpaceEx(pszDrv, NULL, (PULARGE_INTEGER)&nTotalBytes,(PULARGE_INTEGER)&nFreeBytes);

				this->nDiskTotal += static_cast<ULONG>(nTotalBytes / 1024 / 1024);
				this->nDiskFree += static_cast<ULONG>(nFreeBytes / 1024 / 1024);
			}

			pszDrv += _tcslen (pszDrv) + 1;
		}

		delete[] pszDrives;

		return;
	}

	void GetCPUInfo() {
		HKEY hKey;
		DWORD dwSize;
		int nPos = -1;

		if (RegOpenKeyEx(HKEY_LOCAL_MACHINE, _T("HARDWARE\\DESCRIPTION\\System\\CentralProcessor\\0"), 0, KEY_READ, &hKey) == ERROR_SUCCESS) {
			// Get CPU name
			LPTSTR szCPUName = (LPTSTR)malloc(sizeof(TCHAR)*100);
			ZeroMemory(szCPUName, 100);

			if (RegQueryValueEx(hKey, _T("ProcessorNameString"), NULL, NULL, (LPBYTE)szCPUName, &dwSize) == ERROR_SUCCESS) {
				this->strCPUName = szCPUName;
				this->strCPUName.Replace(_T("(TM)"), _T(""));
				this->strCPUName.Replace(_T("(R)"), _T(""));
				this->strCPUName.Remove(_T(' '));
			} else {
				this->strCPUName = _T("Unknown");
			}

			// Get CPU manufacturer
			LPTSTR szCPUManufacturer = (LPTSTR)malloc(sizeof(TCHAR)*100);
			ZeroMemory(szCPUManufacturer, 100);

			if (RegQueryValueEx(hKey, _T("VendorIdentifier"), NULL, NULL, (LPBYTE)szCPUManufacturer, &dwSize) == ERROR_SUCCESS) {
				this->strCPUManufacturer = szCPUManufacturer;
			} else {
				this->strCPUManufacturer = _T("Unknown");
			}

			// Get CPU Frequency
			DWORD dwCPUFreq = 0;
			if (RegQueryValueEx(hKey, _T("~MHz"), NULL, NULL, (LPBYTE)&dwCPUFreq, &dwSize) == ERROR_SUCCESS)
				this->nCPUFreq = dwCPUFreq;

			// Free buffer
			free(szCPUName);
			free(szCPUManufacturer);
		} else {
			this->strCPUManufacturer = _T("Unknown");
			this->strCPUName = _T("Unknown");
		}

		SYSTEM_INFO siSysInfo;
		GetNativeSystemInfo(&siSysInfo);

		if (siSysInfo.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_AMD64)
			this->nCPUArch = 64;
		else
			this->nCPUArch = 32;

		this->nCPUCores = siSysInfo.dwNumberOfProcessors;

		RegCloseKey(hKey);

		return;
	}

	
};
