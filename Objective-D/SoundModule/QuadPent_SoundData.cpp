#include "QuadPent_Sound.h"
#include <windows.h>

bool QP::QuadPent_SoundData::LoadFile(const std::string& Directory, FMOD_MODE Option) {
	HRESULT Result = SoundSystem.System->createSound((Directory).c_str(), Option, 0, &Data);
	if (Result != FMOD_OK) {
		return false;
	}

	return true;
}