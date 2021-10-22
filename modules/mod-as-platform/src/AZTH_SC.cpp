/*
 *  Copyright (С) since 2019 Andrei Guluaev (Winfidonarleyan/Kargatum) https://github.com/Winfidonarleyan
 */

#include "AZTH.h"
#include "ArenaSeason.h"
#include "AzthFirstKills.h"
#include "AzthUtils.h"
#include "azth_custom_hearthstone_mode.h"
#include "AzthObject.h"
#include "AzthLevelStat.h"
#include "AzthSmartStone.h"
#include "AzthPlayer.h"
#include "GuildHouse.h"
#include "Teleport.h"
#include "Solo3v3.h"
#include "SpellAuraEffects.h"
#include "ScriptMgr.h"
#include "ArenaTeamMgr.h"
#include "BattlegroundQueue.h"
#include "Pet.h"
#include "Group.h"
#include "Player.h"
#include "Battleground.h"
#include "WorldSession.h"
#include "PetitionMgr.h"
#include "MapMgr.h"
#include "ExtraDatabase.h"

#if AC_COMPILER == AC_COMPILER_GNU
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
#endif

using namespace Acore::ChatCommands;

enum AZTHLanguage
{
    LANG_AZTH_NO_INFO_ARENA_JOINED = 7000, //[AZTH]
    LANG_AZTH_NO_INFO_ARENA_EXITED = 7001, //[AZTH]
};

class AuraEffect;

// SC
class Arena_SC : public ArenaScript
{
public:
    Arena_SC() : ArenaScript("Arena_SC") { }

    bool CanAddMember(ArenaTeam* team, ObjectGuid /* playerGuid */) override
    {
        if (!team)
            return false;

        if (!team->GetMembersSize())
            return true;

        if (team->GetType() == ARENA_TEAM_1v1 || team->GetType() == ARENA_TEAM_SOLO_3v3)
            return false;

        return true;
    }

    void OnGetPoints(ArenaTeam* team, uint32 /* memberRating */, float& points) override
    {
        if (!team)
            return;

        if (team->GetType() == ARENA_TEAM_1v1)
            points *= sConfigMgr->GetFloatDefault("Azth.Rate.Arena1v1", 0.40f);

        if (team->GetType() == ARENA_TEAM_SOLO_3v3)
            points *= sConfigMgr->GetFloatDefault("Solo.3v3.ArenaPointsMulti", 0.88f);
    }

    bool CanSaveToDB(ArenaTeam* team) override
    {
        if (team->GetId() >= MAX_ARENA_TEAM_ID)
        {
            sSolo->SaveSoloDB(team);
            return false;
        }

        return true;
    }
};

class Battleground_SC : public BGScript
{
public:
    Battleground_SC() : BGScript("Battleground_SC") { }

    // 1v1 Arena - Start arena after 15s, when all players are in arena
    void OnBattlegroundUpdate(Battleground* bg, uint32 /*diff*/) override
    {
        if (bg->GetStatus() != STATUS_WAIT_JOIN)
            return;

        if (bg->GetArenaType() != ARENA_TEAM_1v1)
            return;

        if (bg->GetStartDelayTime() > BG_START_DELAY_15S && (bg->GetPlayersCountByTeam(TEAM_ALLIANCE) + bg->GetPlayersCountByTeam(TEAM_HORDE)) == 2)
            bg->SetStartDelayTime(BG_START_DELAY_15S);
    }

    bool OnBeforeSendExitMessageArenaQueue(BattlegroundQueue* /*queue*/, GroupQueueInfo* ginfo) override
    {
        if (!ginfo->IsRated || !ginfo->ArenaType || ginfo->Players.empty())
            return true;

        if (!sArenaTeamMgr->GetArenaTeamById(ginfo->ArenaTeamId))
            return false;

        sWorld->SendWorldText(LANG_AZTH_NO_INFO_ARENA_EXITED, ginfo->ArenaType, ginfo->ArenaType);
        return false;
    }

    bool OnBeforeSendJoinMessageArenaQueue(BattlegroundQueue* /*queue*/, Player* /*leader*/, GroupQueueInfo* ginfo, PvPDifficultyEntry const* /*bracketEntry*/, bool /*isRated*/) override
    {
        if (!ginfo->IsRated || !ginfo->ArenaType || ginfo->Players.empty())
            return true;

        if (!sArenaTeamMgr->GetArenaTeamById(ginfo->ArenaTeamId))
            return false;

        sWorld->SendWorldText(LANG_AZTH_NO_INFO_ARENA_JOINED, ginfo->ArenaType, ginfo->ArenaType); //[AZTH]
        return false;
    }
};

class Command_SC : public CommandSC
{
public:
    Command_SC() : CommandSC("Command_SC") { }

    void OnHandleDevCommand(Player* player, std::string& argstr) override
    {
        if (!player)
            return;

        if (argstr == "on")
        {
            player->SetPhaseMask(uint32(PHASEMASK_ANYWHERE), false);
            player->SetGameMaster(true);
        }
        else
        {
            player->SetPhaseMask(uint32(PHASEMASK_NORMAL), false);
            player->SetGameMaster(false);
        }
    }
};

class Player_SC : public PlayerScript
{
public:
    Player_SC() : PlayerScript("Player_SC") { }

    void OnAchiComplete(Player* player, AchievementEntry const* achievement) override
    {
        if (!player)
            return;

        sAZTH->GetAZTHPlayer(player)->CreateWowarmoryFeed(1, achievement->ID, 0, 0);
    }

    void OnRewardKillRewarder(Player* player, bool isDungeon, float& rate) override
    {
        if (!player)
            return;

        // [AZTH] workaround for high exp rate in dungeons
        if (isDungeon && sAZTH->GetAZTHPlayer(player)->GetPlayerQuestRate() > 0)
            rate *= sAZTH->GetAZTHPlayer(player)->GetPlayerQuestRate();
    }

    bool CanGiveMailRewardAtGiveLevel(Player* player, uint8 /*level*/) override
    {
        if (!player)
            return false;

        if (sAZTH->GetAZTHPlayer(player)->isTimeWalking())
            return false;

        return true;
    }

    void OnDeleteFromDB(CharacterDatabaseTransaction trans, uint32 guid) override
    {
        if (!guid)
            return;

        trans->PAppend("DELETE FROM armory_character_stats WHERE guid = '%u'", guid);
        trans->PAppend("DELETE FROM character_feed_log WHERE guid = '%u'", guid);
    }

    bool CanRepopAtGraveyard(Player* player) override
    {
        if (!player)
            return false;

        if (sAZTH->GetAZTHPlayer(player)->isInBlackMarket())
        {
            if (!player->IsAlive())
            {
                player->ResurrectPlayer(0.5f);
                player->SpawnCorpseBones();
            }

            player->TeleportTo(AzthSharedDef::blackMarket);
            return false;
        }

        return true;
    }

    void OnGetMaxSkillValue(Player* player, uint32 /*skill*/, int32& result, bool /*IsPure*/) override
    {
        if (!player)
            return;

        //[AZTH] Hack for timewalking
        if (sAZTH->GetAZTHPlayer(player)->isTimeWalking())
            result = sWorld->getIntConfig(CONFIG_MAX_PLAYER_LEVEL) * 5;
    }

    bool CanAreaExploreAndOutdoor(Player* player) override
    {
        if (!player)
            return false;

        if (!sAZTH->GetAZTHPlayer(player)->canExplore())
            return false;

        return true;
    }

    void OnVictimRewardBefore(Player* player, Player* victim, uint32& killer_title, uint32& victim_title) override
    {
        if (!player || !victim)
            return;

        // Get Killer titles, CharTitlesEntry::bit_index
        // PLAYER__FIELD_KNOWN_TITLES describe which titles player can use,
        // so we must find biggest pvp title , even for killer to find extra honor value
        uint32 vtitle = victim->GetUInt32Value(PLAYER__FIELD_KNOWN_TITLES);
        uint32 ktitle = player->GetUInt32Value(PLAYER__FIELD_KNOWN_TITLES);

        if (PLAYER_TITLE_MASK_ALL_PVP & ktitle)
            for (int i = ((player->GetTeamId(true) == TEAM_ALLIANCE) ? 1 : HKRANKMAX); i != ((player->GetTeamId(true) == TEAM_ALLIANCE) ? HKRANKMAX : (2 * HKRANKMAX - 1)); i++)
                if (ktitle & (1 << i))
                    killer_title = i;

        if (PLAYER_TITLE_MASK_ALL_PVP & vtitle)
            for (int i = ((victim->GetTeamId(true) == TEAM_ALLIANCE) ? 1 : HKRANKMAX); i != ((victim->GetTeamId(true) == TEAM_ALLIANCE) ? HKRANKMAX : (2 * HKRANKMAX - 1)); i++)
                if (vtitle & (1 << i))
                    victim_title = i;
    }

