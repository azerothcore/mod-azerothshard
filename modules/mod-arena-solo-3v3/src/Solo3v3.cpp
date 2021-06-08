/*
 *  Copyright (С) since 2019 Andrei Guluaev (Winfidonarleyan/Kargatum) https://github.com/Winfidonarleyan 
 */

#include "Solo3v3.h"
#include "ArenaTeamMgr.h"
#include "BattlegroundMgr.h"
#include "Config.h"
#include "Log.h"
#include "ScriptMgr.h"
#include "Chat.h"
#include "DisableMgr.h"

Solo3v3* Solo3v3::instance()
{
    static Solo3v3 instance;
    return &instance;
}

void Solo3v3::SaveSoloDB(ArenaTeam* team)
{
    if (!team)
        return;

    // Init some variables for speedup the programm
    ArenaTeam* realTeams[3];
    uint32 itrRealTeam = 0;

    for (; itrRealTeam < 3; itrRealTeam++)
        realTeams[itrRealTeam] = nullptr;

    itrRealTeam = 0;
    uint32 oldRating = 0;

    // First get the old average rating by looping through all members in temp team and add up the rating
    for (auto const& itr : team->GetMembers())
    {
        ArenaTeam* plrArenaTeam = nullptr;

        // Find real arena team for player
        for (auto const& itrMgr : sArenaTeamMgr->GetArenaTeams())
        {
            if (itrMgr.first < 0xFFF00000 && itrMgr.second->GetCaptain() == itr.Guid && itrMgr.second->GetType() == ARENA_TEAM_SOLO_3v3)
            {
                plrArenaTeam = itrMgr.second; // found!
                break;
            }
        }

        if (!plrArenaTeam)
            continue; // Not found? Maybe player has left the game and deleted it before the arena game ends.

        ASSERT(itrRealTeam < 3);
        realTeams[itrRealTeam++] = plrArenaTeam;

        oldRating += plrArenaTeam->GetRating(); // add up all ratings from each player team
    }

    if (team->GetMembersSize() > 0)
        oldRating /= team->GetMembersSize(); // Get average

    int32 ratingModifier = team->GetRating() - oldRating; // GetRating() contains the new rating and oldRating is the old average rating

    itrRealTeam = 0;

    // Let's loop again through temp arena team and add the new rating
    for (auto const& _itr : team->GetMembers())
    {
        ArenaTeam* plrArenaTeam = realTeams[itrRealTeam++];

        if (!plrArenaTeam)
            continue;

        ArenaTeamStats atStats = plrArenaTeam->GetStats();

        if (int32(atStats.Rating) + ratingModifier < 0)
            atStats.Rating = 0;
        else
            atStats.Rating += ratingModifier;

        atStats.SeasonGames = _itr.SeasonGames;
        atStats.SeasonWins = _itr.SeasonWins;
        atStats.WeekGames = _itr.WeekGames;
        atStats.WeekWins = _itr.WeekWins;

        for (auto realMemberItr : plrArenaTeam->GetMembers())
        {
            if (realMemberItr.Guid != plrArenaTeam->GetCaptain())
                continue;

            realMemberItr.PersonalRating = _itr.PersonalRating;
            realMemberItr.MatchMakerRating = _itr.MatchMakerRating;
            realMemberItr.SeasonGames = _itr.SeasonGames;
            realMemberItr.SeasonWins = _itr.SeasonWins;
            realMemberItr.WeekGames = _itr.WeekGames;
            realMemberItr.WeekWins = _itr.WeekWins;
        }

        plrArenaTeam->SetArenaTeamStats(atStats);
        plrArenaTeam->NotifyStatsChanged();
        plrArenaTeam->SaveToDB();
    }
}

uint32 Solo3v3::GetAverageMMR(ArenaTeam* team)
{
    if (!team)
        return 0;

    uint32 matchMakerRating = 0;
    uint32 playerDivider = 0;

    for (auto const& itr : team->GetMembers())
    {
        matchMakerRating += itr.MatchMakerRating;
        playerDivider++;
    }

    //x/0 = crash
    if (!playerDivider)
        playerDivider++;

    matchMakerRating /= playerDivider;

    return matchMakerRating;
}

