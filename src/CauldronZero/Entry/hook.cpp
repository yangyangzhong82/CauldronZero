
#include "ll/api/memory/Hook.h"
#include "CauldronZero/logger.h"


#include "mc/deps/scripting/runtime/ContextId.h"
#include "mc/scripting/ScriptPrintLogger.h"
#include "mc\scripting\ScriptPlugin.h"
#include "mc\scripting\ScriptPluginResult.h"
#include "mc\scripting\ScriptPrintLogger.h"
#include "mc\world\actor\registry\SpawnGroupRegistry.h"
#include "mc\world\level\PortalForcer.h"


#include "mc\world\attribute\Amplifier.h"
#include "mc\world\attribute\AttributeCollection.h"
#include "mc\world\item\registry\ItemRegistryRef.h"
#include "mc\world\vanilla_world_systems\Impl.h"
#include "mc\world\vanilla_world_systems\VanillaWorldSystems.h"


#include "mc\certificates\identity\PlayerAuthenticationInfo.h"
#include "mc\deps\ecs\strict\StrictEntityContext.h"
#include "mc\entity\components_json_legacy\BossComponent.h"
#include "mc\events\MinecraftEventing.h"
#include "mc\network\ServerNetworkSystem.h"
#include "mc\util\ActorInventoryUtils.h"
#include "mc\util\DeserializeDataParams.h"
#include "mc\world\ContainerCloseListener.h"
#include "mc\world\ContainerContentChangeListener.h"
#include "mc\world\ContainerSizeChangeListener.h"
#include "mc\world\Direction.h"
#include "mc\world\GameCallbacks.h"
#include "mc\world\GameSession.h"
#include "mc\world\PlayerUIContainer.h"
#include "mc\world\SimplePlayerContainer.h"
#include "mc\world\SimpleSparseContainer.h"
#include "mc\world\actor\ActorComponentFactory.h"
#include "mc\world\actor\ActorSpawnRuleData.h"
#include "mc\world\actor\Description.h"
#include "mc\world\actor\ISynchedActorDataAdapter.h"
#include "mc\world\actor\InternalComponentRegistry.h"
#include "mc\world\actor\KnockbackArmorUpdater.h"
#include "mc\world\actor\KnockbackRules.h"
#include "mc\world\actor\LegacyMolangVariableUpdate.h"
#include "mc\world\actor\NetheriteArmorEquippedListener.h"
#include "mc\world\actor\Parser.h"
#include "mc\world\actor\SpawnChecks.h"
#include "mc\world\actor\SpawnGroupsLegacyParser.h"
#include "mc\world\actor\SynchedActorData.h"
#include "mc\world\actor\VanillaActors.h"
#include "mc\world\actor\VanillaBuiltInEntities.h"
#include "mc\world\actor\provider\ActorEnvironment.h"
#include "mc\world\actor\provider\ActorGlobal.h"
#include "mc\world\actor\provider\ActorLifecycle.h"
#include "mc\world\actor\provider\ActorMovement.h"
#include "mc\world\actor\provider\ActorOffset.h"
#include "mc\world\actor\provider\ActorPermission.h"
#include "mc\world\actor\provider\ActorRiding.h"
#include "mc\world\actor\provider\ActorTransform.h"
#include "mc\world\actor\provider\MobJump.h"
#include "mc\world\actor\provider\PlayerMovement.h"
#include "mc\world\actor\provider\player_destroy\PlayerDestroy.h"
#include "mc\world\attribute\AttributeData.h"
#include "mc\world\attribute\AttributeInstanceDelegate.h"
#include "mc\world\attribute\AttributeInstanceForwarder.h"
#include "mc\world\attribute\ExhaustionAttributeDelegate.h"
#include "mc\world\attribute\HungerAttributeDelegate.h"
#include "mc\world\attribute\InstantaneousAttributeBuff.h"
#include "mc\world\attribute\SharedAmplifiers.h"
#include "mc\world\attribute\SharedBuffs.h"
#include "mc\world\attribute\TemporalAttributeBuff.h"
#include "mc\world\containers\DynamicContainerTracker.h"
#include "mc\world\containers\FullContainerName.h"
#include "mc\world\containers\PlayerContainerRefresher.h"
#include "mc\world\containers\models\ContainerModel.h"
#include "mc\world\containers\models\ExpandoContainerModel.h"
#include "mc\world\containers\models\HudContainerModel.h"
#include "mc\world\containers\models\InventoryContainerModel.h"
#include "mc\world\containers\models\PlayerUIContainerModel.h"
#include "mc\world\containers\models\PlayerUIContainerModelBase.h"
#include "mc\world\inventory\BaseContainerMenu.h"
#include "mc\world\inventory\ContainerWeakRef.h"
#include "mc\world\inventory\CraftingContainer.h"
#include "mc\world\inventory\ExperienceConstants.h"
#include "mc\world\inventory\InventoryMenu.h"
#include "mc\world\inventory\transaction\InventoryAction.h"
#include "mc\world\inventory\transaction\ItemReleaseInventoryTransaction.h"
#include "mc\world\inventory\transaction\PlayerTransactionSubject.h"
#include "mc\world\level\ActorDimensionTransferProxy.h"
#include "mc\world\phys\rope\AABBBucket.h"
#include "mc\world\phys\rope\Rope.h"
#include "mc\world\phys\rope\RopeAABB.h"
#include "mc\world\phys\rope\RopePoints.h"
#include "mc\world\phys\rope\RopeSystem.h"
#include <mc\server\ChunkGenerationManager.h>

