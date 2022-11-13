/*

*/

#include "AzthPlayer.h"
#include "Chat.h"
#include "Define.h"
#include "ObjectAccessor.h"
#include "World.h"
#include "AzthLevelStat.h"
#include "Unit.h"
#include "Pet.h"
#include "Player.h"
#include "SpellAuras.h"
#include "AccountMgr.h"
#include "MapMgr.h"
#include "MapInstanced.h"
#include "InstanceScript.h"
#include "ArenaSeason.h"
#include "AzthAchievement.h"
#include "AzthUtils.h"
#include "LFGMgr.h"
#include "AzthFirstKills.h"
#include "AZTH.h"
#include <Entities/Player/PlayerStorage.cpp>

class Aura;

AzthPlayer::AzthPlayer(Player* origin)
{
    playerQuestRate = sWorld->getRate(RATE_XP_QUEST);
    player = origin;
    customLang = AZTH_LOC_EN;
    groupLevel = 0;
    tmpLevelPg = 0;
    std::fill_n(arena1v1Info, 7, 0);
    std::fill_n(arena3v3Info, 7, 0);
    autoScalingPending = 0;
    m_isPvP = false;

    m_itemsInBank.clear();
}

AzthPlayer::~AzthPlayer()
{
    m_itemsInBank.clear();
}

void AzthPlayer::InitWowarmoryFeeds()
{
    // Clear feeds
    m_wowarmory_feeds.clear();
}

void AzthPlayer::setCustLang(AzthCustomLangs code)
{
    this->customLang = code;
}

AzthCustomLangs AzthPlayer::getCustLang()
{
    return this->customLang;
}

bool AzthPlayer::AzthMaxPlayerSkill()
{
    Player* target = player;
    if (!target)
        return false;

    if (target->getLevel() < 80)
    {
        ChatHandler(player->GetSession()).PSendSysMessage(LANG_LEVEL_MINREQUIRED, 80);
        return false;
    }

    static const SkillSpells spells[] =
    {   ONE_HAND_AXES,
        TWO_HAND_AXES,
        ONE_HAND_MACES,
        TWO_HAND_MACES,
        POLEARMS,
        ONE_HAND_SWORDS,
        TWO_HAND_SWORDS,
        STAVES,
        BOWS,
        GUNS,
        DAGGERS,
        WANDS,
        CROSSBOWS,
        FIST_WEAPONS,
        THROWN
    };

    std::list<SkillSpells> learnList;
    std::size_t max = (sizeof(spells) / sizeof(spells[0]));

    for (std::size_t s = 0; s < max; s++)
    {
        SkillSpells spell = spells[s];
        switch (target->getClass())
        {
        case CLASS_WARRIOR:
            if (spell != WANDS)
                learnList.push_back(spell);
            break;
        case CLASS_DEATH_KNIGHT:
        case CLASS_PALADIN:
            if (spell != THROWN && spell != STAVES && spell != BOWS && spell != GUNS && spell != DAGGERS &&
                spell != WANDS && spell != CROSSBOWS && spell != FIST_WEAPONS)
                learnList.push_back(spell);
            break;
        case CLASS_HUNTER:
            if (spell != ONE_HAND_MACES && spell != TWO_HAND_MACES && spell != WANDS)
                learnList.push_back(spell);
            break;
        case CLASS_ROGUE:
            if (spell != TWO_HAND_AXES && spell != TWO_HAND_MACES && spell != POLEARMS &&
                spell != TWO_HAND_SWORDS && spell != STAVES && spell != WANDS)
                learnList.push_back(spell);
            break;
        case CLASS_PRIEST:
            if (spell == WANDS || spell == ONE_HAND_MACES || spell == STAVES ||
                spell == DAGGERS)
                learnList.push_back(spell);
            break;
        case CLASS_SHAMAN:
            if (spell != THROWN && spell != ONE_HAND_SWORDS && spell != TWO_HAND_SWORDS && spell != POLEARMS &&
                spell != BOWS && spell != GUNS && spell != WANDS && spell != CROSSBOWS)
                learnList.push_back(spell);
            break;
        case CLASS_WARLOCK:
        case CLASS_MAGE:
            if (spell == WANDS || spell == ONE_HAND_SWORDS || spell == STAVES ||
                spell == DAGGERS)
                learnList.push_back(spell);
            break;
        case CLASS_DRUID:
            if (spell != THROWN && spell != ONE_HAND_SWORDS && spell != TWO_HAND_SWORDS &&
                spell != BOWS && spell != GUNS && spell != WANDS && spell != CROSSBOWS &&
                spell != ONE_HAND_AXES && spell != TWO_HAND_AXES)
                learnList.push_back(spell);
            break;
        default:
            break;
        }
    }

    for (std::list<SkillSpells>::const_iterator spell = learnList.begin(), end = learnList.end(); spell != end; ++spell)
        if (!target->HasSpell(uint32(*spell)))
            target->learnSpell(uint32(*spell));

    target->UpdateSkillsToMaxSkillsForLevel();
    return true;
}

