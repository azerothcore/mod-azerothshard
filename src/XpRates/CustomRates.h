#ifndef AZTHCUSTOMRATES_H
#define AZTHCUSTOMRATES_H

#include "ScriptMgr.h"
#include "Chat.h"
#include "Language.h"

// [TODO] fix and re-enable
class CustomRates {
private:
    static float GetRateFromDB(const Player *player);

    static void SaveRateToDB(const Player *player, float rate, bool update);

public:

    static void DeleteRateFromDB(ObjectGuid guid);

    static float GetXpRateFromDB(const Player *player);

    static void SaveXpRateToDB(const Player *player, float rate);
};

#endif