    void OnVictimRewardAfter(Player* player, Player* /*victim*/, uint32& killer_title, uint32& victim_rank, float& honor_f) override
    {
        if (!player)
            return;

        uint32 rank_diff = 0;

        // now find rank difference
        if (killer_title == 0 && victim_rank > 4)
            rank_diff = victim_rank - 4;
        else if (killer_title < HKRANKMAX)
            rank_diff = (victim_rank > (killer_title + 4)) ? (victim_rank - (killer_title + 4)) : 0;
        else if (killer_title < (2 * HKRANKMAX - 1))
            rank_diff = (victim_rank > (killer_title - (HKRANKMAX - 1) + 4)) ? (victim_rank - (killer_title - (HKRANKMAX - 1) + 4)) : 0;

        honor_f *= 1 + sAZTH->GetRatePvPRankExtraHonor() * (((float)rank_diff) / 10.0f);

        uint32 new_title = 0;
        uint32 honor_kills = player->GetUInt32Value(PLAYER_FIELD_LIFETIME_HONORABLE_KILLS);
        uint32 old_title = player->GetUInt32Value(PLAYER_CHOSEN_TITLE);

        player->RemoveFlag64(PLAYER__FIELD_KNOWN_TITLES, PLAYER_TITLE_MASK_ALL_PVP);

        bool max_rank = ((honor_kills >= sAZTH->GetKillsPerRank(HKRANKMAX - 1)) ? true : false);

        for (int i = HKRANK01; i != HKRANKMAX; ++i)
        {
            if (honor_kills < sAZTH->GetKillsPerRank(i) || max_rank)
            {
                new_title = (max_rank ? HKRANKMAX - 1 : i - 1);

                if (new_title)
                    new_title += (player->GetTeamId(true) == TEAM_ALLIANCE ? 0 : HKRANKMAX - 1);

                break;
            }
        }

        player->SetFlag64(PLAYER__FIELD_KNOWN_TITLES, uint64(1) << new_title);

        if (old_title > 0 && old_title < (2 * HKRANKMAX - 1) && new_title > old_title)
            player->SetUInt32Value(PLAYER_CHOSEN_TITLE, new_title);
    }

    void OnCustomScalingStatValueBefore(Player* player, ItemTemplate const* proto, uint8 /*slot*/, bool /*apply*/, uint32& CustomScalingStatValue) override
    {
        if (!player || !proto)
            return;

        if (!sAZTH->GetAZTHPlayer(player)->isTimeWalking(true))
            return;

        CustomScalingStatValue = sAzthUtils->calculateItemScalingValue(proto, player);
    }

    void OnCustomScalingStatValue(Player* player, ItemTemplate const* proto, uint32& statType, int32& val, uint8 itemProtoStatNumber, uint32 ScalingStatValue, ScalingStatValuesEntry const* ssv) override
    {
        if (!player || !proto)
            return;

        statType = proto->ItemStat[itemProtoStatNumber].ItemStatType;
        uint32 posVal = proto->ItemStat[itemProtoStatNumber].ItemStatValue < 0 ? -(proto->ItemStat[itemProtoStatNumber].ItemStatValue) : proto->ItemStat[itemProtoStatNumber].ItemStatValue;

        // hack for items that don't have a required level
        uint32 req = sAzthUtils->getCalcReqLevel(proto);

        ScalingStatValuesEntry const* maxSSV = sScalingStatValuesStore.LookupEntry(req);
        if (maxSSV)
        {
            float mulMax = sAzthUtils->getCustomMultiplier(proto, (float)maxSSV->getssdMultiplier(ScalingStatValue));
            uint32 modifier = ((float)posVal / mulMax) * 10000;

            float mul = sAzthUtils->getCustomMultiplier(proto, (float)ssv->getssdMultiplier(ScalingStatValue));
            val = (mul * modifier) / 10000;

            if (proto->ItemStat[itemProtoStatNumber].ItemStatValue < 0)
                val *= -1;

            //[AZTH] avoid higher values than stats. This means that there's an
            // uncorrect scaling calculation
            if (proto->ScalingStatValue == 0)
            {
                // constant reduction since even with scaling, stats are too large
                if ((player->getLevel() + 20) >= (uint8)req) // from 10 to 20 level diff
                    val = val / 2;
                else // from 21 to max level diff
                    val = val / 3;

                if (abs(val) > abs(proto->ItemStat[itemProtoStatNumber].ItemStatValue))
                    val = proto->ItemStat[itemProtoStatNumber].ItemStatValue;
            }
        }
        else
            val = proto->ItemStat[itemProtoStatNumber].ItemStatValue;
    }

    bool CanArmorDamageModifier(Player* player) override
    {
        if (!player)
            return false;

        if (sAZTH->GetAZTHPlayer(player)->isTimeWalking(true))
            return false;

        return true;
    }

    void OnGetFeralApBonus(Player* player, int32& feral_bonus, int32 dpsMod, ItemTemplate const* proto, ScalingStatValuesEntry const* ssv) override
    {
        if (!player)
            return;

        feral_bonus = sAzthUtils->normalizeFeralAp(feral_bonus, dpsMod, proto, ssv);
    }

    bool CanApplyWeaponDependentAuraDamageMod(Player* player, Item* /*item*/, WeaponAttackType /*attackType*/, AuraEffect const* aura, bool /*apply*/) override
    {
        if (!player)
            return false;

        // but we need other effect of MOD_DAMAGE_PERCENT with SPELL_SCHOOL_MASK_NORMAL (physic spells)
        if (aura->GetSpellInfo()->Id == TIMEWALKING_AURA_MOD_DAMAGESPELL)
            return false;

        return true;
    }

    bool CanApplyEquipSpell(Player* player, SpellInfo const* /* spellInfo */, Item* item, bool /*apply*/, bool /*form_change*/) override
    {
        if (!player || !item)
            return false;

        // [AZTH] Timewalking
        if (!sAZTH->GetAZTHPlayer(player)->itemCheckReqLevel(item->GetTemplate()))
            return false;

        return true;
    }

    bool CanApplyEquipSpellsItemSet(Player* player, ItemSetEffect* eff) override
    {
        if (!player || !eff)
            return false;

        // [AZTH] Timewalking
        bool found = false;
        for (uint8 i = 0; i < INVENTORY_SLOT_BAG_END; ++i)
        {
            Item* item = player->GetItemByPos(i);
            if (!item)
                continue;

            ItemTemplate const* proto = item->GetTemplate();
            if (proto->ItemSet == eff->setid)
            {
                if (proto && !sAZTH->GetAZTHPlayer(player)->itemCheckReqLevel(proto))
                    found = true;

                break;
            }
        }

        if (found)
            return false;

        return true;
    }

    bool CanCastItemCombatSpell(Player* player, Unit* /*target*/, WeaponAttackType /*attType*/, uint32 /*procVictim*/, uint32 /*procEx*/, Item* /*item*/, ItemTemplate const* proto) override
    {
        if (!player)
            return false;

        // [AZTH] Timewalking
        if (!sAZTH->GetAZTHPlayer(player)->itemCheckReqLevel(proto))
            return false;

        return true;
    }

    bool CanCastItemUseSpell(Player* player, Item* item, SpellCastTargets const& /* targets */, uint8 /* cast_count */, uint32 /* glyphIndex */) override
    {
        if (!player)
            return false;

        // [AZTH] Timewalking
        if (!sAZTH->GetAZTHPlayer(player)->canUseItem(item, true))
            return false;

        return true;
    }

