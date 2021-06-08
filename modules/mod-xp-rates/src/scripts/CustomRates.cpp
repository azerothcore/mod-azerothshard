#include "ScriptMgr.h"
#include "Chat.h"
#include "Language.h"
#include "CustomRates.h"
#include "Player.h"

 // [TODO] fix and re-enable
float CustomRates::GetRateFromDB(const Player *player) {
    QueryResult result = CharacterDatabase.PQuery("SELECT xp_rate FROM character_xp_rate WHERE guid = %d", player->GetGUID().GetCounter());

    if (result) {
        Field *fields = result->Fetch();
        return fields[0].GetFloat();
    }

    return -1;
}

void CustomRates::SaveRateToDB(const Player *player, float rate, bool update) {
    if (update) {
        CharacterDatabase.AsyncPQuery("UPDATE character_xp_rate SET xp_rate = %f WHERE guid = %d", rate, player->GetGUID().GetCounter());
    } else {
        CharacterDatabase.AsyncPQuery("INSERT INTO character_xp_rate (guid, xp_rate) VALUES (%d, %f)", player->GetGUID().GetCounter(), rate);
    }
}

void CustomRates::DeleteRateFromDB(ObjectGuid guid) {
    CharacterDatabase.AsyncPQuery("DELETE FROM character_xp_rate WHERE guid = %d", guid.GetCounter());
}

float CustomRates::GetXpRateFromDB(const Player *player) {
    return GetRateFromDB(player);
}

void CustomRates::SaveXpRateToDB(const Player *player, float rate) {
    float rateFromDB = CustomRates::GetXpRateFromDB(player);
    bool update = true;

    if (rateFromDB == -1)
        update = false;

    SaveRateToDB(player, rate, update);
}
