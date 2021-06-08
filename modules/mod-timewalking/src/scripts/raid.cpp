#include "raid.h"

TwRaid::TwRaid()
{
	id = uint32(0);
	name = std::string("");
    exp = uint32(0);
	phase = uint32(0);
    level = uint32(0);
    bonus = uint32(0);
    criteria = uint32(0);
}

TwRaid::TwRaid(uint32 ItsId, std::string ItsName, uint32 ItsExp, uint32 ItsPhase, uint32 ItsLevel, uint32 ItsBonus, uint32 ItsCriteria)
{
	id = ItsId;
	name = ItsName;
    exp = ItsExp;
	phase = ItsPhase;
    level = ItsLevel;
    bonus = ItsBonus;
    criteria = ItsCriteria;
}

std::map<uint32, TwRaid> TwRaid::GetRaidList() const
{
    return raidList;
}

void TwRaid::SetRaidList(std::map<uint32, TwRaid> raidList)
{
    this->raidList = raidList;
}

uint32 TwRaid::GetId() const
{
	return id;
}

void TwRaid::SetId(uint32 ItsId)
{
	id = ItsId;
}

std::string TwRaid::GetName() const
{
	return name;
}

void TwRaid::SetName(std::string ItsName)
{
	name = ItsName;
}

uint32 TwRaid::GetExp() const
{
    return exp;
}

void TwRaid::SetExp(uint32 ItsExp)
{
    exp = ItsExp;
}

uint32 TwRaid::GetPhase() const
{
	return phase;
}

void TwRaid::SetPhase(uint32 ItsPhase)
{
	phase = ItsPhase;
}

uint32 TwRaid::GetLevel() const
{
    return level;
}

void TwRaid::SetLevel(uint32 ItsLevel)
{
    level = ItsLevel;
}

uint32 TwRaid::GetBonus() const
{
    return bonus;
}

uint32 TwRaid::hasBonus() const
{
    return bonus == 1;
}

void TwRaid::SetBonus(uint32 ItsBonus)
{
    bonus = ItsBonus;
}

uint32 TwRaid::GetCriteria() const
{
    return criteria;
}

void TwRaid::SetCriteria(uint32 ItsCriteria)
{
    criteria = ItsCriteria;
}