    void OnApplyAmmoBonuses(Player* player, ItemTemplate const* proto, float& currentAmmoDPS) override
    {
        if (!player || !proto)
            return;

        //[AZTH] Timewalking
        if (!sAZTH->GetAZTHPlayer(player)->itemCheckReqLevel(proto))
        {
            uint32 reqLevel = sAzthUtils->getCalcReqLevel(proto);

            if (player->getLevel() < reqLevel)
            {
                uint32 red = ceil(player->getLevel() * 100 / reqLevel / 2);
                uint32 malus = ceil((reqLevel - player->getLevel()) / 10);
                float pRed = float(red > malus ? red - malus : 1) / 100.0f; // convert to fraction
                currentAmmoDPS = ceil(currentAmmoDPS * pRed);
            }
        }
    }

    bool CanEquipItem(Player* player, uint8 /*slot*/, uint16& /*dest*/, Item* pItem, bool /* swap */, bool /* not_loading */) override
    {
        if (!player || !pItem)
            return false;

        ItemTemplate const* proto = pItem->GetTemplate();
        if (!proto)
            return false;

        //[AZTH] if you have the pvp set or not valid item for this season and you
        // are equipping something, it won't be equipped
        if (!player->GetSession()->PlayerLoading() && proto->InventoryType && !sAZTH->GetAZTHPlayer(player)->canEquipItem(proto))
            return false;

        return true;
    }

    bool CanUnequipItem(Player* player, uint16 pos, bool /* swap */) override
    {
        if (!player)
            return false;

        if (player->IsEquipmentPos(pos) && sAZTH->GetAZTHPlayer(player)->hasGear())
            return false;

        return true;
    }

    bool CanUseItem(Player* player, ItemTemplate const* proto, InventoryResult& result) override
    {
        if (!player || !proto)
        {
            result = EQUIP_ERR_CANT_DO_RIGHT_NOW;
            return false;
        }

        //if player is in bg or arena and tournament is enabled check for item level
        //if it is > of the maximum level doesnt allow the equipment
        if (proto->InventoryType > 0 && !sAZTH->GetAZTHPlayer(player)->canEquipItem(proto))
        {
            result = EQUIP_ERR_CANT_DO_RIGHT_NOW;
            return false;
        }

        //[AZTH] if you are a timewalker you can equip all items
        // because you are an 80 with "fake low level"
        if (!sAZTH->GetAZTHPlayer(player)->isTimeWalking() && player->getLevel() < proto->RequiredLevel)
        {
            result = EQUIP_ERR_CANT_EQUIP_LEVEL_I;
            return false;
        }

        return true;
    }

    bool CanSaveEquipNewItem(Player* player, Item* /*item*/, uint16 /*pos*/, bool /*update*/) override
    {
        if (!player)
            return false;

        if (sAZTH->GetAZTHPlayer(player)->hasGear())
            return false;

        return true;
    }

    bool CanApplyEnchantment(Player* player, Item* item, EnchantmentSlot slot, bool /*apply*/, bool /*apply_dur*/, bool /*ignore_condition*/) override
    {
        if (!player || !item)
            return false;

        //[AZTH] Timewalking
        if (!sAZTH->GetAZTHPlayer(player)->itemCheckReqLevel(item->GetTemplate()))
            return false;

        uint32 enchant_id = item->GetEnchantmentId(slot);
        if (!enchant_id)
            return false;

        SpellItemEnchantmentEntry const* pEnchant = sSpellItemEnchantmentStore.LookupEntry(enchant_id);
        if (!pEnchant)
            return false;

        //[AZTH] Timewalking
        if (sAzthUtils->disableEnchant(player, pEnchant))
            return false;

        return true;
    }

    void OnGetQuestRate(Player* player, float& result) override
    {
        if (!player)
            return;

        result = sAZTH->GetAZTHPlayer(player)->GetPlayerQuestRate();
    }

    bool PassedQuestKilledMonsterCredit(Player* player, Quest const* qinfo, uint32 entry, uint32 real_entry, ObjectGuid guid) override
    {
        if (!player || !qinfo)
            return false;

        //[AZTH] it it's an Hearthstone quest and doesn't satisfy requirements, skip
        // otherwise continue as normal
        if (!sAZTH->GetAZTHPlayer(player)->passHsChecks(qinfo, entry, real_entry, guid))
            return false;

        return true;
    }

    bool CheckItemInSlotAtLoadInventory(Player* player, Item* item, uint8 slot, uint8& err, uint16& dest) override
    {
        if (!player)
            return false;

        //[AZTH] pvp set
        if (!sAZTH->GetAZTHPlayer(player)->hasGear())
            return true;

        ItemTemplate const* pProto = item->GetTemplate();
        if (pProto)
        {
            uint8 eslot = player->FindEquipSlot(pProto, slot, false);
            if (eslot == NULL_SLOT)
                err = EQUIP_ERR_ITEM_CANT_BE_EQUIPPED;

            dest = ((INVENTORY_SLOT_BAG_0 << 8) | eslot);

            return false;
        }
        else
        {
            err = EQUIP_ERR_ITEM_NOT_FOUND;
            return false;
        }

        return true;
    }

    bool NotAvoidSatisfy(Player* player, DungeonProgressionRequirements const* ar, uint32 /* target_map */, bool /* report */) override
    {
        if (!player)
            return false;

        //[AZTH] Timewalking, avoid attunement for TBC & Classic
        if (ar->levelMin <= 70 && sAZTH->GetAZTHPlayer(player)->isTimeWalking())
            return false;

        return true;
    }

    void OnSave(Player* player) override
    {
        if (!player)
            return;

        // Wowarmory feeds
        CharacterDatabaseTransaction wowArmoryTrans = CharacterDatabase.BeginTransaction();

        AzthPlayer* azthPlayer = sAZTH->GetAZTHPlayer(player);
        if (!azthPlayer)
            return;

        if (!azthPlayer->m_wowarmory_feeds.empty())
        {
            std::ostringstream sWowarmory;

            sWowarmory << "INSERT IGNORE INTO character_feed_log (guid,type,data,date,counter,difficulty,item_guid,item_quality) VALUES ";

            for (WowarmoryFeeds::iterator iter = azthPlayer->m_wowarmory_feeds.begin(); iter < azthPlayer->m_wowarmory_feeds.end(); ++iter)
            {
                //                      guid                    type                        data                    date                            counter                   difficulty                        item_guid                      item_quality
                sWowarmory << "(" << (*iter).guid << ", " << (*iter).type << ", " << (*iter).data << ", " << uint64((*iter).date) << ", " << (*iter).counter << ", " << uint32((*iter).difficulty) << ", " << (*iter).item_guid << ", " << (*iter).item_quality << ")";
                if (iter != azthPlayer->m_wowarmory_feeds.end() - 1)
                    sWowarmory << ",";
            }

            wowArmoryTrans->Append(sWowarmory.str().c_str());

            // Clear old saved feeds from storage - they are not required for server core.
            azthPlayer->InitWowarmoryFeeds();
        }

        wowArmoryTrans->PAppend("DELETE FROM armory_character_stats WHERE guid = %u", player->GetGUID().GetCounter());

        // Character stats
        std::ostringstream ps;
        time_t t = time(nullptr);
        ps << "INSERT INTO armory_character_stats (guid, data, save_date) VALUES (" << player->GetGUID().GetCounter() << ", '";

        for (uint16 i = 0; i < player->GetValuesCount(); ++i)
            ps << player->GetUInt32Value(i) << " ";

        ps << "', " << uint64(t) << ");";

        wowArmoryTrans->Append(ps.str().c_str());

        CharacterDatabase.CommitTransaction(wowArmoryTrans);
    }

    bool NotVisibleGloballyFor(Player* player, Player const* /* u */) override
    {
        if (!player)
            return true;

        // if (player->GetSession()->GetSecurity() == SEC_ENTERTAINER && u->GetSession()->GetSecurity() == SEC_T1_SUPPORTER)
        //     return false;

        return true;
    }

    void OnGetArenaPersonalRating(Player* player, uint8 slot, uint32& result) override
    {
        if (!player)
            return;

        if (slot == ArenaTeam::GetSlotByType(ARENA_TEAM_1v1))
            result = sAZTH->GetAZTHPlayer(player)->getArena1v1Info(ARENA_TEAM_PERSONAL_RATING);

        if (slot == ArenaTeam::GetSlotByType(ARENA_TEAM_SOLO_3v3))
            result = sAZTH->GetAZTHPlayer(player)->getArena3v3Info(ARENA_TEAM_PERSONAL_RATING);
    }

