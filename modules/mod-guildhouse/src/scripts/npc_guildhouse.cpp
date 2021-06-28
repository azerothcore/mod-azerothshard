/*
 * Copyright (C) 2009-2010 Trilogy <http://www.wowtrilogy.com/>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

/**
 *
 * @File : GuildHouse_npc.cpp
 *
 * @Authors : ?
 * @Modific : Lazzalf
 *
 * @Date : 31/03/2010
 *
 * @Version : 0.1
 *
 * edited by sceicco
 **/

#include "GuildHouse.h"
#include "ObjectAccessor.h"
#include "MapManager.h"
#include "ObjectMgr.h"
#include "GuildMgr.h"
#include "ScriptMgr.h"
#include "World.h"
#include "Guild.h"
#include "Teleport.h"
#include "AccountMgr.h"
#include "AZTH.h"
#include "ExtraDatabase.h"
#include "ScriptedGossip.h"

//[AZTH]
#include "AzthUtils.h"

#define SPELL_ID_PASSIVE_RESURRECTION_SICKNESS 15007

#define MSG_GOSSIP_MARRIED          "Vai in viaggio di nozze"
#define MSG_GOSSIP_TELE             "Teletrasportami Alla sede di gilda"
#define MSG_GOSSIP_BUY              "Crea Sede di gilda"
#define MSG_GOSSIP_SELL             "Vendi sede di gilda"
#define MSG_GOSSIP_ADD              "Compra aggiunte per la sede di Gilda"
#define MSG_GOSSIP_NEXTPAGE         "Successivo -->"
#define MSG_GOSSIP_CLOSE            "Chiudi"
#define MSG_INCOMBAT                "Sei in combat!"
#define MSG_NOGUILDHOUSE            "La tua gilda non possiede una casa!"
#define MSG_NOFREEGH                "Purtroppo tutte le case sono occupate oppure non hai abbastanza membri per acquistarne una di quelle libere."
#define MSG_NOADDGH                 "Non hai altre GHAdd da comprare"
#define MSG_ALREADYHAVEGH           "La tua gilda possiede gia' una sede."
#define MSG_ALREADYHAVEGHADD        "La tua gilda possiede gia' questo GHAdd."
#define MSG_NOTENOUGHMONEY          "Non hai abbastanza soldi per acquistare. Hai bisogno di %u gold."
#define MSG_NOTENOUGHGUILDMEMBERS   "Non hai abbastanza membri in gilda per acquistare la casa. Hai bisogno di %u membri."
#define MSG_GHOCCUPIED              "Sfortunatamente questa casa e' gia' occupata."
#define MSG_CONGRATULATIONS         "Congratulazioni! La sede e'stata creata."
#define MSG_SOLD                    "La gilda e' stata venduta. ??? ???? %u ??????."
#define MSG_NOTINGUILD              "Non sei in nessuna gilda."
#define MSG_CONFIRM_BUY             "Conferma l'acquisto"
#define MSG_NEGATE_BUY              "Nega l'acquisto"

#define CODE_SELL                   "SELL"
#define MSG_CODEBOX_SELL            "Scrivi \"" CODE_SELL "\" in maiuscolo per vendere la casa, dopo premi accept."

#define OFFSET_CONFIRM_BUY_ID_TO_ACTION       2000
#define OFFSET_CONFIRM_BUY_ADD_ID_TO_ACTION   5000
#define OFFSET_GH_ID_TO_ACTION                7000
#define OFFSET_SHOWBUY_FROM                   12000
#define OFFSET_GH_ADD_ID_TO_ACTION            17000
#define OFFSET_SHOWBUY_FROM_ADD               20000

#define ACTION_MARRIED            1000
#define ACTION_TELE               1001
#define ACTION_SHOW_BUYLIST       1002
#define ACTION_SELL_GUILDHOUSE    1003
#define ACTION_SHOW_BUYADD_LIST   1004
#define ACTION_CLOSE              1005
#define ACTION_NEGATE_BUY         1011
#define ACTION_NEGATE_BUY_ADD     1021

#define ICON_GOSSIP_BALOON       0
#define ICON_GOSSIP_WING         2
#define ICON_GOSSIP_BOOK         3
#define ICON_GOSSIP_WHEEL1       4
#define ICON_GOSSIP_WHEEL2       5
#define ICON_GOSSIP_GOLD         6
#define ICON_GOSSIP_BALOONDOTS   7
#define ICON_GOSSIP_TABARD       8
#define ICON_GOSSIP_XSWORDS      9

#define GOSSIP_COUNT_MAX         7

class npc_guild_master : public CreatureScript
{
    public:
        npc_guild_master() : CreatureScript("guildmaster") { }

    bool isPlayerGuildLeader(Player *player)
    {
        return ((player->GetRank() == 0) && (player->GetGuildId() != 0));
    };

    bool isPlayerHasGuildhouseAdd(Player *player, Creature *_creature, uint32 add, bool whisper = false)
    {
        uint32 guildadd = GHobj.GetGuildHouse_Add(player->GetGuildId());
        bool comprato = ((uint32(1) << (add - 1)) & guildadd);
        if (comprato)
        {
            if (whisper)
            {
                char msg[200];
                sprintf(msg, MSG_ALREADYHAVEGHADD);
				_creature->MonsterWhisper(MSG_ALREADYHAVEGHADD, player, true);
            }
            return true;
        }
        return false;
    };

    bool isPlayerHasGuildhouse(Player *player, Creature *_creature, bool whisper = false)
    {
        if (!player || !_creature)
            return false;

        uint32 map;

        if (GHobj.GetGuildHouseMap(player->GetGuildId(), map))
        {
            if (whisper)
            {
                //whisper to player "already have etc..."
                char msg[200];
                sprintf(msg, MSG_ALREADYHAVEGH);
                _creature->MonsterWhisper(msg, player);
            }
            return true;
        }
        else
            return false;
    };

