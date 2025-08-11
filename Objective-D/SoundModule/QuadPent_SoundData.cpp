#include "QuadPent_Sound.h"
#include <windows.h>
#include <iostream>

bool QP::QuadPent_SoundData::LoadFile(const std::string& Directory, FMOD_MODE Option) {
	HRESULT Result = SoundSystem.System->createSound((Directory).c_str(), Option, 0, &Data);
	if (Result != FMOD_OK) {
		return false;
	}

	FMOD_MODE mode;
	Data->getMode(&mode);

	/*if (mode & FMOD_3D)
		std::cout << Directory <<  " 3D 사운드로 로드됨\n";
	else
		std::cout << Directory << " 3D 모드 아님\n";*/

	return true;
}