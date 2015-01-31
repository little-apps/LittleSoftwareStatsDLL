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

#include "targetver.h"

#if defined _DEBUG || defined DEBUG
#define _SECURE_SCL 0
#endif

#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
// Windows Header Files:
#include <afx.h>
#include <afxtempl.h>
#include <wincrypt.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>	  
#include <iphlpapi.h>
#include <tchar.h>
#include <wininet.h>
#include <typeinfo>
#include <vector>
#include <ctime>
#include <map>
#include <algorithm>
#include <ObjBase.h>

#pragma comment(lib, "iphlpapi.lib")
#pragma comment(lib, "rpcrt4.lib")
#pragma comment(lib, "wininet.lib")
#pragma comment(lib, "advapi32.lib")

#ifndef TRACE
#define TRACE(x) OutputDebugString(x)
#endif

#include "config.h"
#include "Utils.h"
#include "Cache.h"
#include "WindowsOperatingSystem.h"
#include "Events\Events.h"
#include "MachineNameIdentifier.h"
#include "NetworkAdapterIdentifier.h"
#include "VolumeInfoIdentifier.h"
#include "md5/md5.h"
