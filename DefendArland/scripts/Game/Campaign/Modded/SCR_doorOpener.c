modded class SCR_DoorOpenerEntity
{
	[Attribute(defvalue: "0", desc: "Jam the door open, preventing players from closing it again?")]
	protected bool m_fJamDoorOpen;
	
	override void EOnInit(IEntity owner)
	{
		// server only
		RplComponent rplComponent = RplComponent.Cast(owner.FindComponent(RplComponent));
		if (rplComponent && !rplComponent.IsMaster())
			return;
		
		// capture doors around
		m_aQueriedDoors = {};
		GetDoors(m_fRadius);
		// set state
		foreach (DoorStruct door : m_aQueriedDoors)
		{
			door.component.SetControlValue(m_fControlValue);
			
			if (m_fJamDoorOpen)
				JamDoor(door);
		}
		
		// cleanup
		m_aQueriedDoors.Clear();
		m_aQueriedDoors = null;
		
		// destroy self
		delete owner;
	}
	
	protected void JamDoor(DoorStruct door)
	{
		DefendManager dm = DefendHelpers.Get();
		if (dm != null)
		{
			dm.jammedDoors.Insert(door.component);
			door.owner.SetName("JammedDoor");
		}
	}
}