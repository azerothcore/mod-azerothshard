#include "ScriptMgr.h"
#include "Player.h"
#include "Item.h"
#include "ItemTemplate.h"
#include "Battleground.h"
#include "BattlegroundMgr.h"
#include "QueryResult.h"
#include "ArenaSeason.h"
#include "Common.h"
#include "ArenaTeam.h"
#include "Group.h"
#include "Chat.h"
#include "AzthLanguage.h"
#include "AzthLanguageStrings.h"

uint32 defaultMaxItemLevel;

class loadSeason : public WorldScript
{
public:
    loadSeason() : WorldScript("loadSeason") { }

    void OnStartup() override
    {
        QueryResult result = WorldDatabase.PQuery("SELECT * FROM season ORDER BY startingFrom;");
        QueryResult getLastDate = CharacterDatabase.PQuery("SELECT * FROM worldstates WHERE entry = 100000;");

        if (!result)
        {
            sLog->outString(">> Loaded 0 season. DB table `season` is empty.");
            sLog->outString();
            return;
        }

        Field* arena_timestamp_table = getLastDate->Fetch();

        bool enable;

        //get status of the system: enabled or disabled
        std::istringstream(arena_timestamp_table[2].GetString()) >> enable;

        sASeasonMgr->SetEnabled(enable);

        bool actualSeasonFound = false;

        //ACTUAL TIME FROM CORE
        time_t t = time(0);
        struct tm* now = localtime(&t);
        uint32 month = now->tm_mon + 1;
        uint32 day = now->tm_mday;
        uint32 seconds = (now->tm_hour * 60 * 60) + (now->tm_min * 60) + now->tm_sec;
        uint32 date = (month * 31 * 24 * 60 * 60) + (day * 24 * 60 * 60) + seconds;

        //TIME FROM DB
        time_t startingDate;
        time_t endDate;

        do
        {
            Field* season_table = result->Fetch();

            if (!season_table)
                break;

            //retrieve timestamp from db
            startingDate = time_t(season_table[1].GetUInt32());
            endDate = time_t(season_table[2].GetUInt32());

            //convert start timestamp to something that can be read
            struct tm* startingDateConverted = localtime(&startingDate);

            //assign value to every converted value from start timestamp
            uint32 dbMonthStart = startingDateConverted->tm_mon + 1;
            uint32 dbDayStart = startingDateConverted->tm_mday;
            uint32 dbSecondsStart = (startingDateConverted->tm_hour * 60 * 60) + (startingDateConverted->tm_min * 60) + startingDateConverted->tm_sec;

            uint32 dbStartDate = (dbMonthStart * 31 * 24 * 60 * 60) + (dbDayStart * 24 * 60 * 60) + dbSecondsStart;

            //convert end timestamp to something that can be read
            struct tm* EndDateConverted = localtime(&endDate);

            //assign value to every converted value from end timestamp
            uint32 dbMonthEnd = EndDateConverted->tm_mon + 1;
            uint32 dbDayEnd = EndDateConverted->tm_mday;
            uint32 dbSecondsEnd = (EndDateConverted->tm_hour * 60 * 60) + (EndDateConverted->tm_min * 60) + EndDateConverted->tm_sec;

            uint32 dbEndDate = (dbMonthEnd * 31 * 24 * 60 * 60) + (dbDayEnd * 24 * 60 * 60) + dbSecondsEnd;

            if (date > dbStartDate && date < dbEndDate)
            {
                sASeasonMgr->SetItemLevel(season_table[0].GetUInt32());
                sASeasonMgr->SetStartingDate(time_t(season_table[1].GetUInt32()));
                sASeasonMgr->SetEndDate(time_t((season_table[2].GetUInt32())));
                actualSeasonFound = true;
            }

        } while (!actualSeasonFound && result->NextRow());

        // Prevent crashes if any season found
        if (sASeasonMgr->GetItemLevel() == 0)
        {
            sLog->outString(">> No correspondent season found. Check DB table `season`.\n");
            sLog->outString();
            sASeasonMgr->SetEnabled(false);
            return;
        }

        defaultMaxItemLevel = sASeasonMgr->GetItemLevel();

        sLog->outString(">> Season script for PVP loaded, actual item level: %u\n", sASeasonMgr->GetItemLevel());
        sLog->outString();
    }
};

