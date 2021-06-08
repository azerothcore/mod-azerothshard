#include "ScriptMgr.h"
#include "InstanceScript.h"
#include "InstanceSaveMgr.h"
#include "Player.h"
#include "Map.h"
#include "WorldSession.h"
#include "Group.h"
#include "AzthUtils.h"
#include "AZTH.h"

enum achievementStatsType
{
    ACHIEVEMENT_TYPE,
    CRITERIA_TYPE
};

class AzthGroupPlg : public GroupScript {
public:

    AzthGroupPlg() : GroupScript("AzthGroupPlg") {
    }

    void OnAddMember(Group* group, ObjectGuid guid) override {
        Player* player = ObjectAccessor::FindPlayer(guid);

        sAzthUtils->updateTwLevel(player, group);
    }
};

class PlayerStats : public PlayerScript
{
public:

    PlayerStats() : PlayerScript("PlayerStats") {
    }

    void OnUpdateZone(Player* /*player*/, uint32  /*newZone*/, uint32  /*newArea*/) override {
        // sAzthUtils->updateTwLevel(player, player->GetGroup()); <-- we will do it in TimeWalking.cpp script, having correct order with autoscaling
    }


    // Following 2 functions store levels in a temporary map

    void OnAchiComplete(Player *player, AchievementEntry const* achievement) override {
        AzthPlayer::AzthAchiData it = {
            player->getLevel(),
            sAZTH->GetAZTHPlayer(player)->getGroupLevel(),
            sAZTH->GetAZTHPlayer(player)->getPStatsLevel(false)
        };

        sAZTH->GetAZTHPlayer(player)->m_completed_achievement_map[achievement->ID] = it;
    }

    void OnCriteriaProgress(Player *player, AchievementCriteriaEntry const* criteria) override {
        AzthPlayer::AzthAchiData it = {
            player->getLevel(),
            sAZTH->GetAZTHPlayer(player)->getGroupLevel(),
            sAZTH->GetAZTHPlayer(player)->getPStatsLevel(false)
        };

        sAZTH->GetAZTHPlayer(player)->m_completed_criteria_map[criteria->ID] = it;
    }

    // Following 2 functions save our temporary maps inside the db

    void OnAchiSave(SQLTransaction& /* trans */, Player *player, uint16 achId, CompletedAchievementData achiData) override {
        if (sAZTH->GetAZTHPlayer(player)->m_completed_achievement_map.find(achId) != sAZTH->GetAZTHPlayer(player)->m_completed_achievement_map.end()) {
            AzthPlayer::AzthAchiData it = sAZTH->GetAZTHPlayer(player)->m_completed_achievement_map[achId];

            CharacterDatabase.AsyncPQuery("INSERT INTO azth_achievement_stats (playerGuid, achievement, type, level, levelParty, specialLevel, date) VALUES (%u, %u, %u, %u, %u, %u, %u)",
                player->GetGUID().GetCounter(),
                achId,
                ACHIEVEMENT_TYPE,
                it.level,
                it.levelParty,
                it.specialLevel,
                achiData.date
            );

            sAZTH->GetAZTHPlayer(player)->m_completed_achievement_map.erase(achId);
        }
    }

    void OnCriteriaSave(SQLTransaction& /* trans */, Player* player, uint16 critId, CriteriaProgress criteriaData) override {
        if (sAZTH->GetAZTHPlayer(player)->m_completed_criteria_map.find(critId) != sAZTH->GetAZTHPlayer(player)->m_completed_criteria_map.end()) {
            AzthPlayer::AzthAchiData it = sAZTH->GetAZTHPlayer(player)->m_completed_criteria_map[critId];

            CharacterDatabase.AsyncPQuery("INSERT INTO azth_achievement_stats (playerGuid, achievement, type, level, levelParty, specialLevel, date) VALUES (%u, %u, %u, %u, %u, %u, %u)",
                player->GetGUID().GetCounter(),
                critId,
                CRITERIA_TYPE,
                it.level,
                it.levelParty,
                it.specialLevel,
                criteriaData.date
            );

            sAZTH->GetAZTHPlayer(player)->m_completed_criteria_map.erase(critId);
        }
    }

};

void AddSC_playerstats() {
    new AzthGroupPlg();
    new PlayerStats();
}