void AzthPlayer::AddBankItem(uint32 itemEntry, uint32 itemGuid)
{
    m_itemsInBank[itemEntry] = itemGuid;
}

void AzthPlayer::DelBankItem(uint32 itemEntry)
{
    m_itemsInBank.erase(itemEntry);
}

AzthPlayer::ItemInBankMap& AzthPlayer::GetBankItemsList()
{
    return m_itemsInBank;
}

bool AzthPlayer::canEnterMap(MapEntry const* entry, InstanceTemplate const* /*instance*/, bool /*loginCheck*/)
{
    // CHECK BG or ARENA requirements (item level limit for example)
    if (entry->IsBattlegroundOrArena() && !canJoinQueue(AZTH_QUEUE_BG_OR_ARENA))
    {

        player->SendTransferAborted(entry->MapID, TRANSFER_ABORT_MAP_NOT_ALLOWED);
        return false;
    }

    uint32 curDimension = getCurrentDimensionByAura();
    Difficulty targetDifficulty;
    targetDifficulty = player->GetDifficulty(entry->IsRaid());

    if (!entry->IsBattlegroundOrArena() && !sLFGMgr->inLfgDungeonMap(player->GetGUID(), entry->MapID, targetDifficulty))
    {

        // we could use GetContentLevelsForMapAndZone(uint32 mapid, uint32 zoneId) for a more accurate chech (?)

        if (curDimension == DIMENSION_60 && entry->Expansion() > 0) {
            // CLASSIC EXPANSION CHECK
            ChatHandler(player->GetSession()).SendSysMessage(sAzthLang->get(AZTH_LANG_MULTIDIMENSION_CLASSIC_EXPCHECK, player));

            player->SendTransferAborted(entry->MapID, TRANSFER_ABORT_MAP_NOT_ALLOWED);
            return false;

        } if (curDimension == DIMENSION_70 && entry->Expansion() > 1) {
            // TBC EXPANSION CHECK
            ChatHandler(player->GetSession()).SendSysMessage(sAzthLang->get(AZTH_LANG_MULTIDIMENSION_TBC_EXPCHECK, player));

            player->SendTransferAborted(entry->MapID, TRANSFER_ABORT_MAP_NOT_ALLOWED);
            return false;
        }
    }

    /*
     *  CUSTOM ITEM LEVEL CHECK
     */
    uint32 ilvl = getMaxItemLevelByStatus();
    if (ilvl > 0 && !checkItems(ilvl) && (entry->IsDungeon() || entry->IsBattlegroundOrArena()))
    {
        ChatHandler(player->GetSession()).PSendSysMessage("|cffff0000 This zone is limited to item level: %d|r", getMaxItemLevelByStatus());
        player->SendTransferAborted(entry->MapID, TRANSFER_ABORT_MAP_NOT_ALLOWED);
        return false;
    }

    /*
     *  FULL PVP CHECK
     */
    if (isPvP() && entry->IsDungeon())
    {
        ChatHandler(player->GetSession()).PSendSysMessage("|cffff0000 PvP Characters cannot enter Dungeons and Raids|r");
        player->SendTransferAborted(entry->MapID, TRANSFER_ABORT_MAP_NOT_ALLOWED);
        return false;
    }

    return true;
}