class checkItemLevel : public PlayerScript
{
public:
    checkItemLevel() : PlayerScript("checkItemLevel") {}

    //Check not compatible items
    void checkPlayerItems(Player* player, bool inBattleground)
    {
        if (!sASeasonMgr->canJoinArenaOrBg(player) && inBattleground)
        {
            if (!inBattleground)
            {
                /*// we hope it will never happen, since we don't allow it
                // it can cause a crash (to investigate why)
                for (uint32 qslot = 0; qslot < PLAYER_MAX_BATTLEGROUND_QUEUES; ++qslot)
                {
                    if (BattlegroundQueueTypeId q = player->GetBattlegroundQueueTypeId(qslot))
                    {
                        BattlegroundQueue& queue = sBattlegroundMgr->GetBattlegroundQueue(q);
                        queue.RemovePlayer(player->GetGUID(), false, qslot);
                        player->RemoveBattlegroundQueueId(q);
                    }
                }*/
            }
            else
            {
                player->LeaveBattleground(); //works also with arena
            }
        }
    }

    //Check if a player join (queue) battleground with not compatible items
    void OnPlayerJoinBG(Player*  /*player*/)
    {
        //checkPlayerItems(player, false);
    }

    //Check if a player join (queue) arena with not compatible items
    void OnPlayerJoinArena(Player*  /*player*/)
    {
        //checkPlayerItems(player, false);
    }

    //Check if a player is just entered in battleground/arena with not compatible items
    void OnUpdateZone(Player* player, uint32  /*newZone*/, uint32  /*newArea*/)
    {
        if (/*player->InBattleground() ||*/ player->InArena())
        {
            checkPlayerItems(player, true);
        }
    }

    //Check if a player equip not compatible item during battleground/arena
    void OnEquip(Player* player, Item*  /*item*/, uint8  /*bag*/, uint8  /*slot*/, bool  /*update*/)
    {
        if (/*player->InBattleground() ||*/ player->InArena())
        {
            checkPlayerItems(player, true);
        }
    }
};

class arenaPointsModifier : public GlobalScript
{
public:
    arenaPointsModifier() : GlobalScript("arenaPointsModifier") {}


    void OnBeforeUpdateArenaPoints(ArenaTeam* at, std::map<ObjectGuid, uint32>& ap)
    {
        if (sASeasonMgr->IsEnabled())
            for (ArenaTeam::MemberList::const_iterator itr = at->m_membersBegin(); itr != at->m_membersEnd(); ++itr)
            {
                uint32 points = ap[itr->Guid];

                if (points == 0)
                    continue;

                // double arena points on standard weeks
                ap[itr->Guid] = float(points * 2);
            }
    }
};

class setMaxItemLevel : public CommandScript
{
public:
    setMaxItemLevel() : CommandScript("setMaxItemLevel") {}

    std::vector<ChatCommand> GetCommands() const override
    {
        static std::vector<ChatCommand> commandTable =
        {
            { "itemLevel",      SEC_GAMEMASTER,     true,  &HandleSetMaxItemLevelCommand,       "" }
        };

        return commandTable;
    }

    static bool HandleSetMaxItemLevelCommand(ChatHandler* handler, char const* args)
    {
        int itemLevel = atoi(args);

        if (!handler->GetSession() || handler->GetSession()->GetPlayer())
            return true;

        if (itemLevel < -1 || args == nullptr || strcmp(args, "") == 0)
        {
            handler->SendSysMessage(sAzthLang->get(AZTH_LANG_COMMON_NOTVALIDPARAMTER, handler->GetSession()->GetPlayer()));
            return true;
        }

        if (itemLevel == -1)
        {
            sASeasonMgr->SetItemLevel(defaultMaxItemLevel);
            handler->SendSysMessage(sAzthLang->get(AZTH_LANG_BG_ITEM_LEVEL_RESET, handler->GetSession()->GetPlayer())); //solo gm
        }
        else
        {
            sASeasonMgr->SetItemLevel(itemLevel);
            handler->SendSysMessage(sAzthLang->get(AZTH_LANG_BG_ITEM_LEVEL_CHANGED, handler->GetSession()->GetPlayer())); //solo gm
        }

        return true;
    }

};

void AddSC_BGItemLevel()
{
    new checkItemLevel();
    new loadSeason();
    new arenaPointsModifier();
    new setMaxItemLevel();
}
