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

class EventData {
public:
	EventData(CString strEventCode, CString strSessionId, int nFlowId = 0) {
		this->Add(_T("tp"), strEventCode);
		this->Add(_T("ss"), strSessionId);
		this->Add(_T("ts"), static_cast<int>(std::time(nullptr)));

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