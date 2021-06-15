/*
 *  Copyright (С) since 2019 Andrei Guluaev (Winfidonarleyan/Kargatum) https://github.com/Winfidonarleyan
 */

#include "AZTH.h"
#include "Log.h"
#include "ArenaTeamMgr.h"
#include "BattlegroundMgr.h"
#include "DatabaseEnv.h"
#include "ObjectMgr.h"
#include "Config.h"
#include "Solo3v3.h"
//#include "ExtraDatabase.h"

AZTH* AZTH::instance()
{
    static AZTH instance;
    return &instance;
}

void AZTH::LoadConfig(bool reload)
{
    // PvP Ranks
    Tokenizer PvPRankList(sConfigMgr->GetStringDefault("PvPRank.HKPerRank", "10,50,100,200,450,750,1300,2000,3500,6000,9500,15000,21000,30000"), ',');
    for (uint8 i = 0; i < PvPRankList.size(); i++)
        _PvP_Ranks[i] = atoi(PvPRankList[i]);

    _RatePvPRankExtraHonor = sConfigMgr->GetFloatDefault("PvPRank.Rate.ExtraHonor", 1);

    // External Mail
    _IsExternalMailEnable = sConfigMgr->GetBoolDefault("ExternalMail", 0);
    _ExternalMailInterval = sConfigMgr->GetIntDefault("ExternalMailInterval", 1) * MINUTE * IN_MILLISECONDS;

    // Custom XP
    _CustomXPSecurity = sConfigMgr->GetIntDefault("Player.customXP.security", 0);
    _CustomXPMax = sConfigMgr->GetFloatDefault("Player.customXP.maxValue", 1);
    _CustomXPShowOnLogin = sConfigMgr->GetBoolDefault("Player.customXP.showOnLogin", false);

    if (!reload)
        LoadContainers();
}

void AZTH::LoadContainers()
{
    // ArenaSlotByType
    if (!ArenaTeam::ArenaSlotByType.count(ARENA_TEAM_1v1))
        ArenaTeam::ArenaSlotByType[ARENA_TEAM_1v1] = ARENA_SLOT_1v1;

    if (!ArenaTeam::ArenaSlotByType.count(ARENA_TEAM_SOLO_3v3))
        ArenaTeam::ArenaSlotByType[ARENA_TEAM_SOLO_3v3] = ARENA_SLOT_SOLO_3v3;

    // ArenaReqPlayersForType
    if (!ArenaTeam::ArenaReqPlayersForType.count(ARENA_TYPE_1v1))
        ArenaTeam::ArenaReqPlayersForType[ARENA_TYPE_1v1] = 2;

    if (!ArenaTeam::ArenaReqPlayersForType.count(ARENA_TYPE_3v3_SOLO))
        ArenaTeam::ArenaReqPlayersForType[ARENA_TYPE_3v3_SOLO] = 6;

    // queueToBg
    if (!BattlegroundMgr::queueToBg.count(BATTLEGROUND_QUEUE_1v1))
        BattlegroundMgr::queueToBg[BATTLEGROUND_QUEUE_1v1] = BATTLEGROUND_AA;

    if (!BattlegroundMgr::queueToBg.count(BATTLEGROUND_QUEUE_3v3_SOLO))
        BattlegroundMgr::queueToBg[BATTLEGROUND_QUEUE_3v3_SOLO] = BATTLEGROUND_AA;

    // ArenaTypeToQueue
    if (!BattlegroundMgr::ArenaTypeToQueue.count(ARENA_TYPE_1v1))
        BattlegroundMgr::ArenaTypeToQueue[ARENA_TYPE_1v1] = (BattlegroundQueueTypeId)BATTLEGROUND_QUEUE_1v1;

    if (!BattlegroundMgr::ArenaTypeToQueue.count(ARENA_TYPE_3v3_SOLO))
        BattlegroundMgr::ArenaTypeToQueue[ARENA_TYPE_3v3_SOLO] = (BattlegroundQueueTypeId)BATTLEGROUND_QUEUE_3v3_SOLO;

    // QueueToArenaType
    if (!BattlegroundMgr::QueueToArenaType.count(BATTLEGROUND_QUEUE_1v1))
        BattlegroundMgr::QueueToArenaType[BATTLEGROUND_QUEUE_1v1] = (ArenaType)ARENA_TYPE_1v1;

    if (!BattlegroundMgr::QueueToArenaType.count(BATTLEGROUND_QUEUE_3v3_SOLO))
        BattlegroundMgr::QueueToArenaType[BATTLEGROUND_QUEUE_3v3_SOLO] = (ArenaType)ARENA_TYPE_3v3_SOLO;
}

