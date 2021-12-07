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

using namespace Acore::ChatCommands;

class azth_commandscript : public CommandScript
{
public:
    azth_commandscript() : CommandScript("azth_commandscript") { }

    ChatCommandTable GetCommands() const override
    {
        static ChatCommandTable azthCommandsTable =
        {
            { "arealevel",  handlePrintAreaLevel, SEC_PLAYER, Console::No }
        };

        static ChatCommandTable commandTable =
        {
            { "azth", azthCommandsTable}
        };

        return commandTable;
    }

    static bool handlePrintAreaLevel(ChatHandler* handler, Optional<PlayerIdentifier> target)
    {
        if (!target)
            target = PlayerIdentifier::FromTargetOrSelf(handler);

        if (!target || !target->IsConnected())
        {
            handler->SendSysMessage(LANG_PLAYER_NOT_FOUND);
            handler->SetSentErrorMessage(true);
            return false;
        }

        Player* targetPlayer = target->GetConnectedPlayer();
        WorldLocation pos = WorldLocation(targetPlayer->GetMapId(), targetPlayer->GetPositionX(), targetPlayer->GetPositionY(), targetPlayer->GetPositionZ(), targetPlayer->GetOrientation());

        if (uint32 posLvl = sAzthUtils->getPositionLevel(true, targetPlayer->GetMap(), pos))
            handler->PSendSysMessage("Current area level: %d", posLvl);
        else
            handler->PSendSysMessage("No level for this area");

        return true;
    }
};

 void AddSC_azth_commandscript()
 {
     new azth_commandscript();
 }
