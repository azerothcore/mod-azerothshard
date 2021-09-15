#include "AzthSmartStone.h"
#include "Common.h"
#include "Define.h"
#include "GossipDef.h"
#include "Item.h"
#include "Log.h"
#include "ObjectMgr.h"
#include "Opcodes.h"
#include "Player.h"
#include "ScriptMgr.h"
#include "ScriptedCreature.h"
#include "ScriptedGossip.h"
#include "Spell.h"
#include "WorldPacket.h"
#include "WorldSession.h"
#include "MapMgr.h"
#include "Map.h"
#include "Group.h"
#include "AZTH.h"
#include "Apps.h"

class Group;

enum SmartStoneCommands {
    SMRTST_BLACK_MARKET=1,
    SMRTST_CHANGE_FACTION=2,
    SMRTST_RENAME=3,
    SMRTST_CHAR_MENU=4,
    SMRTST_CHANGE_RACE=5,
    SMRTST_JUKEBOX=6,
    SMRTST_HERBALISM_BONUS=7,
    SMRTST_MINING_BONUS=8,
    SMRTST_BONUS_MENU=9,
    SMRTST_MAX_SKILL=10,
    SMRTST_XP_CHANGE=11,
    SMRTST_RESET_AURAS=12,
    SMRTST_TELEPORT_DALARAN=13,
    SMRTST_TELEPORT_HOUSE=14,
    SMRTST_SHOP_MENU=2000, //unused
    SMRTST_BACK_MENU=2001,
    SMRTST_README=60402,
    SMRTST_README_CHILD=60400,
};

std::string SmartStoneCommand::getText(Player *pl)
{
    AzthCustomLangs loc = AZTH_LOC_IT;

    if (pl)
        loc = sAZTH->GetAZTHPlayer(pl)->getCustLang();

    switch(loc)
    {
        case AZTH_LOC_IT:
            return text_it;
        break;
        case AZTH_LOC_EN:
            return text_def;
        break;
    }

    return text_def;
}

class azth_smart_stone : public ItemScript
{
public:

    uint32 parent = 1;
    SmartStoneApps *apps;

    azth_smart_stone() : ItemScript("azth_smart_stone") {
        apps = new SmartStoneApps();
    }

    Player* getHomeOwner(Player *player) {
        Player *owner=player;
        Player* leader=nullptr;
        if (player->GetGroup())
            leader = ObjectAccessor::FindPlayer(player->GetGroup()->GetLeaderGUID());

        if (leader)
            owner = leader;

        return owner;
    }


