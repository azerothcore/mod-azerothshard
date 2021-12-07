#include "AzthPlayer.h"
#include "Player.h"
#include "Group.h"
#include "azth_custom_hearthstone_mode.h"
#include "AzthUtils.h"

// Send a kill credit, skipping the normal checks on raid group.
void AzthPlayer::ForceKilledMonsterCredit(uint32 entry, uint64 guid) {
    uint16 addkillcount = 1;
    uint32 real_entry = entry;
    ObjectGuid _guid = ObjectGuid(guid);
    if (guid) {
        Creature* killed = player->GetMap()->GetCreature(_guid);
        if (killed && killed->GetEntry())
            real_entry = killed->GetEntry();
    }

    player->StartTimedAchievement(ACHIEVEMENT_TIMED_TYPE_CREATURE,
        real_entry); // MUST BE CALLED FIRST
    player->UpdateAchievementCriteria(
        ACHIEVEMENT_CRITERIA_TYPE_KILL_CREATURE, real_entry, addkillcount,
        guid ? player->GetMap()->GetCreature(_guid) : NULL);

    for (uint8 i = 0; i < MAX_QUEST_LOG_SIZE; ++i) {
        uint32 questid = player->GetQuestSlotQuestId(i);
        if (!questid)
            continue;

        Quest const* qInfo = sObjectMgr->GetQuestTemplate(questid);
        if (!qInfo)
            continue;

        QuestStatusMap::const_iterator _itr = player->getQuestStatusMap().find(questid);
        if (_itr == player->getQuestStatusMap().end())
            continue;

        QuestStatusData q_status = _itr->second;
        if (q_status.Status == QUEST_STATUS_INCOMPLETE && (qInfo->IsPVPQuest() && (!player->GetGroup() || (player->GetGroup()->isBFGroup() || player->GetGroup()->isBGGroup()))))
        {
            if (qInfo->HasSpecialFlag(QUEST_SPECIAL_FLAGS_KILL) /*&& !qInfo->HasSpecialFlag(QUEST_SPECIAL_FLAGS_CAST)*/)
            {
                for (uint8 j = 0; j < QUEST_OBJECTIVES_COUNT; ++j)
                {
                    // skip GO activate objective or none
                    if (qInfo->RequiredNpcOrGo[j] <= 0)
                        continue;

                    uint32 reqkill = qInfo->RequiredNpcOrGo[j];

                    if (reqkill == real_entry)
                    {
                        uint32 reqkillcount = qInfo->RequiredNpcOrGoCount[j];
                        uint16 curkillcount = q_status.CreatureOrGOCount[j];

                        if (curkillcount < reqkillcount)
                        {
                            q_status.CreatureOrGOCount[j] = curkillcount + addkillcount;

                            QuestStatusSaveMap::iterator __itr = player->GetQuestStatusSaveMap().find(questid);
                            if (__itr == player->GetQuestStatusSaveMap().end())
                                continue;

                            __itr->second = true;

                            //player->m_QuestStatusSave[questid] = true;

                            player->SendQuestUpdateAddCreatureOrGo(
                                qInfo, _guid, j, curkillcount, addkillcount);
                        }
                        if (player->CanCompleteQuest(questid))
                            player->CompleteQuest(questid);
                        else
                            player->AdditionalSavingAddMask(ADDITIONAL_SAVING_QUEST_STATUS);

                        // same objective target can be in many active quests, but not in 2
                        // objectives for single quest (code optimization).
                        break;
                    }
                }
            }
        }
    }
}

bool AzthPlayer::passHsChecks(Quest const* qInfo, uint32 entry, uint32& realEntry, ObjectGuid /*guid*/) {
    uint32 qId = qInfo->GetQuestId();
    uint32 currentDimension = getCurrentDimensionByAura();

    HearthstoneQuest* hsQuest = sHearthstoneMode->getHeartstoneQuestInfo(qId);

    // if no data present in hearthstone table
    // then use normal behaviour
    if (!hsQuest) {
        // Do not allow official quest completition in empty/custom dimensions
        if (sAzthUtils->isPhasedDimension(currentDimension))
            return false;

        return true; // pass the check if no Hs quest exists
    }

    if (hsQuest->reqDimension > 0 && hsQuest->reqDimension != currentDimension)
        return false;

    // positive value: return false if groupsize is higher than groupLimit
    // negative value: return false if groupsize is lower than groupLimit
    if ((hsQuest->groupLimit > 0 && this->getGroupSize() > uint8(hsQuest->groupLimit))
        || (hsQuest->groupLimit < 0 && this->getGroupSize() < uint8(abs(hsQuest->groupLimit))))
        return false;

    /*
     * check for quests that require timewalking
     */
     // normal timewalking
    if (hsQuest->specialLevel > 0) {
        //special timewalking
        uint32 groupLevel = this->getGroupLevel(false);
        uint32 specialLevel = groupLevel > 0 ? groupLevel : GetTimeWalkingLevel();

        if (specialLevel == hsQuest->specialLevel || sAzthUtils->isMythicCompLvl(hsQuest->specialLevel, specialLevel)) {

            for (uint8 j = 0; j < QUEST_OBJECTIVES_COUNT; ++j) {
                if (qInfo->RequiredNpcOrGo[j] > 0 && uint32(qInfo->RequiredNpcOrGo[j]) == entry)
                    realEntry = entry;
            }

            return true;
        }
    }
    else if (qInfo->GetQuestLevel() <= 80) {
        uint32 groupLevel = this->getGroupLevel();
        uint32 level = groupLevel > 0 ? groupLevel : this->player->getLevel();

        if (level >= qInfo->GetMinLevel() && qInfo->GetQuestLevel() > 0 && level <= uint32(qInfo->GetQuestLevel())) {
            for (uint8 j = 0; j < QUEST_OBJECTIVES_COUNT; ++j) {
                if (qInfo->RequiredNpcOrGo[j] > 0 && uint32(qInfo->RequiredNpcOrGo[j]) == entry)
                    realEntry = entry;
            }

            return true;
        }
    }

    // in this case conditions above are negative
    // so check has not passed
    return false;
}
