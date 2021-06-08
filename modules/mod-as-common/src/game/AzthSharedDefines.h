#ifndef SHAREDDEFINES_H
#define SHAREDDEFINES_H

#include "Object.h"

/*
enum AzthRbac {
    RBAC_PERM_COMMAND_QUESTCOMPLETER = 1000,
    RBAC_PERM_COMMAND_AZTH = 1001,
    RBAC_PERM_COMMAND_AZTH_MAXSKILL = 1002,
    RBAC_PERM_COMMAND_AZTH_XP = 1003,
    RBAC_PERM_PVP_CUSTOM = 1004 // Arena Spectator ADDON commands
};
 */

enum SkillSpells {
    ONE_HAND_AXES = 196,
    TWO_HAND_AXES = 197,
    ONE_HAND_MACES = 198,
    TWO_HAND_MACES = 199,
    POLEARMS = 200,
    ONE_HAND_SWORDS = 201,
    TWO_HAND_SWORDS = 202,
    STAVES = 227,
    BOWS = 264,
    GUNS = 266,
    DAGGERS = 1180,
    WANDS = 5009,
    CROSSBOWS = 5011,
    FIST_WEAPONS = 15590,
    THROWN = 2567,
};

enum AzthCustomLangs {
    AZTH_LOC_IT = 1,
    AZTH_LOC_EN = 0,
};

enum PhaseDimensionsEnum {
    DIMENSION_NORMAL        = 1,
    DIMENSION_GUILD         = 2,
    DIMENSION_PVP           = 3,
    DIMENSION_ENTERTAINMENT = 4,
    DIMENSION_RPG           = 5,
    DIMENSION_60            = 6,
    DIMENSION_70            = 7,
    DIMENSION_TEST          = 8,
    DIMENSION_GM            = 9
};

enum AzthCommonMaps {
    NAXXRAMS_RAID               = 533,
    ULDUAR_RAID                 = 603,
    OBSIDIAN_RAID               = 615,
    THE_EYE_OF_ETERNITY_RAID    = 616,
    TRIAL_OF_THE_CRUSADRE_RAID  = 649
};

#define MAX_DIMENSION 9

// instances
#define DATA_AZTH_HARD_MODE 10000

// Spells
#define AZTH_RIDE_VEHICLE_SPELL        1002000
#define AZTH_RIDE_VEHICLE_FLY_SPELL    1002004
#define AZTH_RIDE_MOUNT_SPELL          1002001
#define AZTH_RIDE_MOUNT_FLY_SPELL      1002002
#define DIMENSION_SPELL_MARK 1000000
#define AZTH_CUSTOM_SPELL_MIN 900000

// Items

// only usable in tw
#define AZTH_TW_ILVL              246
#define AZTH_TW_ILVL_NORMAL_ONLY  247
#define AZTH_TW_ILVL_SPECIAL_ONLY 248


#define AZTH_MARK_OF_AZEROTH 37711
#define AZTH_AS_REP 948

class AzthSharedDef {
public:
    static const WorldLocation dalaran;
    static const WorldLocation blackMarket;
};

#endif /* SHAREDDEFINES_H */