bool AzthPlayer::canGroup(Player* with)
{
    // player -> who invites
    // with -> the invited player
    Group* group = player->GetGroupInvite() != nullptr ? player->GetGroupInvite() : player->GetGroup();
    Player* leader = nullptr;
    Player* invited = nullptr;

    if (group)
        leader = ObjectAccessor::FindPlayer(group->GetLeaderGUID());

    if (with)
    {
        invited = with;

        if (!leader)
            leader = player;
    }
    else
        invited = player;

    if (leader && sAZTH->GetAZTHPlayer(leader)->isTimeWalking() && sAZTH->GetAZTHPlayer(invited)->GetTimeWalkingLevel() != sAZTH->GetAZTHPlayer(leader)->GetTimeWalkingLevel())
    {
        ChatHandler(player->GetSession()).SendSysMessage(sAzthLang->get(AZTH_LANG_TW_GROUP_CHECK_LEADER, player));
        ChatHandler(with->GetSession()).SendSysMessage(sAzthLang->get(AZTH_LANG_TW_GROUP_CHECK_PLAYER, with));
        return false;
    }

    if (with)
    {
        uint32 curDimPlayer = sAZTH->GetAZTHPlayer(player)->getCurrentDimensionByPhase();
        uint32 curDimWith = sAZTH->GetAZTHPlayer(with)->getCurrentDimensionByPhase();
        /*if (curDimPlayer != curDimWith) {
            ChatHandler(player->GetSession()).PSendSysMessage("|cffff0000 Non è possibile entrare in gruppo con PG di dimensioni diverse|r");
            return false;
        }*/

        if (curDimPlayer == DIMENSION_GUILD || curDimWith == DIMENSION_GUILD)
        {
            if (player->GetGuildId() != with->GetGuildId()) {
                ChatHandler(player->GetSession()).SendSysMessage(sAzthLang->get(AZTH_LANG_MULTIDIMENSION_GUILD_GROUPCHECK, player));
                ChatHandler(with->GetSession()).SendSysMessage(sAzthLang->get(AZTH_LANG_MULTIDIMENSION_GUILD_GROUPCHECK, with));
                return false;
            }
        }
    }

    return true;
}

bool AzthPlayer::canJoinQueue(AzthQueueMasks /* type */)
{
    // [AZTH-DISABLED]
    // if ((type & AZTH_QUEUE_ARENA) == type)
    //     if (!sASeasonMgr->canJoinArenaOrBg(player))
    //         return false;

    return true;
}

// this is the most accurate check to avoid exploit in some conditions
uint32 AzthPlayer::getCurrentDimensionByPhase() const
{
    return sAzthUtils->getCurrentDimensionByPhase(player->GetPhaseMask());
}

uint32 AzthPlayer::getCurrentDimensionByMark() const
{
    Aura* aura = player->GetAura(DIMENSION_SPELL_MARK);

    // if no aura, then assume we're on DIMENSION_NORMAL
    uint32 currentDim = aura ? aura->GetStackAmount() : DIMENSION_NORMAL;

    return currentDim;
}

uint32 AzthPlayer::getCurrentDimensionByAura() const
{
    uint32 dim = getCurrentDimensionByPhase();

    if (dim == DIMENSION_NORMAL)
        dim = getCurrentDimensionByMark(); // double integrity check

    return player->HasAura(DIMENSION_SPELL_MARK + dim) ? dim : DIMENSION_NORMAL;
}

bool AzthPlayer::hasDimSpecificAura(uint32 dim)
{
    return player->HasAura(DIMENSION_SPELL_MARK + dim);
}

bool AzthPlayer::canExplore()
{
    uint32 currentDim = getCurrentDimensionByPhase();

    if (sAzthUtils->isPhasedDimension(currentDim))
        return false;

    return true;
}