    void OnGossipSelect(Player *player, Item *item, uint32  /*sender*/,
            uint32 action) override {

        player->PlayerTalkClass->ClearMenus();

        // hack for readme gossip menu
        if (action>=SMRTST_README_CHILD || player->PlayerTalkClass->GetGossipMenu().GetMenuId() >=SMRTST_README_CHILD) {
            uint32 menuId= action; //action >= SMRTST_README_CHILD ? action : player->PlayerTalkClass->GetGossipMenu().GetItemData(action)->GossipActionMenuId;
            player->PlayerTalkClass->GetGossipMenu().SetMenuId(menuId);

            GossipMenuItemsMapBounds menuItemBounds = sObjectMgr->GetGossipMenuItemsMapBounds(menuId);
            uint32 textId = DEFAULT_GOSSIP_MESSAGE;

            GossipMenusMapBounds menuBounds = sObjectMgr->GetGossipMenusMapBounds(menuId);

            for (GossipMenusContainer::const_iterator itr = menuBounds.first; itr != menuBounds.second; ++itr)
                textId = itr->second.TextID;

            for (GossipMenuItemsContainer::const_iterator itr = menuItemBounds.first; itr != menuItemBounds.second; ++itr)
            {
                std::string strOptionText = itr->second.OptionText;
                std::string strBoxText = itr->second.BoxText;

                int32 locale = player->GetSession()->GetSessionDbLocaleIndex();
                if (locale >= 0)
                {
                    uint32 idxEntry = MAKE_PAIR32(menuId, itr->second.OptionID);
                    if (GossipMenuItemsLocale const* no = sObjectMgr->GetGossipMenuItemsLocale(idxEntry))
                    {
                        ObjectMgr::GetLocaleString(no->OptionText, locale, strOptionText);
                        ObjectMgr::GetLocaleString(no->BoxText, locale, strBoxText);
                    }
                }

                player->PlayerTalkClass->GetGossipMenu().AddMenuItem(itr->second.OptionID, itr->second.OptionIcon, strOptionText, 0, itr->second.ActionMenuID, strBoxText, itr->second.BoxMoney, itr->second.BoxCoded);
                player->PlayerTalkClass->GetGossipMenu().AddGossipMenuItemData(itr->second.OptionID, itr->second.ActionMenuID, itr->second.ActionPoiID);
            }

            SendGossipMenuFor(player, textId, item->GetGUID());
            return;
        }

        // back to main menu command
        if (action == 2001) {
            parent = 1;
            CloseGossipMenuFor(player);
            OnUse(player, item, SpellCastTargets());
            return;
        }

        SmartStoneCommand selectedCommand = sSmartStone->getCommandById(action);

        // scripted action
        if (selectedCommand.type == DO_SCRIPTED_ACTION ||
                action >= 2000) // azeroth store
        {
            switch (action) {
                case SMRTST_SHOP_MENU: // store
                    //sSmartStone->SmartStoneSendListInventory(player->GetSession());
                break;
                case SMRTST_BLACK_MARKET: // black market teleport
                    apps->blackMarketTeleport(player);
                break;

                case SMRTST_CHANGE_FACTION: // change faction
                    apps->changeFaction(player);
                break;

                case SMRTST_RENAME: // rename
                    apps->rename(player);
                break;

                case SMRTST_CHANGE_RACE: // change race
                    apps->changeRace(player);
                break;

                case SMRTST_JUKEBOX: // jukebox
                    apps->jukebox(player);
                break;

                case SMRTST_MAX_SKILL: // maxskill
                    apps->maxSkill(player);
                break;

                case SMRTST_RESET_AURAS:
                    apps->resetAuras(player);
                break;

                case SMRTST_TELEPORT_DALARAN:
                    apps->teleportDalaran(player);
                break;

                case SMRTST_TELEPORT_HOUSE:
                    apps->teleportHouse(getHomeOwner(player), player);
                break;

                case 99999:
                    break;
                default:
                    sLog->outError("Smartstone: unhandled command! ID: %u, player GUID: %lu",
                            action, player->GetGUID().GetRawValue());
                    break;
            }
            if (selectedCommand.charges > 0) {
                sAZTH->GetAZTHPlayer(player)->decreaseSmartStoneCommandCharges(
                        selectedCommand.id);
            }
            CloseGossipMenuFor(player);
            // return;
        }

        // open child
        if (selectedCommand.type == OPEN_CHILD) {
            parent = selectedCommand.action;
            CloseGossipMenuFor(player);
            OnUse(player, item, SpellCastTargets());
        }
    }

    void OnGossipSelectCode(Player* player, Item*  /*item*/, uint32  /*sender*/, uint32 action, const char* code) override {
        player->PlayerTalkClass->ClearMenus();

        SmartStoneCommand selectedCommand = sSmartStone->getCommandById(action);

        // scripted action
        if (selectedCommand.type == DO_SCRIPTED_ACTION_WITH_CODE || action == 2000) // azeroth store
        {
            switch (action) {
                case SMRTST_XP_CHANGE: //change exp
                    apps->changeExp(player, code);
                break;

                case 99999:
                    break;
                default:
                    sLog->outError("Smartstone: unhandled command with code! ID: %u, player GUID: %lu", action, player->GetGUID().GetRawValue());
                    break;
            }
            if (selectedCommand.charges > 0) {
                sAZTH->GetAZTHPlayer(player)->decreaseSmartStoneCommandCharges(selectedCommand.id);
            }
            CloseGossipMenuFor(player);
        }
    }

