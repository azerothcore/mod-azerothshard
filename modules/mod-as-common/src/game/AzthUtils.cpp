#include "AzthUtils.h"
#include "AzthLevelStat.h"
#include "Pet.h"
#include "SpellMgr.h"
#include "SpellInfo.h"
#include "AzthSharedDefines.h"
#include "Spell.h"
#include "GuildHouse.h"
#include "BattlefieldWG.h"
#include "InstanceScript.h"
#include "AZTH.h"
#include <ctime>

class InstanceScript;

// Copied from naxxramas.h
enum NxEncouters
{
    BOSS_PATCHWERK                 = 0,
    BOSS_GROBBULUS                 = 1,
    BOSS_GLUTH                     = 2,
    BOSS_NOTH                      = 3,
    BOSS_HEIGAN                    = 4,
    BOSS_LOATHEB                   = 5,
    BOSS_ANUB                      = 6,
    BOSS_FAERLINA                  = 7,
    BOSS_MAEXXNA                   = 8,
    BOSS_THADDIUS                  = 9,
    BOSS_RAZUVIOUS                 = 10,
    BOSS_GOTHIK                    = 11,
    BOSS_HORSEMAN                  = 12,
    BOSS_SAPPHIRON                 = 13,
    BOSS_KELTHUZAD                 = 14,
    MAX_ENCOUNTERS,
};


AzthUtils::AzthUtils()
{
    INVTYPE_NON_EQUIP_STRING = "Generic";
    INVTYPE_HEAD_STRING = "Heads";
    INVTYPE_NECK_STRING = "Necks";
    INVTYPE_SHOULDERS_STRING = "Shoulders";
    INVTYPE_BODY_STRING = "Body";
    INVTYPE_CHEST_STRING = "Chests";
    INVTYPE_WAIST_STRING = "Waists";
    INVTYPE_LEGS_STRING = "Legs";
    INVTYPE_FEET_STRING = "Feets";
    INVTYPE_WRISTS_STRING = "Wrists";
    INVTYPE_HANDS_STRING = "Hands";
    INVTYPE_FINGER_STRING = "Fingers";
    INVTYPE_TRINKET_STRING = "Trinkets";
    INVTYPE_WEAPON_STRING = "Weapons";
    INVTYPE_SHIELD_STRING = "Shields";
    INVTYPE_RANGED_STRING = "Ranged";
    INVTYPE_CLOAK_STRING = "Cloaks";
    INVTYPE_2HWEAPON_STRING = "2H Weapons";
    INVTYPE_BAG_STRING = "Bags";
    INVTYPE_TABARD_STRING = "Tabards";
    INVTYPE_ROBE_STRING = "Robes";
    INVTYPE_WEAPONMAINHAND_STRING = "Main Hand Weapons";
    INVTYPE_WEAPONOFFHAND_STRING = "Off Hand Weapons";
    INVTYPE_HOLDABLE_STRING = "Holdables";
    INVTYPE_AMMO_STRING = "Ammo";
    INVTYPE_THROWN_STRING = "Thrown";
    INVTYPE_RANGEDRIGHT_STRING = "Right Ranged";
    INVTYPE_QUIVER_STRING = "Quiver";
    INVTYPE_RELIC_STRING = "Relics";


    for (ObjectMgr::CharacterConversionMap::iterator i = sObjectMgr->FactionChangeItems.begin(); i != sObjectMgr->FactionChangeItems.end(); ++i)
        this->FactionChangeItemsHorde[i->second] = i->first;

    for (uint8 i=1; i<=MAX_DIMENSION; i++) {
        SpellInfo const* spellInfo =sSpellMgr->GetSpellInfo(DIMENSION_SPELL_MARK+i);
        if (spellInfo) {
            uint32 phase=spellInfo->Effects[0].MiscValue;
            phaseMap[phase]=PhaseDimensionsEnum(i);
        }
    }
}

AzthUtils::~AzthUtils()
{
}

time_t AzthUtils::getStartsOfYear() {
    time_t now = time(0);
    struct tm * tnow = std::gmtime(&now);
    int currentYear = tnow->tm_year + 1900;
    // is it valid until 2100?
    int epochYear = currentYear - 1970;
    int leapYears = (epochYear + 1) / 4;
    time_t result = epochYear * 24 * 60 * 60 * 365;
    // valid until 2106
    result += leapYears * 24 * 60 * 60;

    return result;
}

void AzthUtils::sendMessageToGroup(Player *pl, Group *group, const char* msg) {
    if (!group) {
        // fallback
        if (pl)
            ChatHandler(pl->GetSession()).SendSysMessage(msg);
        return;
    }

    for (GroupReference* itr = group->GetFirstMember(); itr != NULL; itr = itr->next())
    {
        Player* player = itr->GetSource();
        if (!player)
            continue;

        ChatHandler(player->GetSession()).SendSysMessage(msg);
    }
}

