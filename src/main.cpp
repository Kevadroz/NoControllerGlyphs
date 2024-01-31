#include <Geode/Geode.hpp>
#include "Geode/cocos/robtop/keyboard_dispatcher/CCKeyboardDelegate.h"
#include <Geode/modify/CCSprite.hpp>
#include <Geode/modify/MenuLayer.hpp>
#include <Geode/modify/GameToolbox.hpp>
#include <Geode/modify/CCLayer.hpp>
#include <Geode/modify/CCKeyboardDispatcher.hpp>
#include <Geode/modify/CCTouchDispatcher.hpp>
#include <Geode/modify/CCScheduler.hpp>
#include <Geode/loader/SettingEvent.hpp>
#include <iostream>
#include <unordered_set>
#include <vector>

using namespace geode::prelude;


std::unordered_set<std::string> cGlyphs = {
	"controllerBtn_Start_001.png",
	"controllerBtn_A_001.png",
	"controllerBtn_B_001.png",
	"controllerBtn_X_001.png",
	"controllerBtn_Y_001.png",
	"controllerBtn_DPad_Down_001.png",
	"controllerBtn_DPad_Up_001.png",
	"controllerBtn_DPad_Left_001.png",	// Reference
	"controllerBtn_DPad_Right_001.png",	// Reference
	"controllerBtn_LThumb_001.png",
	"controllerBtn_RThumb_001.png"
};

std::unordered_set<std::string> excludedLayers = {
	"class KeybindsLayer"
};


bool isLoaded = false;
bool excludedLayer = false;
bool usingController = false;
bool ignoreNextMouse = false;

// Settings
bool enabled = true;
bool switchMode = true;
bool ignoreMouse = false;

// This should be in code hell.
class $modify(ControlledCCSprite, CCSprite) {

	unsigned char controllerType = 0;

	static CCSprite* createWithSpriteFrameName(char const* pszSpriteFrameName) {
		if (enabled && isLoaded && !excludedLayer && PlatformToolbox::isControllerConnected()) {

			unsigned char isArrow = 0;
			CCSprite* sprite = CCSprite::createWithSpriteFrameName(pszSpriteFrameName);

			if (strcmp("controllerBtn_DPad_Left_001.png", pszSpriteFrameName) == 0) {
				isArrow = 1;
				if (!usingController) {
					sprite->initWithSpriteFrameName("GJ_arrow_01_001.png");
				}
			} else if (strcmp("controllerBtn_DPad_Right_001.png", pszSpriteFrameName) == 0) {
				isArrow = 2;
				if (!usingController) {
					sprite->initWithSpriteFrameName("GJ_arrow_01_001.png");
					sprite->setFlipX(true);
				}
			}

			if (isArrow > 0 || cGlyphs.contains(pszSpriteFrameName)) {
				if (!usingController && isArrow == 0) {
					sprite->setVisible(false);
				}

				static_cast<ControlledCCSprite*>(sprite)->m_fields->controllerType = isArrow + 1;
			}

			return sprite;
		} else {
			return CCSprite::createWithSpriteFrameName(pszSpriteFrameName);
		}
	}

};


class $modify(GameToolbox) {

	static void addRThumbScrollButton(CCLayer* layer) {
		GameToolbox::addRThumbScrollButton(layer);

		if (enabled && !excludedLayer) {
			CCArray* array = layer->getChildren();
			auto count = array->count();

			for (int n = 1; n <= 3; n++) {
				CCObject* object = array->objectAtIndex(count - n);
				if (CCSprite* sprite = typeinfo_cast<CCSprite*>(object)) {
					if (ControlledCCSprite* controlledSprite = static_cast<ControlledCCSprite*>(sprite)) {
						sprite->setVisible(usingController);
						controlledSprite->m_fields->controllerType = 1;
					}
				}
			}
		}
	}

};


class $modify(MenuLayer) {

	bool init() {
		isLoaded = true;

		if (!MenuLayer::init())
			return false;

		if (enabled) {
			if (auto* mouseLabel = this->getChildByID("mouse-gamepad-label"))
				mouseLabel->setVisible(usingController);
			if (auto* clickLabel = this->getChildByID("click-gamepad-label"))
				clickLabel->setVisible(usingController);
		}

		return true;
	}

};

