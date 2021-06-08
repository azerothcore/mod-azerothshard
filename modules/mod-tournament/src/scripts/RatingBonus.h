#ifndef ARENABONUS_H
#define ARENABONUS_H

#include "Define.h"
#include "Common.h"
#include "Chat.h"

struct Bonus
{
    uint32 type;
    float multiplier;
};

class BonusRating
{
    public:
        std::vector<Bonus> & getRatingBonuses();
		void addBonus(uint32 bracket, float multiplier);
		void removeBonus(uint32 bracket);
		void printBonusesToPlayer(ChatHandler * handler, uint32 bracket);
		int AZTH_RATING_BONUS_SECURITY = 2;
		const char* brackets[5] = { "1v1: ", "2v2: ", "3v3: ", "3v3 solo: ", "5v5: " };

    private:
        std::vector<Bonus> bonuses;
};

#define sBonusRating ACE_Singleton<BonusRating, ACE_Null_Mutex>::instance()

#endif
