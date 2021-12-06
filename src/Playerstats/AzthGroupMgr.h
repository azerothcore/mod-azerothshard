#ifndef AZTHGROUPMGR_H
#define AZTHGROUPMGR_H

#include "Group.h"

class AzthGroupMgr
{
public:
    explicit AzthGroupMgr(Group* original);
    ~AzthGroupMgr();

    void saveToDb();

    uint32 levelMaxGroup;
    uint32 groupSize;
    Group* group;
};
#endif
