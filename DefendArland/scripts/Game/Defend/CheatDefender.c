class CheatDefender
{
	static DefendManager dm;
	static bool debugMode = false;

	static void Init(DefendManager initDM)
	{
		if (initDM == null)
			return;

		dm = initDM;
		DefendHelpers.Log("Starting CheatDefender", "CheatDefending is currently starting...");

		debugMode = dm.debugMode;
		GetGame().GetCallqueue().CallLater(CheatDefenderLoop, 30000, true);
		DefendHelpers.Log("CheatDefender Started!", "Tasks are scheduled.");
	}

	static void Shutdown()
	{
		GetGame().GetCallqueue().Remove(CheatDefenderLoop);
		dm = null;
		debugMode = false;
	}

	static void CheatDefenderLoop()
	{
		Task_CheckForGodMode();
	}

	static void Task_CheckForGodMode()
	{
		if (dm == null)
			return;

		if (debugMode) DefendHelpers.Log("CheatDefender Loop", "CheatDefender is checking for any cheaters...");
		if (DefendHelpers.IsHost())
		{
			ref array<DefendPlayer> players = dm.GetAlivePlayers();
			foreach (DefendPlayer player : players)
			{
				if (dm.allowPlayerDamage && player != null) DefendHelpers.AllowCharacterDamage(player.GetEntity());
			}
		}
		else
		{
			ref DefendPlayer myPlayer = dm.GetPlayer(dm.localPlayerId);
			if (dm.allowPlayerDamage && myPlayer != null) DefendHelpers.AllowCharacterDamage(myPlayer.GetEntity());
		}
	}
}
