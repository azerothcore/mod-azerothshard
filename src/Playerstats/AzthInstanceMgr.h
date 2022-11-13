#ifndef AZTHINSTANCEMGR_H
#define AZTHINSTANCEMGR_H

#include "InstanceSaveMgr.h"

class InstanceSave;

class AzthInstanceMgr
{
public:
    explicit AzthInstanceMgr(/*InstanceSave* is*/);
    ~AzthInstanceMgr();

    //AzthInstanceMgr* saveToDb(/*CharacterDatabaseTransaction trans*/);
    [[nodiscard]] uint32 GetInstanceId() const { return m_instanceid; }

    InstanceSave* instanceSave;
    uint32 levelMax;
    uint32 groupSize;
    uint32 startTime;
    uint32 m_instanceid;

};

#endif
