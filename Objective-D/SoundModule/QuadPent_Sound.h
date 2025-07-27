#pragma once
#include <fmod.hpp>
#include <fmod_errors.h>
#include <DirectXMath.h>
#include <string>

using namespace DirectX;
using xmfloat2 = XMFLOAT2;

namespace QP {
	class QuadPent_SoundData;
	class QuadPent_SoundAttribute;
	class Sound;

	class QuadPent_SoundSystem {
	private:
		friend QuadPent_SoundData;
		friend QuadPent_SoundAttribute;
		friend Sound;

		FMOD::System* System{};
		void*         ExtDvData{};
		bool          InitState{};
		FMOD_RESULT   Result{};
		FMOD_VECTOR   ListenerPosition{};

	public:
		void Init();
		void SetSurrondValue(float DoplerScale, float DistanceFactor, float RolloffScale);
		void SetListenerPosition(const xmfloat2& Position);
		void Update();
		void StopAllSounds();
	};
	extern QuadPent_SoundSystem SoundSystem;

	class QuadPent_SoundData {
	protected:
		FMOD::Sound* Data{};
		bool         Loaded{};

		bool LoadFile(const std::string& Directory, FMOD_MODE Option);
	};

	class QuadPent_SoundAttribute : public QuadPent_SoundData {
	protected:
		FMOD::Channel*      Channel{};
		FMOD::ChannelGroup* ChannelGroup{};

		FMOD_VECTOR         SoundPosition{};

		float               PlayVolume{ 1.0 };
		float               PlaySpeed{ 1.0 };
		unsigned int        TotalTime{};

	public:
		unsigned int TotalPlayTime();
		void Play(float Time = 0.0);
		void PlayOnce(bool& BoolValue, float Time = 0.0);
		void Pause();
		void Resume();
		void Stop();

		void SetDistance(float Min, float Max);
		void SetPosition(const xmfloat2& Position, float DifferenceFactor=1.0f);
	};

	class Sound : public QuadPent_SoundAttribute {
	public:
		~Sound();
		void Load(const std::string& Directory, FMOD_MODE Option = FMOD_DEFAULT);
		void Unload();
	};
}