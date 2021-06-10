#ifndef AZTH_RAID_H
#define	AZTH_RAID_H

#include "Common.h"
#include "Define.h"
#include "Config.h"
#include <map>

class TwRaid
{
public:
    static TwRaid* instance();

	//GETTERS
	uint32 GetId() const;
	std::string GetName() const;
    uint32 GetExp() const;
	uint32 GetPhase() const;
    uint32 GetLevel() const;
    uint32 GetBonus() const;
    uint32 hasBonus() const;
    uint32 GetCriteria() const;

	//SETTERS
	void SetId(uint32 id);
	void SetName(std::string name);
    void SetExp(uint32 exp);
	void SetPhase(uint32 phase);
    void SetLevel(uint32 level);
    void SetBonus(uint32 bonus);
    void SetCriteria(uint32 criteria);

	TwRaid();
	TwRaid(uint32 id, std::string name, uint32 exp, uint32 phase, uint32 level, uint32 bonus, uint32 criteria);

    std::map<uint32, TwRaid> GetRaidList() const;
    void SetRaidList(std::map<uint32, TwRaid> raidList);

private:
	uint32 id;
	std::string name;
    uint32 exp;
	uint32 phase;
    uint32 level;
    uint32 bonus;
    uint32 criteria;
    std::map<uint32, TwRaid> raidList;
};

#define sAzthRaid TwRaid::instance()

#endif