    void OnGetArenaTeamId(Player* player, uint8 slot, uint32& result) override
    {
        if (!player)
            return;

        // [AZTH] use static method of ArenaTeam to retrieve the slot
        if (slot == ArenaTeam::GetSlotByType(ARENA_TEAM_1v1))
            result = player->GetArenaTeamIdFromDB(player->GetGUID(), ARENA_TEAM_1v1);

        if (slot == ArenaTeam::GetSlotByType(ARENA_TEAM_SOLO_3v3))
            result = player->GetArenaTeamIdFromDB(player->GetGUID(), ARENA_TEAM_SOLO_3v3);
    }

    void OnIsFFAPvP(Player* player, bool& result) override
    {
        if (!player)
            return;

        result = sAZTH->GetAZTHPlayer(player)->isFFAPvPFlagOn(result);
    }

    void OnIsPvP(Player* player, bool& result) override
    {
        if (!player)
            return;

        result = sAZTH->GetAZTHPlayer(player)->isPvPFlagOn(result);
    }

    void OnGetMaxSkillValueForLevel(Player* player, uint16& result) override
    {
        if (!player)
            return;

        if (sAZTH->GetAZTHPlayer(player)->isTimeWalking())
            result = sWorld->getIntConfig(CONFIG_MAX_PLAYER_LEVEL) * 5;
    }

    bool NotSetArenaTeamInfoField(Player* player, uint8 slot, ArenaTeamInfoType type, uint32 value) override
    {
        if (!player)
            return false;

        // [AZTH] avoid higher slots to be set in datafield
        if (slot == ArenaTeam::GetSlotByType(ARENA_TEAM_1v1))
        {
            sAZTH->GetAZTHPlayer(player)->setArena1v1Info(type, value);
            return false;
        }

        if (slot == ArenaTeam::GetSlotByType(ARENA_TEAM_SOLO_3v3))
        {
            sAZTH->GetAZTHPlayer(player)->setArena3v3Info(type, value);
            return false;
        }

        return true;
    }

    void OnLoadFromDB(Player* player) override
    {
        if (!player)
            return;

        // Cleanup old Wowarmory feeds
        sAZTH->GetAZTHPlayer(player)->InitWowarmoryFeeds();
    }

    bool CanJoinInBattlegroundQueue(Player* player, ObjectGuid /*guid*/, BattlegroundTypeId /*BGTypeID*/, uint8 joinAsGroup, GroupJoinBattlegroundResult& err) override
    {
        if (!player)
            return false;

        if (!joinAsGroup && !sAZTH->GetAZTHPlayer(player)->canJoinQueue(AZTH_QUEUE_BG))
        {
            err = ERR_BATTLEGROUND_NONE;
            return false;
        }

        return true;
    }

    bool CanBattleFieldPort(Player* player, uint8 arenaType, BattlegroundTypeId BGTypeID, uint8 action) override
    {
        if (!player)
            return false;

        BattlegroundQueueTypeId bgQueueTypeId = BattlegroundMgr::BGQueueTypeId(BGTypeID, arenaType);
        if (bgQueueTypeId == BATTLEGROUND_QUEUE_NONE)
            return false;

        if ((bgQueueTypeId == (BattlegroundQueueTypeId)BATTLEGROUND_QUEUE_1v1 || bgQueueTypeId == (BattlegroundQueueTypeId)BATTLEGROUND_QUEUE_3v3_SOLO) && (action == 1 /*accept join*/ && !sSolo->Arena1v1CheckTalents(player)))
            return false;

        return true;
    }

    bool CanJoinInArenaQueue(Player* player, ObjectGuid /* BattlemasterGuid */, uint8 /* arenaslot */, BattlegroundTypeId /* BGTypeID */, uint8 joinAsGroup, uint8 /* IsRated */, GroupJoinBattlegroundResult& err) override
    {
        if (!player)
            return false;

        if (!joinAsGroup && !sAZTH->GetAZTHPlayer(player)->canJoinQueue(AZTH_QUEUE_ARENA))
        {
            err = ERR_BATTLEGROUND_NONE;
            return false;
        }

        return true;
    }

    bool CanGroupInvite(Player* player, std::string& membername) override
    {
        if (!sAZTH->GetAZTHPlayer(player)->canGroup(player))
        {
            player->GetSession()->SendPartyResult(PARTY_OP_INVITE, membername, ERR_BAD_PLAYER_NAME_S);
            return false;
        }

        return true;
    }

    bool CanGroupAccept(Player* player, Group* /*group*/) override
    {
        if (!sAZTH->GetAZTHPlayer(player)->canGroup(player))
        {
            player->GetSession()->SendPartyResult(PARTY_OP_INVITE, "", ERR_BAD_PLAYER_NAME_S);
            return false;
        }

        return true;
    }

    bool CanSellItem(Player* player, Item* item, Creature* creature) override
    {
        if (item->HasFlag(ITEM_FIELD_FLAGS, ITEM_FIELD_FLAG_UNK1))
        {
            player->SendSellError(SELL_ERR_CANT_SELL_ITEM, creature, item->GetGUID(), 0);
            return false;
        }

        return true;
    }

    bool CanSendMail(Player* player, ObjectGuid /*receiverGuid*/, ObjectGuid /*mailbox*/, std::string& /*subject*/, std::string& /*body*/, uint32 /*money*/, uint32 /*COD*/, Item* /*item*/) override
    {
        if (sAZTH->GetAZTHPlayer(player)->isPvP())
        {
            player->SendMailResult(0, MAIL_SEND, MAIL_ERR_DISABLED_FOR_TRIAL_ACC);
            return false;
        }

        return true;
    }

    void PetitionBuy(Player* player, Creature* /*creature*/, uint32& charterid, uint32& cost, uint32& /* type */) override
    {
        if (!player)
            return;

        bool IsPvP = sAZTH->GetAZTHPlayer(player)->isPvP();

        switch (charterid) // arenaSlot+1 as received from client (1 from 3 case)
        {
        case ARENA_TEAM_CHARTER_2v2:
            cost = IsPvP ? 0 : sWorld->getIntConfig(CONFIG_CHARTER_COST_ARENA_2v2);
            break;
        case ARENA_TEAM_CHARTER_3v3:
            cost = IsPvP ? 0 : sWorld->getIntConfig(CONFIG_CHARTER_COST_ARENA_3v3);
            break;
        case ARENA_TEAM_CHARTER_5v5:
            cost = IsPvP ? 0 : sWorld->getIntConfig(CONFIG_CHARTER_COST_ARENA_5v5);
            break;
        default:
            return;
        }
    }

    void PetitionShowList(Player* player, Creature* /* creature */, uint32& CharterEntry, uint32& /* CharterDispayID */, uint32& CharterCost) override
    {
        if (!player)
            return;

        bool IsPvP = sAZTH->GetAZTHPlayer(player)->isPvP();

        switch (CharterEntry)
        {
        case ARENA_TEAM_CHARTER_2v2:
            CharterCost = IsPvP ? 0 : sWorld->getIntConfig(CONFIG_CHARTER_COST_ARENA_2v2);
            break;
        case ARENA_TEAM_CHARTER_3v3:
            CharterCost = IsPvP ? 0 : sWorld->getIntConfig(CONFIG_CHARTER_COST_ARENA_3v3);
            break;
        case ARENA_TEAM_CHARTER_5v5:
            CharterCost = IsPvP ? 0 : sWorld->getIntConfig(CONFIG_CHARTER_COST_ARENA_5v5);
            break;
        default:
            return;
        }
    }

    bool CanJoinLfg(Player* player, uint8 /*roles*/, lfg::LfgDungeonSet& /*dungeons*/, const std::string& /*comment*/) override
    {
        if (!player)
            return false;

        if (!sAZTH->GetAZTHPlayer(player)->canJoinQueue(AZTH_QUEUE_LFG))
            return false;

        return true;
    }

    bool CanInitTrade(Player* player, Player* target) override
    {
        if (sAZTH->GetAZTHPlayer(player)->isPvP() != sAZTH->GetAZTHPlayer(target)->isPvP())
        {
            player->GetSession()->SendTradeStatus(TRADE_STATUS_NOT_ELIGIBLE);
            return false;
        }

        return true;
    }