void Solo3v3::CleanUp3v3SoloQ(Battleground* bg)
{
    // Cleanup temp arena teams for solo 3v3
    if (bg->isArena() && bg->isRated() && bg->GetArenaType() == ARENA_TYPE_3v3_SOLO)
    {
        ArenaTeam* tempAlliArenaTeam = sArenaTeamMgr->GetArenaTeamById(bg->GetArenaTeamIdForTeam(TEAM_ALLIANCE));
        ArenaTeam* tempHordeArenaTeam = sArenaTeamMgr->GetArenaTeamById(bg->GetArenaTeamIdForTeam(TEAM_HORDE));

        if (tempAlliArenaTeam && tempAlliArenaTeam->GetId() >= MAX_ARENA_TEAM_ID)
        {
            sArenaTeamMgr->RemoveArenaTeam(tempAlliArenaTeam->GetId());
            delete tempAlliArenaTeam;
        }

        if (tempHordeArenaTeam && tempHordeArenaTeam->GetId() >= MAX_ARENA_TEAM_ID)
        {
            sArenaTeamMgr->RemoveArenaTeam(tempHordeArenaTeam->GetId());
            delete tempHordeArenaTeam;
        }
    }
}

void Solo3v3::CheckStartSolo3v3Arena(Battleground* bg)
{
    if (bg->GetArenaType() != ARENA_TYPE_3v3_SOLO)
        return;

    if (bg->GetStatus() != STATUS_IN_PROGRESS)
        return; // if CheckArenaWinConditions ends the game

    bool someoneNotInArena = false;

    ArenaTeam* team[2];
    team[0] = sArenaTeamMgr->GetArenaTeamById(bg->GetArenaTeamIdForTeam(TEAM_ALLIANCE));
    team[1] = sArenaTeamMgr->GetArenaTeamById(bg->GetArenaTeamIdForTeam(TEAM_HORDE));

    ASSERT(team[0] && team[1]);

    for (int i = 0; i < 2; i++)
    {
        for (auto const& itr : team[i]->GetMembers())
        {
            Player* plr = sObjectAccessor->FindPlayer(itr.Guid);
            if (!plr)
            {
                someoneNotInArena = true;
                continue;
            }

            if (plr->GetInstanceId() != bg->GetInstanceID())
            {
                if (sConfigMgr->GetBoolDefault("Solo.3v3.CastDeserterOnAfk", true))
                    plr->CastSpell(plr, 26013, true); // Deserter

                someoneNotInArena = true;
            }
        }
    }

    if (someoneNotInArena && sConfigMgr->GetBoolDefault("Solo.3v3.StopGameIncomplete", true))
    {
        bg->SetRated(false);
        bg->EndBattleground(TEAM_NEUTRAL);
    }
}

