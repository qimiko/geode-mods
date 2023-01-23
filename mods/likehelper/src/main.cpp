#include <Geode/Geode.hpp>
#include <Geode/modify/LevelCell.hpp>
#include <Geode/modify/RateStarsLayer.hpp>
#include <Geode/modify/LikeItemLayer.hpp>

USE_GEODE_NAMESPACE();

class LevelRatingWidget : public cocos2d::CCNode,
	public RateLevelDelegate, public LikeItemDelegate {
	cocos2d::CCSprite* likeSprite_{nullptr};
	cocos2d::CCSprite* starSprite_{nullptr};
	CCMenuItemSpriteExtra* likeBtn_{nullptr};
	CCMenuItemSpriteExtra* starBtn_{nullptr};

	int levelID_{-1};

	bool hasLikedLevel() {
		return GameLevelManager::sharedState()->hasLikedItemFullCheck(
				LikeItemType::Level, levelID_, 0);
	}

	bool hasRatedLevel() {
		return GameLevelManager::sharedState()->hasRatedLevelStars(levelID_);
	}

	void updateRatingButtons() {
		if (hasLikedLevel()) {
			auto special_frame = cocos2d::CCSpriteFrameCache::sharedSpriteFrameCache()
				->spriteFrameByName("GJ_like2Btn2_001.png");

			likeSprite_->setDisplayFrame(special_frame);
			likeBtn_->setEnabled(false);
		}

		if (hasRatedLevel()) {
			auto special_frame = cocos2d::CCSpriteFrameCache::sharedSpriteFrameCache()
				->spriteFrameByName("GJ_starBtn2_001.png");

			starSprite_->setDisplayFrame(special_frame);
			starBtn_->setEnabled(false);
		}
	}

	// none of these parameters are important
	void likedItem(LikeItemType, int, bool) override {
		updateRatingButtons();
	}

	void rateLevelClosed() override {
		updateRatingButtons();
	}

	void onStars(cocos2d::CCObject*) {
		auto popup = RateStarsLayer::create(this->levelID_, false);
		popup->m_rateDelegate = this;
		popup->show();
	}

	void onLike(cocos2d::CCObject*) {
		auto popup = LikeItemLayer::create(
			LikeItemType::Level, this->levelID_, 0);
		popup->m_likeDelegate = this;
		popup->show();
	}

	bool init(int levelID, bool isRated) {
		this->levelID_ = levelID;


		auto likeSprite = cocos2d::CCSprite::createWithSpriteFrameName(
			"GJ_like2Btn_001.png");
		likeSprite->setScale(0.5f);
		this->likeSprite_ = likeSprite;

		auto likeBtn = CCMenuItemSpriteExtra::create(
			likeSprite, nullptr, this,
			static_cast<cocos2d::SEL_MenuHandler>(&LevelRatingWidget::onLike)
		);
		this->likeBtn_ = likeBtn;



		auto starSprite = cocos2d::CCSprite::createWithSpriteFrameName(
			"GJ_starBtn_001.png");
		starSprite->setScale(0.5f);
		this->starSprite_ = starSprite;

		auto starBtn = CCMenuItemSpriteExtra::create(
			starSprite, nullptr, this,
			static_cast<cocos2d::SEL_MenuHandler>(&LevelRatingWidget::onStars)
		);
		this->starBtn_ = starBtn;


		auto ratingMenu = cocos2d::CCMenu::createWithItem(likeBtn);
		this->addChild(ratingMenu);
		ratingMenu->setPosition({ 0.0f, 0.0f });

		ratingMenu->addChild(starBtn);

		likeBtn->setPosition({ 13.0f, 0.0f });
		starBtn->setPosition({ -13.0f, 0.0f });

		if (isRated) {
			starBtn->setVisible(false);
		}

		updateRatingButtons();

		return true;
	}

public:
	static LevelRatingWidget* create(int levelID, bool isRated) {
			auto ret = new LevelRatingWidget();

			if (ret && ret->init(levelID, isRated)) {
				ret->autorelease();
				return ret;
			} else {
				delete ret;
				ret = nullptr;
				return nullptr;
			}
	}
};

class $modify(LevelCellMod, LevelCell) {
	void loadCustomLevelCell() {
		LevelCell::loadCustomLevelCell();

		if (this->m_level->m_dailyID > 0) {
			return;
		}

		auto isRated = this->m_level->m_stars > 0;

		auto widget = LevelRatingWidget::create(this->m_level->m_levelID, isRated);

		this->m_mainLayer->addChild(widget, 3);
		widget->setPosition({ this->m_width - 40.0f, this->m_height - 15.0f });

		if (!isRated) {
			auto ratedString = cocos2d::CCString::createWithFormat(
					"Req: %i*", this->m_level->m_starsRequested);
			auto ratedLabel = cocos2d::CCLabelBMFont::create(
				ratedString->getCString(), "chatFont.fnt");

			ratedLabel->setAnchorPoint({ 1.0f, 0.0f });
			ratedLabel->setScale(0.75f);

			this->m_mainLayer->addChild(ratedLabel);
			ratedLabel->setPosition({ this->m_width - 10.0f, 5.0f });
		}
	}
};

class $modify(RateStarsLayer) {
	bool init(int levelID, bool modSuggest) {
		if (RateStarsLayer::init(levelID, modSuggest)) {
			auto window_size = cocos2d::CCDirector::sharedDirector()->getWinSize();

			auto ratedCount = GameStatsManager::sharedState()->getStat("11");
			auto ratedString = cocos2d::CCString::createWithFormat(
				"Rated: %i", ratedCount);

			auto ratedLabel = cocos2d::CCLabelBMFont::create(
				ratedString->getCString(), "chatFont.fnt");

      ratedLabel->setAnchorPoint({ 1.0f, 0.0f });
			ratedLabel->setColor({ 0xBB, 0xBB, 0xBB });
			ratedLabel->setScale(0.75f);

			this->m_mainLayer->addChild(ratedLabel);
			ratedLabel->setPosition({
				(window_size.width / 2.0f) + 180.0f,
				(window_size.height / 2.0f) + 70.0f
			});

			return true;
		}

		return false;
	}
};

class $modify(LikeItemLayer) {
	bool init(LikeItemType type, int itemID, int commentSourceID) {
		if (LikeItemLayer::init(type, itemID, commentSourceID)) {
			if (type == LikeItemType::Level) {
				auto window_size = cocos2d::CCDirector::sharedDirector()->getWinSize();

				auto likedCount = GameStatsManager::sharedState()->getStat("10");
				auto likedString = cocos2d::CCString::createWithFormat(
					"%i", likedCount);

				auto likedLabel = cocos2d::CCLabelBMFont::create(
					likedString->getCString(), "chatFont.fnt");

				likedLabel->setAnchorPoint({ 1.0f, 0.0f });
				likedLabel->setColor({ 0xBB, 0xBB, 0xBB });
				likedLabel->setScale(0.75f);

				this->m_mainLayer->addChild(likedLabel);
				likedLabel->setPosition({
					(window_size.width / 2.0f) + 93.0f,
					(window_size.height / 2.0f) + 40.0f
				});
			}

			return true;
		}

		return false;
	}
};