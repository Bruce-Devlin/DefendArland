class AttachToParentClass : GenericEntityClass
{
}

class AttachToParent : GenericEntity
{
	const static string HEADER = "Attach Configuration";
	[Attribute(defvalue: "1", category: HEADER)]
	bool m_bRotateWithTarget;
	[Attribute(defvalue: "", category: HEADER)]
	string m_sParentName;
	
	private IEntity m_Target;

	//------------------------------------------------------------------------------------------------
	//!
	//! \param[in] target
	void AttachTo(IEntity target)
	{		
		if (!target)
		{
			Print("Error attaching to parent, parent was NULL", LogLevel.ERROR);
			return;	
		}
		
		m_Target = target;
		
		SetEventMask(EntityEvent.POSTFRAME);
		SetFlags(EntityFlags.ACTIVE);
	}
	
	override void EOnActivate(IEntity owner)
	{
		GetGame().GetCallqueue().CallLater(TimeToRenameParent, 3 * 1000)
	}
	
	void TimeToRenameParent()
	{
		IEntity parent = GetGame().GetWorld().FindEntityByName(m_sParentName);
		if (parent == null)
			Print("ERROR - COULDN'T FIND PARENT WITH NAME: " + m_sParentName);
		else
			AttachTo(parent);
	}

	override void EOnPostFrame(IEntity owner, float timeSlice)
	{
		if (!m_Target)
			return;
		
		if (m_bRotateWithTarget)
		{
			vector transform[4];
			m_Target.GetWorldTransform(transform);
			SetLocalTransform(transform);
		}
		
		SetOrigin(m_Target.GetWorldTransformAxis(3));
	}
}