void AzthUtils::loadClassSpells() {
    QueryResult res = WorldDatabase.PQuery("SELECT racemask, classmask, Spell FROM playercreateinfo_spell_custom;");

    if (!res)
    {
        return;
    }


    do
    {
        Field* fields = res->Fetch();
        uint32 raceMask = fields[0].GetUInt32();
        uint32 classMask = fields[1].GetUInt32();
        uint32 spellId = fields[2].GetUInt32();

        if (raceMask != 0 && !(raceMask & RACEMASK_ALL_PLAYABLE))
        {
            sLog->outErrorDb("Wrong race mask %u in `playercreateinfo_spell_custom` table, ignoring.", raceMask);
            continue;
        }

        if (classMask != 0 && !(classMask & CLASSMASK_ALL_PLAYABLE))
        {
            sLog->outErrorDb("Wrong class mask %u in `playercreateinfo_spell_custom` table, ignoring.", classMask);
            continue;
        }


        for (uint32 raceIndex = RACE_HUMAN; raceIndex < MAX_RACES; ++raceIndex)
        {
            if (raceMask == 0 || ((1 << (raceIndex - 1)) & raceMask))
            {
                for (uint32 classIndex = CLASS_WARRIOR; classIndex < MAX_CLASSES; ++classIndex)
                {
                    if (classMask == 0 || ((1 << (classIndex - 1)) & classMask))
                    {
                        startSpells[raceIndex][classIndex].push_back(spellId);
                    }
                }
            }
        }
    } while (res->NextRow());
}

void AzthUtils::learnClassSpells(Player* player, bool /*new_level*/)
{
    ChrClassesEntry const* classEntry = sChrClassesStore.LookupEntry(player->getClass());
    if (!classEntry)
        return;

    std::list<uint32> spells=startSpells[player->getRace(true)][player->getClass()];

    for (std::list<uint32>::iterator it = spells.begin(); it != spells.end(); ++it)
    {
        uint32 s = *it;
        if (!s)
            continue;

        SpellInfo const* spellInfo = sSpellMgr->GetSpellInfo(s);

        do {

            if (!spellInfo)
                continue;

            // skip wrong class/race skills
            //if (!player->IsSpellFitByClassAndRace(spellInfo->Id))
            //    continue;

            // skip other spell families
            //if (spellInfo->SpellFamilyName != family)
            //    continue;

            // skip spells with first rank learned as talent (and all talents then also)
            uint32 firstRank = sSpellMgr->GetFirstSpellInChain(spellInfo->Id);
            if (GetTalentSpellCost(firstRank) > 0)
                continue;

            // skip broken spells
            if (!SpellMgr::IsSpellValid(spellInfo))
                continue;

            player->learnSpell(spellInfo->Id);

        } while((spellInfo = spellInfo->GetPrevRankSpell())); // learn prev ranks
    }
}


uint32 AzthUtils::selectCorrectSpellRank(uint8 level, uint32 spellId) {
    SpellInfo const* spellInfo = sSpellMgr->GetSpellInfo(spellId);

    if (!spellInfo)
        return spellId; // should never happen

    // spells that must not scale
    /*switch (spellId) {
        case 9634: // dire bear form, scaling it cause visual issue on spell button (unfortunately it's scaled anyway, event with this check)
            return spellId;
    }*/

    SpellInfo const* lastFoundSpell;
    for (SpellInfo const* nextSpellInfo = spellInfo; nextSpellInfo != NULL; nextSpellInfo = nextSpellInfo->GetPrevRankSpell())
    {
        lastFoundSpell = nextSpellInfo;
        if ((nextSpellInfo->SpellLevel == 0 && uint32(level) >= nextSpellInfo->BaseLevel) || uint32(level) >= nextSpellInfo->SpellLevel)
                return nextSpellInfo->Id;
    }

    return lastFoundSpell ? lastFoundSpell->Id : spellId; // return last rank found  // return 0 <- informing about not scaled spell
}

uint32 AzthUtils::getCalcReqLevel(ItemTemplate const* pProto) {
    if (pProto->RequiredLevel <= 1 && pProto->ItemLevel >= 6) { // zones >= lvl 5
        // classic rare and lower
        if (pProto->ItemLevel <= 60)
            return pProto->ItemLevel;

        // classic epic
        if (pProto->ItemLevel > 60 && pProto->ItemLevel <= 92 && pProto->Quality >= ITEM_QUALITY_EPIC) {
            return 60;
        }

        // classic / dk
        if (pProto->ItemLevel > 60 && pProto->ItemLevel <= 81) {
            return 60;
        }

        // tbc low level items
        if (pProto->ItemLevel > 81 && pProto->ItemLevel <= 92) {
            return 60;
        }

        // tbc rare and lower
        if (pProto->ItemLevel > 92 && pProto->ItemLevel <= 115) {
            uint32 req = ((pProto->ItemLevel - 92) / 2) + 61;
            return req > 70 ? 70 : req;
        }

        // tbc epic
        if (pProto->ItemLevel > 115 && pProto->ItemLevel <= 164 && pProto->Quality >= ITEM_QUALITY_EPIC) {
            uint32 req = ((pProto->ItemLevel - 115) / 4) + 61;
            return req > 70 ? 70 : req;
        }

        if (pProto->ItemLevel == AZTH_TW_ILVL_NORMAL_ONLY)
            return 1;

        //if (pProto->ItemLevel > 130 && pProto->ItemLevel <= 284)
        //    return 80;

        // 130 - 284
        return sWorld->getIntConfig(CONFIG_MAX_PLAYER_LEVEL);
    }

    if (pProto->RequiredLevel == 0)
        return 1;

    return pProto->RequiredLevel;
}

