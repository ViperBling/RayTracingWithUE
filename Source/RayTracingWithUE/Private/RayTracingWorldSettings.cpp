#include "RayTracingWorldSettings.h"
#include "Editor.h"
#include "EditorLevelLibrary.h"
#include "RayTracingWorldSubSystem.h"
#include "Subsystems/EditorActorSubsystem.h"

ARayTracingWorldSettings::ARayTracingWorldSettings()
{
}

void ARayTracingWorldSettings::BeginPlay()
{
	Super::BeginPlay();

	const UWorld* World = GetWorld();
	if (World)
	{
		URayTracingWorldSubSystem* RayTracingWorldSubSystem = World->GetSubsystem<URayTracingWorldSubSystem>();
		if (RayTracingWorldSubSystem)
		{
			RayTracingWorldSubSystem->OnActorSpawned(this);
		}
	}
}

void ARayTracingWorldSettings::BeginDestroy()
{
	const UWorld* World = GetWorld();
	if (World)
	{
		URayTracingWorldSubSystem* RayTracingWorldSubSystem = World->GetSubsystem<URayTracingWorldSubSystem>();
		if (RayTracingWorldSubSystem)
		{
			RayTracingWorldSubSystem->OnActorDeleted(this);
		}
	}
	
	Super::BeginDestroy();
}

void ARayTracingWorldSettings::GatherSceneMeshData()
{
	UEditorActorSubsystem* EditorActorSubsystem = GEditor->GetEditorSubsystem<UEditorActorSubsystem>();
	TArray<AActor*> Actors = EditorActorSubsystem->GetAllLevelActors();

	for (auto& Actor : Actors)
	{
		TArray<UActorComponent*> Comps;
		Actor->GetComponents(UStaticMeshComponent::StaticClass(), Comps, true);

		for (auto& Comp : Comps)
		{
			UStaticMeshComponent* StaticMeshComponent = Cast<UStaticMeshComponent>(Comp);
			if (StaticMeshComponent)
			{
				UStaticMesh* StaticMesh = StaticMeshComponent->GetStaticMesh();

				if (StaticMesh)
				{
					auto& CurrentLOD = StaticMesh->GetRenderData()->LODResources[0];
					auto& VBuffers = CurrentLOD.VertexBuffers;
					auto& IBuffer = CurrentLOD.IndexBuffer;

					auto& PositionVertexBuffer = VBuffers.PositionVertexBuffer;
					auto& SMVertexBuffer = VBuffers.StaticMeshVertexBuffer;

					const int32 VertexCount = PositionVertexBuffer.GetNumVertices();
					for (int32 Index = 0; Index < VertexCount; Index++)
					{
						const FVector VertexLocationWS = GetActorLocation() + GetTransform().TransformVector(FVector(PositionVertexBuffer.VertexPosition(Index)));
						auto Normal = SMVertexBuffer.VertexTangentZ(Index);
						
						UE_LOG(LogTemp, Warning, TEXT("Vertex Location: %s"), *VertexLocationWS.ToString());
						UE_LOG(LogTemp, Warning, TEXT("Vertex Normal: %s"), *Normal.ToString());
						// Do something with the vertex
					}
				}
			}
		}
	}
}

