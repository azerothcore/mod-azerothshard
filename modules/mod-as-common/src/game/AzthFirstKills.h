#ifndef AZTH_FIRST_KILL_H
#define AZTH_FIRST_KILL_H

#include "Common.h"
#include <ace/Singleton.h>
#include "DatabaseEnv.h"
#include "DBCEnums.h"
#include "DBCStores.h"
#include "Player.h"
#include <chrono>

class Player;

enum FirstKills:uint32 {
    ACHI_NAXXRAMAS=1402, //-- "Conqueror of Naxxramas"    
    ACHI_OBSIDIAN=456, //-- "Obsidian Slayer",      
    ACHI_MAGIC_SEEKER=1400, //-- "Magic Seeker",     
    ACHI_DEATH_DEMISE=3117, //-- "Death's Demise",       
    ACHI_CELESTIAL_DEFENDER=3259, //-- "Celestial Defender",   
    ACHI_GRAND_CRUSADER=4078, //-- "Grand Crusader",       
    ACHI_FALL_OF_LK=4576, //-- "Fall of the Lich King",
};

class AzthFirstKills {
    friend class ACE_Singleton<AzthFirstKills, ACE_Null_Mutex>;
public:
    void loadCurrentFirstkills();
    bool isRealmCompleted(AchievementEntry const* achievement, bool originalValue);
    void setRealmCompleted(AchievementEntry const* achievement);
    bool canCompleteAchi(Player *player, uint32 achiId);
private:
    std::map<uint32 /*achiID*/, std::chrono::system_clock::time_point /*completionTime*/> currentFirstKills;
};

#define sAzthFirstKills ACE_Singleton<AzthFirstKills, ACE_Null_Mutex>::instance()

#endif