    bool CanEnterMap(Player* player, MapEntry const* entry, InstanceTemplate const* instance, MapDifficulty const* /*mapDiff*/, bool loginCheck) override
    {
        if (!sAZTH->GetAZTHPlayer(player)->canEnterMap(entry, instance, loginCheck))
            return false;

        return true;
    }

    void OnSetServerSideVisibility(Player* player, ServerSideVisibilityType& type, AccountTypes& sec) override
    {
        if (!player || type != SERVERSIDE_VISIBILITY_GM || sec == SEC_PLAYER)
            return;

        //[AZTH] xeela
        // if (sec < SEC_ENTERTAINER)
        //     sec = SEC_ENTERTAINER;
    }

    void OnSetServerSideVisibilityDetect(Player* player, ServerSideVisibilityType& type, AccountTypes& sec) override
    {
        if (!player || type != SERVERSIDE_VISIBILITY_GM || sec == SEC_PLAYER)
            return;

        //[AZTH] xeela
        // if (sec < SEC_ENTERTAINER)
        //     sec = SEC_ENTERTAINER;
    }
};

class Achievement_SC : public AchievementScript
{
public:
    Achievement_SC() : AchievementScript("Achievement_SC") { }

    void SetRealmCompleted(AchievementEntry const* achievement) override
    {
        sAzthFirstKills->setRealmCompleted(achievement);
    }

    bool IsCompletedCriteria(AchievementMgr* /*mgr*/, AchievementCriteriaEntry const* /*achievementCriteria*/, AchievementEntry const* achievement, CriteriaProgress const* progress) override
    {
        //[AZTH] if criteria has been achieved before this year, then it's not valid for our first kill system
        if (achievement->flags & (ACHIEVEMENT_FLAG_REALM_FIRST_REACH | ACHIEVEMENT_FLAG_REALM_FIRST_KILL) && progress->date < sAzthUtils->getStartsOfYear())
            return false;

        return true;
    }

    bool IsRealmCompleted(AchievementGlobalMgr const* /*globalmgr*/, AchievementEntry const* achievement, std::chrono::system_clock::time_point completionTime) override
    {
        if (!achievement)
            return false;

        using namespace std::chrono;

        //[AZTH] do not continue, do our check instead
        if (achievement->flags & ACHIEVEMENT_FLAG_REALM_FIRST_KILL)
            return sAzthFirstKills->isRealmCompleted(achievement, ((completionTime == system_clock::time_point::max()) || (system_clock::now() - completionTime) > minutes(1)));

        return true;
    }

    void OnBeforeCheckCriteria(AchievementMgr* mgr, AchievementCriteriaEntryList const* achievementCriteriaList) override
    {
        std::vector<uint32> hsCheckList; //[AZTH]

        for (auto itr = achievementCriteriaList->begin(); itr != achievementCriteriaList->end(); ++itr)
            sHearthstoneMode->sendQuestCredit(mgr->GetPlayer(), *itr, hsCheckList);
    }

    bool CanCheckCriteria(AchievementMgr* mgr, AchievementCriteriaEntry const* achievementCriteria) override
    {
        if (!sAZTH->GetAZTHPlayer(mgr->GetPlayer())->canCompleteCriteria(achievementCriteria))
            return false;

        return true;
    }
};

class Misc_SC : public MiscScript
{
public:
    Misc_SC() : MiscScript("Misc_SC") { }

    void OnAfterLootTemplateProcess(Loot* loot, LootTemplate const* tab, LootStore const& store, Player* lootOwner, bool /* personal */, bool /* noEmptyError */, uint16 lootMode) override
    {
        if (!sConfigMgr->GetBoolDefault("Azth.Multiplies.Drop.Enable", false))
            return;

        //Dangerous since it can drops multiple quest items
        //[AZTH] give another loot process if done with correct level
        if (sAzthUtils->isEligibleForBonusByArea(lootOwner) && (&store == &LootTemplates_Gameobject || &store == &LootTemplates_Creature))
        {
            sAZTH->AddAZTHLoot(loot);
            tab->Process(*loot, store, lootMode, lootOwner);
        }
    }

    // For WoW Armory
    void OnItemCreate(Item* item, ItemTemplate const* itemProto, Player const* /* owner */) override
    {
        if (itemProto->Quality > 2 && itemProto->Flags != 2048 && (itemProto->Class == ITEM_CLASS_WEAPON || itemProto->Class == ITEM_CLASS_ARMOR))
        {
            if (!item->GetOwner())
                return;

            sAZTH->GetAZTHPlayer(item->GetOwner())->CreateWowarmoryFeed(2, itemProto->ItemId, item->GetGUID().GetCounter(), itemProto->Quality);
        }
    }

    //[AZTH] Timewalking (maybe)
    bool CanItemApplyEquipSpell(Player* player, Item* item) override
    {
        if (!player || item)
            return false;

        ItemTemplate const* proto = item->GetTemplate();
        if (!proto)
            return false;

        if (!sAZTH->GetAZTHPlayer(player)->itemCheckReqLevel(proto))
            return false;

        return true;
    }

    bool CanSendAuctionHello(WorldSession const* session, ObjectGuid /* guid */, Creature* /* creature */) override
    {
        if (!session)
            return false;

        Player* player = session->GetPlayer();
        if (!player)
            return false;

        if (sAZTH->GetAZTHPlayer(session->GetPlayer())->isPvP())
        {
            player->GetSession()->SendNotification("This is a Full PvP Account! You cannot use the Auction House.");
            return false;
        }

        return true;
    }

    void ValidateSpellAtCastSpell(Player* player, uint32& /* oldSpellId */, uint32& spellId, uint8& /*castCount*/, uint8& /*castFlags*/) override
    {
        if (!player)
            return;

        spellId = sAzthUtils->selectSpellForTW(player, spellId);
    }

    void ValidateSpellAtCastSpellResult(Player* player, Unit* /* mover */, Spell* spell, uint32 oldSpellId, uint32 spellId) override
    {
        if (!player)
            return;

        //[AZTH] Timewalking: we must send a spell cast result on prev spell
        // to avoid bad visual effect in spell bar
        if (oldSpellId != spellId)
            spell->SetSpellInfo(sSpellMgr->GetSpellInfo(oldSpellId));
    }

    void OnInstanceSave(InstanceSave* instanceSave) override
    {
        sAZTH->GetAZTHInstanceSave(instanceSave)->saveToDb();
    }

    bool CanApplySoulboundFlag(Item* item, ItemTemplate const* /*proto*/) override
    {
        if (!item)
            return false;

        // [AZTH] force soulbound using an unknown flag as workaround
        if (item->HasFlag(ITEM_FIELD_FLAGS, ITEM_FIELD_FLAG_UNK1))
            return false;

        return true;
    }

    void OnPlayerSetPhase(const AuraEffect* auraEff, AuraApplication const* aurApp, uint8 /*mode*/, bool /*apply*/, uint32& newPhase) override
    {
        if (!auraEff)
            return;

        Player* player = aurApp->GetTarget()->ToPlayer();
        if (!player)
            return;

        uint32 auraPhase = aurApp->GetTarget()->GetPhaseByAuras();

        if (player->IsGameMaster() && newPhase == PHASEMASK_ANYWHERE && !player->HasFlag(PLAYER_FLAGS, PLAYER_FLAGS_DEVELOPER))
            newPhase = !auraPhase ? PHASEMASK_NORMAL : auraPhase;
    }
};

class Unit_SC : public UnitScript
{
public:
    Unit_SC() : UnitScript("Unit_SC") { }

    void OnAuraRemove(Unit* /* unit */, AuraApplication* aurApp, AuraRemoveMode mode) override
    {
        sAzthUtils->onAuraRemove(aurApp, mode);
    }

    bool IfNormalReaction(Unit const* unit, Unit const* target, ReputationRank& repRank) override
    {
        int32 _repRank = sAzthUtils->getReaction(unit, target);
        if (_repRank >= 0)
        {
            repRank = (ReputationRank)_repRank;
            return false;
        }

        return true;
    }