float AzthUtils::getCustomMultiplier(ItemTemplate const * pProto,uint32 multiplier) {
    uint32 req=getCalcReqLevel(pProto);
    uint32 maxLevel = sWorld->getIntConfig(CONFIG_MAX_PLAYER_LEVEL);
    if (req >= maxLevel) {
        return multiplier;
    }
    else {
        // ex: 131 ( boost ) / ( 80 - 60 )
        return ( (float)multiplier / float( maxLevel - req ) ) * 5;
    }
}

int32 AzthUtils::normalizeFeralAp(int32 feralBonus, int32 extraDPS, ItemTemplate const* pProto, bool isScaling) {
    if (!feralBonus)
        return 0;

    if (!isScaling)
        return feralBonus;

    float dps;
    if (pProto->Delay == 0)
        return 0;
    float temp = 0;

    if (pProto->Damage[0].DamageMin > 1 || pProto->Damage[0].DamageMax > 1
        || pProto->Damage[1].DamageMin > 1 || pProto->Damage[1].DamageMax > 1) {
        // heirloom items (isScaling==true) must have damage min = 1 and damage max = 0
        // if it's not true, then it's a normal item scaled by our TW system (?)
        // then we have to recalculate Feral AP using modified formula of item prototype (getDPS)

        temp+=1 + 0; // min 1 , max 0
        dps = temp*500/pProto->Delay;

        int32 bonus = int32((extraDPS + dps)*14.0f) - 767;
        if (bonus < 0)
            return 0;

        return bonus;
    }

    return feralBonus;
}

