#include "ArenaSeason.h"
#include "AzthUtils.h"
#include "AZTH.h"
#include "Solo3v3.h"

Season::Season()
{
    itemLevel = uint32(0);
    startingDate = time_t(0);
    endDate = time_t(0);
}

Season::Season(uint32 ItsItemLevel, time_t ItsStartingDate, time_t ItsEndDate)
{
    itemLevel = ItsItemLevel;
    startingDate = ItsStartingDate;
    endDate = ItsEndDate;
}

uint32 Season::GetItemLevel() const
{
    return itemLevel;
}


void Season::SetItemLevel(uint32 ItsItemLevel)
{
    itemLevel = ItsItemLevel;
}

time_t Season::GetStartingDate() const
{
    return startingDate;
}


void Season::SetStartingDate(time_t ItsStartingDate)
{
    startingDate = ItsStartingDate;
}

time_t Season::GetEndDate() const
{
    return endDate;
}

void Season::SetEndDate(time_t ItsEndDate)
{
    endDate = ItsEndDate;
}

bool Season::IsEnabled() const
{
    return enabled;
}

void Season::SetEnabled(bool enable)
{
    this->enabled = enable;
}

//
// Passing player argument will check the player state and automatically shows a message
// 
bool Season::checkItem(ItemTemplate const* proto, Player const* player)
{
    if (!IsEnabled())
        return true; //SYSTEM DISABLED  
    
    if (/*player->InBattleground() ||*/ player->InArena() || /*player->InBattlegroundQueue()*/
        player->InBattlegroundQueueForBattlegroundQueueType(BATTLEGROUND_QUEUE_2v2) ||
                 player->InBattlegroundQueueForBattlegroundQueueType(BATTLEGROUND_QUEUE_3v3) ||
                 player->InBattlegroundQueueForBattlegroundQueueType(BATTLEGROUND_QUEUE_5v5) ||
                 player->InBattlegroundQueueForBattlegroundQueueType((BattlegroundQueueTypeId)BATTLEGROUND_QUEUE_3v3_SOLO) ||
                 player->InBattlegroundQueueForBattlegroundQueueType((BattlegroundQueueTypeId)BATTLEGROUND_QUEUE_1v1)
    )
    {
        if (!sAzthUtils->checkItemLvL(proto, GetItemLevel()))
        {
            ChatHandler(player->GetSession()).SendSysMessage(sAzthLang->getf(AZTH_LANG_TOURNAMENT_LEVEL_TOOHIGH, player, proto->ItemId, proto->Name1.c_str()));
            return false;
        }
    }
    
    return true;
}

bool Season::canJoinArenaOrBg(Player *pl)
{
    if (!IsEnabled())
        return true; // SYSTEM DISABLED
    
    if (sAZTH->GetAZTHPlayer(pl)->checkItems(GetItemLevel()))
        return true;
            
    ChatHandler(pl->GetSession()).SendSysMessage(sAzthLang->getf(AZTH_LANG_TOURNAMENT_LEVEL_ACTUAL, pl, sASeasonMgr->GetItemLevel()));
    return false;
}