#include "mc\network\packet\MobEffectPacket.h"
#include "mc\network\packet\SetActorDataPacket.h"

#include "mc\world\actor\state\ActorPropertyUtils.h"
#include "mc\world\actor\state\PropertyComponent.h"
#include "mc\world\actor\state\PropertyContainer.h"
#include "mc\world\level\dimension\ChunkBuildOrderPolicyBase.h"
#include "mc\world\level\dimension\ChunkBuildOrderPolicy.h"

#include "mc\world\level\dimension\DimensionDataSerializer.h"
#include "mc\world\level\dimension\VanillaDimensionFactory.h"
#include "mc\world\level\dimension\DimensionDefinitionGroup.h"

#include "mc\world\level\chunk\ChunkLoadActionList.h"
#include "mc\world\level\chunk\ChunkLoadedRequest.h"

#include "mc\world\level\chunk\ChunksLoadedInfo.h"
#include "mc\world\level\chunk\DeserializedChunkLoadedRequest.h"
#include "mc\world\level\chunk\ChunkLoadedRequest.h"

#include "mc\world\level\chunk\ChunkPerformanceData.h"
#include "mc\world\level\chunk\ChunkRecyclerTelemetryData.h"
namespace CauldronZero {
/*
LL_AUTO_TYPE_INSTANCE_HOOK(
    hookadsasd,
    HookPriority::Normal,
    ChunkLoadActionList,
    &ChunkLoadActionList::_addChunkLoadedRequest,
    void,
    ::ChunkLoadedRequest chunkLoadedRequest,
    ::std::string const& dimensionPrefix,
    ::ChunksLoadedStatus chunksLoadedStatus,
    ::LevelStorage&      levelStorage
) {
    ActorDimensionTransferProxy::$vftable();
    InventoryMenu::$vftableForContainerContentChangeListener();
    InventoryMenu::$vftableForIContainerManager();
}

LL_AUTO_TYPE_INSTANCE_HOOK(
    hookadsasds,
    HookPriority::Normal,
    ChunkRecyclerTelemetryData,
    &ChunkRecyclerTelemetryData::_reset,
    void,

) {
    PlayerUIContainer::$vftable();
}

LL_AUTO_TYPE_INSTANCE_HOOK(
    asdsad,
    HookPriority::Normal,
    ChunkBuildOrderPolicy,
    &ChunkBuildOrderPolicy::$updateInfluences,
    void,

) {}

LL_AUTO_STATIC_HOOK(
    AS,
    HookPriority::Normal,
    ChunksLoadedInfo::areAllChunksLoadedAndTicking,
    ChunksLoadedStatus,
    ::Dimension const& dimension,
    ::ChunkSource&     source,
    ::Bounds const&    bounds,
    bool               isCircleArea,
    ::Tick             currentLevelTick,
    bool               allowNonTickingPlayerAndTickingAreaChunks
){
    return origin(dimension,source,bounds,isCircleArea,currentLevelTick,allowNonTickingPlayerAndTickingAreaChunks);
}


LL_AUTO_TYPE_INSTANCE_HOOK(
    hook45,
    ll::memory::HookPriority::Normal,
    SetActorDataPacket,
    &SetActorDataPacket::$write,
    void,
    ::BinaryStream& stream

) {
    auto a = stream.mBuffer;
    logger.info("MobEffectPacket::$write Hooked! Buffer size: {}", a);
    return origin(stream);
    logger.info("MobEffectPacket::$write : {}", a);
}
*/

} // namespace CauldronZero

