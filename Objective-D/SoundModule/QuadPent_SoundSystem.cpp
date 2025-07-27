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
	XMMATRIX invView = XMMatrixInverse(nullptr, XMLoadFloat4x4(&camera.GetViewMatrix()));

	XMVECTOR forwardVec = XMVector3Normalize(invView.r[2]); // ← 오른손계 기준 Z+
	XMVECTOR upVec = XMVector3Normalize(invView.r[1]);      // ← 위쪽 Y+
	XMVECTOR camPosVec = invView.r[3];                      // ← 위치

	FMOD_VECTOR listenerPos = {
		XMVectorGetX(camPosVec),
		XMVectorGetY(camPosVec),
		XMVectorGetZ(camPosVec)
	};
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

	System->set3DListenerAttributes(0, &listenerPos, nullptr, &forward, &up);
}

void QP::QuadPent_SoundSystem::Update() {
	System->update();
}

void QP::QuadPent_SoundSystem::StopAllSounds() {
	FMOD::ChannelGroup* MasterChannelGroup{};
	System->getMasterChannelGroup(&MasterChannelGroup);
	MasterChannelGroup->stop();
}