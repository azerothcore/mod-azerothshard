#include "ExtraDatabase.h"
#include "ScriptMgr.h"
#include "Unit.h"
#include "SharedDefines.h"
#include "Player.h"
#include "Common.h"
#include "raid.h"
#include "Creature.h"
#include "AzthPlayer.h"
#include "AzthLevelStat.h"
#include "AzthUtils.h"
#include "AzthAchievement.h"
#include "Pet.h"
#include "ScriptedGossip.h"
#include "LFG.h"
#include "LFGMgr.h"
#include "AutoBalance.h"
#include "MapManager.h"
#include "ReputationMgr.h"
#include "AZTH.h"

class ReputationMgr;

typedef std::map<uint32, TwRaid> RaidList;
RaidList raidList;
std::map<uint32, AzthLevelStat> timeWalkingLevelsStatsList;
std::map<uint32, AzthAchievement> azthAchievementList;

enum npc_timewalking_enum
{
    TIMEWALKING_GOSSIP_NPC_TEXT_MAIN = 50100,
    TIMEWALKING_GOSSIP_NPC_TEXT_BONUS = 50101,
    TIMEWALKING_GOSSIP_NPC_TEXT_EXP = 50102,
    TIMEWALKING_GOSSIP_NPC_TEXT_PHASE = 50103,
    TIMEWALKING_GOSSIP_NPC_TEXT_RAID = 50104,
    TIMEWALKING_GOSSIP_NPC_TEXT_ALREADYAPPLIED = 50105,
	TIMEWALKING_GOSSIP_NPC_TEXT_ACTIVELFG = 50106,
	TIMEWALKING_GOSSIP_NPC_TEXT_INGROUP = 50107
};

class MapMythicInfo : public DataMap::Base
{
public:
    MapMythicInfo() {}
    MapMythicInfo(uint32 level) : mythicLevel(level){}
    uint32 mythicLevel = 0;
};

class TWVasScript: public ABModuleScript
{
public:
    TWVasScript(): ABModuleScript("TWVasScript") { }

    bool OnBeforeModifyAttributes(Creature* creature, uint32 & instancePlayerCount) override
    {
        // it doesn't mean that we're allowing it when not in dungeon
        // but we're letting module to decide via its config.
        if (!creature->GetMap()->IsDungeon())
            return true;

        // Flexible content is allowed only for pre-wotlk instances and some specific wotlk
        if (!sAzthUtils->canMythicHere(creature))
            return false;

        Map::PlayerList const &playerList = creature->GetMap()->GetPlayers();
        if (!playerList.isEmpty())
        {
            for (Map::PlayerList::const_iterator playerIteration = playerList.begin(); playerIteration != playerList.end(); ++playerIteration)
            {
                if (Player* playerHandle = playerIteration->GetSource())
                {
                    if (playerHandle->IsGameMaster())
                        continue;

                    uint32 specialLevel = sAZTH->GetAZTHPlayer(playerHandle)->getPStatsLevel(false);

                    MapMythicInfo *myth=creature->GetMap()->CustomData.GetDefault<MapMythicInfo>("AZTH_Mythic_Info");

                    if (sAzthUtils->isMythicLevel(specialLevel))
                    {
                        myth->mythicLevel = specialLevel;
                        return true;
                    }
                    else if (specialLevel == TIMEWALKING_SPECIAL_LVL_MIXED && myth->mythicLevel)
                    {
                        instancePlayerCount = 100; // workaround to break "Flex modality" with mixed group
                        return true;
                    }
                }
            }
        }

        return false;
    }

    bool OnBeforeUpdateStats(Creature* creature, uint32 &scaledHealth, uint32 &scaledMana, float &damageMultiplier, uint32 &newBaseArmor) override
    {
        MapMythicInfo* myth = creature->GetMap()->CustomData.GetDefault<MapMythicInfo>("AZTH_Mythic_Info");

        if (myth->mythicLevel)
        {
            uint32 mythicLvl = myth->mythicLevel - TIMEWALKING_LVL_VAS_START + 1;

            // mythic 1: 1 normal vas rate
            // mythic 13: 3.13 (214%) (for ulduar)
            // mythic 16: 4.17 (318%) (for naxx, eye and obsidian)
            float rate = std::pow(1.1f, mythicLvl-1); // formula from 7.2.5 patch

            scaledHealth *= rate;
            scaledMana   *= rate;
            newBaseArmor += newBaseArmor * (rate / 10);
            damageMultiplier *= rate;

            return true;
        }

        return false; // something wrong....
    }
};

