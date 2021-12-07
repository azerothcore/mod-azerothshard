#ifndef AZTHINSTANCEMGR_H
#define AZTHINSTANCEMGR_H

#include "InstanceSaveMgr.h"

class InstanceSave;

class AzthInstanceMgr
{
public:
    explicit AzthInstanceMgr(InstanceSave* is);
    ~AzthInstanceMgr();

    void saveToDb();

    InstanceSave* instanceSave;
    uint32 levelMax;
    uint32 groupSize;
    uint32 startTime;
};
#endif
