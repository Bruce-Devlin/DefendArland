modded class SCR_VehicleDamageManagerComponent
{
	protected IEntity m_Owner;
	override void OnDamageStateChanged(EDamageState state)
	{
		DefendManager dm = DefendHelpers.Get();
		if (dm != null)
		{
			DefendHelpers.Log("Vehicle state changed", state.ToString());
			if (state == EDamageState.DESTROYED || state == EDamageState.INTERMEDIARY)
			{
				DefendHelpers.Log("Vehicle Destroyed!", "A Vehicle has been destroyed!");
				if (dm.activeAIVehicles.Contains(m_Owner)) dm.activeAIVehicles.RemoveItem(m_Owner);
			}
		}
		
		super.OnDamageStateChanged(state);
	}
	
	override void OnPostInit(IEntity owner)
	{
		m_Owner = owner;
		super.OnPostInit(owner);
	}
}