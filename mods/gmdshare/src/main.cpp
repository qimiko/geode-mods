#include <Geode/Geode.hpp>
#include <Geode/Modify.hpp>
#include <Geode/modify/EditLevelLayer.hpp>
#include <Geode/modify/LevelBrowserLayer.hpp>

#include <base64.h>
#include <tinyxml2.h>
#include <fstream>

#include "file-dialog.hpp"

USE_GEODE_NAMESPACE();

namespace {
	class LevelImportCallback : public cocos2d::CCNode {
		public:
			void onLevelImported(cocos2d::CCNode*) {
				goToScene();
			}

			void goToScene() {
				if (toLevel_ == nullptr) {
					return;
				}

				EditLevelLayer::scene(toLevel_);
			}

			LevelImportCallback() : toLevel_(nullptr) {}

			CC_SYNTHESIZE(GJGameLevel*, toLevel_, ToLevel);

			CREATE_FUNC(LevelImportCallback)
		};

	std::string createLevelExportString(GJGameLevel* level)
	{
		// port of gdshare::exportLevel function at HJfod/GDShare-mod@38f00df3d1af115fb2ddca30b02d6acd12f89661/src/utils/gdshare.cpp#L150
		auto song = std::string();

		if (level->m_songID != 0)
		{
				std::stringstream song_stream;
				song_stream << "<k>k45</k><i>" << level->m_songID << "</i>";
				song = song_stream.str();
		}
		else
		{
				std::stringstream song_stream;
				song_stream << "<k>k8</k><i>" << level->m_audioTrack << "</i>";
				song = song_stream.str();
		}

		// i love stringstreams
		// and also strings in general

		// gdshare double encodes description strings, and we have to replicate this
		auto desc = base64_encode(level->m_levelDesc);

//            auto level_string = ptr_to_offset<std::string>(level, 0xF8)->c_str();
//            spdlog::get("global")->info("level_string `{}`", level_string);

		auto level_string = std::string(level->m_levelString);

		// remove null if last character is null
		if (level_string.back() == '\0') {
			level_string.pop_back();
		}

		std::stringstream data_stream;
		data_stream << "<d>";
		data_stream << "<k>kCEK</k>";
		data_stream << "<i>4</i>";
		data_stream << "<k>k2</k>";
		data_stream << "<s>" << std::string(level->m_levelName) << "</s>";
		data_stream << "<k>k3</k>";
		data_stream << "<s>" << desc << "</s>";
		data_stream << "<k>k4</k>";
		data_stream << "<s>" << level_string << "</s>";
		data_stream << song;
		data_stream << "<k>k13</k>";
		data_stream << "<t/>";
		data_stream << "<k>k21</k>";
		data_stream << "<i>2</i>";
		data_stream << "<k>k50</k>";
		data_stream << "<i>24</i>";
		data_stream << "</d>";

		return data_stream.str();
	}
}

class $modify(EditLevelLayerMod, EditLevelLayer) {
	void onExportFile(cocos2d::CCObject*) {
		auto level = this->m_level;
		if (std::string(level->m_levelString).empty())
		{
			FLAlertLayer::create(nullptr, "Empty", "You can't export an empty level.", "OK", nullptr, 300.0f)->show();
			return;
		}

		try {
			showSaveDialogCxx(std::string(level->m_levelName.c_str()) + ".gmd", [level](const std::string& path) {
					std::ofstream out(path);
					out << createLevelExportString(level);
			});
		} catch (const std::exception& e) {
			// this is what happens when your offset is off
			FLAlertLayer::create(nullptr, "oh no", "Exception found in save dialog. It's probably a bad_alloc.", "OK", nullptr, 300.0f)->show();
		}
	}

