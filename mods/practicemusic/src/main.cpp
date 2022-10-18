/*
 * this code was originally from the following url:
 * https://github.com/camila314/geode-mods/blob/main/qol/src/practice-music.cpp
 * ownership belongs to them
 * (copied to retargest latest geode)
 */

#include <Geode/Geode.hpp>
#include <Geode/Modify.hpp>
#include <Geode/modify/PlayLayer.hpp>

class $modify(PlayLayer) {
	void startMusic() {
		auto p = m_isPracticeMode;
		m_isPracticeMode = false; // pretend there is no practice mode
		PlayLayer::startMusic();
		m_isPracticeMode = p;
	}

	void togglePracticeMode(bool p) {
		if (!m_isPracticeMode && p) {
			// receate function without the music restart
			m_isPracticeMode = p;
			m_UILayer->toggleCheckpointsMenu(p);
			PlayLayer::startMusic();
			if (p) stopActionByTag(0x12);

		} else {
			PlayLayer::togglePracticeMode(p);
		}
	}
};

class $(PlayerObject) {
	void playDeathEffect() {
		// stop music on death, just like normal gameplay
		GameSoundManager::sharedManager()->stopBackgroundMusic();
		PlayerObject::playDeathEffect();
	}
};
