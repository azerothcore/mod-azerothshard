#ifndef HEARTHSTONE_MODE_H
#define HEARTHSTONE_MODE_H

#include "ScriptMgr.h"
#include "ScriptedCreature.h"
#include "ScriptedGossip.h"
#include "Player.h"
#include "WorldSession.h"
#include "WorldPacket.h"
#include "Chat.h"
#include "Spell.h"
#include "Define.h"
#include "GossipDef.h"
#include "Item.h"
#include "Common.h"
#include "Opcodes.h"
#include "Log.h"
#include "ObjectMgr.h"
#include <vector>       // std::vector

struct HearthstoneAchievement
{
    uint32 data0;
    uint32 data1;
    uint32 creature;
    uint32 type;
};

struct HearthstoneQuest
{
    uint32 id;
    uint32 flag;
    uint32 specialLevel;
    uint32 reqDimension;
    int32 groupLimit;
    uint32 startTime;
    uint32 endTime;
};

struct HearthstoneVendor
{
    uint32 id;
    int32 reputationId;
    int32 repValue;
    uint32 gossipOk;
    uint32 gossipNope;
    bool pvpVendor;
};

enum bitmasksHs
{
    BITMASK_NORMAL              = 0,    //normal quests but with hs checks
    BITMASK_DAILY_RANDOM        = 1,    //hs daily random
    BITMASK_PVP                 = 2,    //hs daily random
    BITMASK_WEEKLY_RND1         = 4,    //hs weekly random
    BITMASK_TW_WEEKLY           = 8,    //hs TW weekly
    BITMASK_TW_DAILY            = 16,   //hs TW daily
    BITMASK_TW_DAILY_RANDOM     = 32,   //hs TW daily random
    BITMASK_WEEKLY_RND2         = 64,   //hs weekly random
    BITMASK_WEEKLY_RND3         = 128,  //hs wotlk weekly random
    BITMASK_TW_WEEKLY_RANDOM    = 256,  //hs tw weekly random
};

enum miscHs
{
    PVE_QUEST_NUMBER = 1,
    MAX_PVE_QUEST_NUMBER = 0,
    PVP_QUEST_NUMBER = 1,
    MAX_PVP_QUEST_NUMBER = 3,
    WEEKLY_QUEST_NUMBER = 0,
    MAX_WEEKLY_QUEST_NUMBER = 3,
    AZTH_REPUTATION_ID = 948
};

enum otherMiscHs
{
    QUALITY_TO_FILL_PERCENTAGE  = 1, // if percentage "explode", then use quality 1
    ONLY_COMMON                 = 2, // number of common item (grey, white)
    NOT_COMMON                  = 1, // number of rare items
    EVERYTHING                  = 2, // number of items of any quality
    TIME_TO_RECEIVE_MAIL        = 0,
    SUPPORTED_CRITERIA_NUMBER   = 15,
    MAX_RETRY_GET_ITEM          = 30
};

class HearthstoneMode
{
    public:
        static HearthstoneMode* instance();

        void AzthSendListInventory(ObjectGuid vendorGuid, WorldSession * session, uint32 extendedCostStartValue);
        void sendQuestCredit(Player *player, AchievementCriteriaEntry const* criteria, std::vector<uint32>& hsCheckList);
        int returnData0(AchievementCriteriaEntry const* criteria);
        int returnData1(AchievementCriteriaEntry const* criteria);
        std::vector<HearthstoneAchievement> hsAchievementTable;
        std::unordered_map<uint32, HearthstoneQuest> allQuests;
        std::unordered_map<uint32, HearthstoneQuest> hsPveQuests;
        std::unordered_map<uint32, HearthstoneQuest> hsPvpQuests;
        std::unordered_map<uint32, HearthstoneQuest> hsWeeklyClassicQuests;
        std::unordered_map<uint32, HearthstoneQuest> hsWeeklyTBCQuests;
        std::unordered_map<uint32, HearthstoneQuest> hsWeeklyWotlkQuests;

        std::unordered_map<uint32, HearthstoneQuest> hsTwWeeklyRandomQuests;
        std::unordered_map<uint32, HearthstoneQuest> hsTwWeeklyQuests;
        std::unordered_map<uint32, HearthstoneQuest> hsTwDailyQuests;
        std::unordered_map<uint32, HearthstoneQuest> hsTwDailyRandomQuests;
        int getQuality();
        std::vector<int> items[8];
        bool isInArray(int val);
        bool PlayerCanUseItem(Item const* item, Player* player, bool classCheck);
        void loadHearthstone();
        std::vector<HearthstoneVendor> hsVendors;
		HearthstoneQuest *getHeartstoneQuestInfo(uint64 id);

    private:
        float CHANCES[8] = { 10.f, 30.f, 20.f, 15.f, 5.f, 1.f, 0.5f, 1.f };
};

#define sHearthstoneMode HearthstoneMode::instance()

#endif // !HEARTHSTONE_MODE