    void teleportPlayerToGuildHouse(Player *player, Creature *_creature)
    {
        if (player->GetGuildId() == 0)
        {
            //if player has no guild
            _creature->MonsterWhisper(MSG_NOTINGUILD, player);
            return;
        }

        if (!player->getAttackers().empty())
        {
            //if player in combat
            _creature->MonsterSay(MSG_INCOMBAT, LANG_UNIVERSAL, 0);
            return;
        }

        float x, y, z, o;
        uint32 map;

        if (GHobj.GetGuildHouseLocation(player->GetGuildId(), x, y, z, o, map))
        {
            //teleport player to the specified location
            player->TeleportTo(map, x, y, z, o);
        }
        else
            _creature->MonsterWhisper(MSG_NOGUILDHOUSE, player);
    };

    bool showBuyList(Player *player, Creature *_creature, uint32 showFromId = 0)
    {
        if (!player)
            return false;

        //show not occupied guildhouses

        QueryResult result;

        uint32 guildsize = 1;

        Guild * guild = sGuildMgr->GetGuildById(player->GetGuildId());
        if (guild)
            guildsize = guild->GetMemberSize();

        if (player->IsGameMaster())
            guildsize = 20000;

        result = ExtraDatabase.PQuery("SELECT `id`, `comment`, `price` FROM `guildhouses` WHERE `guildId` = 0 AND (`faction` = 3 OR `faction` = %u) AND `id` > %u AND `minguildsize` <= %u ORDER BY `id` ASC LIMIT %u",
            (player->GetTeamId(true) == TEAM_HORDE)?1:0, showFromId, guildsize, GOSSIP_COUNT_MAX);

        if (result)
        {
            uint32 guildhouseId = 0;
            std::string comment = "";
            uint32 price = 0;
            do
            {
                Field *fields = result->Fetch();
                if (!fields)
                    break;

                guildhouseId = fields[0].GetInt32();
                comment = fields[1].GetString();
                price = fields[2].GetUInt32();

                std::stringstream complete_comment;
                complete_comment << "price " << price << " - " << comment;

                //send comment as a gossip item

                //transmit guildhouseId in Action variable
                AddGossipItemFor(player,ICON_GOSSIP_TABARD, complete_comment.str().c_str(), GOSSIP_SENDER_MAIN,
                    guildhouseId + OFFSET_GH_ID_TO_ACTION);

            } while (result->NextRow());

            if (result->GetRowCount() == GOSSIP_COUNT_MAX)
            {
                //assume that we have additional page

                //add link to next GOSSIP_COUNT_MAX items
                AddGossipItemFor(player,ICON_GOSSIP_BALOONDOTS, MSG_GOSSIP_NEXTPAGE, GOSSIP_SENDER_MAIN,
                    guildhouseId + OFFSET_SHOWBUY_FROM);
            }
            AddGossipItemFor(player,ICON_GOSSIP_BALOONDOTS, MSG_GOSSIP_CLOSE, GOSSIP_SENDER_MAIN,
                ACTION_CLOSE);

            SendGossipMenuFor(player,DEFAULT_GOSSIP_MESSAGE, _creature->GetGUID());

            return true;
        }
        else
        {
            if (showFromId == 0)
            {
                //all guildhouses are occupied
                _creature->MonsterWhisper(MSG_NOFREEGH, player);
                CloseGossipMenuFor(player);
            }
            else
            {
                //this condition occurs when COUNT(guildhouses) % GOSSIP_COUNT_MAX == 0
                //just show GHs from beginning
                showBuyList(player, _creature, 0);
            }
        }

        return false;
    };

    bool showBuyAddList(Player *player, Creature *_creature, uint32 showFromId = 0)
    {
        if (!player)
            return false;

        QueryResult result;

        uint32 guildsize = 1;
        uint32 guild_add = GHobj.GetGuildHouse_Add(player->GetGuildId());

		Guild *guild = sGuildMgr->GetGuildById(player->GetGuildId());
        if (guild)
            guildsize = guild->GetMemberSize();

        if (player->IsGameMaster())
            guildsize = 20000;

        result = WorldDatabase.PQuery("SELECT `add_type`, `comment`, `price` FROM `guildhouses_addtype` WHERE `minguildsize` <= %u AND `add_type` > %u ORDER BY `add_type` ASC LIMIT %u",
            guildsize, showFromId, GOSSIP_COUNT_MAX);

        if (result)
        {
            uint32 add_typeId = 0;
            std::string comment = "";
            uint32 price = 0;
            do
            {
                Field *fields = result->Fetch();

                add_typeId = fields[0].GetInt32();
                comment = fields[1].GetString();
                price = fields[2].GetUInt32();

                uint32 comprato = ((uint32(1) << (add_typeId - 1)) & guild_add);

                std::stringstream complete_comment;
                if(comprato)
                    complete_comment << "(Comprato) "<< comment;
                else
                    complete_comment << "price " << price << " - " << comment;

                //send comment as a gossip item
                //transmit guildhouseId in Action variable
                AddGossipItemFor(player,ICON_GOSSIP_TABARD, complete_comment.str().c_str(), GOSSIP_SENDER_MAIN,
                    add_typeId + OFFSET_GH_ADD_ID_TO_ACTION);

            } while (result->NextRow());

            if (result->GetRowCount() == GOSSIP_COUNT_MAX)
            {
                //assume that we have additional page
                //add link to next GOSSIP_COUNT_MAX items
                AddGossipItemFor(player,ICON_GOSSIP_BALOONDOTS, MSG_GOSSIP_NEXTPAGE, GOSSIP_SENDER_MAIN,
                    add_typeId + OFFSET_SHOWBUY_FROM_ADD);
            }

            AddGossipItemFor(player,ICON_GOSSIP_BALOONDOTS, MSG_GOSSIP_CLOSE, GOSSIP_SENDER_MAIN,
                ACTION_CLOSE);

            SendGossipMenuFor(player,DEFAULT_GOSSIP_MESSAGE, _creature->GetGUID());

            return true;
        }
        else
        {
            if (showFromId == 0)
            {
                //all no GhAdd to Show
                _creature->MonsterWhisper(MSG_NOADDGH, player);
                CloseGossipMenuFor(player);
            }
            else
            {
                //just show GHsAdd from beginning
                showBuyAddList(player, _creature, 0);
            }
        }
        return false;
    };

