#include "ScriptMgr.h"
#include "Chat.h"
#include "Language.h"
#include "CustomRates.h"
#include "Player.h"
#include "AZTH.h"

class AzthXPRatePlayerScripts : public PlayerScript
{
public:
    AzthXPRatePlayerScripts() : PlayerScript("AzthXPRatePlayerScripts") { }

    void OnDelete(ObjectGuid guid, uint32 /*AccountID*/) override
    {
        CustomRates::DeleteRateFromDB(guid);
    }

    void OnLogin(Player* player) override
    {
        float rate = CustomRates::GetXpRateFromDB(player);

        // player has custom xp rate set. Load it from DB. Otherwise use default set in AzthPlayer::AzthPlayer
        if (rate != -1)
        {
            sAZTH->GetAZTHPlayer(player)->SetPlayerQuestRate(rate);

            if (sAZTH->IsCustomXPShowOnLogin())
            {
                if (!rate)
                    ChatHandler(player->GetSession()).SendSysMessage("|CFF7BBEF7[Custom Rates]|r: Your quest XP rate was set to 0. You won't gain any XP from quest completation.");
                else
                    ChatHandler(player->GetSession()).PSendSysMessage("|CFF7BBEF7[Custom Rates]|r: Your quest XP rate was set to %.2f.", rate);
            }
        }
    }
};

void AddSC_Custom_Rates()
{
    new AzthXPRatePlayerScripts();
}
