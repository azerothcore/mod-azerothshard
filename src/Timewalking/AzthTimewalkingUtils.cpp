#include "AzthUtils.h"
#include "AzthLevelStat.h"
#include "Pet.h"
#include "Spell.h"
#include "AZTH.h"
#include "AzthGroupMgr.h"

class Spell;

std::string AzthUtils::getLevelInfo(uint32 level) {
    switch (level) {
        case TIMEWALKING_LVL_NAXX:
            return "Naxxramas Timewalking";
        case TIMEWALKING_LVL_OBSIDIAN:
            return "Obsidian Sanctum Timewalking";
        case TIMEWALKING_LVL_THE_EYE:
            return "The Eye of Eternity Timewalking";
        case TIMEWALKING_LVL_TOGC:
            return "Trial of the Grand Crusader";
        case TIMEWALKING_LVL_ULDUAR:
            return "Ulduar";
        case TIMEWALKING_LVL_AUTO:
            return "Auto scaling";
        case TIMEWALKING_SPECIAL_LVL_MIXED:
            return "Mixed level";
        default:
            if (sAzthUtils->isMythicLevel(level)) {
                return "Flex Mythic+ level "+std::to_string(level-TIMEWALKING_LVL_VAS_START+1);
            } else {
                return std::to_string(level);
            }
    }
}

// This function is used to decide which level must prevail
// on another when select the group level for example.
uint32 AzthUtils::maxTwLevel(uint32 sourceLvl, uint32 compareLevel) const {
    uint32 maxLevel=sWorld->getIntConfig(CONFIG_MAX_PLAYER_LEVEL);

    if (sourceLvl == compareLevel || compareLevel<=0)
        return sourceLvl;

    // normal level case (from 0 to 255)
    if (sourceLvl < TIMEWALKING_SPECIAL_LVL_MIN && compareLevel < TIMEWALKING_SPECIAL_LVL_MIN) {
        return sourceLvl > compareLevel ? sourceLvl : compareLevel;
    }
    //else  special level >= TIMEWALKING_SPECIAL_LVL_MIN || compareLevel >= TIMEWALKING_SPECIAL_LVL_MIN


    //1) TW Wotlk level case
    if (sourceLvl >= TIMEWALKING_SPECIAL_LVL_WOTLK_START && sourceLvl <= TIMEWALKING_SPECIAL_LVL_WOTLK_END) {
        // TW Wotlk vs TW Wotlk
        if (compareLevel  >= TIMEWALKING_SPECIAL_LVL_WOTLK_START && compareLevel <= TIMEWALKING_SPECIAL_LVL_WOTLK_END) {
            return sourceLvl > compareLevel ? sourceLvl : compareLevel;
        }

        // TW Wotlk vs lower than endlevels
        // a level lower than maxlevel is also
        // lower than special wotlk, so sourceLevel will prevail
        if (compareLevel < maxLevel) {
            return sourceLvl;
        }

        // TW Wotlk vs endlevels
        // if sourceLevel is a special level but
        // compare level is not and it's range is from 80 to TIMEWALKING_SPECIAL_LVL_MIN -1
        // then compareLevel must prevail because TW Wotlk levels
        // are something between maxlevel and maxlevel -1
        if (compareLevel >= maxLevel && compareLevel < TIMEWALKING_SPECIAL_LVL_MIN)
            return compareLevel;

        // TW Wotlk vs others
        // comparing another level with TW Wotlk
        // it must be considered a not compatible
        // case, so mixed level is used
        return TIMEWALKING_SPECIAL_LVL_MIXED;
    }

    //2) Mythic case
    if (sourceLvl >= TIMEWALKING_LVL_VAS_START && sourceLvl <= TIMEWALKING_LVL_VAS_END) {
        if (compareLevel  >= TIMEWALKING_LVL_VAS_START && compareLevel <= TIMEWALKING_LVL_VAS_END) {
            return sourceLvl > compareLevel ? sourceLvl : compareLevel;
        }

        // comparing another level with mythic
        // it must be considered a not compatible
        // case, so mixed level is used
        return TIMEWALKING_SPECIAL_LVL_MIXED;
    }

    //3) AUTO Scaling
    if (sourceLvl == TIMEWALKING_LVL_AUTO) {
        // already handled at beginning of this function
        // if (compareLevel == TIMEWALKING_LVL_AUTO)
        //    return TIMEWALKING_LVL_AUTO;

        // autoscaling is compatible only with itself
        // so if compareLevel is different, we will use mixed case
        return TIMEWALKING_SPECIAL_LVL_MIXED;
    }

    // special level not handled, must be always considered a mixed case
    // NOTE: it will avoid loops
    if (sourceLvl > TIMEWALKING_SPECIAL_LVL_MIN && compareLevel < TIMEWALKING_SPECIAL_LVL_MIN)
        return TIMEWALKING_SPECIAL_LVL_MIXED;

    return maxTwLevel(compareLevel, sourceLvl); // inverse case
}