    bool confirmBuy(Player *player, Creature *_creature, uint32 guildhouseId)
    {
        if (!player)
            return false;

        if (isPlayerHasGuildhouse(player, _creature, true))
        {
            //player already have GH
            return false;
        }

        AddGossipItemFor(player,ICON_GOSSIP_GOLD, MSG_CONFIRM_BUY, GOSSIP_SENDER_MAIN,
            guildhouseId + OFFSET_CONFIRM_BUY_ID_TO_ACTION);
        AddGossipItemFor(player,ICON_GOSSIP_BALOONDOTS, MSG_NEGATE_BUY, GOSSIP_SENDER_MAIN,
                    ACTION_NEGATE_BUY);

        SendGossipMenuFor(player,DEFAULT_GOSSIP_MESSAGE, _creature->GetGUID());

        return true;
    };

    void buyGuildhouse(Player *player, Creature *_creature, uint32 guildhouseId)
    {
        if (!player)
            return;

        if (isPlayerHasGuildhouse(player, _creature, true))
        {
            //player already have GH
            return;
        }

        QueryResult result;

        result = ExtraDatabase.PQuery("SELECT `price` FROM `guildhouses` WHERE `id` = %u AND `guildId` = 0" , guildhouseId);

        if (!result)
        {
            _creature->MonsterWhisper(MSG_GHOCCUPIED, player);
            return;
        }

        Field *fields = result->Fetch();
        int32 price = fields[0].GetInt32();

        if (player->GetMoney() < uint32(price) * 10000)
        {
            //show how much money player need to buy GH (in gold)
            char msg[200];
            sprintf(msg, MSG_NOTENOUGHMONEY, price);
            _creature->MonsterWhisper(msg, player);
            return;
        }

        GHobj.ChangeGuildHouse(player->GetGuildId(), guildhouseId);

        player->ModifyMoney(-(price*10000));
        _creature->MonsterSay(MSG_CONGRATULATIONS, LANG_UNIVERSAL, 0);
    };

    bool confirmBuyAdd(Player *player, Creature *_creature, uint32 gh_Add)
    {
        if (!player)
            return false;

        AddGossipItemFor(player,ICON_GOSSIP_GOLD, MSG_CONFIRM_BUY, GOSSIP_SENDER_MAIN,
            gh_Add + OFFSET_CONFIRM_BUY_ADD_ID_TO_ACTION);
        AddGossipItemFor(player,ICON_GOSSIP_BALOONDOTS, MSG_NEGATE_BUY, GOSSIP_SENDER_MAIN,
                    ACTION_NEGATE_BUY_ADD);

        SendGossipMenuFor(player,DEFAULT_GOSSIP_MESSAGE, _creature->GetGUID());

        return true;
    };

    void buyGuildhouseAdd(Player *player, Creature *_creature, uint32 gh_Add)
    {
        if (!player)
            return;

        if (isPlayerHasGuildhouseAdd(player, _creature, gh_Add, true))
        {
            //player already have GHAdd
            return;
        }

        QueryResult result = WorldDatabase.PQuery("SELECT `price` FROM `guildhouses_addtype` WHERE `add_type` = %u ", gh_Add);
        if (!result)
            return;

        Field *fields = result->Fetch();
        int32 price = fields[0].GetInt32();

        if (player->GetMoney() < uint32(price) * 10000)
        {
            //show how much money player need to buy GH (in gold)
            char msg[200];
            sprintf(msg, MSG_NOTENOUGHMONEY, price);
            _creature->MonsterWhisper(msg, player);
            return;
        }

        GHobj.Add_GuildhouseAdd(player->GetGuildId(), gh_Add);

        player->ModifyMoney(-(price*10000));
    };

    void sellGuildhouse(Player *player, Creature *_creature)
    {
        if (!player)
            return;

        if (isPlayerHasGuildhouse(player, _creature))
        {
            QueryResult result;

            result = ExtraDatabase.PQuery("SELECT `price` FROM `guildhouses` WHERE `guildId` = %u", player->GetGuildId());

            if (!result)
                return;

            Field *fields = result->Fetch();
            uint32 price = fields[0].GetUInt32();

            GHobj.ChangeGuildHouse(player->GetGuildId(),0);

            uint32 sellPrice = ( (price * GOLD) / 100 ) * 75; // 75% of refunding
            player->ModifyMoney(sellPrice);
            //display message e.g. "here your money etc."
            char msg[200];
            sprintf(msg, MSG_SOLD, price * 3 / 4);
            _creature->MonsterWhisper(msg, player);
        }
    };

    bool OnGossipHello(Player *player, Creature *_creature)
    {
        if (!player)
            return true;
		/*
        if (isPlayerMarried(player))
            AddGossipItemFor(player,ICON_GOSSIP_BALOON, MSG_GOSSIP_MARRIED,
                GOSSIP_SENDER_MAIN, ACTION_MARRIED);
	    */
        AddGossipItemFor(player,ICON_GOSSIP_BALOON, MSG_GOSSIP_TELE,
            GOSSIP_SENDER_MAIN, ACTION_TELE);

        if (isPlayerGuildLeader(player))
        {
            //show additional menu for guild leader
            AddGossipItemFor(player,ICON_GOSSIP_GOLD, MSG_GOSSIP_BUY, GOSSIP_SENDER_MAIN, ACTION_SHOW_BUYLIST);
            if (isPlayerHasGuildhouse(player, _creature))
            {
                //and additional for guildhouse owner
                AddGossipItemFor(player,ICON_GOSSIP_GOLD, MSG_GOSSIP_SELL, GOSSIP_SENDER_MAIN, ACTION_SELL_GUILDHOUSE, MSG_CODEBOX_SELL, 0, true);
                AddGossipItemFor(player,ICON_GOSSIP_GOLD, MSG_GOSSIP_ADD, GOSSIP_SENDER_MAIN, ACTION_SHOW_BUYADD_LIST);
            }
        }
        SendGossipMenuFor(player,DEFAULT_GOSSIP_MESSAGE, _creature->GetGUID());
        return true;
    };