bool AzthPlayer::changeDimension(uint32 dim, bool validate /* = false*/, bool temp /* =false */)
{
    // dimension we're marked for
    // this property will contain the special dimension
    // even if we're temporary in normal (no phasing auras)
    uint32 markedDimension = getCurrentDimensionByMark();
    // real dimension based on auras. If we've the
    // DIMENSION_SPELL_MARK but no auras then this property will contain
    // DIM_NORMAL (temporary case)
    uint32 auraDimension = getCurrentDimensionByAura();
    // this is the most accurate check for validity since
    // with an exploit we could have a special dimension phase even without auras
    // then we must not allow it
    //uint32 phaseDimension     = getCurrentDimensionByPhase();
    bool changed = false;

    /*bool inGmParty=false;
    if (player->GetGroup()) {
        uint32 leaderGuid=player->GetGroup()->GetLeaderGUID();
        if (Player* leader = ObjectAccessor::FindPlayer(leaderGuid)) {
            if (leader->GetSession()->GetSecurity() > SEC_PLAYER) {
                inGmParty=true;
            }
        }
    }*/

    if (validate)
    {
        /*if (auraDimension != dim && !inGmParty && player->GetGroup()) {
            ChatHandler(player->GetSession()).SendSysMessage("Non è possibile cambiare dimensione mentre si è in gruppo");
            return false;
        }*/

        if (dim == DIMENSION_60) {
            if (player->getLevel() > 60 && sAZTH->GetAZTHPlayer(player)->GetTimeWalkingLevel() != TIMEWALKING_LVL_AUTO) {
                ChatHandler(player->GetSession()).SendSysMessage(sAzthLang->get(AZTH_LANG_MULTIDIMENSION_TW_CHECK60, player));
                return false;
            }
        }

        if (dim == DIMENSION_70) {
            if (player->getLevel() > 70 && sAZTH->GetAZTHPlayer(player)->GetTimeWalkingLevel() != TIMEWALKING_LVL_AUTO) {
                ChatHandler(player->GetSession()).SendSysMessage(sAzthLang->get(AZTH_LANG_MULTIDIMENSION_TW_CHECK70, player));
                return false;
            }
        }

        if (dim == DIMENSION_GUILD) {
            if (!player->GetGuild()) {
                ChatHandler(player->GetSession()).SendSysMessage(sAzthLang->get(AZTH_LANG_MULTIDIMENSION_GUILD_ACCESSCHECK, player));
                return false;
            }
        }

        if (dim == DIMENSION_GM) {
            if (player->GetSession()->GetSecurity() <= SEC_PLAYER) {
                ChatHandler(player->GetSession()).SendSysMessage(sAzthLang->get(AZTH_LANG_MULTIDIMENSION_GM_ACCESSCHECK, player));
                return false;
            }
        }
    }

    /*if (!inGmParty) {
        if (phaseDimension != dim && player->GetGroup()) {
            // if we changed phase and we're grouped
            // then we should ungroup to avoid exploits
            ///- If the player is in a group (or invited), remove him. If the group if then only 1 person, disband the group.
            player->UninviteFromGroup();

            if (player->GetGroup())
                player->RemoveFromGroup();
        }
    }*/

    if (!temp)
    {
        if (markedDimension > DIMENSION_NORMAL) {
            player->RemoveAura(DIMENSION_SPELL_MARK);
            changed = true;
        }
    }

    if (auraDimension > DIMENSION_NORMAL)
    {
        player->RemoveAura(DIMENSION_SPELL_MARK + auraDimension);
        changed = true;
    }

    if (dim > DIMENSION_NORMAL)
    {
        if (!temp) {
            player->SetAuraStack(DIMENSION_SPELL_MARK, player, dim);
        }

        player->SetAuraStack(DIMENSION_SPELL_MARK + dim, player, 1);
        changed = true;
    }

    if (changed)
    {
        player->CastSpell(player, 35517, true);
        std::string msg = sAzthLang->get(AZTH_LANG_MULTIDIMENSION_TEMP_TELEPORT, player);
        std::string dimName = sAzthUtils->getDimensionName(dim);
        std::string suffix = temp ? sAzthLang->get(AZTH_LANG_COMMON_TEMPORARILY, player) : "";
        msg += " <" + dimName + "> " + suffix + " |r";

        ChatHandler(player->GetSession()).PSendSysMessage("%s", msg.c_str());
    }

    return changed;
}

bool AzthPlayer::canCompleteCriteria(AchievementCriteriaEntry const* criteria)
{
    if (!criteria)
        return true;

    // first kill checks for timewalking, boss state etc
    if (!sAzthFirstKills->canCompleteAchi(player, criteria->referredAchievement))
        return false;

    // achievements/criteria should not be completed in dimensions
    uint32 currentDimension = getCurrentDimensionByAura();
    if (sAzthUtils->isPhasedDimension(currentDimension))
        return false;

    return true;
}

