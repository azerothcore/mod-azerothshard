#ifndef AZTHPLAYER_H
#define AZTHPLAYER_H

#include "AzthSmartStone.h"
#include "Config.h"
#include "Define.h"
#include "CustomRates.h"
#include "AzthSharedDefines.h"
#include <unordered_map>

struct SmartStoneCommand;

class CrossFaction;
class Player;
class SmartStone;
class WorldLocation;

enum AzthQueueMasks
{
    AZTH_QUEUE_BG           = 1,
    AZTH_QUEUE_ARENA        = 2,
    AZTH_QUEUE_BG_OR_ARENA  = 3, // mask BG+Arena
    AZTH_QUEUE_LFG          = 4
};

//[AZTH] PVP Rank Patch
enum HonorKillPvPRank
{
    HKRANK00,
    HKRANK01,
    HKRANK02,
    HKRANK03,
    HKRANK04,
    HKRANK05,
    HKRANK06,
    HKRANK07,
    HKRANK08,
    HKRANK09,
    HKRANK10,
    HKRANK11,
    HKRANK12,
    HKRANK13,
    HKRANK14,
    HKRANKMAX
};

struct WowarmoryFeedEntry
{
    uint32 guid;         // Player GUID
    time_t date;         // Log date
    uint32 type;         // TYPE_ACHIEVEMENT_FEED, TYPE_ITEM_FEED, TYPE_BOSS_FEED
    uint32 data;         // TYPE_ITEM_FEED: item_entry, TYPE_BOSS_FEED: creature_entry
    uint32 item_guid;    // Can be 0
    uint32 item_quality; // Can be 0
    uint8  difficulty;   // Can be 0
    int    counter;      // Can be 0
};

typedef std::vector<WowarmoryFeedEntry> WowarmoryFeeds;

// used for PLAYER__FIELD_KNOWN_TITLES field (uint64), (1<<bit_index) without (-1)
// can't use enum for uint64 values
#define PLAYER_TITLE_DISABLED              UI64LIT(0x0000000000000000)
#define PLAYER_TITLE_NONE                  UI64LIT(0x0000000000000001)
#define PLAYER_TITLE_PRIVATE               UI64LIT(0x0000000000000002) // 1
#define PLAYER_TITLE_CORPORAL              UI64LIT(0x0000000000000004) // 2
#define PLAYER_TITLE_SERGEANT_A            UI64LIT(0x0000000000000008) // 3
#define PLAYER_TITLE_MASTER_SERGEANT       UI64LIT(0x0000000000000010) // 4
#define PLAYER_TITLE_SERGEANT_MAJOR        UI64LIT(0x0000000000000020) // 5
#define PLAYER_TITLE_KNIGHT                UI64LIT(0x0000000000000040) // 6
#define PLAYER_TITLE_KNIGHT_LIEUTENANT     UI64LIT(0x0000000000000080) // 7
#define PLAYER_TITLE_KNIGHT_CAPTAIN        UI64LIT(0x0000000000000100) // 8
#define PLAYER_TITLE_KNIGHT_CHAMPION       UI64LIT(0x0000000000000200) // 9
#define PLAYER_TITLE_LIEUTENANT_COMMANDER  UI64LIT(0x0000000000000400) // 10
#define PLAYER_TITLE_COMMANDER             UI64LIT(0x0000000000000800) // 11
#define PLAYER_TITLE_MARSHAL               UI64LIT(0x0000000000001000) // 12
#define PLAYER_TITLE_FIELD_MARSHAL         UI64LIT(0x0000000000002000) // 13
#define PLAYER_TITLE_GRAND_MARSHAL         UI64LIT(0x0000000000004000) // 14
#define PLAYER_TITLE_SCOUT                 UI64LIT(0x0000000000008000) // 15
#define PLAYER_TITLE_GRUNT                 UI64LIT(0x0000000000010000) // 16
#define PLAYER_TITLE_SERGEANT_H            UI64LIT(0x0000000000020000) // 17
#define PLAYER_TITLE_SENIOR_SERGEANT       UI64LIT(0x0000000000040000) // 18
#define PLAYER_TITLE_FIRST_SERGEANT        UI64LIT(0x0000000000080000) // 19
#define PLAYER_TITLE_STONE_GUARD           UI64LIT(0x0000000000100000) // 20
#define PLAYER_TITLE_BLOOD_GUARD           UI64LIT(0x0000000000200000) // 21
#define PLAYER_TITLE_LEGIONNAIRE           UI64LIT(0x0000000000400000) // 22
#define PLAYER_TITLE_CENTURION             UI64LIT(0x0000000000800000) // 23
#define PLAYER_TITLE_CHAMPION              UI64LIT(0x0000000001000000) // 24
#define PLAYER_TITLE_LIEUTENANT_GENERAL    UI64LIT(0x0000000002000000) // 25
#define PLAYER_TITLE_GENERAL               UI64LIT(0x0000000004000000) // 26
#define PLAYER_TITLE_WARLORD               UI64LIT(0x0000000008000000) // 27
#define PLAYER_TITLE_HIGH_WARLORD          UI64LIT(0x0000000010000000) // 28
#define PLAYER_TITLE_GLADIATOR             UI64LIT(0x0000000020000000) // 29
#define PLAYER_TITLE_DUELIST               UI64LIT(0x0000000040000000) // 30
#define PLAYER_TITLE_RIVAL                 UI64LIT(0x0000000080000000) // 31
#define PLAYER_TITLE_CHALLENGER            UI64LIT(0x0000000100000000) // 32
#define PLAYER_TITLE_SCARAB_LORD           UI64LIT(0x0000000200000000) // 33
#define PLAYER_TITLE_CONQUEROR             UI64LIT(0x0000000400000000) // 34
#define PLAYER_TITLE_JUSTICAR              UI64LIT(0x0000000800000000) // 35
#define PLAYER_TITLE_CHAMPION_OF_THE_NAARU UI64LIT(0x0000001000000000) // 36
#define PLAYER_TITLE_MERCILESS_GLADIATOR   UI64LIT(0x0000002000000000) // 37
#define PLAYER_TITLE_OF_THE_SHATTERED_SUN  UI64LIT(0x0000004000000000) // 38
#define PLAYER_TITLE_HAND_OF_ADAL          UI64LIT(0x0000008000000000) // 39
#define PLAYER_TITLE_VENGEFUL_GLADIATOR    UI64LIT(0x0000010000000000) // 40

