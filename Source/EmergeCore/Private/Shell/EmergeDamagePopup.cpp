#include "Shell/EmergeDamagePopup.h"

float UEmergeDamagePopup::PopupScale(float Damage, float RefDamage)
{
	return FMath::Clamp(Damage / RefDamage, 0.5f, 2.0f);
}

int32 UEmergeDamagePopup::SeverityColor(float Damage, float RefDamage)
{
	const float f = Damage / RefDamage;
	if (f < 0.5f) return 0;
	if (f < 1.0f) return 1;
	if (f < 2.0f) return 2;
	return 3;
}

bool UEmergeDamagePopup::IsCrit(float Damage, float RefDamage)
{
	return Damage >= RefDamage * 2.0f;
}