    bool OnUse(Player *player, Item *item, SpellCastTargets const & /*targets*/) override
    {
        player->PlayerTalkClass->ClearMenus();

        if (parent == 1) // not-to-buy commands for the main menu
        {
            // black market teleport id 1
            SmartStoneCommand teleport = sSmartStone->getCommandById(SMRTST_BLACK_MARKET);

            if (sConfigMgr->GetBoolDefault("Azth.Smartstone.Teleport.Enable", false))
            {
                if (!sAZTH->GetAZTHPlayer(player)->isInBlackMarket())
                {
                    /*if (sAZTH->GetAZTHPlayer(player)->isPvP())
                        AddGossipItemFor(player,teleport.icon, sAzthLang->get(AZTH_LANG_SS_TELEPORT_BACK, player), GOSSIP_SENDER_MAIN, teleport.id);*/

                    AddGossipItemFor(player,teleport.icon, teleport.getText(player), GOSSIP_SENDER_MAIN, teleport.id);
                }
                else
                    AddGossipItemFor(player,teleport.icon, sAzthLang->get(AZTH_LANG_SS_TELEPORT_BACK, player), GOSSIP_SENDER_MAIN, teleport.id);
            } else {
                AddGossipItemFor(player,teleport.icon, sAzthLang->get(AZTH_LANG_SS_TELEPORT_DISABLED, player), GOSSIP_SENDER_MAIN, 0);
            }

            if (!sAZTH->GetAZTHPlayer(player)->isPvP())
                player->PlayerTalkClass->GetGossipMenu().AddMenuItem(SMRTST_README, 0, GOSSIP_SENDER_MAIN, SMRTST_README_CHILD);

            // menu character (rename, change faction, etc) id 4
            SmartStoneCommand characterMenu = sSmartStone->getCommandById(SMRTST_CHAR_MENU);
            AddGossipItemFor(player,characterMenu.icon, characterMenu.getText(player), GOSSIP_SENDER_MAIN, characterMenu.id);

            // menu passive bonus id 9
            SmartStoneCommand passiveMenu = sSmartStone->getCommandById(SMRTST_BONUS_MENU);
            AddGossipItemFor(player,passiveMenu.icon, passiveMenu.getText(player), GOSSIP_SENDER_MAIN, passiveMenu.id);
        }

        if (parent == 2) // not-to-buy commands for the characters menu
        {
            // max skill command
            SmartStoneCommand maxSkill = sSmartStone->getCommandById(SMRTST_MAX_SKILL);
            AddGossipItemFor(player,maxSkill.icon, maxSkill.getText(player), GOSSIP_SENDER_MAIN, maxSkill.id);

            // azth xp command
            SmartStoneCommand azthXp = sSmartStone->getCommandById(SMRTST_XP_CHANGE);
            AddGossipItemFor(player,azthXp.icon, azthXp.getText(player), GOSSIP_SENDER_MAIN, azthXp.id, sAzthLang->get(AZTH_LANG_SS_VALUE, player), 0, true);

            // reset auras
            SmartStoneCommand resetAuras = sSmartStone->getCommandById(SMRTST_RESET_AURAS);
            AddGossipItemFor(player,resetAuras.icon, resetAuras.getText(player), GOSSIP_SENDER_MAIN, resetAuras.id);

            if (sAZTH->GetAZTHPlayer(player)->isPvP()) {
                // dalaran teleport
                SmartStoneCommand dalaranTeleport = sSmartStone->getCommandById(SMRTST_TELEPORT_DALARAN);
                AddGossipItemFor(player,dalaranTeleport.icon, dalaranTeleport.getText(player), GOSSIP_SENDER_MAIN, dalaranTeleport.id);
            }

            if (sAZTH->GetAZTHPlayer(player)->getCurrentDimensionByAura() == DIMENSION_RPG) {
                Player *owner=getHomeOwner(player);

                if (MapMgr::IsValidMapCoord(sAZTH->GetAZTHPlayer(owner)->getLastPositionInfo(AZTH_SMRTST_POSITION_HOUSE_INDEX))) {
                    // home teleport for RPG world
                    SmartStoneCommand homeTeleport = sSmartStone->getCommandById(SMRTST_TELEPORT_HOUSE);
                    std::string str=homeTeleport.getText(player) + " (" +owner->GetName()+")";

                    AddGossipItemFor(player,homeTeleport.icon, str.c_str() , GOSSIP_SENDER_MAIN, homeTeleport.id);
                }
            }
        }

        std::vector<SmartStonePlayerCommand> & playerCommands =
                sAZTH->GetAZTHPlayer(player)->getSmartStoneCommands();
        int n = playerCommands.size();

        for (int i = 0; i < n; i++) {
            SmartStoneCommand command =
                    sSmartStone->getCommandById(playerCommands[i].id);

            // if expired or no charges
            if ((playerCommands[i].duration <= static_cast<uint32>(time(NULL)) &&
                    playerCommands[i].duration != 0) ||
                    playerCommands[i].charges == 0) {
                sAZTH->GetAZTHPlayer(player)->removeSmartStoneCommand(playerCommands[i], true);
                continue;
            }

            std::string text = command.getText(player);

            if (playerCommands[i].charges != -1)
                text = text + " (" + std::to_string(playerCommands[i].charges) + ")";

            if (playerCommands[i].duration != 0) {
                uint64 timeDiff = playerCommands[i].duration - time(NULL);
                uint64 seconds = timeDiff % 60;
                uint64 minutes = floor(timeDiff / 60);
                uint64 hours = floor(timeDiff / 3600);
                uint64 days = floor(timeDiff / 3600 / 24);
                if (days >= 1) {
                    text = text + " (" + std::to_string(days) + " days)";
                } else {
                    text = text + " (" + std::to_string(hours) + ":" +
                            std::to_string(minutes) + ":" + std::to_string(seconds) + ")";
                }
            }

            if (command.id != 0 && command.parent_menu == parent) {
                if (command.type != DO_SCRIPTED_ACTION_WITH_CODE) {
                    AddGossipItemFor(player,command.icon, text, GOSSIP_SENDER_MAIN, command.id);
                } else {
                    AddGossipItemFor(player,command.icon, text, GOSSIP_SENDER_MAIN, command.id, sAzthLang->get(AZTH_LANG_SS_VALUE, player), 0, true);
                }
            }
        }

        // acquista app

        /*if (parent == 1)
          AddGossipItemFor(player,
              0, "|TInterface/ICONS/INV_Misc_Coin_03:30|t Azeroth Store",
              GOSSIP_SENDER_MAIN, 2000);*/

        if (parent != 1) {
            // back to main menu command
            AddGossipItemFor(player,0, sAzthLang->get(AZTH_LANG_SS_BACK, player), GOSSIP_SENDER_MAIN, 2001);
        }

        SendGossipMenuFor(player,DEFAULT_GOSSIP_MESSAGE, item->GetGUID());

        parent = 1;
        return false;
    }
};