    bool OnGossipSelect(Player *player, Creature *_creature, uint32 sender, uint32 action )
    {
        player->PlayerTalkClass->ClearMenus();

        if (sender != GOSSIP_SENDER_MAIN)
            return false;

        switch (action)
        {
            case ACTION_TELE:
                //teleport player to GH
                CloseGossipMenuFor(player);
                teleportPlayerToGuildHouse(player, _creature);
                break;
            case ACTION_SHOW_BUYLIST:
                //show list of GHs which currently not occupied
                showBuyList(player, _creature);
                break;
            case ACTION_SHOW_BUYADD_LIST:
                //show list of GHs add
                showBuyAddList(player, _creature);
                break;
            case ACTION_CLOSE:
                CloseGossipMenuFor(player);
                break;
            case ACTION_NEGATE_BUY:
                CloseGossipMenuFor(player);
                break;
             case ACTION_NEGATE_BUY_ADD:
                CloseGossipMenuFor(player);
                break;
            default:
                if (action > OFFSET_SHOWBUY_FROM_ADD)
                {
                    showBuyAddList(player, _creature, action - OFFSET_SHOWBUY_FROM_ADD);
                }
                else if (action > OFFSET_GH_ADD_ID_TO_ACTION)
                {
                    confirmBuyAdd(player, _creature, action - OFFSET_GH_ADD_ID_TO_ACTION);
                }
                else if (action > OFFSET_SHOWBUY_FROM)
                {
                    showBuyList(player, _creature, action - OFFSET_SHOWBUY_FROM);
                }
                else if (action > OFFSET_GH_ID_TO_ACTION)
                {
                    confirmBuy(player, _creature, action - OFFSET_GH_ID_TO_ACTION);
                }
                else if (action > OFFSET_CONFIRM_BUY_ADD_ID_TO_ACTION)
                {
                    //player clicked on buy list
                    //get guildhouseAddId from action
                    //guildhouseAddId = action - OFFSET_CONFIRM_BUY_ADD_ID_TO_ACTION
                    buyGuildhouseAdd(player, _creature, action - OFFSET_CONFIRM_BUY_ADD_ID_TO_ACTION);
                    CloseGossipMenuFor(player);
                    player->SaveToDB(false,false);
                }
                else if (action > OFFSET_CONFIRM_BUY_ID_TO_ACTION)
                {
                    //player clicked on buy list
                    //get guildhouseId from action
                    //guildhouseId = action - OFFSET_CONFIRM_BUY_ID_TO_ACTION
                    buyGuildhouse(player, _creature, action - OFFSET_CONFIRM_BUY_ID_TO_ACTION);
                    CloseGossipMenuFor(player);
                    player->SaveToDB(false, false);
                }
                break;
        }

        return true;
    };

    bool OnGossipSelectCode( Player *player, Creature *_creature,
                             uint32 sender, uint32 action, const char* sCode )
    {
        if (sender == GOSSIP_SENDER_MAIN)
        {
            if (action == ACTION_SELL_GUILDHOUSE)
            {
                int i = -1;
                try
                {
                    //compare code
                    if (strlen(sCode) + 1 == sizeof CODE_SELL)
                        i = strcmp(CODE_SELL, sCode);

                }
				catch(char* /*str*/) {
					//TC_LOG_DEBUG("guildhouses",str);
				}

                if (i == 0)
                {
                    //right code
                    sellGuildhouse(player, _creature);
                    player->SaveToDB(false,false);
                }
                CloseGossipMenuFor(player);
                return true;
            }
        }
        return false;
    };
};

/*########
# guild_guard
#########*/

#define SAY_AGGRO "Nemico individuato! Obiettivo Eliminato!"
#define SAY_WARNING "Attenzione! Ti stai avvicinando ad una sede di gilda. Allontanti o verrai ucciso se ti avvicini ulteriormente"
#define NPC_GUARD_1 5000003

class guild_guard : public CreatureScript
{
    public:
    guild_guard() : CreatureScript("guild_guard") { }

    bool OnGossipHello(Player* player, Creature* _Creature)
    {
        uint32 guardguild = GHobj.GetGuildByGuardID(_Creature);
		/////////////DEBUG////////////////
		char msg[500];
		if (AccountMgr::IsGMAccount(player->GetSession()->GetSecurity()))
		{
			_Creature->MonsterWhisper("Ciao gm!", player); //solo gm traduzione non necessaria
			if (guardguild)
			{
				sprintf(msg, "Il mio id di gilda: %u", guardguild);
				_Creature->MonsterWhisper(msg, player);
				if (_Creature->GetEntry() == NPC_GUARD_1)
					_Creature->MonsterWhisper("Il mio raggio d'azione: 100y", player);
				else
					_Creature->MonsterWhisper("Il mio raggio d'azione: 50y", player);
			}
			else
				_Creature->MonsterWhisper("Sono una Guardia Bugga!", player);
		}

		/////////////////////////////////
        uint32 guild = player->GetGuildId();
        if (guardguild && (guild == guardguild || AccountMgr::IsGMAccount(player->GetSession()->GetSecurity())))
        {
            if (_Creature->GetAI())
            {
                if (_Creature->GetAI()->GetData(0))
					AddGossipItemFor(player,5, "Disable Protection", GOSSIP_SENDER_MAIN, 11);
                else
                    AddGossipItemFor(player,5, "Activate Protection", GOSSIP_SENDER_MAIN, 10);
            }
        }
		AddGossipItemFor(player,5, "Chiudi", GOSSIP_SENDER_MAIN, 12);
		SendGossipMenuFor(player,DEFAULT_GOSSIP_MESSAGE, _Creature->GetGUID());
        return true;
	};