// Add
void AZTH::AddAZTHPlayer(Player* player)
{
    if (_playersStore.count(player))
    {
        sLog->outError("AZTH::AddAZTHPlayer - _playersStore.count(player)");
        return;
    }

    _playersStore.insert(std::make_pair(player, new AzthPlayer(player)));
}

void AZTH::AddAZTHObject(Object* object)
{
    if (_objectsStore.count(object))
    {
        sLog->outError("AZTH::AddAZTHObject - _objectsStore.count(object)");
        return;
    }

    _objectsStore.insert(std::make_pair(object, new AzthObject(object)));
}

void AZTH::AddAZTHGroup(Group* group)
{
    if (_groupStore.count(group))
    {
        sLog->outError("AZTH::AddAZTHGroup - _groupStore.count(group)");
        return;
    }

    _groupStore.insert(std::make_pair(group, new AzthGroupMgr(group)));
}

void AZTH::AddAZTHInstanceSave(InstanceSave* instanceSave)
{
    if (_instanceSaveStore.count(instanceSave))
    {
        sLog->outError("AZTH::AddAZTHInstanceSave - _instanceSaveStore.count(instanceSave)");
        return;
    }

    _instanceSaveStore.insert(std::make_pair(instanceSave, new AzthInstanceMgr(instanceSave)));
}

void AZTH::AddAZTHLoot(Loot* loot)
{
    if (_lootStore.count(loot))
    {
        sLog->outError("AZTH::AddAZTHLoot - _lootStore.count(instanceSave)");
        return;
    }

    _lootStore.insert(std::make_pair(loot, true));
}

// Get
AzthPlayer* AZTH::GetAZTHPlayer(Player* player)
{
    if (!_playersStore.count(player))
    {
        sLog->outError("AZTH::GetAZTHPlayer - !_playersStore.count(player)");
        return nullptr;
    }

    return _playersStore[player];
}

AzthObject* AZTH::GetAZTHObject(Object* object)
{
    if (!_objectsStore.count(object))
    {
        sLog->outError("AZTH::GetAZTHObject - !_objectsStore.count(object)");
        return nullptr;
    }

    return _objectsStore[object];
}

AzthGroupMgr* AZTH::GetAZTHGroup(Group* group)
{
    if (!group)
    {
        return nullptr;
    }

    if (!_groupStore.count(group))
    {
        sLog->outError("AZTH::GetAZTHGroup - !_groupStore.count(group)");
        return nullptr;
    }

    return _groupStore[group];
}

AzthInstanceMgr* AZTH::GetAZTHInstanceSave(InstanceSave* instanceSave)
{
    if (!_instanceSaveStore.count(instanceSave))
    {
        sLog->outError("AZTH::GetAZTHInstanceSave - !_instanceSaveStore.count(instanceSave)");
        return nullptr;
    }

    return _instanceSaveStore[instanceSave];
}

bool AZTH::GetAZTHLoot(Loot* loot)
{
    if (!_lootStore.count(loot))
        return false;

    return _lootStore[loot];
}

// Delete
void AZTH::DeleteAZTHPlayer(Player* player)
{
    if (!_playersStore.count(player))
    {
        sLog->outError("AZTH::DeleteAZTHPlayer - !_playersStore.count(player)");
        return;
    }

    _playersStore.erase(player);
}

void AZTH::DeleteAZTHObject(Object* object)
{
    if (!_objectsStore.count(object))
    {
        sLog->outError("AZTH::DeleteAZTHObject - !_objectsStore.count(object)");
        return;
    }

    _objectsStore.erase(object);
}

void AZTH::DeleteAZTHGroup(Group* group)
{
    if (!_groupStore.count(group))
    {
        sLog->outError("AZTH::DeleteAZTHGroup - !_groupStore.count(group)");
        return;
    }

    _groupStore.erase(group);
}

void AZTH::DeleteAZTHInstanceSave(InstanceSave* instanceSave)
{
    if (!_instanceSaveStore.count(instanceSave))
    {
        sLog->outError("AZTH::DeleteAZTHInstanceSave - !_instanceSaveStore.count(instanceSave)");
        return;
    }

    _instanceSaveStore.erase(instanceSave);
}

void AZTH::DeleteAZTHLoot(Loot* loot)
{
    if (!_lootStore.count(loot))
    {
        sLog->outError("AZTH::DeleteAZTHLoot - !_instanceSaveStore.count(loot)");
        return;
    }

    _lootStore.erase(loot);
}

