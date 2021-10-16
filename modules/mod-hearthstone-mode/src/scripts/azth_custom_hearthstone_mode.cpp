// Copyright (c) 2016 AzerothCore
// Author: Gargarensis
// Refactored by: mik1893
// This software is provided 'as-is', without any express or implied
// warranty.  In no event will the authors be held liable for any damages
// arising from the use of this software.

#include "azth_custom_hearthstone_mode.h"
#include <iostream>
#include <cstdlib> // now using C++ header
#include "AzthPlayer.h"
#include "ArenaTeamMgr.h"
#include "ObjectMgr.h"
#include "AZTH.h"
#include "Solo3v3.h"
#include "ExtraDatabase.h"

HearthstoneMode* HearthstoneMode::instance()
{
    static HearthstoneMode instance;
    return &instance;
}


// old
void HearthstoneMode::AzthSendListInventory(ObjectGuid vendorGuid, WorldSession * session, uint32 /*extendedCostStartValue*/)
{
    ;//sLog->outDebug(LOG_FILTER_NETWORKIO, "WORLD: Sent SMSG_LIST_INVENTORY");

    Creature* vendor = session->GetPlayer()->GetNPCIfCanInteractWith(vendorGuid, UNIT_NPC_FLAG_VENDOR);
    if (!vendor)
    {
        ;//sLog->outDebug(LOG_FILTER_NETWORKIO, "WORLD: SendListInventory - Unit (GUID: %u) not found or you can not interact with him.", uint32(GUID_LOPART(vendorGuid)));
        session->GetPlayer()->SendSellError(SELL_ERR_CANT_FIND_VENDOR, NULL, ObjectGuid::Empty, 0);
        return;
    }

    // remove fake death
    if (session->GetPlayer()->HasUnitState(UNIT_STATE_DIED))
        session->GetPlayer()->RemoveAurasByType(SPELL_AURA_FEIGN_DEATH);

    // Stop the npc if moving
    if (vendor->HasUnitState(UNIT_STATE_MOVING))
        vendor->StopMoving();

    VendorItemData const* items = vendor->GetVendorItems();
    if (!items)
    {
        WorldPacket data(SMSG_LIST_INVENTORY, 8 + 1 + 1);
        data << uint64(vendorGuid.GetRawValue());
        data << uint8(0);                                   // count == 0, next will be error code
        data << uint8(0);                                   // "Vendor has no inventory"
        session->SendPacket(&data);
        return;
    }

    uint8 itemCount = items->GetItemCount();
    uint8 count = 0;

    WorldPacket data(SMSG_LIST_INVENTORY, 8 + 1 + itemCount * 8 * 4);
    data << uint64(vendorGuid.GetRawValue());

    size_t countPos = data.wpos();
    data << uint8(count);

    float discountMod = session->GetPlayer()->GetReputationPriceDiscount(vendor);

    for (uint8 slot = 0; slot < itemCount; ++slot)
    {
        if (VendorItem const* item = items->GetItem(slot))
        {
            if (ItemTemplate const* itemTemplate = sObjectMgr->GetItemTemplate(item->item))
            {
                if (!(itemTemplate->AllowableClass & session->GetPlayer()->getClassMask()) && itemTemplate->Bonding == BIND_WHEN_PICKED_UP && !session->GetPlayer()->IsGameMaster())
                    continue;
                // Only display items in vendor lists for the team the
                // player is on. If GM on, display all items.
                if (!session->GetPlayer()->IsGameMaster() && ((itemTemplate->Flags2 & ITEM_FLAGS_EXTRA_HORDE_ONLY && session->GetPlayer()->GetTeamId() == TEAM_ALLIANCE) || (itemTemplate->Flags2 == ITEM_FLAGS_EXTRA_ALLIANCE_ONLY && session->GetPlayer()->GetTeamId() == TEAM_HORDE)))
                    continue;

                // Items sold out are not displayed in list
                uint32 leftInStock = !item->maxcount ? 0xFFFFFFFF : vendor->GetVendorItemCurrentCount(item);
                if (!session->GetPlayer()->IsGameMaster() && !leftInStock)
                    continue;

                ConditionList conditions = sConditionMgr->GetConditionsForNpcVendorEvent(vendor->GetEntry(), item->item);
                if (!sConditionMgr->IsObjectMeetToConditions(session->GetPlayer(), vendor, conditions))
                {
                    sLog->outError("SendListInventory: conditions not met for creature entry %u item %u", vendor->GetEntry(), item->item);
                    continue;
                }

                // the no-patch extended cost (shown in gold)
                //uint32 ExtendedToGold = item->ExtendedCost > extendedCostStartValue ? (item->ExtendedCost - extendedCostStartValue) * 10000 : 0;
                //int32 price = item->IsGoldRequired(itemTemplate) ? uint32(floor(itemTemplate->BuyPrice * discountMod)) : ExtendedToGold;

                int32 price = item->IsGoldRequired(itemTemplate) ? uint32(floor(itemTemplate->BuyPrice * discountMod)) : 0;


                data << uint32(slot + 1);       // client expects counting to start at 1
                data << uint32(item->item);
                data << uint32(itemTemplate->DisplayInfoID);
                data << int32(leftInStock);
                data << uint32(price);
                data << uint32(itemTemplate->MaxDurability);
                data << uint32(itemTemplate->BuyCount);
                data << uint32(item->ExtendedCost);

                if (++count >= MAX_VENDOR_ITEMS)
                    break;
            }
        }
    }

    if (count == 0)
    {
        data << uint8(0);
        session->SendPacket(&data);
        return;
    }

    data.put<uint8>(countPos, count);
    session->SendPacket(&data);
}

bool HearthstoneMode::isInArray(int val)
{
    int SUPPORTED_CRITERIA[SUPPORTED_CRITERIA_NUMBER] = { 0,1,8,30,31,32,33,37,38,39,52,53,56,76,113 };
    int i;
    for (i = 0; i < SUPPORTED_CRITERIA_NUMBER; i++) {
        if (SUPPORTED_CRITERIA[i] == val)
            return true;
    }
    return false;
}


int HearthstoneMode::getQuality()
{
    float c = (float)rand_chance();
    //float chance = (float)c;
    float i = CHANCES[0];
    int quality = 0;
    while (i < c)
    {
        quality = quality + 1;
        i = i + CHANCES[quality];
    }
    if (quality > 7)
        quality = QUALITY_TO_FILL_PERCENTAGE;
    return quality;
}

