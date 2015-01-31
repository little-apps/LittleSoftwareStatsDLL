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