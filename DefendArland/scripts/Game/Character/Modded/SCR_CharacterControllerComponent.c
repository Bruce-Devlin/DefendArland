modded class SCR_CharacterControllerComponent
{
	override void OnDeath(IEntity instigatorEntity, notnull Instigator instigator)
	{
		DefendManager dm = DefendHelpers.Get();
		if (dm != null)
		{
			dm.localPlayerAlive = false;
		}
		super.OnDeath(instigatorEntity, instigator);
	}

	override void OnConsciousnessChanged(bool conscious)
	{
		super.OnConsciousnessChanged(conscious);
		
		DefendManager dm = DefendHelpers.Get();	
		if (dm == null)
			return;
		
		if (GetLifeState() == ECharacterLifeState.INCAPACITATED)
		{
			DefendHelpers.Log("Nap time", "Sleeping on the job is bad for your health...");
			ForceDeath();
		}
	}
}