class loadTimeWalkingRaid : public WorldScript
{
public:
    loadTimeWalkingRaid() : WorldScript("loadTimeWalkingRaid") {}
    void OnStartup()
    {

        //Loading timewalking instance
        QueryResult timewalking_table = ExtraDatabase.PQuery("SELECT id,name,exp,phase,level,bonus,criteria FROM timewalking ORDER BY exp, phase, level, name;");
        if (!timewalking_table)
        {
            sLog->outString(">> Loaded 0 raids for TimeWalking. DB table `timewalking` is empty.\n");
            sLog->outString();
            return;
        }

        do
        {
            Field* timeWalking_Field = timewalking_table->Fetch();
            raidList[timeWalking_Field[0].GetUInt32()] = TwRaid(timeWalking_Field[0].GetUInt32(), timeWalking_Field[1].GetString(), timeWalking_Field[2].GetUInt32(), timeWalking_Field[3].GetUInt32(), timeWalking_Field[4].GetUInt32(), timeWalking_Field[5].GetUInt32(), timeWalking_Field[6].GetUInt32());
        } while (timewalking_table->NextRow());

        sAzthRaid->SetRaidList(raidList);

        //-------------------------------------------------------------------
        //Loading levels
        QueryResult timewalkingLevel_table = ExtraDatabase.PQuery("SELECT level,race,class,"
        "strength_pct,agility_pct,stamina_pct,intellect_pct,spirit_pct,"
        "dodge_pct, parry_pct, block_pct, crit_pct, armor_pen_pct, health_pct, resistance_pct, power_cost_pct, stat_pct,"
        "damage_pct,heal_pct FROM timewalking_levels ORDER BY level;");
        if (!timewalkingLevel_table)
        {
            sLog->outString(">> Loaded 0 levels for TimeWalking. DB table `timewalking_levels` is empty.\n");
            sLog->outString();
            return;
        }

        do
        {
            Field* timeWalkingLevel_Field = timewalkingLevel_table->Fetch();

            std::map<aura_timewalking_enum, uint32> _pctMap;
            _pctMap[TIMEWALKING_AURA_MOD_STR_PCT] = timeWalkingLevel_Field[3].GetUInt32();
            _pctMap[TIMEWALKING_AURA_MOD_AGI_PCT] = timeWalkingLevel_Field[4].GetUInt32();
            _pctMap[TIMEWALKING_AURA_MOD_STA_PCT] = timeWalkingLevel_Field[5].GetUInt32();
            _pctMap[TIMEWALKING_AURA_MOD_INT_PCT] = timeWalkingLevel_Field[6].GetUInt32();
            _pctMap[TIMEWALKING_AURA_MOD_SPI_PCT] = timeWalkingLevel_Field[7].GetUInt32();
            _pctMap[TIMEWALKING_AURA_MOD_DODGE_PCT] = timeWalkingLevel_Field[8].GetUInt32();
            _pctMap[TIMEWALKING_AURA_MOD_PARRY_PCT] = timeWalkingLevel_Field[9].GetUInt32();
            _pctMap[TIMEWALKING_AURA_MOD_BLOCK_PCT] = timeWalkingLevel_Field[10].GetUInt32();
            _pctMap[TIMEWALKING_AURA_MOD_CRIT_PCT] = timeWalkingLevel_Field[11].GetUInt32();
            _pctMap[TIMEWALKING_AURA_MOD_ARMOR_PENET_PCT] = timeWalkingLevel_Field[12].GetUInt32();
            _pctMap[TIMEWALKING_AURA_MOD_INCREASE_HEALTH_PCT] = timeWalkingLevel_Field[13].GetUInt32();
            _pctMap[TIMEWALKING_AURA_MOD_RESISTANCE_PCT] = timeWalkingLevel_Field[14].GetUInt32();
            _pctMap[TIMEWALKING_AURA_MOD_POWER_COST_SCHOOL_PCT] = timeWalkingLevel_Field[15].GetUInt32();
            _pctMap[TIMEWALKING_AURA_MOD_STAT_PCT] = timeWalkingLevel_Field[16].GetUInt32();
            _pctMap[TIMEWALKING_AURA_MOD_DAMAGESPELL] = timeWalkingLevel_Field[17].GetUInt32();
            _pctMap[TIMEWALKING_AURA_MOD_HEALING] = timeWalkingLevel_Field[18].GetUInt32();
            timeWalkingLevelsStatsList[timeWalkingLevel_Field[0].GetUInt32()*10000+timeWalkingLevel_Field[1].GetUInt32()*100+timeWalkingLevel_Field[2].GetUInt32()] = AzthLevelStat(timeWalkingLevel_Field[0].GetUInt32(), timeWalkingLevel_Field[1].GetUInt32(), timeWalkingLevel_Field[2].GetUInt32(), _pctMap);
        } while (timewalkingLevel_table->NextRow());

        sAzthLevelStatMgr->SetLevelStatList(timeWalkingLevelsStatsList);

        //-------------------------------------------------------------------
        //Loading achievement
        QueryResult azthAchievement_table = ExtraDatabase.PQuery("SELECT achievement,criteria,Points,category,parentCategory,difficulty,levelMax,levelMin,level,originalPoints,Name,Description,reward,rewardCount,killCredit,specialLevelReq,reqDimension FROM azth_achievements WHERE criteria != 0 ORDER BY achievement;");
        if (!azthAchievement_table)
        {
            sLog->outString(">> Loaded 0 achievements for TimeWalking. DB table `azth_achievements` is empty.\n");
            sLog->outString();
            return;
        }

        do
        {
            Field* azthAchievement_field = azthAchievement_table->Fetch();

            azthAchievementList[azthAchievement_field[1].GetUInt32()] = AzthAchievement(
                // achievement                                criteria                           Points                                 category                           parentCategory                    difficulty
                azthAchievement_field[0].GetUInt32(), azthAchievement_field[1].GetUInt32(), azthAchievement_field[2].GetUInt32(), azthAchievement_field[3].GetUInt32(), azthAchievement_field[4].GetUInt32(), azthAchievement_field[5].GetUInt32(),
                // levelMax                                 levelMin                                    level                              originalPoints                        Name                              Description
                azthAchievement_field[6].GetUInt32(), azthAchievement_field[7].GetUInt32(), azthAchievement_field[8].GetUInt32(), azthAchievement_field[9].GetUInt32(), azthAchievement_field[10].GetString(), azthAchievement_field[11].GetString(),
                //reward                                        rewardCount                             killcredit                      specialLevelReq                         reqDimension
                azthAchievement_field[12].GetUInt32(), azthAchievement_field[13].GetUInt32(), azthAchievement_field[14].GetUInt32(), azthAchievement_field[15].GetUInt32(), azthAchievement_field[16].GetUInt32());
        } while (azthAchievement_table->NextRow());

        sAzthAchievementMgr->achievementList=azthAchievementList;
    }
};

