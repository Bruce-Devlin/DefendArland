modded class SCR_XPHandlerComponent
{
	override void AwardXP(notnull PlayerController controller, SCR_EXPRewards rewardID, float multiplier = 1.0, bool volunteer = false, int customXP = 0)
	{
		Print("Adding XP to player...");
		DefendManager dm = DefendHelpers.Get();
		if (dm != null)
		{
			int id = GetGame().GetPlayerManager().GetPlayerIdFromControlledEntity(controller);
			dm.AddPlayerXP(id, GetXPRewardAmount(rewardID));
		}
		
		super.AwardXP(controller, rewardID, multiplier, volunteer, customXP);
	}
}