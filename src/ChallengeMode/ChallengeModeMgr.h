/*
 *  Copyright (С) since 2019 Andrei Guluaev (Winfidonarleyan/Kargatum) https://github.com/Winfidonarleyan
 */

#ifndef _CHALLENGEMODE_MGR_H_
#define _CHALLENGEMODE_MGR_H_

#include "Common.h"
#include "Player.h"

class ChallengeModeMgr
{
public:
    static ChallengeModeMgr* instance();

    void LoadConfig(bool reload);

    bool isEligibleForReward(Player const *player);

    [[nodiscard]] uint32 getStartLevelHigher() {
    	return this->startLevelHigher;
    }

    [[nodiscard]] uint32 getDeltaLevelLower() {
    	return this->deltaLevelLower;
    }

    [[nodiscard]] uint32 getStartLevelLower() {
    	return this->startLevelLower;
    }

    [[nodiscard]] uint32 getDeltaLevelHigher() {
    	return this->deltaLevelHigher;
    }

private:
    uint32 deltaLevelHigher = 0;
    uint32 startLevelHigher = 0;
    uint32 deltaLevelLower = 0;
    uint32 startLevelLower = 0;
};

#define sChallengeMode ChallengeModeMgr::instance()

#endif // _CHALLENGEMODE_MGR_H_
