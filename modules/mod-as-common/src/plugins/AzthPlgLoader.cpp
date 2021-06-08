#include "AzthPlgLoader.h"
#include "Log.h"

#ifndef SCRIPTLOADER_CPP
#define	SCRIPTLOADER_CPP

/* This is where custom AzerothShard function scripts declarations should be added. */
void AddSC_azth_commandscript();
void AddSC_azth_player_plg();
void AddSC_AzthWorldScript();


void AddAzthScripts()
{
    sLog->outString("Loading AzerothShard Plugins...");
    /* This is where custom AzerothShard scripts should be added. */
    AddSC_azth_commandscript();
    AddSC_azth_player_plg();
    AddSC_AzthWorldScript();
}


#endif	/* SCRIPTLOADER_CPP */

