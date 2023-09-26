#pragma once

#include "CoreMinimal.h"
#include "PropItem.generated.h"

UENUM(BlueprintType)
enum class EInventoryItemType :uint8
{
    /**杂项 未归类的道具  注意：在快速拾取下会被快速拾取到背包 **/
    Miscellaneous,
    /**装备，指消防衣服，消防工具，各类穿戴或手持道具，注意：在快速拾取下会直接进行装备**/
    Equipment,
    /**消耗型道具 ，注意：在快速拾取下会被快速拾取到背包**/
    Consumable,
    /**纯数字类道具，如货币，注意：在快速拾取下会被快速拾取到背包**/
    Currency,
    /**放置类道具，如坐标旗，家具等 ，注意：在快速拾取下会被快速拾取到背包**/
    Place,
    /**任务类道具 ，不会受快速拾取影响**/
    Quest,
};

UENUM(BlueprintType)
enum class EInventoryEquipmentType :uint8
{
    /**头**/
    Head,
    /**身体**/
    Body,
    /**手部**/
    Hand,
    /**鞋子**/
    Foot,
    /**工具**/
    Tool
};

/**
 * 装备部位 指人员装备位置
 */
UENUM(BlueprintType)
enum class EEquipmentSlots :uint8
{
    /**头**/
    Head,
    /**胸部**/
    Chest,
    /**手部**/
    Hands,
    /**脚部**/
    Feet,
    /**主手**/
    MainHand,
    /**副手**/
    OffHand,
    /**身体**/
    Body,
};

/**
 * 装备槽类型 指装备的
 */
UENUM(BlueprintType)
enum class EEquipmentSlotType:uint8
{
    /**头**/
    Head,
    /**胸部**/
    Chest,
    /**手部**/
    Hands,
    /**脚部**/
    Feet,
    /**主手**/
    MainHand,
    /**副手**/
    OffHand,
    /**身体**/
    Body,
};


USTRUCT(BlueprintType)
struct FPropItem
{
    GENERATED_USTRUCT_BODY()
	
public:
	
    FPropItem()
    {
        ID = -1;
        Name = "";
        ItemType = EInventoryItemType::Miscellaneous;
        bIsStackable = false;
        Amount = 0;
        MaxStackSize = 0;
        Icon = nullptr;
        ItemClass = nullptr;
        ItemMainMaterial = nullptr;
        EquipmentType = EInventoryEquipmentType::Body;
        EquipSlot = EEquipmentSlotType::Head;
        EquipmentMesh = nullptr;
    };

    /**
     * 物品Id
     * 通常为SavableActorDataComponent->ActorData.Id
     */
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 ID;

    /**
     * 物品名称
     */
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FString Name;

    /**
     * 库存项类型
     */
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    EInventoryItemType ItemType;

    /**
     * 装备项类型
     */
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    EInventoryEquipmentType EquipmentType;

    /**
     * 要装备到目标槽的类型
     */
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    EEquipmentSlotType EquipSlot;

    /**
     * @brief 是否可以堆叠
     */
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    bool bIsStackable = false;

    /**
     *
     * @brief 是否可以丢弃
     */
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    bool bIsDroppable = false;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 Amount;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 MaxStackSize;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    UTexture2D* Icon;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TSubclassOf<AActor> ItemClass;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    UMaterialInstance* ItemMainMaterial;

    /**
     * 装备Mesh
     */
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    UStaticMesh* EquipmentMesh;

    bool IsValid() const;

    int32 ItemFreshStackSpace();

    bool ItemIsSame(FPropItem ToItem);

    bool IsDroppable();

    void NextItemId();
    
    static int32 GetEquipmentSlotsIndex(EEquipmentSlots InType);

private:
    static int32 ItemNextId;
};

