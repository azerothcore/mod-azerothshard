#include "AzthPlayer.h"
#include "Define.h"
#include "ObjectAccessor.h"
#include "World.h"
#include "Player.h"


void AzthPlayer::loadPvPInfo() {
    m_isPvP = false;

    uint32 accId = this->player->GetSession()->GetAccountId();
    QueryResult isPvPAccount = CharacterDatabase.Query("SELECT isPvP FROM pvp_accounts WHERE id = '%d';", accId);
    if (!isPvPAccount)
    {
        LOG_WARN("server.loading", ">> Loaded 0 pvp_accounts. DB table `pvp_accounts` is empty.", accId);
        LOG_INFO("server.loading", " ", accId);
        return;
    }
    if (isPvPAccount != nullptr && isPvPAccount->GetRowCount() > 0)
        this->m_isPvP = true;

    if (!this->m_isPvP) {
        uint32 characterId = this->player->GetGUID().GetCounter();
        QueryResult isPvPCharacter = CharacterDatabase.Query("SELECT isPvP FROM pvp_characters WHERE id = '%d';", characterId);

        if (isPvPCharacter != nullptr && isPvPCharacter->GetRowCount() > 0)
            this->m_isPvP = true;
    }
}

bool AzthPlayer::isPvP() {
    return this->m_isPvP;
}