    bool OnGossipSelect(Player* player, Creature* _Creature, uint32 sender, uint32 action)
    {
        player->PlayerTalkClass->ClearMenus();

        // Main menu
        if (sender == GOSSIP_SENDER_MAIN)
        {
            if (action == 10)
            {
                if (_Creature->GetAI())
                {
                    _Creature->GetAI()->SetData(0,1);
                    _Creature->MonsterYell("Protezione Attivata", LANG_UNIVERSAL, 0);
                }

            }
            else if (action == 11)
            {
                 if (_Creature->GetAI())
                 {
                    _Creature->GetAI()->SetData(0,0);
                    _Creature->MonsterYell("Protezione Disattivata", LANG_UNIVERSAL, 0);
                 }
            }
        }
        CloseGossipMenuFor(player);
        return true;
    };

    struct guild_guardAI : public ScriptedAI
    {
        guild_guardAI(Creature *c) : ScriptedAI(c)
        {
            activate = true;

            if (me->GetEntry() == NPC_GUARD_1)
            {
                dist_kill = 100;
                dist_warning = 140;
            }
            else
            {
                dist_kill = 50;
                dist_warning = 70;
            }
        }

        bool activate;
        uint32 Check_Timer;
        uint32 dist_kill;
        uint32 dist_warning;
		uint32 guardguild;

        uint32 GetData(uint32  /*type*/) const override
        {
            if (activate)
                return 1;
            else
                return 0;
        }

		void SetData(uint32  /*id*/, uint32 value) override
        {
            if (value)
                activate = true;
            else
                activate = false;
        }

        void Reset() override
        {
            Check_Timer = 1000;
			guardguild = GHobj.GetGuildByGuardID(me);

        }

        void DamageTaken(Unit* /*attacker*/, uint32& damage, DamageEffectType /*damagetype*/, SpellSchoolMask /*damageSchoolMask*/) override
        {
            damage = 0;
        }

        void UpdateAI(uint32 uiDiff) override
        {
            if (!activate)
                return;

            if (Check_Timer <= uiDiff)
            {

                Map::PlayerList const &PlayerList = me->GetMap()->GetPlayers();
                if (!PlayerList.isEmpty())
                    for (Map::PlayerList::const_iterator i = PlayerList.begin(); i != PlayerList.end(); ++i)
                    {
                        Player* plr = i->GetSource();

                        if (!plr)
                            continue;

                        if (!plr->IsAlive() || plr->GetTransport() || !plr->CanFreeMove())
                            continue;

                        if (plr->GetSession() && AccountMgr::IsGMAccount(plr->GetSession()->GetSecurity()))
						{
                            continue;
						}

						if (plr->GetDistance(me) <= dist_kill)
                        {
                            uint32 guild = plr->GetGuildId();
                            if (guardguild && guild != guardguild)
                            {
                                me->MonsterYell(SAY_AGGRO, LANG_UNIVERSAL, 0);
                                me->Kill(me,plr);
                            }
                        }
                        else if (plr->GetDistance(me) <= dist_warning)
                        {
                            uint32 guild = plr->GetGuildId();
                            if (guardguild && guild != guardguild)
                            {
								me->MonsterWhisper(SAY_WARNING, plr);
                            }
                        }
                    }
                Check_Timer = 2000;
            } else Check_Timer -= uiDiff;
        }
    };

    CreatureAI* GetAI(Creature *_Creature) const
    {
        return new guild_guardAI(_Creature);
    };
};

/*########
# npc_buffnpc
#########*/

class npc_buffnpc : public CreatureScript
{
    public:

    npc_buffnpc() : CreatureScript("buffnpc") { }

