modded class Vehicle
{
	int CountOccupants()
	{
		if (IsOccupied())
		{
			return m_pFactionComponent.CountTotalOccupants();
		}
		else return 0;
	}
	
	int CountPlayerOccupants()
	{
		if (IsOccupied())
		{
			return m_pFactionComponent.CountPlayerOccupants();
		}
		else return 0;
	}
	
	int CountAliveOccupants()
	{
		if (IsOccupied())
		{
			return m_pFactionComponent.CountAliveOccupants();
		}
		else return 0;
	}
	
	array<IEntity> GetAllOccupants()
	{
		array<IEntity> occupants = {};
		if (IsOccupied())
		{
			occupants = m_pFactionComponent.GetAllOccupants();
		}
		
		return occupants;
	}
}