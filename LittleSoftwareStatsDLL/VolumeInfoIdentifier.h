#include "MachineIdentifier.h"

class VolumeInfoIdentifier : MachineIdentifier {
public:
	VolumeInfoIdentifier(void) { }
	~VolumeInfoIdentifier(void) { }
	LPBYTE GetIdentifierHash() {
		TCHAR szId[20];
		TCHAR szVolumeName[MAX_PATH];
		TCHAR szFileSystemName[MAX_PATH];
		DWORD dwSerialNumber = 0;
		DWORD dwMaxComponentLen = 0;
		DWORD dwFileSystemFlags = 0;

		ZeroMemory(szId, 20);
		ZeroMemory(szVolumeName, MAX_PATH);
		ZeroMemory(szFileSystemName, MAX_PATH);

		if (GetVolumeInformation(
			NULL,
			szVolumeName,
			sizeof(szVolumeName),
			&dwSerialNumber,
			&dwMaxComponentLen,
			&dwFileSystemFlags,
			szFileSystemName,
			sizeof(szFileSystemName))) {
				_stprintf_s(szId, 20, _T("%lu"), dwSerialNumber); 
		}


		return this->ComputeHash(szId);
	}
};