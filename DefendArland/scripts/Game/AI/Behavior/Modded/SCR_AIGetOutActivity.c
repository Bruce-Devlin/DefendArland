modded class SCR_AIGetOutActivity
{
	void SCR_AIGetOutActivity(SCR_AIGroupUtilityComponent utility, AIWaypoint relatedWaypoint, IEntity vehicle, SCR_AIBoardingParameters boardingParameters = null, float priority = PRIORITY_ACTIVITY_GET_OUT, float priorityLevel = PRIORITY_LEVEL_NORMAL)
	{
		DefendManager dm = DefendHelpers.Get();
		if (dm != null)
		{
			DefendHelpers.Log("AI Getting out", "An AI Unit is attempting to out of a vehicle");
			if (vehicle.GetName() != "Extraction")
			{
				DefendHelpers.Log("AI Getting out", "An AI Unit is getting out of a vehicle");
				InitParameters(vehicle, boardingParameters, priorityLevel);
		
				m_sBehaviorTree = "AI/BehaviorTrees/Chimera/Group/ActivityGetOut.bt";
				SetPriority(priority);
			}
		}
		else
		{
			DefendHelpers.Log("No DM for AI Get Out", "No Defend Manager present, allowing AI Get Out");

			InitParameters(vehicle, boardingParameters, priorityLevel);
		
			m_sBehaviorTree = "AI/BehaviorTrees/Chimera/Group/ActivityGetOut.bt";
			SetPriority(priority);
		}
	}
}