int HearthstoneMode::returnData0(AchievementCriteriaEntry const* criteria)
{
    int value = -1;

    switch (criteria->requiredType)
    {
    case 0:
        value = criteria->kill_creature.creatureID;
    case 1:
        value = criteria->win_bg.bgMapID;
    case 8:
        value = criteria->complete_achievement.linkedAchievement;
        break;
    case 30:
        value = criteria->bg_objective.objectiveId;
    case 31:
        value = criteria->honorable_kill_at_area.areaID;
        break;
    case 32: // win arena - no use of column 4
        break;
    case 33:
        value = criteria->play_arena.mapID;
        break;
    case 37: // win rated arena unsed column 4
        break;
    case 38:
        value = criteria->highest_team_rating.teamtype;
        break;
    case 39:
        // MISSING !!
        break;
    case 52:
        value = criteria->hk_class.classID;
        break;
    case 53:
        value = criteria->hk_race.raceID;
        break;
    case 56: // unused
        break;
    case 76: // unused
        break;
    case 113: //unused
        break;

    default:
        value = -1;

    }
    return value;
}

int HearthstoneMode::returnData1(AchievementCriteriaEntry const* criteria)
{
    int value = -1;
    switch (criteria->requiredType)
    {
    case 0:
        value = criteria->kill_creature.creatureCount;
    case 1:
        value = criteria->win_bg.winCount;
    case 8: // no column 5
        break;
    case 30:
        value = criteria->bg_objective.completeCount;
    case 31:
        value = criteria->honorable_kill_at_area.killCount;
        break;
    case 32: // win arena - no use of column 4
        break;
    case 33: // unused
        break;
    case 37: // win rated arena unsed column 4
        value = criteria->win_rated_arena.count;
        break;
    case 38: //unused
        break;
    case 39:
        // MISSING !!
        break;
    case 52:
        value = criteria->hk_class.count;
        break;
    case 53:
        value = criteria->hk_race.count;
        break;
    case 56:
        value = criteria->get_killing_blow.killCount;
        break;
    case 76:
        value = criteria->win_duel.duelCount;
        break;
    case 113:
        // MISSING !!
        break;

    default:
        value = -1;

    }
    return value;
}

/// ---------------- START OF SCRIPTS ------------------------- ///

class npc_han_al : public CreatureScript
{
public:
    npc_han_al() : CreatureScript("npc_han_al") { }

    uint32 _weeklyRandom(std::unordered_map<uint32, HearthstoneQuest> &list) {
            time_t t = time(NULL);

            uint32 index;
            uint64 seed;
            std::unordered_map<uint32, HearthstoneQuest>::iterator _tmpItr;

            int firstTuesday = 446400; // Tuesday 1970/01/06 at 04:00
            seed = (((t - firstTuesday )/60/60/24))/7;
            srand(seed);

            uint32 count= list.size();
            index=count > 0 ? rand() % count : 0;

            _tmpItr = list.begin();
            std::advance( _tmpItr, index );
            return _tmpItr->second.id;
    }

    uint32 _dailyRandom(std::unordered_map<uint32, HearthstoneQuest> &list, Player *player = NULL) {
            time_t t = time(NULL);
            tm *lt = localtime(&t);

            uint32 index;
            uint64 seed;
            std::unordered_map<uint32, HearthstoneQuest>::iterator _tmpItr;

            seed = lt->tm_mday + lt->tm_mon + 1 + lt->tm_year + 1900 + (player ? player->GetGUID() : 0);
            srand(seed);

            index=list.size() > 1 ? rand() % (list.size()) : 0;

            _tmpItr = list.begin();
            std::advance( _tmpItr, index );
            return _tmpItr->second.id;
    }

    bool OnGossipSelect(Player* player, Creature*  creature, uint32 sender, uint32 action) override
    {
        player->PlayerTalkClass->SendCloseGossip();

        if (sender != GOSSIP_SENDER_MAIN)
            return true;

        if (!action) {
            creature->Whisper(sAzthLang->get(AZTH_LANG_HS_QUEST_LIMIT_REACHED, player), LANG_UNIVERSAL, player);
            return true;
        }

        Quest const * quest = sObjectMgr->GetQuestTemplate(action);


        if (player->CanAddQuest(quest, false) && player->CanTakeQuest(quest, false))
        {
            player->PlayerTalkClass->SendQuestGiverQuestDetails(quest, creature->GetGUID(), true);
        }

        return true;
    }

