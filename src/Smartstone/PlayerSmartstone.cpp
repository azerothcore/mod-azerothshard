#include "AzthPlayer.h"
#include "Player.h"
#include "Opcodes.h"
#include "AzthSharedDefines.h"
#include "AZTH.h"

std::vector<SmartStonePlayerCommand>& AzthPlayer::getSmartStoneCommands()
{
    return smartStoneCommands;
}

void AzthPlayer::addSmartStoneCommand(SmartStonePlayerCommand command, bool query)
{

    uint64 sec = 0;

    if (sSmartStone->getCommandById(command.id).duration > 0)
    {
        sec = time(NULL) + (sSmartStone->getCommandById(command.id).duration * 60);
    }

    command.duration = sec;

    smartStoneCommands.push_back(command);

    if (query)
    {
        CharacterDatabase.Execute(
                "REPLACE INTO `character_smartstone_commands` (playerGuid, command, "
                "dateExpired, charges) VALUES ({}, {}, {}, {});",
                player->GetGUID().GetCounter(), command.id, sec, command.charges);
    }
}

// called only at player login
void AzthPlayer::addSmartStoneCommand(uint32 id, bool /*query*/, uint64 dateExpired, int32 charges)
{
    if (static_cast<uint32>(time(NULL)) >= dateExpired)
        return;

    SmartStonePlayerCommand command;

    command.id = id;
    command.charges = charges;
    command.duration = dateExpired;

    smartStoneCommands.push_back(command);
}

bool AzthPlayer::hasSmartStoneCommand(uint32 id)
{
    std::vector<SmartStonePlayerCommand> & playerCommands = getSmartStoneCommands();
    int n = playerCommands.size();

    for (int i = 0; i < n; i++)
    {
        if (playerCommands[i].id == id)
            return true;
    }

    return false;
}

void AzthPlayer::removeSmartStoneCommand(SmartStonePlayerCommand command,
        bool query) {
    // we need to specify the equal operator for struct to be able to run it:
    smartStoneCommands.erase(std::remove(smartStoneCommands.begin(),
            smartStoneCommands.end(), command),
            smartStoneCommands.end());

    if (query) {
        CharacterDatabase.Execute("DELETE FROM `character_smartstone_commands` "
                "WHERE playerGuid = %u AND command = %u;",
                player->GetGUID().GetCounter(), command.id);
    }
}

void AzthPlayer::decreaseSmartStoneCommandCharges(uint32 id) {
    for (std::vector<int>::size_type i = 0; i < smartStoneCommands.size(); i++) {
        if (smartStoneCommands[i].id == id) {
            smartStoneCommands[i].charges = smartStoneCommands[i].charges - 1;
            if (smartStoneCommands[i].charges == 0 ||
                    smartStoneCommands[i].charges < -1)
                removeSmartStoneCommand(smartStoneCommands[i], true);
            else
                CharacterDatabase.Execute(
                    "UPDATE character_smartstone_commands SET charges = %i WHERE "
                    "playerGuid = %u AND command = %u ;",
                    smartStoneCommands[i].charges, player->GetGUID().GetCounter(), id);

            return;
        }
    }
}

