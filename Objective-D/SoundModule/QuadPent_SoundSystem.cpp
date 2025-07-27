#include "QuadPent_Sound.h"
#include "..//CameraUtil.h"

QP::QuadPent_SoundSystem QP::SoundSystem;

void QP::QuadPent_SoundSystem::Init() {
	if (InitState)
		return;

	Result = FMOD::System_Create(&System);

	if (Result != FMOD_OK)
		exit(EXIT_FAILURE);

	System->init(128, FMOD_INIT_NORMAL, ExtDvData);

	FMOD::ChannelGroup* MasterGroup;
	System->getMasterChannelGroup(&MasterGroup);

	float Volume;
	MasterGroup->getVolume(&Volume);
	if (Volume == 0.0f)
		MasterGroup->setVolume(1.0f);

	System->set3DSettings(1.0, 1.0, 1.0);

	InitState = true;
}

void QP::QuadPent_SoundSystem::SetSurrondValue(float DoplerScale, float DistanceFactor, float RolloffScale) {
	System->set3DSettings(DoplerScale, DistanceFactor, RolloffScale);
}

void QP::QuadPent_SoundSystem::SetListenerPosition(const xmfloat3& Position) {
	XMFLOAT4X4 viewMatrix = camera.GetViewMatrix();
	XMMATRIX viewMat = XMLoadFloat4x4(&viewMatrix);

	// 2. ViewMatrix의 z축은 "리스너가 바라보는 방향" (forward)
	XMVECTOR forwardVec = XMVector3Normalize(viewMat.r[2]); // z-axis
	XMVECTOR upVec = XMVector3Normalize(viewMat.r[1]);       // y-axis

	// 3. XMVECTOR → FMOD_VECTOR로 변환
	FMOD_VECTOR forward = {
		XMVectorGetX(forwardVec),
		XMVectorGetY(forwardVec),
		XMVectorGetZ(forwardVec)
	};

	FMOD_VECTOR up = {
		XMVectorGetX(upVec),
		XMVectorGetY(upVec),
		XMVectorGetZ(upVec)
	};

	xmfloat3 camPos = camera.GetPosition();

	// 4. 리스너 위치도 필요
	FMOD_VECTOR listenerPos = { camPos.x, camPos.y, camPos.z }; // XMFLOAT3에서 추출
	FMOD_VECTOR vel = { 0.0f, 0.0f, 0.0f }; // 정지 중일 경우

	// 5. 설정
	System->set3DListenerAttributes(0, &listenerPos, &vel, &forward, &up);
}

void QP::QuadPent_SoundSystem::Update() {
	System->update();
}

void QP::QuadPent_SoundSystem::StopAllSounds() {
	FMOD::ChannelGroup* MasterChannelGroup{};
	System->getMasterChannelGroup(&MasterChannelGroup);
	MasterChannelGroup->stop();
}