    bool OnGossipHello(Player* player, Creature* creature) override
    {
        uint32 bitmask = 0;
        int gossip = 100000;

        time_t t = time(NULL);

        bool isEmpty=true;
        uint32 pveId=0, pvpId=0, weeklyClassicId=0, weeklyTBCId=0, weeklyWotlkId=0, weeklyRandomTwId=0, dailyRandomTwId=0;
        std::list<uint32> dailyTwIds, weeklyTwIds;
        std::unordered_map<uint32, HearthstoneQuest>::iterator _tmpItr;

        // PVP
        if (sHearthstoneMode->hsPvpQuests.size() > 0) {
            pvpId = _dailyRandom(sHearthstoneMode->hsPvpQuests, player);
        }

        Quest const * questPvp = sObjectMgr->GetQuestTemplate(pvpId);

        // PVE RANDOM
        if (sHearthstoneMode->hsPveQuests.size() > 0) {
            pveId = _dailyRandom(sHearthstoneMode->hsPveQuests);
        }

        Quest const * questPve = sObjectMgr->GetQuestTemplate(pveId);

        // WEEKLY CLASSIC RANDOM
        if (sHearthstoneMode->hsWeeklyClassicQuests.size() > 0) {
            weeklyClassicId = _weeklyRandom(sHearthstoneMode->hsWeeklyClassicQuests);
        }

        Quest const * questClassicWeekly = sObjectMgr->GetQuestTemplate(weeklyClassicId);

        // WEEKLY TBC RANDOM
        if (sHearthstoneMode->hsWeeklyTBCQuests.size() > 0) {
            weeklyTBCId =  _weeklyRandom(sHearthstoneMode->hsWeeklyTBCQuests);
        }

        Quest const * questTBCWeekly = sObjectMgr->GetQuestTemplate(weeklyTBCId);

        // WEEKLY WOTLK RANDOM
        if (sHearthstoneMode->hsWeeklyWotlkQuests.size() > 0) {
            weeklyWotlkId = _weeklyRandom(sHearthstoneMode->hsWeeklyWotlkQuests);
        }

        Quest const * questWotlkWeekly = sObjectMgr->GetQuestTemplate(weeklyWotlkId);

        // WEEKLY TW
        for (std::unordered_map<uint32, HearthstoneQuest>::iterator it = sHearthstoneMode->hsTwWeeklyQuests.begin(); it != sHearthstoneMode->hsTwWeeklyQuests.end(); it++ )
        {
            if (t >= it->second.startTime  &&  t <= it->second.endTime) {
                weeklyTwIds.push_back(it->second.id);
            }
        }

        // DAILY TW
        for (std::unordered_map<uint32, HearthstoneQuest>::iterator it = sHearthstoneMode->hsTwDailyQuests.begin(); it != sHearthstoneMode->hsTwDailyQuests.end(); it++ )
        {
            if (t >= it->second.startTime  &&  t <= it->second.endTime) {
                dailyTwIds.push_back(it->second.id);
            }
        }

        // DAILY RANDOM TW
        std::unordered_map<uint32, HearthstoneQuest> _dailyRandomTwList;
        for (std::unordered_map<uint32, HearthstoneQuest>::iterator it = sHearthstoneMode->hsTwDailyRandomQuests.begin(); it != sHearthstoneMode->hsTwDailyRandomQuests.end(); it++ )
        {
            if (t >= it->second.startTime  &&  t <= it->second.endTime) {
                _dailyRandomTwList[it->second.id] = it->second;
            }
        }

        if (_dailyRandomTwList.size() > 0) {
            dailyRandomTwId = _dailyRandom(_dailyRandomTwList);
        }

        Quest const * questDailyRandomTw = sObjectMgr->GetQuestTemplate(dailyRandomTwId);

        // WEEKLY RANDOM TW
        std::unordered_map<uint32, HearthstoneQuest> _weeklyRandomTwList;
        for (std::unordered_map<uint32, HearthstoneQuest>::iterator it = sHearthstoneMode->hsTwWeeklyRandomQuests.begin(); it != sHearthstoneMode->hsTwWeeklyRandomQuests.end(); it++ )
        {
            if (t >= it->second.startTime  &&  t <= it->second.endTime) {
                _weeklyRandomTwList[it->second.id] = it->second;
            }
        }

        if (_weeklyRandomTwList.size() > 0) {
            weeklyRandomTwId = _weeklyRandom(_weeklyRandomTwList);
        }

        Quest const * questWeeklyRandomTw = sObjectMgr->GetQuestTemplate(weeklyRandomTwId);

//"Pvp Quest Check"
        int PvpMaxCheck = 0;
        _tmpItr = sHearthstoneMode->hsPvpQuests.begin();
        while (_tmpItr != sHearthstoneMode->hsPvpQuests.end()  && PvpMaxCheck <= MAX_PVE_QUEST_NUMBER) // NEED PVP MAX?
        {
            if (player->GetQuestStatus(_tmpItr->second.id) != QUEST_STATUS_NONE)
            {
                PvpMaxCheck = PvpMaxCheck + 1;
            }
            _tmpItr++;
        }
        if (questPvp && player->CanAddQuest(questPvp, false) && player->CanTakeQuest(questPvp, false))
        {
            if (PvpMaxCheck >= MAX_PVP_QUEST_NUMBER)
                pvpId = 0;  // it means this player reached the limit

            isEmpty = false;
            bitmask = bitmask | BITMASK_PVP;
        }
//endcheck

//"Pve Quest Check"
        int PveMaxCheck = 0;
        _tmpItr = sHearthstoneMode->hsPveQuests.begin();
        while (_tmpItr != sHearthstoneMode->hsPveQuests.end() && PveMaxCheck <= MAX_PVE_QUEST_NUMBER)
        {
            if (player->GetQuestStatus(_tmpItr->second.id) != QUEST_STATUS_NONE)
            {
                PveMaxCheck = PveMaxCheck + 1;
            }
            _tmpItr++;
        }
        if (questPve && player->CanAddQuest(questPve, false) && player->CanTakeQuest(questPve, false))
        {
            if (PveMaxCheck >= MAX_PVE_QUEST_NUMBER)
                pveId = 0;  // it means this player reached the limit

            isEmpty = false;
            bitmask = bitmask | BITMASK_DAILY_RANDOM;
        }
//endcheck

//"Weekly Classic Quest Check"
        int WeeklyClassicMaxCheck = 0;
        _tmpItr = sHearthstoneMode->hsWeeklyClassicQuests.begin();
        while (_tmpItr != sHearthstoneMode->hsWeeklyClassicQuests.end() && WeeklyClassicMaxCheck <= MAX_WEEKLY_QUEST_NUMBER)
        {
            if (player->GetQuestStatus(_tmpItr->second.id) != QUEST_STATUS_NONE)
            {
                WeeklyClassicMaxCheck = WeeklyClassicMaxCheck + 1;
            }
            _tmpItr++;
        }
        if (questClassicWeekly && player->CanAddQuest(questClassicWeekly, false) && player->CanTakeQuest(questClassicWeekly, false))
        {
            if (WeeklyClassicMaxCheck >= MAX_WEEKLY_QUEST_NUMBER)
                weeklyClassicId = 0;  // it means this player reached the limit

            isEmpty = false;
            bitmask = bitmask | BITMASK_WEEKLY_RND1;
        }
//endcheck

//"Weekly TBC Quest Check"
        int WeeklyTBCMaxCheck = 0;
        _tmpItr = sHearthstoneMode->hsWeeklyTBCQuests.begin();
        while (_tmpItr != sHearthstoneMode->hsWeeklyTBCQuests.end() && WeeklyTBCMaxCheck <= MAX_WEEKLY_QUEST_NUMBER)
        {
            if (player->GetQuestStatus(_tmpItr->second.id) != QUEST_STATUS_NONE)
            {
                WeeklyTBCMaxCheck = WeeklyTBCMaxCheck + 1;
            }
            _tmpItr++;
        }
        if (questTBCWeekly && player->CanAddQuest(questTBCWeekly, false) && player->CanTakeQuest(questTBCWeekly, false))
        {
            if ( WeeklyTBCMaxCheck >= MAX_WEEKLY_QUEST_NUMBER)
                weeklyTBCId = 0;  // it means this player reached the limit

            isEmpty = false;
            bitmask = bitmask | BITMASK_WEEKLY_RND2;
        }
//endcheck

//"Weekly WOTLK Quest Check"
        int WeeklyWotlkMaxCheck = 0;
        _tmpItr = sHearthstoneMode->hsWeeklyWotlkQuests.begin();
        while (_tmpItr != sHearthstoneMode->hsWeeklyWotlkQuests.end() && WeeklyWotlkMaxCheck <= MAX_WEEKLY_QUEST_NUMBER)
        {
            if (player->GetQuestStatus(_tmpItr->second.id) != QUEST_STATUS_NONE)
            {
                WeeklyWotlkMaxCheck = WeeklyWotlkMaxCheck + 1;
            }
            _tmpItr++;
        }
        if (questWotlkWeekly && player->CanAddQuest(questWotlkWeekly, false) && player->CanTakeQuest(questWotlkWeekly, false))
        {
            if (WeeklyWotlkMaxCheck >= MAX_WEEKLY_QUEST_NUMBER)
                weeklyWotlkId = 0;  // it means this player reached the limit

            isEmpty = false;
            bitmask = bitmask | BITMASK_WEEKLY_RND3;
        }
//endcheck


//"TW Weekly Random Quest Check"
        int TwWeeklyRandomMaxCheck = 0;
        _tmpItr = sHearthstoneMode->hsTwWeeklyQuests.begin();
        while (_tmpItr != sHearthstoneMode->hsTwWeeklyRandomQuests.end() && TwWeeklyRandomMaxCheck <= MAX_WEEKLY_QUEST_NUMBER)
        {
            if (player->GetQuestStatus(_tmpItr->second.id) != QUEST_STATUS_NONE)
            {
                TwWeeklyRandomMaxCheck = TwWeeklyRandomMaxCheck + 1;
            }
            _tmpItr++;
        }
        if (questWeeklyRandomTw && player->CanAddQuest(questWeeklyRandomTw, false) && player->CanTakeQuest(questWeeklyRandomTw, false))
        {
            if (TwWeeklyRandomMaxCheck >= MAX_WEEKLY_QUEST_NUMBER)
                weeklyRandomTwId = 0; // it means this player reached the limit

            isEmpty = false;
            bitmask = bitmask | BITMASK_TW_WEEKLY_RANDOM;
        }
//endcheck

//"TW Daily Random Quest Check"
        int TwDailyRandMaxCheck = 0;
        _tmpItr = sHearthstoneMode->hsTwDailyRandomQuests.begin();
        while (_tmpItr != sHearthstoneMode->hsTwDailyRandomQuests.end() && TwDailyRandMaxCheck <= MAX_PVE_QUEST_NUMBER)
        {
            if (player->GetQuestStatus(_tmpItr->second.id) != QUEST_STATUS_NONE)
            {
                TwDailyRandMaxCheck = TwDailyRandMaxCheck + 1;
            }
            _tmpItr++;
        }
        if (questDailyRandomTw && player->CanAddQuest(questDailyRandomTw, false) && player->CanTakeQuest(questDailyRandomTw, false))
        {
            if (TwDailyRandMaxCheck >= MAX_WEEKLY_QUEST_NUMBER)
                dailyRandomTwId = 0;  // it means this player reached the limit

            isEmpty = false;
            bitmask = bitmask | BITMASK_TW_DAILY_RANDOM;
        }
//endcheck

        if (bitmask>0)
            AddGossipItemFor(player,GOSSIP_ICON_DOT, sAzthLang->get(AZTH_LANG_HS_QUESTS, player), GOSSIP_SENDER_MAIN, 0);

        if ((bitmask & BITMASK_PVP) == BITMASK_PVP)
        {
            if (questPvp)
                AddGossipItemFor(player,GOSSIP_ICON_BATTLE, sAzthLang->getf(AZTH_LANG_HS_PVP_QUEST, player, (questPvp->GetTitle() + (pvpId ? "" : sAzthLang->get(AZTH_LANG_HS_QUEST_LIMIT_SUFFIX, player))).c_str()), GOSSIP_SENDER_MAIN, pvpId);
        }

        if (!sAZTH->GetAZTHPlayer(player)->isPvP()) {
            if ((bitmask & BITMASK_DAILY_RANDOM) == BITMASK_DAILY_RANDOM)
            {
                if (questPve)
                    AddGossipItemFor(player,GOSSIP_ICON_TABARD, sAzthLang->getf(AZTH_LANG_HS_DAILY_QUEST, player, (questPve->GetTitle() + (pveId ? "" : sAzthLang->get(AZTH_LANG_HS_QUEST_LIMIT_SUFFIX, player))).c_str()), GOSSIP_SENDER_MAIN, pveId);
            }


            if ((bitmask & BITMASK_WEEKLY_RND1) == BITMASK_WEEKLY_RND1)
            {
                if (questClassicWeekly)
                    AddGossipItemFor(player,GOSSIP_ICON_TABARD, sAzthLang->getf(AZTH_LANG_HS_WEEKLY_QUEST, player, (questClassicWeekly->GetTitle() + (weeklyClassicId ? "" : sAzthLang->get(AZTH_LANG_HS_QUEST_LIMIT_SUFFIX, player))).c_str()), GOSSIP_SENDER_MAIN, weeklyClassicId);
            }

            if ((bitmask & BITMASK_WEEKLY_RND2) == BITMASK_WEEKLY_RND2)
            {
                if (questTBCWeekly)
                    AddGossipItemFor(player,GOSSIP_ICON_TABARD, sAzthLang->getf(AZTH_LANG_HS_WEEKLY_QUEST, player, (questTBCWeekly->GetTitle() + (weeklyTBCId ? "" : sAzthLang->get(AZTH_LANG_HS_QUEST_LIMIT_SUFFIX, player))).c_str()), GOSSIP_SENDER_MAIN, weeklyTBCId);
            }

            if ((bitmask & BITMASK_WEEKLY_RND3) == BITMASK_WEEKLY_RND3)
            {
                if (questWotlkWeekly)
                    AddGossipItemFor(player,GOSSIP_ICON_TABARD, sAzthLang->getf(AZTH_LANG_HS_WEEKLY_QUEST, player, (questWotlkWeekly->GetTitle()+ (weeklyWotlkId ? "" : sAzthLang->get(AZTH_LANG_HS_QUEST_LIMIT_SUFFIX, player) )).c_str()), GOSSIP_SENDER_MAIN, weeklyWotlkId);
            }

            if (bitmask>0)
                AddGossipItemFor(player,GOSSIP_ICON_DOT, sAzthLang->get(AZTH_LANG_HS_TW_QUESTS, player), GOSSIP_SENDER_MAIN, 0);


            //"TW Weekly Quest Check & gossip"
            for (std::list<uint32>::const_iterator it = weeklyTwIds.begin(), end = weeklyTwIds.end(); it != end; ++it) {
                int weeklyTwMaxCheck = 0;
                _tmpItr = sHearthstoneMode->hsTwWeeklyQuests.begin();
                while (_tmpItr != sHearthstoneMode->hsTwWeeklyQuests.end())
                {
                    if (player->GetQuestStatus(_tmpItr->second.id) != QUEST_STATUS_NONE)
                    {
                        weeklyTwMaxCheck = weeklyTwMaxCheck + 1;
                    }
                    _tmpItr++;
                }

                Quest const *quest = sObjectMgr->GetQuestTemplate(*it);
                if (quest && player->CanAddQuest(quest, false) && player->CanTakeQuest(quest, false) && weeklyTwMaxCheck < MAX_PVE_QUEST_NUMBER)
                {
                    if (quest) {
                        isEmpty = false;
                        AddGossipItemFor(player,GOSSIP_ICON_TABARD, sAzthLang->getf(AZTH_LANG_HS_TW_WEEKLY_QUEST, player, (quest->GetTitle() + (weeklyTwMaxCheck <= MAX_PVE_QUEST_NUMBER ? "" : sAzthLang->get(AZTH_LANG_HS_QUEST_LIMIT_SUFFIX, player) )).c_str()), GOSSIP_SENDER_MAIN, weeklyTwMaxCheck <= MAX_PVE_QUEST_NUMBER ? *it : 0);
                    }
                }
            }
//endcheck

            if ((bitmask & BITMASK_TW_WEEKLY_RANDOM) == BITMASK_TW_WEEKLY_RANDOM)
            {
                if (questWeeklyRandomTw)
                    AddGossipItemFor(player,GOSSIP_ICON_TABARD, sAzthLang->getf(AZTH_LANG_HS_TW_WEEKLY_RANDOM_QUEST, player, (questWeeklyRandomTw->GetTitle() + (weeklyRandomTwId ? "" : sAzthLang->get(AZTH_LANG_HS_QUEST_LIMIT_SUFFIX, player))).c_str()), GOSSIP_SENDER_MAIN, weeklyRandomTwId);
            }

//"TW Daily Quest Check & gossip"
            for (std::list<uint32>::const_iterator it = dailyTwIds.begin(), end = dailyTwIds.end(); it != end; ++it) {
                int dailyTwMaxCheck = 0;
                _tmpItr = sHearthstoneMode->hsTwDailyQuests.begin();
                while (_tmpItr != sHearthstoneMode->hsTwDailyQuests.end() && dailyTwMaxCheck <= MAX_PVE_QUEST_NUMBER)
                {
                    if (player->GetQuestStatus(_tmpItr->second.id) != QUEST_STATUS_NONE)
                    {
                        dailyTwMaxCheck = dailyTwMaxCheck + 1;
                    }
                    _tmpItr++;
                }

                Quest const *quest = sObjectMgr->GetQuestTemplate(*it);
                if (quest && player->CanAddQuest(quest, false) && player->CanTakeQuest(quest, false))
                {
                    if (quest) {
                        isEmpty = false;
                        AddGossipItemFor(player,GOSSIP_ICON_TABARD, sAzthLang->getf(AZTH_LANG_HS_TW_DAILY_QUEST, player, (quest->GetTitle() + (dailyTwMaxCheck <= MAX_PVE_QUEST_NUMBER ? "" :  sAzthLang->get(AZTH_LANG_HS_QUEST_LIMIT_SUFFIX, player))).c_str()), GOSSIP_SENDER_MAIN, dailyTwMaxCheck <= MAX_PVE_QUEST_NUMBER ? *it : 0);
                    }
                }
            }
//endcheck
            if ((bitmask & BITMASK_TW_DAILY_RANDOM) == BITMASK_TW_DAILY_RANDOM)
            {
                if (questDailyRandomTw)
                    AddGossipItemFor(player,GOSSIP_ICON_TABARD, sAzthLang->getf(AZTH_LANG_HS_TW_DAILY_RANDOM_QUEST, player, (questDailyRandomTw->GetTitle() + (dailyRandomTwId ? "" :  sAzthLang->get(AZTH_LANG_HS_QUEST_LIMIT_SUFFIX, player))).c_str()), GOSSIP_SENDER_MAIN, dailyRandomTwId);
            }
        }

        if (bitmask == 0)
        {
            gossip = isEmpty ? 100001 : 100002;
        }

        /*if (PveMaxCheck >= MAX_PVE_QUEST_NUMBER &&
            PvpMaxCheck >= MAX_PVP_QUEST_NUMBER &&
            WeeklyClassicMaxCheck >= MAX_WEEKLY_QUEST_NUMBER
        )
            gossip = 100003;
        */

        SendGossipMenuFor(player,gossip, creature->GetGUID());
        return true;
    }
};

