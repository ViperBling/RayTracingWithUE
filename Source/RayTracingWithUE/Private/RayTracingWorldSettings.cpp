#include "RayTracingWorldSettings.h"

#include "Editor.h"
#include "EditorLevelLibrary.h"
#include "Subsystems/EditorActorSubsystem.h"

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
						UE_LOG(LogTemp, Warning, TEXT("Vertex Location: %s"), *VertexLocationWS.ToString());

						auto Normal = SMVertexBuffer.VertexTangentZ(Index);
						// Do something with the vertex
					}
				}
			}
		}
	}
	
}
