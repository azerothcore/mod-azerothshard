#include "AzthInstanceMgr.h"

 AzthInstanceMgr::AzthInstanceMgr(InstanceSave* is)
 {
     levelMax = 0;
     groupSize = 1;
     startTime = static_cast<uint32>(time(nullptr));
     instanceSave = is;
 }

 void AzthInstanceMgr::saveToDb()
 {
     ASSERT(instanceSave);

     CharacterDatabase.PExecute("UPDATE instance SET levelPg = %u, groupSize = %u, startTime = %u WHERE id = %u", levelMax, groupSize, instanceSave->GetInstanceId(), startTime);
 }