bool AzthUtils::isEligibleForBonusByArea(Player const* player) {
    WorldLocation pos = WorldLocation(player->GetMapId(), player->GetPositionX(), player->GetPositionY(), player->GetPositionZ(), player->GetOrientation());
    uint32 posLvl=sAzthUtils->getPositionLevel(true, player->GetMap(), pos);

    if (posLvl>0)
    {
        uint32 level = sAZTH->GetAZTHPlayer((Player*)player)->getPStatsLevel();

        if (posLvl>TIMEWALKING_SPECIAL_LVL_MIN)
        {
            //if (sAZTH->GetAZTHPlayer(player)->GetTimeWalkingLevel() == posLvl) // with special level we must have the exact value
            //    return true;
        } else if (level < sWorld->getIntConfig(CONFIG_MAX_PLAYER_LEVEL) && posLvl < sWorld->getIntConfig(CONFIG_MAX_PLAYER_LEVEL) // avoid double loot and other bonuses on end-game instances
            && level<=posLvl)
            return true;
    }

    return false;
}

// used for pet and players
void AzthUtils::setTwAuras(Unit *unit, AzthLevelStat const *stats, bool apply, bool skipDefense) {
    if (apply) {
        for ( auto& a : stats->pctMap) {
            if (a.first != TIMEWALKING_AURA_VISIBLE && a.second > 0)
                unit->SetAuraStack(a.first, unit, a.second);
        }

        // stats->GetLevel is higher only when special levels
        // are used (except TIMEWALKING_LVL_AUTO)
        if (sWorld->getIntConfig(CONFIG_MAX_PLAYER_LEVEL) > stats->GetLevel()) {
            // reduce melee damage based on level diff
            unit->SetAuraStack(TIMEWALKING_AURA_MOD_MELEE_DAMAGE_PCT, unit,
                               ceil(((sWorld->getIntConfig(CONFIG_MAX_PLAYER_LEVEL)- stats->GetLevel())*100 / sWorld->getIntConfig(CONFIG_MAX_PLAYER_LEVEL)) / 2)+30); // scaling by level + 30% base

            // defense aura mod must not be applied in this context when login
            // because skill is not calculated correctly.
            // This code is called on loadFromDb but defense must be calculated after login instead
            // so we need to call it separately
            if (!skipDefense && unit->GetTypeId() == TYPEID_PLAYER) {
                setTwDefense(unit->ToPlayer(), true);
            }
        }

        unit->SetAuraStack(TIMEWALKING_AURA_VISIBLE, unit, stats->GetLevel());
    } else {
        for ( auto a : stats->pctMap) {
            unit->RemoveAura(a.first);
        }

        unit->RemoveAura(TIMEWALKING_AURA_MOD_MELEE_DAMAGE_PCT);
        // defense aura mod must not be applied in this context when login
        // because skill is not calculated correctly.
        // This code is called on loadFromDb but defense must be calculated after login instead
        // so we need to call it separately
        if (!skipDefense && unit->GetTypeId() == TYPEID_PLAYER) {
            setTwDefense(unit->ToPlayer(), false);
        }
        unit->RemoveAura(TIMEWALKING_AURA_VISIBLE);
    }
}

