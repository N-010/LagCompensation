#include "Data/ProjectileData.h"
#include "Data/Types.h"

bool FProjectileData::IsValidProjectileData(const FProjectileData& ProjectileData)
{
	return ProjectileData.StartPosition
		!= LagCompensationTypes::InvalidLocation && ProjectileData.Velocity != LagCompensationTypes::InvalidVelocity /*&&
		ProjectileData.ProjectileShapeData.IsValidShapeData()*/;
}
