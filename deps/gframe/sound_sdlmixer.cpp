#include "sound_sdlmixer.h"
#define SDL_MAIN_HANDLED
#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>
#include <android/log.h>
#define printff(...) __android_log_print(ANDROID_LOG_DEBUG, "LOG_TAG", __VA_ARGS__);
SoundMixer::SoundMixer() {
	SDL_SetMainReady();
	if(SDL_Init(SDL_INIT_AUDIO) < 0){
		printff("Failed to init sdl audio device!");
		printff("SDL_Init: %s\n", Mix_GetError());
		throw std::runtime_error("Failed to init sdl audio device!");
	}
	int flags = MIX_INIT_OGG | MIX_INIT_MP3 | MIX_INIT_FLAC;
	int initted = Mix_Init(flags);
	printff("MIX_INIT_OGG: %s\n", initted&MIX_INIT_OGG ? "true" : "false");
	printff("MIX_INIT_MP3: %s\n", initted&MIX_INIT_MP3 ? "true" : "false");
	printff("MIX_INIT_FLAC: %s\n", initted&MIX_INIT_FLAC ? "true" : "false");
	if((initted&flags) != flags) {
		printff("Mix_Init: %s\n", Mix_GetError());
		throw std::runtime_error("Not all flags set");
	}
	if(Mix_OpenAudio(MIX_DEFAULT_FREQUENCY, MIX_DEFAULT_FORMAT, MIX_DEFAULT_CHANNELS, 4096) == -1){
		printff("Mix_OpenAudio: %s\n", Mix_GetError());
		throw std::runtime_error("Cannot open channels");
	}
}
void SoundMixer::SetSoundVolume(double volume) {
	sound_volume = (int)(volume * 128);
	Mix_Volume(-1, sound_volume);
}
void SoundMixer::SetMusicVolume(double volume) {
	music_volume = (int)(volume * 128);
	Mix_VolumeMusic(music_volume);
}
bool SoundMixer::PlayMusic(const std::string& name, bool loop) {
	if(cur_music == name)
		return false;
	if(music) {
		Mix_HaltMusic();
		Mix_FreeMusic(music);
	}
	music = Mix_LoadMUS(name.c_str());
	if(music) {
		if(Mix_PlayMusic(music, loop ? -1 : 0) == -1) {
			printff("Mix_PlayMusic: %s\n", Mix_GetError());
			Mix_FreeMusic(music);
			return false;
		} else {
			cur_music = name;
		}
	} else {
		printff("Mix_LoadMUS: %s\n", Mix_GetError());
		return false;
	}
	return true;
}
bool SoundMixer::PlaySound(const std::string& name) {
	auto chunk = Mix_LoadWAV(name.c_str());
	if(chunk) {
		auto channel = Mix_PlayChannel(-1, chunk, 0);
		if(channel == -1) {
			printff("Mix_PlayChannel: %s\n", Mix_GetError());
			Mix_FreeChunk(chunk);
			return false;
		}
		sounds[channel] = chunk;
		Mix_Volume(-1, sound_volume);
	} else {
		printff("Mix_LoadWAV: %s\n", Mix_GetError());
		return false;
	}
	return true;
}
void SoundMixer::StopMusic() {
	if(music) {
		Mix_HaltMusic();
		Mix_FreeMusic(music);
		music = nullptr;
		cur_music = "";
	}
}
bool SoundMixer::MusicPlaying() {
	return Mix_PlayingMusic();
}
void SoundMixer::Tick() {
	for(auto chunk = sounds.begin(); chunk != sounds.end();) {
		if(Mix_Playing(chunk->first) == 0) {
			Mix_FreeChunk(chunk->second);
			sounds.erase(chunk++);
		} else
			chunk++;
	}
}
SoundMixer::~SoundMixer() {
	Mix_HaltChannel(-1);
	Mix_HaltMusic();
	for(auto& chunk : sounds) {
		if(chunk.second)
			Mix_FreeChunk(chunk.second);
	}
	if(music)
		Mix_FreeMusic(music);
	Mix_CloseAudio();
}
