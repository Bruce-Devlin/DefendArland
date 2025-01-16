modded class SCR_AIGoalReaction_GetOutVehicle
{
	override void PerformReaction(notnull SCR_AIUtilityComponent utility, SCR_AIMessageBase message)
	{
		DefendHelpers.Log("AI GOT OUT REACTION 1", "AI reacted by getting out vehicles.");
		super.PerformReaction(utility, message);
	}
	
	override void PerformReaction(notnull SCR_AIGroupUtilityComponent utility, SCR_AIMessageBase message)
	{
		DefendHelpers.Log("AI GOT OUT REACTION 2", "AI reacted by getting out vehicles.");
		super.PerformReaction(utility, message);
	}

}