bool AzthPlayer::BuySmartStoneCommand(ObjectGuid vendorguid, uint32 vendorslot,
        uint32 item, uint8 count, uint8 bag,
        uint8 slot) {

    // cheating attempt
    if (count < 1)
        count = 1;

    // cheating attempt
    if (slot > MAX_BAG_SIZE && slot != NULL_SLOT)
        return false;

    if (!player->IsAlive())
        return false;

    ItemTemplate const *pProto = sObjectMgr->GetItemTemplate(item);
    if (!pProto) {
        player->SendBuyError(BUY_ERR_CANT_FIND_ITEM, NULL, item, 0);
        return false;
    }

    if (!(pProto->AllowableClass & player->getClassMask()) &&
            pProto->Bonding == BIND_WHEN_PICKED_UP && !player->IsGameMaster()) {
        player->SendBuyError(BUY_ERR_CANT_FIND_ITEM, NULL, item, 0);
        return false;
    }

    if (!player->IsGameMaster() &&
            ((pProto->Flags2 & ITEM_FLAGS_EXTRA_HORDE_ONLY &&
            player->GetTeamId(true) == TEAM_ALLIANCE) ||
            (pProto->Flags2 == ITEM_FLAGS_EXTRA_ALLIANCE_ONLY &&
            player->GetTeamId(true) == TEAM_HORDE)))
        return false;

    Creature* creature = player->GetNPCIfCanInteractWith(vendorguid, UNIT_NPC_FLAG_VENDOR);
    if (!creature) {
        //sLog->outDebug(LOG_FILTER_NETWORKIO, "WORLD: BuyItemFromVendor - Unit   (GUID: %u) not found or you can't interact with him.",   uint32(GUID_LOPART(vendorguid)));
        player->SendBuyError(BUY_ERR_DISTANCE_TOO_FAR, NULL, item, 0);
        return false;
    }

    /* ConditionList conditions =
     sConditionMgr->GetConditionsForNpcVendorEvent(creature->GetEntry(), item);
     if (!sConditionMgr->IsObjectMeetToConditions(this, creature, conditions))
     {
         //TC_LOG_DEBUG("condition", "BuyItemFromVendor: conditions not met for
     creature entry %u item %u", creature->GetEntry(), item);
         SendBuyError(BUY_ERR_CANT_FIND_ITEM, creature, item, 0);
         return false;
     }*/

    VendorItemData const *vItems = sObjectMgr->GetNpcVendorItemList(sSmartStone->SMARTSTONE_VENDOR_ENTRY);
    if (!vItems || vItems->Empty()) {
        player->SendBuyError(BUY_ERR_CANT_FIND_ITEM, NULL, item, 0);
        return false;
    }

    if (vendorslot >= vItems->GetItemCount()) {
        player->SendBuyError(BUY_ERR_CANT_FIND_ITEM, NULL, item, 0);
        return false;
    }

    VendorItem const *crItem = vItems->GetItem(vendorslot);
    // store diff item (cheating)
    if (!crItem || crItem->item != item) {
        player->SendBuyError(BUY_ERR_CANT_FIND_ITEM, NULL, item, 0);
        return false;
    }

    std::vector<SmartStonePlayerCommand> & playerCommands = getSmartStoneCommands();
    int n = playerCommands.size();
    SmartStoneCommand command = sSmartStone->getCommandByItem(item);

    for (int i = 0; i < n; i++) {

        // sLog->outError("Smartstone: isnullcommand: %u, command: %u,
        // playercommand: %u", sSmartStone->isNullCommand(command), command.id,
        // playerCommands[i].id);

        if (!sSmartStone->isNullCommand(command) &&
                command.id == playerCommands[i].id) {
            player->SendBuyError(BUY_ERR_ITEM_ALREADY_SOLD, NULL, item, 0);
            return false;
        }
    }

    // check current item amount if it limited
    /*
    if (crItem->maxcount != 0)
    {
        if (creature->GetVendorItemCurrentCount(crItem) < pProto->BuyCount *
    count)
        {
            SendBuyError(BUY_ERR_ITEM_ALREADY_SOLD, creature, item, 0);
            return false;
        }
    }*/

    if (pProto->RequiredReputationFaction &&
            (uint32(player->GetReputationRank(pProto->RequiredReputationFaction)) <
            pProto->RequiredReputationRank)) {
        player->SendBuyError(BUY_ERR_REPUTATION_REQUIRE, NULL, item, 0);
        return false;
    }

    if (crItem->ExtendedCost) {
        ItemExtendedCostEntry const *iece =
                sItemExtendedCostStore.LookupEntry(crItem->ExtendedCost);
        if (!iece) {
            LOG_INFO("Module", "Item %u have wrong ExtendedCost field value %u",
                    pProto->ItemId, crItem->ExtendedCost);
            return false;
        }

        // honor points price
        if (player->GetHonorPoints() < (iece->reqhonorpoints * count)) {
            player->SendEquipError(EQUIP_ERR_NOT_ENOUGH_HONOR_POINTS, NULL, NULL);
            return false;
        }

        // arena points price
        if (player->GetArenaPoints() < (iece->reqarenapoints * count)) {
            player->SendEquipError(EQUIP_ERR_NOT_ENOUGH_ARENA_POINTS, NULL, NULL);
            return false;
        }

        // item base price
        for (uint8 i = 0; i < MAX_ITEM_EXTENDED_COST_REQUIREMENTS; ++i) {
            if (iece->reqitem[i] &&
                    !player->HasItemCount(iece->reqitem[i],
                    (iece->reqitemcount[i] * count))) {
                player->SendEquipError(EQUIP_ERR_VENDOR_MISSING_TURNINS, NULL, NULL);
                return false;
            }
        }

        // check for personal arena rating requirement
        if (player->GetMaxPersonalArenaRatingRequirement(iece->reqarenaslot) <
                iece->reqpersonalarenarating) {
            // probably not the proper equip err
            player->SendEquipError(EQUIP_ERR_CANT_EQUIP_RANK, NULL, NULL);
            return false;
        }
    }

    uint32 price = 0;
    if (crItem->IsGoldRequired(pProto) &&
            pProto->BuyPrice >
            0) // Assume price cannot be negative (do not know why it is int32)
    {
        uint32 maxCount = MAX_MONEY_AMOUNT / pProto->BuyPrice;
        if ((uint32) count > maxCount) {
           LOG_INFO("Player %s tried to buy %u item id %u, causing overflow",
                    player->GetName().c_str(), (uint32) count, pProto->ItemId);
            count = (uint8) maxCount;
        }
        price = pProto->BuyPrice * count; // it should not exceed MAX_MONEY_AMOUNT

        // reputation discount
        /*
        price = uint32(floor(price * player->GetReputationPriceDiscount(creature)));
         */
        if (!player->HasEnoughMoney(price)) {
            player->SendBuyError(BUY_ERR_NOT_ENOUGHT_MONEY, NULL, item, 0);
            return false;
        }
    }

    player->ModifyMoney(-int32(price));

    if (crItem->ExtendedCost) // case for new honor system
    {
        ItemExtendedCostEntry const *iece =
                sItemExtendedCostStore.LookupEntry(crItem->ExtendedCost);
        if (iece->reqhonorpoints)
            player->ModifyHonorPoints(-int32(iece->reqhonorpoints * count));

        if (iece->reqarenapoints)
            player->ModifyArenaPoints(-int32(iece->reqarenapoints * count));

        for (uint8 i = 0; i < MAX_ITEM_EXTENDED_COST_REQUIREMENTS; ++i) {
            if (iece->reqitem[i])
                player->DestroyItemCount(iece->reqitem[i],
                    (iece->reqitemcount[i] * count), true);
        }
    }

    WorldPacket data(SMSG_BUY_ITEM, (8 + 4 + 4 + 4));
    data << uint64(1);
    data << uint32(vendorslot + 1); // numbered from 1 at client
    data << int32(0);
    data << uint32(1);
    player->GetSession()->SendPacket(&data);

    /*
    if ((bag == NULL_BAG && slot == NULL_SLOT) || player->IsInventoryPos(bag,
    slot))
    {
        if (!player->_StoreOrEquipNewItem(vendorslot, item, count, bag, slot,
    price, pProto, creature, crItem, true))
            return false;
    }
    else if (player->IsEquipmentPos(bag, slot))
    {
        if (pProto->BuyCount * count != 1)
        {
            player->SendEquipError(EQUIP_ERR_ITEM_CANT_BE_EQUIPPED, NULL, NULL);
            return false;
        }
        if (!player->_StoreOrEquipNewItem(vendorslot, item, count, bag, slot,
    price, pProto, creature, crItem, false))
            return false;
    }
    else
    {
        player->SendEquipError(EQUIP_ERR_ITEM_DOESNT_GO_TO_SLOT, NULL, NULL);
        return false;
    }*/

    command = sSmartStone->getCommandByItem(item);

    if (!sSmartStone->isNullCommand(command)) {
        sAZTH->GetAZTHPlayer(player)->addSmartStoneCommand(
                sSmartStone->toPlayerCommand(command), true);
        ChatHandler(player->GetSession())
                .SendSysMessage(sAzthLang->get(AZTH_LANG_SS_NEWAPP, player));

        sScriptMgr->OnBeforeStoreOrEquipNewItem(player, vendorslot, item, count, bag, slot, pProto, creature, crItem, false);
    }

    // return crItem->maxcount != 0;
    return true;
}

