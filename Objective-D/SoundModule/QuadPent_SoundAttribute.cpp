#include "QuadPent_Sound.h"

unsigned int QP::QuadPent_SoundAttribute::TotalPlayTime() {
	return TotalTime;
}

void QP::QuadPent_SoundAttribute::Play(float Time) {
	SoundSystem.System->playSound(Data, ChannelGroup, false, &Channel);
	
}

void QP::QuadPent_SoundAttribute::Play3D(float Time) {
	SoundSystem.System->playSound(Data, ChannelGroup, false, &Channel);
	Channel->set3DAttributes(&SoundPosition, 0);
	Channel->set3DMinMaxDistance(1.0f, 50.0f);
}

void QP::QuadPent_SoundAttribute::Stop() {
	ChannelGroup->stop();
}

void QP::QuadPent_SoundAttribute::Resume() {
	ChannelGroup->setPaused(false);
}

void QP::QuadPent_SoundAttribute::Pause() {
	ChannelGroup->setPaused(true);
}

void QP::QuadPent_SoundAttribute::SetDistance(float Min, float Max) {
	ChannelGroup->set3DMinMaxDistance(Min, Max);
}

void QP::QuadPent_SoundAttribute::SetPosition(const xmfloat3& Position) {
	SoundPosition.x = Position.x;
	SoundPosition.y = Position.y;
	SoundPosition.z = Position.z;
}