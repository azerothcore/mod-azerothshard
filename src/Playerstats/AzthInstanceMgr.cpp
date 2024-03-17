#include "AzthInstanceMgr.h"
#include "Log.h"
#include <boost/stacktrace.hpp>

AzthInstanceMgr::AzthInstanceMgr(InstanceSave* is)
{
    levelMax = 0;
    groupSize = 1;
    startTime = static_cast<uint32>(time(nullptr));
    instanceSave = is;
}

void AzthInstanceMgr::saveToDb()
{
    if(!instanceSave)
    {
        // instanceSave is a variable that is initializated when an instance is created and cannot be null
        // if it happens we are loosing important data and it's prone to abuses
        LOG_FATAL("azth.AzthInstanceMgr","saveToDb() cannot have null instanceSave, stacktrace: %s", boost::stacktrace::to_string(boost::stacktrace::basic_stacktrace()).c_str());
        return;
    }

    CharacterDatabase.Execute("UPDATE `instance` SET `levelPg`={}, groupSize={}, startTime={} WHERE id={}", levelMax, groupSize, instanceSave->GetInstanceId(), startTime);
}
