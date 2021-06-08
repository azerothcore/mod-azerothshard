#include "Common.h"
#include "Define.h"
#include "Config.h"
#include "ItemTemplate.h"
#include "Player.h"

class Season
{
    friend class ACE_Singleton<Season, ACE_Null_Mutex>;
public:
    //GETTERS
    uint32 GetItemLevel() const;
    time_t GetStartingDate() const;
    time_t GetEndDate() const;
    bool IsEnabled() const;

    //SETTERS
    void SetItemLevel(uint32 itemLevel);
    void SetStartingDate(time_t startingDate);
    void SetEndDate(time_t endDate);
    void SetEnabled(bool enable);
    
    
    bool checkItem(ItemTemplate const* proto);
    bool checkItem(ItemTemplate const* proto, Player const* player);

    Season();
    Season(uint32 itemLevel, time_t startingDate, time_t endDate);
    bool checkItems(Player *pl);
    bool canJoinArenaOrBg(Player *pl);

private:
    bool enabled;
    uint32 itemLevel;
    time_t startingDate;
    time_t endDate;
};


#define sASeasonMgr ACE_Singleton<Season, ACE_Null_Mutex>::instance()
