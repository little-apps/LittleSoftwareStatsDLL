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

#include "..\rapidjson\writer.h"
#include "..\rapidjson\stringbuffer.h"

#include "EventDataValue.h"
#include "EventData.h"

using namespace rapidjson;

extern int nApiFormat;

class Events {
public:
	Events() { }
	~Events() { }

	void Add(EventData ev) {
		this->events_vector.push_back(ev);
	}

	CStringW Serialize() {
		CStringW strOutput;
		size_t i, j;
		std::map<CString, EventDataValue>::iterator it;

		if (nApiFormat == FORMAT_JSON) {
			StringBuffer s;
			Writer<StringBuffer> writer(s);

			writer.StartArray();

			for (i = 0; i != this->events_vector.size(); i++) {
				EventData ev = this->events_vector[i];

				writer.StartObject();

				j = 0;

				for (it = ev.hash_table.begin(); it != ev.hash_table.end(); ++it) {
					CString key = it->first;
					EventDataValue value = it->second;

#ifdef UNICODE
					LPSTR szKey = ConvertUTF16ToUTF8(key);
#else
					LPCSTR szKey = key;
#endif

					writer.String(szKey);

					if (value.Type == EventDataValue::STRING) {
						CString szValue = (CString)value;

#ifdef UNICODE
						LPSTR szValueMultiByte = ConvertUTF16ToUTF8(szValue);
#else
						LPCSTR szValueMultiByte = szValue;
#endif

						writer.String(szValueMultiByte);
					}
					else if (value.Type == EventDataValue::INT) {
						int n = value;
						writer.Int(n);
					}
					else if (value.Type == EventDataValue::DOUBLE) {
						double d = value;
						writer.Double(d);
					}
					else if (value.Type == EventDataValue::ULONG) {
						ULONG ul = value;
						writer.Uint(ul);
					}
				}

				writer.EndObject();
			}

			writer.EndArray();

			return s.GetString();
		} else {
			strOutput += L"<Events>";

			for (i = 0; i != this->events_vector.size(); i++) {
				EventData ev = this->events_vector[i];

				strOutput += _T("<Event>");

				j = 0;

				for (it = ev.hash_table.begin(); it != ev.hash_table.end(); ++it) {
					CString key = it->first;
					EventDataValue value = it->second;

					strOutput += StringFormat(_T("<%s>"), key.GetString());

					if (value.Type == EventDataValue::STRING) {
						CString str = value;
						strOutput += str;
					}
					else if (value.Type == EventDataValue::INT) {
						int n = value;
						strOutput += StringFormat(_T("%d"), n);
					}
					else if (value.Type == EventDataValue::DOUBLE) {
						double d = value;
						strOutput += StringFormat(_T("%d"), static_cast<int>(d));
					}
					else if (value.Type == EventDataValue::ULONG) {
						ULONG ul = value;
						strOutput += StringFormat(_T("%u"), ul);
					}

					strOutput += StringFormat(_T("</%s>"), key.GetString());
				}

				strOutput += _T("</Event>");
			}

			strOutput += L"</Events>";

			return strOutput;
		}

		
	}

	std::vector<EventData> events_vector;
};