class TimeWalkingGossip : public CreatureScript
{
public:
    TimeWalkingGossip() : CreatureScript("TimeWalkingGossip") {}

    bool OnGossipHello(Player* player, Creature* creature) override
    {
		if (player->isUsingLfg())
		{
			SendGossipMenuFor(player,TIMEWALKING_GOSSIP_NPC_TEXT_ACTIVELFG, creature->GetGUID());
			return true;
		}

        if (player->GetGroup())
        {
            SendGossipMenuFor(player,TIMEWALKING_GOSSIP_NPC_TEXT_INGROUP, creature->GetGUID());
            return true;
        }

        if (!sAZTH->GetAZTHPlayer(player)->isTimeWalking()) {
            if (player->getLevel()>=sWorld->getIntConfig(CONFIG_MAX_PLAYER_LEVEL)) {
                //AddGossipItemFor(player,GOSSIP_ICON_TABARD, "Tutte le fasi", GOSSIP_SENDER_MAIN, 5);
                AddGossipItemFor(player,GOSSIP_ICON_INTERACT_1, "Livello specifico", GOSSIP_SENDER_MAIN, 6, "Imposta un livello", 0, true);
                if (player->IsGameMaster()) {
                    AddGossipItemFor(player,GOSSIP_ICON_TRAINER, "Scaling automatico (beta)", GOSSIP_SENDER_MAIN, TIMEWALKING_LVL_AUTO+10000);
                }
                AddGossipItemFor(player,GOSSIP_ICON_TABARD, "TimeWalking (Wotlk)", GOSSIP_SENDER_MAIN, 4);
            }
            AddGossipItemFor(player,0, "|TInterface/ICONS/INV_Misc_Coin_01:30|tFlex Mythic+ (Beta)", GOSSIP_SENDER_MAIN, 9); // we can't use another icon otherwise will be automatically selected on gossip hello
        } else {
            AddGossipItemFor(player,0, "Esci dalla modalità TimeWalking", GOSSIP_SENDER_MAIN, 7);
        }

        SendGossipMenuFor(player,TIMEWALKING_GOSSIP_NPC_TEXT_MAIN, creature->GetGUID());

        return true;
    }

    bool _is_number(const std::string& s)
    {
        return !s.empty() && std::find_if(s.begin(),
            s.end(), [](char c) { return !isdigit(c); }) == s.end();
    }

    void setTimeWalking(Player* player,uint32 level)
    {
        sAZTH->GetAZTHPlayer(player)->SetTimeWalkingLevel(level, true, true, false);
        player->CastSpell(player, 47292, TRIGGERED_FULL_MASK); // visual
    }

    bool OnGossipSelectCode(Player* player, Creature*  /*creature*/, uint32 sender, uint32 action, const char* code) override
    {
        player->PlayerTalkClass->ClearMenus();

        if (sender != GOSSIP_SENDER_MAIN)
            return false;


        if (action == 6 && _is_number(code))
        {
            int level = atoi(code);
            // we cannot set manually a level > 80
            // special level must be set by gossip menus
            if (level <= 0 || uint32(level) >= sWorld->getIntConfig(CONFIG_MAX_PLAYER_LEVEL))
            {
                return false;
            }

            setTimeWalking(player, level);
            player->PlayerTalkClass->SendCloseGossip();
        }

        return true;
    }

