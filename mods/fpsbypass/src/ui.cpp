/*
 * this code was originally from the following url:
 * https://github.com/camila314/geode-mods/blob/main/fps-bypass/src/UI.cpp
 * ownership belongs to them
 * (copied to retargest latest geode)
 */

#include <Geode/Geode.hpp>
#include <Geode/Modify.hpp>
#include <Geode/modify/MoreVideoOptionsLayer.hpp>
#include <Geode/modify/PlayLayer.hpp>

extern double GLOBAL_FPS;
extern void updateFPS();

// ac bypass
// this seems really hacky. should figure out why this exists one day
class $modify(PlayLayer) {
	void update(float dt) {
		auto lt = m_level->m_levelType;
		m_level->m_levelType = GJLevelType::Local;
		PlayLayer::update(dt);
		m_level->m_levelType = lt;
	}
};

// the UI
class $modify(MoreVideoOptionsLayerFpsMod, MoreVideoOptionsLayer) {
	CCTextInputNode* fps;

	bool init() {
		MoreVideoOptionsLayer::init();

		auto winSize = cocos2d::CCDirector::sharedDirector()->getWinSize();
		auto menu = m_buttonMenu;

		auto btn = ButtonSprite::create("Set FPS", 50, 0, 0.7, 0, "bigFont.fnt", "GJ_button_01.png", 30);
		auto mitem = CCMenuItemSpriteExtra::create(
			btn, btn, this, static_cast<cocos2d::SEL_MenuHandler>(&MoreVideoOptionsLayerFpsMod::onClose));
		mitem->setPosition({ 0, -50 });

		auto input = CCTextInputNode::create(50, 50, "FPS", 12, "bigFont.fnt");
		input->setPosition({ 0, 0 });
		input->setLabelPlaceholderColor({ 120, 170, 240 });
		input->setString(std::to_string(static_cast<int>(GLOBAL_FPS)).c_str());

		auto box = cocos2d::extension::CCScale9Sprite::create("square02b_small.png");
		box->setOpacity(100);
		box->setColor({ 0,0,0 });
		box->setContentSize({ 60, 30 });

		m_buttonMenu->addChild(mitem);
		m_buttonMenu->addChild(box);
		m_buttonMenu->addChild(input);

		this->fps = input;

		return true;
	}

	void onClose(cocos2d::CCObject* m) {
		auto input = this->fps;
		input->onClickTrackNode(false);
		MoreVideoOptionsLayer::onClose(m);

		int fpsVal = atof(input->getString());
		if (fpsVal < 1)
			return;
		if (fpsVal != GLOBAL_FPS) {
			GLOBAL_FPS = fpsVal;
			cocos2d::CCApplication::sharedApplication()->setAnimationInterval(fpsVal);
		}
	}
};
