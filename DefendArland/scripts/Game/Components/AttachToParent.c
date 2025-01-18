class AttachToParentClass : GenericEntityClass
{
}

class AttachToParent : GenericEntity
{
	const static string HEADER = "Attach Configuration";
	[Attribute(defvalue: "1", category: HEADER)]
	bool m_bRotateWithTarget;
	[Attribute(defvalue: "1", category: HEADER)]
	bool m_bMoveWithTarget;
	[Attribute(defvalue: "0", category: HEADER)]
	bool m_bSetChild;
	[Attribute(defvalue: "", category: HEADER)]
	string m_sParentName;
	[Attribute(category: HEADER)]
	vector m_sChildPosition;
	
	
	private IEntity m_Target;

	//------------------------------------------------------------------------------------------------
	//!
	//! \param[in] target
	void AttachTo(IEntity child, IEntity parent)
	{		
		if (!parent)
		{
			Print("Error attaching to parent, parent was NULL", LogLevel.ERROR);
			return;	
		}
		
		m_Target = parent;
		vector transform[4];
		
		if (m_bSetChild) 
		{
			parent.AddChild(child, -1);
			child.Update();
		}
		
		child.SetWorldTransform(m_sChildPosition);
		child.Update();

		SetEventMask(EntityEvent.POSTFRAME);
		SetFlags(EntityFlags.ACTIVE);
	}
	
	override void EOnActivate(IEntity owner)
	{
		if (m_sParentName != "")
			GetGame().GetCallqueue().CallLater(TimeToRenameParent, 3 * 1000, false, owner)
	}
	
	void TimeToRenameParent(IEntity owner)
	{
		IEntity parent = GetGame().GetWorld().FindEntityByName(m_sParentName);
		if (parent == null)
			Print("ERROR - COULDN'T FIND PARENT WITH NAME: " + m_sParentName);
		else
			AttachTo(owner, parent);
	}

	override void EOnPostFrame(IEntity owner, float timeSlice)
	{
		if (!m_Target)
			return;
		
		bool changeMade = false;
		
		if (m_bRotateWithTarget)
		{
			vector transform[4];
			m_Target.GetWorldTransform(transform);
			SetWorldTransform(transform);
			changeMade = true;
		}
		
		if (m_bMoveWithTarget)
		{
			SetOrigin(m_Target.GetWorldTransformAxis(3));
			changeMade = true;
		}
		
		
		if (changeMade) Update();
	}
}