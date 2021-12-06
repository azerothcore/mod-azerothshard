#include "ChallengeModeMgr.h"
#include "Config.h"
#include "AzthUtils.h"

ChallengeModeMgr* ChallengeModeMgr::instance()
{
    static ChallengeModeMgr instance;
    return &instance;
}

void ChallengeModeMgr::LoadConfig(bool /* reload */)
{
    this->deltaLevelHigher = sConfigMgr->GetOption<uint32>("ChallengeMode.noReward.deltaLevelHigher", 0);
    this->deltaLevelLower = sConfigMgr->GetOption<uint32>("ChallengeMode.noReward.deltaLevelLower", 0);
    this->startLevelHigher = sConfigMgr->GetOption<uint32>("ChallengeMode.noReward.startLevelHigher", 0);
    this->startLevelLower = sConfigMgr->GetOption<uint32>("ChallengeMode.noReward.startLevelLower", 0);
}

bool ChallengeModeMgr::isEligibleForReward(Player const *player) {
    if (!player->IsInWorld())
        return true;

    // disable rewards only for dungeons
    if (!player->GetMap()->IsDungeon() && !player->GetMap()->IsRaid())
        return true;

    WorldLocation pos = WorldLocation(player->GetMapId(), player->GetPositionX(), player->GetPositionY(), player->GetPositionZ(), player->GetOrientation());
    uint32 posLvl=sAzthUtils->getPositionLevel(false, player->GetMap(), pos);

    uint32 level = player->getLevel();

    // disable reward when the player has a higher level than the one specified in the configs
    // it can be useful to force players doing the challenges at the correct level (E.G. during the levelling)
    if (this->startLevelLower && this->startLevelLower >= posLvl && level > posLvl && deltaLevelLower >= level - posLvl) {
        return false;
    }

    // disable reward when the player has a lower level than the one specified in the configs
    // it can be useful to avoid power-levelling/character-towing or to avoid cheating on certain progression-based servers
    if (this->startLevelHigher && posLvl >= this->startLevelHigher && posLvl > level && posLvl - level >= deltaLevelHigher) {
        return false;
    }

    return true;
}