uint32 AzthUtils::calculateItemScalingValue(ItemTemplate const * pProto, Player *pl)
{
    if (pl == nullptr)
        return 0;

    uint32 req=getCalcReqLevel(pProto);
    if (req <= pl->getLevel()) // remove / apply
        return 0;

    uint8 lowLevel = 0; // Default stats progression by item type

    /* Mul values
     * 1 Normal stats progression
       2 Trinket stats progression (only seen on trinkets)
       4 Low stats progression
       8 Boosted stats progression
       16 Very low stats progression
     */

    /*if (pl->getLevel() + 10 > req) {        // from 0 to 9 level diff
        lowLevel = 0;  // Default stats progression by item type
    } else if (pl->getLevel() + 20 >= req) {  // from 10 to 20 level diff
        lowLevel = 4;  // Low Stats progression
    } else {*/                                // from 21 to max level diff
        //lowLevel = 16; // Very low stats progression
    //}
    uint32 mul;

    // SHOULDERS
    mul = lowLevel ? lowLevel :  1;
    if (pProto->InventoryType == INVTYPE_SHOULDERS) {
        if (pProto->Class == ITEM_CLASS_ARMOR) {
            switch (pProto->SubClass) {
            case ITEM_SUBCLASS_ARMOR_CLOTH :
                return mul + 32; //return 1 + 32;
            case ITEM_SUBCLASS_ARMOR_LEATHER :
                return mul + 64; //return 1 + 64;
            case ITEM_SUBCLASS_ARMOR_MAIL :
                return mul + 128; //return 1 + 128;
            case ITEM_SUBCLASS_ARMOR_PLATE :
                return mul + 128; //return 1 + 256;
            }
        }

        return 0;
    }

    mul = lowLevel ? lowLevel :  8;
    if (pProto->InventoryType == INVTYPE_CHEST || pProto->InventoryType == INVTYPE_ROBE) {
        switch (pProto->SubClass) {
        case ITEM_SUBCLASS_ARMOR_CLOTH:
            return mul + 1048576; //return 8 + 1048576;
        case ITEM_SUBCLASS_ARMOR_LEATHER:
            return mul + 2097152; //return 8 + 2097152;
        case ITEM_SUBCLASS_ARMOR_MAIL :
            return mul + 4194304; // return 8 + 4194304;
        case ITEM_SUBCLASS_ARMOR_PLATE:
            return mul + 8388608;  //return 8 + 8388608;
        }

        return 0;
    }

    // WEAPONS + 512 to 16384 <-- this is the dps modifiers ( getDPSMod ) , 32768 <-- spell power modifier, 65536 <-- it's for feral dps (unused?)
    if (pProto->InventoryType == INVTYPE_2HWEAPON) {
        bool isCaster=false;
        for (uint8 i = 0; i < MAX_ITEM_PROTO_STATS; ++i)
        {
            if (i >= pProto->StatsCount)
                break;

            // not accurate but mostly
            if (pProto->ItemStat[i].ItemStatType == ITEM_MOD_SPELL_POWER || pProto->ItemStat[i].ItemStatType == ITEM_MOD_SPELL_PENETRATION
                || pProto->ItemStat[i].ItemStatType == ITEM_MOD_HASTE_SPELL_RATING || pProto->ItemStat[i].ItemStatType == ITEM_MOD_CRIT_SPELL_RATING
                || pProto->ItemStat[i].ItemStatType == ITEM_MOD_INTELLECT || pProto->ItemStat[i].ItemStatType == ITEM_MOD_SPIRIT
            )
                isCaster=true;
        }

        mul = lowLevel ? lowLevel :  8;
        if ( isCaster ) {
            return mul + 4096 + 32768; // return 8 + 4096 + 32768;  // 2h dps caster
        } else {
            return mul + 1024;         // return 8 + 1024;          // 2h weapon
        }
    }

    if (pProto->InventoryType == INVTYPE_WEAPON || pProto->InventoryType == INVTYPE_WEAPONMAINHAND
        || pProto->InventoryType == INVTYPE_WEAPONOFFHAND) {
        bool isCaster=false;
        for (uint8 i = 0; i < MAX_ITEM_PROTO_STATS; ++i)
        {
            if (i >= pProto->StatsCount)
                break;

            // not accurate but mostly
            if (pProto->ItemStat[i].ItemStatType == ITEM_MOD_SPELL_POWER || pProto->ItemStat[i].ItemStatType == ITEM_MOD_SPELL_PENETRATION
                || pProto->ItemStat[i].ItemStatType == ITEM_MOD_HASTE_SPELL_RATING || pProto->ItemStat[i].ItemStatType == ITEM_MOD_CRIT_SPELL_RATING
                || pProto->ItemStat[i].ItemStatType == ITEM_MOD_INTELLECT || pProto->ItemStat[i].ItemStatType == ITEM_MOD_SPIRIT
            )
                isCaster=true;
        }

        mul = lowLevel ? lowLevel :  8;
        if (isCaster) {
            return mul + 2048 + 32768; // return 8 + 2048 + 32768;  // 1h dps caster
        } else {
            return mul + 512;          // return 8 + 512;           // 1h weapon
        }
    }

    // RANGED
    if (pProto->InventoryType == INVTYPE_RANGED || pProto->InventoryType == INVTYPE_AMMO || pProto->InventoryType == INVTYPE_THROWN)
        return 16 + 8192; // should be 16 ?

    // WANDS
    if (pProto->InventoryType == INVTYPE_RANGEDRIGHT)
        return 16 + 16384; // should be 16 ?


    if (pProto->InventoryType == INVTYPE_TRINKET) {
        return 2; // should be 2 ?
    }


    if (pProto->InventoryType == INVTYPE_FINGER)
        return 16; // return 262144;


    if (pProto->InventoryType == INVTYPE_NECK)
        return 16; //return 262144;

    //
    // special unknown cases
    //

    mul = lowLevel ? lowLevel :  4;
    if (pProto->InventoryType == INVTYPE_HOLDABLE || pProto->InventoryType == INVTYPE_RELIC) {
        return mul; //return 4 + 512;
    }

    // CLOAK
    mul = lowLevel ? lowLevel :  4;
    if (pProto->InventoryType == INVTYPE_CLOAK) {
        return mul + 524288; //return 4 + 524288;
    }

    // ARMOR
    mul = lowLevel ? lowLevel :  4;
    if (pProto->InventoryType == INVTYPE_SHIELD) {
        return mul + 8388608; // return 4 + 8388608;
    }

    mul = lowLevel ? lowLevel :  4;
    if (pProto->Class == ITEM_CLASS_ARMOR) {
        switch (pProto->SubClass) {
        case ITEM_SUBCLASS_ARMOR_CLOTH:
            return mul + 1048576; // return 4 + 1048576;
        case ITEM_SUBCLASS_ARMOR_LEATHER:
            return mul + 2097152; //return 4 + 2097152;
        case ITEM_SUBCLASS_ARMOR_MAIL :
            return mul + 4194304; //return 4 + 4194304;
        case ITEM_SUBCLASS_ARMOR_PLATE:
            return mul + 8388608; //return 4 + 8388608;
        }
    }

    return 0;

}



