#include "Apps.h"
#include "SpellAuras.h"
#include "AZTH.h"

void SmartStoneApps::changeExp(Player *player, const char* code) {
    if (isFloatNumber(code)) {
        float exp = atof(code);
        sAZTH->GetAZTHPlayer(player)->AzthSelfChangeXp(exp);
    } else {
        ChatHandler(player->GetSession()).PSendSysMessage("|CFF7BBEF7[Custom Rates]|r: Invalid rate specified!");
    }
}

void SmartStoneApps::maxSkill(Player *player) {
    sAZTH->GetAZTHPlayer(player)->AzthMaxPlayerSkill();   
}

void SmartStoneApps::changeRace(Player *player) {
    player->SetAtLoginFlag(AT_LOGIN_CHANGE_RACE);
    ChatHandler(player->GetSession())
            .SendSysMessage(sAzthLang->get(AZTH_LANG_SS_CHANGE_RACE, player));
}

void SmartStoneApps::changeFaction(Player *player) {
    player->SetAtLoginFlag(AT_LOGIN_CHANGE_FACTION);
    ChatHandler(player->GetSession())
            .SendSysMessage(sAzthLang->get(AZTH_LANG_SS_CHANGE_FACTION, player));
}

void SmartStoneApps::rename(Player *player) {
    player->SetAtLoginFlag(AT_LOGIN_RENAME);
    ChatHandler(player->GetSession()).SendSysMessage(sAzthLang->get(AZTH_LANG_SS_CHANGE_NAME, player));
}

void SmartStoneApps::resetAuras(Player *player) {
    ChatHandler(player->GetSession()).SendSysMessage("Warning: this app is in beta stage!");

    if (player->HasFlag(PLAYER_FLAGS, PLAYER_FLAGS_RESTING) 
        && !player->IsInCombat()) {
        player->RemoveArenaAuras();

        ChatHandler(player->GetSession()).SendSysMessage("PLEASE RELOG NOW!");
    } else {
        ChatHandler(player->GetSession()).SendSysMessage(sAzthLang->get(AZTH_LANG_RESET_AURAS_ADVICE, player));
    }
}
