#ifndef SMARTSTONE_H
#define SMARTSTONE_H

#include "Define.h"
#include "Common.h"
#include "AzthLanguageStrings.h"
#include "AzthSharedDefines.h"

class WorldSession;
class Player;

struct SmartStoneCommand
{
    // struct doesn't have equal operator by default
    bool operator==(const SmartStoneCommand& rhs)
    {
        return id == rhs.id;
    }

    uint32 id;
    std::string text_def;
    std::string text_it;
    uint32 item;
    uint32 icon;
    uint32 parent_menu;
    uint32 type;
    uint32 action;
    int32 charges;
    uint64 duration;
    
    std::string getText(Player *pl = nullptr);
};

struct SmartStonePlayerCommand
{
    bool operator==(const SmartStonePlayerCommand& rhs)
    {
        return id == rhs.id;
    }

    uint32 id;
    int32 charges;
    uint64 duration;
};

enum MenuType {
    DO_SCRIPTED_ACTION = 1,
    OPEN_CHILD = 2,
    DO_SCRIPTED_ACTION_WITH_CODE = 3
    // DO_DB_ACTION=4 [TO IMPLEMENT]
};

class SmartStone
{
public:

    int SMARTSTONE_VENDOR_ENTRY = 170000;

    const SmartStoneCommand nullCommand = SmartStoneCommand{ 0, "", "", 0, 0, 0, 0, 0, 0, 0};

    std::vector<SmartStoneCommand> ssCommands2;

    void loadCommands();
    void SmartStoneSendListInventory(WorldSession * session, uint64 vendorGuid);
    SmartStoneCommand getCommandById(uint32 id);
    SmartStoneCommand getCommandByItem(uint32 item);
    bool isNullCommand(SmartStoneCommand command);
    SmartStonePlayerCommand toPlayerCommand(SmartStoneCommand command);
};

#define sSmartStone ACE_Singleton<SmartStone, ACE_Null_Mutex>::instance()

#endif
