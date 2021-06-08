#include "AzthFirstKills.h"
#include "Player.h"
#include "AzthUtils.h"
#include "InstanceScript.h"
#include <stdio.h>
#include <time.h>
#include <ctime>

class InstanceScript;

// Copied from naxxramas.h
enum NxEncouters
{
    BOSS_PATCHWERK                 = 0,
    BOSS_GROBBULUS                 = 1,
    BOSS_GLUTH                     = 2,
    BOSS_NOTH                      = 3,
    BOSS_HEIGAN                    = 4,
    BOSS_LOATHEB                   = 5,
    BOSS_ANUB                      = 6,
    BOSS_FAERLINA                  = 7,
    BOSS_MAEXXNA                   = 8,
    BOSS_THADDIUS                  = 9,
    BOSS_RAZUVIOUS                 = 10,
    BOSS_GOTHIK                    = 11,
    BOSS_HORSEMAN                  = 12,
    BOSS_SAPPHIRON                 = 13,
    BOSS_KELTHUZAD                 = 14,
    MAX_ENCOUNTERS,
};

bool AzthFirstKills::canCompleteAchi(Player */* player */, uint32 /* achiId */)
{
    return true;
    /*
    uint32 plLevel=sAZTH->GetAZTHPlayer(player)->isTimeWalking() ? sAZTH->GetAZTHPlayer(player)->GetTimeWalkingLevel() : player->getLevel();
    uint32 groupLevel=sAZTH->GetAZTHPlayer(player)->getGroupLevel(false);

    uint32 level=sAzthUtils->maxTwLevel(groupLevel, plLevel);

    switch (achiId) {
        case ACHI_NAXXRAMAS:
        {
            if (InstanceScript* iscript=player->GetInstanceScript()) {
                if (
                    iscript->GetBossState(BOSS_MAEXXNA) == DONE &&
                    iscript->GetBossState(BOSS_LOATHEB) == DONE &&
                    iscript->GetBossState(BOSS_HORSEMAN) == DONE &&
                    iscript->GetBossState(BOSS_THADDIUS) == DONE
                ) {
                    return level == TIMEWALKING_LVL_NAXX;
                }
            }
            return false;
        }
        case ACHI_OBSIDIAN:
            return level == TIMEWALKING_LVL_VAS_LVL13;
        case ACHI_MAGIC_SEEKER:
            return level == TIMEWALKING_LVL_VAS_LVL13;
        case ACHI_DEATH_DEMISE:
        case ACHI_CELESTIAL_DEFENDER:
            return level == TIMEWALKING_LVL_VAS_LVL4;
        case ACHI_GRAND_CRUSADER:
            return level == TIMEWALKING_LVL_VAS_LVL4;
    }

    if (achiId == ACHI_FALL_OF_LK) {
        if (player->GetInstanceScript() && player->GetInstanceScript()->GetData(DATA_AZTH_HARD_MODE)) {
            return true;
        }

        return false;
    }

    return true;
    * */
}

void AzthFirstKills::setRealmCompleted(const AchievementEntry* achievement)
{
    this->currentFirstKills[achievement->ID] = std::chrono::system_clock::now();
}

bool AzthFirstKills::isRealmCompleted(AchievementEntry const* achievement, bool originalValue) {
    if (!achievement) //should never happen
        return originalValue;

    // if first kill exists in our std::map and has been achieved more than 1 minute ago
    // then it has been completed.
    std::map<uint32, std::chrono::system_clock::time_point>::iterator itr=this->currentFirstKills.find(achievement->ID);
    if (itr!=this->currentFirstKills.end()
        && (std::chrono::system_clock::now() - itr->second) > std::chrono::minutes(1))
        return true;

    // if the first kill does not exists in our map, it is not completed
    return false;
/*
    time_t now = time(0);
    struct tm * tnow = std::gmtime(&now);

    switch(achievement->ID) {
        case ACHI_NAXXRAMAS:
            if (tnow->tm_mon >= 0) { // January : 1-1
                return false;
            }
        break;
        case ACHI_OBSIDIAN:
            if (tnow->tm_mon >= 1) {// February
                AchievementEntry const* achievement = sAchievementStore.LookupEntry(ACHI_NAXXRAMAS);
                if (isRealmCompleted(achievement, false))
                    return false;
            }
        break;
        case ACHI_MAGIC_SEEKER:
            if (tnow->tm_mon >= 2) { // March
                AchievementEntry const* achievement = sAchievementStore.LookupEntry(ACHI_OBSIDIAN);
                if (isRealmCompleted(achievement, false))
                    return false;
            }
        break;
        case ACHI_DEATH_DEMISE:
            if (tnow->tm_mon >= 3) { // Avril
                AchievementEntry const* achievement = sAchievementStore.LookupEntry(ACHI_MAGIC_SEEKER);
                if (isRealmCompleted(achievement, false))
                    return false;
            }
        break;
        case ACHI_CELESTIAL_DEFENDER:
            if (tnow->tm_mon >= 4) { // May
                AchievementEntry const* achievement = sAchievementStore.LookupEntry(ACHI_DEATH_DEMISE);
                if (isRealmCompleted(achievement, false))
                    return false;
            }
        break;
        case ACHI_GRAND_CRUSADER:
            if (tnow->tm_mon >= 7) { // August
                AchievementEntry const* achievement = sAchievementStore.LookupEntry(ACHI_CELESTIAL_DEFENDER);
                if (isRealmCompleted(achievement, false))
                    return false;
            }
        break;
        case ACHI_FALL_OF_LK:
            if (tnow->tm_mon >= 8) { // September
                AchievementEntry const* achievement = sAchievementStore.LookupEntry(ACHI_CELESTIAL_DEFENDER);
                if (isRealmCompleted(achievement, false))
                    return false;
            }
        break;
    }

    // we assume that the achievement
    // has been completed (not available) at this point
    return true;
    */
}


void AzthFirstKills::loadCurrentFirstkills() {
    uint32 t= static_cast<uint32>(sAzthUtils->getStartsOfYear());

    QueryResult fkAchievements = CharacterDatabase.PQuery("SELECT DISTINCT(achievement) FROM character_achievement WHERE achievement IN (%u,%u,%u,%u,%u,%u,%u) AND date >= %u",
        ACHI_NAXXRAMAS,
        ACHI_OBSIDIAN,
        ACHI_MAGIC_SEEKER,
        ACHI_DEATH_DEMISE,
        ACHI_CELESTIAL_DEFENDER,
        ACHI_GRAND_CRUSADER,
        ACHI_FALL_OF_LK,
        t
    );

    if (fkAchievements)
    {
        do {
            Field* fkAchievementsFields = fkAchievements->Fetch();

            if (!fkAchievementsFields)
                break;

            uint32 achievement = fkAchievementsFields[0].GetUInt32();
            // initialize all completed first kill with the timestamp of beginning of the year ( > 1 minute )
            this->currentFirstKills[achievement]=std::chrono::system_clock::from_time_t(t);
        } while (fkAchievements->NextRow());

    }

}