// used for pet and players
void AzthUtils::setTwDefense(Player *player, bool apply) {
    player->RemoveAura(TIMEWALKING_AURA_MOD_DEFENSE_SKILL);

    if (apply) {
        uint32 reduction= sWorld->getIntConfig(CONFIG_MAX_PLAYER_LEVEL) - player->getLevel();
        // 1 stack = 5 defense skill reduction
        player->SetAuraStack(TIMEWALKING_AURA_MOD_DEFENSE_SKILL, player, reduction);
    }
}

AzthLevelStat const* AzthUtils::getTwStats(Player *player, uint32 level) {
    std::map<uint32, AzthLevelStat>::const_iterator itr = sAzthLevelStatMgr->GetLevelStatList().find(level * 10000 + player->getRace() * 100 + player->getClass());

    if (itr == sAzthLevelStatMgr->GetLevelStatList().end())
        return nullptr;

    return &itr->second;
}

bool AzthUtils::updateTwLevel(Player *player,Group *group)
{
    bool result = false;

    if (!player || !sAZTH->GetAZTHPlayer(player) || player->IsGameMaster())
        return result;

    uint32 levelPlayer = sAZTH->GetAZTHPlayer(player)->isTimeWalking() ? sAZTH->GetAZTHPlayer(player)->GetTimeWalkingLevel() : player->getLevel();

    if (AzthGroupMgr *azthGroup = sAZTH->GetAZTHGroup(group); azthGroup)
    {
        bool updated = false;
        uint32 maxLevel = sAzthUtils->maxTwLevel(levelPlayer, azthGroup->levelMaxGroup);

        if (maxLevel != azthGroup->levelMaxGroup)
        {
            azthGroup->levelMaxGroup = maxLevel;
            updated = true;
        }

        if (group->GetMembersCount() > azthGroup->groupSize)
        {
            azthGroup->groupSize = group->GetMembersCount();
            updated = true;
        }

        if (updated)
        {
            std::string _slvl=sAzthUtils->getLevelInfo(azthGroup->levelMaxGroup);
            std::string msg = sAzthLang->getf(AZTH_LANG_GROUP_LEVEL_REG,player, player->GetName().c_str(),_slvl.c_str(), group->GetMembersCount());
            sAzthUtils->sendMessageToGroup(player, player->GetGroup(), msg.c_str());
            azthGroup->saveToDb();
            result = true;
        }
    }

    Map* map = player->FindMap();

    if (map->IsDungeon() || map->IsRaid())
    {
        bool updated = false;
        InstanceSave* is = sInstanceSaveMgr->PlayerGetInstanceSave(player->GetGUID(), map->GetId(), player->GetDifficulty(map->IsRaid()));
        if (is) // negative condition shouldn't happen (maybe only on login?)
        {
            if (sAZTH->GetAZTHInstanceSave(is)->levelMax == 0)
            {
                sAZTH->GetAZTHPlayer(player)->instanceID = map->GetInstanceId();

                QueryResult queryRes = CharacterDatabase.Query("SELECT `levelPg`, `groupSize`, `startTime` FROM `instance` WHERE id = {}", sAZTH->GetAZTHPlayer(player)->instanceID);
                if (!queryRes)
                    return result;

                Field* fields = queryRes->Fetch();
                sAZTH->GetAZTHInstanceSave(is)->levelMax = fields[0].Get<uint32>();
                sAZTH->GetAZTHInstanceSave(is)->groupSize = fields[1].Get<uint32>();
                uint32 startTime = fields[2].Get<uint32>();

                if (startTime)
                    sAZTH->GetAZTHInstanceSave(is)->startTime = startTime;

                updated = true;
            }

            uint32 maxLevel = sAzthUtils->maxTwLevel(levelPlayer, sAZTH->GetAZTHInstanceSave(is)->levelMax);
            if (maxLevel != sAZTH->GetAZTHInstanceSave(is)->levelMax)
            {
                sAZTH->GetAZTHInstanceSave(is)->levelMax = maxLevel;
                updated = true;
            }

            uint32 cnt = map->GetPlayersCountExceptGMs();
            if (cnt > sAZTH->GetAZTHInstanceSave(is)->groupSize)
            {
                sAZTH->GetAZTHInstanceSave(is)->groupSize = cnt;
                updated = true;
            }

            if (updated)
            {
                is->InsertToDB();
                std::string _slvl = sAzthUtils->getLevelInfo(sAZTH->GetAZTHInstanceSave(is)->levelMax);
                std::string msg = sAzthLang->getf(AZTH_LANG_INSTANCE_LEVEL_REG, player, player->GetName().c_str(), _slvl.c_str(), sAZTH->GetAZTHInstanceSave(is)->groupSize);

                sAzthUtils->sendMessageToGroup(player, player->GetGroup(), msg.c_str());
                result = true;
            }
        }
    }

    return result;
}

