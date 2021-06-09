#include "ScriptMgr.h"
#include "InstanceScript.h"
#include "InstanceSaveMgr.h"
#include "Player.h"
#include "Map.h"
#include "WorldSession.h"
#include "Group.h"
#include "AzthUtils.h"
#include "AZTH.h"

class AzthMiscPlg : public MiscScript
{
public:
    AzthMiscPlg() : MiscScript("AzthMiscPlg") { }

    void OnConstructPlayer(Player* origin) override
    {
        sAZTH->AddAZTHPlayer(origin);
    }

    void OnDestructPlayer(Player* origin) override
    {
        sAZTH->DeleteAZTHPlayer(origin);
    }

    void OnConstructObject(Object* origin) override
    {
        sAZTH->AddAZTHObject(origin);
    }

    void OnDestructObject(Object* origin) override
    {
        sAZTH->DeleteAZTHObject(origin);
    }

    void OnConstructGroup(Group* origin) override
    {
        sAZTH->AddAZTHGroup(origin);
    }

    void OnDestructGroup(Group* origin) override
    {
        sAZTH->DeleteAZTHGroup(origin);
    }

    void OnConstructInstanceSave(InstanceSave* origin) override
    {
        sAZTH->AddAZTHInstanceSave(origin);
    }

    void OnDestructInstanceSave(InstanceSave* origin) override
    {
        sAZTH->DeleteAZTHInstanceSave(origin);
    }
};

void AddSC_azth_misc_plg() {
    new AzthMiscPlg();
}