std::vector<std::string> AzthUtils::getCategoryIconAndNameByItemType(uint32 itemType)
{
    std::vector<std::string> category;

    switch (itemType)
    {
    case INVTYPE_NON_EQUIP:
        category.push_back(INVTYPE_NON_EQUIP_STRING);
        category.push_back("INV_Misc_Questionmark");
        break;
    case INVTYPE_HEAD:
        category.push_back(INVTYPE_HEAD_STRING);
        category.push_back("INV_Helmet_12");
        break;
    case INVTYPE_NECK:
        category.push_back(INVTYPE_NECK_STRING);
        category.push_back("INV_Jewelry_Necklace_02");
        break;
    case INVTYPE_SHOULDERS:
        category.push_back(INVTYPE_SHOULDERS_STRING);
        category.push_back("INV_Shoulder_92");
        break;
    case INVTYPE_BODY:
        category.push_back(INVTYPE_BODY_STRING);
        category.push_back("INV_Shirt_04");
        break;
    case INVTYPE_CHEST:
        category.push_back(INVTYPE_CHEST_STRING);
        category.push_back("INV_Chest_Leather_05");
        break;
    case INVTYPE_WAIST:
        category.push_back(INVTYPE_WAIST_STRING);
        category.push_back("INV_Belt_45b");
        break;
    case INVTYPE_LEGS:
        category.push_back(INVTYPE_LEGS_STRING);
        category.push_back("INV_Pants_Cloth_10");
        break;
    case INVTYPE_FEET:
        category.push_back(INVTYPE_FEET_STRING);
        category.push_back("INV_Boots_02");
        break;
    case INVTYPE_WRISTS:
        category.push_back(INVTYPE_WRISTS_STRING);
        category.push_back("INV_Bracer_21c");
        break;
    case INVTYPE_HANDS:
        category.push_back(INVTYPE_HANDS_STRING);
        category.push_back("INV_Gauntlets_12");
        break;
    case INVTYPE_FINGER:
        category.push_back(INVTYPE_FINGER_STRING);
        category.push_back("INV_Jewelry_Talisman_08");
        break;
    case INVTYPE_TRINKET:
        category.push_back(INVTYPE_TRINKET_STRING);
        category.push_back("INV_Jewelry_Trinket_04");
        break;
    case INVTYPE_WEAPON:
        category.push_back(INVTYPE_WEAPON_STRING);
        category.push_back("INV_Weapon_Glave_01");
        break;
    case INVTYPE_SHIELD:
        category.push_back(INVTYPE_SHIELD_STRING);
        category.push_back("INV_Shield_05");
        break;
    case INVTYPE_RANGED:
        category.push_back(INVTYPE_RANGED_STRING);
        category.push_back("INV_Weapon_Crossbow_23");
        break;
    case INVTYPE_CLOAK:
        category.push_back(INVTYPE_CLOAK_STRING);
        category.push_back("INV_Misc_Cape_18");
        break;
    case INVTYPE_2HWEAPON:
        category.push_back(INVTYPE_2HWEAPON_STRING);
        category.push_back("INV_Mace_72");
        break;
    case INVTYPE_BAG:
        category.push_back(INVTYPE_BAG_STRING);
        category.push_back("INV_Misc_Bag_09");
        break;
    case INVTYPE_TABARD:
        category.push_back(INVTYPE_TABARD_STRING);
        category.push_back("INV_Shirt_Guildtabard_01");
        break;
    case INVTYPE_ROBE:
        category.push_back(INVTYPE_ROBE_STRING);
        category.push_back("INV_Robe_Emberweavegown_a_01");
        break;
    case INVTYPE_WEAPONMAINHAND:
        category.push_back(INVTYPE_WEAPONMAINHAND_STRING);
        category.push_back("INV_Mace_48");
        break;
    case INVTYPE_WEAPONOFFHAND:
        category.push_back(INVTYPE_WEAPONOFFHAND_STRING);
        category.push_back("INV_Offhand_Dalaran_d_01");
        break;
    case INVTYPE_HOLDABLE:
        category.push_back(INVTYPE_HOLDABLE_STRING);
        category.push_back("INV_Misc_Giftwrap_02");
        break;
    case INVTYPE_AMMO:
        category.push_back(INVTYPE_AMMO_STRING);
        category.push_back("INV_Misc_Ammo_Arrow_03");
        break;
    case INVTYPE_THROWN:
        category.push_back(INVTYPE_THROWN_STRING);
        category.push_back("INV_Throwingaxe_03");
        break;
    case INVTYPE_RANGEDRIGHT:
        category.push_back(INVTYPE_RANGEDRIGHT_STRING);
        category.push_back("INV_Weapon_Rifle_27");
        break;
    case INVTYPE_QUIVER:
        category.push_back(INVTYPE_QUIVER_STRING);
        category.push_back("INV_Misc_Quiver");
        break;
    case INVTYPE_RELIC:
        category.push_back(INVTYPE_RELIC_STRING);
        category.push_back("INV_Relics_Libramofgrace");
        break;
    }

    return category;
}

std::string AzthUtils::GetItemIcon(uint32 entry, uint32 width, uint32 height, int x, int y)
{
    std::ostringstream ss;
    ss << "|TInterface";
    const ItemTemplate* temp = sObjectMgr->GetItemTemplate(entry);
    const ItemDisplayInfoEntry* dispInfo = NULL;
    if (temp)
    {
        dispInfo = sItemDisplayInfoStore.LookupEntry(temp->DisplayInfoID);
        if (dispInfo)
            ss << "/ICONS/" << dispInfo->inventoryIcon;
    }
    if (!dispInfo)
        ss << "/InventoryItems/WoWUnknownItem01";
    ss << ":" << width << ":" << height << ":" << x << ":" << y << "|t";
    return ss.str();
}


std::string AzthUtils::getDimensionName(uint32 dim) {
    std::string name = "";
    switch (dim) {
        case DIMENSION_NORMAL:
            name += "Illusory World";
        break;
        case DIMENSION_GUILD:
            name += "Guild World";
        break;
        case DIMENSION_PVP:
            name += "Corrupted World";
        break;
        case DIMENSION_ENTERTAINMENT:
            name += "Toys World";
        break;
        case DIMENSION_RPG:
            name += "Warcraft Tales";
        break;
        case DIMENSION_60:
            name += "Classic World";
        break;
        case DIMENSION_70:
            name += "TBC World";
        break;
        break;
        case DIMENSION_TEST:
            name += "Test World";
        break;
        case DIMENSION_GM:
            name += "GameMaster World";
        break;
        default:
            name += "Unknown";
        break;
    }

    return name;
}