    bool OnGossipSelect(Player* player, Creature* creature, uint32  /*sender*/, uint32 action) override
    {
        player->PlayerTalkClass->ClearMenus();
        if (action == 4)
        {
            for (std::map<uint32, TwRaid>::iterator it = raidList.begin(); it != raidList.end(); it++)
            {
                if (it->second.GetBonus() == 1)
                {
                    AddGossipItemFor(player,GOSSIP_ICON_TABARD, "|cff00ff00|TInterface/ICONS/INV_Misc_Coin_01:30|t|rBonus: " + it->second.GetName()+"|r", GOSSIP_SENDER_MAIN, 10000+it->second.GetLevel());
                }
            }
            SendGossipMenuFor(player,TIMEWALKING_GOSSIP_NPC_TEXT_BONUS, creature->GetGUID());
        }

        if (action == 5)
        {
            std::vector<uint32> expList;

            for (std::map<uint32, TwRaid>::iterator it = raidList.begin(); it != raidList.end(); it++)
            {
                std::string exp;
                if (it->second.GetExp() == 1)
                {
                    exp = "Classic";
                }
                else if (it->second.GetExp() == 2)
                {
                    exp = "The Burning Crusade";
                }
                else
                {
                    exp = "Wrath of The Lich King";
                }

                if (std::find(expList.begin(), expList.end(), it->second.GetExp()) == expList.end()) {
                    expList.push_back(it->second.GetExp());
                    AddGossipItemFor(player,GOSSIP_ICON_TABARD, exp, GOSSIP_SENDER_MAIN, it->second.GetExp()); // go to phase menu
                }
            }
            SendGossipMenuFor(player,TIMEWALKING_GOSSIP_NPC_TEXT_EXP, creature->GetGUID());
        }

        if (action == 9) {
            for (uint32 i=TIMEWALKING_LVL_VAS_START; i<=TIMEWALKING_LVL_VAS_LVL7; i++) {
                AddGossipItemFor(player,GOSSIP_ICON_TABARD, "Flex Mythic+ "+std::to_string(i-TIMEWALKING_LVL_VAS_START+1), GOSSIP_SENDER_MAIN, i+10000);
            }

            AddGossipItemFor(player,GOSSIP_ICON_TABARD, "Flex Mythic+ 13 Epic", GOSSIP_SENDER_MAIN, 10413);
            AddGossipItemFor(player,GOSSIP_ICON_TABARD, "Flex Mythic+ 16 Heroic", GOSSIP_SENDER_MAIN, 10416);

            SendGossipMenuFor(player,TIMEWALKING_GOSSIP_NPC_TEXT_MAIN, creature->GetGUID());
        }

        if (action <= 3) //generate phase menu
        {
            std::vector<uint32> phaseList;

            for (std::map<uint32, TwRaid>::iterator it = raidList.begin(); it != raidList.end(); it++)
            {
                if (it->second.GetExp() == action)
                {
                    if (std::find(phaseList.begin(), phaseList.end(), it->second.GetPhase()) == phaseList.end()) {
                        phaseList.push_back(it->second.GetPhase());
                        std::stringstream s;
                        s << "Fase " << it->second.GetPhase();
                        AddGossipItemFor(player,GOSSIP_ICON_TABARD, s.str().c_str(), GOSSIP_SENDER_MAIN, 1000 + it->second.GetPhase());
                    }
                }
            }
            SendGossipMenuFor(player,TIMEWALKING_GOSSIP_NPC_TEXT_PHASE, creature->GetGUID());
        }
        else if(action >= 1000 && action < 10000) //generate raid menu
        {
            for (std::map<uint32, TwRaid>::iterator it = raidList.begin(); it != raidList.end(); it++)
            {
                if (it->second.GetPhase() == action - 1000)
                {
                    AddGossipItemFor(player,0, it->second.GetName(), GOSSIP_SENDER_MAIN, it->second.GetLevel()+10000);
                }
            }
            SendGossipMenuFor(player,TIMEWALKING_GOSSIP_NPC_TEXT_RAID, creature->GetGUID());
        }
        else if (action >= 10000) //apply level
        {
            uint32 level = action - 10000;

            if (sAZTH->GetAZTHPlayer(player)->GetTimeWalkingLevel() == 0)
            {
                setTimeWalking(player, level);
                player->PlayerTalkClass->SendCloseGossip();
            }
            else
            {
                SendGossipMenuFor(player,TIMEWALKING_GOSSIP_NPC_TEXT_ALREADYAPPLIED, creature->GetGUID());
            }
        }

        else if (action == 7)
        {
            setTimeWalking(player, 0 );
            player->PlayerTalkClass->SendCloseGossip();
        }

        return true;
    }
};

class timeWalkingPlayer : public PlayerScript
{
public:
    timeWalkingPlayer() : PlayerScript("timeWalkingPlayer") {}

    bool _autoscaling(Player *player, uint32 newZone = 0, uint32 newArea = 0)
    {
        sAZTH->GetAZTHPlayer(player)->autoScalingPending=(player->IsInCombat() || !player->IsAlive() || !player->IsInWorld()) ? time(NULL) + 2 /*2 seconds after */: 0;

        if (sAZTH->GetAZTHPlayer(player)->autoScalingPending > 0)
            return false;

        uint32 posLvl = 0;

        if (newArea && newZone)
            posLvl = sAzthUtils->getPositionLevel(false, player->GetMap(), newZone, newArea);
        else
        {
            WorldLocation pos = WorldLocation(player->GetMapId(), player->GetPositionX(), player->GetPositionY(), player->GetPositionZ(), player->GetOrientation());
            posLvl = sAzthUtils->getPositionLevel(false, player->GetMap(), pos);
        }

        if (!posLvl)
            return false;

        if (player->getLevel() == posLvl)
            return false;

        sAZTH->GetAZTHPlayer(player)->SetTimeWalkingLevel(TIMEWALKING_LVL_AUTO, true, true, false);

        return true;
    }

    void OnUpdateArea(Player* player, uint32 oldArea, uint32 newArea) override
    {
        if (!player)
            return;

        if (oldArea != newArea && sAZTH->GetAZTHPlayer(player)->GetTimeWalkingLevel() == TIMEWALKING_LVL_AUTO)
        {
            AreaTableEntry const* area = sAreaTableStore.LookupEntry(newArea);
            if (!area)
                return;

            _autoscaling(player, area->zone, newArea);
        }

        sAzthUtils->updateTwLevel(player, player->GetGroup()); // needed for player stats
    }