uint32 AzthUtils::selectSpellForTW(Player* player, uint32 spellId) {
    if (sAZTH->GetAZTHPlayer(player)->isTimeWalking(true)) {
        uint32 spell=this->selectCorrectSpellRank(player->getLevel(), spellId);
        if (spell)
            return spell;
    }

    return spellId;
}

void AzthUtils::removeTimewalkingAura(Unit *unit) {

    std::list<uint32> spells;

    for (Unit::AuraApplicationMap::iterator iter = unit->GetAppliedAuras().begin(); iter != unit->GetAppliedAuras().end(); iter++)
    {
        AuraApplication const* aurApp = iter->second;
        Aura const* aura = aurApp->GetBase();

        if (aura->GetSpellInfo()->Id >= TIMEWALKING_AURA_MIN && aura->GetSpellInfo()->Id <= TIMEWALKING_AURA_MAX) {
            continue;
        }

        uint32 spellLevel = sWorld->getIntConfig(CONFIG_MAX_PLAYER_LEVEL);
        // not not allowed -> allowed
        if (!isNotAllowedSpellForTw(aura->GetSpellInfo())) {
            spellLevel = aura->GetSpellInfo()->SpellLevel == 0 ? aura->GetSpellInfo()->BaseLevel : aura->GetSpellInfo()->SpellLevel;
        }

        // item exploit check
        if (unit->GetTypeId() == TYPEID_PLAYER) {
            Player *pl = (Player*)unit;
            ItemTemplate const *proto = NULL;
            Item* castItem = aura->GetCastItemGUID() ? pl->GetItemByGuid(aura->GetCastItemGUID()) : NULL;
            if (!castItem && aura->GetCastItemEntry()) {
                proto = sObjectMgr->GetItemTemplate(aura->GetCastItemEntry());
            } else if (castItem) {
                proto = castItem->GetTemplate();
            }

            if (proto) {
                uint32 reqLevel=getCalcReqLevel(proto);
                if (reqLevel > spellLevel) {
                    spellLevel = reqLevel;
                }
            }
        }

        if (aurApp->IsPositive() || unit->IsPet() || !aura->GetSpellInfo()->HasAttribute(SPELL_ATTR3_ALLOW_AURA_WHILE_DEAD)) // not negative death persistent auras
        {
            // keep auras that are lower or equal to unit level
            if (spellLevel > 1 && spellLevel <= unit->getLevel())
                continue;

            unit->RemoveAura(iter);
        }
        // xinef: special marker, 95% sure
        else if (aura->GetSpellInfo()->HasAttribute(SPELL_ATTR5_REMOVE_ENTERING_ARENA))
        {
            // keep auras that are lower or equal to unit level
            if (spellLevel > 1 && spellLevel <= unit->getLevel())
                continue;

            unit->RemoveAura(iter);
        }
    }

    if (unit->GetTypeId() == TYPEID_PLAYER) {
        Player *pl = (Player*)unit;

        // xinef: add talent auras
        for (PlayerTalentMap::const_iterator itr = pl->GetTalentMap().begin(); itr != pl->GetTalentMap().end(); ++itr)
        {
            if (itr->second->State == PLAYERSPELL_REMOVED)
                continue;

            // xinef: talent not in new spec
            if (!(itr->second->specMask & pl->GetActiveSpecMask()))
                continue;

            spells.push_back(itr->first);
        }

        for (PlayerSpellMap::iterator itr = pl->GetSpellMap().begin(); itr != pl->GetSpellMap().end();) {
            SpellInfo const* spellInfo = sSpellMgr->GetSpellInfo(itr->first);
            if (spellInfo->IsPassive() && spellInfo->IsPositive()) {
                spells.push_back(itr->first);
            }
            ++itr;
        }

        for (uint8 i = 0; i < MAX_GLYPH_SLOT_INDEX; ++i)
        {
            if (uint32 glyph = pl->GetGlyph(i))
            {
                if (GlyphPropertiesEntry const* glyphEntry = sGlyphPropertiesStore.LookupEntry(glyph))
                {
                    if (GlyphSlotEntry const* glyphSlotEntry = sGlyphSlotStore.LookupEntry(pl->GetGlyphSlot(i)))
                    {
                        const SpellInfo* spellInfo = sSpellMgr->GetSpellInfo(glyphEntry->SpellId);
                        if (glyphEntry->TypeFlags == glyphSlotEntry->TypeFlags)
                        {
                            if (!spellInfo->Stances)
                                spells.push_back(glyphEntry->SpellId);
                            continue;
                        }
                    }
                }
            }
        }

        for (std::list<uint32>::const_iterator iterator = spells.begin(), end = spells.end(); iterator != end; ++iterator) {
            pl->RemoveAurasDueToSpell((*iterator));
            const SpellInfo *spellInfo = sSpellMgr->GetSpellInfo((*iterator));

            if (spellInfo->IsPassive() || (spellInfo->HasAttribute(SPELL_ATTR0_DO_NOT_DISPLAY) && spellInfo->Stances))
            {
                if (pl->IsNeedCastPassiveSpellAtLearn(spellInfo))
                    pl->CastSpell(pl, (*iterator), true);
            }
            // pussywizard: cast and return, learnt spells will update profession count, etc.
            else if (spellInfo->HasEffect(SPELL_EFFECT_SKILL_STEP))
            {
                pl->CastSpell(pl, (*iterator), true);
            }
        }

        //pl->_ApplyAllLevelScaleItemMods(false);
        //pl->_ApplyAllLevelScaleItemMods(true);

        //pl->InitStatsForLevel(true);
    }
    else if (unit->IsPet()) {
        Pet *pet = (Pet*)unit;

        for (PetSpellMap::iterator itr = pet->m_spells.begin(); itr != pet->m_spells.end();) {
            SpellInfo const* spellInfo = sSpellMgr->GetSpellInfo(itr->first);
            if (spellInfo->IsPassive() && spellInfo->IsPositive()) {
                spells.push_back(itr->first);
            }
            ++itr;
        }

        for (std::list<uint32>::const_iterator iterator = spells.begin(), end = spells.end(); iterator != end; ++iterator) {
            pet->RemoveAurasDueToSpell((*iterator));
            pet->CastSpell(pet, (*iterator), true);
        }
    }

}