// PvP Kanks
uint32 AZTH::GetKillsPerRank(uint32 Rank)
{
    return Rank < HKRANKMAX ? _PvP_Ranks[Rank] : 0;
}

float AZTH::GetRatePvPRankExtraHonor()
{
    return _RatePvPRankExtraHonor;
}

// [AZTH] Custom functions
void AZTH::SendGameMail(Player* receiver, std::string subject, std::string body, uint32 money, uint32 itemId /* =0 */, uint32 itemCount /* =0 */)
{
    uint32 receiver_guid = receiver->GetGUID().GetCounter();

    SQLTransaction trans = CharacterDatabase.BeginTransaction();
    MailDraft* mail = new MailDraft(subject, body);

    if (money)
        mail->AddMoney(money);

    if (itemId)
    {
        ItemTemplate const* pProto = sObjectMgr->GetItemTemplate(itemId);
        if (pProto)
        {
            Item* mailItem = Item::CreateItem(itemId, itemCount);
            if (mailItem)
            {
                mailItem->SaveToDB(trans);
                mail->AddItem(mailItem);
            }
        }
    }

    mail->SendMailTo(trans, receiver ? receiver : MailReceiver(receiver_guid), MailSender(MAIL_NORMAL, 0, MAIL_STATIONERY_GM), MAIL_CHECK_MASK_RETURNED);
    delete mail;

    CharacterDatabase.CommitTransaction(trans);
}

// External Mail
bool AZTH::IsExternalMailEnable()
{
    return _IsExternalMailEnable;
}

uint32 AZTH::GetExternalMailInterval()
{
    return _ExternalMailInterval;
}

void AZTH::SendExternalMails()
{
    // [AZTH-DISABLED]
    // sLog->outString("EXTERNAL MAIL> Sending mails in queue...");

    // PreparedStatement* stmt = ExtraDatabase.GetPreparedStatement(EXTRA_GET_EXTERNAL_MAIL);
    // PreparedQueryResult result = ExtraDatabase.Query(stmt);
    // if (!result)
    // {
    //     sLog->outError("EXTERNAL MAIL> No mails in queue...");
    //     return;
    // }

    // SQLTransaction trans = CharacterDatabase.BeginTransaction();
    // SQLTransaction trans2 = ExtraDatabase.BeginTransaction();

    // MailDraft* mail = nullptr;

    // do
    // {
    //     Field* fields = result->Fetch();
    //     uint32 id = fields[0].GetUInt32();
    //     uint32 receiver_guid = fields[1].GetUInt32();
    //     std::string subject = fields[2].GetString();
    //     std::string body = fields[3].GetString();
    //     uint32 money = fields[4].GetUInt32();
    //     uint32 itemId = fields[5].GetUInt32();
    //     uint32 itemCount = fields[6].GetUInt32();

    //     Player* receiver = ObjectAccessor::FindPlayerByLowGUID(receiver_guid);

    //     mail = new MailDraft(subject, body);

    //     if (money)
    //         mail->AddMoney(money);

    //     if (itemId)
    //     {
    //         ItemTemplate const* pProto = sObjectMgr->GetItemTemplate(itemId);
    //         if (pProto)
    //         {
    //             Item* mailItem = Item::CreateItem(itemId, itemCount);
    //             if (mailItem)
    //             {
    //                 mailItem->SaveToDB(trans);
    //                 mail->AddItem(mailItem);
    //             }
    //         }
    //         else
    //             sLog->outError("non-existing item");
    //     }

    //     mail->SendMailTo(trans, receiver ? receiver : MailReceiver(receiver_guid), MailSender(MAIL_NORMAL, 0, MAIL_STATIONERY_GM), MAIL_CHECK_MASK_RETURNED);
    //     delete mail;

    //     stmt = ExtraDatabase.GetPreparedStatement(EXTRA_DEL_EXTERNAL_MAIL);
    //     stmt->setUInt32(0, id);
    //     trans2->Append(stmt);

    // } while (result->NextRow());

    // CharacterDatabase.CommitTransaction(trans);
    // ExtraDatabase.CommitTransaction(trans2);
    // sLog->outError("EXTERNAL MAIL> ALL MAILS SENT!");
}

// Custom XP
uint32 AZTH::GetCustomXPSecurity()
{
    return _CustomXPSecurity;
}

float AZTH::GetCustomXPMax()
{
    return _CustomXPMax;
}

bool AZTH::IsCustomXPShowOnLogin()
{
    return _CustomXPShowOnLogin;
}
