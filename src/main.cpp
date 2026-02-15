#include <Geode/Geode.hpp>
#include <Geode/modify/PauseLayer.hpp>
#include <Geode/modify/PlayLayer.hpp>
#include <Geode/modify/OptionsLayer.hpp>

using namespace geode::prelude;

class $modify(MyPauseLayer, PauseLayer) {
	struct Fields {
		bool mod_on = Mod::get()->getSettingValue<bool>("mod-on");
	};

	void removeNode(CCNode* node) {
		if (node) node->removeFromParentAndCleanup(true);
	}

	void updateSkullButtonState() {
		if (Mod::get()->getSettingValue<bool>("disable-dt") && Mod::get()->getSettingValue<bool>("mod-on")) {
			auto menu = typeinfo_cast<CCMenu*>(this->getChildByID("left-button-menu"));
			if (!menu) return;
			if (auto skull = typeinfo_cast<CCMenuItem*>(menu->getChildByID("dt-skull-button"))) {
				bool locked = Mod::get()->getSettingValue<bool>("mod-on");
				skull->setEnabled(!locked);
			}
		}
	}

	void showTooltip(const char* text) {
		CCPoint anchorPos;
		bool haveAnchor = false;
		if (auto menu = typeinfo_cast<CCMenu*>(this->getChildByID("left-button-menu"))) {
			if (auto btn = typeinfo_cast<CCNode*>(menu->getChildByID("locked-in-btn"_spr))) {
				auto world = btn->getParent()->convertToWorldSpace(btn->getPosition());
				anchorPos = this->convertToNodeSpace(world);
				haveAnchor = true;
			}
		}

		auto label = CCLabelBMFont::create(text, "bigFont.fnt");
		if (!label) return;
		label->setScale(0.35f);
		label->setOpacity(0);
		label->setAnchorPoint({0.4f, 1.f});

		if (haveAnchor) {
			label->setPosition(anchorPos + ccp(0, 28));
		} else {
			auto sz = this->getContentSize();
			label->setPosition({sz.width / 2.f, sz.height / 2.f});
		}

		this->addChild(label, 1000);

		auto appear = CCFadeIn::create(0.12f);
		auto moveUp = CCMoveBy::create(0.7f, ccp(0, 14));
		auto fadeOut = CCFadeOut::create(0.7f);
		auto spawn = CCSpawn::create(moveUp, fadeOut, nullptr);
		auto cleanup = CCCallFuncN::create(this, callfuncN_selector(MyPauseLayer::removeNode));
		label->runAction(CCSequence::create(appear, spawn, cleanup, nullptr));
	}

	void customSetup() {
		PauseLayer::customSetup();
		auto myButton = CCMenuItemSpriteExtra::create(
			CCSprite::createWithSpriteFrameName("warpLockOnBtn_001.png"),
			this,
			menu_selector(MyPauseLayer::onMyButton)
		);
		myButton->setScale(0.6f);
		auto menu = this->getChildByID("left-button-menu");
		menu->addChild(myButton);

		myButton->setID("locked-in-btn"_spr);

		menu->updateLayout();

	updateSkullButtonState();
	}

	void onMyButton(CCObject* sender) {
		if (Mod::get()->getSettingValue<bool>("mod-on")) {
			if (Mod::get()->getSettingValue<bool>("confirm-unlock")) {
				createQuickPopup(
					"Unlock?",
					"Turn off Lock In?",
					"No",
					"Yes",
					[this](auto, bool btn2) {
						if (btn2) {
							Mod::get()->setSettingValue<bool>("mod-on", false);
							showTooltip("Switched off");
						}
					}
				);

			} else {
				Mod::get()->setSettingValue<bool>("mod-on", false);
				showTooltip("Switched off");
				updateSkullButtonState();
			}
		} else {
			Mod::get()->setSettingValue<bool>("mod-on", true);
			showTooltip("Switched on");
			updateSkullButtonState();
		}
	}

	void onQuit(CCObject* sender) {
		if (Mod::get()->getSettingValue<bool>("mod-on")) {
			return;
		} else {
			PauseLayer::onQuit(sender);
		}
	}

	void onRestartFull(CCObject* sender) {
		if (Mod::get()->getSettingValue<bool>("mod-on")) {
			return;
		} else {
			PauseLayer::onRestartFull(sender);
		}
	}

	void onRestart(CCObject* sender) {
		if (Mod::get()->getSettingValue<bool>("mod-on") && Mod::get()->getSettingValue<bool>("disable-restart")) {
			return;
		} else {
			PauseLayer::onRestart(sender);
		}
	}

	void onPracticeMode(CCObject* sender) {
		if (Mod::get()->getSettingValue<bool>("mod-on")) {
			return;
		} else {
			PauseLayer::onPracticeMode(sender);
		}
	}
};

class $modify(MyPlayLayer, PlayLayer) {
	bool init(GJGameLevel* level, bool useReplay, bool dontCreateObjects) {
		if (!PlayLayer::init(level, useReplay, dontCreateObjects)) return false;
		if (!Mod::get()->getSettingValue<bool>("mod-on") && Mod::get()->getSettingValue<bool>("auto-enable")) {
			Mod::get()->setSettingValue<bool>("mod-on", true);
		}
		return true;
	}
	void resetLevel() {
		if (Mod::get()->getSettingValue<bool>("mod-on") && Mod::get()->getSettingValue<bool>("disable-restart")) {
			return;
		} else {
			PlayLayer::resetLevel();
		}
	}

	void fullReset() {
		if (Mod::get()->getSettingValue<bool>("mod-on")) {
			return;
		} else {
			PlayLayer::fullReset();
		}
	}


};

class $modify(MyOptionsLayer, OptionsLayer) {
	void onOptions(CCObject* sender) {
		if (Mod::get()->getSettingValue<bool>("mod-on") && Mod::get()->getSettingValue<bool>("options-disabled")) {
			return;
		} else {
			OptionsLayer::onOptions(sender);
		}
	}
};