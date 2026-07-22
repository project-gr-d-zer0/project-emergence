#include "Misc/AutomationTest.h"
#if WITH_AUTOMATION_TESTS
#include "Sensor/EmergeSensorBucket.h"

BEGIN_DEFINE_SPEC(FEmergeSensorBucketSpec, "Emergence.Sensor.Bucket",
	EAutomationTestFlags::ProductFilter | EAutomationTestFlags::EditorContext)
END_DEFINE_SPEC(FEmergeSensorBucketSpec)
void FEmergeSensorBucketSpec::Define()
{
	Describe("BearingSector (8-way)", [this]()
	{
		It("matches exact golden values incl negatives", [this]()
		{
			TestEqual(TEXT("+X -> 0"), UEmergeSensorBucket::BearingSector(1.f, 0.f), 0);
			TestEqual(TEXT("+Y -> 2"), UEmergeSensorBucket::BearingSector(0.f, 1.f), 2);
			TestEqual(TEXT("-X -> 4"), UEmergeSensorBucket::BearingSector(-1.f, 0.f), 4);
			TestEqual(TEXT("-Y -> 6"), UEmergeSensorBucket::BearingSector(0.f, -1.f), 6);
			TestEqual(TEXT("NE 45 -> 1"), UEmergeSensorBucket::BearingSector(1.f, 1.f), 1);
		});
	});
	Describe("DistanceBucket", [this]()
	{
		It("near/med/far boundaries", [this]()
		{
			TestEqual(TEXT("100 -> near 0"), UEmergeSensorBucket::DistanceBucket(100.f), 0);
			TestEqual(TEXT("300 -> med 1"), UEmergeSensorBucket::DistanceBucket(300.f), 1);
			TestEqual(TEXT("999 -> med 1"), UEmergeSensorBucket::DistanceBucket(999.f), 1);
			TestEqual(TEXT("1000 -> far 2"), UEmergeSensorBucket::DistanceBucket(1000.f), 2);
		});
	});
}
#endif
