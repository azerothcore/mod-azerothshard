#include "ScriptMgr.h"
#include "InstanceScript.h"
#include "InstanceSaveMgr.h"
#include "Player.h"
#include "Map.h"
#include "WorldSession.h"
#include "Group.h"
#include "AzthUtils.h"
#include "AZTH.h"
#include "ChallengeModeMgr.h"

class AzthUtils;

class ChallengeModeWorld : public WorldScript
{
public:
    ChallengeModeWorld() : WorldScript("ChallengeModeWorld") { }

    void OnAfterConfigLoad(bool reload) override
    {
        sChallengeMode->LoadConfig(reload);
    }
};
class ChallengeModeMisc : public MiscScript
{
public:
    ChallengeModeMisc() : MiscScript("ChallengeModeMisc") { }

    void OnAfterLootTemplateProcess(Loot* /* loot */, LootTemplate const* /* tab */, LootStore const& /* store */, Player* /* lootOwner */, bool /* personal */, bool /* noEmptyError */, uint16 /* lootMode */) override
    {
        // if (!sConfigMgr->GetBoolDefault("Azth.Multiplies.Drop.Enable", false))
        //     return;

        //Dangerous since it can drops multiple quest items
        //[AZTH-DISABLED] give another loot process if done with correct level
        // if (sAzthUtils->isEligibleForBonusByArea(lootOwner) && (&store == &LootTemplates_Gameobject || &store == &LootTemplates_Creature))
        // {
        //     sAZTH->AddAZTHLoot(loot);
        //     tab->Process(*loot, store, lootMode, lootOwner);
        // }
    }
};

class ChallengeModePlayer : public PlayerScript
{
public:
    ChallengeModePlayer() : PlayerScript("ChallengeModePlayer") {

    }

    bool OnBeforeAchiComplete(Player* player, AchievementEntry const* /* achievement */) override
    {
        return sChallengeMode->isEligibleForReward(player);
    }

    bool OnBeforeCriteriaProgress(Player* player, AchievementCriteriaEntry const* /*criteria*/) override
    {
        return sChallengeMode->isEligibleForReward(player);
    }

    void OnGiveXP(Player* player, uint32& amount, Unit* /* victim */) override
    {
        if (!sChallengeMode->isEligibleForReward(player)) {
            amount = 0;
        }
    }
};

class ChallengeModeGlobal : public GlobalScript
{
public:

    ChallengeModeGlobal() : GlobalScript("ChallengeModeGlobal") {
    }

    bool OnBeforeLootEqualChanced(Player const* player, LootStoreItemList EqualChanced, Loot& loot, LootStore const& store) override
    {
        if (!sChallengeMode->isEligibleForReward(player)) {
            return false;
        }

        return true
    }

    bool OnItemRoll(Player const* player, LootStoreItem const */* item */, float &chance, Loot &/* loot */, LootStore const& /* store */) override
    {
        if (!sChallengeMode->isEligibleForReward(player)) {
            chance = 0;
            return false;
        }

        // [AZTH-DISABLED]
        // this check assume that sAzthUtils->isEligibleForBonusByArea(player) has been already checked
        // if (!sAZTH->GetAZTHLoot(&loot))
        //     return;

        // if ((loot.quest_items.size() + loot.items.size()) >= MAX_NR_LOOT_ITEMS)
        // {
        //     chance = 0;
        //     return;
        // }

        // if (chance >= 100.0f || chance <= 0)
        //     return;

        // if (!player->GetMap()->IsDungeon() && !player->GetMap()->IsRaid())
        //     return;

        // if (item->mincount >= 0)
        // {
        //     ItemTemplate const* i = sObjectMgr->GetItemTemplate(item->itemid);

        //     if (i && i->Quality < ITEM_QUALITY_RARE)
        //         return;
        // }

        // if (&store != &LootTemplates_Gameobject && &store != &LootTemplates_Creature)
        //     return;

        // if (chance < 20.f)
        //     chance += 20.f;

        // sAZTH->DeleteAZTHLoot(&loot);

        return true;
    }

};

void AddSC_challengemode() {
    new ChallengeModeMisc();
    new ChallengeModeGlobal();
    new ChallengeModePlayer();
    new ChallengeModeWorld();
}
