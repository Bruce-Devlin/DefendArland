modded class SCR_AIGetOutActivity
{
	void SCR_AIGetOutActivity(SCR_AIGroupUtilityComponent utility, AIWaypoint relatedWaypoint, IEntity vehicle, SCR_AIBoardingParameters boardingParameters = null, float priority = PRIORITY_ACTIVITY_GET_OUT, float priorityLevel = PRIORITY_LEVEL_NORMAL)
	{
		DefendManager dm = DefendHelpers.Get();
		if (dm != null && vehicle != null && vehicle.GetName() == "Extraction")
		{
			DefendHelpers.Log("AI Get Out Blocked", "Preventing extraction AI from leaving the extraction vehicle.");
			InitParameters(vehicle, boardingParameters, priorityLevel);
			SetPriority(0);
			return;
		}

		InitParameters(vehicle, boardingParameters, priorityLevel);

		m_sBehaviorTree = "AI/BehaviorTrees/Chimera/Group/ActivityGetOut.bt";
		SetPriority(priority);
	}
}