    void OnBeforeUpdate(Player *player, uint32 /*p_time*/) override
    {
        if (!player)
            return;

        if (sAZTH->GetAZTHPlayer(player)->GetTimeWalkingLevel() != TIMEWALKING_LVL_AUTO)
            return;

        if (sAZTH->GetAZTHPlayer(player)->autoScalingPending > time(NULL))
            return;

        _autoscaling(player);
    }

    void OnCriteriaProgress(Player *player, AchievementCriteriaEntry const* criteria) override
    {
        if (!player || !player->IsInWorld())
            return;

		if (sAzthAchievementMgr->achievementList.find(criteria->ID) == sAzthAchievementMgr->achievementList.end())
			return;

        AzthAchievement achi = sAzthAchievementMgr->achievementList[criteria->ID];

        uint32 count = achi.GetRewardCount();
        uint32 reward = achi.GetReward();
        uint32 killCredit = achi.GetKillCredit();

        if (reward == AZTH_MARK_OF_AZEROTH && player->inRandomLfgDungeon())
        {
            /* initialize random seed: */
            srand (time(NULL));
            /* generate secret number between 3 and 10: */
            uint8 moaBonus = 1 + rand() % static_cast<int>(2 - 1 + 1);

            if (player->GetGroup())
            {
                std::list<Group::MemberSlot> memberSlots = player->GetGroup()->GetMemberSlots();
                for (std::list<Group::MemberSlot>::iterator membersIterator = memberSlots.begin(); membersIterator != memberSlots.end(); membersIterator++)
                {
                    if ((*membersIterator).guid == player->GetGUID())
                    {
                        uint8 role = (*membersIterator).roles;

                        if (role & lfg::PLAYER_ROLE_TANK || role & lfg::PLAYER_ROLE_HEALER)
                            count += moaBonus;

                        break;
                    }
                }
            }
        }

        if (!count && !killCredit)
            return;

        /*
         *  CHECK REQUIREMENTS
         */
        if (achi.GetReqDimension() > 0 && achi.GetReqDimension() != sAZTH->GetAZTHPlayer(player)->getCurrentDimensionByAura())
            return;

        uint32 level = sAZTH->GetAZTHPlayer(player)->getPStatsLevel(false);

        // skip rewards if you don't have the required special level
        if (achi.GetSpecialLevelReq() > 0 && !sAzthUtils->isMythicCompLvl(achi.GetSpecialLevelReq(), level) && level != achi.GetSpecialLevelReq())
            return;

        // if levelMin and Level max has been set, then the level must be in this range
        if (achi.GetLevelMin() && achi.GetLevelMax() && (level < achi.GetLevelMin() || level  > achi.GetLevelMax()))
            return;
        /*
         *  END
         */

        // to achieve it you must be in range
        if (count > 0) {
            ItemTemplate const* _proto = sObjectMgr->GetItemTemplate(reward);
            if (!_proto)
                return;

            if (_proto->IsCurrencyToken())
            {
                if (player->GetMap()->IsDungeon() || player->GetMap()->IsRaid())
                {
                    // bonus for who complete that achievement with a recommended level
                    if (level <= achi.GetLevel())
                        count *= 2;
                }

                player->AddItem(reward, count);
            }
            else
            {
                uint32 mailItems=0;

                CharacterDatabaseTransaction trans = CharacterDatabase.BeginTransaction();
                MailDraft* draft = new MailDraft(_proto->Name1, "");

                for (uint32 i = 0; i < count; i++)
                {
                    Item *item = NewItemOrBag(_proto);

                    if (!item->Create(sObjectMgr->GetGenerator<HighGuid::Item>().Generate(), _proto->ItemId, player)) {
                        delete item;
                        return;
                    }

                    ItemPosCountVec off_dest;
                    if (mailItems == 0 && player->CanStoreItem(NULL_BAG, NULL_SLOT, off_dest, item, false) == EQUIP_ERR_OK)
                    {
                        player->StoreItem(off_dest, item, true);
                    } else {
                        draft->AddItem(item);
                        mailItems++;

                        if ((i==count-1) || (mailItems > 0 && mailItems % 10 == 0)) {
                            draft->SendMailTo(trans, player, MailSender(player, MAIL_STATIONERY_GM), MAIL_CHECK_MASK_COPIED);
                            CharacterDatabase.CommitTransaction(trans);

                            if (i<count-1) {
                                trans = CharacterDatabase.BeginTransaction();
                                draft = new MailDraft(_proto->Name1, "");
                            }
                        }
                    }
                }

                ChatHandler(player->GetSession()).SendSysMessage(sAzthLang->getf(AZTH_LANG_TW_NEW_ITEM_OBTAINED, player, _proto->ItemId, _proto->Name1.c_str(), count));
                if (mailItems > 0)
                    ChatHandler(player->GetSession()).SendSysMessage(sAzthLang->get(AZTH_LANG_BAG_FULL_SENT_TO_MAIL, player));
            }
        }

        if (killCredit)
            sAZTH->GetAZTHPlayer(player)->ForceKilledMonsterCredit(killCredit, 0); // send credit

    }

