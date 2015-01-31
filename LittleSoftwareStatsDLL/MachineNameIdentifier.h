#include "MachineIdentifier.h"

class MachineNameIdentifier : MachineIdentifier {
public:
	MachineNameIdentifier(void) { }
	~MachineNameIdentifier(void) { }
	LPBYTE GetIdentifierHash() {
		TCHAR szId[100];
		DWORD nLength = 100;

		ZeroMemory(szId, 100);

		GetComputerName(szId, &nLength);

		return this->ComputeHash(szId);
	}
};