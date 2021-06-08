/**
    This plugin can be used for common player customizations
 */

#include "ScriptMgr.h"
#include "RatingBonus.h"
#include "Battleground.h"
#include "AzthLanguage.h"
#include "AzthLanguageStrings.h"

class RatingBonus : public FormulaScript{
public:

    RatingBonus() : FormulaScript("RatingBonus") { }

    void OnAfterArenaRatingCalculation(Battleground *const bg, int32 &winnerMatchmakerChange, int32 & /*loserMatchmakerChange*/, int32 &winnerChange, int32 & /*loserChange*/) override
    {
        std::vector<Bonus> & currBonuses = sBonusRating->getRatingBonuses();

        // no bonuses right now
        if (currBonuses.size() == 0)
            return;

        for (std::size_t i = 0; i < currBonuses.size(); i++)
        {
            if (currBonuses[i].type == bg->GetArenaType())
            {
                winnerMatchmakerChange = winnerMatchmakerChange * currBonuses[i].multiplier;
                winnerChange = winnerChange * currBonuses[i].multiplier;
            }
        }
    }

	void OnBeforeUpdatingPersonalRating(int32 &mod, uint32 type) override
	{
		std::vector<Bonus> & currBonuses = sBonusRating->getRatingBonuses();

		// no bonuses right now
		if (currBonuses.size() == 0)
			return;

		for (std::size_t i = 0; i < currBonuses.size(); i++)
		{
			if (currBonuses[i].type == type)
			{
				mod = (int32)ceil(mod * currBonuses[i].multiplier);
			}
		}
	}

};

void BonusRating::addBonus(uint32 bracket, float multiplier)
{
	// remove an existing same-type bonus if any
	removeBonus(bracket);

	Bonus bonus = {};
	bonus.type = bracket;
	bonus.multiplier = multiplier;
	//sLog->outString("Bonus added: bracket: %d mult: %.2f", bracket, multiplier);
	bonuses.push_back(bonus);
}

void BonusRating::removeBonus(uint32 bracket)
{
	for (std::size_t i = 0; i < bonuses.size(); i++)
	{
		if (bonuses[i].type == bracket)
		{
			//sLog->outString("Bonus removed: %d", bracket);
			bonuses.erase(bonuses.begin() + i);
		}
	}
}

void BonusRating::printBonusesToPlayer(ChatHandler * handler, uint32 bracket)
{
	for (std::size_t i = 0; i < bonuses.size(); i++)
	{
		if (bonuses[i].type == bracket || bracket == 0)
		{
			handler->PSendSysMessage("%s", sAzthLang->getf(AZTH_LANG_BR_ACTIVE_BONUS , handler->GetSession()->GetPlayer(), brackets[bonuses[i].type - 1], bonuses[i].multiplier));
		}
	}
	if (bonuses.size() == 0)
	{
		handler->PSendSysMessage("%s", sAzthLang->get(AZTH_LANG_BR_NO_BONUS, handler->GetSession()->GetPlayer()));
	}
}

std::vector<Bonus> & BonusRating::getRatingBonuses()
{
    return bonuses;
}

void AddAzTournamentScripts() {
    new RatingBonus();
}