bool AzthUtils::dimIntegrityCheck(Unit const* target, uint32 phasemask)
{
    if (target && target->GetTypeId() == TYPEID_PLAYER)
    {
        Player const* player = target->ToPlayer();

        if (player)
        {
            PhaseDimensionsEnum dim=getCurrentDimensionByPhase(phasemask);

            // If target has temporary normal dimension
            // we must consider as it's in normal phase
            // avoiding dimension changing from <temp normal> to <normal>,
            // if we're summoning a player from instance to instance for example.
            // Of course if we're summoning an instanced player with <temp normal> dimension
            // outside of an instance on <normal dimension>,
            // then the map changing check will restore the correct dimension
            //
            // Moreover checking aura instead phase, if it differ by dim
            // then the changeDimension can fix eventually exploits
            // with phased players without correct aura
            uint32 targetDim = sAZTH->GetAZTHPlayer((Player*)player)->getCurrentDimensionByAura();

            if (isPhasedDimension(dim) && dim != targetDim)
                return sAZTH->GetAZTHPlayer((Player*)player)->changeDimension(dim, true);
        }
    }

    return true;
}

bool AzthUtils::isPhasedDimension(uint32 dim) {
    return dim > DIMENSION_NORMAL;
}

bool AzthUtils::isSharedArea(Player* /*player*/, MapEntry const *mEntry, uint32 zone, uint32 /*area*/) {
    return
    mEntry->IsBattlegroundOrArena() // all bg and arena
    || mEntry->IsDungeon()          // is dungeon
    || mEntry->IsRaid()             // is raid
    || zone == BATTLEFIELD_WG_ZONEID; // WG must be shared mainly because it's an open world BG, then because it apply phase 1 + horde/ally phase...so can collide
}

PhaseDimensionsEnum AzthUtils::getCurrentDimensionByPhase(uint32 phase) {
    // some aura phases include 1 normal map in addition to phase itself
    // we should catch also characters that, for some odd reason, have
    // special dimension + normal (shouldn't happen)
    phase = phase & ~PHASEMASK_NORMAL;

    if (phase>0) {
        if (phaseMap.find(phase) != phaseMap.end())
            return phaseMap[phase];
    }

    return DIMENSION_NORMAL;
}


bool AzthUtils::checkItemLvL(ItemTemplate const* proto,uint32 level) {
    // return false if item level > current season
    return !(proto->InventoryType > 0 && proto->InventoryType != INVTYPE_AMMO && proto->ItemLevel > level);
}


int AzthUtils::getReaction(Unit const* unit, Unit const* target) {
    Player const* selfPlayerOwner = unit->GetAffectingPlayer();
    Player const* targetPlayerOwner = target->GetAffectingPlayer();

    uint32 dimUnit=selfPlayerOwner ? sAZTH->GetAZTHPlayer((Player*)selfPlayerOwner)->getCurrentDimensionByPhase() : getCurrentDimensionByPhase(unit->GetPhaseMask());
    uint32 dimTarget=targetPlayerOwner ? sAZTH->GetAZTHPlayer((Player*)targetPlayerOwner)->getCurrentDimensionByPhase() : getCurrentDimensionByPhase(target->GetPhaseMask());

    // case of 2 players
    if (selfPlayerOwner && targetPlayerOwner) {
        // before guild check
        if (sAZTH->GetAZTHPlayer((Player*)selfPlayerOwner)->isInBlackMarket() && sAZTH->GetAZTHPlayer((Player*)targetPlayerOwner)->isInBlackMarket())
            return REP_FRIENDLY;

        if (dimUnit == DIMENSION_PVP && dimTarget == DIMENSION_PVP) {
            return REP_HOSTILE; // fight against everyone on pvp dimension (maybe we must allow it only in contested territory?)
        }
    }

    if (dimUnit== DIMENSION_GUILD && dimTarget == DIMENSION_GUILD) {
        GH_unit *su = nullptr;
        if (!selfPlayerOwner) {
            su = GHobj.GetUnitByGuid(unit->GetGUID().GetCounter());

            if (!su)
                return -1;

            if (su->type == NPC_DUMMY)
                return REP_HOSTILE;
        }

        GH_unit *tu = nullptr;
        if (!targetPlayerOwner) {
            tu = GHobj.GetUnitByGuid(target->GetGUID().GetCounter());

            if (!tu)
                return -1;

            if (tu->type == NPC_DUMMY)
                return REP_HOSTILE;
        }

        uint32 guildUnit=selfPlayerOwner ? selfPlayerOwner->GetGuildId() : su->guild;
        uint32 guildTarget=targetPlayerOwner ? targetPlayerOwner->GetGuildId() : tu->guild;

        // it can happen only when the unit is a creature and
        // not assigned to any guild (blackmarket NPCs for example)
        // A player with guild = 0 shouldn't be allowed to enter
        // this dimension and there are checks about it in other places
        if (!guildTarget || !guildUnit) {
            // normal creature (not assigned to any guild)
            // get the default reaction
            return -1;
        }

        return guildUnit != guildTarget ? REP_HOSTILE : REP_FRIENDLY;
    }

    return -1;
}

