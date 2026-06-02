class VehiclePlayerOccupant
{
	IEntity entity = null;
	int playerId = 0;
	void VehiclePlayerOccupant(IEntity playerEntity, int playerID)
	{
		entity = playerEntity;
		playerId = playerID;
	}
}
modded class SCR_VehicleFactionAffiliationComponent
{
	protected ref array<IEntity> m_aOccupants = {};
	protected ref array<VehiclePlayerOccupant> m_aPlayerOccupants = {};

	int CountTotalOccupants()
	{
		if (!super.IsVehicleOccupied()) return 0;
		else return m_aOccupants.Count();
	}

	int CountPlayerOccupants()
	{
		if (!super.IsVehicleOccupied()) return 0;
		else return m_aPlayerOccupants.Count();
	}

	array<VehiclePlayerOccupant> GetPlayerOccupants()
	{
		return m_aPlayerOccupants;
	}

	int CountAliveOccupants()
	{
		if (!m_iAliveOccupantCount) return 0;
		else return m_iAliveOccupantCount;
	}

	array<IEntity> GetAllOccupants()
	{
		return m_aOccupants;
	}

	override protected void OnCompartmentEntered(IEntity vehicle, IEntity occupant, BaseCompartmentSlot compartment, bool move)
	{
		super.OnCompartmentEntered(vehicle, occupant, compartment, move);

		SCR_ChimeraCharacter character = SCR_ChimeraCharacter.Cast(occupant);
		if(!character)
			return;

		if (m_aOccupants == null)
			m_aOccupants = {};
		if (m_aPlayerOccupants == null)
			m_aPlayerOccupants = {};

		int playerId = GetGame().GetPlayerManager().GetPlayerIdFromControlledEntity(occupant);
		if (playerId != 0)
		{
			if (!m_aOccupants.Contains(occupant))
			{
				VehiclePlayerOccupant newPlayerOccupant = new VehiclePlayerOccupant(occupant, playerId);
				m_aPlayerOccupants.Insert(newPlayerOccupant);
			}
		}

		if (!m_aOccupants.Contains(occupant)) m_aOccupants.Insert(occupant);
	}

	override protected void OnCompartmentLeft(IEntity vehicle, IEntity occupant, BaseCompartmentSlot compartment, bool move)
	{
		super.OnCompartmentLeft(vehicle, occupant, compartment, move);

		if (move) // moving only between compartments
			return;

		if (m_aOccupants != null)
			m_aOccupants.RemoveItem(occupant);

		int playerId = GetGame().GetPlayerManager().GetPlayerIdFromControlledEntity(occupant);
		if (m_aPlayerOccupants != null)
		{
			for (int i = m_aPlayerOccupants.Count() - 1; i >= 0; i--)
			{
				VehiclePlayerOccupant playerOccupant = m_aPlayerOccupants[i];
				if (playerOccupant == null || playerOccupant.entity == occupant || playerOccupant.playerId == playerId)
					m_aPlayerOccupants.Remove(i);
			}
		}

	}
}