void SmartStone::loadCommands() {
    // initialize count and array
    uint32 count = 0;
    // sHearthstoneMode->hsAchievementTable.clear();

    // TODO: re-enable
    // QueryResult ssCommandsResult = ExtraDatabase.PQuery(
    //         "SELECT id, text_def, text_it, item, icon, parent_menu, type, action, charges, "
    //         "duration FROM smartstone_commands");

    // if (ssCommandsResult) {
    //     do {
    //         SmartStoneCommand command = {};
    //         command.id = (*ssCommandsResult)[0].GetUInt32();
    //         command.text_def = (*ssCommandsResult)[1].GetString();
    //         command.text_it = (*ssCommandsResult)[2].GetString();
    //         command.item = (*ssCommandsResult)[3].GetUInt32();
    //         command.icon = (*ssCommandsResult)[4].GetUInt32();
    //         command.parent_menu = (*ssCommandsResult)[5].GetUInt32();
    //         command.type = (*ssCommandsResult)[6].GetUInt32();
    //         command.action = (*ssCommandsResult)[7].GetUInt32();
    //         command.charges = (*ssCommandsResult)[8].GetInt32();
    //         command.duration = (*ssCommandsResult)[9].GetUInt64();

    //         ssCommands2.push_back(command);

    //         count++;

    //     } while (ssCommandsResult->NextRow());
    // }

    sLog->outString("Smartstone: loaded %u commands", count);
}

