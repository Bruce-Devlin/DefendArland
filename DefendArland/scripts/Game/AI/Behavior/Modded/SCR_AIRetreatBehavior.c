modded class SCR_AIRetreatWhileLookAtBehavior
{
	ref SCR_BTParam<bool> m_IsZombie = new SCR_BTParam<bool>("IsZombie");


	void SCR_AIRetreatWhileLookAtBehavior(SCR_AIUtilityComponent utility, SCR_AIActivityBase groupActivity, float priorityLevel = PRIORITY_LEVEL_NORMAL)
	{
		DefendManager dm = DefendHelpers.Get();
		if (dm != null)
			m_IsZombie.Init(this, dm.zombiesMode);
		else
			m_IsZombie.Init(this, false);
		 
		m_sBehaviorTree = "{9673533977BD9950}AI/BehaviorTrees/Chimera/Soldier/RetreatWhileLookAt.bt";
		SetPriority(PRIORITY_BEHAVIOR_RETREAT_MELEE);
		m_fPriorityLevel.m_Value = priorityLevel;
	}
}

modded class SCR_AIRetreatFromTargetBehavior
{
	ref SCR_BTParam<bool> m_IsZombie = new SCR_BTParam<bool>("IsZombie");
	
	void SCR_AIRetreatFromTargetBehavior(SCR_AIUtilityComponent utility, SCR_AIActivityBase groupActivity, notnull BaseTarget retreatFromTarget, float priorityLevel = PRIORITY_LEVEL_NORMAL)
	{				
		DefendManager dm = DefendHelpers.Get();

		if (dm != null)
		{
			m_IsZombie.Init(this, dm.zombiesMode);
		}
		else
		{
			m_IsZombie.Init(this, false);
		}

		
		m_sBehaviorTree = "{91B8D5FDB60C1C80}AI/BehaviorTrees/Chimera/Soldier/RetreatFromTarget.bt";
		SetPriority(PRIORITY_BEHAVIOR_RETREAT_FROM_TARGET);
		m_fPriorityLevel.m_Value = priorityLevel;
	}
}