    bool IsNeedModSpellDamagePercent(Unit const* unit, AuraEffect* auraEff, float& doneTotalMod, SpellInfo const* spellProto) override
    {
        //[AZTH] Timewalking scaled damage spells shouldn't have the
        // percent reduction of tw table, but we can apply a minor modifier
        Player* modOwner = unit->GetSpellModOwner();
        if (modOwner && sAZTH->GetAZTHPlayer(modOwner)->isTimeWalking(true) && auraEff->GetId() == TIMEWALKING_AURA_MOD_DAMAGESPELL)
        {
            int32 reduction = sAzthUtils->getSpellReduction(modOwner, spellProto);
            if (reduction >= 0)
            {
                // replicate conditions below
                if (auraEff->GetMiscValue() & spellProto->GetSchoolMask())
                {
                    if (auraEff->GetSpellInfo()->EquippedItemClass == -1)
                        AddPct(doneTotalMod, -(reduction));
                    else if (!auraEff->GetSpellInfo()->HasAttribute(SPELL_ATTR5_AURA_AFFECTS_NOT_JUST_REQ_EQUIPED_ITEM) && (auraEff->GetSpellInfo()->EquippedItemSubClassMask == 0))
                        AddPct(doneTotalMod, -(reduction));
                    else if (unit->ToPlayer() && unit->ToPlayer()->HasItemFitToSpellRequirements(auraEff->GetSpellInfo()))
                        AddPct(doneTotalMod, -(reduction));
                }
                return false;
            }
        }

        return true;
    }

    bool IsNeedModMeleeDamagePercent(Unit const* unit, AuraEffect* auraEff, float& doneTotalMod, SpellInfo const* spellProto) override
    {
        //[AZTH] Timewalking scaled damage spells shouldn't have the
        // percent reduction of tw table, but we can apply a minor modifier
        Player* modOwner = unit->GetSpellModOwner();
        if (modOwner && sAZTH->GetAZTHPlayer(modOwner)->isTimeWalking(true) && auraEff->GetId() == TIMEWALKING_AURA_MOD_DAMAGESPELL)
        {
            int32 reduction = sAzthUtils->getSpellReduction(modOwner, spellProto);
            if (reduction >= 0)
            {
                //  replicate conditions below
                if ((auraEff->GetMiscValue() & spellProto->GetSchoolMask()) && !(auraEff->GetMiscValue() & SPELL_SCHOOL_MASK_NORMAL))
                {
                    if (auraEff->GetSpellInfo()->EquippedItemClass == -1)
                        AddPct(doneTotalMod, -(reduction));
                    else if (!auraEff->GetSpellInfo()->HasAttribute(SPELL_ATTR5_AURA_AFFECTS_NOT_JUST_REQ_EQUIPED_ITEM) && (auraEff->GetSpellInfo()->EquippedItemSubClassMask == 0))
                        AddPct(doneTotalMod, -(reduction));
                    else if (unit->ToPlayer() && unit->ToPlayer()->HasItemFitToSpellRequirements(auraEff->GetSpellInfo()))
                        AddPct(doneTotalMod, -(reduction));
                }

                return false;
            }
        }

        return true;
    }

    bool IsNeedModHealPercent(Unit const* unit, AuraEffect* auraEff, float& doneTotalMod, SpellInfo const* spellProto) override
    {
        //[AZTH] Timewalking scaled healing spells shouldn't have the
        // percent reduction of tw table, but we can apply a minor modifier
        Player* modOwner = unit->GetSpellModOwner();
        if (modOwner && sAZTH->GetAZTHPlayer(modOwner)->isTimeWalking(true) && auraEff->GetId() == TIMEWALKING_AURA_MOD_HEALING)
        {
            int32 reduction = sAzthUtils->getSpellReduction(modOwner, spellProto);
            if (reduction >= 0)
            {
                AddPct(doneTotalMod, -(reduction));
                return false;
            }
        }

        return true;
    }

    bool CanSetPhaseMask(Unit const* unit, uint32 newPhaseMask, bool /*update*/) override
    {
        if (!sAzthUtils->dimIntegrityCheck(unit, newPhaseMask))
            return false;

        if (newPhaseMask != uint32(PHASEMASK_ANYWHERE))
            return true;

        auto player = unit->ToPlayer();
        if (!player)
            return false;

        if (!player->HasFlag(PLAYER_FLAGS, PLAYER_FLAGS_DEVELOPER))
            return false;

        return true;
    }

    bool IsCustomBuildValuesUpdate(Unit const* unit, uint8 /*updateType*/, ByteBuffer& fieldBuffer, Player const* target, uint16 index) override
    {
        int repRank = sAzthUtils->getReaction(unit, target);

        if (target != unit && repRank >= REP_HATED)
        {
            // implicit change of unit fields from azth
            FactionTemplateEntry const* ft1 = unit->GetFactionTemplateEntry();
            FactionTemplateEntry const* ft2 = target->GetFactionTemplateEntry();

            if (repRank >= REP_FRIENDLY && ft1 && ft2 && !ft1->IsFriendlyTo(*ft2))
            {
                if (index == UNIT_FIELD_BYTES_2) // Allow targetting opposite faction in party when enabled in config
                    fieldBuffer << (unit->GetUInt32Value(UNIT_FIELD_BYTES_2) & ((UNIT_BYTE2_FLAG_SANCTUARY) << 8)); // this flag is at uint8 offset 1 !!
                else // pretend that all other HOSTILE players have own faction, to allow follow, heal, rezz (trade wont work)
                    fieldBuffer << uint32(target->getFaction());

                return true;
            }
            else if (repRank <= REP_HOSTILE && ft1 && ft2 && ft1->IsFriendlyTo(*ft2))
            {
                // this needs that the realm has the PvP switch (rp pvp realm type for example) to work
                fieldBuffer << (unit->GetUInt32Value(UNIT_FIELD_BYTES_2) | (UNIT_BYTE2_FLAG_FFA_PVP << 8));
                return true;
            }
        }

        return false;
    }
};

class Group_SC : public GroupScript
{
public:
    Group_SC() : GroupScript("Group_SC") { }

    bool CanGroupJoinBattlegroundQueue(Group const* group, Player* member, Battleground const* bgTemplate, uint32 /*MinPlayerCount*/, bool /*isRated*/, uint32 /*arenaSlot*/) override
    {
        if (!group || !member)
            return false;

        if (!sAZTH->GetAZTHPlayer(member)->canJoinQueue(bgTemplate->isArena() ? AZTH_QUEUE_ARENA : AZTH_QUEUE_BG))
            return false;

        return true;
    }

    void OnCreate(Group* group, Player* leader) override
    {
        if (!group || !leader)
            return;

        CharacterDatabase.PExecute("INSERT INTO azth_groups (guid, MaxLevelGroup) VALUES (%u, %u)", group->GetGUID().GetCounter(), leader->getLevel());
    }

    void OnDisband(Group* group) override
    {
        if (!group)
            return;

        CharacterDatabase.PExecute("DELETE FROM `azth_groups` WHERE `guid` = '%u'", group->GetGUID().GetCounter());
    }
};

class Guild_SC : public GuildScript
{
public:
    Guild_SC() : GuildScript("Guild_SC") { }

    bool CanGuildSendBankList(Guild const* guild, WorldSession* session, uint8 /* tabId */, bool /* sendAllSlots */) override
    {
        if (!guild || session)
            return false;

        if (session->GetPlayer() && sAZTH->GetAZTHPlayer(session->GetPlayer())->isPvP())
            return false;

        return true;
    }
};

class World_SC : public WorldScript
{
public:
    World_SC() : WorldScript("World_SC") { }

    void OnAfterConfigLoad(bool reload) override
    {
        sAZTH->LoadConfig(reload);
    }

    void OnUpdate(uint32 diff) override
    {
        if (_TimeCheckEM < diff)
        {
            sAZTH->SendExternalMails();

            _TimeCheckEM = sAZTH->GetExternalMailInterval();
        }
        else
            _TimeCheckEM -= diff;

        if (_TimeCheckGH <= diff)
        {
            GHobj.ControlGuildHouse();
            _TimeCheckGH = 600000;
        }
        else
            _TimeCheckGH -= diff;
    }

    void OnStartup() override
    {
        GHobj.LoadGuildHouseSystem();           // [AZTH] Load Guildhouses
        nsNpcTel::LoadNpcTele();                // Teleport
        sHearthstoneMode->loadHearthstone();    // [AZTH] Load Hearthstone Mode
    }

private:
    uint32 _TimeCheckEM = sAZTH->GetExternalMailInterval();
    uint32 _TimeCheckGH = 600000;
};

