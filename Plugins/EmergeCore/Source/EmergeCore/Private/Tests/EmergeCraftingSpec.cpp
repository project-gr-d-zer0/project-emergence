// HUMAN-AUTHORED trusted acceptance spec (do not edit). Feature: hideout crafting math (knowledge curve).
// Deterministic crafting: can-craft check, craft time by complexity, and skill reducing craft time
// (the progression/"knowledge" differentiator). Pure logic, testable headless.
// CONTRACT — NEW class UEmergeCrafting : public UBlueprintFunctionLibrary
//   in Public/Economy/EmergeCrafting.h + Private/Economy/EmergeCrafting.cpp, static UFUNCTIONs:
//   static bool  CanCraft(int32 HaveQty, int32 NeedQty);       // return HaveQty >= NeedQty && NeedQty > 0
//   static float CraftTimeSeconds(int32 Complexity);           // return 5.0f + FMath::Clamp(Complexity,0,10)*10.0f
//   static float CraftTimeWithSkill(int32 Complexity, int32 SkillLevel);
//     // return CraftTimeSeconds(Complexity) * (1.0f - FMath::Clamp(SkillLevel,0,10) * 0.05f)
// Deterministic, exact golden values, boundaries covered.
#include "Misc/AutomationTest.h"
#if WITH_AUTOMATION_TESTS
#include "Economy/EmergeCrafting.h"

BEGIN_DEFINE_SPEC(FEmergeCraftingSpec, "Emergence.Economy.Crafting",
	EAutomationTestFlags::ProductFilter | EAutomationTestFlags::EditorContext)
END_DEFINE_SPEC(FEmergeCraftingSpec)
void FEmergeCraftingSpec::Define()
{
	Describe("CanCraft (enough inputs, and a positive requirement)", [this]()
	{
		It("only crafts with enough of a positive requirement", [this]()
		{
			TestTrue(TEXT("have 5 need 3 -> yes"),   UEmergeCrafting::CanCraft(5, 3));
			TestTrue(TEXT("have 3 need 3 -> yes"),   UEmergeCrafting::CanCraft(3, 3));
			TestFalse(TEXT("have 2 need 3 -> no"),   UEmergeCrafting::CanCraft(2, 3));
			TestFalse(TEXT("need 0 -> no"),          UEmergeCrafting::CanCraft(5, 0));
		});
	});

	Describe("CraftTimeSeconds (5 + clamp(complexity,0,10)*10)", [this]()
	{
		It("matches exact golden values and clamps", [this]()
		{
			TestEqual(TEXT("complexity 0 -> 5"),        UEmergeCrafting::CraftTimeSeconds(0), 5.f);
			TestEqual(TEXT("complexity 1 -> 15"),       UEmergeCrafting::CraftTimeSeconds(1), 15.f);
			TestEqual(TEXT("complexity 3 -> 35"),       UEmergeCrafting::CraftTimeSeconds(3), 35.f);
			TestEqual(TEXT("complexity 12 -> 105 (cap)"),UEmergeCrafting::CraftTimeSeconds(12), 105.f);
		});
	});

	Describe("CraftTimeWithSkill (skill shaves up to 50% off)", [this]()
	{
		It("matches exact golden values", [this]()
		{
			TestEqual(TEXT("cx1 skill0 -> 15"),      UEmergeCrafting::CraftTimeWithSkill(1, 0), 15.f);
			TestEqual(TEXT("cx1 skill10 -> 7.5"),    UEmergeCrafting::CraftTimeWithSkill(1, 10), 7.5f);
			TestEqual(TEXT("cx1 skill5 -> 11.25"),   UEmergeCrafting::CraftTimeWithSkill(1, 5), 11.25f);
			TestEqual(TEXT("cx0 skill10 -> 2.5"),    UEmergeCrafting::CraftTimeWithSkill(0, 10), 2.5f);
		});
	});
}
#endif
