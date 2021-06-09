#include "ScriptMgr.h"
#include "Player.h"
#include "LFG.h"
#include "azth_custom_hearthstone_mode.h"
#include "AzthUtils.h"
#include "AzthPlayer.h"
#include "MapManager.h"
#include "AZTH.h"

class PvPMode : public GlobalScript
{
public:
    PvPMode() : GlobalScript("PvPMode") {}

    void OnInitializeLockedDungeons(Player* player, uint8&  /*level*/, uint32& lockData, lfg::LFGDungeonData const* /*dungeon*/) override
    {
        if (sAZTH->GetAZTHPlayer(player)->isPvP())
        {
            lockData = 6; // LFG_LOCKSTATUS_RAID_LOCKED; //disable raid/dungeon if is a character/pvp account
        }
    }

    void OnAfterInitializeLockedDungeons(Player* player) override
    {
        if ((player->GetMap()->IsDungeon() && !player->GetMap()->IsBattlegroundOrArena()) && sAZTH->GetAZTHPlayer(player)->isPvP())
        {
            player->TeleportTo(player->m_recallMap, player->m_recallX, player->m_recallY, player->m_recallZ, player->m_recallO);
            ChatHandler(player->GetSession()).SendSysMessage(sAzthLang->get(AZTH_LANG_PVPACCOUNT_DUNGEON, player));
        }
    }
};

class PvPModeFirstLogin : public PlayerScript
{
public:
    PvPModeFirstLogin() : PlayerScript("PvPModeFirstLogin") {}

    void OnCreate(Player* player) override
    {
        sAZTH->GetAZTHPlayer(player)->loadPvPInfo();

        if (sAZTH->GetAZTHPlayer(player)->isPvP()) {
            player->setCinematic(1);
            player->SetLevel(80);

            WorldLocation startLoc;
            startLoc.m_positionX = 4718.45f;
            startLoc.m_positionY = -1974.84f;
            startLoc.m_positionZ = 1086.91f;
            startLoc.m_orientation = 0.06f;
            startLoc.m_mapId  = 1;

            player->Relocate(&startLoc);
            player->ResetMap();
            player->SetMap(sMapMgr->CreateMap(1, player));

            CharacterDatabase.PQuery("REPLACE INTO azth_pvp_characters (id,isPvP) VALUES(%d, 1)", player->GetGUID().GetCounter());

            player->SaveToDB(false,false);
        }
    }

    void OnFirstLogin(Player* player) override
    {
        if (sAZTH->GetAZTHPlayer(player)->isPvP())
        {
            // restore at first login flag for now, to be used inside OnLogin function
            //player->SetAtLoginFlag(AT_LOGIN_FIRST);
        }
    }

