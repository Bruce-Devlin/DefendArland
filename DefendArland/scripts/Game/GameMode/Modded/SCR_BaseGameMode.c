modded class SCR_BaseGameMode: SCR_EditorBaseEntity
{
	override void OnGameStart()
	{
		super.OnGameStart();
	}

	override void OnPlayerConnected(int playerId)
	{
		super.OnPlayerConnected(playerId);
		DefendManager dm = DefendHelpers.Get();	
		if (dm == null)
			return;

		DefendHelpers.Log("Player Connected!", "Player id: " + playerId + " connected.");
		DefendPlayer newPlayer = new DefendPlayer();
		newPlayer.Create(playerId);
		dm.AddPlayer(newPlayer);
	}

	override void OnPlayerDisconnected(int playerId, int cause, int timeout)
	{
		super.OnPlayerDisconnected(playerId, cause, timeout);	
		
		DefendManager dm = DefendHelpers.Get();
		if (dm == null)
			return;
		
		DefendHelpers.Log("Player Disconnected!", "Player id: " + playerId + " disconnected.");
		dm.RemovePlayer(playerId);
	}
	
	override void OnControllableDestroyed(IEntity entity, IEntity killerEntity, notnull Instigator instigator)
	{	
		super.OnControllableDestroyed(entity, killerEntity, instigator);
		
		DefendManager dm = DefendHelpers.Get();
		if (dm == null)
			return;
		
		int killerId = GetGame().GetPlayerManager().GetPlayerIdFromControlledEntity(killerEntity);
		int deadId = GetGame().GetPlayerManager().GetPlayerIdFromControlledEntity(entity);

		
		if (killerId != 0 && deadId == 0)
		{
			DefendHelpers.Log("Player Killed AI", "Killer ID: " + killerId);

			dm.RegisterKill(killerId, 1);
			dm.RegisterAIDeath(entity);
		}
		else if (deadId != 0 && killerId == 0)
		{
			DefendHelpers.Log("AI Killed Player", "Dead ID: " + deadId);
			dm.RegisterDeath(deadId);
		}
		else if (killerId != 0 && deadId != 0)
		{
			DefendHelpers.Log("Player Killed Player", "Killer ID: " + killerId + " | Dead ID: " + deadId);
		}
	}
	
	override void OnGameEnd()
	{
		DefendManager dm = DefendHelpers.Get();
		if (dm != null)
		{
			dm.OnGameDestroyed();
		}
		
		super.OnGameEnd();		
	}
	
	override void OnPlayerRegistered(int playerId)
	{
		super.OnPlayerRegistered(playerId);

		DefendManager dm = DefendHelpers.Get();
		
		if (dm == null)
			return;
		
		if (!DefendHelpers.IsHost())
		{
			PlayerController localController = GetGame().GetPlayerController();
			
			if (localController == null) DefendHelpers.Log("ERROR", "Player local controller not found! (id:" + playerId + ")");
			
			if (localController.GetPlayerId() == playerId && !dm.localPlayerInitComplete)
			{
				DefendHelpers.Log("Player registered", "Player was registered and is me, running Init... (id:" + playerId + ")");
				GetGame().GetCallqueue().Call(dm.InitPlayerLocal, playerId)
			}
			else if (dm.localPlayerInitComplete)
			{
				DefendHelpers.Log("Player registered but not me", "Player was registered but is not me as I've already initilized! (id:" + playerId + ")");
			}
			else DefendHelpers.Log("Player registered but not me", "Player was registered but is not me. (my id:" + playerId + ", other id: " + localController.GetPlayerId() + ")");
		}
		else 
		{
			DefendHelpers.Log("Player registered on server", "Player was registered but is not me as I'm the server! (id:" + playerId + ")");
		}		
	}
}