    bool OnBeforeQuestComplete(Player *player, uint32 quest_id) override
    {
        QuestStatusMap::iterator qsitr = player->getQuestStatusMap().find(quest_id);
        if (qsitr == player->getQuestStatusMap().end()) // should always be true in this moment
            return true;

        //if (player->GetGroup())
        //    groupId=player->GetGroup()->GetLowGUID();

        Map* map = player->FindMap();
        if (!map->IsDungeon())
            return true;

        InstanceSave* is = sInstanceSaveMgr->PlayerGetInstanceSave(player->GetGUID(), map->GetId(), player->GetDifficulty(map->IsRaid()));
        if (!is)
            return true;

        if (sAZTH->GetAZTHInstanceSave(is)->startTime == 0)
            return true;

        uint32 guid,quest,sLevel,nLevel, instanceStart=0, questEnd, groupId;
        uint8 gSize;
        uint64 id=0;

        guid = player->GetGUID().GetCounter();
        quest = quest_id;
        sLevel = sAZTH->GetAZTHPlayer(player)->getPStatsLevel(false);
        nLevel = sAZTH->GetAZTHPlayer(player)->getPStatsLevel(true);
        gSize = sAZTH->GetAZTHPlayer(player)->getGroupSize();

        uint8 difficulty = map->GetDifficulty();
        uint32 dungeon = map->GetId();

        /*const Quest *questTmpl = sObjectMgr->GetQuestTemplate(quest_id);
        if (questTmpl && sAzthUtils->isMythicLevel(sLevel) && sLevel > TIMEWALKING_LVL_VAS_LVL1) {
            for (int i=0;i<4;i++) {
                if (questTmpl->RewardItemId[i] == AZTH_MARK_OF_AZEROTH) {
                    uint32 bonus=sLevel-TIMEWALKING_LVL_VAS_LVL1;
                    ChatHandler(player->GetSession()).SendSysMessage(sAzthLang->getf(AZTH_LANG_MYTHIC_MOA_BONUS, player,sLevel-TIMEWALKING_LVL_VAS_LVL1+1, bonus));
                    player->AddItem(AZTH_MARK_OF_AZEROTH, bonus);
                }
            }

            for (int i=0;i<5;i++) {
                if (questTmpl->RewardFactionId[i] == AZTH_AS_REP && questTmpl->RewardFactionValueIdOverride[i] > 0) {
                    uint32 bonus = (questTmpl->RewardFactionValueIdOverride[i] * std::pow(1.1, sLevel-TIMEWALKING_LVL_VAS_LVL1-1)) / 100; // proportional to mythic
                    ChatHandler(player->GetSession()).SendSysMessage(sAzthLang->getf(AZTH_LANG_MYTHIC_ASREP_BONUS, player, sLevel-TIMEWALKING_LVL_VAS_LVL1+1, bonus));
                    player->GetReputationMgr().ModifyReputation(sFactionStore.LookupEntry(AZTH_AS_REP), bonus);
                }
            }
        }*/

        groupId = is->GetInstanceId();

        instanceStart = sAZTH->GetAZTHInstanceSave(is)->startTime;

        questEnd = static_cast<uint32>(time(NULL));

        // a small trick to create unique id here
        id = ObjectGuid::Create<HighGuid::Unit>(instanceStart, groupId).GetCounter();

        CharacterDatabase.PExecute("INSERT INTO azth_quest_log (guid, groupId, quest, dungeon, difficulty, sLevel, nLevel, gSize, instanceStartTime, questEndTime) VALUES(%u,%u,%u,%u,%u,%u,%u,%u,%u,%u);",
                                guid, id, quest, dungeon, difficulty, sLevel,nLevel,gSize,instanceStart,questEnd);

        return true;
    }

    void OnLoadFromDB(Player *player) override
    {
        QueryResult timewalkingCharactersActive_table = CharacterDatabase.PQuery("SELECT id,level FROM azth_timewalking_characters_active WHERE id = %d", player->GetGUID().GetCounter());
        if (timewalkingCharactersActive_table) //if is in timewalking mode apply debuff
        {
            Field* timewalkingCharactersActive_field = timewalkingCharactersActive_table->Fetch();

            sAZTH->GetAZTHPlayer(player)->SetTimeWalkingLevel(timewalkingCharactersActive_field[1].GetUInt32(), false, false , true);
        }
    }

