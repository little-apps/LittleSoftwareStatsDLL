#pragma once

#include "md5/md5.h"

class MachineIdentifier {
public:
	LPBYTE ComputeHash(LPTSTR szId) {
		CMD5 cMD5;

		cMD5.Calculate(szId);

		return cMD5.Hash();
	}
	virtual LPBYTE GetIdentifierHash() = 0;
};