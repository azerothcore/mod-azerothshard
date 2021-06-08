#ifndef AZTH_UTILS_H
#define	AZTH_UTILS_H

#include "Common.h"
#include "Define.h"
#include "Config.h"
#include "Item.h"
#include "ItemTemplate.h"
#include "ObjectMgr.h"
#include "Player.h"
#include "Group.h"
#include "SpellAuras.h"
#include "SharedDefines.h"
#include <stdio.h>
#include <time.h>
#include "AzthLevelStat.h"
#include "AzthSharedDefines.h"

class AuraApplication;
class Group;

class AzthUtils
{
    friend class ACE_Singleton<AzthUtils, ACE_Null_Mutex>;
public:
    
    std::string INVTYPE_NON_EQUIP_STRING;
    std::string INVTYPE_HEAD_STRING;
    std::string INVTYPE_NECK_STRING;
    std::string INVTYPE_SHOULDERS_STRING;
    std::string INVTYPE_BODY_STRING;
    std::string INVTYPE_CHEST_STRING;
    std::string INVTYPE_WAIST_STRING;
    std::string INVTYPE_LEGS_STRING;
    std::string INVTYPE_FEET_STRING;
    std::string INVTYPE_WRISTS_STRING;
    std::string INVTYPE_HANDS_STRING;
    std::string INVTYPE_FINGER_STRING;
    std::string INVTYPE_TRINKET_STRING;
    std::string INVTYPE_WEAPON_STRING;
    std::string INVTYPE_SHIELD_STRING;
    std::string INVTYPE_RANGED_STRING;
    std::string INVTYPE_CLOAK_STRING;
    std::string INVTYPE_2HWEAPON_STRING;
    std::string INVTYPE_BAG_STRING;
    std::string INVTYPE_TABARD_STRING;
    std::string INVTYPE_ROBE_STRING;
    std::string INVTYPE_WEAPONMAINHAND_STRING;
    std::string INVTYPE_WEAPONOFFHAND_STRING;
    std::string INVTYPE_HOLDABLE_STRING;
    std::string INVTYPE_AMMO_STRING;
    std::string INVTYPE_THROWN_STRING;
    std::string INVTYPE_RANGEDRIGHT_STRING;
    std::string INVTYPE_QUIVER_STRING;
    std::string INVTYPE_RELIC_STRING;
    
    AzthUtils();
    ~AzthUtils();

    uint32 selectCorrectSpellRank(uint8 level, uint32 spellId);

    float getCustomMultiplier(ItemTemplate const * pProto, uint32 multiplier);

    uint32 getCalcReqLevel(ItemTemplate const* pProto);
    
    void learnClassSpells(Player* player, bool new_level);
    
    void loadClassSpells();
    
    void sendMessageToGroup(Player *pl, Group *group, const char* msg);
    
    time_t getStartsOfYear();
    
    bool checkItemLvL(ItemTemplate const* proto,uint32 level);
    
    int getReaction(Unit const* unit, Unit const* target);
    
    bool canFly(Unit* const caster, Unit* originalCaster);
    
    uint32 getFreeSpaceInBags(Player *player);

    uint32 calculateItemScalingValue(ItemTemplate const* pProto, Player *pl = NULL);    
    int32 normalizeFeralAp(int32 feralBonus, int32 extraDPS, ItemTemplate const* pProto, bool isScaling);
    
    uint32 getMaxWeaponSkillByLevel(uint32 level) {
        return level * 5;
    }
    
    bool isAshenBand(uint32 entry);
    
    SpellCastResult checkSpellCast(Player *player, SpellInfo const* spell, bool notify);
    bool canPrepareSpell(Spell *spell, Unit *m_caster, SpellInfo const* m_spellInfo, SpellCastTargets const* targets, AuraEffect const* triggeredByAura);
    void onAuraRemove(AuraApplication * aurApp, AuraRemoveMode mode);
    
    // [Timewalking]
    uint32 getPositionLevel(bool includeSpecialLvl, Map *map, uint32 zone = 0, uint32 area = 0) const;
    uint32 getPositionLevel(bool includeSpecialLvl, Map *map, WorldLocation const& posInfo) const;
    
    uint32 selectSpellForTW(Player* player, uint32 spellId);

    void removeTimewalkingAura(Unit * unit);
    
    std::string getLevelInfo(uint32 level);
    
    bool updateTwLevel(Player *player, Group *group=nullptr);
    
    int32 getSpellReduction(Player *player, SpellInfo const* spellProto);
    
    bool isEligibleForBonusByArea(Player const *player);
    
    bool canScaleSpell(SpellInfo const* spellProto);
    
    bool isSpecialSpellForTw(SpellInfo const* spellProto);
    
    bool isNotAllowedSpellForTw(SpellInfo const* spellProto);
    
    void setTwAuras(Unit *unit, AzthLevelStat const *stats, bool apply, bool skipDefense);
    void setTwDefense(Player *player, bool apply);
    AzthLevelStat const* getTwStats(Player *player, uint32 level);
    
    uint32 maxTwLevel(uint32 sourceLvl, uint32 compareLevel) const;
    
    bool disableEnchant(Player *player, SpellItemEnchantmentEntry const* pEnchant);
    // [/Timewalking]
    
    // DIMENSIONS
    std::string getDimensionName(uint32 dim);
    bool isPhasedDimension(uint32 dim);
    PhaseDimensionsEnum getCurrentDimensionByPhase(uint32 phase);
    bool dimIntegrityCheck(Unit const* target, uint32 phasemask);
    bool isSharedArea(Player *player, MapEntry const *mEntry, uint32 zone, uint32 area);
    // /DIMENSIONS
    
    // [Flex Mythic]
    bool isMythicLevel(uint32 level);
    bool isMythicCompLvl(uint32 reqLvl, uint32 checkLvl);
    bool canMythicHere(Unit const* source);
    // [/Flex Mythic]

    // horde version of objectmgr factionchangeitems map
    ObjectMgr::CharacterConversionMap FactionChangeItemsHorde;
    
    std::list<uint32> startSpells[MAX_RACES][MAX_CLASSES];
    
    std::string GetItemIcon(uint32 entry, uint32 width, uint32 height, int x, int y);
    std::vector<std::string> getCategoryIconAndNameByItemType(uint32 itemType);
    
private:
    std::map <uint32,PhaseDimensionsEnum> phaseMap;

};

#define sAzthUtils ACE_Singleton<AzthUtils, ACE_Null_Mutex>::instance()

#endif
