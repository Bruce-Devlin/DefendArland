modded class SCR_RespawnSystemComponent
{
	protected ref HintHelpers hint = new HintHelpers();
	
	override void OnPlayerSpawnFinalize_S(SCR_SpawnRequestComponent requestComponent, SCR_SpawnHandlerComponent handlerComponent, SCR_SpawnData data, IEntity entity)
	{
		super.OnPlayerSpawnFinalize_S(requestComponent, handlerComponent, data, entity);
		
		int id = requestComponent.GetPlayerId();
		DefendManager dm = DefendHelpers.Get();
		
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
		
		GetGame().GetCallqueue().CallLater(dm.SendHUDUpdate, 1000, false, dm.currentWave, dm.hud.timerTimeLeft, id)
	}
	
	override bool PreparePlayerEntity_S(SCR_SpawnRequestComponent requestComponent, SCR_SpawnHandlerComponent handlerComponent, SCR_SpawnData data, IEntity entity)
	{
		DamageManagerComponent damageManager = DamageManagerComponent.Cast(entity.FindComponent(DamageManagerComponent));
		if (damageManager != null)
		{
			DefendHelpers.Log("Spawn Protection", "Spawn protecting entity");
			damageManager.EnableDamageHandling(false);
			GetGame().GetCallqueue().CallLater(DisableSafeSpawn, 10000, false, damageManager);
		}
		
		return super.PreparePlayerEntity_S(requestComponent, handlerComponent, data, entity);
	}
	
	protected void DisableSafeSpawn(DamageManagerComponent damageManager)
	{
		DefendHelpers.Log("Done Spawn Protection", "Done spawn protecting entity");
		damageManager.EnableDamageHandling(true);
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
		GetGame().GetCallqueue().Call(ShowIntro1, 10);

		
		Rpc(RpcDo_PlayerSpawned, playerId, first, livesLeft);
	}
	
	
	[RplRpc(RplChannel.Reliable, RplRcver.Broadcast)]	
	protected void RpcDo_PlayerSpawned(int playerId, bool first, int livesLeft) 
	{
		DefendHelpers.Log("RpcDo: Player Spawned", playerId.ToString() + " | " + first);
		PlayerController controller = GetGame().GetPlayerController();
		DefendManager dm = DefendHelpers.Get();
		
		if (controller != null)
		{
			int currPlayerId = controller.GetPlayerId();
			bool isMe = playerId == currPlayerId;
			
			if (isMe)
			{
				if (first)
				{
					GetGame().GetCallqueue().CallLater(ShowIntro1, 5000, false, 10);
				}
				else 
				{
					hint.ShowHint("Respawned", "You've just been respawned, you have a few seconds of spawn protection which will prevent you from dying again but be more careful as we only have " + livesLeft + " lives left until the mission is failed.", 10);	
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
		hint.ShowHint("Prepare to defend.", "Soon russians will try to attack our position, they have us surrounded... Gear up and get ready to fight for our lives.", secondsToDisplay);	
		GetGame().GetCallqueue().CallLater(ShowIntro2, secondsToDisplay * 1000, false, secondsToDisplay);
	}
	
	protected void ShowIntro2(int secondsToDisplay)
	{
		DefendManager dm = DefendHelpers.Get();
		if (dm.endlessMode)
		{
			hint.ShowHint("Survive Forever...", "We have to hold this position for as long as we can!");
		}
		else
		{
			hint.ShowHint("Survive " + dm.numberOfWaves + " Waves...", "Backup is arriving soon, we just need to hold them off for a litte while longer!");
		}
	}
}
