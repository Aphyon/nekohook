/*
 * AntiCheat.cpp
 *
 *  Created on: Jun 5, 2017
 *      Author: nullifiedcat
 */

#include "AntiCheat.hpp"
#include "../common.h"

#include "ac/aimbot.hpp"
#include "ac/antiaim.hpp"
#include "ac/removecond.hpp"
#include "ac/bhop.hpp"

namespace hacks { namespace shared { namespace anticheat {

static CatVar enabled(CV_SWITCH, "ac_enabled", "0", "Enable AC");

void Accuse(int eid, const std::string& hack, const std::string& details) {
	player_info_s info;
	if (g_IEngine->GetPlayerInfo(eid, &info)) {
		logging::Info("%s %s %s", info.name, hack.c_str(), details.c_str());
		//logging::Info("[AC] \x07%06X%s\x01 is suspected of using \x07%06%s\x01: %s", 0xa06ba0, info.name, 0xe05938, hack.c_str(), details.c_str());
		PrintChat("[AC] \x07%06X%s\x01 is suspected of using \x07%06X%s\x01: %s", colors::chat::team(ENTITY(eid)->m_iTeam), info.name, 0xe05938, hack.c_str(), details.c_str());
	}
}

void CreateMove() {
	if (!enabled) return;
	for (int i = 1; i < 33; i++) {
		CachedEntity* ent = ENTITY(i);
		if (CE_GOOD(ent)) {
			if ((CE_BYTE(ent, netvar.iLifeState) == 0)) {
				ac::aimbot::Update(ent);
				ac::antiaim::Update(ent);
				ac::bhop::Update(ent);
			}
		}
		ac::removecond::Update(ent);
	}
}

void ResetPlayer(int index) {
	ac::aimbot::ResetPlayer(index);
	ac::antiaim::ResetPlayer(index);
	ac::bhop::ResetPlayer(index);
	ac::removecond::ResetPlayer(index);
}

void ResetEverything() {
	ac::aimbot::ResetEverything();
	ac::antiaim::ResetEverything();
	ac::bhop::ResetEverything();
	ac::removecond::ResetEverything();
}

class ACListener : public IGameEventListener {
public:
	virtual void FireGameEvent(KeyValues* event) {
		if (!enabled) return;
		std::string name(event->GetName());
		if (name == "player_activate") {
			int uid = event->GetInt("userid");
			int entity = g_IEngine->GetPlayerForUserID(uid);
			ResetPlayer(entity);
		} else if (name == "player_disconnect") {
			int uid = event->GetInt("userid");
			int entity = g_IEngine->GetPlayerForUserID(uid);
			ResetPlayer(entity);
		}

		ac::aimbot::Event(event);
	}
};

ACListener listener;

void Init() {
	// FIXME free listener
	g_IGameEventManager->AddListener(&listener, false);
}

}}}