    void OnLogin(Player* player) override
    {
        sAzthUtils->updateTwLevel(player, player->GetGroup()); // to fix level on instance that cannot be calculated OnLoadFromDB (too early)
        sAZTH->GetAZTHPlayer(player)->prepareTwSpells(player->getLevel());
        sAzthUtils->setTwDefense(player, sAZTH->GetAZTHPlayer(player)->isTimeWalking(true));

        int32 rep=player->GetReputationMgr().GetReputation(AZTH_AS_REP);
        // reduce only if positive reputation
        if (rep > 0)
        {
            QueryResult last_activity_rep = CharacterDatabase.PQuery("SELECT c.logout_time,MAX(questEndTime),MAX(instanceEndTime),MAX(instanceStartTime) FROM azth_quest_log AS q LEFT JOIN characters AS c ON q.guid=c.guid WHERE q.guid = %d;", player->GetGUID().GetCounter());
            if (last_activity_rep)
            {
                Field* last_activity_fields = last_activity_rep->Fetch();

                uint32 logoutTime=last_activity_fields[0].GetUInt32();
                uint32 lastQuestEnd=last_activity_fields[1].GetUInt32();
                uint32 lastInstanceEnd=last_activity_fields[2].GetUInt32();
                uint32 lastInstanceStart=last_activity_fields[3].GetUInt32();

                uint32 lastActivity = lastQuestEnd ? lastQuestEnd : (lastInstanceEnd ? lastInstanceEnd : lastInstanceStart);

                if (lastActivity) {
                    uint32 now = static_cast<uint32>(time(NULL));
                    uint32 diffActivity = now > lastActivity ? now - lastActivity : 0;
                    uint32 diffLogout = now > logoutTime ? now - logoutTime : 0;
                    uint32 daysActivity=diffActivity/60/60/24;
                    uint32 daysLogout=diffLogout/60/60/24;
                    if (daysActivity>5 && daysLogout > 1) {
                        // Gompertz formula for rep reduction growt
                        uint32 repRed=43000*std::exp(-4*std::exp(-float(daysLogout)/100))-810;
                        repRed = repRed > (uint32)rep ? rep : repRed; // never go below 0
                        ChatHandler(player->GetSession()).SendSysMessage(sAzthLang->getf(AZTH_LANG_TW_REP_REMOVED_LOGOUT, player, daysActivity ));
                        player->GetReputationMgr().ModifyReputation(sFactionStore.LookupEntry(AZTH_AS_REP), -repRed);
                    }
                }
            }
        }
    }

    void OnLevelChanged(Player* player, uint8 /*oldlevel*/) override
    {
        sAzthUtils->updateTwLevel(player, player->GetGroup());
    }

    void OnBeforeInitTalentForLevel(Player* player, uint32& talentPointsForLevel) override
    {
        if (!sAzthUtils->isMythicLevel(sAZTH->GetAZTHPlayer(player)->GetTimeWalkingLevel()) // redundant (?)
            && (sAZTH->GetAZTHPlayer(player)->isTimeWalking(true) || sAZTH->GetAZTHPlayer(player)->GetTimeWalkingLevel() == TIMEWALKING_LVL_AUTO) )
        {
            talentPointsForLevel = 71; // to avoid talent points reset after relog in timewalking
        }
    }
};

class achievement_timewalking_check : public AchievementCriteriaScript
{
public:
    achievement_timewalking_check(char const* name) : AchievementCriteriaScript(name) { }

    //Should be deprecated since canCompleteCriteria used in official AchievementMgr.cpp check is enough
    //however you can attach this script on achievement_criteria_data if needed
    bool OnCheck(Player*  player, Unit* /*target*/, uint32 criteria_id) override
    {
        AchievementCriteriaEntry const* criteria = sAchievementCriteriaStore.LookupEntry(criteria_id);
        return sAZTH->GetAZTHPlayer(player)->canCompleteCriteria(criteria);
    }
};

class global_timewalking : public GlobalScript
{
public:
    global_timewalking() : GlobalScript("global_timewalking_script") { }

