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

class EventDataValue {
public:
	enum Types {
		NONE,
		INT,
		STRING,
		DOUBLE,
		ULONG
	};

	EventDataValue() { this->Type = NONE; }

	EventDataValue(int value) {
		this->Type = INT;
		this->i = value;
	}

	EventDataValue(CString value) {
		this->Type = STRING;
		this->s = value;
	}

	EventDataValue(LPCTSTR value) {
		this->Type = STRING;
		this->s = CString(value);
	}

	EventDataValue(double value) {
		this->Type = DOUBLE;
		this->d = value;
	}

	EventDataValue(unsigned long value) {
		this->Type = ULONG;
		this->ul = value;
	}

	~EventDataValue() { }

	operator int(void) const {
		return this->i;
	}

	operator CString(void) const {
		return this->s;
	}

	operator double(void) const {
		return this->d;
	}

	operator unsigned long(void) const {
		return this->ul;
	}


	Types Type;
	int i;
	CString s;
	double d;
	unsigned long ul;
};

class EventData {
public:
	EventData(CString strEventCode, CString strSessionId, int nFlowId = 0) {
		this->Add(_T("tp"), strEventCode);
		this->Add(_T("ss"), strSessionId);
		this->Add(_T("ts"), (int)std::time(0));

		if (nFlowId != 0)
			this->Add(_T("fl"), nFlowId);
	}

	template <typename T>
	void Add(CString name, T value) {
		EventDataValue event_value(value);

		if (this->hash_table.find(name) != this->hash_table.end())
			return;

		this->hash_table.insert(std::make_pair(name, value));
	}

	std::map<CString, EventDataValue> hash_table;
};

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
			strOutput += L"[";

			for (i = 0; i != this->events_vector.size(); i++) {
				EventData ev = this->events_vector[i];

				strOutput += L"{";

				j = 0;

				for (it = ev.hash_table.begin(); it != ev.hash_table.end(); ++it) {
					CString key = it->first;
					EventDataValue value = it->second;

					strOutput += StringFormat(_T("\"%s\": "), key.GetString());

					if (value.Type == EventDataValue::STRING) {
						CString str = value;
						strOutput += Enquoute(str);
					} else if (value.Type == EventDataValue::INT) {
						int n = value;
						strOutput += StringFormat(_T("%d"), n);
					} else if (value.Type == EventDataValue::DOUBLE) {
						double d = value;
						strOutput += StringFormat(_T("%d"), static_cast<int>(d));
					} else if (value.Type == EventDataValue::ULONG) {
						ULONG ul = value;
						strOutput += StringFormat(_T("%u"), ul);
					}

					if (j++ < ev.hash_table.size() - 1)
						strOutput += _T(",");
				}

				strOutput += _T("}");

				if (i < this->events_vector.size() - 1)
					strOutput += _T(",");
			}

			strOutput += L"]";
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
					} else if (value.Type == EventDataValue::INT) {
						int n = value;
						strOutput += StringFormat(_T("%d"), n);
					} else if (value.Type == EventDataValue::DOUBLE) {
						double d = value;
						strOutput += StringFormat(_T("%d"), static_cast<int>(d));
					} else if (value.Type == EventDataValue::ULONG) {
						ULONG ul = value;
						strOutput += StringFormat(_T("%u"), ul);
					}

					strOutput += StringFormat(_T("</%s>"), key.GetString());
				}

				strOutput += _T("</Event>");
			}

			strOutput += L"</Events>";
		}

		return strOutput;
	}

	std::vector<EventData> events_vector;
};