    bool OnGossipHello(Player *player, Creature *_Creature)
    {
        //player->SetTaxiCheater(true);

        // Main Menu for Alliance
        if ( player->GetTeamId(true) == TEAM_ALLIANCE )
        {
            AddGossipItemFor(player, 5, "Remove Res Sickness"                           , GOSSIP_SENDER_MAIN, 1180);
            //AddGossipItemFor(player, 5, "Give me gold"                                , GOSSIP_SENDER_MAIN, 1185);
            //AddGossipItemFor(player, 5, "Give me Soul Shards"                         , GOSSIP_SENDER_MAIN, 1190);
            AddGossipItemFor(player, 5, "Heal me please"                                , GOSSIP_SENDER_MAIN, 1195);
            AddGossipItemFor(player, 5, "Ritual of Souls please"                        , GOSSIP_SENDER_MAIN, 1200);
            AddGossipItemFor(player, 5, "Table please"                                  , GOSSIP_SENDER_MAIN, 1205);
            if (player->getLevel() > 69)
            {
                AddGossipItemFor(player, 5, "Buff me Arcane Intelect"                       , GOSSIP_SENDER_MAIN, 1210);
                AddGossipItemFor(player, 5, "Buff me Mark of the Wild"                      , GOSSIP_SENDER_MAIN, 1215);
                AddGossipItemFor(player, 5, "Buff me Thorns"                                , GOSSIP_SENDER_MAIN, 1220);
                AddGossipItemFor(player, 5, "Buff me Blessing of Sanctuary"                 , GOSSIP_SENDER_MAIN, 1225);
                AddGossipItemFor(player, 5, "Buff me Blessing of Might"                     , GOSSIP_SENDER_MAIN, 1230);
                //AddGossipItemFor(player, 5, "Buff me Greater Blessing of Light"             , GOSSIP_SENDER_MAIN, 1235);
                AddGossipItemFor(player, 5, "Buff me Blessing of Wisdom"                    , GOSSIP_SENDER_MAIN, 1240);
                AddGossipItemFor(player, 5, "Buff me Blessing of Kings"                     , GOSSIP_SENDER_MAIN, 1245);
                AddGossipItemFor(player, 5, "Buff me Divine Spirit"                         , GOSSIP_SENDER_MAIN, 1250);
                AddGossipItemFor(player, 5, "Buff me Shadow Protection"                     , GOSSIP_SENDER_MAIN, 1251);
                AddGossipItemFor(player, 5, "Buff me Power Word: Fortitude"                 , GOSSIP_SENDER_MAIN, 1252);
            }
        }
        else // Main Menu for Horde
        {
            AddGossipItemFor(player, 5, "Remove Res Sickness"                           , GOSSIP_SENDER_MAIN, 1180);
            //AddGossipItemFor(player, 5, "Give me gold"                                , GOSSIP_SENDER_MAIN, 1185);
            //AddGossipItemFor(player, 5, "Give me Soul Shards"                         , GOSSIP_SENDER_MAIN, 1190);
            AddGossipItemFor(player, 5, "Heal me please"                                , GOSSIP_SENDER_MAIN, 1195);
            AddGossipItemFor(player, 5, "Ritual of Souls please"                        , GOSSIP_SENDER_MAIN, 1200);
            AddGossipItemFor(player, 5, "Table please"                                  , GOSSIP_SENDER_MAIN, 1205);
            if (player->getLevel() > 69)
            {
                AddGossipItemFor(player, 5, "Buff me Arcane Intelect"                       , GOSSIP_SENDER_MAIN, 1210);
                AddGossipItemFor(player, 5, "Buff me Mark of the Wild"                      , GOSSIP_SENDER_MAIN, 1215);
                AddGossipItemFor(player, 5, "Buff me Thorns"                                , GOSSIP_SENDER_MAIN, 1220);
                AddGossipItemFor(player, 5, "Buff me Blessing of Sanctuary"                 , GOSSIP_SENDER_MAIN, 1225);
                AddGossipItemFor(player, 5, "Buff me Blessing of Might"                     , GOSSIP_SENDER_MAIN, 1230);
                //AddGossipItemFor(player, 5, "Buff me Greater Blessing of Light"             , GOSSIP_SENDER_MAIN, 1235);
                AddGossipItemFor(player, 5, "Buff me Blessing of Wisdom"                    , GOSSIP_SENDER_MAIN, 1240);
                AddGossipItemFor(player, 5, "Buff me Blessing of Kings"                     , GOSSIP_SENDER_MAIN, 1245);
                AddGossipItemFor(player, 5, "Buff me Divine Spirit"                         , GOSSIP_SENDER_MAIN, 1250);
                AddGossipItemFor(player, 5, "Buff me Shadow Protection"                     , GOSSIP_SENDER_MAIN, 1251);
                AddGossipItemFor(player, 5, "Buff me Power Word: Fortitude"                 , GOSSIP_SENDER_MAIN, 1252);
            }
        }

        SendGossipMenuFor(player,DEFAULT_GOSSIP_MESSAGE,_Creature->GetGUID());

        return true;
    };

    void SendDefaultMenu_buffnpc(Player *player, Creature *_Creature, uint32 action )
    {
        // Not allow in combat
        if(!player->getAttackers().empty())
        {
            CloseGossipMenuFor(player);
            _Creature->MonsterSay(MSG_INCOMBAT, LANG_UNIVERSAL, 0);
            return;
        }

        switch(action)
        {
            case 1180://Remove Res Sickness
                if(!player->HasAura(SPELL_ID_PASSIVE_RESURRECTION_SICKNESS))
                {
                    OnGossipHello(player, _Creature);
                    return;
                }

                _Creature->CastSpell(player,38588,false); // Healing effect
                player->RemoveAurasDueToSpell(SPELL_ID_PASSIVE_RESURRECTION_SICKNESS);
                CloseGossipMenuFor(player);
                break;

            /*case 1185://Give me Gold
                _Creature->CastSpell(player,46642,false); // 5000 gold
            break;*/

            /*case 1190://Give me Soul Shards
                player->CastSpell(player,24827,false);
            break;*/

            case 1195: // Heal me please
                CloseGossipMenuFor(player);
                _Creature->CastSpell(player,/*38588*/25840,false);
                break;
            case 1200: // Ritual of Souls please
                CloseGossipMenuFor(player);
                player->CastSpell(player,58889,false);
                break;
            case 1205: // Table please
                CloseGossipMenuFor(player);
                player->CastSpell(player,58661,false);
                break;
            case 1210: // Buff me Arcane Intelect
                CloseGossipMenuFor(player);
                _Creature->CastSpell(player,42995,false);
                break;
            case 1215: // Buff me Mark of the Wild
                CloseGossipMenuFor(player);
                _Creature->CastSpell(player,48469,false);
                break;
            case 1220: // Buff me Thorns
                CloseGossipMenuFor(player);
                _Creature->CastSpell(player,26992,false);
                break;
            case 1225: // Buff me Blessing of Sanctuary
                CloseGossipMenuFor(player);
                _Creature->CastSpell(player,20911,false);
                break;
            case 1230: // Buff me Blessing of Might
                CloseGossipMenuFor(player);
                _Creature->CastSpell(player,48932,false);
                break;
            case 1235: // Buff me Greater Blessing of Light
                CloseGossipMenuFor(player);
                _Creature->CastSpell(player,27145,false);
                break;
            case 1240: // Buff me Blessing of Wisdom
                CloseGossipMenuFor(player);
                _Creature->CastSpell(player,48936,false);
                break;
            case 1245: // Buff me Blessing of Kings
                CloseGossipMenuFor(player);
                _Creature->CastSpell(player,20217,false);
                break;
            case 1250: // Buff me Divine Spirit
                CloseGossipMenuFor(player);
                _Creature->CastSpell(player,48073,false);
                break;
            case 1251: // Buff me Shadow Protection
                CloseGossipMenuFor(player);
                _Creature->CastSpell(player,48169,false);
                break;
            case 1252: // Buff me Power Word: Fortitude
                CloseGossipMenuFor(player);
                _Creature->CastSpell(player,48161,false);
                break;
        }
    };

