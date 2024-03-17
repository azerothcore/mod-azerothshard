#include "AzthLanguage.h"
#include "AzthLanguageStrings.h"
#include "AzthPlayer.h"
#include "Player.h"
#include "SharedDefines.h"
#include "AZTH.h"

class Player;

AzthLangString::AzthLangString(std::string const def, std::string const it)
{
    this->def = def;
    this->it  = it;
}

AzthLangString::~AzthLangString()
{
}

AzthLang::AzthLang()
{
    strings = std::unordered_map<uint32, AzthLangString*>();
}

AzthLang::~AzthLang()
{
    //for (map<uint32, AzthLangString*>::iterator it = strings.begin(); it != strings.end(); ++it)
    //    delete it->second;
}

AzthLang* AzthLang::instance()
{
    static AzthLang instance;
    return &instance;
}

void AzthLang::add(uint32 strId, std::string const def, std::string const it)
{
    this->strings[strId]=new AzthLangString(def,it);
}

const char * AzthLang::get(uint32 strId, Player const* pl) const
{
    AzthCustomLangs loc = AZTH_LOC_EN;

    if (pl)
        loc = sAZTH->GetAZTHPlayer((Player*)pl)->getCustLang();

    std::unordered_map<uint32, AzthLangString*>::const_iterator itr=strings.find(strId);
    if (itr != strings.end())
    {
        switch (loc)
        {
            case AZTH_LOC_IT:
                return itr->second->it.c_str();
            break;
            case AZTH_LOC_EN:
                return itr->second->def.c_str();
            break;
        }
    }

    return "Unknown Azth string";
}

const char* AzthLang::getf(uint32 strId, Player const* pl, ...) const
{
    const char* format = get(strId, pl);
    va_list ap;
    va_start(ap, pl);

    va_list ap_copy;
    va_copy(ap_copy, ap);
    int len = vsnprintf(nullptr, 0, format, ap_copy);
    va_end(ap_copy);

    std::string result;
    result.resize(len + 1);
    vsnprintf(&result[0], len + 1, format, ap);
    va_end(ap);

    return result.c_str();
}
