modded class SCR_GetOutGroupCommand
{
	override bool Execute(IEntity cursorTarget, IEntity target, vector targetPosition, int playerID, bool isClient)
	{
		DefendManager dm = DefendHelpers.Get();
		if (dm != null)
		{
			DefendHelpers.Log("Preventing AI Dismount", "Preventing AI from ordering a dismount.");
			return false;
		}
		else return super.Execute(cursorTarget, target, targetPosition, playerID, isClient);
	}
}