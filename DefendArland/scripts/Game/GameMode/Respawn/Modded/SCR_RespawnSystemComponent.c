modded class SCR_RespawnSystemComponent
{
	protected static ref HintHelpers hint = null;
	protected DefendManager dm = null;
	
	protected void SetHelpers()
	{
		if (dm == null) dm = DefendHelpers.Get();
		if (dm != null)
		{
			if (hint == null) hint = dm.hint;
			if (hint == null)
			{
				DefendHelpers.Log("ERROR", "Was unable to find hint helpers!");
			}
		}
		else DefendHelpers.Log("Not Defend", "Couldn't find Defend Manager so not in Game Mode.");
	}
	
	override void OnPlayerSpawnFinalize_S(SCR_SpawnRequestComponent requestComponent, SCR_SpawnHandlerComponent handlerComponent, SCR_SpawnData data, IEntity entity)
	{
		super.OnPlayerSpawnFinalize_S(requestComponent, handlerComponent, data, entity);
		
		int id = requestComponent.GetPlayerId();
		SetHelpers();
		
		if (dm != null)
		{
			dm.SetPlayerState(id, true);
		
			int livesLeft = DefendHelpers.Get().livesLeft;
			bool isFirstTime = !dm.activePlayerIds.Contains(id);
			
			if (isFirstTime)
			{
				DefendHelpers.Log("Spawned Player", "Player with the ID: " + id + " has been spawned for the first time.");
				
				dm.activePlayerIds.Insert(id);
				
				GetGame().GetCallqueue().Call(PlayerSpawned, requestComponent.GetPlayerId(), isFirstTime, dm.livesLeft);
			}
			else
			{
				DefendHelpers.Log("Respawned Player", "Player with the ID: " + id + " has been respawned");
				GetGame().GetCallqueue().Call(PlayerSpawned, requestComponent.GetPlayerId(), isFirstTime, dm.livesLeft);
			}			
		}
	}
	
	override bool PreparePlayerEntity_S(SCR_SpawnRequestComponent requestComponent, SCR_SpawnHandlerComponent handlerComponent, SCR_SpawnData data, IEntity entity)
	{
		DefendHelpers.Log("Spawn Protection", "Spawn protecting entity");
		SetHelpers();
		
		if (dm.debugMode && !dm.allowPlayerDamage)
			GetGame().GetCallqueue().Call(DefendHelpers.PreventCharacterDamage, entity, -1, 0);
		else GetGame().GetCallqueue().Call(DefendHelpers.PreventCharacterDamage, entity, 20, 0);
		
		return super.PreparePlayerEntity_S(requestComponent, handlerComponent, data, entity);
	}
	
	
	
	override void OnPlayerRegistered_S(int playerId)
	{
		super.OnPlayerRegistered_S(playerId);
	}
	
	override void OnPlayerKilled_S(int playerId, IEntity playerEntity, IEntity killerEntity, notnull Instigator killer)
	{
		super.OnPlayerKilled_S(playerId, playerEntity, killerEntity, killer);
		
		DefendHelpers.Get().SetPlayerState(playerId, false);
	}
	
	void PlayerSpawned(int playerId, bool first, int livesLeft = 0)
	{
		DefendHelpers.Log("Sending PlayerSpawned", "Player " + playerId + " spawned, firstTime: " + first);
		if (DefendHelpers.IsHost()) GetGame().GetCallqueue().Call(CheckPlayerSpawn, playerId, first, livesLeft);

		Rpc(RpcDo_PlayerSpawned, playerId, first, livesLeft);
	}
	
	
	[RplRpc(RplChannel.Reliable, RplRcver.Broadcast)]	
	protected void RpcDo_PlayerSpawned(int playerId, bool first, int livesLeft) 
	{
		DefendHelpers.Log("RpcDo: Player Spawned", playerId.ToString() + " | " + first);

		GetGame().GetCallqueue().Call(CheckPlayerSpawn, playerId, first, livesLeft);
	}
	
	void CheckPlayerSpawn(int playerId, bool first, int livesLeft)
	{
		SetHelpers();
		
		PlayerController controller = GetGame().GetPlayerController();
		SetHelpers();
			
		if (controller != null)
		{
			int currPlayerId = controller.GetPlayerId();
			bool isMe = playerId == currPlayerId;
			
			if (isMe)
			{
				if (first)
				{
					GetGame().GetCallqueue().CallLater(ShowIntro1, 5000, false, 10);
					dm.hud.ShowHUD(0,0,0, customText:"STARTING GAME...");
				}
				else 
				{
					hint.ShowHint(livesLeft.ToString() + " lives left", "You've just been respawned, you have a few seconds of spawn protection which will prevent you from dying again immediately but be more careful as all players only have " + livesLeft + " lives left until the mission is failed.", 10, dm.localPlayerId, isDebug:dm.hintLogging);	
				}
			}	
			else
			{
				DefendHelpers.Log("Ignore spawned message","Player isn't me, I'm ID: " + currPlayerId);
			}
		}
	}
	
	
	protected void ShowIntro1(int secondsToDisplay)
	{
		SetHelpers();
		hint.ShowHint("Prepare to defend.", "Soon russians will try to attack our position, they have us surrounded... Gear up and get ready to fight for our lives.", secondsToDisplay, dm.localPlayerId, isDebug:dm.hintLogging);	
		GetGame().GetCallqueue().CallLater(ShowIntro2, secondsToDisplay * 1000, false, secondsToDisplay);
	}
	
	protected void ShowIntro2(int secondsToDisplay)
	{
		if (dm.endlessMode)
		{
			hint.ShowHint("Survive Forever...", "We have to hold this position for as long as we can!", secondsToDisplay, dm.localPlayerId, isDebug:dm.hintLogging);
		}
		else
		{
			hint.ShowHint("Survive " + dm.GetNumberOfWaves() + " Waves...", "Backup is arriving soon, we just need to hold them off for a litte while longer!", secondsToDisplay, dm.localPlayerId, isDebug:dm.hintLogging);
		}
	}
}
