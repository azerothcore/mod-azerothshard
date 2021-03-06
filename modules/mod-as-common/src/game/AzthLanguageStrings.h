#ifndef AZTHLANGUAGE_STRINGS_H
#define AZTHLANGUAGE_STRINGS_H

#include "AzthLanguage.h" // including LanguageStrings.h will include the system too

enum AzthLangStrEnum 
{
    AZTH_LANG_REMOVED_ITEMS = 0,
    AZTH_LANG_PVP_NPC_SET_ADVICE,
    AZTH_LANG_PVP_NPC_CANNOT_EQUIP,
    AZTH_LANG_RESET_AURAS_ADVICE,
    AZTH_LANG_SHOW_BANK_ITEMS,
    AZTH_LANG_INSTANCE_ONLY,
    AZTH_LANG_RAID_COMBAT,
    AZTH_LANG_GROUP_LEVEL_REG,
    AZTH_LANG_INSTANCE_LEVEL_REG,
    //collection npc
    AZTH_LANG_COLLECTION_DEPOSIT_ITEMS,
    AZTH_LANG_COLLECTION_SEARCH_ITEMS,
    AZTH_LANG_COLLECTION_SEARCH_ITEMS_CODE,
    AZTH_LANG_COLLECTION_ITEM_NOT_RECOVERED,
    AZTH_LANG_COLLECTION_ITEM_NOT_VALID,
    AZTH_LANG_COLLECTION_ITEM_WITH_DURATION,
    AZTH_LANG_COLLECTION_ITEM_NOT_EMPTY_BAG,
    AZTH_LANG_COLLECTION_ITEM_NOT_STACKED,
    AZTH_LANG_COLLECTION_ITEM_ASHEN_BAND,
    AZTH_LANG_COLLECTION_ITEM_ALREADY_STORED,
    AZTH_LANG_COLLECTION_ITEM_STORED,
    //common
    AZTH_LANG_COMMON_TEMPORARILY,
    AZTH_LANG_COMMON_NOTVALIDPARAMTER,
    AZTH_LANG_CHECK_EMAIL,
    AZTH_LANG_BAG_FULL_SENT_TO_MAIL,
    //ss
    AZTH_LANG_SS_README,
    AZTH_LANG_SS_TELEPORT_BACK,
    AZTH_LANG_SS_TELEPORT_DISABLED,
    AZTH_LANG_SS_BACK,
    AZTH_LANG_SS_VALUE,
    AZTH_LANG_SS_POSSES_CHECK,
    AZTH_LANG_SS_NEWAPP,
    AZTH_LANG_SS_CHANGE_RACE,
    AZTH_LANG_SS_CHANGE_FACTION,
    AZTH_LANG_SS_CHANGE_NAME,
    //hs
    AZTH_LANG_HS_QUESTS,
    AZTH_LANG_HS_PVP_QUEST,
    AZTH_LANG_HS_DAILY_QUEST,
    AZTH_LANG_HS_WEEKLY_QUEST,
    AZTH_LANG_HS_TW_QUESTS,
    AZTH_LANG_HS_TW_WEEKLY_QUEST,
    AZTH_LANG_HS_TW_WEEKLY_RANDOM_QUEST,
    AZTH_LANG_HS_TW_DAILY_QUEST,
    AZTH_LANG_HS_TW_DAILY_RANDOM_QUEST,
    AZTH_LANG_HS_QUEST_LIMIT_REACHED,
    AZTH_LANG_HS_QUEST_LIMIT_SUFFIX,
    //multidimension
    AZTH_LANG_MULTIDIMENSION_CLASSIC_EXPCHECK,
    AZTH_LANG_MULTIDIMENSION_TBC_EXPCHECK,
    AZTH_LANG_MULTIDIMENSION_GUILD_GROUPCHECK,
    AZTH_LANG_MULTIDIMENSION_TW_CHECK60,
    AZTH_LANG_MULTIDIMENSION_TW_CHECK70,
    AZTH_LANG_MULTIDIMENSION_GUILD_ACCESSCHECK,
    AZTH_LANG_MULTIDIMENSION_GM_ACCESSCHECK,
    AZTH_LANG_MULTIDIMENSION_TEMP_TELEPORT,
    AZTH_LANG_MULTIDIMENSION_CANNOT_BE_SUMMONED,
    AZTH_LANG_MULTIDIMENSION_CANNOT_BE_SUMMONED_SUMMONER,
    //custom commands
    AZTH_LANG_CCOMMANDS_QUEST,
    AZTH_LANG_CCOMANDS_QUEST_HASBUG,
    AZTH_LANG_CCOMANDS_QUEST_HASBUGANDCOMPLETED,
    AZTH_LANG_CCOMANDS_QUEST_NOBUG,
    //season tournament
    AZTH_LANG_TOURNAMENT_LEVEL_TOOHIGH,
    AZTH_LANG_TOURNAMENT_LEVEL_ACTUAL,
    //pvp account
    AZTH_LANG_PVPACCOUNT_DUNGEON,
    //pvp items
    AZTH_LANG_PVPITEMS_LEVEL_CHECK,
    AZTH_LANG_PVPITEMS_MLEVEL_CHECK,
    AZTH_LANG_PVPITEMS_REMOVED,
    AZTH_LANG_PVPITEMS_FORCE_REMOVED,
    //timewalking & mythic
    AZTH_LANG_TW_LEVEL_MAX,
    AZTH_LANG_TW_MODE_ON,
    AZTH_LANG_TW_MODE_OFF,
    AZTH_LANG_TW_NEW_ITEM_OBTAINED,
    AZTH_LANG_TW_GROUP_CHECK_LEADER,
    AZTH_LANG_TW_GROUP_CHECK_PLAYER,
    AZTH_LANG_TW_MOA_BONUS,
    AZTH_LANG_TW_ASREP_BONUS,
    AZTH_LANG_TW_BOSS_KILLED,
    AZTH_LANG_TW_LAST_BOSS_KILLED,
    AZTH_LANG_TW_INSTANCE_COMPLETED,
    AZTH_LANG_TW_REP_REMOVED_KILL,
    AZTH_LANG_TW_REP_REMOVED_LOGOUT,
    //rating bonus
    AZTH_LANG_BR_ACTIVE_BONUS,
    AZTH_LANG_BR_NO_BONUS,
    //bg item level
    AZTH_LANG_BG_ITEM_LEVEL_RESET,
    AZTH_LANG_BG_ITEM_LEVEL_CHANGED,
    //donation vendor
    AZTH_LANG_DONATION_UPDATE_MENU,
    AZTH_LANG_DONATION_ABILITY,
    AZTH_LANG_DONATION_BACK,
    AZTH_LANG_DONATION_ITEM_RECEIVED,
    AZTH_LANG_DONATION_ITEM_NO_DB,
    // dimension npc
    AZTH_LANG_DIMENSION_CURRENT,
    AZTH_LANG_DIMENSION_ILLUSORY,
    AZTH_LANG_DIMENSION_GUILD,
    AZTH_LANG_DIMENSION_PVP,
    AZTH_LANG_DIMENSION_ENTERTAIN,
    AZTH_LANG_DIMENSION_GDR,
    AZTH_LANG_DIMENSION_LVL60,
    AZTH_LANG_DIMENSION_LVL70,
    AZTH_LANG_DIMENSION_TEST,
    AZTH_LANG_DIMENSION_GM
};

#endif