#define PLAYER_TITLE_MASK_ALLIANCE_PVP                                      \
    (PLAYER_TITLE_PRIVATE | PLAYER_TITLE_CORPORAL |                         \
      PLAYER_TITLE_SERGEANT_A | PLAYER_TITLE_MASTER_SERGEANT |              \
      PLAYER_TITLE_SERGEANT_MAJOR | PLAYER_TITLE_KNIGHT |                   \
      PLAYER_TITLE_KNIGHT_LIEUTENANT | PLAYER_TITLE_KNIGHT_CAPTAIN |        \
      PLAYER_TITLE_KNIGHT_CHAMPION | PLAYER_TITLE_LIEUTENANT_COMMANDER |    \
      PLAYER_TITLE_COMMANDER | PLAYER_TITLE_MARSHAL |                       \
      PLAYER_TITLE_FIELD_MARSHAL | PLAYER_TITLE_GRAND_MARSHAL)

#define PLAYER_TITLE_MASK_HORDE_PVP                             \
    (PLAYER_TITLE_SCOUT | PLAYER_TITLE_GRUNT |                  \
      PLAYER_TITLE_SERGEANT_H | PLAYER_TITLE_SENIOR_SERGEANT |  \
      PLAYER_TITLE_FIRST_SERGEANT | PLAYER_TITLE_STONE_GUARD |  \
      PLAYER_TITLE_BLOOD_GUARD | PLAYER_TITLE_LEGIONNAIRE |     \
      PLAYER_TITLE_CENTURION | PLAYER_TITLE_CHAMPION |          \
      PLAYER_TITLE_LIEUTENANT_GENERAL | PLAYER_TITLE_GENERAL |  \
      PLAYER_TITLE_WARLORD | PLAYER_TITLE_HIGH_WARLORD)

#define PLAYER_TITLE_MASK_ALL_PVP  \
    (PLAYER_TITLE_MASK_ALLIANCE_PVP | PLAYER_TITLE_MASK_HORDE_PVP)

class AzthPlayer
{
public:
    explicit AzthPlayer(Player* origin);
    ~AzthPlayer();

    struct AzthAchiData
    {
        uint32 level;
        uint32 levelParty;
        uint32 specialLevel;
    };

    typedef std::unordered_map<uint16 /*achiId*/, AzthAchiData /*data*/> CompletedAchievementMap;
    typedef std::unordered_map<uint16 /*critId*/, AzthAchiData /*data*/> CompletedCriteriaMap;
    typedef std::unordered_map<uint32 /*entry*/, uint32 /*guid*/> ItemInBankMap;
    std::vector<SmartStonePlayerCommand>& getSmartStoneCommands();
    std::map<uint32, WorldLocation> getLastPositionInfoFromDB();

    // WInfi: TODO private
    CompletedAchievementMap m_completed_achievement_map;
    CompletedCriteriaMap m_completed_criteria_map;

    void SetTempGear(bool hasGear);
    // bool hasGear();
    void SetPlayerQuestRate(float rate);

    /*
        Get player quest rate. If player hasn't set it before, returns server
        configuration

        @returns Quest Rate. If player set his own, return it; if he hasn't it
        returns server default
     */
    float GetPlayerQuestRate();

