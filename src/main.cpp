#include <Geode/Geode.hpp>
#include <Geode/modify/CCSprite.hpp>
#include <Geode/modify/MenuLayer.hpp>
#include <Geode/modify/GameToolbox.hpp>

using namespace geode::prelude;


std::unordered_set<std::string> cGlyphs = {
	"controllerBtn_Start_001.png",
	"controllerBtn_A_001.png",
	"controllerBtn_B_001.png",
	"controllerBtn_X_001.png",
	"controllerBtn_Y_001.png",
	"controllerBtn_DPad_Down_001.png",
	"controllerBtn_DPad_Left_001.png",	// Reference
	"controllerBtn_DPad_Right_001.png",	// Reference
	"controllerBtn_LThumb_001.png",
	"controllerBtn_RThumb_001.png"
};


bool isLoaded = false;

// This should be in code hell.
class $modify(CCSprite) {

	static CCSprite* createWithSpriteFrameName(char const* pszSpriteFrameName) {
		if (isLoaded) {

			CCSprite* sprite;
			if (strcmp("controllerBtn_DPad_Left_001.png", pszSpriteFrameName) == 0)
				sprite = CCSprite::createWithSpriteFrameName("GJ_arrow_01_001.png");
			else if (strcmp("controllerBtn_DPad_Right_001.png", pszSpriteFrameName) == 0) {
				sprite = CCSprite::createWithSpriteFrameName("GJ_arrow_01_001.png");
				sprite->setFlipX(true);
			} else {
				sprite = CCSprite::createWithSpriteFrameName(pszSpriteFrameName);

				if (cGlyphs.find(pszSpriteFrameName) != cGlyphs.end())
					sprite->setVisible(false);
				
			}

			return sprite;
		} else {
			return CCSprite::createWithSpriteFrameName(pszSpriteFrameName);
		}
	}

};


class $modify(GameToolbox) {

	static void addRThumbScrollButton(CCLayer* param_1) {
		return;
	}

};


class $modify(MenuLayer) {

	bool init() {
		isLoaded = true;

		if (!MenuLayer::init())
			return false;

		if (auto* mouseLabel = this->getChildByID("mouse-gamepad-label"))
			mouseLabel->setVisible(false);
		if (auto* clickLabel = this->getChildByID("click-gamepad-label"))
			clickLabel->setVisible(false);

		return true;
	}

};
