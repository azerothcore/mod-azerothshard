#include "Configuration/Config.h"
#include "AzthLanguageStrings.h"
#include "AzthUtils.h"
#include "ScriptMgr.h"
#include "AzthFirstKills.h"
#include "ExtraDatabase.h"
#include "DatabaseLoader.h"

class AzthWorldScript : public WorldScript
{
public:
    AzthWorldScript() : WorldScript("AzthWorldScript") { }

    void OnStartup() override
    {
        // DatabaseLoader loader("server.worldserver");
        // loader
        //     .AddDatabase(ExtraDatabase, "Extra");
        sAzthLang->loadStrings();
        sAzthUtils->loadClassSpells();
        sAzthFirstKills->loadCurrentFirstkills();
    }
};

void AddSC_AzthWorldScript()
{
    new AzthWorldScript();
}