#define GOSSIP_ITEM_SHOW_ACCESS     "Vorrei vedere la tua merce, per favore."

class npc_azth_vendor : public CreatureScript
{
public:
    npc_azth_vendor() : CreatureScript("npc_azth_vendor") { }

    bool OnGossipSelect(Player* player, Creature* creature, uint32 /*sender*/, uint32 action)
    {
        if (action == 50000) {
            player->PlayerTalkClass->ClearMenus();
            sHearthstoneMode->AzthSendListInventory(creature->GetGUID(), player->GetSession(), 100000);
            return true;
        }

        return false; // execute default gossipselect

    }

    bool OnGossipHello(Player* player, Creature* creature)
    {
        std::vector<HearthstoneVendor> vendors = sHearthstoneMode->hsVendors;
        int pos = 0;
        for (std::size_t i = 0; i < vendors.size(); i++)
        {
            HearthstoneVendor temp = vendors.at(i);
            if (temp.id == creature->GetEntry())
                pos = i;
        }

        if (pos == 0)
            return false;

        HearthstoneVendor vendor = vendors.at(pos);

        if (vendor.pvpVendor && !sAZTH->GetAZTHPlayer(player)->isPvP() && !player->IsGameMaster())
        {
            AddGossipItemFor(player,GOSSIP_ICON_CHAT,  "Non sei un player Full PvP! Non posso mostrarti nulla!", GOSSIP_SENDER_MAIN, 0);
            SendGossipMenuFor(player,vendor.gossipNope, creature->GetGUID());
            return true;
        }

        // if reputation id = 0 then show the gossip as normal
        if (vendor.reputationId == 0) {
            if (vendor.gossipOk > 0) {
                SendGossipMenuFor(player,vendor.gossipOk, creature->GetGUID());
            } else {
                // if not gossip set, then send the default
                return false;
            }

            return true;
        }


        if (vendor.reputationId < 0)
        {
            int16 bracket = vendor.reputationId * -1;

            ArenaTeam* at = sArenaTeamMgr->GetArenaTeamById(player->GetArenaTeamId(ArenaTeam::GetSlotByType(ARENA_TEAM_SOLO_3v3)));

            int32 rating = (bracket==ARENA_TEAM_SOLO_3v3 && at) ? at->GetStats().Rating : player->GetArenaPersonalRating(ArenaTeam::GetSlotByType(bracket));

            if (!player->IsGameMaster() && (!creature->IsVendor() || rating < vendor.repValue)) {
                std::stringstream ss;
                ss << vendor.repValue;

                std::string str="You need "+ss.str();

                str+=(bracket==ARENA_TEAM_SOLO_3v3 && at) ? " team rating in" : " personal rating in";

                switch(bracket) {
                    case 1:
                        str+=" 1v1";
                    break;
                    case 2:
                        str+=" 2v2";
                    break;
                    case 3:
                        str+=" 3v3";
                    break;
                    case 4:
                        str+=" 3v3 Solo Queue";
                    break;
                    case 5:
                        str+=" 5v5";
                    break;
                }

                AddGossipItemFor(player,GOSSIP_ICON_CHAT, str, GOSSIP_SENDER_MAIN, 0);
                SendGossipMenuFor(player,vendor.gossipNope, creature->GetGUID());
            } else {
                if (vendor.gossipOk > 0) {
                    AddGossipItemFor(player,GOSSIP_ICON_VENDOR, GOSSIP_ITEM_SHOW_ACCESS, GOSSIP_SENDER_MAIN, 50000);
                    SendGossipMenuFor(player,vendor.gossipOk, creature->GetGUID());
                } else {
                    // if not gossip set, then send the default
                    return false;
                }
            }
            return true;
        }

        // REPUTATION VENDOR
        int32 rep = player->GetReputation(vendor.reputationId);


        if (!player->IsGameMaster() && rep < vendor.repValue) {
            std::stringstream ss;
            ss << vendor.repValue;
            std::string str = "Hai bisogno di " + ss.str() + " reputazione con AzerothShard.";

            AddGossipItemFor(player,GOSSIP_ICON_CHAT, str, GOSSIP_SENDER_MAIN, 0);

            SendGossipMenuFor(player,vendor.gossipNope, creature->GetGUID());
        }
        else {
            if (vendor.gossipOk > 0 && creature->IsVendor()) {
                AddGossipItemFor(player,GOSSIP_ICON_VENDOR, GOSSIP_ITEM_SHOW_ACCESS, GOSSIP_SENDER_MAIN, 50000);
                SendGossipMenuFor(player,vendor.gossipOk, creature->GetGUID());
            } else {
                // if not gossip set, then send the default
                return false;
            }
        }

        return true;
    }
};