SmartStoneCommand SmartStone::getCommandById(uint32 id) {
    std::vector<SmartStoneCommand> temp(ssCommands2);
    int n = temp.size();
    for (int i = 0; i < n; i++) {
        if (temp[i].id == id)
            return temp[i];
    }
    return nullCommand;
};

SmartStoneCommand SmartStone::getCommandByItem(uint32 item) {
    std::vector<SmartStoneCommand> temp(ssCommands2);
    int n = temp.size();
    for (int i = 0; i < n; i++) {
        if (temp[i].item == item)
            return temp[i];
    }
    return nullCommand;
};

bool SmartStone::isNullCommand(SmartStoneCommand command) {
    return (command.id == 0 && command.text_def == "" && command.item == 0 &&
            command.icon == 0 && command.parent_menu == 0 &&
            command.type == 0 && command.action == 0);
};

SmartStonePlayerCommand SmartStone::toPlayerCommand(SmartStoneCommand command) {
    SmartStonePlayerCommand result;
    result.id = command.id;
    result.charges = command.charges;
    // result.duration = command.duration * 60 + time(NULL);
    result.duration = 0;
    return result;
};

class azth_smartstone_world : public WorldScript {
public:

    azth_smartstone_world() : WorldScript("azth_smartstone_world") {
    }

    void OnAfterConfigLoad(bool  /*reload*/) override {
        sSmartStone->loadCommands();
    }
};

class azth_smartstone_player_commands : public PlayerScript {
public:

    azth_smartstone_player_commands()
    : PlayerScript("azth_smartstone_player_commands") {
    }

    void OnLogin(Player *player) override {
        QueryResult ssCommandsResult = CharacterDatabase.PQuery(
                "SELECT command, dateExpired, charges FROM "
                "character_smartstone_commands WHERE playerGuid = %d ;",
                player->GetGUID().GetCounter());

        if (ssCommandsResult) {
            do {
                uint32 id = (*ssCommandsResult)[0].GetUInt32();
                uint64 date = (*ssCommandsResult)[1].GetUInt64();
                int32 charges = (*ssCommandsResult)[2].GetInt32();
                sAZTH->GetAZTHPlayer(player)->addSmartStoneCommand(id, false, date, charges);
            } while (ssCommandsResult->NextRow());
        }

        sAZTH->GetAZTHPlayer(player)->getLastPositionInfoFromDB();
    }

    void OnLogout(Player* player) override {
        sAZTH->GetAZTHPlayer(player)->saveLastPositionInfoToDB(player);
    }

    void OnBeforeBuyItemFromVendor(Player* player, ObjectGuid vendorguid, uint32 vendorslot, uint32 &item, uint8 count, uint8  /*bag*/, uint8 /*slot*/) override {
        if (!sSmartStone->isNullCommand(sSmartStone->getCommandByItem(item))) {
            sAZTH->GetAZTHPlayer(player)->BuySmartStoneCommand(vendorguid, vendorslot, item, count, NULL_BAG, NULL_SLOT);
            item = 0;
        }
    }

};

class smartstone_vendor : public CreatureScript {
public:

    smartstone_vendor() : CreatureScript("smartstone_vendor") {
    }

    bool OnGossipHello(Player* player, Creature* creature) override {
        player->PlayerTalkClass->ClearMenus();

        AddGossipItemFor(player,0, "Hello, I would like to buy new apps!", GOSSIP_SENDER_MAIN, 1);
        SendGossipMenuFor(player,DEFAULT_GOSSIP_MESSAGE, creature->GetGUID());
        return true;
    }

    bool OnGossipSelect(Player* player, Creature* creature, uint32 /*sender*/, uint32 action) override {
        player->PlayerTalkClass->ClearMenus();

        if (action == 1)
            sSmartStone->SmartStoneSendListInventory(player->GetSession(), creature->GetGUID().GetRawValue());

        return true;
    }
};

