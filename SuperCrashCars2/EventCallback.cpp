#include "EventCallback.h"
#include "PVehicle.h"
#include "PowerUp.h"

void EventCallback::onConstraintBreak(PxConstraintInfo* constraints, PxU32 count) { PX_UNUSED(constraints); PX_UNUSED(count); }
void EventCallback::onWake(PxActor** actors, PxU32 count) { PX_UNUSED(actors); PX_UNUSED(count); }
void EventCallback::onSleep(PxActor** actors, PxU32 count) { PX_UNUSED(actors); PX_UNUSED(count); }
void EventCallback::onAdvance(const PxRigidBody* const*, const PxTransform*, const PxU32) {}

void EventCallback::onContact(const PxContactPairHeader& pairHeader, const PxContactPair* pairs, PxU32 nbPairs) {

	PxRigidDynamic* car0 = dynamic_cast<PxRigidDynamic*>(pairHeader.actors[0]);
	PxRigidDynamic* car1 = dynamic_cast<PxRigidDynamic*>(pairHeader.actors[1]);

	// car1 hits car 0!

	if (car0 && car1) {

		// figure out who hit who
		float car0Mag = car0->getLinearVelocity().magnitudeSquared();
		float car1Mag = car1->getLinearVelocity().magnitudeSquared();

		// the faster car is the attacker, the slower car is the victim
		PxRigidDynamic* attacker;
		PxRigidDynamic* victim;

		if (car0Mag > car1Mag) {
			attacker = car0;
			victim = car1;
		}  else {
			attacker = car1;
			victim = car0;
		}
		
		// getting naive launch vector
		PxVec3 attackerPos = attacker->getGlobalPose().p;
		PxVec3 victimPos = victim->getGlobalPose().p;
		PxVec3 launchVector = victimPos - attackerPos;
		launchVector = launchVector.getNormalized();

		PVehicle* victimVehicle = (PVehicle*)victim->userData;

		//car1->setLinearVelocity(car1->getLinearVelocity() / 10.f);
		//car0->addForce(launchVector * 300000, PxForceMode::eIMPULSE);
		float attackerMag = attacker->getLinearVelocity().magnitude();
		Log::debug("Attacker magnitude: {}", attackerMag);
		// launch formula: base 100k + 20k, multiplied by the collisionCoeff, and multiplied by a number from 1 to *around* 4 based on the magnitude of the velocity of the attacker.
		// *The max for the multiplier is not necessarily 4, but practically, the magnitudes of the cars rarely reach above 70 from my tests
		
		victimVehicle->getRigidDynamic()->addForce(PxVec3(launchVector * (100000.f + 20000 * victimVehicle->vehicleAttr.collisionCoefficient * (1.f + 3.f * attackerMag / 70.f))), PxForceMode::eIMPULSE);
		victimVehicle->vehicleAttr.collisionCoefficient = victimVehicle->vehicleAttr.collisionCoefficient + 0.5f;

	}
}

void EventCallback::onTrigger(PxTriggerPair* pairs, PxU32 count) { 

	PVehicle* vehicle = dynamic_cast<PVehicle*>((PVehicle*)(pairs->otherActor->userData));
	PowerUp* powerUp = dynamic_cast<PowerUp*>((PowerUp*)(pairs->triggerActor->userData));

	if (!(powerUp->getRigidStatic() && vehicle->getRigidDynamic())) return;

	switch (powerUp->getType()) {
		case PowerUpType::eBOOST:
		{
			vehicle->vehicleParams.boost *= 1.25;
			break;
		}
		case PowerUpType::eDAMAGE:
		{
			break;
		}
		case PowerUpType::eHEALTH:
		{
			break;
		}
	}

	powerUp->destroy();

}