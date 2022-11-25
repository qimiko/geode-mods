#include <Geode/Geode.hpp>
#include <Geode/Modify.hpp>

#include "mac-audio.hpp"

USE_GEODE_NAMESPACE();

namespace {
	int* getMusicOffsetPtr(FMODAudioEngine* engine) {
		// right now offset in the geode bindings is off
		// don't know what's up with that...
		return reinterpret_cast<int*>(reinterpret_cast<uintptr_t>(engine) + 0x188);
	}
}

class $modify(GameSoundManager) {
	void playBackgroundMusic(gd::string filename, bool p1, bool p2) {
		auto latency = getOutputLatencyMs();

		auto audioEngine = FMODAudioEngine::sharedEngine();

		auto musicOffsetPtr = getMusicOffsetPtr(audioEngine);
		*musicOffsetPtr = latency;

		GameSoundManager::playBackgroundMusic(filename, p1, p2);
	}
};
