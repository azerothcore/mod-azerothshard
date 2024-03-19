#include "AzthPlayer.h"
#include "Player.h"
#include "AzthLevelStat.h"
#include "AzthUtils.h"
#include "Pet.h"
#include "Opcodes.h"
#include "AZTH.h"

uint32 AzthPlayer::GetTimeWalkingLevel() const
{
    return timeWalkingLevel;
}

bool AzthPlayer::isTimeWalking(bool skipSpecial) const
{
    if (skipSpecial && timeWalkingLevel>0 && ( timeWalkingLevel <= sWorld->getIntConfig(CONFIG_MAX_PLAYER_LEVEL) || timeWalkingLevel == TIMEWALKING_LVL_AUTO )) {
        // it includes TIMEWALKING_LEVEL_AUTO
        return true;
    } else if (!skipSpecial && timeWalkingLevel>0) {
        return true;
    }

    return false;
}

uint32 AzthPlayer::getTwItemLevel(uint32 twLevel) {
    switch (twLevel) {
        case TIMEWALKING_LVL_NAXX:
        case TIMEWALKING_LVL_OBSIDIAN:
        case TIMEWALKING_LVL_THE_EYE:
            return 213;
        break;
        case TIMEWALKING_LVL_ULDUAR:
        case TIMEWALKING_LVL_TOGC:
            return 239;
        break;
    }

    return 0;
}