class Pet_SC : public PetScript
{
public:
    Pet_SC() : PetScript("Pet_SC") { }

    //[AZTH] Timewalking
    void OnInitStatsForLevel(Guardian* guardian, uint8 /*petlevel*/) override
    {
        if (!guardian->GetOwner())
            return;

        if (guardian->GetOwner()->GetTypeId() == TYPEID_PLAYER)
        {
            Player* player = guardian->GetOwner()->ToPlayer();
            if (!player)
                return;

            if (sAZTH->GetAZTHPlayer(player)->isTimeWalking(guardian))
                if (AzthLevelStat const* stats = sAzthUtils->getTwStats(player, player->getLevel()))
                    sAzthUtils->setTwAuras(guardian, stats, true, true);
        }
    }

    //[AZTH] Timewalking
    void OnCalculateMaxTalentPointsForLevel(Pet* pet, uint8 /*level*/, uint8& points) override
    {
        Unit* owner = pet->GetOwner();
        if (!owner || owner->GetTypeId() != TYPEID_PLAYER)
            return;

        if (!sAZTH->GetAZTHPlayer(owner->ToPlayer())->isTimeWalking())
            return;

        uint32 ConfMaxPlayerLevel = sWorld->getIntConfig(CONFIG_MAX_PLAYER_LEVEL);

        points = (ConfMaxPlayerLevel >= 20) ? ((ConfMaxPlayerLevel - 16) / 4) : 0;

        // Mod points from owner SPELL_AURA_MOD_PET_TALENT_POINTS
        points += owner->GetTotalAuraModifier(SPELL_AURA_MOD_PET_TALENT_POINTS);
    }

    //[AZTH] Timewalking
    bool CanUnlearnSpellSet(Pet* pet, uint32 /*level*/, uint32 /*spell*/) override
    {
        if (pet->GetOwner() && sAZTH->GetAZTHPlayer(pet->GetOwner())->isTimeWalking())
            return false;

        return true;
    }

    //[AZTH] Timewalking
    bool CanUnlearnSpellDefault(Pet* pet, SpellInfo const* /*spellEntry*/) override
    {
        if (pet->GetOwner() && sAZTH->GetAZTHPlayer(pet->GetOwner())->isTimeWalking())
            return false;

        return true;
    }

    //[AZTH] Timewalking
    bool CanResetTalents(Pet* pet) override
    {
        Player* player = pet->GetOwner()->ToPlayer();
        if (!player)
            return false;

        if (sAZTH->GetAZTHPlayer(player)->isTimeWalking())
            return false;

        return true;
    }
};

class Spell_SC : public SpellSC
{
public:
    Spell_SC() : SpellSC("Spell_SC") { }

    bool CanModAuraEffectDamageDone(AuraEffect const* auraEff, Unit* /* target */, AuraApplication const* /* aurApp */, uint8 /* mode */, bool /* apply */) override
    {
        if (auraEff->GetSpellInfo()->Id == TIMEWALKING_AURA_MOD_DAMAGESPELL)
            return false;

        return true;
    }

    bool CanModAuraEffectModDamagePercentDone(AuraEffect const* auraEff, Unit* /* target */, AuraApplication const* /* aurApp */, uint8 /* mode */, bool /* apply */) override
    {
        //[AZTH] weapon damage is already handled by our item scaling system
        // but we need other effect of MOD_DAMAGE_PERCENT with SPELL_SCHOOL_MASK_NORMAL (physic spells)
        if (auraEff->GetSpellInfo()->Id == TIMEWALKING_AURA_MOD_DAMAGESPELL)
            return false;

        return true;
    }

    void OnSpellCheckCast(Spell* spell, bool /* strict */, SpellCastResult& res) override
    {
        Player* player = spell->GetCaster()->GetSpellModOwner();
        if (!player)
            return;

        res = sAzthUtils->checkSpellCast(player, spell->GetSpellInfo(), true);
    }

    bool CanPrepare(Spell* spell, SpellCastTargets const* targets, AuraEffect const* triggeredByAura) override
    {
        if (!spell)
            return false;

        if (!sAzthUtils->canPrepareSpell(spell, spell->GetCaster(), spell->GetSpellInfo(), targets, triggeredByAura))
            return false;

        return true;
    }

    bool CanScalingEverything(Spell* spell) override
    {
        //[AZTH] timewalking scale everything!!!
        if (spell->GetCaster()->ToPlayer() && sAZTH->GetAZTHPlayer(spell->GetCaster()->ToPlayer())->isTimeWalking(true))
            return true;

        return false;
    }

    bool CanSelectSpecTalent(Spell* spell) override
    {
        if (!spell)
            return false;

        if (spell->GetCaster()->GetTypeId() == TYPEID_PLAYER)
        {
            Player* plr = spell->GetCaster()->ToPlayer();

            if (plr->InBattlegroundQueueForBattlegroundQueueType((BattlegroundQueueTypeId)BATTLEGROUND_QUEUE_3v3_SOLO) ||
                plr->InBattlegroundQueueForBattlegroundQueueType((BattlegroundQueueTypeId)BATTLEGROUND_QUEUE_1v1))
            {
                plr->GetSession()->SendAreaTriggerMessage("You can't change your talents while in queue for 1v1 or 3v3.");
                return false;
            }
        }

        return true;
    }

    void OnScaleAuraUnitAdd(Spell* spell, Unit* target, uint32 /*effectMask*/, bool /*checkIfValid*/, bool /*implicit*/, uint8 auraScaleMask, TargetInfo& targetInfo) override
    {
        if (!spell || !target)
            return;

        if (!targetInfo.scaleAura && auraScaleMask && targetInfo.effectMask == auraScaleMask)
        {
            SpellInfo const* auraSpell = spell->GetSpellInfo()->GetFirstRankSpell();
            if (uint32(target->getLevel() + 10) >= auraSpell->SpellLevel)
                targetInfo.scaleAura = true;
        }
    }

    void OnRemoveAuraScaleTargets(Spell* spell, TargetInfo& targetInfo, uint8 /* auraScaleMask */, bool& needErase) override
    {
        if (!spell)
            return;

        // TW: Do not check for selfcast
        if (needErase && !targetInfo.scaleAura && sAzthUtils->isSpecialSpellForTw(spell->m_spellInfo))
            needErase = false;
    }

    void OnBeforeAuraRankForLevel(SpellInfo const* spellInfo, SpellInfo const* nextSpellInfo, uint8 level) override
    {
        SpellInfo const* latestSpellInfo = spellInfo; // we can use after
        for (SpellInfo const* latestSpellInfo = spellInfo; latestSpellInfo != nullptr; latestSpellInfo = latestSpellInfo->GetPrevRankSpell())
        {
            // Timewalking
            if (!latestSpellInfo->SpellLevel && uint32(level) >= latestSpellInfo->BaseLevel)
            {
                nextSpellInfo = latestSpellInfo;
                return;
            }
            else if (uint32(level) >= latestSpellInfo->SpellLevel)
            {
                // if found appropriate level
                nextSpellInfo = latestSpellInfo;
                return;
            }
        }

        // if any low level found, we could pass the first
        // one that is in a 10 level higher range as official code did
        if (uint32(level + 10) >= latestSpellInfo->SpellLevel)
        {
            nextSpellInfo = latestSpellInfo;
            return;
        }
    }
};

class CommandAZTH_SC : public CommandScript
{
public:
    CommandAZTH_SC() : CommandScript("CommandAZTH_SC") { }

    std::vector<ChatCommand> GetCommands() const override
    {
        static std::vector<ChatCommand> AZTHGOCommandTable =
        {
            { "guildhouse",     SEC_GAMEMASTER,     false, &HandleGuildhouseCommand,            "" }
        };

        static std::vector<ChatCommand> AZTHGobjectCommandTable =
        {
            { "guildadd",       SEC_GAMEMASTER,		false, &HandleGameObjectAddGuildCommand,    "" }
        };

        static std::vector<ChatCommand> AZTHNpcCommandTable =
        {
            { "guildadd",       SEC_GAMEMASTER,		false, &HandleNpcAddGuildCommand,           "" }
        };

        static std::vector<ChatCommand> AZTHCommandTable =
        {
            { "go",             SEC_MODERATOR,      false,   nullptr,                           "",  AZTHGOCommandTable},
            { "gobject",        SEC_MODERATOR,		false,   nullptr,                           "",  AZTHGobjectCommandTable},
            { "npc",            SEC_MODERATOR,		false,   nullptr,                           "",  AZTHNpcCommandTable},
        };

        static std::vector<ChatCommand> commandTable =
        {
            { "azth",           SEC_PLAYER,         true,   nullptr,                            "",  AZTHCommandTable},
        };

        return commandTable;
    }

