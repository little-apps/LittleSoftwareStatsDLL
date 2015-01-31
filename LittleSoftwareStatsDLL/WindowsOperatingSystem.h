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

#include "WindowsHardware.h"

class WindowsOperatingSystem {
public:
	WindowsOperatingSystem() {
		// Get LCID
		this->nLcid = GetThreadLocale();

		// Get operating system info
		GetOSInfo();

		// Get .NET Framework version
		GetFrameworkVer();

		// Get Java version
		GetJavaVer();
	}
	~WindowsOperatingSystem() { }

	CString strVersion;
	int nArchitecture;
	int nServicePack, nFrameworkSP;
	CString strFrameworkVer;
	CString strJavaVer;
	int nLcid;
	WindowsHardware cWindowsHardware;
private:
	void GetOSInfo() {
		OSVERSIONINFOEX osvi;
		SYSTEM_INFO si;
		HKEY hKey;
		DWORD dwSize;
		CString strOSName;

		ZeroMemory(&osvi, sizeof(OSVERSIONINFOEX));
		osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);

		if (GetVersionEx((OSVERSIONINFO*)&osvi) == FALSE) {
			this->strVersion = _T("Unknown");
			this->nServicePack = 0;
			return;
		}

		ZeroMemory(&si, sizeof(SYSTEM_INFO));
		GetSystemInfo(&si);

		switch (osvi.dwPlatformId) {
			case VER_PLATFORM_WIN32_WINDOWS: 
				{
					switch (osvi.dwMajorVersion) {
						case 4:
							{
								switch (osvi.dwMinorVersion) {
									case 0:	
										{
											if (_tcscmp(osvi.szCSDVersion, _T("B")) == 0 || _tcscmp(osvi.szCSDVersion, _T("C")) == 0)
												strOSName = _T("Windows 95 R2");
											else
												strOSName = _T("Windows 95");
											break;
										}
									case 10:
										{
											if (_tcscmp(osvi.szCSDVersion, _T("A")) == 0)
												strOSName = _T("Windows 98 SE");
											else
												strOSName = _T("Windows 98");
											break;
										}
									case 90:
										{
											strOSName = _T("Windows ME");
											break;
										}
								}
								
								break;
							}
					}
					break;
				}
				case VER_PLATFORM_WIN32_NT: 
					{
						switch (osvi.dwMajorVersion) {
							case 3:
								{
									strOSName = _T("Windows NT 3.5.1");
									break;
								}
							case 4: 
								{
									strOSName = _T("Windows NT 4.0");
									break;
								}
							case 5:
								{
									switch (osvi.dwMinorVersion) {
										case 0: 
											{
												strOSName = _T("Windows 2000");
												break;
											}
										case 1:
											{
												strOSName = _T("Windows XP");
												break;
											}
										case 2:
											{
												if (osvi.wSuiteMask == VER_SUITE_WH_SERVER)
													strOSName = _T("Windows Home Server");
												else if (osvi.wProductType == VER_NT_WORKSTATION && si.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_AMD64)
													strOSName = _T("Windows XP");
												else {
													if (GetSystemMetrics(SM_SERVERR2) == 0)
														strOSName = _T("Windows Server 2003");
													else
														strOSName = _T("Windows Server 2003 R2");
												}

												break;
											}
									}
								}
							case 6:
								{
									switch (osvi.dwMinorVersion) {
										case 0: 
											{
												if (osvi.wProductType == VER_NT_WORKSTATION)
													strOSName = _T("Windows Vista");
												else
													strOSName = _T("Windows Server 2008");
												break;
											}
										case 1:
											{
												if (osvi.wProductType == VER_NT_WORKSTATION)
													strOSName = _T("Windows 7");
												else
													strOSName = _T("Windows Server 2008 R2");
												break;
											}
										case 2:
											{
												if (osvi.wProductType == VER_NT_WORKSTATION)
													strOSName = _T("Windows 8");
												else
													strOSName = _T("Windows Server 2012");

												break;
											}
									}
									break;
								}
						}
					}
					break;
		}

		this->strVersion = strOSName;
		this->nServicePack = osvi.wServicePackMajor;