#define GOSSIP_ITEM_RESS_ME     "Vorrei resuscitare, per favore."

class npc_azth_resser : public CreatureScript
{
public:
    npc_azth_resser() : CreatureScript("npc_azth_resser") { }

    bool OnGossipSelect(Player* player, Creature* /*creature*/, uint32 /*sender*/, uint32 action)
    {
        if (!player) {
            return false;
        }

        if (action == 50001) {
            player->ResurrectPlayer(1.f, false);
            player->SpawnCorpseBones();
            player->SaveToDB(false, false);
        }
        player->PlayerTalkClass->ClearMenus();
        return false;
    }

    bool OnGossipHello(Player* player, Creature* creature)
    {
        AddGossipItemFor(player,GOSSIP_ICON_CHAT, GOSSIP_ITEM_RESS_ME, GOSSIP_SENDER_MAIN, 50001);
        // 83 spirit healer gossip
        SendGossipMenuFor(player,310262, creature->GetGUID());
        return true;
    }
};

class item_azth_hearthstone_loot_sack : public ItemScript
{
public:
    item_azth_hearthstone_loot_sack() : ItemScript("item_azth_hearthstone_loot_sack") {}

    bool OnUse(Player* player, Item* item, SpellCastTargets const& /*target*/)
    {
        //sHearthstoneMode->getItems();
        CharacterDatabaseTransaction trans = CharacterDatabase.BeginTransaction();n();
        int16 deliverDelay = TIME_TO_RECEIVE_MAIL;
        MailDraft* draft = new MailDraft("Sacca Hearthstone", "");
        int i = 1;
        time_t t = time(NULL);
        tm *lt = localtime(&t);
        int seed = lt->tm_mday + lt->tm_mon + 1 + lt->tm_year + 1900 + lt->tm_sec + player->GetGUID().GetRawValue() + player->GetItemCount(item->GetEntry(), true, 0);
        int loopCheck = 0;

        Item* newItem = Item::CreateItem(100017, 1, 0);

        while (i <= EVERYTHING)
        {
            //srand(seed);
            int quality = 0;
            quality = sHearthstoneMode->getQuality();
            uint32 id = 0;
            do
            {
                if (sHearthstoneMode->items[quality].size() != 0)
                {
                    id = rand() % sHearthstoneMode->items[quality].size();
                    newItem = Item::CreateItem(sHearthstoneMode->items[quality][id], 1, 0);
                }
                loopCheck++;
            } while (!sHearthstoneMode->PlayerCanUseItem(newItem, player, true) && loopCheck < MAX_RETRY_GET_ITEM);

            if (newItem)
            {
                newItem->SaveToDB(trans);
                draft->AddItem(newItem);
            }

            i = i + 1;
            seed = seed + i;
        }
        i = 1;
        loopCheck = 0;
        newItem = Item::CreateItem(100017, 1, 0);
        while (i <= ONLY_COMMON)
        {
            //srand(seed + 3);
            int quality = 1;
            if ((float)rand_chance() > 80.f)
            {
                quality = 0;
            }

            uint32 id = 0;
            do
            {
                if (sHearthstoneMode->items[quality].size() != 0)
                {
                    id = rand() % sHearthstoneMode->items[quality].size();
                    newItem = Item::CreateItem(sHearthstoneMode->items[quality][id], 1, 0);
                }
                loopCheck++;
            } while (!sHearthstoneMode->PlayerCanUseItem(newItem, player, true) && loopCheck < MAX_RETRY_GET_ITEM);
            if (newItem)
            {
                newItem->SaveToDB(trans);
                draft->AddItem(newItem);
            }

            i = i + 1;
            seed = seed + i;
        }
        i = 1;
        loopCheck = 0;
        newItem = Item::CreateItem(100017, 1, 0);
        while (i <= NOT_COMMON)
        {
            //srand(seed + 4);
            int quality = 0;
            quality = sHearthstoneMode->getQuality();
            while (quality < 3)
            {
                quality = sHearthstoneMode->getQuality();
            }
            uint32 id = 0;
            do
            {
                if (sHearthstoneMode->items[quality].size() != 0)
                {
                    id = rand() % sHearthstoneMode->items[quality].size();
                    newItem = Item::CreateItem(sHearthstoneMode->items[quality][id], 1, 0);
                }
                loopCheck++;
            } while (!sHearthstoneMode->PlayerCanUseItem(newItem, player, true) && loopCheck < MAX_RETRY_GET_ITEM);
            if (newItem)
            {
                newItem->SaveToDB(trans);
                draft->AddItem(newItem);
            }

            i = i + 1;
            seed = seed + i;
        }

        draft->SendMailTo(trans, MailReceiver(player), MailSender(player), MAIL_CHECK_MASK_RETURNED, deliverDelay);
        CharacterDatabase.CommitTransaction(trans);

        ChatHandler(player->GetSession()).SendSysMessage(sAzthLang->get(AZTH_LANG_CHECK_EMAIL, player));

        player->DestroyItem(item->GetBagSlot(), item->GetSlot(), true);
        return true;
    }
};