void AzthPlayer::SetTimeWalkingLevel(uint32 itsTimeWalkingLevel, bool clearAuras/*=true*/, bool save /*=true*/, bool login/*=false*/)
{
    std::string lvlName=sAzthUtils->getLevelInfo(itsTimeWalkingLevel);
    char* ret = nullptr;

    if (!login) {
        // hacking attempt?
        if (timeWalkingLevel==0 && !sAzthUtils->isMythicLevel(itsTimeWalkingLevel) && player->getLevel()<sWorld->getIntConfig(CONFIG_MAX_PLAYER_LEVEL))
            return;

        uint32 iLvl=sAZTH->GetAZTHPlayer(player)->getTwItemLevel(itsTimeWalkingLevel);
        if (iLvl && !sAZTH->GetAZTHPlayer(player)->checkItems(iLvl))
        {
            ChatHandler(player->GetSession()).SendSysMessage(sAzthLang->getf(AZTH_LANG_TW_LEVEL_MAX, player, ret, iLvl));
            return;
        }

        // we should skip invalid levels and use available specials (>300) if needed
        if (itsTimeWalkingLevel >= sWorld->getIntConfig(CONFIG_MAX_PLAYER_LEVEL) && lvlName.compare(std::to_string(itsTimeWalkingLevel)) == 0 /*equals*/)
            return;
    }

    uint32 realLevel = itsTimeWalkingLevel;
    uint32 statsLevel = itsTimeWalkingLevel;

    uint32 oldLevel = player->getLevel();

    if (itsTimeWalkingLevel>TIMEWALKING_SPECIAL_LVL_MIN && itsTimeWalkingLevel<=TIMEWALKING_SPECIAL_LVL_MAX) { // 300 + 255 levels

        if (itsTimeWalkingLevel == TIMEWALKING_LVL_AUTO) {
            WorldLocation pos = WorldLocation(player->GetMapId(), player->GetPositionX(), player->GetPositionY(), player->GetPositionZ(), player->GetOrientation());

            uint32 posLvl=sAzthUtils->getPositionLevel(false, player->GetMap(), pos);
            realLevel = statsLevel = posLvl ? posLvl : player->getLevel();

            switch(sAZTH->GetAZTHPlayer(player)->getCurrentDimensionByAura()) {
                case DIMENSION_60:
                    if (player->getLevel() > 60)
                        realLevel = 60;
                break;
                case DIMENSION_70:
                    if (player->getLevel() > 70)
                        realLevel = 70;
                break;
            }

        } else {
            realLevel =  player->getLevel();
        }
    }

    Player* player = this->player;

    //apply debuf/buff section (spell) and enable timewalking mode
    if (itsTimeWalkingLevel > 0)
    {
        if (player->HasAura(TIMEWALKING_AURA_VISIBLE))
            // we must remove before set again
            SetTimeWalkingLevel(0, false, false, login);

        AzthLevelStat const *stats = sAzthUtils->getTwStats(player, statsLevel);
        if (!stats)
            return;

        //set must be before givelevel allowing make the check inside givelevel to avoid level mail
        timeWalkingLevel = itsTimeWalkingLevel;

        if (oldLevel != realLevel)
        {
            player->GiveLevel(realLevel);

            player->SendActionButtons(1);

            if (player->GetPet() && player->GetPet()->getLevel() != realLevel) { // could happen
                player->GetPet()->GivePetLevel(realLevel);
            }
        }

        if (!sAzthUtils->isMythicLevel(timeWalkingLevel))
            player->SetFlag(PLAYER_FLAGS, PLAYER_FLAGS_NO_XP_GAIN);

        sAzthUtils->setTwAuras(player, stats, true, login);

        if (player->GetPet() && (!player->HasAura(TIMEWALKING_AURA_VISIBLE) || (player->GetPet()->HasAura(TIMEWALKING_AURA_VISIBLE) && player->GetPet()->GetAura(TIMEWALKING_AURA_VISIBLE)->GetStackAmount() != stats->GetLevel())))
            sAzthUtils->setTwAuras(player->GetPet(), stats, true, login);

        if (save)
            QueryResult timewalkingCharactersActive_table = CharacterDatabase.Query(("INSERT IGNORE INTO `azth_timewalking_characters_active` (`id`, `level`) VALUES ('{}', '{}');"), player->GetGUID().GetCounter(), sAZTH->GetAZTHPlayer(player)->GetTimeWalkingLevel());

        ChatHandler(player->GetSession()).SendSysMessage(sAzthLang->getf(AZTH_LANG_TW_MODE_ON, player, ret, lvlName.c_str()));
    }
    else
    {
        // used just to be able to loop on keys to remove auras
        AzthLevelStat const *stats = sAzthUtils->getTwStats(player, player->getLevel());
        if (!stats)
            return;

        if (save) {
            if (!sAzthUtils->isMythicLevel(timeWalkingLevel))
                player->GiveLevel(sWorld->getIntConfig(CONFIG_MAX_PLAYER_LEVEL));

            player->SendActionButtons(1);

            if (player->GetPet() && !sAzthUtils->isMythicLevel(timeWalkingLevel)) {
                player->GetPet()->GivePetLevel(sWorld->getIntConfig(CONFIG_MAX_PLAYER_LEVEL));
            }
        }

        if (!sAzthUtils->isMythicLevel(timeWalkingLevel))
            player->RemoveFlag(PLAYER_FLAGS, PLAYER_FLAGS_NO_XP_GAIN);

        sAzthUtils->setTwAuras(player, stats, false, login);
        if (player->GetPet()) {
            sAzthUtils->setTwAuras(player->GetPet(), stats, false, login);
        }

        // reset must be after givelevel allowing make the check inside givelevel to avoid level mail
        timeWalkingLevel = 0;

        if (save) {
            QueryResult timewalkingCharactersActive_table = CharacterDatabase.Query(("DELETE FROM `azth_timewalking_characters_active` WHERE `id`={}"), player->GetGUID().GetCounter());
            // we announce it only when not login, because TW could be removed temporary (on level switch or on login for example)
            ChatHandler(player->GetSession()).SendSysMessage(sAzthLang->get(AZTH_LANG_TW_MODE_OFF, player));
        }
    }

    prepareTwSpells(oldLevel);

    sAzthUtils->updateTwLevel(player, player->GetGroup());

    if (clearAuras) {
        sAzthUtils->removeTimewalkingAura(player);
        if (player->GetPet() != NULL) {
            sAzthUtils->removeTimewalkingAura(player->GetPet());
        }
    }

    // START: RESET EQUIP SPELLS
    player->UpdateEquipSpellsAtFormChange();

    // unapply and reapply equip spells
    for (uint8 i = 0; i < INVENTORY_SLOT_BAG_END; ++i)
    {
        Item* item = player->GetItemByPos(i);
        if (!item)
            continue;

        if (!item->IsBroken() && player->CanUseAttackType(player->GetAttackBySlot(i)))
        {
            player->ApplyItemEquipSpell(item, false);
            player->ApplyItemEquipSpell(item, true);
        }
    }

    // unapply and reapply set bonuses
    for (size_t setindex = 0; setindex < player->ItemSetEff.size(); ++setindex)
    {
        ItemSetEffect* eff = player->ItemSetEff[setindex];
        if (!eff)
            continue;

        for (uint32 y = 0; y < MAX_ITEM_SET_SPELLS; ++y)
        {
            SpellInfo const* spellInfo = eff->spells[y];
            if (!spellInfo)
                continue;

            player->ApplyEquipSpell(spellInfo, nullptr, false);

            // [AZTH] Timewalking
            bool found = false;

            for (uint8 i = 0; i < INVENTORY_SLOT_BAG_END; ++i)
            {
                Item* item = player->GetItemByPos(i);
                if (!item)
                    continue;

                ItemTemplate const* proto = item->GetTemplate();
                if (proto->ItemSet == eff->setid && proto && !sAZTH->GetAZTHPlayer(player)->itemCheckReqLevel(proto))
                {
                    break;
                }
            }

            if (found)
                break;

            //[/AZTH]

            player->ApplyEquipSpell(spellInfo, nullptr, true);
        }
    }
    // END: RESET EQUIP SPELLS

    /*if (save) {
        player->SaveToDB(false, false);
    }*/
}