bool Solo3v3::CheckSolo3v3Arena(BattlegroundQueue* queue, BattlegroundBracketId bracket_id)
{
    bool soloTeam[BG_TEAMS_COUNT][MAX_TALENT_CAT]; // 2 teams and each team 3 players - set to true when slot is taken

    for (int i = 0; i < BG_TEAMS_COUNT; i++)
        for (int j = 0; j < MAX_TALENT_CAT; j++)
            soloTeam[i][j] = false; // default false = slot not taken

    queue->m_SelectionPools[TEAM_ALLIANCE].Init();
    queue->m_SelectionPools[TEAM_HORDE].Init();

    uint32 MinPlayersPerTeam = sBattlegroundMgr->isArenaTesting() ? 1 : 3;

    bool filterTalents = sConfigMgr->GetBoolDefault("Solo.3v3.FilterTalents", false);

    for (int teamId = 0; teamId < 2; teamId++) // BG_QUEUE_PREMADE_ALLIANCE and BG_QUEUE_PREMADE_HORDE
    {
        for (BattlegroundQueue::GroupsQueueType::iterator itr = queue->m_QueuedGroups[bracket_id][teamId].begin(); itr != queue->m_QueuedGroups[bracket_id][teamId].end(); ++itr)
        {
            if ((*itr)->IsInvitedToBGInstanceGUID) // Skip when invited
                continue;

            std::set<uint64>* grpPlr = &(*itr)->Players;
            for (std::set<uint64>::iterator grpPlrItr = grpPlr->begin(); grpPlrItr != grpPlr->end(); grpPlrItr++)
            {
                Player* plr = sObjectAccessor->FindPlayer(*grpPlrItr);
                if (!plr)
                    continue;

                if (!filterTalents && queue->m_SelectionPools[TEAM_ALLIANCE].GetPlayerCount() + queue->m_SelectionPools[TEAM_HORDE].GetPlayerCount() == MinPlayersPerTeam * 2)
                    return true;

                Solo3v3TalentCat plrCat = GetTalentCatForSolo3v3(plr); // get talent cat

                if ((filterTalents && soloTeam[TEAM_ALLIANCE][plrCat] == false) // is slot free in alliance team?
                    || (!filterTalents && queue->m_SelectionPools[TEAM_ALLIANCE].GetPlayerCount() != MinPlayersPerTeam))
                {
                    if (queue->m_SelectionPools[TEAM_ALLIANCE].AddGroup((*itr), MinPlayersPerTeam)) // added successfully?
                    {
                        soloTeam[TEAM_ALLIANCE][plrCat] = true; // okay take this slot

                        if ((*itr)->teamId != TEAM_ALLIANCE) // move to other team
                        {
                            (*itr)->teamId = TEAM_ALLIANCE;
                            (*itr)->_groupType = BG_QUEUE_PREMADE_ALLIANCE;
                            queue->m_QueuedGroups[bracket_id][BG_QUEUE_PREMADE_ALLIANCE].push_front((*itr));
                            itr = queue->m_QueuedGroups[bracket_id][BG_QUEUE_PREMADE_HORDE].erase(itr);
                            return CheckSolo3v3Arena(queue, bracket_id);
                        }
                    }
                }
                else if ((filterTalents && soloTeam[TEAM_HORDE][plrCat] == false) || !filterTalents) // nope? and in horde team?
                {
                    if (queue->m_SelectionPools[TEAM_HORDE].AddGroup((*itr), MinPlayersPerTeam))
                    {
                        soloTeam[TEAM_HORDE][plrCat] = true;

                        if ((*itr)->teamId != TEAM_HORDE) // move to other team
                        {
                            (*itr)->teamId = TEAM_HORDE;
                            (*itr)->_groupType = BG_QUEUE_PREMADE_HORDE;
                            queue->m_QueuedGroups[bracket_id][BG_QUEUE_PREMADE_HORDE].push_front((*itr));
                            itr = queue->m_QueuedGroups[bracket_id][BG_QUEUE_PREMADE_ALLIANCE].erase(itr);
                            return CheckSolo3v3Arena(queue, bracket_id);
                        }
                    }
                }
            }
        }
    }

    uint32 countAll = 0;
    for (int i = 0; i < BG_TEAMS_COUNT; i++)
        for (int j = 0; j < MAX_TALENT_CAT; j++)
            if (soloTeam[i][j])
                countAll++;

    return countAll == MinPlayersPerTeam * 2 ||
        (!filterTalents && queue->m_SelectionPools[TEAM_ALLIANCE].GetPlayerCount() + queue->m_SelectionPools[TEAM_HORDE].GetPlayerCount() == MinPlayersPerTeam * 2);
}