int32 AzthUtils::getSpellReduction(Player *player, SpellInfo const* spellProto) {
    uint32 lastSpell=sSpellMgr->GetLastSpellInChain(spellProto->Id);
    // special cases where we must consider valid spell
    // even if no level (e.g. custom spell triggered by scripts)
    switch(lastSpell) {
        case 20647: // EXECUTE   (triggered)
        case 58381: // mind flay (triggered)
            return 0;
    }

    // we need to use damage/heal reduction from TW table when in TW
    // at 80 special levels
    uint32 twLevel=sAZTH->GetAZTHPlayer(player)->GetTimeWalkingLevel();
    if (twLevel != TIMEWALKING_LVL_AUTO && twLevel >= TIMEWALKING_SPECIAL_LVL_MIN && twLevel <= TIMEWALKING_SPECIAL_LVL_MAX)
        return -1;

    uint32 spellLevel = spellProto->SpellLevel == 0 ? spellProto->BaseLevel : spellProto->SpellLevel;

    if (spellLevel <= 1 && spellProto->MaxLevel == 0 && spellProto->GetNextRankSpell() == NULL)
        spellLevel = sWorld->getIntConfig(CONFIG_MAX_PLAYER_LEVEL); // assume that spell is max level
        //return -1; // not valid and not scalable. Use reduction pct from TW table

    float rate; // higher values means higher reduction
    uint32 min; // min reduction
    uint32 max; // max reduction


    if (player->getLevel() > 70) { // from 71
        rate = 1.0f;
        min = 0;
        max = 50;
    } else if (player->getLevel() >= 60) {
        rate = 1.5f;
        min = 30;
        max = 95;
    } else {
        rate = 1.8f;
        min = 30;
        max = 95;
    }

    // proportional reduction with ranked spells
    if (spellLevel > player->getLevel()) {
        // when spell rank has an higher level
        // then player we must consider a special reduction

        // the most spell level is higher related to player level
        // the more the reduction is high
        uint32 diff = uint8(spellLevel - player->getLevel());
        uint32 pct = ceil((diff * rate * 100) / spellLevel);

        return pct > max ? max : ( pct < min ? min : pct );
    } else {
        uint32 diff = uint8(player->getLevel() - spellLevel);
        uint32 bonus = ceil((player->getLevel() + diff) / 10 / 2);
        return bonus >= min ? 0 : min - bonus;
    }
}

