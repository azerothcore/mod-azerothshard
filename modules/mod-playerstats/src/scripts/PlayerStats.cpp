#include "AzthPlayer.h"
#include "AzthLevelStat.h"
#include "Group.h"
#include "Player.h"
#include "AZTH.h"
#include "AzthGroupMgr.h"

uint32 AzthPlayer::normalizeLvl(uint32 level)
{
    if (level>=TIMEWALKING_SPECIAL_LVL_WOTLK_START && level<=TIMEWALKING_SPECIAL_LVL_WOTLK_END)
        return sWorld->getIntConfig(CONFIG_MAX_PLAYER_LEVEL);

    if (!player)
        return level;

    uint32 rLevel = level;

    // to normalize level auto or vas we need to get the max level of the group or the level of player
    // however since the special level is related to a dynamic real level (that can be increased/decreased)
    // we need to get the max level in a specific time (for now, should be avoided when possible)
    if (level >= TIMEWALKING_SPECIAL_LVL_MIN)
    {
        Group *group = player->GetGroup();
        if (group)
        {
            for (Group::member_citerator mitr = group->GetMemberSlots().begin(); mitr != group->GetMemberSlots().end(); ++mitr)
            {
                if (const GlobalPlayerData* gpd = sWorld->GetGlobalPlayerData(mitr->guid.GetCounter()))
                {
                    if (groupLevel < gpd->level)
                        rLevel = gpd->level;
                }
            }
        }
        else
            rLevel = player->getLevel();
    }

    return rLevel;
}

uint32 AzthPlayer::getGroupLevel(bool normalize /*=true*/, bool checkInstance /*=true*/) {
    if (!player)
        return 0;

    uint32 groupLevel = 0;

    if (checkInstance) {
        groupLevel = getInstanceLevel(normalize);
    }


    Group *group = player->GetGroup();
    if (AzthGroupMgr *azthGroup = sAZTH->GetAZTHGroup(group); azthGroup)
    {
        if (!azthGroup)
        {
            return 0;
        }

        // outworld party or limit case for dungeon
        groupLevel = azthGroup->levelMaxGroup;

        if (normalize)
            groupLevel = normalizeLvl(groupLevel);
    }

    return groupLevel;
}

uint32 AzthPlayer::getInstanceLevel(bool normalize /*=true*/)
{
    if (!player)
        return 0;

    uint32 instanceLevel=0;

    Map *map = player->FindMap();
    if (map && (map->IsDungeon() || map->IsRaid()))
    {
        // when in instance
        InstanceSave *is = sInstanceSaveMgr->PlayerGetInstanceSave(
            player->GetGUID(), map->GetId(),player->GetDifficulty((map->IsRaid())));

        if (is && sAZTH->GetAZTHInstanceSave(is))
        {
            instanceLevel = sAZTH->GetAZTHInstanceSave(is)->levelMax;

            if (normalize)
                instanceLevel = normalizeLvl(instanceLevel);
        }
    }

    return instanceLevel;
}

// this function help to find the current level for player stats and timewalking
// in order of importance: instance -> group -> player
// but you can disable instance or group check or both in special situations
uint32 AzthPlayer::getPStatsLevel(bool normalize /*=true*/, bool checkInstance /*=true*/, bool checkGroup /*=true*/)
{
    if (!player)
        return 0;

    uint32 level = 0;

    // instance
    if (checkInstance)
        level = getInstanceLevel(normalize);

    // group
    if (checkGroup && !level)
        level = getGroupLevel(normalize);

    // player
    if (!level)
    {
        level = isTimeWalking() ? GetTimeWalkingLevel() : player->getLevel();

        if (normalize)
            level = normalizeLvl(level);
    }

    return level;
}

uint32 AzthPlayer::getGroupSize(bool checkInstance /*=true*/)
{
    if (!player)
        return 0;

    uint32 groupSize = 0;

    if (checkInstance)
        groupSize = getInstanceSize();

    if (!groupSize)
    {
        Group *group = player->GetGroup();
        AzthGroupMgr *azthGroup = sAZTH->GetAZTHGroup(group);
        if (azthGroup)
            groupSize = azthGroup->groupSize; // outworld party or limit case for dungeon
    }

    return groupSize;
}

uint32 AzthPlayer::getInstanceSize()
{
    if (!player)
        return 0;

    Map *map = player->FindMap();
    if (map && (map->IsDungeon() || map->IsRaid()))
    {
        // caso party instance
        InstanceSave *is = sInstanceSaveMgr->PlayerGetInstanceSave(player->GetGUID(), map->GetId(), player->GetDifficulty((map->IsRaid())));

        if (is)
            return sAZTH->GetAZTHInstanceSave(is)->groupSize;
    }

    return 0;
}
