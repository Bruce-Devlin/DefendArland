modded class SCR_CharacterControllerComponent
{
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