    // [AZTH] Guildhouse GO command
    static bool HandleGuildhouseCommand(ChatHandler* handler, char const* args)
    {
        if (!*args)
            return false;

        Player* player = handler->GetSession()->GetPlayer();

        char* goGH = strtok((char*)args, " ");

        if (!goGH)
            return false;

        uint32 guildhouse = (uint32)atof(goGH);
        if (!guildhouse)
            return false;

        float x;
        float y;
        float z;
        float o;
        uint32 map;

        // here i have to retrieve coordinates for the GH.
        if (!GHobj.GetGuildHouseLocation(guildhouse, x, y, z, o, map))
            return false;

        // stop flight if need
        if (player->IsInFlight())
        {
            player->GetMotionMaster()->MovementExpired();
            player->CleanupAfterTaxiFlight();
        }
        else // save only in non-flight case
            player->SaveRecallPosition();

        if (!MapMgr::IsValidMapCoord(map, x, y, z))
        {
            handler->PSendSysMessage(LANG_INVALID_TARGET_COORD, x, y, map);
            handler->SetSentErrorMessage(true);
            return false;
        }

        // teleport player to the specified location
        player->TeleportTo(map, x, y, z, o);
        return true;
    }

    // [AZTH] Gobject guildhouse add
    static bool HandleGameObjectAddGuildCommand(ChatHandler* handler, const char* args)
    {
        if (!*args)
            return false;

        // number or [name] Shift-click form |color|Hgameobject_entry:go_id|h[name]|h|r
        char* cId = handler->extractKeyFromLink((char*)args, "Hgameobject_entry");
        if (!cId)
            return false;

        char* c_guildhouse = strtok(NULL, " ");
        if (!c_guildhouse)
            return false;

        char* c_guildhouseadd = strtok(NULL, " ");
        if (!c_guildhouseadd)
            return false;

        uint32 id = atol(cId);
        uint32 guildhouseid = atol(c_guildhouse);
        uint32 guildhouseaddid = atol(c_guildhouseadd);

        if (!id || !guildhouseid || !guildhouseaddid)
            return false;

        char* spawntimeSecs = strtok(NULL, " ");

        const GameObjectTemplate* gInfo = sObjectMgr->GetGameObjectTemplate(id);

        if (!gInfo)
        {
            handler->PSendSysMessage(LANG_GAMEOBJECT_NOT_EXIST, id);
            handler->SetSentErrorMessage(true);
            return false;
        }

        if (gInfo->displayId && !sGameObjectDisplayInfoStore.LookupEntry(gInfo->displayId))
        {
            // report to DB errors log as in loading case
            //sLog->outErrorDb("Gameobject (Entry %u GoType: %u) have invalid displayId (%u), not spawned.", id, gInfo->type, gInfo->displayId);
            handler->PSendSysMessage(LANG_GAMEOBJECT_HAVE_INVALID_DATA, id);
            handler->SetSentErrorMessage(true);
            return false;
        }

        Player* chr = handler->GetSession()->GetPlayer();
        float x = float(chr->GetPositionX());
        float y = float(chr->GetPositionY());
        float z = float(chr->GetPositionZ());
        float o = float(chr->GetOrientation());
        Map* map = chr->GetMap();

        GameObject* pGameObj = new GameObject;
        uint32 db_lowGUID = map->GenerateLowGuid<HighGuid::GameObject>();

        if (!pGameObj->Create(db_lowGUID, gInfo->entry, map, chr->GetPhaseMaskForSpawn(), x, y, z, o, G3D::Quat(), 0, GO_STATE_READY))
        {
            delete pGameObj;
            return false;
        }

        if (spawntimeSecs)
        {
            uint32 value = atoi((char*)spawntimeSecs);
            pGameObj->SetRespawnTime(value);
            //sLog->outDebug(LOG_FILTER_TSCR, "*** spawntimeSecs: %d", value);
        }

        // fill the gameobject data and save to the db
        pGameObj->SaveToDB(map->GetId(), (1 << map->GetSpawnMode()), chr->GetPhaseMaskForSpawn());

        // this will generate a new guid if the object is in an instance
        if (!pGameObj->LoadFromDB(db_lowGUID, map))
        {
            delete pGameObj;
            return false;
        }

        map->AddToMap(pGameObj);

        std::string new_str(pGameObj->GetName());
        WorldDatabase.EscapeString(new_str);

        WorldDatabase.PQuery("INSERT INTO `guildhouses_add` (guid, type, id, add_type, comment) VALUES (%u, 1, %u, %u, '%s')",
            pGameObj->GetGUID().GetCounter(), guildhouseid, guildhouseaddid, new_str.c_str());

        // TODO: is it really necessary to add both the real and DB table guid here ?
        sObjectMgr->AddGameobjectToGrid(db_lowGUID, sObjectMgr->GetGOData(db_lowGUID));

        handler->PSendSysMessage(LANG_GAMEOBJECT_ADD, id, gInfo->name.c_str(), db_lowGUID, x, y, z);
        return true;
    }

    // [AZTH] add spawn of creature for a GuildHouse
    static bool HandleNpcAddGuildCommand(ChatHandler* handler, const char* args)
    {
        if (!*args)
            return false;

        char* charID = handler->extractKeyFromLink((char*)args, "Hcreature_entry");
        if (!charID)
            return false;

        char* guildhouse = strtok(NULL, " ");
        if (!guildhouse)
            return false;

        char* guildhouseadd = strtok(NULL, " ");
        if (!guildhouseadd)
            return false;

        uint32 id = atoi(charID);
        uint32 guildhouseid = atoi(guildhouse);
        uint32 guildhouseaddid = atoi(guildhouseadd);

        if (!id || !guildhouseid || !guildhouseaddid)
            return false;

        Player* chr = handler->GetSession()->GetPlayer();
        float x = chr->GetPositionX();
        float y = chr->GetPositionY();
        float z = chr->GetPositionZ();
        float o = chr->GetOrientation();
        Map* map = chr->GetMap();

        Creature* creature = new Creature();
        if (!creature->Create(map->GenerateLowGuid<HighGuid::Unit>(), map, chr->GetPhaseMaskForSpawn(), id, 0, x, y, z, o))
        {
            delete creature;
            return false;
        }

        creature->SaveToDB(map->GetId(), (1 << map->GetSpawnMode()), chr->GetPhaseMaskForSpawn());

        uint32 db_guid = creature->GetGUID().GetCounter();

        // To call _LoadGoods(); _LoadQuests(); CreateTrainerSpells();
        creature->LoadFromDB(db_guid, map);

        std::string new_str(creature->GetName());
        WorldDatabase.EscapeString(new_str);

        WorldDatabase.PQuery("INSERT INTO `guildhouses_add` (guid, type, id, add_type, comment) VALUES (%u, 0, %u, %u, '%s')",
            creature->GetGUID().GetCounter(), guildhouseid, guildhouseaddid, new_str.c_str());

        map->AddToMap(creature);
        sObjectMgr->AddCreatureToGrid(db_guid, sObjectMgr->GetCreatureData(db_guid));

        if (guildhouseaddid == 2)
        {
            QueryResult guildResult = ExtraDatabase.PQuery("SELECT guildid FROM `guildhouses` WHERE id = %u", guildhouseid);
            if (guildResult)
                GHobj.UpdateGuardMap(creature->GetGUID().GetCounter(), guildResult->Fetch()->GetInt32());
        }

        return true;
    }
};

void AddSC_AZTH()
{
    new Arena_SC();
    new Battleground_SC();
    new Command_SC();
    new Player_SC();
    new Achievement_SC();
    new Misc_SC();
    new Unit_SC();
    new Group_SC();
    new Guild_SC();
    new World_SC();
    new Pet_SC();
    new Spell_SC();
    new CommandAZTH_SC();
}