void ARayTracingWorldSettings::AddRTSceneComponent(URTRenderingComponent* RTComponent)
{
    // auto Data = RTRenderData.FindByPredicate([&](const FRTMeshRenderData& InData)
    // {
    //     return InData.ComponentID == RTComponent->GetUniqueID();
    // });
    // if (!Data)
    // {
    //     FRTMeshRenderData NewData;
    //     NewData.ComponentID = RTComponent->GetUniqueID();
    //     NewData.Position = FVector3f(RTComponent->GetComponentLocation());
    //     NewData.Radius = RTComponent->Radius;
    //     NewData.Material = RTComponent->Material;
    //     RTRenderData.Add(NewData);
    // }
    // else
    // {
    //     Data->Position = FVector3f(RTComponent->GetComponentLocation());
    //     Data->Radius = RTComponent->Radius;
    //     Data->Material = RTComponent->Material;
    // }

    auto MeshInfo = RTMeshInfos.FindByPredicate([&](const FRTMeshInfo& InMeshInfo)
    {
        return InMeshInfo.ComponentID == RTComponent->GetUniqueID();
    });
    
    auto ComponentTransform = RTComponent->GetComponentTransform();
    auto StaticMesh = RTComponent->GetStaticMesh();
    ensure(StaticMesh);
    auto& IndexBuffer = StaticMesh->GetRenderData()->LODResources[0].IndexBuffer;
    auto& VertexPositionBuffer = StaticMesh->GetRenderData()->LODResources[0].VertexBuffers.PositionVertexBuffer;
    auto& VertexBuffer = StaticMesh->GetRenderData()->LODResources[0].VertexBuffers.StaticMeshVertexBuffer;
    
    if (!MeshInfo)
    {
        FRTMeshInfo NewInfo;
        NewInfo.ComponentID = RTComponent->GetUniqueID();
        NewInfo.FirstTriangleIdx = RTTriangles.Num();
        NewInfo.NumTriangles = StaticMesh->GetRenderData()->LODResources[0].IndexBuffer.GetNumIndices() / 3;
        NewInfo.BoundMin = FVector3f(ComponentTransform.TransformPosition(StaticMesh->GetBoundingBox().Min));
        NewInfo.BoundMax = FVector3f(ComponentTransform.TransformPosition(StaticMesh->GetBoundingBox().Max));
        for (uint32 i = 0; i < NewInfo.NumTriangles; i++)
        {
            const FIndexArrayView IndexArray = IndexBuffer.GetArrayView();
            int32 Index1 = IndexArray[i * 3 + 0];
            int32 Index2 = IndexArray[i * 3 + 1];
            int32 Index3 = IndexArray[i * 3 + 2];
            FRTTriangle NewTriangle;
            NewTriangle.PosA = FVector3f(ComponentTransform.TransformPosition(FVector(VertexPositionBuffer.VertexPosition(Index1))));
            NewTriangle.PosB = FVector3f(ComponentTransform.TransformPosition(FVector(VertexPositionBuffer.VertexPosition(Index2))));
            NewTriangle.PosC = FVector3f(ComponentTransform.TransformPosition(FVector(VertexPositionBuffer.VertexPosition(Index3))));
            NewTriangle.NormalA = FVector3f(ComponentTransform.TransformVector(FVector(VertexBuffer.VertexTangentZ(Index1))));
            NewTriangle.NormalB = FVector3f(ComponentTransform.TransformVector(FVector(VertexBuffer.VertexTangentZ(Index2))));
            NewTriangle.NormalC = FVector3f(ComponentTransform.TransformVector(FVector(VertexBuffer.VertexTangentZ(Index3))));
            RTTriangles.Add(NewTriangle);
        }
        NewInfo.Material = RTComponent->Material;
        RTMeshInfos.Add(NewInfo);
    }
    else
    {
        MeshInfo->BoundMin = FVector3f(ComponentTransform.TransformPosition(StaticMesh->GetBoundingBox().Min));
        MeshInfo->BoundMax = FVector3f(ComponentTransform.TransformPosition(StaticMesh->GetBoundingBox().Max));
        
        for (uint32 i = MeshInfo->FirstTriangleIdx; i < MeshInfo->NumTriangles; i++)
        {
            const FIndexArrayView IndexArray = IndexBuffer.GetArrayView();
            int32 Index1 = IndexArray[i * 3 + 0];
            int32 Index2 = IndexArray[i * 3 + 1];
            int32 Index3 = IndexArray[i * 3 + 2];
            RTTriangles[i].PosA = FVector3f(ComponentTransform.TransformPosition(FVector(VertexPositionBuffer.VertexPosition(Index1))));
            RTTriangles[i].PosB = FVector3f(ComponentTransform.TransformPosition(FVector(VertexPositionBuffer.VertexPosition(Index2))));
            RTTriangles[i].PosC = FVector3f(ComponentTransform.TransformPosition(FVector(VertexPositionBuffer.VertexPosition(Index3))));
            RTTriangles[i].NormalA = FVector3f(ComponentTransform.TransformVector(FVector(VertexBuffer.VertexTangentZ(Index1))));
            RTTriangles[i].NormalB = FVector3f(ComponentTransform.TransformVector(FVector(VertexBuffer.VertexTangentZ(Index2))));
            RTTriangles[i].NormalC = FVector3f(ComponentTransform.TransformVector(FVector(VertexBuffer.VertexTangentZ(Index3))));
        }
        MeshInfo->Material = RTComponent->Material;
    }
}

