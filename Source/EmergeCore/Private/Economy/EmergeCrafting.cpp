#include "Economy/EmergeCrafting.h"

bool UEmergeCrafting::CanCraft(int32 HaveQty, int32 NeedQty)
{
	return HaveQty >= NeedQty && NeedQty > 0;
}

float UEmergeCrafting::CraftTimeSeconds(int32 Complexity)
{
	return 5.0f + FMath::Clamp(Complexity, 0, 10) * 10.0f;
}

float UEmergeCrafting::CraftTimeWithSkill(int32 Complexity, int32 SkillLevel)
{
	return CraftTimeSeconds(Complexity) * (1.0f - FMath::Clamp(SkillLevel, 0, 10) * 0.05f);
}
