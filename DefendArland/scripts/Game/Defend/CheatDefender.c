class CheatDefender
{
	static DefendManager dm;
	static bool debugMode = false;
	
	static void Init(DefendManager dm)
	{
		this.dm = dm;
		DefendHelpers.Log("Starting CheatDefender", "CheatDefending is currently starting...");
		
		debugMode = dm.debugMode;
		GetGame().GetCallqueue().CallLater(CheatDefenderLoop, 10000, true);
		DefendHelpers.Log("CheatDefender Started!", "Tasks are scheduled.");
	}
	
	static void CheatDefenderLoop()
	{
		Task_CheckForGodMode();
	}
	
	static void Task_CheckForGodMode()
	{
		if (debugMode) DefendHelpers.Log("CheatDefender Loop", "CheatDefender is checking for any cheaters...");
		if (DefendHelpers.IsHost())
		{
			ref array<DefendPlayer> players = dm.GetAlivePlayers();
			foreach (DefendPlayer player : players)
			{
				if (dm.allowPlayerDamage) DefendHelpers.AllowCharacterDamage(player.GetEntity());
			}
		}
		else
		{
			ref DefendPlayer myPlayer = dm.GetPlayer(dm.localPlayerId);
			if (dm.allowPlayerDamage) DefendHelpers.AllowCharacterDamage(myPlayer.GetEntity());
		}
	}
}