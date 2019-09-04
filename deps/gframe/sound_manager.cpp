#undef ANDROID_SOUND_POOL_WRAPPER
#undef ANDROID_OPEN_SL_SOUND_TRACKER
#include "sound_manager.h"
#include "utils.h"
#ifdef IRRKLANG_STATIC
#include "../ikpmp3/ikpMP3.h"
#endif

namespace ygo {

SoundManager soundManager;

bool SoundManager::Init(void* payload) {
	rnd.seed(time(0));
	bgm_scene = -1;
	RefreshBGMList();
	RefreshChantsList();
#ifdef YGOPRO_USE_IRRKLANG
	#define playsound(sound) soundEngine->play2D(sound)
	soundEngine = irrklang::createIrrKlangDevice();
// 	engineMusic = irrklang::createIrrKlangDevice();
	if(!soundEngine) {
		return false;
	} else {
#ifdef IRRKLANG_STATIC
		irrklang::ikpMP3Init(engineMusic);
#endif
		return true;
	}
#elif (defined(ANDROID_SOUND_POOL_WRAPPER) || defined(ANDROID_OPEN_SL_SOUND_TRACKER))
#define playsound(sound) m_pAudioTracker->playBGM(sound, AP_TYPE_ASSET)
	auto app = *static_cast<ANDROID_APP*>(payload);
#ifdef ANDROID_OPEN_SL_SOUND_TRACKER
	m_pAudioTracker = new OpenSLSoundTracker(app->activity->assetManager);
#elif defined (ANDROID_SOUND_POOL_WRAPPER)
	m_pAudioTracker = new SoundPoolWrapperTracker(app);
#endif
	if(m_pAudioTracker)
		return true;
#else //YGOPRO_USE_IRRKLANG
#define playsound(sound) break
#endif // YGOPRO_USE_IRRKLANG
	// TODO: Implement other sound engines
	return false;
}
void SoundManager::RefreshBGMList() {
	Utils::Makedirectory(TEXT("./sound/BGM/"));
	Utils::Makedirectory(TEXT("./sound/BGM/duel"));
	Utils::Makedirectory(TEXT("./sound/BGM/menu"));
	Utils::Makedirectory(TEXT("./sound/BGM/deck"));
	Utils::Makedirectory(TEXT("./sound/BGM/advantage"));
	Utils::Makedirectory(TEXT("./sound/BGM/disadvantage"));
	Utils::Makedirectory(TEXT("./sound/BGM/win"));
	Utils::Makedirectory(TEXT("./sound/BGM/lose"));
	Utils::Makedirectory(TEXT("./sound/chants"));
	RefershBGMDir(TEXT(""), BGM_DUEL);
	RefershBGMDir(TEXT("duel"), BGM_DUEL);
	RefershBGMDir(TEXT("menu"), BGM_MENU);
	RefershBGMDir(TEXT("deck"), BGM_DECK);
	RefershBGMDir(TEXT("advantage"), BGM_ADVANTAGE);
	RefershBGMDir(TEXT("disadvantage"), BGM_DISADVANTAGE);
	RefershBGMDir(TEXT("win"), BGM_WIN);
	RefershBGMDir(TEXT("lose"), BGM_LOSE);
}
void SoundManager::RefershBGMDir(path_string path, int scene) {
	for(auto& file : Utils::FindfolderFiles(TEXT("./sound/BGM/") + path, { TEXT("mp3"), TEXT("ogg"), TEXT("wav") })) {
		auto conv = Utils::ToUTF8IfNeeded(path + TEXT("/") + file);
		BGMList[BGM_ALL].push_back(conv);
		BGMList[scene].push_back(conv);
	}
}
void SoundManager::RefreshChantsList() {
	for(auto& file : Utils::FindfolderFiles(TEXT("./sound/chants"), { TEXT("mp3"), TEXT("ogg"), TEXT("wav") })) {
		auto scode = Utils::GetFileName(TEXT("./sound/chants/") + file);
		unsigned int code = std::stoi(scode);
		if(code && !ChantsList.count(code))
			ChantsList[code] = Utils::ToUTF8IfNeeded(file);
	}
}
void SoundManager::PlaySoundEffect(Sounds sound) {
#ifdef YGOPRO_USE_IRRKLANG
	if(!mainGame->chkEnableSound->isChecked())
		return;
	switch(sound) {
		case SUMMON: {
			playsound("./sound/summon.wav");
			break;
		}
		case SPECIAL_SUMMON: {
			playsound("./sound/specialsummon.wav");
			break;
		}
		case ACTIVATE: {
			playsound("./sound/activate.wav");
			break;
		}
		case SET: {
			playsound("./sound/set.wav");
			break;
		}
		case FLIP: {
			playsound("./sound/flip.wav");
			break;
		}
		case REVEAL: {
			playsound("./sound/reveal.wav");
			break;
		}
		case EQUIP: {
			playsound("./sound/equip.wav");
			break;
		}
		case DESTROYED: {
			playsound("./sound/destroyed.wav");
			break;
		}
		case BANISHED: {
			playsound("./sound/banished.wav");
			break;
		}
		case TOKEN: {
			playsound("./sound/token.wav");
			break;
		}
		case ATTACK: {
			playsound("./sound/attack.wav");
			break;
		}
		case DIRECT_ATTACK: {
			playsound("./sound/directattack.wav");
			break;
		}
		case DRAW: {
			playsound("./sound/draw.wav");
			break;
		}
		case SHUFFLE: {
			playsound("./sound/shuffle.wav");
			break;
		}
		case DAMAGE: {
			playsound("./sound/damage.wav");
			break;
		}
		case RECOVER: {
			playsound("./sound/gainlp.wav");
			break;
		}
		case COUNTER_ADD: {
			playsound("./sound/addcounter.wav");
			break;
		}
		case COUNTER_REMOVE: {
			playsound("./sound/removecounter.wav");
			break;
		}
		case COIN: {
			playsound("./sound/coinflip.wav");
			break;
		}
		case DICE: {
			playsound("./sound/diceroll.wav");
			break;
		}
		case NEXT_TURN: {
			playsound("./sound/nextturn.wav");
			break;
		}
		case PHASE: {
			playsound("./sound/phase.wav");
			break;
		}
		case PLAYER_ENTER: {
			playsound("./sound/playerenter.wav");
			break;
		}
		case CHAT: {
			playsound("./sound/chatmessage.wav");
			break;
		}
		default:
			break;
	}
	soundEngine->setSoundVolume(mainGame->gameConf.volume);
#endif
}
void SoundManager::PlayMusic(const std::string& song, bool loop) {
#ifdef YGOPRO_USE_IRRKLANG
	if(!mainGame->chkEnableMusic->isChecked())
		return;
	if(!soundEngine->isCurrentlyPlaying(song.c_str())) {
// 		engineMusic->stopAllSounds();
		if(soundBGM) {
			soundBGM->drop();
			soundBGM = nullptr;
		}
		soundBGM = soundEngine->play2D(song.c_str(), loop, false, true);
		soundEngine->setSoundVolume(mainGame->gameConf.volume);
	}
#endif
}
void SoundManager::PlayBGM(int scene) {
#ifdef YGOPRO_USE_IRRKLANG
	int count = BGMList[scene].size();
	if(mainGame->chkEnableMusic->isChecked() && (scene != bgm_scene || (soundBGM && soundBGM->isFinished()) || !soundBGM) && count > 0) {
		bgm_scene = scene;
		int bgm = (std::uniform_int_distribution<>(0, count - 1))(rnd);
		std::string BGMName = "./sound/BGM/" + BGMList[scene][bgm];
		PlayMusic(BGMName, true);
	}
#endif
}
void SoundManager::StopBGM() {
#ifdef YGOPRO_USE_IRRKLANG
	soundEngine->stopAllSounds();
#endif
}
bool SoundManager::PlayChant(unsigned int code) {
#ifdef YGOPRO_USE_IRRKLANG
	if(ChantsList.count(code)) {
		if(!soundEngine->isCurrentlyPlaying(("./sound/chants/" + ChantsList[code]).c_str()))
			soundEngine->play2D(("./sound/chants/" + ChantsList[code]).c_str());
		return true;
	}
#endif
	return false;
}
void SoundManager::SetSoundVolume(double volume) {
#ifdef YGOPRO_USE_IRRKLANG
	soundEngine->setSoundVolume(volume);
#endif
}
void SoundManager::SetMusicVolume(double volume) {
#ifdef YGOPRO_USE_IRRKLANG
	soundEngine->setSoundVolume(volume);
#endif
}
}
