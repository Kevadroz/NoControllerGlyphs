#include <Geode/Geode.hpp>
#include <Geode/modify/MenuLayer.hpp>
#include <Geode/modify/CCSprite.hpp>
#include <Geode/modify/GameToolbox.hpp>

using namespace geode::prelude;


std::unordered_set<std::string> cGlyphs = {
	"controllerBtn_A_001.png",
	"controllerBtn_B_001.png",
	"controllerBtn_DPad_Down_001.png",
	"controllerBtn_DPad_Left_001.png",	// Reference
	"controllerBtn_DPad_Right_001.png",	// Reference
	"controllerBtn_LThumb_001.png",
	"controllerBtn_RThumb_001.png",
	"controllerBtn_Start_001.png",
	"controllerBtn_X_001.png",
	"controllerBtn_Y_001.png"
};


class $modify(CCSprite) {
	static CCSprite* createWithSpriteFrameName(char const* pszSpriteFrameName) {
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
	}
};


class $modify(MenuLayer) {

bool init() {
	if (!MenuLayer::init())
		return false;

	CCObject* object = NULL;
		CCARRAY_FOREACH(this->getChildren(), object)
		{
			if (auto textLabel = typeinfo_cast<CCLabelBMFont*>(object)) {
				if (strcmp(this->getID().c_str(), "MenuLayer") == 0) {
					auto text = textLabel->getString();
					if (strcmp(text, "Mouse") == 0 || strcmp(text, "Click") == 0 ) {
						textLabel->setVisible(false);
					}
				}
			}
		}

	return true;
}

};


// Bindings do not exist
// class $modify(GameToolbox) {

// static void addRThumbScrollButton(CCLayer* param_1) {
// 	return; // Removed
// }

// };


// Manual hook
void GameToolbox_addRThumbScrollButton() {
	return;
}

// While bindings for this function do not exist this is here
$execute {
	Mod::get()->hook(
		reinterpret_cast<void*>(geode::base::get() + 0x419c0),
		&GameToolbox_addRThumbScrollButton,
		"GameToolbox::addRThumbScrollButton",
		tulip::hook::TulipConvention::Thiscall
	);
}