    bool OnGossipSelect(Player *player, Creature *_Creature, uint32 sender, uint32 action)
    {
        player->PlayerTalkClass->ClearMenus();

        // Main menu
        if (sender == GOSSIP_SENDER_MAIN)
        SendDefaultMenu_buffnpc( player, _Creature, action );

        return true;
    };
};

/*########
# npc_portal
#########*/

#define SPELL_VISUAL_TELEPORT   35517

// Teleport Player
void Teleport(Player *player, uint16 map,
                float X, float Y, float Z, float orient)
{
    uint32 aurDim=sAZTH->GetAZTHPlayer(player)->getCurrentDimensionByAura();
    if (sAzthUtils->isPhasedDimension(aurDim) && !sAZTH->GetAZTHPlayer(player)->changeDimension(DIMENSION_NORMAL, true))
        return;

    CloseGossipMenuFor(player);
    player->CastSpell(player, SPELL_VISUAL_TELEPORT, true);
    if (!player->TeleportTo(map, X, Y, Z, orient) && sAzthUtils->isPhasedDimension(aurDim))
        sAZTH->GetAZTHPlayer(player)->changeDimension(aurDim); // we have to restore old dimension to avoid exploits if teleport failed
};

class npc_portal : public CreatureScript
{
    public:

    npc_portal() : CreatureScript("portal_npc") { }

    bool OnGossipHello(Player *player, Creature *_Creature)
    {
        AddGossipItemFor(player, 5, "Teleport Dalaran"              , GOSSIP_SENDER_MAIN, 1005);
        AddGossipItemFor(player, 5, "Teleport Shattrath"            , GOSSIP_SENDER_MAIN, 1010);
        AddGossipItemFor(player, 5, "Teleport Wintergrasp"          , GOSSIP_SENDER_MAIN, 1100);

        // Main Menu for Alliance
        if ( player->GetTeamId(true) == TEAM_ALLIANCE )
        {
            AddGossipItemFor(player, 5, "Teleport Stormwind"            , GOSSIP_SENDER_MAIN, 1015);
            AddGossipItemFor(player, 5, "Teleport Ironforge"            , GOSSIP_SENDER_MAIN, 1020);
            AddGossipItemFor(player, 5, "Teleport Darnassus"            , GOSSIP_SENDER_MAIN, 1025);
            AddGossipItemFor(player, 5, "Teleport Exodar"               , GOSSIP_SENDER_MAIN, 1030);
        }
        else // Main Menu for Horde
        {
            AddGossipItemFor(player, 5, "Teleport Orgrimmar"            , GOSSIP_SENDER_MAIN, 1035);
            AddGossipItemFor(player, 5, "Teleport Undercity"            , GOSSIP_SENDER_MAIN, 1040);
            AddGossipItemFor(player, 5, "Teleport Thunder Bluff"        , GOSSIP_SENDER_MAIN, 1045);
            AddGossipItemFor(player, 5, "Teleport Silvermoon"           , GOSSIP_SENDER_MAIN, 1050);
        }

        SendGossipMenuFor(player,DEFAULT_GOSSIP_MESSAGE,_Creature->GetGUID());

        return true;
    };

    void SendDefaultMenu_portal_npc(Player *player, Creature *_Creature, uint32 action)
    {
        // Not allow in combat
        if(!player->getAttackers().empty())
        {
            CloseGossipMenuFor(player);
            _Creature->MonsterSay(MSG_INCOMBAT, LANG_UNIVERSAL, 0);
            return;
        }

		//OutdoorPvP *pvpWG = (OutdoorPvP*)sOutdoorPvPMgr->GetOutdoorPvPToZoneId(4197);

        switch(action)
        {
            case 1005: //Dalaran
                Teleport(player, 571, 5804.15f, 624.77f, 647.8f, 1.64f);
                break;
            case 1010: // Shattrath
                Teleport(player, 530, -1838.16f, 5301.79f, -12.5f, 5.95f);
                break;
            case 1015: // Stormwind
                Teleport(player, 0, -8992.20f, 848.46f, 29.63f, 0);
                break;
            case 1020: // Ironforge
                Teleport(player, 0, -4602.75f, -906.53f, 502.76f, 0);
                break;
            case 1025: // Darnassus
                Teleport(player, 1, 9952.13f, 2283.35f, 1341.40f, 0);
                break;
            case 1030: // Exodar
                Teleport(player, 530, -4021.21f, -11561.82f, -138.14f, 0);
                break;
            case 1035: // Orgrimmar
                Teleport(player, 1, 1469.64f, -4221.07f, 59.23f, 0);
                break;
            case 1040: // Undercity
                Teleport(player, 0, 1769.64f, 64.17f, -46.33f, 0);
                break;
            case 1045: // Thunder Bluff
                Teleport(player, 1, -970.36f, 284.84f, 111.41f, 0);
                break;
            case 1050: // Silvermoon
                Teleport(player, 530, 10000.25f, -7112.02f, 47.71f, 0);
                break;
            case 1100: //Wintergrasp
                Teleport(player, 571, 4525.60f, 2828.08f, 390, 0.28f); //Out the Fortress
                break;
        }
    };

    bool OnGossipSelect(Player *player, Creature *_Creature, uint32 sender, uint32 action)
    {
        player->PlayerTalkClass->ClearMenus();

        // Main menu
        if (sender == GOSSIP_SENDER_MAIN)
            SendDefaultMenu_portal_npc( player, _Creature, action );

        return true;
    };
};

#define GOSSIP_SHOW_DEST        1000
#define GOSSIP_TELEPORT         1001
#define GOSSIP_NEXT_PAGEC       1002
#define GOSSIP_PREV_PAGEC       1003
#define GOSSIP_NEXT_PAGED       1004
#define GOSSIP_PREV_PAGED       1005
#define GOSSIP_MAIN_MENU        1006

