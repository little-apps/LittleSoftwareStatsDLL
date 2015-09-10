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
		if (g_bStarted)
			return;

		g_strApiUrl = szApiUrl;
		g_nApiFormat = nApiType;

		g_strApplicationId = szAppId;
		g_strApplicationVer = szAppVer;

		if (g_strApiUrl.IsEmpty() || g_strApplicationId.IsEmpty() || g_strApplicationVer.IsEmpty())
			return;

		if (g_nApiFormat != FORMAT_JSON && g_nApiFormat != FORMAT_XML)
			return;
		
		GetMachineHash(g_strUniqueId);

		g_strSessionId = GenerateId();

		EventData ev(_T("strApp"), g_strSessionId);
		WindowsOperatingSystem cWindowsOS;

		ev.Add(_T("ID"), g_strUniqueId);
		ev.Add(_T("aid"), g_strApplicationId);
		ev.Add(_T("aver"), g_strApplicationVer);

		ev.Add(_T("osv"), cWindowsOS.m_strVersion);
		ev.Add(_T("ossp"), cWindowsOS.m_nServicePack);
		ev.Add(_T("osar"), cWindowsOS.m_nArchitecture);
		ev.Add(_T("osjv"), cWindowsOS.m_strJavaVer);
		ev.Add(_T("osnet"), cWindowsOS.m_strFrameworkVer);
		ev.Add(_T("osnsp"), cWindowsOS.m_nFrameworkSP);
		ev.Add(_T("oslng"), cWindowsOS.m_nLcid);

		ev.Add(_T("osscn"), cWindowsOS.m_cWindowsHardware.m_strScreenRes);
		ev.Add(_T("cnm"), cWindowsOS.m_cWindowsHardware.m_strCPUName);
		ev.Add(_T("car"), cWindowsOS.m_cWindowsHardware.m_nCPUArch);
		ev.Add(_T("cbr"), cWindowsOS.m_cWindowsHardware.m_strCPUManufacturer);
		ev.Add(_T("cfr"), cWindowsOS.m_cWindowsHardware.m_nCPUFreq);
		ev.Add(_T("ccr"), cWindowsOS.m_cWindowsHardware.m_nCPUCores);
		ev.Add(_T("mtt"), cWindowsOS.m_cWindowsHardware.m_nMemTotal);
		ev.Add(_T("mfr"), cWindowsOS.m_cWindowsHardware.m_nMemFree);
		ev.Add(_T("dtt"), cWindowsOS.m_cWindowsHardware.m_nDiskTotal);
		ev.Add(_T("dfr"), cWindowsOS.m_cWindowsHardware.m_nDiskFree);

		cEvents.Add(ev);

		g_bStarted = TRUE;

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
		if (!g_bStarted)
			return;

		DWORD dwRetVal;

		Cache cCache(g_strApplicationId);

		cEvents.Add(EventData(CString(_T("stApp")), g_strSessionId));

		CStringW strData, strPostData;

		strData = cEvents.Serialize();

		strPostData = cCache.GetPostData(strData);

		DWORD dwThreadId;
		HANDLE hThread = CreateThread(nullptr, 0, SendPost, static_cast<LPVOID>(&strPostData), 0, &dwThreadId);

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
		if (!g_bStarted)
			return;

		EventData ev(CString(_T("ev")), g_strSessionId, ++g_nFlowNumber);

		ev.Add(_T("ca"), szCategoryName);
		ev.Add(_T("nm"), szEventName);

		cEvents.Add(ev);
	}

	__declspec(dllexport) void EventValue(LPCTSTR szCategoryName, LPCTSTR szEventName, LPCTSTR szEventValue) {
		if (!g_bStarted)
			return;

		EventData ev(CString(_T("evV")), g_strSessionId, ++g_nFlowNumber);

		ev.Add(_T("ca"), szCategoryName);
		ev.Add(_T("nm"), szEventName);
		ev.Add(_T("vl"), szEventValue);

		cEvents.Add(ev);
	}

	__declspec(dllexport) void EventPeriod(LPCTSTR szCategoryName, LPCTSTR szEventName, int nEventDuration, BOOL bCompleted) {
		if (!g_bStarted)
			return;

		EventData ev(CString(_T("evP")), g_strSessionId, ++g_nFlowNumber);

		ev.Add(_T("ca"), szCategoryName);
		ev.Add(_T("nm"), szEventName);
		ev.Add(_T("tm"), nEventDuration);
		ev.Add(_T("ec"), static_cast<int>(bCompleted));

		cEvents.Add(ev);
	}

	__declspec(dllexport) void Log(LPCTSTR szLogMessage) {
		if (!g_bStarted)
			return;

		EventData ev(CString(_T("lg")), g_strSessionId, ++g_nFlowNumber);

		ev.Add(_T("ms"), szLogMessage);

		cEvents.Add(ev);
	}

	__declspec(dllexport) void CustomData(LPCTSTR szDataName, LPCTSTR szDataValue) {
		if (!g_bStarted)
			return;

		EventData ev(CString(_T("ctD")), g_strSessionId, ++g_nFlowNumber);

		ev.Add(_T("nm"), szDataName);
		ev.Add(_T("vl"), szDataValue);

		cEvents.Add(ev);
	}

	__declspec(dllexport) void License(TCHAR chLicense) {
		if (!g_bStarted)
			return;

		if (islower(chLicense))
			chLicense = toupper(chLicense);

		if (chLicense != LICENSE_FREE &&
			chLicense != LICENSE_TRIAL &&
			chLicense != LICENSE_DEMO &&
			chLicense != LICENSE_REGISTERED &&
			chLicense != LICENSE_CRACKED)
			return;


		EventData ev(CString(_T("ctD")), g_strSessionId, ++g_nFlowNumber);

		ev.Add(_T("nm"), _T("License"));
		ev.Add(_T("vl"), CString(chLicense, 1));

		cEvents.Add(ev);
	}

	__declspec(dllexport) void Exception(LPCTSTR szMessage, LPCTSTR szStackTrace, LPCTSTR szSource, LPCTSTR szTargetSite) {
		if (!g_bStarted)
			return;

		EventData ev(CString(_T("exC")), g_strSessionId, ++g_nFlowNumber);

		ev.Add(_T("msg"), szMessage);
		ev.Add(_T("stk"), szStackTrace);
		ev.Add(_T("src"), szSource);
		ev.Add(_T("tgs"), szTargetSite);

		cEvents.Add(ev);
	}

	__declspec(dllexport) void Install(LPCTSTR szAppId, LPCTSTR szAppVer) {
		EventData ev(CString(_T("ist")), g_strSessionId, ++g_nFlowNumber);

		if (g_strUniqueId.IsEmpty())
			GetMachineHash(g_strUniqueId);
		if (g_strSessionId.IsEmpty())
			g_strSessionId = GenerateId();

		g_strApplicationId = szAppId;
		g_strApplicationVer = szAppVer;

		if (g_strApplicationId.IsEmpty() || g_strApplicationVer.IsEmpty())
			return;

		ev.Add(_T("ID"), g_strUniqueId);
		ev.Add(_T("aid"), g_strApplicationId);
		ev.Add(_T("aver"), g_strApplicationVer);

		cEvents.Add(ev);
	}

	__declspec(dllexport) void Uninstall(LPCTSTR szAppId, LPCTSTR szAppVer) {
		EventData ev(CString(_T("ust")), g_strSessionId, ++g_nFlowNumber);

		if (g_strUniqueId.IsEmpty())
			GetMachineHash(g_strUniqueId);
		if (g_strSessionId.IsEmpty())
			g_strSessionId = GenerateId();

		g_strApplicationId = szAppId;
		g_strApplicationVer = szAppVer;

		if (g_strApplicationId.IsEmpty() || g_strApplicationVer.IsEmpty())
			return;

		ev.Add(_T("ID"), g_strUniqueId);
		ev.Add(_T("aid"), g_strApplicationId);
		ev.Add(_T("aver"), g_strApplicationVer);

		cEvents.Add(ev);
	}
};