uint32 AzthPlayer::getMaxItemLevelByStatus()
{
    // timewalking case
    if (isTimeWalking())
    {
        uint32 twLimit = getTwItemLevel(GetTimeWalkingLevel());

        if (twLimit)
            return twLimit;
    }

    // raid case
    if (player->GetMap()->IsDungeon() && player->GetMapId() == 631 /*icc*/ )
    {
        if (InstanceScript * inst = player->GetInstanceScript()) {

            //[AZTH]
            if (inst->GetData(DATA_AZTH_HARD_MODE))
                return 0;
        }

        return 284; // 284 is the latest equippable level in wow
    }

    if (player->GetMap()->IsBattlegroundOrArena() || player->GetMap()->IsDungeon())
    {
        // basically, even if we've an item level limit specific for arena and bg
        // we must limit custom weapons there
        return 284;
    }

    return 0; // no limit outside
}

bool AzthPlayer::canEquipItem(ItemTemplate const* proto)
{
    // skip bag and quivers
    if (proto->InventoryType == INVTYPE_BAG || proto->InventoryType == INVTYPE_QUIVER)
        return true;

    // it is needed to avoid equip in empty slots
    // [AZTH-DISABLED]
    // if (hasGear())
    //     return false;
    // [AZTH-DISABLED]
    // if (!sASeasonMgr->checkItem(proto, player))
    //     return false;

    if (!checkItem(proto))
        return false;

    // do not allow to equip items with too high item level
    // when not in timewalking: some items doesn't have a required level and can potentially equipped by low level players.
    // excluding heirlooms
    uint32 calcLevel = sAzthUtils->getCalcReqLevel(proto);
    if (!isTimeWalking(true)
        && proto->ScalingStatDistribution == 0
        && player->getLevel() + 15 < static_cast<uint8>(calcLevel))
    {
        ChatHandler(player->GetSession()).PSendSysMessage("Cannot equip this item. Its level is too high");
        return false;
    }

    //Double ashen block system
    if (sAzthUtils->isAshenBand(proto->ItemId))
    {
        // finger slots
        for (uint32 INVENTORY_INDEX = EQUIPMENT_SLOT_FINGER1; INVENTORY_INDEX <= EQUIPMENT_SLOT_FINGER2; INVENTORY_INDEX++)
        {
            Item* itemToCheck = player->GetItemByPos(INVENTORY_SLOT_BAG_0, INVENTORY_INDEX);
            if (itemToCheck != nullptr)
            {
                if (sAzthUtils->isAshenBand(itemToCheck->GetEntry()))
                {
                    ChatHandler(player->GetSession()).PSendSysMessage("Are you a double ashen abuser?!");
                    return false;
                }
            }
        }
    }

    return true;
}

//
// Passing player argument will check the player state and automatically shows a message
//
bool AzthPlayer::checkItem(ItemTemplate const* proto)
{
    uint32 _ilvl = getMaxItemLevelByStatus();
    if (_ilvl && !sAzthUtils->checkItemLvL(proto, _ilvl))
    {
        ChatHandler(player->GetSession()).SendSysMessage(sAzthLang->getf(AZTH_LANG_PVPITEMS_LEVEL_CHECK, player, proto->ItemId, proto->Name1.c_str()));
        return false;
    }

    return true;
}

