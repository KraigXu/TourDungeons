// Copyright © 2021-2022 GDI. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "InventoryComponent.h"
#include "EquipmentInventoryComponent.generated.h"

/**
 * 表示装备的槽数据
 * 0-4为装备槽
 */
static int32 EquipmentSlots = 5;

/**
 * 背包上限
 */
static int32 InventorySlotMax = 25;

/**
 * 背包大小
 */
static int32 InventorySlotSize = 20;

/**
 * 装备库存组件
 * 管理更新装备的库存列表
 * 同时更新角色的装备信息
 */
UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class UEquipmentInventoryComponent : public UInventoryComponent
{
    GENERATED_BODY()

public:
 
    UEquipmentInventoryComponent();
 
    /**
     * 初始化库存数组到指定的大小
     * 在设备清单中，第一组库存槽是设备槽，所以我们需要以不同的方式处理它
     * @param InInventorySize
     */
    virtual bool InitInventory(int32 InInventorySize) override;

    /**
     * 返回第一个可用的库存空间
     * 在装备清单中，第一组装备槽是装备槽，所以我们需要以不同的方式处理它
     * @return
     */
    virtual void GetEmptyInventorySpace(bool& Success, int32& Index) override;

    /**
     * 返回有效库存项目的计数
     * 在装备清单中，要过滤装备槽的计算
     * @return
     */
    virtual int32 GetInventoryItemCount() const override;

    /**
     * 服务器:当我们设置一个新项目时，检查我们是否需要更新装备的网格
     * @param InInventorySlot
     * @param InInventoryItem
     * @return
     */
    virtual bool SetInventoryItem(int32 InInventorySlot, FPropItem InInventoryItem) override;

    /**
     * 服务器:更新装备的网格并复制更改
     * @param InventorySlot
     */
    void UpdateEquippedMeshes(int32 InventorySlot);

    /**
     * 服务器:在指定的情况下将工具角色生成到主手中(如果项目中没有设置角色，则分配一个默认的装备网格)并复制到客户端。
     * @param InEquipmentMesh
     * @param InToolId
     */
    void UpdateMainHandTool(UStaticMesh* InEquipmentMesh, int32 InToolId);
};