void Solo3v3::CreateTempArenaTeamForQueue(BattlegroundQueue* queue, ArenaTeam* arenaTeams[])
{
    // Create temp arena team
    for (uint32 i = 0; i < BG_TEAMS_COUNT; i++)
    {
        ArenaTeam* tempArenaTeam = new ArenaTeam();  // delete it when all players have left the arena match. Stored in sArenaTeamMgr
        std::vector<Player*> playersList;
        uint32 atPlrItr = 0;

        for (auto const& itr : queue->m_SelectionPools[TEAM_ALLIANCE + i].SelectedGroups)
        {
            if (atPlrItr >= 3)
                break; // Should never happen

            for (auto const& itr2 : itr->Players)
            {
                uint64 _PlayerGuid = itr2;
                if (Player * _player = sObjectAccessor->FindPlayer(_PlayerGuid))
                {
                    playersList.push_back(_player);
                    atPlrItr++;
                }

                break;
            }
        }

        std::stringstream ssTeamName;
        ssTeamName << "Solo Team - " << (i + 1);

        tempArenaTeam->CreateTempArenaTeam(playersList, ARENA_TEAM_SOLO_3v3, i, ssTeamName.str());
        sArenaTeamMgr->AddArenaTeam(tempArenaTeam);
        arenaTeams[i] = tempArenaTeam;
    }
}

bool Solo3v3::Arena1v1CheckTalents(Player* player)
{
    if (!player)
        return false;

    if (sConfigMgr->GetBoolDefault("Arena.1v1.BlockForbiddenTalents", false) == false)
        return true;

    uint32 count = 0;
    for (uint32 talentId = 0; talentId < sTalentStore.GetNumRows(); ++talentId)
    {
        TalentEntry const* talentInfo = sTalentStore.LookupEntry(talentId);

        if (!talentInfo)
            continue;

        for (int8 rank = MAX_TALENT_RANK - 1; rank >= 0; --rank)
        {
            if (talentInfo->RankID[rank] == 0)
                continue;

            if (player->HasTalent(talentInfo->RankID[rank], player->GetActiveSpec()))
            {
                for (int8 i = 0; FORBIDDEN_TALENTS_IN_1V1_ARENA[i] != 0; i++)
                    if (FORBIDDEN_TALENTS_IN_1V1_ARENA[i] == talentInfo->TalentTab)
                        count += rank + 1;
            }
        }
    }

    if (count >= 36)
    {
        ChatHandler(player->GetSession()).SendSysMessage("You can't join, because you have invested to much points in a forbidden talent. Please edit your talents.");
        return false;
    }
    else
        return true;
}

Solo3v3TalentCat Solo3v3::GetTalentCatForSolo3v3(Player* player)
{
    if (!player || !sConfigMgr->GetBoolDefault("Solo.3v3.FilterTalents", false))
        return MELEE;

    uint32 count[MAX_TALENT_CAT];

    for (int i = 0; i < MAX_TALENT_CAT; i++)
        count[i] = 0;

    for (uint32 talentId = 0; talentId < sTalentStore.GetNumRows(); ++talentId)
    {
        TalentEntry const* talentInfo = sTalentStore.LookupEntry(talentId);

        if (!talentInfo)
            continue;

        for (int8 rank = MAX_TALENT_RANK - 1; rank >= 0; --rank)
        {
            if (talentInfo->RankID[rank] == 0)
                continue;

            if (player->HasTalent(talentInfo->RankID[rank], player->GetActiveSpec()))
            {
                for (int8 i = 0; SOLO_3V3_TALENTS_MELEE[i] != 0; i++)
                    if (SOLO_3V3_TALENTS_MELEE[i] == talentInfo->TalentTab)
                        count[MELEE] += rank + 1;

                for (int8 i = 0; SOLO_3V3_TALENTS_RANGE[i] != 0; i++)
                    if (SOLO_3V3_TALENTS_RANGE[i] == talentInfo->TalentTab)
                        count[RANGE] += rank + 1;

                for (int8 i = 0; SOLO_3V3_TALENTS_HEAL[i] != 0; i++)
                    if (SOLO_3V3_TALENTS_HEAL[i] == talentInfo->TalentTab)
                        count[HEALER] += rank + 1;
            }
        }
    }

    uint32 prevCount = 0;

    Solo3v3TalentCat talCat = MELEE; // Default MELEE (if no talent points set)

    for (int i = 0; i < MAX_TALENT_CAT; i++)
    {
        if (count[i] > prevCount)
        {
            talCat = (Solo3v3TalentCat)i;
            prevCount = count[i];
        }
    }

    return talCat;
}
