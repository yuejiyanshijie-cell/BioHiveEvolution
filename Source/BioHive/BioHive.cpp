// BioHive.cpp - 模块实现

#include "BioHive.h"
#include "Modules/ModuleManager.h"

DEFINE_LOG_CATEGORY(LogBioHive);

void FBioHiveModule::StartupModule()
{
    UE_LOG(LogBioHive, Log, TEXT("BioHive module started"));
}

void FBioHiveModule::ShutdownModule()
{
    UE_LOG(LogBioHive, Log, TEXT("BioHive module shutdown"));
}

IMPLEMENT_MODULE(FBioHiveModule, BioHive);