		// Get OS Architecture
		if (RegOpenKeyEx(HKEY_LOCAL_MACHINE, _T("SYSTEM\\CurrentControlSet\\Control\\Session Manager\\Environment"), 0, KEY_READ, &hKey) == ERROR_SUCCESS) {
			LPTSTR szArch = (LPTSTR)malloc(sizeof(TCHAR)*100);

			ZeroMemory(szArch, 100);

			if (RegQueryValueEx(hKey, _T("PROCESSOR_ARCHITECTURE"), NULL, NULL, (LPBYTE)szArch, &dwSize) == ERROR_SUCCESS) {
				if (_tcscmp(szArch, _T("AMD64")) == 0)
					this->nArchitecture = 64;
				else
					this-> nArchitecture = 32;
			} else {
				this->nArchitecture = (sizeof(PVOID) == 4 ? 32 : 64);
			}

			free(szArch);

			RegCloseKey(hKey);
		}
	}

	void GetJavaVer() {
		HKEY hKey;
		DWORD dwSize;
		LPTSTR szJavaVer = (LPTSTR)malloc(sizeof(TCHAR)*100);

		ZeroMemory(szJavaVer, 100);

		if (cWindowsHardware.nCPUArch == 64) {
			if (RegOpenKeyEx(HKEY_LOCAL_MACHINE, _T("SOFTWARE\\JavaSoft\\Java Runtime Environment"), 0, KEY_READ, &hKey) == ERROR_SUCCESS) {
				if (RegQueryValueEx(hKey, _T("CurrentVersion"), NULL, NULL, (LPBYTE)szJavaVer, &dwSize) == ERROR_SUCCESS)
					this->strJavaVer = szJavaVer;

				RegCloseKey(hKey);
			}
		}

		if (this->strJavaVer.IsEmpty()) {
			if (RegOpenKeyEx(HKEY_LOCAL_MACHINE, _T("SOFTWARE\\JavaSoft\\Java Runtime Environment"), 0, KEY_READ|KEY_WOW64_32KEY, &hKey) == ERROR_SUCCESS) {
				ZeroMemory(szJavaVer, 100);

				if (RegQueryValueEx(hKey, _T("CurrentVersion"), NULL, NULL, (LPBYTE)szJavaVer, &dwSize) == ERROR_SUCCESS)
					this->strJavaVer = szJavaVer;

				RegCloseKey(hKey);
			} else {
				this->strJavaVer = _T("0.0");
			}
		}

		free(szJavaVer);
	}

	void GetFrameworkVer() {
		HKEY hKey, hSubKey;
		DWORD dwSize;

		this->nFrameworkSP = 0;

		if (RegOpenKeyEx(HKEY_LOCAL_MACHINE, _T("SOFTWARE\\Microsoft\\NET Framework Setup\\NDP"), 0, KEY_READ, &hKey) == ERROR_SUCCESS) {
			DWORD dwServicePack = 0;

			if (RegOpenKeyEx(hKey, _T("v4"), 0, KEY_READ, &hSubKey) == ERROR_SUCCESS) {
				this->strFrameworkVer = _T("4.0");

				if (RegQueryValueEx(hKey, _T("SP"), NULL, NULL, (LPBYTE)&dwServicePack, &dwSize) == ERROR_SUCCESS)
					this->nFrameworkSP = dwServicePack;
			} else if (RegOpenKeyEx(hKey, _T("v3.5"), 0, KEY_READ, &hSubKey) == ERROR_SUCCESS) {
				this->strFrameworkVer = _T("3.5");

				if (RegQueryValueEx(hKey, _T("SP"), NULL, NULL, (LPBYTE)&dwServicePack, &dwSize) == ERROR_SUCCESS)
					this->nFrameworkSP = dwServicePack;
			} else if (RegOpenKeyEx(hKey, _T("v3.0"), 0, KEY_READ, &hSubKey) == ERROR_SUCCESS) {
				this->strFrameworkVer = _T("3.0");

				if (RegQueryValueEx(hKey, _T("SP"), NULL, NULL, (LPBYTE)&dwServicePack, &dwSize) == ERROR_SUCCESS)
					this->nFrameworkSP = dwServicePack;
			} else if (RegOpenKeyEx(hKey, _T("v2.0.50727"), 0, KEY_READ, &hSubKey) == ERROR_SUCCESS) {
				this->strFrameworkVer = _T("2.0.50727");

				if (RegQueryValueEx(hKey, _T("SP"), NULL, NULL, (LPBYTE)&dwServicePack, &dwSize) == ERROR_SUCCESS)
					this->nFrameworkSP = dwServicePack;
			} else if (RegOpenKeyEx(hKey, _T("v1.1.4322"), 0, KEY_READ, &hSubKey) == ERROR_SUCCESS) {
				this->strFrameworkVer = _T("1.1.4322");

				if (RegQueryValueEx(hKey, _T("SP"), NULL, NULL, (LPBYTE)&dwServicePack, &dwSize) == ERROR_SUCCESS)
					this->nFrameworkSP = dwServicePack;
			} 

			if (hSubKey)
				RegCloseKey(hSubKey);

			RegCloseKey(hKey);
		} else if (RegOpenKeyEx(HKEY_LOCAL_MACHINE, _T("SOFTWARE\\Microsoft\\.NETFramework\\policy\\v1.0"), 0, KEY_READ, &hKey) == ERROR_SUCCESS) {
			DWORD dwServicePack = 0;

			if (RegQueryValueEx(hKey, _T("SP"), NULL, NULL, (LPBYTE)&dwServicePack, &dwSize) == ERROR_SUCCESS)
				this->nFrameworkSP = dwServicePack;

			RegCloseKey(hKey);
		} else {
			this->strFrameworkVer = _T("0.0");
			this->nFrameworkSP = 0;
		}
	}
};