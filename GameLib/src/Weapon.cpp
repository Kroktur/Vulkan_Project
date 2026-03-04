#include "Weapon.h"

const KGR::GameLib::WeaponData& KGR::GameLib::WeaponComponent::GetCurrentWeaponData() const
{
	switch (current)
	{
	case WeaponType::Shotgun:
		return shotgun;
	case WeaponType::Auto:
		return autoRifle;
	case WeaponType::Sniper:
		return sniper;
	default:
		return autoRifle;
	}
}

void KGR::GameLib::WeaponComponent::SwitchWeapon(WeaponType newType)
{
	current = newType;
	currentAmmo = GetCurrentWeaponData().maxAmmo;
	cooldown = 0.0f;
}

void KGR::GameLib::WeaponComponent::StartReload()
{
	if (isReloading)
		return;

	isReloading = true;
	reloadTimer = GetCurrentWeaponData().reloadTime;
}

void KGR::GameLib::WeaponComponent::UpdateReload(float dt)
{
	if (!isReloading)
		return;

	reloadTimer -= dt;
	if (reloadTimer <= 0.0f)
	{
		isReloading = false;
		currentAmmo = GetCurrentWeaponData().maxAmmo;
	}
}