    void OnAfterUpdateEncounterState(Map* map, EncounterCreditType /*type*/,  uint32 /*creditEntry*/, Unit* /*source*/, Difficulty difficulty_fixed, DungeonEncounterList const* encounters, uint32 dungeonCompleted, bool updated) override
    {
        if (!map->IsDungeon())
            return;

        if (!encounters)
            return;

        if (!updated)
            return;

        std::vector<Player*> list;

        for (auto const& itr : map->GetPlayers())
            if (!itr.GetSource()->IsGameMaster())
                list.push_back(itr.GetSource());

        if (list.size() == 0)
            return;

        uint32 guid,sLevel,nLevel, instanceStart=0, now, groupId;
        uint8 gSize;
        uint64 id=0;
        uint8 difficulty = map->GetDifficulty();
        uint32 dungeon = map->GetId();

        for (size_t i = 0; i < list.size(); i++)
        {
            Player* player = list[i];
            if (!player)
                return;

            InstanceSave* is = sInstanceSaveMgr->PlayerGetInstanceSave(player->GetGUID(), map->GetId(), player->GetDifficulty(map->IsRaid()));
            if (!is)
                return;

            if (sAZTH->GetAZTHInstanceSave(is)->startTime == 0)
                return;

            uint8 count = 0;
            uint8 total = 0;

            for (DungeonEncounterList::const_iterator itr = encounters->begin(); itr != encounters->end(); ++itr)
            {
                DungeonEncounter const* encounter = *itr;

                if (encounter->dbcEntry->mapId == dungeon && ((1 << encounter->dbcEntry->encounterIndex) & is->GetCompletedEncounterMask()) > 0)
                    count++;

                total++;
            }

            if (!count)
                return;

            guid = player->GetGUID().GetCounter();
            sLevel = sAZTH->GetAZTHPlayer(player)->getPStatsLevel(false);
            nLevel = sAZTH->GetAZTHPlayer(player)->getPStatsLevel(true);
            gSize = sAZTH->GetAZTHPlayer(player)->getGroupSize();

            groupId = is->GetInstanceId();
            instanceStart = sAZTH->GetAZTHInstanceSave(is)->startTime;
            now = static_cast<uint32>(time(nullptr));

            std::string lvlTxt = sAzthUtils->getLevelInfo(sLevel);

            uint32 posLevel=sAzthUtils->getPositionLevel(true, map, player->GetZoneId(), player->GetAreaId());
            if ((sAzthUtils->canMythicHere(player) && sAzthUtils->isMythicLevel(sLevel)) || // MYTHIC CASE
                (nLevel<=70 && sLevel == TIMEWALKING_LVL_AUTO && nLevel <= posLevel) || // TW Auto CASE
                (sLevel<=70 && sLevel <= posLevel) || // TIMEWALKING NORMAL LEVELS (AVOID ENDGAME CONTENTS)
                (sLevel > 80 && sLevel == posLevel)) // TIMEWALKING SPECIAL LEVELS
            {
                uint32 moaBonus=1 + uint8(difficulty_fixed);
                ChatHandler(player->GetSession()).SendSysMessage(sAzthLang->getf(AZTH_LANG_TW_MOA_BONUS, player, lvlTxt.c_str(), moaBonus));
                player->AddItem(AZTH_MARK_OF_AZEROTH, moaBonus);

                float mythBonus = sAzthUtils->isMythicLevel(sLevel) ? std::pow(1.1, sLevel-TIMEWALKING_LVL_VAS_LVL1+1) : 1;
                // Dungeon Normal: 1 boss every 10 minutes (this value scales up with raid and difficulty)
                // if time is lower it will gain more points
                float timeBonus = count / (float(now-instanceStart)/60/10);
                uint32 repBonus = 3 * mythBonus * timeBonus * ((map->IsRaid() ? (difficulty+1) * 3 : difficulty+1));

                if (repBonus)
                {
                    ChatHandler(player->GetSession()).SendSysMessage(sAzthLang->getf(AZTH_LANG_TW_ASREP_BONUS, player, lvlTxt.c_str(), repBonus));
                    player->GetReputationMgr().ModifyReputation(sFactionStore.LookupEntry(AZTH_AS_REP), repBonus);
                }

                ChatHandler(player->GetSession()).SendSysMessage(sAzthLang->getf(AZTH_LANG_TW_BOSS_KILLED, player, count, total, uint32(round(float(now-instanceStart)/60))));

                if (count>=total)
                {
                    player->GetSession()->SendNotification("%s", sAzthLang->get(AZTH_LANG_TW_INSTANCE_COMPLETED, player));
                    ChatHandler(player->GetSession()).SendSysMessage(sAzthLang->get(AZTH_LANG_TW_INSTANCE_COMPLETED, player));
                }

                if (dungeonCompleted)
                {
                    player->GetSession()->SendNotification("%s", sAzthLang->get(AZTH_LANG_TW_LAST_BOSS_KILLED, player));
                    ChatHandler(player->GetSession()).SendSysMessage(sAzthLang->get(AZTH_LANG_TW_LAST_BOSS_KILLED, player));
                }

            }
            else if (posLevel <= 70 && nLevel > posLevel + 10)
            {
                ChatHandler(player->GetSession()).SendSysMessage(sAzthLang->getf(AZTH_LANG_TW_REP_REMOVED_KILL, player, nLevel-posLevel));
                player->GetReputationMgr().ModifyReputation(sFactionStore.LookupEntry(AZTH_AS_REP), -(nLevel-posLevel));
            }

            if (count >= total)
            {
                // a small trick to create unique id here
                id = ObjectGuid::Create<HighGuid::Unit>(instanceStart, groupId).GetCounter();

                CharacterDatabase.PExecute("INSERT INTO azth_quest_log (guid, groupId, quest, dungeon, difficulty, sLevel, nLevel, gSize, instanceStartTime, instanceEndTime)"
                    "VALUES (%u,%u,%u,%u,%u,%u,%u,%u,%u,%u)", guid, id, 0, dungeon, difficulty, sLevel,nLevel,gSize,instanceStart,now);
            }
        }
    }

    void OnInitializeLockedDungeons(Player* player, uint8& /*level*/, uint32& lockData, lfg::LFGDungeonData const* dungeon) override
    {
        switch(lockData)
        {
        case lfg::LFG_LOCKSTATUS_MISSING_ACHIEVEMENT:
        case lfg::LFG_LOCKSTATUS_QUEST_NOT_COMPLETED:
        case lfg::LFG_LOCKSTATUS_MISSING_ITEM:
            Player* leader = nullptr;

            ObjectGuid leaderGuid = player->GetGroup() ? player->GetGroup()->GetLeaderGUID() : player->GetGUID();

            if (leaderGuid != player->GetGUID())
                leader = HashMapHolder<Player>::Find(leaderGuid);

            if (!leader || !leader->IsInWorld())
                leader = player;

            if (sWorld && leader->IsInWorld() && sAZTH->GetAZTHPlayer(leader) && sAZTH->GetAZTHPlayer(leader)->isTimeWalking() && dungeon && dungeon->minlevel <= 70)
                lockData = 0;
        break;
        }
    }
};

void AddSC_TimeWalking()
{
    new loadTimeWalkingRaid();
    new global_timewalking();
    new TimeWalkingGossip();
    new timeWalkingPlayer();
    new achievement_timewalking_check("achievement_timewalking_check");
    new TWVasScript();
}