void AzthPlayer::prepareTwSpells(uint32 oldLevel)
{
    if (!player || !player->IsInWorld()) // shouldn't happen
        return;

    bool isTw=isTimeWalking(true);

    // do nothing if we are not changing timewalking level
    if (!isTw && oldLevel==player->getLevel())
        return;

    std::map<uint32 /*old*/, uint32 /*new*/> spellMap;

    bool apply=isTw && player->getLevel() < sWorld->getIntConfig(CONFIG_MAX_PLAYER_LEVEL);

    for (auto itr : player->GetSpellMap())
    {
        SpellInfo const* spellInfo = sSpellMgr->GetSpellInfo(itr.first);

        if (!itr.second->IsInSpec(player->GetActiveSpec())) //!spellInfo->HasAttribute(SPELL_ATTR4_UNK21))
            continue;

        bool isValidSpell = false;

        // All stance spells. if any better way, change it.
        for (uint8 i = 0; i < MAX_SPELL_EFFECTS; ++i)
        {
            switch (spellInfo->SpellFamilyName)
            {
                case SPELLFAMILY_PALADIN:
                    // Paladin aura Spell
                    if (spellInfo->Effects[i].Effect == SPELL_EFFECT_APPLY_AREA_AURA_RAID)
                        isValidSpell=true;
                    break;
                case SPELLFAMILY_DRUID:
                    // Druid form Spell
                    if (spellInfo->Effects[i].Effect == SPELL_EFFECT_APPLY_AURA &&
                        spellInfo->Effects[i].ApplyAuraName == SPELL_AURA_MOD_SHAPESHIFT)
                        isValidSpell=true;
                    break;
            }
        }

        if (!isValidSpell)
            continue;

        if (!sAzthUtils->canScaleSpell(spellInfo))
            continue;

        uint32 lastKnownSpell = sSpellMgr->GetLastSpellInChain(itr.first);

        while (!player->HasSpell(lastKnownSpell))
            lastKnownSpell = sSpellMgr->GetPrevSpellInChain(lastKnownSpell);

        uint32 spell = sAzthUtils->selectCorrectSpellRank(apply ? player->getLevel() : oldLevel, lastKnownSpell);

        uint32 remove = apply ? lastKnownSpell : spell;
        uint32 learn  = apply ? spell : lastKnownSpell;

        if (remove == learn)
            continue;

        if (itr.first == remove)
        {
            // SUPERCED high ranks with low ranks
            itr.second->Active = false;
            spellMap[remove] = learn;
        }
    }

    for (std::map<uint32, uint32>::const_iterator itr = spellMap.begin(), end = spellMap.end(); itr != end; ++itr)
    {
        PlayerSpellMap::iterator s = player->GetSpellMap().find(itr->second);
        if (s == player->GetSpellMap().end() || player->HasActiveSpell(s->first) || player->HasActiveSpell(itr->first))
            continue;

        s->second->Active = true;

        // update ranks in action bar
        WorldPacket data(SMSG_SUPERCEDED_SPELL, 4 + 4);
        data << uint32(itr->first); // old
        data << uint32(itr->second); // new
        player->GetSession()->SendPacket(&data);

        player->SendLearnPacket(itr->first, false); // be sure that old spell is removed from spellbook
    }
}

