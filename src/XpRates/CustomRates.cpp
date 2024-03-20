#include "ScriptMgr.h"
#include "Chat.h"
#include "Language.h"
#include "CustomRates.h"
#include "Player.h"

 // [TODO] fix and re-enable
float CustomRates::GetRateFromDB(const Player *player) {
    QueryResult result = CharacterDatabase.Query("SELECT `xp_rate` FROM `character_xp_rate` WHERE `guid`={}", player->GetGUID().GetCounter());

    if (result) {
        Field *fields = result->Fetch();
        return fields[0].Get<float>();
    }

    return -1;
}

void CustomRates::SaveRateToDB(const Player *player, float rate, bool update)
{
    if (update)
    {
        CharacterDatabase.Execute("UPDATE `character_xp_rate` SET `xp_rate`={} WHERE `guid`={}", rate, player->GetGUID().GetCounter());
    }
    else
    {
        CharacterDatabase.Execute("INSERT INTO `character_xp_rate` (`guid`, `xp_rate`) VALUES ({}, {})", player->GetGUID().GetCounter(), rate);
    }
}

void CustomRates::DeleteRateFromDB(ObjectGuid guid)
{
    CharacterDatabase.Execute("DELETE FROM `character_xp_rate` WHERE `guid`={}", guid.GetCounter());
}

float CustomRates::GetXpRateFromDB(const Player *player)
{
    return GetRateFromDB(player);
}

void CustomRates::SaveXpRateToDB(const Player *player, float rate)
{
    float rateFromDB = CustomRates::GetXpRateFromDB(player);
    bool update = true;

    if (rateFromDB == -1)
        update = false;

    SaveRateToDB(player, rate, update);
}