bool HearthstoneMode::PlayerCanUseItem(Item const* item, Player* player, bool classCheck)
{
    if (!item)
        return false;

    ItemTemplate const* proto = item->GetTemplate();

    if (proto)
    {
        if ((proto->Flags2 & ITEM_FLAGS_EXTRA_HORDE_ONLY) && player->GetTeamId(true) != TEAM_HORDE)
            return false;

        if ((proto->Flags2 & ITEM_FLAGS_EXTRA_ALLIANCE_ONLY) && player->GetTeamId(true) != TEAM_ALLIANCE)
            return false;

        if ((proto->AllowableClass & player->getClassMask()) == 0 || (proto->AllowableRace & player->getRaceMask()) == 0)
            return false;

        if (proto->RequiredSkill != 0)
        {
            if (player->GetSkillValue(proto->RequiredSkill) == 0)
                return false;
            else if (player->GetSkillValue(proto->RequiredSkill) < proto->RequiredSkillRank)
                return false;
        }

        if (proto->RequiredSpell != 0 && !player->HasSpell(proto->RequiredSpell))
            return false;

        if (player->getLevel() < proto->RequiredLevel)
            return false;

        const static uint32 item_weapon_skills[MAX_ITEM_SUBCLASS_WEAPON] =
        {
            SKILL_AXES,     SKILL_2H_AXES,  SKILL_BOWS,          SKILL_GUNS,      SKILL_MACES,
            SKILL_2H_MACES, SKILL_POLEARMS, SKILL_SWORDS,        SKILL_2H_SWORDS, 0,
            SKILL_STAVES,   0,              0,                   SKILL_FIST_WEAPONS,   0,
            SKILL_DAGGERS,  SKILL_THROWN,   SKILL_ASSASSINATION, SKILL_CROSSBOWS, SKILL_WANDS,
            SKILL_FISHING
        };

        if (classCheck && proto->Class == ITEM_CLASS_WEAPON && player->GetSkillValue(item_weapon_skills[proto->SubClass]) == 0)
            return false;

        if (classCheck && proto->Class == ITEM_CLASS_ARMOR)
        {
            uint32 type = proto->SubClass;
            uint32 pClass = player->getClass();

            switch (type)
            {
            case 1: //cloth
                if (pClass != CLASS_PRIEST && pClass != CLASS_MAGE && pClass != CLASS_WARLOCK)
                    return false;
                break;
            case 2: //leather
                if (pClass != CLASS_ROGUE && pClass != CLASS_DRUID)
                    return false;
                break;
            case 3: //mail
                if (pClass != CLASS_HUNTER && pClass != CLASS_SHAMAN)
                    return false;
                break;
            case 4: //plate
                if (pClass != CLASS_WARRIOR && pClass != CLASS_PALADIN && pClass != CLASS_DEATH_KNIGHT)
                    return false;
                break;
            default:
                return true;
            }
        }
        return true;
    }
    return false;
}


