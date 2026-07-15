// BioHiveInputConfig.h - 输入配置辅助类
// 将此文件添加到项目中以自动设置输入映射

#pragma once

#include "CoreMinimal.h"
#include "BioHiveInputConfig.generated.h"

/*
 * 在UE编辑器中设置输入映射的步骤:
 *
 * 1. 打开 Project Settings -> Input
 *
 * 2. 添加 Axis Mappings:
 *    - MoveForward
 *      * Key: W, Scale: 1.0
 *      * Key: S, Scale: -1.0
 *    - MoveRight
 *      * Key: A, Scale: -1.0
 *      * Key: D, Scale: 1.0
 *    - LookUp
 *      * Key: Mouse Y, Scale: 1.0
 *    - Turn
 *      * Key: Mouse X, Scale: 1.0
 *
 * 3. 添加 Action Mappings:
 *    - Consume
 *      * Key: E
 *    - EvolveOrgan1
 *      * Key: One (数字1)
 *    - EvolveOrgan2
 *      * Key: Two (数字2)
 *    - EvolveOrgan3
 *      * Key: Three (数字3)
 *    - EvolveOrgan4
 *      * Key: Four (数字4)
 *    - EvolveOrgan5
 *      * Key: Five (数字5)
 */

UCLASS()
class BIOHIVE_API UBioHiveInputConfig : public UObject
{
    GENERATED_BODY()

public:
    // 这个类仅作为文档参考
    // 实际输入映射需要在编辑器中手动设置
};
