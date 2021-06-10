#ifndef AZTH_ACHIEVEMENT_H
#define	AZTH_ACHIEVEMENT_H

#include "Common.h"
#include "Define.h"
#include "Config.h"
#include <map>

class AzthAchievement
{
public:
    //GETTERS
    uint32 GetAchievement() const;
    uint32 GetCriteria() const;
    uint32 GetPoints() const;
    uint32 GetCategory() const;
    uint32 GetParCategory() const;
    uint32 GetDifficulty() const;
    uint32 GetLevelMax() const;
    uint32 GetLevelMin() const;
    uint32 GetLevel() const;
    uint32 GetOriginalPoints() const;
    std::string GetName() const;
    std::string GetDescription() const;
    uint32 GetReward() const;
    uint32 GetRewardCount() const;
    uint32 GetKillCredit() const;
    uint32 GetSpecialLevelReq() const;
    uint32 GetReqDimension() const;

    AzthAchievement(uint32 achievement = 0, uint32 criteria = 0, uint32 points = 0, uint32 category = 0, uint32 parentCategory = 0, uint32 difficulty = 0, uint32 levelMax = 0,
                    uint32 levelMin = 0, uint32 level = 0, uint32 originalPoints = 0, std::string name = "", std::string description = "", uint32 reward = 0, uint32 rewardCount = 0, uint32 killCredit = 0,
                    uint32 specialLevelReq = 0, uint32 reqDimension = 0);


private:
    uint32 achievement;
    uint32 criteria;
    uint32 points;
    uint32 category;
    uint32 parentCategory;
    uint32 difficulty;
    uint32 levelMax;
    uint32 levelMin;
    uint32 level;
    uint32 originalPoints;
    std::string name;
    std::string description;
    uint32 reward;
    uint32 rewardCount;
    uint32 killCredit;
    uint32 specialLevelReq;
    uint32 reqDimension;
};

class AzthAchievementMgr {
public:
    static AzthAchievementMgr* instance();

    std::map<uint32, AzthAchievement> achievementList;
};

#define sAzthAchievementMgr AzthAchievementMgr::instance()

#endif