void HearthstoneMode::sendQuestCredit(Player *player, AchievementCriteriaEntry const* criteria, std::vector<uint32>& hsCheckList)
{
    uint32 entry = 0;

    if (!isInArray(criteria->requiredType))
        return;

    uint32 achievementType = criteria->requiredType;

    // iterate through the loaded achievemements available
    for (std::vector<HearthstoneAchievement>::iterator itr = hsAchievementTable.begin(); itr != hsAchievementTable.end(); itr++)
    {
        if ((*itr).type == achievementType) { // match the type
            int data=returnData0(criteria);
            if (data > 0 && (*itr).data0 == uint32(returnData0(criteria)) && criteria->timerStartEvent == 0) // match criteria
            {
                entry = (*itr).creature; // set credit
                break;
            }
        }
    }

    /*for (int i = 0; i < hsAchievementTable.size(); i++)
	{
		if (hsAchievementTable[i].type == achievementType)
			if
	}*/

    if (entry && std::find(hsCheckList.begin(), hsCheckList.end(), entry) == hsCheckList.end())
    {
        hsCheckList.push_back(entry);
        sAZTH->GetAZTHPlayer(player)->ForceKilledMonsterCredit(entry, 0); // send credit
    }
}

HearthstoneQuest* HearthstoneMode::getHeartstoneQuestInfo(uint64 id)
{
    std::unordered_map<uint32,HearthstoneQuest>::iterator itr = allQuests.find(id);
    if (itr != allQuests.end())
        return &itr->second;

    return nullptr;
};

