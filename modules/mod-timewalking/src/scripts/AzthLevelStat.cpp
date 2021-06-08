#include "AzthLevelStat.h"


const std::map<uint32, AzthLevelStat> & AzthLevelStatMgr::GetLevelStatList()
{
    const std::map<uint32, AzthLevelStat> &ptr = levelStatList;
    return ptr;
}

void AzthLevelStatMgr::SetLevelStatList(std::map<uint32, AzthLevelStat> levelStatList)
{
    this->levelStatList = levelStatList;
}

AzthLevelStatMgr* AzthLevelStatMgr::instance()
{
    static AzthLevelStatMgr instance;
    return &instance;
}

AzthLevelStat::AzthLevelStat(uint32 level, uint32 race, uint32 Class, std::map<aura_timewalking_enum, uint32> pctMap)
{
    this->level = level;
    this->race = race;
    this->Class = Class;
    this->pctMap = pctMap;
}


uint32 AzthLevelStat::GetLevel() const
{
    return level;
}

void AzthLevelStat::SetLevel(uint32 level)
{
    this->level = level;
}

uint32 AzthLevelStat::GetRace() const
{
    return race;
}

void AzthLevelStat::SetRace(uint32 race)
{
    this->race = race;
}

uint32 AzthLevelStat::GetClass() const
{
    return Class;
}

void AzthLevelStat::SetClass(uint32 Class)
{
    this->Class = Class;
}


uint32 AzthLevelStat::GetPct(aura_timewalking_enum aura)
{
    return this->pctMap[aura];
}


void AzthLevelStat::SetPct(aura_timewalking_enum aura, uint32 val)
{
    this->pctMap[aura]=val;
}