bool AzthPlayer::checkItems(uint32 iLvlMax, uint8 type /*=0*/)
{
    if (player->IsGameMaster())
        return true;

    if (type == 0)
    {
        // cap limit for each item
        uint32 INVENTORY_END = 18;
        uint32 counter = 0;

        for (uint32 INVENTORY_INDEX = 0; INVENTORY_INDEX <= INVENTORY_END; INVENTORY_INDEX++)
        {
            // don't check tabard or shirt
            if (INVENTORY_INDEX == EQUIPMENT_SLOT_TABARD || INVENTORY_INDEX == EQUIPMENT_SLOT_BODY)
                continue;

            Item* itemToCheck = player->GetItemByPos(INVENTORY_SLOT_BAG_0, INVENTORY_INDEX);
            if (itemToCheck != nullptr)
            {
                if (!sAzthUtils->checkItemLvL(itemToCheck->GetTemplate(), iLvlMax))
                {
                    ChatHandler(player->GetSession()).SendSysMessage(sAzthLang->getf(AZTH_LANG_PVPITEMS_LEVEL_CHECK, player, itemToCheck->GetTemplate()->ItemId, itemToCheck->GetTemplate()->Name1.c_str()));
                    counter++;
                }
            }
        }

        return counter == 0;
    }
    else
    {
        uint32 avg = 0;
        switch (type)
        {
        case 1:
            avg = player->GetAverageItemLevel();
            break;
        case 2:
            avg = player->GetAverageItemLevelForDF();
            break;
        }

        if (avg > iLvlMax)
        {
            ChatHandler(player->GetSession()).PSendSysMessage(sAzthLang->getf(AZTH_LANG_PVPITEMS_MLEVEL_CHECK, player), avg);
            return false;
        }
        else
            return true;
    }
}

bool AzthPlayer::isPvPFlagOn(bool def)
{
    if (getCurrentDimensionByPhase() == DIMENSION_PVP || getCurrentDimensionByPhase() == DIMENSION_ENTERTAINMENT)
    {
        player->SetByteFlag(UNIT_FIELD_BYTES_2, 1, UNIT_BYTE2_FLAG_PVP);
        return true;
    }

    return def;
}

bool AzthPlayer::isFFAPvPFlagOn(bool def)
{
    if (getCurrentDimensionByPhase() == DIMENSION_PVP || getCurrentDimensionByPhase() == DIMENSION_ENTERTAINMENT)
    {
        player->SetByteFlag(UNIT_FIELD_BYTES_2, 1, UNIT_BYTE2_FLAG_FFA_PVP);
        return true;
    }

    return def;
}

void AzthPlayer::CreateWowarmoryFeed(uint32 type, uint32 data, uint32 item_guid, uint32 item_quality)
{
    /*
    *   1 - TYPE_ACHIEVEMENT_FEED
    *   2 - TYPE_ITEM_FEED
    *   3 - TYPE_BOSS_FEED
    */

    if (!player->GetGUID().IsEmpty())
    {
       LOG_INFO("Module", "[Wowarmory]: player is not initialized, unable to create log entry!");
        return;
    }

    if (type <= 0 || type > 3)
    {
        LOG_INFO("Module", "[Wowarmory]: unknown feed type: %d, ignore.", type);
        return;
    }

    if (data == 0)
    {
        LOG_INFO("Module", "[Wowarmory]: empty data (GUID: %u), ignore.", player->GetGUID().GetCounter());
        return;
    }

    WowarmoryFeedEntry feed;
    feed.guid = player->GetGUID().GetCounter();
    feed.type = type;
    feed.data = data;
    feed.difficulty = type == 3 ? player->GetMap()->GetDifficulty() : 0;
    feed.item_guid = item_guid;
    feed.item_quality = item_quality;
    feed.counter = 0;
    feed.date = time(nullptr);

    LOG_DEBUG("Module", "[Wowarmory]: create wowarmory feed (GUID: %u, type: %d, data: %u).", feed.guid, feed.type, feed.data);
    m_wowarmory_feeds.push_back(feed);
}

// Solo 3v3
uint32 AzthPlayer::getArena3v3Info(uint8 type)
{
    return arena3v3Info[type];
}

void AzthPlayer::setArena3v3Info(uint8 type, uint32 value)
{
    arena3v3Info[type] = value;
}

bool AzthPlayer::isInBlackMarket() {
    std::vector<float> pos = {player->GetPositionX(), player->GetPositionY(), player->GetPositionZ()};
    // (1, 4818.27f, -1971.3f, 1069.75f, 0.174f, 0);
    if (player->GetMapId() != 37)
        return false;

    /*if (player->GetZoneId() != 331)
        return false;

    if (pos[0] < 2650.f || pos[0] > 3100.f)
        return false;

    if (pos[1] < 2250.f || pos[1] > -2800.f)
        return false;

    if (pos[2] < 80.f || pos[2] > 180.f)
        return false;
    */
    return true;
};
