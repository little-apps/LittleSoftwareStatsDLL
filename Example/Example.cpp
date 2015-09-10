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

#include "stdafx.h"

#ifdef UNICODE
	#define WIDEN2(x) L ## x
	#define WIDEN(x) WIDEN2(x)
	#define __WFILE__ WIDEN(__FILE__)
#endif

// Formats
#define FORMAT_XML 0
#define FORMAT_JSON 1

typedef void (__cdecl *fStartProc)(LPCTSTR, int, LPCTSTR, LPCTSTR); 
typedef void (__cdecl *fStopProc)(); 
typedef void (__cdecl *fEventProc)(LPCTSTR, LPCTSTR); 
typedef void (__cdecl *fEventValueProc)(LPCTSTR, LPCTSTR, LPCTSTR); 
typedef void (__cdecl *fEventPeriodProc)(LPCTSTR, LPCTSTR, int, BOOL); 
typedef void (__cdecl *fLogProc)(LPCTSTR); 
typedef void (__cdecl *fCustomDataProc)(LPCTSTR, LPCTSTR); 
typedef void (__cdecl *fLicenseProc)(TCHAR); 
typedef void (__cdecl *fExceptionProc)(LPCTSTR, LPCTSTR, LPCTSTR, LPCTSTR); 
typedef void (__cdecl *fInstallProc)(LPCTSTR, LPCTSTR); 
typedef void (__cdecl *fUninstallProc)(LPCTSTR, LPCTSTR); 

fStartProc fStart;
fStopProc fStop;
fEventProc fEvent;
fEventValueProc fEventValue;
fEventPeriodProc fEventPeriod;
fLogProc fLog;
fCustomDataProc fCustomData;
fLicenseProc fLicense;
fExceptionProc fException;
fInstallProc fInstall;
fUninstallProc fUninstall;

bool LoadDll() {
	HINSTANCE hInst = LoadLibrary(_T("LittleSoftwareStatsDLL.dll"));

	if (hInst != nullptr) {
		if ((fStart = reinterpret_cast<fStartProc>(GetProcAddress(hInst, "Start"))) == nullptr)
			return false;
		if ((fStop = reinterpret_cast<fStopProc>(GetProcAddress(hInst, "Stop"))) == nullptr)
			return false;
		if ((fEvent = reinterpret_cast<fEventProc>(GetProcAddress(hInst, "Event"))) == nullptr)
			return false;
		if ((fEventValue = reinterpret_cast<fEventValueProc>(GetProcAddress(hInst, "EventValue"))) == nullptr)
			return false;
		if ((fEventPeriod = reinterpret_cast<fEventPeriodProc>(GetProcAddress(hInst, "EventPeriod"))) == nullptr)
			return false;
		if ((fLog = reinterpret_cast<fLogProc>(GetProcAddress(hInst, "Log"))) == nullptr)
			return false;
		if ((fCustomData = reinterpret_cast<fCustomDataProc>(GetProcAddress(hInst, "CustomData"))) == nullptr)
			return false;
		if ((fLicense = reinterpret_cast<fLicenseProc>(GetProcAddress(hInst, "License"))) == nullptr)
			return false;
		if ((fException = reinterpret_cast<fExceptionProc>(GetProcAddress(hInst, "Exception"))) == nullptr)
			return false;
		if ((fInstall = reinterpret_cast<fInstallProc>(GetProcAddress(hInst, "Install"))) == nullptr)
			return false;
		if ((fUninstall = reinterpret_cast<fUninstallProc>(GetProcAddress(hInst, "Uninstall"))) == nullptr)
			return false;

		return true;
	}

	return false;
}

void Event() {
	// Event info
	const TCHAR szCategory[] = _T("Buttons");
	const TCHAR szEvent[] = _T("OK");

	// Send event
	fEvent(szCategory, szEvent);
}

void EventValue() {
	// Event value info
	const TCHAR szCategory[] = _T("View");
	const TCHAR szEventName[] = _T("Zoom");
	const TCHAR szEventValue[] = _T("In"); // Or Out

	// Send event value
	fEventValue(szCategory, szEventName, szEventValue);
}

void EventPeriod() {
	// Event period info
	const TCHAR szCategory[] = _T("Main");
	const TCHAR szEventName[] = _T("Scan");
	int nDuration = 60; // equals 60 seconds or 1 minute
	bool bCompleted = false;

	// Send event period
	fEventPeriod(szCategory, szEventName, nDuration, bCompleted);
}

void Log() {
	// Log message
	const TCHAR szLogMsg[] = _T("Hello World!");

	// Send message
	fLog(szLogMsg);
}

void CustomData() {
	// Custom data
	const TCHAR szCustomName[] = _T("Name");
	const TCHAR szCustomValue[] = _T("Joe Blow");

	// Send custom data
	fCustomData(szCustomName, szCustomValue);
}

void License() {
	// Software licenses
	TCHAR chLicenseFree = _T('F');
	TCHAR chLicenseDemo = _T('D');
	TCHAR chLicenseTrial = _T('T');
	TCHAR chLicenseRegistered = _T('R');
	TCHAR chLicenseCracked = _T('C');

	// Cracked license
	fLicense(chLicenseCracked);
}

int _tmain(int argc, _TCHAR* argv[]) {
	UNREFERENCED_PARAMETER(argc);
	UNREFERENCED_PARAMETER(argv);

	if (!LoadDll()) {
		_tprintf(_T("Unable to load DLL"));
		return -1;
	}

	// API info
	const TCHAR szApiUrl[] = _T(""); // URL to API (ie: http://YOURWEBSITEHERE/api.json)
	int nApiFormat = FORMAT_JSON; // Must correspond with extension in URL (FORMAT_XML or FORMAT_JSON)

	// Application info
	const TCHAR szAppId[] = _T(""); // Application ID (ie: 26ad4eb8f804200f1e23b27c1873d703)
	const TCHAR szAppVer[] = _T(""); // Application version (ie: 1.0)

	if (_tccmp(szApiUrl, _T("")) == 0) {
		_tprintf(_T("API URL cannot be empty"));
		return -1;
	}

	if (_tccmp(szAppId, _T("")) == 0) {
		_tprintf(_T("Application ID cannot be empty"));
		return -1;
	}

	if (_tccmp(szAppVer, _T("")) == 0) {
		_tprintf(_T("Application version cannot be empty"));
		return -1;
	}

	// Start tracking
	fStart(szApiUrl, nApiFormat, szAppId, szAppVer);

	Log();

	// Exceptions
	try {
		throw 20;
	} catch(...) {
		LPCTSTR szStackTrace = _T("N/A"); // Set stack trace to "n/a" since were unable to get a stack trace
		LPCTSTR szMessage = _T("Divide by zero");
		LPCTSTR szTarget = _T("");

		TCHAR szSource[512];
		ZeroMemory(szSource, 512);
#ifdef UNICODE
		_stprintf_s(szSource, 512, _T("%s %d"), __WFILE__, __LINE__);
#else
		_stprintf_s(szSource, 512, _T("%s %d"), __FILE__, __LINE__);
#endif

		fException(szMessage, szStackTrace, szSource, szTarget);
	}

	fStop();


	return 0;
}