WorldLocation & AzthPlayer::getLastPositionInfo(uint32 dimension) {
    return lastPositionInfo[dimension];
};

void AzthPlayer::setLastPositionInfo(uint32 dimension, WorldLocation posInfo) {
    lastPositionInfo[dimension] = posInfo;
};

/**
 * We should not return a reference (&) in this case since
 * the returned value is local. Then must be copied
 *
 */
std::map<uint32,WorldLocation> AzthPlayer::getLastPositionInfoFromDB() {
    std::map<uint32,WorldLocation> lastPos;

    QueryResult savedPosResult = CharacterDatabase.Query(
            "SELECT type, mapId, posX, posY, posZ FROM character_saved_position WHERE charGuid = {};", player->GetGUID().GetCounter());

    if (!savedPosResult)
        return lastPos;

    do
    {
        Field* posFields = savedPosResult->Fetch();

        uint32 type = posFields[0].Get<uint32>();
        lastPos[type] = WorldLocation(posFields[1].Get<float>(),posFields[2].Get<float>(),posFields[3].Get<float>(), posFields[4].Get<float>(), player->GetOrientation());

        sAZTH->GetAZTHPlayer(player)->setLastPositionInfo(type, lastPos[type]);
    } while (savedPosResult->NextRow());

    return lastPos;
};

void AzthPlayer::saveLastPositionInfoToDB(Player *pl)
{
    CharacterDatabaseTransaction trans = CharacterDatabase.BeginTransaction();

    std::map<uint32,WorldLocation>::iterator it;
    for ( it = lastPositionInfo.begin(); it != lastPositionInfo.end(); it++ )
    {
        WorldLocation _loc= it->second;
        trans->Append("REPLACE INTO character_saved_position(charGuid,type,posX,posY,posZ,mapId) VALUES ({}, {}, {}, {}, {}, {});", pl->GetGUID().GetCounter(), it->first, _loc.GetPositionX(), _loc.GetPositionY(), _loc.GetPositionZ(), _loc.GetMapId());
    }

    CharacterDatabase.CommitTransaction(trans);
};