bool AzthUtils::canFly(Unit*const /*caster*/, Unit* originalCaster)
{
    if (originalCaster && originalCaster->GetTypeId() == TYPEID_PLAYER)
    {
        uint32 curDim = sAZTH->GetAZTHPlayer((Player*)originalCaster)->getCurrentDimensionByAura();

        if (curDim == DIMENSION_TEST || curDim == DIMENSION_GM)
            return true;
    }

    // return false to continue with other checks
    return false;
}

SpellCastResult AzthUtils::checkSpellCast(Player* player, SpellInfo const* spell, bool notify)
{
    // Great feast and Fish Feast must be blocked for level < 70
    // they have been fixed in cataclysm but on wotlk it can be used
    // as exploit with low level players
    if (player->getLevel() < 70 && (
        spell->Id == 57399 // Well Fed - Fish Feast
        || spell->Id == 57294 // Well Fed - Great Feast
    )) {
        return SPELL_FAILED_LOWLEVEL;
    }

    if (sAZTH->GetAZTHPlayer(player)->isTimeWalking(true) && sAzthUtils->isNotAllowedSpellForTw(spell)) {
        if (notify)
            player->GetSession()->SendNotification("This spell is not allowed in Timewalking");
        return SPELL_FAILED_DONT_REPORT;
    }

    return SPELL_CAST_OK;
}

bool AzthUtils::canPrepareSpell(Spell* /*spell*/, Unit* /*m_caster*/, SpellInfo const* m_spellInfo, SpellCastTargets const* targets, AuraEffect const* /*triggeredByAura*/) {
    // naxxramas teleport disabled when timewalking
    Unit* target = targets->GetUnitTarget();
    //Player *player;

    if (!target)
        return true;

    if (!m_spellInfo)
        return true;

    /*if (target->GetTypeId() == TYPEID_PLAYER)
        player = target->ToPlayer();

    if (m_spellInfo->Id == 72613  && player && sAZTH->GetAZTHPlayer(player)->GetTimeWalkingLevel() == TIMEWALKING_LVL_NAXX) {
        if (InstanceScript* iscript=player->GetInstanceScript()) {
            if (
                iscript->GetBossState(BOSS_MAEXXNA) == DONE &&
                iscript->GetBossState(BOSS_LOATHEB) == DONE &&
                iscript->GetBossState(BOSS_HORSEMAN) == DONE &&
                iscript->GetBossState(BOSS_THADDIUS) == DONE
            ) {
                player->GetSession()->SendNotification("TIMEWALKING: You cannot be teleported! You must kill all bosses.");
                return true;
            } else {
                spell->SendCastResult(SPELL_FAILED_DONT_REPORT);
                return false;
            }
        }
    }*/

    return true;
}

void AzthUtils::onAuraRemove(AuraApplication * aurApp, AuraRemoveMode /*mode*/) {
    Aura* aura = aurApp->GetBase();
    if (aura->GetSpellInfo()->Id == AZTH_RIDE_VEHICLE_SPELL || aura->GetSpellInfo()->Id == AZTH_RIDE_VEHICLE_FLY_SPELL ||
        aura->GetSpellInfo()->Id == AZTH_RIDE_MOUNT_SPELL   || aura->GetSpellInfo()->Id == AZTH_RIDE_MOUNT_FLY_SPELL
    ) {
        Unit *caster = aura->GetCaster();
        if (caster->GetTypeId() == TYPEID_PLAYER) {
            Player *player = caster->ToPlayer();
            if (player) {
                if (player->IsMounted())
                    player->Dismount();

                if (player->m_mover != player && player->IsOnVehicle(player->m_mover))
                    player->ExitVehicle();
            }
        }
    }
}