void setStateFromNode(CCNode* node, bool state, bool force) {
	if (CCSprite* sprite = typeinfo_cast<CCSprite*>(node)) {
		ControlledCCSprite* controlledSprite = static_cast<ControlledCCSprite*>(sprite);
		unsigned char glyphType = controlledSprite->m_fields->controllerType;
		switch (glyphType) {
			case 1:
				sprite->setVisible(state);
				break;
			case 2:
				if (state) {
					sprite->initWithSpriteFrameName("controllerBtn_DPad_Left_001.png");
				} else {
					sprite->initWithSpriteFrameName("GJ_arrow_01_001.png");
				}
			break;
			case 3:
				if (state) {
					sprite->initWithSpriteFrameName("controllerBtn_DPad_Right_001.png");
					sprite->setFlipX(false);
				} else {
					sprite->initWithSpriteFrameName("GJ_arrow_01_001.png");
					sprite->setFlipX(true);
				}
			break;
		}
	}
	CCObject* object = NULL;
	CCARRAY_FOREACH(node->getChildren(), object)
	{
		if (CCNode* child = typeinfo_cast<CCNode*>(object)) {
			if (force) {
				setStateFromNode(child, state, true);
			} else if (CCLayer* layer = typeinfo_cast<CCLayer*>(child)) {
				const char* layerName = typeid(*layer).name();
				if (MenuLayer* menuLayer = typeinfo_cast<MenuLayer*>(layer)) {
					if (auto* mouseLabel = menuLayer->getChildByID("mouse-gamepad-label"))
						mouseLabel->setVisible(state);
					if (auto* clickLabel = menuLayer->getChildByID("click-gamepad-label"))
						clickLabel->setVisible(state);
				}
				if (typeinfo_cast<PlayLayer*>(layer)) {
				} else if (!excludedLayers.contains(layerName)) {
					setStateFromNode(layer, state, false);
				}
			} else {
				setStateFromNode(child, state, false);
			}
		}
	}
}

class $modify(CCLayer) {

	bool isExcluded = false;
	bool tracked = false;
	int trackPos = -1;

	void onEnter() {
		if (enabled && isLoaded) {
			if (excludedLayers.contains(typeid(*this).name())) {
				excludedLayer = true;
				m_fields->isExcluded = true;
				if (PlatformToolbox::isControllerConnected())
					setStateFromNode(this, true, true);
			}
		}
		CCLayer::onEnter();
	}

	void onExit() {
		if (enabled && isLoaded) {
			if (m_fields->isExcluded)
				excludedLayer = false;
		}
		CCLayer::onExit();
	}

};

class $modify(CCKeyboardDispatcher) {

	bool dispatchKeyboardMSG(enumKeyCodes key, bool down, bool p2) {
		if (enabled && switchMode && down) {
			if (key >= enumKeyCodes::CONTROLLER_A && key <= 0x413) {
				if (!usingController) {
					usingController = true;
					setStateFromNode(CCScene::get(), true, false);
				}
				if (key == enumKeyCodes::CONTROLLER_A) {
					ignoreNextMouse = true;
				}
			} else if (usingController) {
				usingController = false;
				setStateFromNode(CCScene::get(), false, false);
			}
		}

		return CCKeyboardDispatcher::dispatchKeyboardMSG(key, down, p2);
	}

};

class $modify(CCTouchDispatcher) {

	void touchesBegan(CCSet* set, CCEvent* event) {
		if (enabled && switchMode && !ignoreMouse && usingController && !ignoreNextMouse) {
			usingController = false;
			setStateFromNode(CCScene::get(), false, false);
		}

		CCTouchDispatcher::touchesBegan(set, event);
	}

};

class $modify(CCScheduler) {

	void update(float dt) {
		ignoreNextMouse = false;

		return CCScheduler::update(dt);
	}

};

$execute {
	enabled = Mod::get()->getSettingValue<bool>("enabled");
	switchMode = Mod::get()->getSettingValue<bool>("switch-mode");
	ignoreMouse = Mod::get()->getSettingValue<bool>("ignore-mouse");
	
	listenForSettingChanges("enabled", +[](bool value) {
			enabled = value;
	});
	listenForSettingChanges("switch-mode", +[](bool value) {
			switchMode = value;
	});
	listenForSettingChanges("ignore-mouse", +[](bool value) {
			ignoreMouse = value;
	});
}
