#include "Apps.h"
#include "AzthSharedDefines.h"

void SmartStoneApps::teleportDalaran(Player* player)
{
    player->TeleportTo(AzthSharedDef::dalaran);
}
