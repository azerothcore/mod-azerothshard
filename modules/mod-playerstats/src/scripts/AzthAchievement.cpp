#include "AzthAchievement.h"

AzthAchievement::AzthAchievement(uint32 achievement, uint32 criteria, uint32 points, uint32 category, uint32 parentCategory, uint32 difficulty, uint32 levelMax, uint32 levelMin,
                                 uint32 level, uint32 originalPoints, std::string name, std::string description, uint32 reward, uint32 rewardCount, uint32 killCredit, uint32 specialLevelReq, uint32 reqDimension)
{
    this->achievement = achievement;
    this->criteria = criteria;
    this->points = points;
    this->category = category;
    this->parentCategory = parentCategory;
    this->difficulty = difficulty;
    this->levelMax = levelMax;
    this->levelMin = levelMin;
    this->level = level;
    this->originalPoints = originalPoints;
    this->name = name;
    this->description = description;
    this->reward = reward;
    this->rewardCount = rewardCount;
    this->killCredit = killCredit;
    this->specialLevelReq = specialLevelReq;
    this->reqDimension = reqDimension;
}

uint32 AzthAchievement::GetAchievement() const
{
    return achievement;
}

uint32 AzthAchievement::GetCriteria() const
{
    return criteria;
}

uint32 AzthAchievement::GetPoints() const
{
    return points;
}

uint32 AzthAchievement::GetCategory() const
{
    return category;
}

uint32 AzthAchievement::GetParCategory() const
{
    return parentCategory;
}

uint32 AzthAchievement::GetDifficulty() const
{
    return difficulty;
}

uint32 AzthAchievement::GetLevelMax() const
{
    return levelMax;
}

uint32 AzthAchievement::GetLevelMin() const
{
    return levelMin;
}

uint32 AzthAchievement::GetLevel() const
{
    return level;
}

uint32 AzthAchievement::GetOriginalPoints() const
{
    return originalPoints;
}

std::string AzthAchievement::GetName() const
{
    return name;
}

std::string AzthAchievement::GetDescription() const
{
    return description;
}

uint32 AzthAchievement::GetReward() const
{
    return reward;
}

uint32 AzthAchievement::GetRewardCount() const
{
    return rewardCount;
}

uint32 AzthAchievement::GetKillCredit() const
{
    return killCredit;
}

uint32 AzthAchievement::GetSpecialLevelReq() const
{
    return specialLevelReq;
}

uint32 AzthAchievement::GetReqDimension() const
{
    return reqDimension;
}
