#include "sound_manager.h"
#include "utils.h"
#if defined(YGOPRO_USE_IRRKLANG)
#include "sound_irrklang.h"
#define BACKEND SoundIrrklang
#else
#include "sound_sdlmixer.h"
#define BACKEND SoundMixer
#endif
#include <android/log.h>

#define printff(...) __android_log_print(ANDROID_LOG_DEBUG, "LOG_TAG", __VA_ARGS__);
namespace ygo {
std::string wd;
SoundManager soundManager;

bool SoundManager::Init(void* payload) {
	wd = *(std::string*)payload;
	#ifdef BACKEND
	try {
		mixer = std::unique_ptr<SoundBackend>(new BACKEND());
		/*mixer->SetMusicVolume(50);
		mixer->SetSoundVolume(50);*/
	}
	catch(const std::exception & e) {
		printff("something's wrong: %s", e.what());
		return true;
	}
	rnd.seed(time(0));
	bgm_scene = -1;
	RefreshBGMList();
	RefreshChantsList();
	return true;
	#else
	return false;
	#endif // BACKEND
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
	for(auto& file : Utils::FindfolderFiles(TEXT("./sound/chants"), { TEXT("ogg"), TEXT("wav") })) {
		auto scode = Utils::GetFileName(TEXT("./sound/chants/") + file);
		unsigned int code = std::stoi(scode);
		if(code && !ChantsList.count(code))
			ChantsList[code] = Utils::ToUTF8IfNeeded(file);
	}
}
#define playsound(c) mixer->PlaySound(c);
void SoundManager::PlaySoundEffect(Sounds sound) {
	if(!mainGame->chkEnableSound->isChecked())
		return;
	switch(sound) {
		case SUMMON: {
			playsound(wd+"/sound/summon.wav");
			break;
		}
		case SPECIAL_SUMMON: {
			playsound(wd+"/sound/specialsummon.wav");
			break;
		}
		case ACTIVATE: {
			playsound(wd+"/sound/activate.wav");
			break;
		}
		case SET: {
			playsound(wd+"/sound/set.wav");
			break;
		}
		case FLIP: {
			playsound(wd+"/sound/flip.wav");
			break;
		}
		case REVEAL: {
			playsound(wd+"/sound/reveal.wav");
			break;
		}
		case EQUIP: {
			playsound(wd+"/sound/equip.wav");
			break;
		}
		case DESTROYED: {
			playsound(wd+"/sound/destroyed.wav");
			break;
		}
		case BANISHED: {
			playsound(wd+"/sound/banished.wav");
			break;
		}
		case TOKEN: {
			playsound(wd+"/sound/token.wav");
			break;
		}
		case ATTACK: {
			playsound(wd+"/sound/attack.wav");
			break;
		}
		case DIRECT_ATTACK: {
			playsound(wd+"/sound/directattack.wav");
			break;
		}
		case DRAW: {
			playsound(wd+"/sound/draw.wav");
			break;
		}
		case SHUFFLE: {
			playsound(wd+"/sound/shuffle.wav");
			break;
		}
		case DAMAGE: {
			playsound(wd+"/sound/damage.wav");
			break;
		}
		case RECOVER: {
			playsound(wd+"/sound/gainlp.wav");
			break;
		}
		case COUNTER_ADD: {
			playsound(wd+"/sound/addcounter.wav");
			break;
		}
		case COUNTER_REMOVE: {
			playsound(wd+"/sound/removecounter.wav");
			break;
		}
		case COIN: {
			playsound(wd+"/sound/coinflip.wav");
			break;
		}
		case DICE: {
			playsound(wd+"/sound/diceroll.wav");
			break;
		}
		case NEXT_TURN: {
			playsound(wd+"/sound/nextturn.wav");
			break;
		}
		case PHASE: {
			playsound(wd+"/sound/phase.wav");
			break;
		}
		case PLAYER_ENTER: {
			playsound(wd+"/sound/playerenter.wav");
			break;
		}
		case CHAT: {
			playsound(wd+"/sound/chatmessage.wav");
			break;
		}
		default:
			break;
	}
}
void SoundManager::PlayBGM(int scene) {
#ifdef BACKEND
	auto& list = BGMList[scene];
	int count = list.size();
	if((scene != bgm_scene || !mixer->MusicPlaying()) && count > 0) {
		bgm_scene = scene;
		int bgm = (std::uniform_int_distribution<>(0, count - 1))(rnd);
		std::string BGMName = wd+"/sound/BGM/" + BGMList[scene][bgm];
		mixer->PlayMusic(BGMName, true);
	}
#endif
}
void SoundManager::StopBGM() {
#ifdef YGOPRO_USE_IRRKLANG
	soundEngine->stopAllSounds();
#endif
}
bool SoundManager::PlayChant(unsigned int code) {
	#ifdef BACKEND
	if(ChantsList.count(code)) {
		mixer->PlaySound(wd+"/sound/chants/" + ChantsList[code]);
		return true;
	}
	#endif
	return false;
}
void SoundManager::SetSoundVolume(double volume) {
	if(!mixer)
		return;
	mixer->SetSoundVolume(volume);
}
void SoundManager::SetMusicVolume(double volume) {
	#ifdef BACKEND
	if(!mixer)
		return;
	mixer->SetMusicVolume(volume);
	#endif
}
}
