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
#include "Events\Events.h"
#include "globals.h"

#define LICENSE_FREE _T('F')
#define LICENSE_DEMO _T('D')
#define LICENSE_TRIAL _T('T')
#define LICENSE_REGISTERED _T('R')
#define LICENSE_CRACKED _T('C')


extern "C" {
	Events cEvents;

	__declspec(dllexport) void Start(LPCTSTR szApiUrl, int nApiType, LPCTSTR szAppId, LPCTSTR szAppVer) {
		if (bStarted)
			return;

		strApiUrl = szApiUrl;
		nApiFormat = nApiType;

		strApplicationId = szAppId;
		strApplicationVer = szAppVer;

		if (strApiUrl.IsEmpty() || strApplicationId.IsEmpty() || strApplicationVer.IsEmpty())
			return;

		if (nApiFormat != FORMAT_JSON && nApiFormat != FORMAT_XML)
			return;
		
		GetMachineHash(strUniqueId);

		strSessionId = GenerateId();

		EventData ev(_T("strApp"), strSessionId);
		WindowsOperatingSystem cWindowsOS;

		ev.Add(_T("ID"), strUniqueId);
		ev.Add(_T("aid"), strApplicationId);
		ev.Add(_T("aver"), strApplicationVer);

		ev.Add(_T("osv"), cWindowsOS.strVersion);
		ev.Add(_T("ossp"), cWindowsOS.nServicePack);
		ev.Add(_T("osar"), cWindowsOS.nArchitecture);
		ev.Add(_T("osjv"), cWindowsOS.strJavaVer);
		ev.Add(_T("osnet"), cWindowsOS.strFrameworkVer);
		ev.Add(_T("osnsp"), cWindowsOS.nFrameworkSP);
		ev.Add(_T("oslng"), cWindowsOS.nLcid);

		ev.Add(_T("osscn"), cWindowsOS.cWindowsHardware.strScreenRes);
		ev.Add(_T("cnm"), cWindowsOS.cWindowsHardware.strCPUName);
		ev.Add(_T("car"), cWindowsOS.cWindowsHardware.nCPUArch);
		ev.Add(_T("cbr"), cWindowsOS.cWindowsHardware.strCPUManufacturer);
		ev.Add(_T("cfr"), cWindowsOS.cWindowsHardware.nCPUFreq);
		ev.Add(_T("ccr"), cWindowsOS.cWindowsHardware.nCPUCores);
		ev.Add(_T("mtt"), cWindowsOS.cWindowsHardware.nMemTotal);
		ev.Add(_T("mfr"), cWindowsOS.cWindowsHardware.nMemFree);
		ev.Add(_T("dtt"), cWindowsOS.cWindowsHardware.nDiskTotal);
		ev.Add(_T("dfr"), cWindowsOS.cWindowsHardware.nDiskFree);

		cEvents.Add(ev);

		bStarted = TRUE;

		/* For debugging use */
		/*CString strOutput = cEvents.Serialize();

		_tprintf(_T("Output: %s\n"), strOutput.GetString());

		_tprintf(_T("Hardware info: \n"));
		_tprintf(_T("Memory: %d/%d\n"), (int)cWindowsOS.cWindowsHardware.nMemFree, (int)cWindowsOS.cWindowsHardware.nMemTotal);
		_tprintf(_T("Disk space: %lu/%lu\n"), cWindowsOS.cWindowsHardware.nDiskFree, cWindowsOS.cWindowsHardware.nDiskTotal);
		_tprintf(_T("CPU Name: %s\n"), cWindowsOS.cWindowsHardware.strCPUName.GetString());
		_tprintf(_T("CPU Manufacturer: %s\n"), cWindowsOS.cWindowsHardware.strCPUManufacturer.GetString());

		_tprintf(_T("\nOS info: \n"));
		_tprintf(_T("Version: %s\n"), cWindowsOS.strVersion.GetString());
		_tprintf(_T("Service Pack: %d\n"), cWindowsOS.nServicePack);
		_tprintf(_T("Architecture: %d-bit\n"), cWindowsOS.nArchitecture);
		_tprintf(_T(".NET Framework: %s (SP: %d)\n"), cWindowsOS.strFrameworkVer.GetString(), cWindowsOS.nFrameworkSP);
		_tprintf(_T("Java: %s\n"), cWindowsOS.strJavaVer.GetString());*/
	}

	__declspec(dllexport) void Stop() {
		if (!bStarted)
			return;

		DWORD dwRetVal;

		Cache cCache(strApplicationId);

		cEvents.Add(EventData(CString(_T("stApp")), strSessionId));

		CStringW strData, strPostData;

		strData = cEvents.Serialize();

		strPostData = cCache.GetPostData(strData);

		DWORD dwThreadId;
		HANDLE hThread = CreateThread(NULL, 0, SendPost, (LPVOID)&strPostData, 0, &dwThreadId);

		WaitForSingleObject(hThread, INFINITE);

		GetExitCodeThread(hThread, &dwRetVal);

		// If failed to send data to API -> cache it for later
		if (dwRetVal == FALSE) {
			cCache.SaveData(strData);
		} else {
			// Otherwise, clear cache
			cCache.Clear();
		}
	}

	__declspec(dllexport) void Event(LPCTSTR szCategoryName, LPCTSTR szEventName) {
		if (!bStarted)
			return;

		EventData ev(CString(_T("ev")), strSessionId, ++nFlowNumber);

		ev.Add(_T("ca"), szCategoryName);
		ev.Add(_T("nm"), szEventName);

		cEvents.Add(ev);
	}

	__declspec(dllexport) void EventValue(LPCTSTR szCategoryName, LPCTSTR szEventName, LPCTSTR szEventValue) {
		if (!bStarted)
			return;

		EventData ev(CString(_T("evV")), strSessionId, ++nFlowNumber);

		ev.Add(_T("ca"), szCategoryName);
		ev.Add(_T("nm"), szEventName);
		ev.Add(_T("vl"), szEventValue);

		cEvents.Add(ev);
	}

	__declspec(dllexport) void EventPeriod(LPCTSTR szCategoryName, LPCTSTR szEventName, int nEventDuration, BOOL bCompleted) {
		if (!bStarted)
			return;

		EventData ev(CString(_T("evP")), strSessionId, ++nFlowNumber);

		ev.Add(_T("ca"), szCategoryName);
		ev.Add(_T("nm"), szEventName);
		ev.Add(_T("tm"), nEventDuration);
		ev.Add(_T("ec"), static_cast<int>(bCompleted));

		cEvents.Add(ev);
	}

	__declspec(dllexport) void Log(LPCTSTR szLogMessage) {
		if (!bStarted)
			return;

		EventData ev(CString(_T("lg")), strSessionId, ++nFlowNumber);

		ev.Add(_T("ms"), szLogMessage);

		cEvents.Add(ev);
	}

	__declspec(dllexport) void CustomData(LPCTSTR szDataName, LPCTSTR szDataValue) {
		if (!bStarted)
			return;

		EventData ev(CString(_T("ctD")), strSessionId, ++nFlowNumber);

		ev.Add(_T("nm"), szDataName);
		ev.Add(_T("vl"), szDataValue);

		cEvents.Add(ev);
	}

	__declspec(dllexport) void License(TCHAR chLicense) {
		if (!bStarted)
			return;

		if (islower(chLicense))
			chLicense = toupper(chLicense);

		if (chLicense != LICENSE_FREE &&
			chLicense != LICENSE_TRIAL &&
			chLicense != LICENSE_DEMO &&
			chLicense != LICENSE_REGISTERED &&
			chLicense != LICENSE_CRACKED)
			return;


		EventData ev(CString(_T("ctD")), strSessionId, ++nFlowNumber);

		ev.Add(_T("nm"), _T("License"));
		ev.Add(_T("vl"), CString(chLicense, 1));

		cEvents.Add(ev);
	}

	__declspec(dllexport) void Exception(LPCTSTR szMessage, LPCTSTR szStackTrace, LPCTSTR szSource, LPCTSTR szTargetSite) {
		if (!bStarted)
			return;

		EventData ev(CString(_T("exC")), strSessionId, ++nFlowNumber);

		ev.Add(_T("msg"), szMessage);
		ev.Add(_T("stk"), szStackTrace);
		ev.Add(_T("src"), szSource);
		ev.Add(_T("tgs"), szTargetSite);

		cEvents.Add(ev);
	}

	__declspec(dllexport) void Install(LPCTSTR szAppId, LPCTSTR szAppVer) {
		EventData ev(CString(_T("ist")), strSessionId, ++nFlowNumber);

		if (strUniqueId.IsEmpty())
			GetMachineHash(strUniqueId);
		if (strSessionId.IsEmpty())
			strSessionId = GenerateId();

		strApplicationId = szAppId;
		strApplicationVer = szAppVer;

		if (strApplicationId.IsEmpty() || strApplicationVer.IsEmpty())
			return;

		ev.Add(_T("ID"), strUniqueId);
		ev.Add(_T("aid"), strApplicationId);
		ev.Add(_T("aver"), strApplicationVer);

		cEvents.Add(ev);
	}

	__declspec(dllexport) void Uninstall(LPCTSTR szAppId, LPCTSTR szAppVer) {
		EventData ev(CString(_T("ust")), strSessionId, ++nFlowNumber);

		if (strUniqueId.IsEmpty())
			GetMachineHash(strUniqueId);
		if (strSessionId.IsEmpty())
			strSessionId = GenerateId();

		strApplicationId = szAppId;
		strApplicationVer = szAppVer;

		if (strApplicationId.IsEmpty() || strApplicationVer.IsEmpty())
			return;

		ev.Add(_T("ID"), strUniqueId);
		ev.Add(_T("aid"), strApplicationId);
		ev.Add(_T("aver"), strApplicationVer);

		cEvents.Add(ev);
	}
};