bool AzthPlayer::canUseItem(Item * item, bool notify) {
    if (!item)
        return true; // we are not responsible, so continue

    ItemTemplate const* proto=item->GetTemplate();

    if (!proto)
        return true;

    if (!itemCheckReqLevel(proto, notify))
        return false;

    uint32 level = player->getLevel();

    // we must check also level because
    // this is called even when timewalking is set but level is not changed yet (when removing item bonuses)
    if (level < sWorld->getIntConfig(CONFIG_MAX_PLAYER_LEVEL) && sAZTH->GetAZTHPlayer(player)->isTimeWalking(true)) {
        if (proto->ItemLevel == AZTH_TW_ILVL_NORMAL_ONLY)
            return true;

        for (uint8 i = 0; i < MAX_ITEM_PROTO_SPELLS; ++i)
        {
            _Spell const& spellData = proto->Spells[i];
            if (spellData.SpellId) {
                SpellInfo const* spellInfo = sSpellMgr->GetSpellInfo(spellData.SpellId);
                if (spellInfo && sAzthUtils->isNotAllowedSpellForTw(spellInfo)) {
                    if (notify) {
                        player->GetSession()->SendNotification("This item is not allowed in Timewalking");
                        player->SendEquipError(EQUIP_ERR_NONE, item, NULL);
                    }
                    return false;
                }
            }
        }
    }

    return true;
}

bool AzthPlayer::itemCheckReqLevel(ItemTemplate const* proto, bool notify) {
    uint32 level = player->getLevel();

    if (proto->ItemLevel == AZTH_TW_ILVL_NORMAL_ONLY) {
        if (!sAZTH->GetAZTHPlayer(player)->isTimeWalking(true)) {
            if (notify) {
                player->GetSession()->SendNotification("This item can be used only with Timewalking level 1 to 79");
                player->SendEquipError(EQUIP_ERR_NONE, NULL, NULL);
            }

            return false;
        } else {
            return true; // in this case we know what we're doing with this items, so we can return true directly
        }
    }

    // we must check also level because
    // this is called even when timewalking is set but level is not changed yet (when removing item bonuses)
    if (level < sWorld->getIntConfig(CONFIG_MAX_PLAYER_LEVEL) && sAZTH->GetAZTHPlayer(player)->isTimeWalking(true)) {
        uint32 req=sAzthUtils->getCalcReqLevel(proto);
        if (req > level) {
            if (notify) {
                player->GetSession()->SendNotification("Level Required for this item: %u", req);
                player->SendEquipError(EQUIP_ERR_NONE, NULL, NULL);
            }

            return false;
        }
    }

    return true;
}