uint32 AzthUtils::getPositionLevel(bool includeSpecialLvl, Map *map, uint32 zone, uint32 area) const {

    uint32 level=0;

    if (includeSpecialLvl) {
        switch(map->GetId()) {
            case NAXXRAMS_RAID:
                level = TIMEWALKING_LVL_NAXX; // keep old TW level for now
            break;
            case OBSIDIAN_RAID:
                level = TIMEWALKING_LVL_VAS_LVL13;
            break;
            case THE_EYE_OF_ETERNITY_RAID:
                level = TIMEWALKING_LVL_VAS_LVL13;
            break;
            case ULDUAR_RAID:
                level = TIMEWALKING_LVL_VAS_LVL4;
                break;
            case TRIAL_OF_THE_CRUSADRE_RAID:
                level = TIMEWALKING_LVL_VAS_LVL4;
            break;
        }
    }

    // before area table because more accurate in dungeon case
    if (!level) {
        LFGDungeonEntry const* dungeon = GetLFGDungeon(map->GetId(), map->GetDifficulty());
        if (dungeon && (map->IsDungeon() || map->IsRaid())) {
            level  = dungeon->recminlevel ? dungeon->recminlevel : ( dungeon->reclevel ?  dungeon->reclevel : dungeon->minlevel);
        }
    }

    if (!level) {
        DungeonProgressionRequirements const* ar=sObjectMgr->GetAccessRequirement(map->GetId(), map->GetDifficulty());
        if (ar)
            level = ar->levelMin;
    }

    if (!level)
    {
        AreaTableEntry const* areaEntry = sAreaTableStore.LookupEntry(area);
        if (areaEntry && areaEntry->area_level > 0)
            level = areaEntry->area_level;
    }

    if (!level) {
        // some area entry doesn't have the area_level defined, so we can try
        // a workaround with LFGDungeon list on outworld zones
        MapEntry const* mapEntry = sMapStore.LookupEntry(map->GetId());
        AreaTableEntry const* zoneEntry = sAreaTableStore.LookupEntry(zone);
        AreaTableEntry const* areaEntry = sAreaTableStore.LookupEntry(area);

        std::string mapName  = mapEntry ? mapEntry->name[0] : "";
        std::string zoneName = zoneEntry ? zoneEntry->area_name[0] : "";
        std::string areaName = areaEntry ? areaEntry->area_name[0] : "";

        if (mapName != "" || zoneName != "" || areaName != "") {
            for (uint32 i = 0; i < sLFGDungeonStore.GetNumRows(); ++i)
            {
                LFGDungeonEntry const* dungeon = sLFGDungeonStore.LookupEntry(i);
                if (!dungeon)
                    continue;

                if (areaName == dungeon->name[0] || zoneName == dungeon->name[0] || mapName == dungeon->name[0]) {
                    level  = dungeon->recminlevel ? dungeon->recminlevel : ( dungeon->reclevel ?  dungeon->reclevel : (dungeon->maxlevel+dungeon->minlevel)/2);
                    break;
                }
            }
        }
    }

    return level;
}

uint32 AzthUtils::getPositionLevel(bool includeSpecialLvl, Map *map, WorldLocation const& posInfo) const {
    uint32 zoneid,areaid;
    map->GetZoneAndAreaId(zoneid, areaid, posInfo.GetPositionX(), posInfo.GetPositionY(), posInfo.GetPositionY());
    return getPositionLevel(includeSpecialLvl, map, zoneid, areaid);
}

// not count special bags
uint32 AzthUtils::getFreeSpaceInBags(Player *player) {
    uint32 count=0;

    //main bag
    for (uint8 i = INVENTORY_SLOT_ITEM_START; i < INVENTORY_SLOT_ITEM_END; i++) {
        Item* pItem = player->GetItemByPos(INVENTORY_SLOT_BAG_0, i);
        if (!pItem)
            count++;
    }

    // other bags
    for (uint8 i = INVENTORY_SLOT_BAG_START; i < INVENTORY_SLOT_BAG_END; i++) {
        if (Bag* pBag = player->GetBagByPos(i)) {
            switch (pBag->GetTemplate()->Class)
            {
                case ITEM_CLASS_CONTAINER:
                    switch (pBag->GetTemplate()->SubClass)
                    {
                        case ITEM_SUBCLASS_CONTAINER:
                            count+=pBag->GetFreeSlots();
                        break;
                    }
                break;
            }
        }
    }

    return count;
}


bool AzthUtils::isAshenBand(uint32 entry) {
    switch(entry) {
        //EXALTED id
        case 50398:
        case 52572:
        case 50402:
        case 50404:
        case 50400:

        //REVERED id
        case 50397:
        case 52571:
        case 50401:
        case 50403:
        case 50399:

        //HONORED id
        case 50384:
        case 52570:
        case 50387:
        case 50388:
        case 50386:

        //FRIENDLY id
        case 50377:
        case 52569:
        case 50376:
        case 50375:
        case 50378:
            return true;
    }

    return false;
}

bool AzthUtils::isMythicLevel(uint32 level)
{
    return level >= TIMEWALKING_LVL_VAS_START && level <= TIMEWALKING_LVL_VAS_END;
}

bool AzthUtils::isMythicCompLvl(uint32 reqLvl, uint32 checkLvl) {
    return isMythicLevel(reqLvl) && isMythicLevel(checkLvl) && reqLvl <= checkLvl;
}


bool AzthUtils::canMythicHere(Unit const* source) {
    if (!source->GetMap()->IsDungeon())
        return false;

    uint32 expansion = source->GetMap()->GetEntry()->Expansion();
    uint32 mapId=source->GetMap()->GetId();
    uint32 posLvl = sAzthUtils->getPositionLevel(false, source->GetMap(), source->GetZoneId(), source->GetAreaId());
    if ((posLvl > 70 || expansion >= 2)
        && mapId != 603 // Ulduar
        && mapId != 533 // Naxxramas
        && mapId != 616 // The Eye of Eternity
        && mapId != 615 // Obsidian Sanctum
        && mapId != 249 // Onyxia's Lair
    )
        return false;

    return true;
}
