#include "AudioManager.h"
#include <fmod_errors.h>

void AudioManager::init() {
	FMOD_RESULT result;

	result = FMOD::System_Create(&system);

	if (result != FMOD_OK) {
		Log::error("Failed to create FMOD system");
	}

	result = system->init(2048, FMOD_INIT_NORMAL | FMOD_INIT_3D_RIGHTHANDED, 0);
	if (result != FMOD_OK) {
		Log::error("Failed to initialize FMOD system");
	}

	this->volume = 0.6f;
	this->muted = false;

	loadBackgroundSound(BGM_CLOUDS);

	loadSound(SFX_MENUBUTTON);
	loadSound(SFX_CAR_HIT);


	setListenerPosition(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, 1.0f, 0.0f));

	//loadSound(SOUND_WOOF);
	//loadSound(SOUND_STEP);
	//loadSound(SOUND_PEW);
	//loadSound(SOUND_DEATH);
	//loadSound(SOUND_RIFLE);
	//loadSound(SOUND_SHOTGUN);
	//loadSound(SOUND_FLASHBANG);
	//loadSound(SOUND_FREEZE);
	//loadSound(SOUND_EASY_GAME);
	//loadSound(SOUND_DAB);
	//loadSound(SOUND_FOG);
	//loadSound(SOUND_FLAG_CAPTURE);

	playBackgroundMusic(BGM_CLOUDS);
}


void AudioManager::playBackgroundMusic(std::string filePath)
{
	FMOD::Sound* sound = mSounds[filePath.c_str()];

	// 3rd parameter is true to pause sound on load.
	FMOD_RESULT result = system->playSound(sound, nullptr, true, &backgroundChannel);
	result = backgroundChannel->setVolume(this->volume / 5.0f);
	result = backgroundChannel->setPaused(false);
}

void AudioManager::updateBackgroundChannelVolume()
{
	backgroundChannel->setPaused(true);
	backgroundChannel->setVolume(this->volume / 5.0f);
	backgroundChannel->setPaused(false);
}

void AudioManager::loadBackgroundSound(std::string filePath)
{
	FMOD::Sound* sound;
	FMOD_RESULT result = system->createSound(filePath.c_str(), FMOD_2D | FMOD_LOOP_NORMAL, nullptr, &sound);

	if (result != FMOD_OK) {
		Log::error("Failed to load sound file, {}", filePath);
		Log::error(FMOD_ErrorString(result));
		return;
	}

	// save sound pointer to map
	mSounds[filePath] = sound;
}

void AudioManager::loadSound(std::string filePath) {
	FMOD::Sound* sound;
	FMOD_RESULT result = system->createSound(filePath.c_str(), FMOD_3D, nullptr, &sound);

	FMOD::SoundGroup* sounds;
	system->createSoundGroup("sounds", &sounds);
	sound->setSoundGroup(sounds);

	if (result != FMOD_OK) {
		Log::error("Failed to load sound file, {}", filePath);
		Log::error(FMOD_ErrorString(result));
		return;
	}

	// save sound pointer to map
	mSounds[filePath] = sound;
}

void AudioManager::playSound(std::string soundName, float soundVolume) {
	FMOD::Sound* sound = mSounds[soundName];

	FMOD::Channel* channel;

	// 3rd parameter is true to pause sound on load.
	FMOD_RESULT result = system->playSound(sound, nullptr, true, &channel);
	result = channel->setVolume(this->volume * soundVolume);
	result = channel->setPaused(false);
}

void AudioManager::playSound(std::string soundName, glm::vec3 position, float soundVolume) {
	FMOD::Sound* sound = mSounds[soundName];

	FMOD::Channel* channel;

	// 3rd parameter is true to pause sound on load.
	FMOD_RESULT result = system->playSound(sound, nullptr, true, &channel);

	// set position
	FMOD_VECTOR fmodPos = {
		position.x,
		position.y,
		position.z
	};

	channel->set3DAttributes(&fmodPos, nullptr);

	result = channel->setVolume(this->volume * soundVolume);
	result = channel->setPaused(false);
}


void AudioManager::adjustVolume(float dVolume) {
	// Change volume variable
	this->volume += dVolume;

	// Clamp
	if (this->volume > 1.0f)
		this->volume = 1.0f;
	else if (this->volume < 0.0f)
		this->volume = 0.0f;

	updateBackgroundChannelVolume();
}

void AudioManager::setVolume(float newVolume) {
	// Change volume variable
	this->volume = newVolume;

	// Clamp
	if (this->volume > 1.0f)
		this->volume = 1.0f;
	else if (this->volume < 0.0f)
		this->volume = 0.0f;

	updateBackgroundChannelVolume();
}

void AudioManager::setListenerPosition(glm::vec3 position, glm::vec3 forward, glm::vec3 up)
{
	FMOD_VECTOR fmodPos = {
		position.x,
		position.y,
		position.z
	};

	FMOD_VECTOR fmodForward = {
		forward.x,
		forward.y,
		forward.z
	};

	FMOD_VECTOR fmodUp = {
		up.x,
		up.y,
		up.z
	};

	system->set3DListenerAttributes(0, &fmodPos, nullptr, &fmodForward, &fmodUp);
}

void AudioManager::muteToggle() {
	if (muted) {
		setVolume(unmutedVolume);
		muted = false;
	} 
	else {
		unmutedVolume = this->volume;
		setVolume(0.0f);
		muted = true;

	}
}

bool AudioManager::getMuteStatus()
{
	return muted;
}