// SPELLS TO DISABLE IN TW
bool AzthUtils::isNotAllowedSpellForTw(SpellInfo const* spellProto) {
    uint32 lastSpell=sSpellMgr->GetLastSpellInChain(spellProto->Id);

    switch(lastSpell) {
        // flask of the north
        case 67016:
        case 67017:
        case 67018:
        case 67019:
            return true;
    }

    return false;
}

// SPELLS TO ENABLE IN TW
bool AzthUtils::isSpecialSpellForTw(SpellInfo const* spellProto) {
    uint32 lastSpell=sSpellMgr->GetLastSpellInChain(spellProto->Id);

    switch(lastSpell) {
        // PALADIN
        case 32223: // Crusader Aura
        case 54428: // Divine Plea
        case 53736: // Seal of Corruption
        case 31884: // Avenging Wrath
        case 642:   // Divine shield
        case 1038:  // Hand of Salvation
        case 20166: // Seal of Wisdom
        case 20165: // Seal of Light
        case 20164: // Seal of Justice
        case 20925: // Holy Shield
        case 31801: // Seal of Vengeance
        case 6940:  // Hand of Sacrifice
        case 19746: // concentration aura
        case 25858: // Greater blessing of Kings
        case 53563: // Beacon of Light
        case 20911: // Blessing of Sanctuary
        case 25899: // Greater blessing of sanctuary


        // WARRIOR
        case 2458:  // Berserker stance
        case 18499: // Berserker rage
        case 12292: // Death Wish
        case 55694: // Enraged Regeneration
        case 23920: // Spell reflection
        case 871:   // Shield Wall
        case 1719:  // Recklessness

        // DRUID
        case 50334: // berserker
        case 29166: // innervate
        case 33357: // dash
        case 2893:  // abolish poison
        case 33891: // tree form
        case 24858: // moonkin form
        case 48451: // lifebloom
        case 22812: // barkskin
        case 53251: // wild Growth
        case 62071: // savage roar
        case 53201: // starfall

        // DK
        case 48263: // Frost Presence
        case 48265: // Unholy Presence
        case 48266: // Blood Presence
        case 48792: // Icebound Fortitude
        case 3714:  // Path of Frost
        case 48707: // Anti-Magic Shell
        case 49016: // Hysteria
        case 49039: // Lichborne
        case 49796: // Deathchill
        case 49028: // Dancing rune weapon

        // HUNTER
        case 19263: // Deterrence
        case 5384:  // Feign Death
        case 13161: // Aspect of the Beast
        case 62757: // Call Stabled Pet
        case 13159: // Aspect of the Pack
        case 34026: // Kill command
        case 34027: // Kill Command (triggered)
        case 19878: // Track Demons
        case 19879: // Track Dragonkin
        case 19880: // Track Elementals
        case 19882: // Track Giants
        case 5225:  // Track Humanoids
        case 3045:  // Rapid Fire

        // WARLOCK
        case 48018: //  Demon circle: summon
        case 132:   //  Detect invisibility
        case 47893: // Fel armor
        case 50589: // Immolation
        case 48020: // demonic circle: teleport

        // SHAMAN
        case 2825: // bloodlust
        case 6495: // sentry totem
        case 546:  // water walking
        case 30823: // shamanistic rage
        case 49284: // earth shield
        case 16191: // Mana Tide (ability triggered by Mana Tide Totem)

        // PRIEST
        case 47585: // Dispersion
        case 1706: // levitate
        case 552:  // abolish disease
        case 528:  // cure disease
        case 47788: // guardian spirit
        case 10060: // power infusion
        case 64904: // Hymn of Hope (triggered)

        // ROGUE
        case 51713: // Shadow Dance

        // MAGE
        case 66:    // Invisibility
        case 11129: // Combustion
        case 43046: // Molten Armor
        case 43008: // Ice Armor
        case 43024: // Mage Armor
        case 45438: // Ice block
        case 43039: // Ice Barrier
        case 44645: // waterbolt (mage elemental)
            return true;
    }

    return false;
}

