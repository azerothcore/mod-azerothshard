#include "Configuration/Config.h"
#include "AzthLanguageStrings.h"
#include "AzthUtils.h"
#include "ScriptMgr.h"
#include "AzthFirstKills.h"

class AzthWorldScript : public WorldScript
{
public:
    AzthWorldScript() : WorldScript("AzthWorldScript") { }
    
    void OnStartup() override
    {
        sAzthLang->loadStrings();
        sAzthUtils->loadClassSpells();
        sAzthFirstKills->loadCurrentFirstkills();
    }
};

void AddSC_AzthWorldScript()
{
    new AzthWorldScript();
}