void ARayTracingWorldSettings::RemoveRTSceneComponent(URTRenderingComponent* RTComponent)
{
    // auto RenderData = RTRenderData.FindByPredicate([&](const FRTMeshRenderData& InData)
    // {
    //     return InData.ComponentID == RTComponent->GetUniqueID();
    // });
    // if (RenderData)
    // {
    //     RTRenderData.RemoveAt(RTRenderData.Find(*RenderData));
    // }
    auto MeshInfo = RTMeshInfos.FindByPredicate([&](const FRTMeshInfo& InMeshInfo)
    {
        return InMeshInfo.ComponentID == RTComponent->GetUniqueID();
    });
    if (MeshInfo)
    {
        RTTriangles.RemoveAt(MeshInfo->FirstTriangleIdx, MeshInfo->NumTriangles);

        for (int32 i = 0; i < RTMeshInfos.Num(); i++)
        {
            if (RTMeshInfos[i].FirstTriangleIdx > MeshInfo->FirstTriangleIdx)
            {
                RTMeshInfos[i].FirstTriangleIdx -= MeshInfo->NumTriangles;
            }
        }
        RTMeshInfos.RemoveAt(RTMeshInfos.Find(*MeshInfo));
    }
}

void ARayTracingWorldSettings::UpdateRTSceneComponent(URTRenderingComponent* RTComponent)
{
    // auto Data = RTRenderData.FindByPredicate([&](const FRTMeshRenderData& InData)
    // {
    //     return InData.ComponentID == RTComponent->GetUniqueID();;
    // });
    //
    // if (Data)
    // {
    //     Data->Position = FVector3f(RTComponent->GetComponentLocation());
    //     Data->Radius = RTComponent->Radius;
    //     Data->Material = RTComponent->Material;
    // }

    auto MeshInfo = RTMeshInfos.FindByPredicate([&](const FRTMeshInfo& InMeshInfo)
    {
        return InMeshInfo.ComponentID == RTComponent->GetUniqueID();
    });

    auto ComponentTransform = RTComponent->GetComponentTransform();
    auto StaticMesh = RTComponent->GetStaticMesh();
    ensure(StaticMesh);
    auto& IndexBuffer = StaticMesh->GetRenderData()->LODResources[0].IndexBuffer;
    auto& VertexPositionBuffer = StaticMesh->GetRenderData()->LODResources[0].VertexBuffers.PositionVertexBuffer;
    auto& VertexBuffer = StaticMesh->GetRenderData()->LODResources[0].VertexBuffers.StaticMeshVertexBuffer;
    
    if (MeshInfo)
    {
        MeshInfo->BoundMin = FVector3f(ComponentTransform.TransformPosition(StaticMesh->GetBoundingBox().Min));
        MeshInfo->BoundMax = FVector3f(ComponentTransform.TransformPosition(StaticMesh->GetBoundingBox().Max));
        
        for (uint32 i = MeshInfo->FirstTriangleIdx; i < MeshInfo->NumTriangles; i++)
        {
            ;
            const FIndexArrayView IndexArray = IndexBuffer.GetArrayView();
            int32 Index1 = IndexArray[i * 3 + 0];
            int32 Index2 = IndexArray[i * 3 + 1];
            int32 Index3 = IndexArray[i * 3 + 2];
            RTTriangles[i].PosA = FVector3f(ComponentTransform.TransformPosition(FVector(VertexPositionBuffer.VertexPosition(Index1))));
            RTTriangles[i].PosB = FVector3f(ComponentTransform.TransformPosition(FVector(VertexPositionBuffer.VertexPosition(Index2))));
            RTTriangles[i].PosC = FVector3f(ComponentTransform.TransformPosition(FVector(VertexPositionBuffer.VertexPosition(Index3))));
            RTTriangles[i].NormalA = FVector3f(ComponentTransform.TransformVector(FVector(VertexBuffer.VertexTangentZ(Index1))));
            RTTriangles[i].NormalB = FVector3f(ComponentTransform.TransformVector(FVector(VertexBuffer.VertexTangentZ(Index2))));
            RTTriangles[i].NormalC = FVector3f(ComponentTransform.TransformVector(FVector(VertexBuffer.VertexTangentZ(Index3))));
        }
        MeshInfo->Material = RTComponent->Material;
    }
}