void HearthstoneMode::loadHearthstone()
{
    uint32 count = 0;
    sHearthstoneMode->hsAchievementTable.clear();

    QueryResult hsAchiResult = ExtraDatabase.PQuery("SELECT data0, data1, creature, type FROM hearthstone_criteria_credits");

    if (hsAchiResult)
    {
        do
        {
            HearthstoneAchievement ha = {};
            ha.data0 = (*hsAchiResult)[0].GetUInt32();
            ha.data1 = (*hsAchiResult)[1].GetUInt32();
            ha.creature = (*hsAchiResult)[2].GetUInt32();
            ha.type = (*hsAchiResult)[3].GetUInt32();

            sHearthstoneMode->hsAchievementTable.push_back(ha);
            count++;
        } while (hsAchiResult->NextRow());
    }

    sLog->outString("Hearthstone Mode: loaded %u achievement definitions", count);

    int itemCount = 0;

    sHearthstoneMode->items[0].clear();
    sHearthstoneMode->items[1].clear();
    sHearthstoneMode->items[2].clear();
    sHearthstoneMode->items[3].clear();
    sHearthstoneMode->items[4].clear();
    sHearthstoneMode->items[5].clear();
    sHearthstoneMode->items[6].clear();
    sHearthstoneMode->items[7].clear();
    QueryResult result = ExtraDatabase.Query("SELECT entry FROM transmog_items WHERE entry >= 100017 LIMIT 0, 200000");
    do
    {
        Field* fields = result->Fetch();
        uint32 entry = fields[0].GetUInt32();
        uint32 quality = sObjectMgr->GetItemTemplate(entry)->Quality; //= fields[1].GetUInt32();

        sHearthstoneMode->items[quality].push_back(entry);
        itemCount++;
    } while (result->NextRow());

    sLog->outString("Hearthstone Mode: loaded %u transmog items", itemCount);

        uint32 questCount = 0;
        sHearthstoneMode->hsPveQuests.clear();
        sHearthstoneMode->hsPvpQuests.clear();
        sHearthstoneMode->hsWeeklyClassicQuests.clear();
        sHearthstoneMode->hsWeeklyTBCQuests.clear();
        sHearthstoneMode->hsWeeklyWotlkQuests.clear();
        sHearthstoneMode->hsTwWeeklyRandomQuests.clear();
        sHearthstoneMode->hsTwWeeklyQuests.clear();
        sHearthstoneMode->hsTwDailyQuests.clear();
        sHearthstoneMode->hsTwDailyRandomQuests.clear();

        QueryResult hsQuestResult = ExtraDatabase.PQuery("SELECT id, flag, specialLevel, reqDimension, groupLimit, startTime, endTime FROM hearthstone_quests");

        if (hsQuestResult)
        {
            do
            {
                HearthstoneQuest hq = {};
                hq.id = (*hsQuestResult)[0].GetUInt32();
                hq.flag = (*hsQuestResult)[1].GetUInt32();
                hq.specialLevel = (*hsQuestResult)[2].GetUInt32();
                hq.reqDimension = (*hsQuestResult)[3].GetUInt32();
                hq.groupLimit = (*hsQuestResult)[4].GetInt32();
                hq.startTime = (*hsQuestResult)[5].GetUInt32();
                hq.endTime = (*hsQuestResult)[6].GetUInt32();
                uint32 bitmask = hq.flag;

                sHearthstoneMode->allQuests[hq.id]=hq;
                // PVP
                if ((bitmask & BITMASK_PVP) == BITMASK_PVP)
                    sHearthstoneMode->hsPvpQuests[hq.id]=hq;
                // PVE
                if ((bitmask & BITMASK_DAILY_RANDOM) == BITMASK_DAILY_RANDOM)
                    sHearthstoneMode->hsPveQuests[hq.id]=hq;
                if ((bitmask & BITMASK_WEEKLY_RND1) == BITMASK_WEEKLY_RND1)
                    sHearthstoneMode->hsWeeklyClassicQuests[hq.id]=hq;
                if ((bitmask & BITMASK_WEEKLY_RND2) == BITMASK_WEEKLY_RND2)
                    sHearthstoneMode->hsWeeklyTBCQuests[hq.id]=hq;
                if ((bitmask & BITMASK_WEEKLY_RND3) == BITMASK_WEEKLY_RND3)
                    sHearthstoneMode->hsWeeklyWotlkQuests[hq.id]=hq;

                // TIMEWALKING
                if ((bitmask & BITMASK_TW_WEEKLY_RANDOM) == BITMASK_TW_WEEKLY_RANDOM)
                    sHearthstoneMode->hsTwWeeklyRandomQuests[hq.id]=hq;
                if ((bitmask & BITMASK_TW_WEEKLY) == BITMASK_TW_WEEKLY)
                    sHearthstoneMode->hsTwWeeklyQuests[hq.id]=hq;
                if ((bitmask & BITMASK_TW_DAILY) == BITMASK_TW_DAILY)
                    sHearthstoneMode->hsTwDailyQuests[hq.id]=hq;
                if ((bitmask & BITMASK_TW_DAILY_RANDOM) == BITMASK_TW_DAILY_RANDOM)
                    sHearthstoneMode->hsTwDailyRandomQuests[hq.id]=hq;

                questCount++;
            } while (hsQuestResult->NextRow());
        }

    sLog->outString("Hearthstone Mode: loaded %u quests", questCount);


    uint32 vendorCount = 0;
    sHearthstoneMode->hsVendors.clear();

    QueryResult hsVendorResult = ExtraDatabase.PQuery("SELECT vendorId, reputation, `value`, gossipSatisfied, gossipUnsatisfied, PvPVendor FROM reputation_vendor");

    if (hsVendorResult)
    {
        do
        {
            HearthstoneVendor hv = {};
            hv.id = (*hsVendorResult)[0].GetUInt32();
            hv.reputationId = (*hsVendorResult)[1].GetInt32();
            hv.repValue = (*hsVendorResult)[2].GetInt32();
            hv.gossipOk = (*hsVendorResult)[3].GetUInt32();
            hv.gossipNope = (*hsVendorResult)[4].GetUInt32();
            ((*hsVendorResult)[5].GetUInt32() == 1) ? (hv.pvpVendor = true) : (hv.pvpVendor = false); //if == 1 ->pvp vendor || else-> non pvp vendor

            sHearthstoneMode->hsVendors.push_back(hv); // push the newly created element in the list

            vendorCount++;
        } while (hsVendorResult->NextRow());
    }

    sLog->outString("Hearthstone Mode: loaded %u vendors", vendorCount);
}

void AddSC_hearthstone()
{
    new npc_han_al();
    new npc_azth_vendor();
    new item_azth_hearthstone_loot_sack();
    new npc_azth_resser();
    //new azth_hearthstone_world();
}