bool AzthUtils::canScaleSpell(SpellInfo const* spellProto) {
    // do not allow spell with reagents to be scaled
    for (uint32 i = 0; i < MAX_SPELL_REAGENTS; i++)
    {
        if (spellProto->Reagent[i] > 0)
            return false;
    }

    return true;
}

bool AzthUtils::disableEnchant(Player *player, SpellItemEnchantmentEntry const* pEnchant) {
    uint32 level = player->getLevel();

    // we can't only check isTimeWalking here because
    // at this time timewalking variable has been already set when switching to timewalking
    // so we need just to check also appropriate level
    if (level >= sWorld->getIntConfig(CONFIG_MAX_PLAYER_LEVEL) || !sAZTH->GetAZTHPlayer(player)->isTimeWalking(true))
        return false;

    if (pEnchant->GemID) {
        switch(pEnchant->GemID) {
            case 42142:
            case 36766:
            case 42148:
            case 42143:
            case 42152:
            case 42153:
            case 42146:
            case 42158:
            case 42154:
            case 42150:
            case 42156:
            case 42144:
            case 42149:
            case 36767:
            case 42145:
            case 42155:
            case 42151:
            case 42157:
                return true;
        }
    }

    if (pEnchant->requiredSkillValue) {
        if (pEnchant->requiredSkillValue >= 350 && level < 65) {
            return true;
        } else if (pEnchant->requiredSkillValue >= 275 && level < 50) {
            return true;
        } else if (pEnchant->requiredSkillValue >= 200 && level < 35) {
            return true;
        } else if (pEnchant->requiredSkillValue >= 125 && level < 20) {
            return true;
        } else if (pEnchant->requiredSkillValue >= 50 && level < 10) {
            return true;
        }
    }

    return false;
}
