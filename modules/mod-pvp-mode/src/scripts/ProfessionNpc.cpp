#include "AzthLanguageStrings.h"
#include "AzthPlayer.h"
#include "AZTH.h"
#include "ScriptedGossip.h"

#define MIN_RATING 1200
#define AP_REQ 1000

class Professions_NPC : public CreatureScript
{
        public:
                Professions_NPC () : CreatureScript("Professions_NPC") {}

                void CreatureWhisperBasedOnBool(const char *text, Creature *_creature, Player *pPlayer, bool value)
                {
                    if (value)
                        _creature->MonsterWhisper(text, pPlayer);
                }

                uint32 PlayerMaxLevel() const
                {
                    return sWorld->getIntConfig(CONFIG_MAX_PLAYER_LEVEL);
                }

                bool PlayerHasItemOrSpell(const Player *plr, uint32 itemId, uint32 spellId) const
                {
                    return plr->HasItemCount(itemId, 1, true) || plr->HasSpell(spellId);
                }

                bool OnGossipHello(Player *pPlayer, Creature* _creature)
                {
                        if (!pPlayer->IsGameMaster()
                            && !sAZTH->GetAZTHPlayer(pPlayer)->isPvP() && pPlayer->GetMaxPersonalArenaRatingRequirement(2) < MIN_RATING) {
                            std::string msg  = "You need to be a Full PvP player and least "+std::to_string(MIN_RATING)+" 2v2 Rating to talk with me!";
                            _creature->MonsterWhisper(msg.c_str(), pPlayer);
                            return true;
                        }

                        AddGossipItemFor(pPlayer, GOSSIP_ICON_CHAT, "Welcome to AzerothShard profession NPC", GOSSIP_SENDER_MAIN, 0);
                        AddGossipItemFor(pPlayer, GOSSIP_ICON_TRAINER, "[Professions] ->", GOSSIP_SENDER_MAIN, 196);
                        pPlayer->PlayerTalkClass->SendGossipMenu(907, _creature->GetGUID());
                        return true;
                }

                bool PlayerAlreadyHasTwoProfessions(const Player *pPlayer) const
                {
                        uint32 skillCount = 0;

                        if (pPlayer->HasSkill(SKILL_MINING))
                            skillCount++;
                        if (pPlayer->HasSkill(SKILL_SKINNING))
                            skillCount++;
                        if (pPlayer->HasSkill(SKILL_HERBALISM))
                            skillCount++;

                        if (skillCount >= 2)
                                return true;

                        for (uint32 i = 1; i < sSkillLineStore.GetNumRows(); ++i)
                        {
                                SkillLineEntry const *SkillInfo = sSkillLineStore.LookupEntry(i);
                                if (!SkillInfo)
                                        continue;

                                if (SkillInfo->categoryId == SKILL_CATEGORY_SECONDARY)
                                        continue;

                                if ((SkillInfo->categoryId != SKILL_CATEGORY_PROFESSION) || !SkillInfo->canLink)
                                        continue;

                                const uint32 skillID = SkillInfo->id;
                                if (pPlayer->HasSkill(skillID))
                                        skillCount++;

                                if (skillCount >= 2)
                                        return true;
                        }
                        return false;
                }

                bool LearnAllRecipesInProfession(Player *pPlayer, SkillType skill)
                {
                        ChatHandler handler(pPlayer->GetSession());
                        char* skill_name;

                        SkillLineEntry const *SkillInfo = sSkillLineStore.LookupEntry(skill);
                        skill_name = SkillInfo->name[handler.GetSessionDbcLocale()];

                        if (!SkillInfo)
                        {
                                sLog->outError("Profession NPC: received non-valid skill ID (LearnAllRecipesInProfession)");
                                return false;
                        }

                        LearnSkillRecipesHelper(pPlayer, SkillInfo->id);

                        pPlayer->SetSkill(SkillInfo->id, pPlayer->GetSkillStep(SkillInfo->id), 450, 450);
                        handler.PSendSysMessage(LANG_COMMAND_LEARN_ALL_RECIPES, skill_name);

                        pPlayer->ModifyArenaPoints(-int32(AP_REQ));

                        return true;
                }

                void LearnSkillRecipesHelper(Player *player, uint32 skillLine)
                {
                        uint32 classmask = player->getClassMask();

                        for (uint32 j = 0; j < sSkillLineAbilityStore.GetNumRows(); ++j)
                        {
                                SkillLineAbilityEntry const *skillLineEntry = sSkillLineAbilityStore.LookupEntry(j);
                                if (!skillLineEntry)
                                        continue;

                                // wrong skill
                                if (skillLineEntry->SkillLine != skillLine)
                                        continue;

                                // not high rank
                                if (skillLineEntry->SupercededBySpell)
                                        continue;

                                // skip racial skills
                                if (skillLineEntry->RaceMask != 0)
                                        continue;

                                // skip wrong class skills
                                if (skillLineEntry->ClassMask && (skillLineEntry->ClassMask & classmask) == 0)
                                        continue;

                                SpellInfo const * spellInfo = sSpellMgr->GetSpellInfo(skillLineEntry->Spell);
                                if (!spellInfo || !SpellMgr::IsSpellValid(spellInfo))
                                        continue;

                                player->learnSpell(skillLineEntry->Spell);
                        }
                }