void SmartStone::SmartStoneSendListInventory(WorldSession *session, uint64 vendorGuid) {
    VendorItemData const *items = sObjectMgr->GetNpcVendorItemList(SMARTSTONE_VENDOR_ENTRY);



    if (!items) {
        WorldPacket data(SMSG_LIST_INVENTORY, 8 + 1 + 1);
        data << vendorGuid;
        data << uint8(0); // count == 0, next will be error code
        data << uint8(0); // "Vendor has no inventory"
        session->SendPacket(&data);
        return;
    }

    uint8 itemCount = items->GetItemCount();
    uint8 count = 0;

    WorldPacket data(SMSG_LIST_INVENTORY, 8 + 1 + itemCount * 8 * 4);
    data << vendorGuid;

    size_t countPos = data.wpos();
    data << uint8(count);

    for (uint8 slot = 0; slot < itemCount; ++slot) {
        if (VendorItem const *item = items->GetItem(slot)) {
            if (ItemTemplate const *itemTemplate =
                    sObjectMgr->GetItemTemplate(item->item)) {
                if (!(itemTemplate->AllowableClass &
                        session->GetPlayer()->getClassMask()) &&
                        itemTemplate->Bonding == BIND_WHEN_PICKED_UP &&
                        !session->GetPlayer()->IsGameMaster())
                    continue;
                // Only display items in vendor lists for the team the
                // player is on. If GM on, display all items.
                if (!session->GetPlayer()->IsGameMaster() &&
                        ((itemTemplate->Flags2 & ITEM_FLAGS_EXTRA_HORDE_ONLY &&
                        session->GetPlayer()->GetTeamId() == TEAM_ALLIANCE) ||
                        (itemTemplate->Flags2 == ITEM_FLAGS_EXTRA_ALLIANCE_ONLY &&
                        session->GetPlayer()->GetTeamId() == TEAM_HORDE)))
                    continue;

                uint32 leftInStock = 0xFFFFFFFF;

                std::vector<SmartStonePlayerCommand> & playerCommands = sAZTH->GetAZTHPlayer(session->GetPlayer())->getSmartStoneCommands();
                int n = playerCommands.size();
                SmartStoneCommand command = sSmartStone->getCommandByItem(item->item);

                // we hide commands that the player already has
                for (int i = 0; i < n; i++) {
                    // sLog->outError("Smartstone: isnullcommand: %u, command: %u,
                    // playercommand: %u", isNullCommand(command), command.id,
                    // playerCommands[i]);

                    if (!isNullCommand(command) && command.id == playerCommands[i].id)
                        leftInStock = 0;
                }

                /* if (!session->GetPlayer()->IsGameMaster() && !leftInStock)
                     continue;*/

                /*ConditionList conditions =
                sConditionMgr->GetConditionsForNpcVendorEvent(SMARTSTONE_VENDOR_ENTRY,
                item->item);
                if (!sConditionMgr->IsObjectMeetToConditions(session->GetPlayer(),
                vendor, conditions))
                {
                    sLog->outError("SendListInventory: conditions not met for creature
                entry %u item %u", vendor->GetEntry(), item->item);
                    continue;
                }*/

                // reputation discount
                int32 price = item->IsGoldRequired(itemTemplate)
                        ? uint32(floor(itemTemplate->BuyPrice))
                        : 0;

                data << uint32(slot + 1); // client expects counting to start at 1
                data << uint32(item->item);
                data << uint32(itemTemplate->DisplayInfoID);
                data << int32(leftInStock);
                data << uint32(price);
                data << uint32(itemTemplate->MaxDurability);
                data << uint32(itemTemplate->BuyCount);
                data << uint32(item->ExtendedCost);

                if (++count >= MAX_VENDOR_ITEMS)
                    break;
            }
        }
    }

    if (count == 0) {
        data << uint8(0);
        session->SendPacket(&data);
        return;
    }

    data.put<uint8>(countPos, count);
    session->SendPacket(&data);
}

void AddSC_azth_smart_stone() // Add to scriptloader normally
{
    new azth_smart_stone();
    new azth_smartstone_world();
    new azth_smartstone_player_commands();
    new smartstone_vendor();
}
