/*
 * Copyright (C) 2008-2015 TrinityCore <http://www.trinitycore.org/>
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation; either version 2 of the License, or (at your
 * option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#include "Chat.h"
#include "ScriptMgr.h"
#include "AccountMgr.h"
#include "ArenaTeamMgr.h"
#include "CellImpl.h"
#include "GridNotifiers.h"
#include "Group.h"
#include "Language.h"
#include "Opcodes.h"
#include "Player.h"
#include "Pet.h"
#include "ReputationMgr.h"
#include "CustomRates.h"
#include "AzthSharedDefines.h"
#include "RatingBonus.h"
#include "AzthUtils.h"
#include "AZTH.h"

 class azth_commandscript : public CommandScript {
 public:

     azth_commandscript() : CommandScript("azth_commandscript") {}

     std::vector<ChatCommand> GetCommands() const override {
         // name , level, allowConsole, function
         static std::vector<ChatCommand> lookupAzthCommands = {
            //{ "fixranks"       , SEC_PLAYER, false, &handleFixRanks,           ".azth fixranks : fix all previous not learned spell ranks, expecially used for portings" },
            { "arealevel"      , SEC_PLAYER, false, &handlePrintAreaLevel,     ".azth arealevel : to show calculated level for current area" },
            // { "maxskill"       , SEC_PLAYER, false, &handleAzthMaxSkill,       ".azth maxskill: if you're level 80, you can use this command to max out weapons skills"},
            // { "xp"             , SEC_PLAYER, false, &handleAzthXP,             ".azth xp <rate>: you can set an arbitrary experience rate"},
            //{ "pstatinfo"      , SEC_PLAYER, false, &handleAzthCurrLevel,      ".azth pstatinfo: show info about level of selected player and size&level of its group if exists)"},
            // { "smartstone"     , SEC_PLAYER, false, &handleAzthSmartstone,     ".azth smartstone: create a smartstone in your bag" },
            // { "bonus"          , SEC_ADMINISTRATOR, false, &handleAzthBonus,   ".azth bonus <rate> <bracket> :  set temp a rating bonus for brackets" },
            // { "ptype"          , SEC_PLAYER, false, &handlePlayerType,         ".azth ptype : shows player tipology (normal,fullpvp)" },
         };

         static std::vector<ChatCommand> commandTable = {
             { "azth"    , SEC_PLAYER, false, nullptr, "", lookupAzthCommands},
            //  { "qc"      , SEC_PLAYER, false, &HandleQuestCompleterCommand, ""},
         };
         return commandTable;
     }

    static bool handleAzthCurrLevel(ChatHandler* handler, char const* /*args*/) {
        Player* target = handler->getSelectedPlayerOrSelf();

        if (!target)
            target = handler->GetSession() ? handler->GetSession()->GetPlayer() : NULL;

        if (!target || !target->GetSession())
            return false;


        uint32 finalLevel = sAZTH->GetAZTHPlayer(target)->getPStatsLevel(false);
        uint32 nFinalLevel = sAZTH->GetAZTHPlayer(target)->getPStatsLevel(true);
        uint32 level = sAZTH->GetAZTHPlayer(target)->getPStatsLevel(false, false, false);
        uint32 nLevel = sAZTH->GetAZTHPlayer(target)->getPStatsLevel(true, false, false);
        uint32 groupLevel = sAZTH->GetAZTHPlayer(target)->getGroupLevel(false, false);
        uint32 nGroupLevel = sAZTH->GetAZTHPlayer(target)->getGroupLevel(true, false);
        uint32 instanceLevel = sAZTH->GetAZTHPlayer(target)->getInstanceLevel(false);
        uint32 nInstanceLevel = sAZTH->GetAZTHPlayer(target)->getInstanceLevel(true);
        uint32 groupSize = sAZTH->GetAZTHPlayer(target)->getGroupSize(false);
        uint32 instanceSize = sAZTH->GetAZTHPlayer(target)->getInstanceSize();

        std::string finalLevelInfo = sAzthUtils->getLevelInfo(finalLevel);
        std::string nFinalLevelInfo = sAzthUtils->getLevelInfo(nFinalLevel);
        std::string levelInfo = sAzthUtils->getLevelInfo(level);
        std::string nLevelInfo = sAzthUtils->getLevelInfo(nLevel);
        std::string groupLevelInfo = sAzthUtils->getLevelInfo(groupLevel);
        std::string nGroupLevelInfo = sAzthUtils->getLevelInfo(nGroupLevel);
        std::string instanceLevelInfo = sAzthUtils->getLevelInfo(instanceLevel);
        std::string nInstanceLevelInfo = sAzthUtils->getLevelInfo(nInstanceLevel);

        handler->PSendSysMessage("PStat Level: %s", finalLevelInfo.c_str());
        handler->PSendSysMessage("PStat Level (Normalized): %s", nFinalLevelInfo.c_str());
        handler->PSendSysMessage("Player Level: %s", levelInfo.c_str());
        handler->PSendSysMessage("Player Level (Normalized): %s", nLevelInfo.c_str());
        handler->PSendSysMessage("Group Level: %s", groupLevelInfo.c_str());
        handler->PSendSysMessage("Group Level (Normalized): %s", nGroupLevelInfo.c_str());
        handler->PSendSysMessage("Instance Level: %s", instanceLevelInfo.c_str());
        handler->PSendSysMessage("Instance Level (Normalized): %s", nInstanceLevelInfo.c_str());
        handler->PSendSysMessage("Group Size: %u", groupSize);
        handler->PSendSysMessage("Instance Members Size: %u", instanceSize);

        return true;
    }

    static bool handleFixRanks(ChatHandler* handler, char const* /*args*/) {
        Player *player = handler->GetSession() ? handler->GetSession()->GetPlayer() : NULL;

        if (!player || !player->GetSession())
            return false;

        bool fixed=false;
        PlayerSpellMap spellMap = player->GetSpellMap();
        for (PlayerSpellMap::const_iterator iter = spellMap.begin(); iter != spellMap.end(); ++iter) {
            if (!iter->second->IsInSpec(player->GetActiveSpec()))
                continue;

            uint32 prev = sSpellMgr->GetPrevSpellInChain(iter->first);
            while (prev) {
                if (!player->HasSpell(prev)) {
                    player->learnSpell(prev);
                    fixed=true;
                }
                prev = sSpellMgr->GetPrevSpellInChain(prev);

            }
        }

        handler->PSendSysMessage(fixed ? "Old ranks fixed!" : "No old ranks to learn");

        return true;
    }

    static bool handlePlayerType(ChatHandler* handler, char const* /*args*/) {
        Player* target = handler->getSelectedPlayerOrSelf();

        if (!target)
            return false;

        if (sAZTH->GetAZTHPlayer(target)->isPvP()) {
            handler->PSendSysMessage("This is a full pvp character");
            return true;
        }

        handler->PSendSysMessage("This is a normal character");

        return true;
    }

    static bool handlePrintAreaLevel(ChatHandler* handler, char const* /*args*/) {
        Player* target = handler->getSelectedPlayerOrSelf();

        if (!target)
            return false;

        WorldLocation pos = WorldLocation(target->GetMapId(), target->GetPositionX(), target->GetPositionY(), target->GetPositionZ(), target->GetOrientation());
        uint32 posLvl=sAzthUtils->getPositionLevel(true, target->GetMap(), pos);

        if (posLvl)
            handler->PSendSysMessage("Current area level: %d", posLvl);
        else
            handler->PSendSysMessage("No level for this area");

        return true;
    }

     // Based on HandleQuestComplete method of cs_quest.cpp

    //  static bool HandleQuestCompleterCompHelper(Player* player, uint32 entry, ChatHandler* handler, char const* /*args*/, uint8 checked) {
    //      if (!player) {
    //          handler->SendSysMessage(LANG_NO_CHAR_SELECTED);
    //          handler->SetSentErrorMessage(true);
    //          return false;
    //      }

    //      Quest const* quest = sObjectMgr->GetQuestTemplate(entry);

    //      // If player doesn't have the quest
    //      if (!quest || player->GetQuestStatus(entry) == QUEST_STATUS_NONE) {
    //          handler->PSendSysMessage(LANG_COMMAND_QUEST_NOTFOUND, entry);
    //          handler->SetSentErrorMessage(true);
    //          return false;
    //      }

    //      // Add quest items for quests that require items
    //      for (uint8 x = 0; x < QUEST_ITEM_OBJECTIVES_COUNT; ++x) {
    //          uint32 id = quest->RequiredItemId[x];
    //          uint32 count = quest->RequiredItemCount[x];
    //          if (!id || !count)
    //              continue;

    //          uint32 curItemCount = player->GetItemCount(id, true);

    //          ItemPosCountVec dest;
    //          uint8 msg = player->CanStoreNewItem(NULL_BAG, NULL_SLOT, dest, id, count - curItemCount);
    //          if (msg == EQUIP_ERR_OK) {
    //              Item* item = player->StoreNewItem(dest, id, true);
    //              player->SendNewItem(item, count - curItemCount, true, false);
    //          }
    //      }

    //      // All creature/GO slain/cast (not required, but otherwise it will display "Creature slain 0/10")
    //      for (uint8 i = 0; i < QUEST_OBJECTIVES_COUNT; ++i) {
    //          int32 creature = quest->RequiredNpcOrGo[i];
    //          uint32 creatureCount = quest->RequiredNpcOrGoCount[i];

    //          if (creature > 0) {
    //              if (CreatureTemplate const* creatureInfo = sObjectMgr->GetCreatureTemplate(creature))
    //                  for (uint16 z = 0; z < creatureCount; ++z)
    //                      player->KilledMonster(creatureInfo, ObjectGuid::Empty);
    //          } else if (creature < 0)
    //              for (uint16 z = 0; z < creatureCount; ++z)
    //                  player->KillCreditGO(creature);
    //      }

    //      // If the quest requires reputation to complete
    //      if (uint32 repFaction = quest->GetRepObjectiveFaction()) {
    //          uint32 repValue = quest->GetRepObjectiveValue();
    //          uint32 curRep = player->GetReputationMgr().GetReputation(repFaction);
    //          if (curRep < repValue)
    //              if (FactionEntry const* factionEntry = sFactionStore.LookupEntry(repFaction))
    //                  player->GetReputationMgr().SetReputation(factionEntry, repValue);
    //      }

    //      // If the quest requires a SECOND reputation to complete
    //      if (uint32 repFaction = quest->GetRepObjectiveFaction2()) {
    //          uint32 repValue2 = quest->GetRepObjectiveValue2();
    //          uint32 curRep = player->GetReputationMgr().GetReputation(repFaction);
    //          if (curRep < repValue2)
    //              if (FactionEntry const* factionEntry = sFactionStore.LookupEntry(repFaction))
    //                  player->GetReputationMgr().SetReputation(factionEntry, repValue2);
    //      }

    //      // If the quest requires money
    //      int32 ReqOrRewMoney = quest->GetRewOrReqMoney();
    //      if (ReqOrRewMoney < 0)
    //          player->ModifyMoney(-ReqOrRewMoney);

    //      if (sWorld->getBoolConfig(CONFIG_QUEST_ENABLE_QUEST_TRACKER)) // check if Quest Tracker is enabled
    //      {
    //          // prepare Quest Tracker datas
    //          PreparedStatement* stmt = CharacterDatabase.GetPreparedStatement(CHAR_UPD_QUEST_TRACK_GM_COMPLETE);
    //          stmt->setUInt32(0, quest->GetQuestId());
    //          stmt->setUInt32(1, player->GetGUID().GetCounter());

    //          // add to Quest Tracker
    //          CharacterDatabase.Execute(stmt);
    //      }

    //      player->CompleteQuest(entry);

    //      // if bugged field is set on 2
    //      // then reward the quest too
    //      if (checked == 2) {
    //          Quest const* quest = sObjectMgr->GetQuestTemplate(entry);

    //          // If player doesn't have the quest
    //          if (!quest || player->GetQuestStatus(entry) != QUEST_STATUS_COMPLETE) {
    //              handler->PSendSysMessage(LANG_COMMAND_QUEST_NOTFOUND, entry);
    //              handler->SetSentErrorMessage(true);
    //              return false;
    //          }

    //          player->RewardQuest(quest, 0, player);
    //      }

    //      return true;
    //  }

     // Based on TrueWoW code

    //  static bool HandleQuestCompleterCommand(ChatHandler* handler, char const* args) {
    //     Player *player = handler->GetSession() ? handler->GetSession()->GetPlayer() : NULL;

    //      if (!player || !player->GetSession())
    //          return false;

    //      char* cId = handler->extractKeyFromLink((char*) args, "Hquest");
    //      if (!cId) {
    //          handler->SendSysMessage(sAzthLang->get(AZTH_LANG_CCOMMANDS_QUEST, player)); //va usato getf forse?
    //          handler->SetSentErrorMessage(true);
    //          return false;
    //      }

    //      uint32 entry = atol(cId);
    //      Quest const* quest = sObjectMgr->GetQuestTemplate(entry);
    //      if (!quest || quest == 0) {
    //          handler->PSendSysMessage("Please enter a quest link.");
    //          handler->SetSentErrorMessage(true);
    //          return false;
    //      } else {
    //          uint32 checked = 0;
    //          PreparedStatement* stmt = WorldDatabase.GetPreparedStatement(WORLD_SEL_QUESTCOMPLETER);
    //          stmt->setUInt32(0, entry);
    //          PreparedQueryResult resultCheck = WorldDatabase.Query(stmt);

    //          if (!resultCheck) {
    //              handler->PSendSysMessage("Error: quest not found.");
    //              handler->SetSentErrorMessage(true);
    //              return false;
    //          }

    //          std::string questTitle = quest->GetTitle();

    //          checked = (*resultCheck)[0].GetUInt8();

    //          if (checked >= 1) {
    //              std::string name;
    //              const char* playerName = handler->GetSession() ? handler->GetSession()->GetPlayer()->GetName().c_str() : NULL;

    //              if (playerName) {
    //                  name = playerName;
    //                  normalizePlayerName(name);

    //                  Player* player = ObjectAccessor::FindPlayerByName(name);
    //                  if (player->GetQuestStatus(entry) != QUEST_STATUS_INCOMPLETE) {
    //                      handler->SendSysMessage(sAzthLang->getf(AZTH_LANG_CCOMANDS_QUEST_HASBUG, player, questTitle.c_str()));
    //                      return true;
    //                  } else {
    //                      HandleQuestCompleterCompHelper(player, entry, handler, args, checked);
    //                      handler->SendSysMessage(sAzthLang->getf(AZTH_LANG_CCOMANDS_QUEST_HASBUGANDCOMPLETED, player, questTitle.c_str()));
    //                      return true;
    //                  }
    //              } else {
    //                  handler->SendSysMessage(sAzthLang->getf(AZTH_LANG_CCOMANDS_QUEST_HASBUG, player, questTitle.c_str()));
    //                  return true;
    //              }
    //          } else {
    //              handler->SendSysMessage(sAzthLang->getf(AZTH_LANG_CCOMANDS_QUEST_NOBUG, player, questTitle.c_str()));
    //              return true;
    //          }
    //      }
    //  }

     static bool handleAzthMaxSkill(ChatHandler* handler, const char* /*args*/) {

         Player* target = handler->getSelectedPlayerOrSelf();
         if (!target) {
             handler->SendSysMessage(LANG_NO_CHAR_SELECTED);
             handler->SetSentErrorMessage(true);
             return false;
         }
         if (target->getLevel() < 80) {
             handler->PSendSysMessage(LANG_LEVEL_MINREQUIRED, 80);
             handler->SetSentErrorMessage(true);
             return false;
         }

         static const SkillSpells spells[] = {ONE_HAND_AXES, TWO_HAND_AXES, ONE_HAND_MACES,
             TWO_HAND_MACES, POLEARMS, ONE_HAND_SWORDS, TWO_HAND_SWORDS, STAVES, BOWS,
             GUNS, DAGGERS, WANDS, CROSSBOWS, FIST_WEAPONS};

         std::list<SkillSpells> learnList;
         for (std::size_t s=0; s<sizeof(spells); s++) {
             SkillSpells spell = spells[s];
             switch (target->getClass()) {
                 case CLASS_WARRIOR:
                     if (spell != WANDS)
                         learnList.push_back(spell);
                     break;
                 case CLASS_DEATH_KNIGHT:
                 case CLASS_PALADIN:
                     if (spell != STAVES && spell != BOWS && spell != GUNS && spell != DAGGERS &&
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
                     if (spell != ONE_HAND_SWORDS && spell != TWO_HAND_SWORDS && spell != POLEARMS &&
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
                     if (spell != ONE_HAND_SWORDS && spell != TWO_HAND_SWORDS &&
                             spell != BOWS && spell != GUNS && spell != WANDS && spell != CROSSBOWS &&
                             spell != ONE_HAND_AXES && spell != TWO_HAND_AXES)
                         learnList.push_back(spell);
                     break;
                 default:
                     break;
             }
         }

         for (std::list<SkillSpells>::const_iterator spell = learnList.begin(), end = learnList.end(); spell != end; ++spell) {
             if (!target->HasSpell(*spell))
                 target->learnSpell(*spell);
         }

         target->UpdateSkillsToMaxSkillsForLevel();
         return true;
     }

     static bool handleAzthBonus(ChatHandler* handler, const char* args) {
         Player *me = handler->GetSession() ? handler->GetSession()->GetPlayer() : NULL;

         if (!me || !me->GetSession())
             return false;

		 // If no arguments provided, show the current bonuses
		 if (!*args) {
			 sBonusRating->printBonusesToPlayer(handler, 0);
			 return true;
		 }

         // if there are parameters, check if I can use the command
         if (me->GetSession()->GetSecurity() < sBonusRating->AZTH_RATING_BONUS_SECURITY) {
             handler->SendSysMessage(LANG_YOURS_SECURITY_IS_LOW);
             handler->SetSentErrorMessage(true);
             return false;
         }

		 char* arg1 = strtok((char*)args, " ");
		 char* arg2 = strtok(nullptr, " ");

		 if (!arg1 || !arg2)
		 {
			 handler->PSendSysMessage("|CFF7BBEF7[Rating Bonus]|r: Invalid parameters.");
			 handler->SetSentErrorMessage(true);
			 return false;
		 }

		 float rate = atof((char *)arg1);
		 float maxRate = 10; // a soft cap to prevent large abuses
         if (rate < 1 || rate > maxRate) {
             handler->PSendSysMessage("|CFF7BBEF7[Rating Bonus]|r: Invalid bonus multiplier specified, must be in interval [1, %.2f].", maxRate);
             handler->SetSentErrorMessage(true);
             return false;
         }

		 int bracket = atoi((char *)arg2);
		 if (bracket < 1 || bracket > 5) {
			 handler->PSendSysMessage("|CFF7BBEF7[Rating Bonus]|r: Invalid bracket specified.");
			 handler->SetSentErrorMessage(true);
			 return false;
		 }

		 if (rate != 1.f)
		 {
			 sBonusRating->addBonus(bracket, rate);
			 handler->PSendSysMessage("|CFF7BBEF7[Rating Bonus]|r: Bonus added! Multiplier: %.2f Bracket: %s", rate, sBonusRating->brackets[bracket - 1]);
		 }
		 else
		 {
			 sBonusRating->removeBonus(bracket);
			 handler->PSendSysMessage("|CFF7BBEF7[Rating Bonus]|r: Bonus removed from bracket %d!", bracket);
		 }


         return true;
     }

     static bool handleAzthSmartstone(ChatHandler* handler, const char* /*args*/)
     {
         Player* player = handler->GetSession()->GetPlayer();

         if (!player)
             return false;

         if (player->HasItemCount(32547, 1U, true))
         {
             handler->SendSysMessage(sAzthLang->get(AZTH_LANG_SS_POSSES_CHECK, player));
             return true;
         }
         else
         {
             player->AddItem(32547, 1);
         }

         return true;
     }

	 static bool handleAzthXP(ChatHandler* handler, const char* args)
     {
         Player *me = handler->GetSession() ? handler->GetSession()->GetPlayer() : nullptr;
		 Player *target = handler->getSelectedPlayer();
		 Player *player = nullptr;

		 if (!me || !me->GetSession())
			 return false;

         // If no arguments provided, show current custom XP rate
         if (!*args)
         {
             handler->PSendSysMessage("|CFF7BBEF7[Custom Rates]|r: Your current XP rate is %.2f.", sAZTH->GetAZTHPlayer(me)->GetPlayerQuestRate());
             return true;
         }

		 // first, check if I can use the command
		 if (me->GetSession()->GetSecurity() < sAZTH->GetCustomXPSecurity())
         {
			 handler->SendSysMessage(LANG_YOURS_SECURITY_IS_LOW);
			 handler->SetSentErrorMessage(true);
			 return false;
		 }

		 float rate = atof((char *)args);
		 float maxRate = sAZTH->GetCustomXPMax();

         if (rate < 0 || rate > maxRate)
         {
			 handler->PSendSysMessage("|CFF7BBEF7[Custom Rates]|r: Invalid rate specified, must be in interval [0, %.2f].", maxRate);
			 handler->SetSentErrorMessage(true);
			 return false;
		 }

		 // Without target; Set my XP rate
		 if (!target || !target->GetSession())
			 player = me;
		 else
         {
			 // Have a target AND my security level is higher than target's (I am a GM, he is a player); Set target XP rate
			 if (me->GetSession()->GetSecurity() > target->GetSession()->GetSecurity())
				 player = target;
			 else
				 player = me;
		 }

		 CustomRates::SaveXpRateToDB(player, rate);

		 sAZTH->GetAZTHPlayer(player)->SetPlayerQuestRate(rate);

		 if (sAZTH->GetAZTHPlayer(me)->GetPlayerQuestRate() == 0.0f)
			 handler->PSendSysMessage("|CFF7BBEF7[Custom Rates]|r: Quest & Dungeons XP Rate set to 0. You won't gain any experience from now on.");
		 else
			 handler->PSendSysMessage("|CFF7BBEF7[Custom Rates]|r: Quest & Dungeons XP Rate set to %.2f.", sAZTH->GetAZTHPlayer(me)->GetPlayerQuestRate());
		 return true;
	 }
 };

 void AddSC_azth_commandscript()
 {
     new azth_commandscript();
 }