                bool IsSecondarySkill(SkillType skill) const
                {
                        return skill == SKILL_COOKING || skill == SKILL_FIRST_AID;
                }

                void CompleteLearnProfession(Player *pPlayer, Creature *pCreature, SkillType skill)
                {
                        if (PlayerAlreadyHasTwoProfessions(pPlayer) && !IsSecondarySkill(skill)) {
                            pCreature->MonsterWhisper("You already know two professions!", pPlayer);
                            return;
                        }

                        if (pPlayer->GetArenaPoints() < AP_REQ) {
                            pCreature->MonsterWhisper("You don't have enough Arena Points to buy this profession!", pPlayer);
                            return;
                        }

                        if (!LearnAllRecipesInProfession(pPlayer, skill))
                            pCreature->MonsterWhisper("Internal error occured!", pPlayer);
                }

                bool OnGossipSelect(Player* pPlayer, Creature* _creature, uint32 uiSender, uint32 uiAction)
                {
                        if (!uiAction)
                            return true;

                        if (!pPlayer->IsGameMaster()
                            && !sAZTH->GetAZTHPlayer(pPlayer)->isPvP() && pPlayer->GetMaxPersonalArenaRatingRequirement(2) < MIN_RATING) {
                            return true;
                        }

                        pPlayer->PlayerTalkClass->ClearMenus();

                        if (uiSender == GOSSIP_SENDER_MAIN)
                        {

                                switch (uiAction)
                                {
                                        case 196:
                                                AddGossipItemFor(pPlayer, GOSSIP_ICON_CHAT, "Alchemy (1000 AP)", GOSSIP_SENDER_MAIN, 1);
                                                AddGossipItemFor(pPlayer, GOSSIP_ICON_CHAT, "Blacksmithing (1000 AP)", GOSSIP_SENDER_MAIN, 2);
                                                AddGossipItemFor(pPlayer, GOSSIP_ICON_CHAT, "Leatherworking (1000 AP)", GOSSIP_SENDER_MAIN, 3);
                                                AddGossipItemFor(pPlayer, GOSSIP_ICON_CHAT, "Tailoring (1000 AP)", GOSSIP_SENDER_MAIN, 4);
                                                AddGossipItemFor(pPlayer, GOSSIP_ICON_CHAT, "Engineering (1000 AP)", GOSSIP_SENDER_MAIN, 5);
                                                AddGossipItemFor(pPlayer, GOSSIP_ICON_CHAT, "Enchanting (1000 AP)", GOSSIP_SENDER_MAIN, 6);
                                                AddGossipItemFor(pPlayer, GOSSIP_ICON_CHAT, "Jewelcrafting (1000 AP)", GOSSIP_SENDER_MAIN, 7);
                                                AddGossipItemFor(pPlayer, GOSSIP_ICON_CHAT, "Inscription (1000 AP)", GOSSIP_SENDER_MAIN, 8);
                                                AddGossipItemFor(pPlayer, GOSSIP_ICON_CHAT, "Herbalism (1000 AP)", GOSSIP_SENDER_MAIN, 11);
                                                AddGossipItemFor(pPlayer, GOSSIP_ICON_CHAT, "Skinning (1000 AP)", GOSSIP_SENDER_MAIN, 12);
                                                AddGossipItemFor(pPlayer, GOSSIP_ICON_CHAT, "Mining (1000 AP)", GOSSIP_SENDER_MAIN, 13);
                                                AddGossipItemFor(pPlayer, GOSSIP_ICON_CHAT, "Cooking (1000 AP)", GOSSIP_SENDER_MAIN, 9);
                                                AddGossipItemFor(pPlayer, GOSSIP_ICON_CHAT, "First Aid (1000 AP)", GOSSIP_SENDER_MAIN, 10);

                                                pPlayer->PlayerTalkClass->SendGossipMenu(907, _creature->GetGUID());
                                                break;
                                        case 1:
                                                if(pPlayer->HasSkill(SKILL_ALCHEMY))
                                                {
                                                        pPlayer->PlayerTalkClass->SendCloseGossip();
                                                        break;
                                                }


                                                {
                                                CompleteLearnProfession(pPlayer, _creature, SKILL_ALCHEMY);
                                                pPlayer->PlayerTalkClass->SendCloseGossip();
                                                }break;

                                        case 2:
                                                if(pPlayer->HasSkill(SKILL_BLACKSMITHING))
                                                {
                                                        pPlayer->PlayerTalkClass->SendCloseGossip();
                                                        break;
                                                }
                                                else
                                                {
                                                CompleteLearnProfession(pPlayer, _creature, SKILL_BLACKSMITHING);
                                                pPlayer->PlayerTalkClass->SendCloseGossip();
                                                }break;
                                        case 3:
                                                if(pPlayer->HasSkill(SKILL_LEATHERWORKING))
                                                {
                                                        pPlayer->PlayerTalkClass->SendCloseGossip();
                                                        break;
                                                }
                                                else
                                                {
                                                CompleteLearnProfession(pPlayer, _creature, SKILL_LEATHERWORKING);
                                                pPlayer->PlayerTalkClass->SendCloseGossip();
                                                }break;
                                         case 4:
                                                if(pPlayer->HasSkill(SKILL_TAILORING))
                                                {
                                                        pPlayer->PlayerTalkClass->SendCloseGossip();
                                                        break;
                                                }
                                                else
                                                {
                                                CompleteLearnProfession(pPlayer, _creature, SKILL_TAILORING);
                                                pPlayer->PlayerTalkClass->SendCloseGossip();
                                                }break;
                                        case 5:
                                                if(pPlayer->HasSkill(SKILL_ENGINEERING))
                                                {
                                                        pPlayer->PlayerTalkClass->SendCloseGossip();
                                                        break;
                                                }
                                                else
                                                {
                                                CompleteLearnProfession(pPlayer, _creature, SKILL_ENGINEERING);
                                                pPlayer->PlayerTalkClass->SendCloseGossip();
                                                }break;

                                        case 6:
                                                if(pPlayer->HasSkill(SKILL_ENCHANTING))
                                                {
                                                        pPlayer->PlayerTalkClass->SendCloseGossip();
                                                        break;
                                                }
                                                else
                                                {
                                                CompleteLearnProfession(pPlayer, _creature, SKILL_ENCHANTING);
                                                pPlayer->PlayerTalkClass->SendCloseGossip();
                                                }break;
                                        case 7:
                                                if(pPlayer->HasSkill(SKILL_JEWELCRAFTING))
                                                {
                                                        pPlayer->PlayerTalkClass->SendCloseGossip();
                                                        break;
                                                }
                                                else
                                                {
                                                CompleteLearnProfession(pPlayer, _creature, SKILL_JEWELCRAFTING);
                                                pPlayer->PlayerTalkClass->SendCloseGossip();
                                                }break;
                                        case 8:
                                                if(pPlayer->HasSkill(SKILL_INSCRIPTION))
                                                {
                                                        pPlayer->PlayerTalkClass->SendCloseGossip();
                                                        break;
                                                }
                                                else
                                                {
                                                CompleteLearnProfession(pPlayer, _creature, SKILL_INSCRIPTION);
                                                pPlayer->PlayerTalkClass->SendCloseGossip();
                                                }break;
                                        case 9:
                                                if(pPlayer->HasSkill(SKILL_COOKING))
                                                {
                                                        pPlayer->PlayerTalkClass->SendCloseGossip();
                                                        break;
                                                }
                                                else
                                                {
                                                CompleteLearnProfession(pPlayer, _creature, SKILL_COOKING);
                                                pPlayer->PlayerTalkClass->SendCloseGossip();
                                                }break;
                                        case 10:
                                                if(pPlayer->HasSkill(SKILL_FIRST_AID))
                                                {
                                                        pPlayer->PlayerTalkClass->SendCloseGossip();
                                                        break;
                                                }
                                                else
                                                {
                                                CompleteLearnProfession(pPlayer, _creature, SKILL_FIRST_AID);
                                                pPlayer->PlayerTalkClass->SendCloseGossip();
                                                }break;
                                        case 11:
                                            if(pPlayer->HasSkill(SKILL_HERBALISM))
                                            {
                                                pPlayer->PlayerTalkClass->SendCloseGossip();
                                                break;
                                            }
                                            else
                                            {
                                            CompleteLearnProfession(pPlayer, _creature, SKILL_HERBALISM);
                                            pPlayer->PlayerTalkClass->SendCloseGossip();
                                            }break;
                                        case 12:
                                            if(pPlayer->HasSkill(SKILL_SKINNING))
                                            {
                                                pPlayer->PlayerTalkClass->SendCloseGossip();
                                                break;
                                            }
                                            else
                                            {
                                            CompleteLearnProfession(pPlayer, _creature, SKILL_SKINNING);
                                            pPlayer->PlayerTalkClass->SendCloseGossip();
                                            }break;
                                        case 13:
                                            if(pPlayer->HasSkill(SKILL_MINING))
                                            {
                                                pPlayer->PlayerTalkClass->SendCloseGossip();
                                                break;
                                            }
                                            else
                                            {
                                            CompleteLearnProfession(pPlayer, _creature, SKILL_MINING);
                                            pPlayer->PlayerTalkClass->SendCloseGossip();
                                            }break;
                                }


                        }
                        return true;
                }
};

void AddSC_Professions_NPC()
{
    new Professions_NPC();
}
