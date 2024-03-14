#include "AzthInstanceMgr.h"
#include "Log.h"
#include <Scripting/ScriptMgr.h>
#include <Instances/InstanceSaveMgr.cpp>
#include <Entities/Unit/Unit.cpp>
#include <boost/stacktrace.hpp>
#include <mod-azerothshard/src/ASCommon/AzthUtils.h>

 AzthInstanceMgr::AzthInstanceMgr(/*InstanceSave* is*/)
 {

    /* instanceSave = is;

     QueryResult result = CharacterDatabase.Query("SELECT `levelPg`, `groupSize`, `startTime` FROM `instance_custom` WHERE `id` = '{}'", levelMax, groupSize, instanceSave->GetInstanceId(), startTime);
     if (!result)
     {
         levelMax = 0;
         groupSize = 1;
     }
     else
     {
         Field* fields = result->Fetch();

         levelMax = fields[0].Get<uint32>();
         groupSize = fields[1].Get<uint32>();
         startTime = fields[2].Get<uint32>();

     }*/
     levelMax = 0;
     groupSize = 1;
     startTime = static_cast<uint32>(time(nullptr));

 }
 /*
 void AzthInstanceMgr::saveToDb()
 {
     if (!instanceSave)
     {
        // instanceSave is a variable that is initializated when an instance is created and cannot be null
        // if it happens we are loosing important data and it's prone to abuses
        LOG_FATAL("azth.AzthInstanceMgr","saveToDb() cannot have null instanceSave, stacktrace: %s", boost::stacktrace::to_string(boost::stacktrace::basic_stacktrace()).c_str());
        return;
     }
                                                                                                                                                                        
     CharacterDatabase.Execute("UPDATE instance SET levelPg = {}, groupSize = {}, startTime = {} WHERE id = {}", levelMax, groupSize, instanceSave->GetInstanceId(), startTime);

 }*/
 /*
 AzthInstanceMgr* AzthInstanceMgr::saveToDb()
 {
     CharacterDatabaseTransaction trans = CharacterDatabase.BeginTransaction();

     std::string _slvl = sAzthUtils->getLevelInfo(levelMax);

     CharacterDatabasePreparedStatement* stmt = CharacterDatabase.GetPreparedStatement(CHAR_INS_INSTANCE_CUSTOM_SAVE);
     stmt->SetData(6, uint8(levelMax)); 
     CharacterDatabase.Execute(stmt);

     /*trans->Append(stmt);
     CharacterDatabase.CommitTransaction(trans);

 }*/