    void OnLogin(Player* player) override
    {
        sAZTH->GetAZTHPlayer(player)->loadPvPInfo();

        if (sAZTH->GetAZTHPlayer(player)->isPvP()){
            if (!player->HasAchieved(13))//if (player->HasAtLoginFlag(AT_LOGIN_FIRST))
            {
                // delevel + levelup to fix achievements
                player->GiveLevel(79);
                player->GiveLevel(80);

                player->SetHonorPoints(2000);

                player->InitTalentForLevel();
                player->learnDefaultSpells();
                player->SetUInt32Value(PLAYER_XP, 0);
                sAzthUtils->learnClassSpells(player, false);

                sAZTH->GetAZTHPlayer(player)->changeDimension(DIMENSION_PVP);

                // riding + flying
                player->learnSpell(34091); // artisan riding
                player->learnSpell(54197); // coldweather flying

                player->learnSpell(41514); // flying mount, netherdrake
                player->learnSpell(54753); // ground mount, bear

                // Cast spells that teach dual spec
                // Both are also ImplicitTarget self and must be cast by player
                player->CastSpell(player, 63680, true, NULL, NULL, player->GetGUID());
                player->CastSpell(player, 63624, true, NULL, NULL, player->GetGUID());


                // explore all zones
                for (uint8 i = 0; i<PLAYER_EXPLORED_ZONES_SIZE; i++)
                    player->SetFlag(PLAYER_EXPLORED_ZONES_1 + i, 0xFFFFFFFF);

                player->UpdateSkillsToMaxSkillsForLevel(); // set all skills

                sAZTH->GetAZTHPlayer(player)->AzthMaxPlayerSkill();

                player->StoreNewItemInBestSlots(23162, 8);

                player->RemoveAtLoginFlag(AT_LOGIN_FIRST);

                for (uint8 i = INVENTORY_SLOT_ITEM_START; i < INVENTORY_SLOT_ITEM_END; i++)
                {
                    if (Item* pItem = player->GetItemByPos(INVENTORY_SLOT_BAG_0, i))
                    {
                        uint16 eDest;
                        // equip offhand weapon/shield if it attempt equipped before main-hand weapon
                        InventoryResult msg = player->CanEquipItem(NULL_SLOT, eDest, pItem, false);
                        if (msg == EQUIP_ERR_OK)
                        {
                            player->RemoveItem(INVENTORY_SLOT_BAG_0, i, true);
                            player->EquipItem(eDest, pItem, true);
                        }
                        // move other items to more appropriate slots (ammo not equipped in special bag)
                        else
                        {
                            ItemPosCountVec sDest;
                            msg = player->CanStoreItem(NULL_BAG, NULL_SLOT, sDest, pItem, false);
                            if (msg == EQUIP_ERR_OK)
                            {
                                player->RemoveItem(INVENTORY_SLOT_BAG_0, i, true);
                                pItem = player->StoreItem(sDest, pItem, true);
                            }

                            // if  this is ammo then use it
                            msg = player->CanUseAmmo(pItem->GetEntry());
                            if (msg == EQUIP_ERR_OK)
                                player->SetAmmo(pItem->GetEntry());
                        }
                    }
                }

                player->SetHealth(player->GetMaxHealth());
                if (player->getPowerType() == POWER_MANA || player->getClass() == CLASS_DRUID)
                {
                    player->SetPower(POWER_MANA, player->GetMaxPower(POWER_MANA));
                }


                //player->TeleportTo(1, 4718.45, -1974.84, 1086.91, 0.19); //teleport to black market
                player->SaveToDB(false, false);
            }


            player->SetTaxiCheater(true); // must be set at each login
        }
    }

    void OnMapChanged(Player* player) override {
        if (!player)
            return;

        MapEntry const* mEntry = sMapStore.LookupEntry(player->GetMapId());

        if (player->InArena() && mEntry->IsBattleArena() && !player->IsSpectator()) {
            switch(player->getClass()) {
                case CLASS_WARLOCK:
                    player->CastSpell(player, 58889, TRIGGERED_FULL_MASK); // warlock soulwell in arena
                break;
                case CLASS_MAGE:
                    player->CastSpell(player, 58661, TRIGGERED_FULL_MASK); // mage refreshment table in arena
                break;
            }
        }
    }

    // logger for custom extended costs
    void OnAfterStoreOrEquipNewItem(Player* player, uint32  /*vendorslot*/, Item* item, uint8  /*count*/, uint8  /*bag*/, uint8  /*slot*/, ItemTemplate const*  /*pProto*/, Creature* /*pVendor*/, VendorItem const*  /*crItem*/, bool  /*bStore*/) override
    {
        if (!sAZTH->GetAZTHPlayer(player)->isPvP())
            return;

        /*if (pVendor->GetScriptName() == "npc_azth_vendor") {

            std::vector<HearthstoneVendor> & vendors = sHearthstoneMode->hsVendors;
            int pos = 0;
            for (std::size_t i = 0; i < vendors.size(); i++)
            {
                HearthstoneVendor temp = vendors.at(i);
                if (temp.id == pVendor->GetEntry())
                    pos = i;
            }

            if (pos == 0)
                return;

            HearthstoneVendor vendor = vendors.at(pos);

            if (vendor.pvpVendor) {
         */
                // all items stored in pvp account should be soulbound
                item->SetBinding(true);
                item->ApplyModFlag(ITEM_FIELD_FLAGS, ITEM_FIELD_FLAG_UNK1, true);
                /* try to avoid for performances
                SQLTransaction trans = CharacterDatabase.BeginTransaction();
                item->SaveToDB(trans);
                CharacterDatabase.CommitTransaction(trans);*/
        /*    }
        }*/
    }

    void OnBeforeDurabilityRepair(Player * player, ObjectGuid  /*npcGUID*/, ObjectGuid  /*itemGUID*/, float & discountMod, uint8  /*guildbank*/) override {
        if (!sAZTH->GetAZTHPlayer(player)->isPvP())
            return;

        discountMod = 0.f;
    }
};


void AddSC_PvPMode()
{
    new PvPMode();
    new PvPModeFirstLogin();
}