	bool init(GJGameLevel* lvl) {
		if (EditLevelLayer::init(lvl)) {
			auto share_sprite = cocos2d::CCSprite::createWithSpriteFrameName("GJ_downloadBtn_001.png");

			auto share_btn = CCMenuItemSpriteExtra::create(
							share_sprite, nullptr, this,
							static_cast<cocos2d::SEL_MenuHandler>(&EditLevelLayerMod::onExportFile));

			auto menu = cocos2d::CCMenu::createWithItem(share_btn);
			this->addChild(menu, 1);

			auto director = cocos2d::CCDirector::sharedDirector();

			auto pos_x = director->getScreenRight() - 23.0f;
			auto pos_y = director->getScreenBottom() + 23.0f;

			menu->setPosition(pos_x, pos_y);

			return true;
		}

		return false;
	}
};

class $modify(LevelBrowserLayerMod, LevelBrowserLayer) {
	void onImport(cocos2d::CCObject*) {
		showOpenDialogCxx([](const std::string& path) {
			// this was ported from the 1.9 code
			auto doc = tinyxml2::XMLDocument();
			if (doc.LoadFile(path.c_str()) != tinyxml2::XML_SUCCESS) {
				FLAlertLayer::create(
								nullptr,
								"Import Failed",
								"A valid file was not provided for import.",
								"OK", nullptr)->show();
				return;
			}

			// plist parsing, tinyxml style
			// there's no validation on this at all. enjoy

			auto level = GameLevelManager::sharedState()->createNewLevel();

			auto is_new_level = false;

			auto document = doc.FirstChildElement("d");
			for (auto elem = document->FirstChildElement(); elem != nullptr; elem = elem->NextSiblingElement()) {
				if (strcmp(elem->Value(), "k") == 0) {
					auto key = elem->GetText();

					elem = elem->NextSiblingElement();
					if (elem == nullptr) {
						break;
					}

					auto value = elem->GetText();
					if (value == nullptr) {
						continue;
					}

					// what nice parsing we have
					if (strcmp(key, "kCEK") == 0) {
						auto obj_type = atoi(value);
						if (obj_type != 4) {
								FLAlertLayer::create(
												nullptr,
												"Import Failed",
												"A valid level was not provided for import.",
												"OK", nullptr)->show();
								return;
						}
					} else if (strcmp(key, "k2") == 0) {
						level->m_levelName = value;
					} else if (strcmp(key, "k3") == 0) {
						// undo the double encode
						auto desc = base64_decode(std::string(value));
						level->m_levelDesc = desc;
					} else if (strcmp(key, "k4") == 0) {
						level->m_levelString = value;
					} else if (strcmp(key, "k8") == 0) {
						level->m_audioTrack = atoi(value);
					} else if (strcmp(key, "k45") == 0) {
						level->m_songID = atoi(value);
					}
				}
			}

			level->m_levelType = GJLevelType::Editor;
			level->retain();
			// i'm unsure if this needs to be run on main thread
			EditLevelLayer::scene(level);
		});
	}

	bool init(GJSearchObject* so) {
		if (LevelBrowserLayer::init(so)) {
			if (so->getType() == SearchType::MyLevels) {
				auto import_sprite = cocos2d::CCSprite::createWithSpriteFrameName(
					"GJ_downloadBtn_001.png");
				auto import_button = CCMenuItemSpriteExtra::create(
								import_sprite,
								nullptr,
								this,
								static_cast<cocos2d::SEL_MenuHandler>(&LevelBrowserLayerMod::onImport));

				auto button_menu = cocos2d::CCMenu::createWithItem(import_button);
/*
				if (dynamic_cast<TextArea*>(button_menu) != nullptr) {
						// tap new offsets the amount of children by 1
						button_menu = reinterpret_cast<cocos2d::CCNode*>(
										self->getChildren()->objectAtIndex(10));
				}
*/
				this->addChild(button_menu);

				auto director = cocos2d::CCDirector::sharedDirector();

				auto pos_x = director->getScreenLeft() + 25.0f;
				auto pos_y = (director->getScreenTop() / 2) - 50.0f;

				button_menu->setPosition(pos_x, pos_y);
			}

			return true;
		}

		return false;
	}
};