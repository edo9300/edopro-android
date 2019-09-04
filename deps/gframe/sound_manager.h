#ifndef SOUNDMANAGER_H
#define SOUNDMANAGER_H

#include "game.h"
#include <random>
#ifdef YGOPRO_USE_IRRKLANG
#include <irrKlang.h>
#elif defined(_IRR_ANDROID_PLATFORM_)
#ifdef ANDROID_SOUND_POOL_WRAPPER
#include <android/SoundPoolWrapperTracker.h>
#elif defined (ANDROID_OPEN_SL_SOUND_TRACKER)
#include <android/OpenSLSoundTracker.h>
#endif
#endif

namespace ygo {

class SoundManager {
private:
	std::vector<std::string> BGMList[8];
	std::map<unsigned int,std::string> ChantsList;
	int bgm_scene;
#ifdef YGOPRO_USE_IRRKLANG
	irrklang::ISoundEngine* engineSound;
	irrklang::ISoundEngine* engineMusic;
	irrklang::ISound* soundBGM;
#elif (defined(ANDROID_SOUND_POOL_WRAPPER) || defined(ANDROID_OPEN_SL_SOUND_TRACKER))
	IAudioTracker* m_pAudioTracker;
// 	IYGOSoundEffectPlayer* soundEffectPlayer;
#endif
	void RefershBGMDir(path_string path, int scene);
	void RefreshChantsList();
	std::mt19937 rnd;

public:
	~SoundManager() {
#ifdef YGOPRO_USE_IRRKLANG
		if(engineSound)
			engineSound->drop();
		if(engineMusic)
			engineMusic->drop();
		if(soundBGM)
			soundBGM->drop();
#elif (defined(ANDROID_SOUND_POOL_WRAPPER) || defined(ANDROID_OPEN_SL_SOUND_TRACKER))
		if(m_pAudioTracker)
			delete m_pAudioTracker;
#endif
	}
	enum Sounds {
		SUMMON,
		SPECIAL_SUMMON,
		ACTIVATE,
		SET,
		FLIP,
		REVEAL,
		EQUIP,
		DESTROYED,
		BANISHED,
		TOKEN,
		ATTACK,
		DIRECT_ATTACK,
		DRAW,
		SHUFFLE,
		DAMAGE,
		RECOVER,
		COUNTER_ADD,
		COUNTER_REMOVE,
		COIN,
		DICE,
		NEXT_TURN,
		PHASE,
		PLAYER_ENTER,
		CHAT
	};
	bool Init(void* payload = nullptr);
	void RefreshBGMList();
	void PlaySoundEffect(Sounds sound);
	void PlayMusic(const std::string& song, bool loop);
	void PlayBGM(int scene);
	void StopBGM();
	bool PlayChant(unsigned int code);
	void SetSoundVolume(double volume);
	void SetMusicVolume(double volume);
};

extern SoundManager soundManager;

#define BGM_ALL						0
#define BGM_DUEL					1
#define BGM_MENU					2
#define BGM_DECK					3
#define BGM_ADVANTAGE				4
#define BGM_DISADVANTAGE			5
#define BGM_WIN						6
#define BGM_LOSE					7

}

#endif //SOUNDMANAGER_H