#define NB_ITEM_PAGE            10
#define MSG_CAT                 100000
#define MSG_DEST                100001

#define NEXT_PAGE               "-> [Next Page]"
#define PREV_PAGE               "<- [Previous Page]"
#define MAIN_MENU               "<= [Main Menu]"


using namespace nsNpcTel;


namespace
{
    Page PageC, PageD;
    Page Cat;

    // Conversion function int->string
    std::string ConvertStr(const int64 &val)
    {
        std::ostringstream ostr;
        ostr << val;
        return ostr.str();
    };

    // Conversion function intMoney->stringMoney
    std::string ConvertMoney(const uint32 &Money)
    {
        std::string Str(ConvertStr(Money));
        uint32 SizeStr = Str.length();

        if (SizeStr > 4)
            Str = Str.insert(Str.length()-4, "po");
        if (SizeStr > 2)
            Str = Str.insert(Str.length()-2, "pa");
        Str += "pc";

        return Str;
    };

    // Display categories
    void AffichCat(Player * const player, Creature * const creature)
    {
        if (PageC[player] > 0)
            AddGossipItemFor(player,7, PREV_PAGE, GOSSIP_PREV_PAGEC, 0);

        VCatDest_t i (PageC[player] * NB_ITEM_PAGE);
        for ( ; i < TabCatDest.size() && i < (NB_ITEM_PAGE * (PageC[player] + 1)); ++i)
        {
            if (TabCatDest[i].IsAllowedToTeleport(player))
                AddGossipItemFor(player,7, TabCatDest[i].GetName(player->IsGameMaster()).c_str(), GOSSIP_SHOW_DEST, i);
        }

        if (i < TabCatDest.size())
            AddGossipItemFor(player,7, NEXT_PAGE, GOSSIP_NEXT_PAGEC, 0);

        SendGossipMenuFor(player,MSG_CAT, creature->GetGUID());
    };

    // Display destination categories
    void AffichDest(Player * const player, Creature * const creature)
    {
        if (PageD[player] > 0)
            AddGossipItemFor(player,7, PREV_PAGE, GOSSIP_PREV_PAGED, 0);

        CatDest::VDest_t i (PageD[player] * NB_ITEM_PAGE);
        for ( ; i < TabCatDest[Cat[player]].size() && i < (NB_ITEM_PAGE * (PageD[player] + 1)); ++i)
        {
            AddGossipItemFor(player,5, TabCatDest[Cat[player]].GetDest(i).m_name.c_str(), GOSSIP_TELEPORT, i);
        }

        if (i < TabCatDest[Cat[player]].size())
            AddGossipItemFor(player,7, NEXT_PAGE, GOSSIP_NEXT_PAGED, 0);

        if (CatDest::CountOfCategoryAllowedBy(player) > 1)
            AddGossipItemFor(player,7, MAIN_MENU, GOSSIP_MAIN_MENU, 0);

        SendGossipMenuFor(player,MSG_DEST, creature->GetGUID());
    };

    // Verification before teleportation
    void ActionTeleport(Player * const player, Creature * const creature, const uint32 &id)
    {
        Dest dest (TabCatDest[Cat[player]].GetDest(id));

        if (player->getLevel() < dest.m_level && !player->IsGameMaster())
        {
            std::string msg ("You do not have the required level. This destination requires level " + ConvertStr(dest.m_level) + ".");
            creature->MonsterWhisper(msg.c_str(), player);
            return;
        }

        if (player->GetMoney() < dest.m_cost && !player->IsGameMaster())
        {
            std::string msg ("You do not have enough money. The price for teleportation is " + ConvertMoney(dest.m_cost) + ".");
            creature->MonsterWhisper(msg.c_str(), player);
            return;
        }

        if (!player->IsGameMaster() && dest.m_cost)
            player->ModifyMoney(-1 * dest.m_cost);

        Teleport(player, dest.m_map, dest.m_X, dest.m_Y, dest.m_Z, dest.m_orient);
    };
};

class npc_teleport : public CreatureScript
{
    public:
    npc_teleport() : CreatureScript("npc_teleport") { }

    bool OnGossipHello(Player *player, Creature *creature)
    {
        PageC(player) = PageD(player) = Cat(player) = 0;

        if(player->IsInCombat())
        {
            CloseGossipMenuFor(player);
            creature->MonsterWhisper("You are in combat. Come back later", player);
            return true;
        }
        AffichCat(player, creature);
        return true;
    };

    bool OnGossipSelect(Player *player, Creature *creature, uint32 sender, uint32 param)
    {
        player->PlayerTalkClass->ClearMenus();
        switch(sender)
        {
          // Display destinations
          case GOSSIP_SHOW_DEST:
            Cat(player) = param;
            AffichDest(player, creature);
            break;

          // Previous categories page
          case GOSSIP_PREV_PAGEC:
            --PageC(player);
            AffichCat(player, creature);
            break;

          // Next page categories
          case GOSSIP_NEXT_PAGEC:
            ++PageC(player);
            AffichCat(player, creature);
            break;

          // Previous destinations page
          case GOSSIP_PREV_PAGED:
            --PageD(player);
            AffichDest(player, creature);
            break;

          // Next destination page
          case GOSSIP_NEXT_PAGED:
            ++PageD(player);
            AffichDest(player, creature);
            break;

          // Display main menu
          case GOSSIP_MAIN_MENU:
            OnGossipHello(player, creature);
            break;

          // Teleportation
          case GOSSIP_TELEPORT:
            CloseGossipMenuFor(player);
            /* what is this code? removing sickness? meh..
            if(player->HasAura(15007,0))
            {
                creature->CastSpell(player,38588,false); // Healing effect
                player->RemoveAurasDueToSpell(15007);
            }*/

            ActionTeleport(player, creature, param);
            break;
        }
        return true;
    };
};

void AddSC_guildhouse_npcs()
{
	new npc_guild_master();
	new guild_guard();
	new npc_buffnpc();
	new npc_portal();
	new npc_teleport();
}
