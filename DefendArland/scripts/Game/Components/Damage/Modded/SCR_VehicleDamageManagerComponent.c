modded class SCR_VehicleDamageManagerComponent
{
	protected IEntity m_Owner;
	override void OnDamageStateChanged(EDamageState newState, EDamageState previousDamageState, bool isJIP)
	{
		DefendManager dm = DefendHelpers.Get();
		if (dm != null)
		{
			DefendHelpers.Log("Vehicle state changed", newState.ToString());
			if (newState == EDamageState.DESTROYED || newState == EDamageState.INTERMEDIARY)
			{
				DefendHelpers.Log("Vehicle Destroyed!", "A Vehicle has been destroyed!");
				dm.RemoveAIVehicle(m_Owner);
			}
		}

		super.OnDamageStateChanged(newState, previousDamageState, isJIP);
	}

	override void OnPostInit(IEntity owner)
	{
		m_Owner = owner;
		super.OnPostInit(owner);
	}
}
