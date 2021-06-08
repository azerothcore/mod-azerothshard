/*
 *  Copyright (С) since 2019 Andrei Guluaev (Winfidonarleyan/Kargatum) https://github.com/Winfidonarleyan 
 */

#ifndef _AZTH_MODULE_H_
#define _AZTH_MODULE_H_

#include "Common.h"
#include "AzthPlayer.h"
#include "AzthObject.h"
#include "AzthGroupMgr.h"
#include "AzthInstanceMgr.h"
#include "Object.h"
#include "GroupMgr.h"
#include "Player.h"
#include "Spell.h"
#include <unordered_map>

class AZTH
{
public:
    static AZTH* instance();

    void LoadConfig(bool reload);
    void LoadContainers();

    // Add in store
    void AddAZTHPlayer(Player* player);
    void AddAZTHObject(Object* object);
    void AddAZTHGroup(Group* group);
    void AddAZTHInstanceSave(InstanceSave* instanceSave);
    void AddAZTHLoot(Loot* loot);

    // Get from store
    AzthPlayer* GetAZTHPlayer(Player* player);
    AzthObject* GetAZTHObject(Object* object);
    AzthGroupMgr* GetAZTHGroup(Group* group);
    AzthInstanceMgr* GetAZTHInstanceSave(InstanceSave* instanceSave);
    bool GetAZTHLoot(Loot* loot);

    // Delete in store
    void DeleteAZTHPlayer(Player* player);
    void DeleteAZTHObject(Object* object);
    void DeleteAZTHGroup(Group* group);
    void DeleteAZTHInstanceSave(InstanceSave* instanceSave);
    void DeleteAZTHLoot(Loot* loot);

    // PVP Rank Patch
    uint32 GetKillsPerRank(uint32 Rank);
    float GetRatePvPRankExtraHonor();

    //[AZTH] in-game mailer
    void SendGameMail(Player* receiver, std::string subject, std::string body, uint32 money, uint32 itemId = 0, uint32 itemCount = 0); 

    // ExternalMail
    bool IsExternalMailEnable();
    uint32 GetExternalMailInterval();
    void SendExternalMails();

    // Custom XP
    uint32 GetCustomXPSecurity();
    float GetCustomXPMax();
    bool IsCustomXPShowOnLogin();

private:
    typedef std::unordered_map<Player*, AzthPlayer*> AZTHPlayersContainer;
    typedef std::unordered_map<Object*, AzthObject*> AZTHObjectsContainer;
    typedef std::unordered_map<Group*, AzthGroupMgr*> AZTHGroupsContainer;
    typedef std::unordered_map<InstanceSave*, AzthInstanceMgr*> AZTHInstanceSaveContainer;
    typedef std::unordered_map<Loot*, bool /*second round*/> AZTHLootContainer;

    AZTHPlayersContainer _playersStore;
    AZTHObjectsContainer _objectsStore;
    AZTHGroupsContainer _groupStore;
    AZTHInstanceSaveContainer _instanceSaveStore;
    AZTHLootContainer _lootStore;

    // PVP Rank Patch
    uint32 _PvP_Ranks[HKRANKMAX]; 
    float _RatePvPRankExtraHonor = 0.0f;

    // ExternalMail
    bool _IsExternalMailEnable = false;
    uint32 _ExternalMailInterval = MINUTE * IN_MILLISECONDS;

    // Custom XP
    uint32 _CustomXPSecurity = 0;
    float _CustomXPMax = 0.0f;
    bool _CustomXPShowOnLogin = false;
};

#define sAZTH AZTH::instance()

#endif // _AZTH_MODULE_H_