    // uint32 getArena1v1Info(uint8 type);
    // void setArena1v1Info(uint8 type, uint32 value);
    uint32 getArena3v3Info(uint8 type);
    void setArena3v3Info(uint8 type, uint32 value);

    void ForceKilledMonsterCredit(uint32 entry, uint64 guid);
    bool passHsChecks(Quest const* qInfo, uint32 entry, uint32& realEntry, ObjectGuid guid);
    time_t lastSent = time(nullptr);

    void addSmartStoneCommand(SmartStonePlayerCommand command, bool query);
    void addSmartStoneCommand(uint32 id, bool query, uint64 dateExpired, int32 charges);
    void removeSmartStoneCommand(SmartStonePlayerCommand command, bool query);
    void decreaseSmartStoneCommandCharges(uint32 id);
    bool BuySmartStoneCommand(ObjectGuid vendorguid, uint32 vendorslot, uint32 item, uint8 count, uint8 bag, uint8 slot);
    bool hasSmartStoneCommand(uint32 id);
    void loadAzthPlayerFromDB();
    bool AzthMaxPlayerSkill();
    bool AzthSelfChangeXp(float rate);
    WorldLocation& getLastPositionInfo(uint32 dimension);
    void setLastPositionInfo(uint32 dimension, WorldLocation posIinfo);
    bool isInBlackMarket();
    void saveLastPositionInfoToDB(Player* pl);
    bool isPvP();
    void loadPvPInfo();
    AzthCustomLangs getCustLang();
    void setCustLang(AzthCustomLangs code);
    bool isPvPFlagOn(bool def);
    bool isFFAPvPFlagOn(bool def);
    void AddBankItem(uint32 itemEntry, uint32 itemGuid);
    void DelBankItem(uint32 itemEntry);
    void SetBankItemsList(ItemInBankMap itemsInBankList);
    ItemInBankMap& GetBankItemsList();
    bool canEnterMap(MapEntry const* entry, InstanceTemplate const* instance = nullptr, bool loginCheck = false);
    bool canJoinQueue(AzthQueueMasks type);
    bool canGroup(Player* with = nullptr);
    bool canExplore();
    bool canCompleteCriteria(AchievementCriteriaEntry const* criteria);
    bool canEquipItem(ItemTemplate const* proto);
    bool checkItems(uint32 iLvlMax, uint8 type = 0);
    bool checkItem(ItemTemplate const* proto);
    uint32 getMaxItemLevelByStatus();

    //TIMEWALKING
    bool canUseItem(Item* item, bool notify = false);
    bool itemCheckReqLevel(ItemTemplate const* item, bool notify = false);
    uint32 getPStatsLevel(bool normalized = true, bool checkInstance = true, bool checkGroup = true);
    uint32 getGroupLevel(bool normalized = true, bool checkInstance = true);
    uint32 getInstanceLevel(bool normalized = true);
    uint32 normalizeLvl(uint32 level);
    uint32 getGroupSize(bool checkInstance = true);
    uint32 getInstanceSize();
    uint32 GetTimeWalkingLevel() const;
    bool isTimeWalking(bool skipSpecial = false) const;
    void SetTimeWalkingLevel(uint32 timeWalkingLevel, bool clearAuras = true, bool save = true, bool login = false);
    void prepareTwSpells(uint32 oldLevel);
    uint32 getTwItemLevel(uint32 twLevel);
    time_t autoScalingPending;

    //DIMENSIONS
    uint32 getCurrentDimensionByMark() const;
    uint32 getCurrentDimensionByPhase() const;
    uint32 getCurrentDimensionByAura() const;
    bool hasDimSpecificAura(uint32 dim);
    bool changeDimension(uint32 dim, bool validate = false, bool temp = false);

    // WInfi: TODO private
    uint32 instanceID;

    // World of Warcraft Armory Feeds
    void InitWowarmoryFeeds();
    void CreateWowarmoryFeed(uint32 type, uint32 data, uint32 item_guid, uint32 item_quality);

    // World of Warcraft Armory Feeds
    WowarmoryFeeds m_wowarmory_feeds;

private:
    Player* player;
    uint32 arena1v1Info[7]; // ARENA_TEAM_END
    uint32 arena3v3Info[7]; // ARENA_TEAM_END
    uint32 levelPlayer;
    uint32 tmpLevelPg;
    uint32 groupLevel;

    float playerQuestRate, maxQuestRate;
    bool m_isPvP;
    AzthCustomLangs customLang;
    uint32 timeWalkingLevel = 0;
    bool PVPTempGear = false;

    ItemInBankMap m_itemsInBank;
    std::vector<SmartStonePlayerCommand> smartStoneCommands;
    std::map<uint32, WorldLocation> lastPositionInfo;
};

#endif /* AZTHPLAYER_H */
