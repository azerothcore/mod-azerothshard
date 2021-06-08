#include "Apps.h"
#include "MapManager.h"
#include "AzthSharedDefines.h"
#include "GameGraveyard.h"
#include "AZTH.h"

void SmartStoneApps::blackMarketTeleport(Player *player) {
    if (player->IsInCombat())
        return;
    
    if (!sAZTH->GetAZTHPlayer(player)->isInBlackMarket()) {
        WorldLocation pos = WorldLocation(player->GetMapId(), player->GetPositionX(), player->GetPositionY(), player->GetPositionZ(), player->GetOrientation());
        sAZTH->GetAZTHPlayer(player)->setLastPositionInfo(sAZTH->GetAZTHPlayer(player)->getCurrentDimensionByAura(), pos);

        player->TeleportTo(AzthSharedDef::blackMarket);
    }
    else {
        WorldLocation const& pos = sAZTH->GetAZTHPlayer(player)->getLastPositionInfo(sAZTH->GetAZTHPlayer(player)->getCurrentDimensionByAura());
        if (!MapManager::IsValidMapCoord(pos))
            return;

        Map *m = sMapMgr->FindBaseMap(pos.GetMapId());

        if (m && m->IsDungeon())
        {
            GraveyardStruct const* graveyard = sGraveyard->GetClosestGraveyard(pos.GetPositionX(), pos.GetPositionY(), pos.GetPositionZ(), pos.GetMapId(), TEAM_NEUTRAL);

            if (graveyard)
                player->TeleportTo(graveyard->Map, graveyard->x, graveyard->y, graveyard->z, player->GetOrientation());
        }
        else
            player->TeleportTo(pos);
    }   
}

void SmartStoneApps::teleportHouse(Player *owner, Player *guest) {
    if (guest->IsInCombat())
        return;

    WorldLocation const& pos = sAZTH->GetAZTHPlayer(owner)->getLastPositionInfo(AZTH_SMRTST_POSITION_HOUSE_INDEX);
    if (!MapManager::IsValidMapCoord(pos))
        return;

    guest->TeleportTo(pos);
}

void SmartStoneApps::jukebox(Player *player) {
    if (player->FindNearestCreature(300205, 20.f, true) != NULL)
        return;

    TempSummon* jukebox_summon = player->SummonCreature(300205, player->GetPositionX(),
            player->GetPositionY(), player->GetPositionZ(),
            player->GetOrientation(),
            TEMPSUMMON_TIMED_DESPAWN, 600 * 1000, 0);
    Map* playerMap = player->GetMap();
    Creature* jukebox = playerMap->GetCreature(jukebox_summon->GetGUID());
    jukebox->GetMotionMaster()->MoveFollow(player, 2, 2);   
}
