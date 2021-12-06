#ifndef SMARTSTONE_APPS_H
#define SMARTSTONE_APPS_H

#include "Define.h"
#include "Common.h"
#include "Player.h"
#include "AzthLanguageStrings.h"

#define AZTH_SMRTST_POSITION_HOUSE_INDEX 100000

class SmartStoneApps
{
public:
    
    /*
     *  PvP Menu
     */
    void teleportDalaran(Player *player);

    /*
     * CharMenu
     */
    void maxSkill(Player *player);
    void changeRace(Player *player);
    void changeFaction(Player *player);
    void rename(Player *player);
    void changeExp(Player *player, const char* code);
    void resetAuras(Player *player);
    
    /*
     * Misc
     */
    void jukebox(Player *player);
    void blackMarketTeleport(Player *player);
    void teleportHouse(Player *owner, Player *guest);
    
    /*
     * Utils
     */
    
    bool isFloatNumber(const std::string& string) {
        std::string::const_iterator it = string.begin();
        bool decimalPoint = false;
        int minSize = 0;
        if (string.size() > 0 && (string[0] == '-' || string[0] == '+')) {
            it++;
            minSize++;
        }
        while (it != string.end()) {
            if (*it == '.' || *it == ',') {
                if (!decimalPoint) decimalPoint = true;
                else break;
            } else if (!isdigit(*it) && ((*it != 'f') || it + 1 != string.end() || !decimalPoint)) {
                break;
            }
            ++it;
        }
        return string.size() > (size_t)minSize && it == string.end();
    }
    
};

#endif
