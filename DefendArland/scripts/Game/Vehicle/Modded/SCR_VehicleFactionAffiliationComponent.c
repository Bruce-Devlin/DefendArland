modded class SCR_VehicleFactionAffiliationComponent
{
	protected ref array<IEntity> m_aOccupants;

	int CountTotalOccupants()
	{
		return super.m_iOccupantCount;
	}
	
	int CountAliveOccupants()
	{
		return super.m_iAliveOccupantCount;
	}
	
	array<IEntity> GetAllOccupants()
	{
		return m_aOccupants;
	}
	
	override protected void OnCompartmentEntered(IEntity vehicle, IEntity occupant, BaseCompartmentSlot compartment, bool move)
	{
		SCR_ChimeraCharacter character = SCR_ChimeraCharacter.Cast(occupant);
		if(!character)
			return;
		
		if (m_aOccupants == null) 
			m_aOccupants = {};
		
		if (!m_aOccupants.Contains(occupant)) m_aOccupants.Insert(occupant);
		super.OnCompartmentEntered(vehicle, occupant, compartment, move);
	}
	
	override protected void OnCompartmentLeft(IEntity vehicle, IEntity occupant, BaseCompartmentSlot compartment, bool move)
	{
		if (move) // moving only between compartments
			return;
		
		m_aOccupants.RemoveItem(occupant);
		
		super.OnCompartmentLeft(vehicle, occupant, compartment, move);
	}
	
	

}