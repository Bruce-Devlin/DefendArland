class DefendManagerClass: GenericEntityClass
{
	
}

class DefendManager: GenericEntity
{
	//! Registers a player's death, updates their death count, and reduces lives left for the team.
	//! -------------------------------------
	//! Parameters:
	//! deadID - The ID of the player who has died.
	void RegisterDeath(int deadID)
	{
	    if (DefendHelpers.IsHost())
	    {
	        DefendPlayer player = GetPlayer(deadID);
	        player.AddDeaths(1);
			SetPlayerState(player.GetID(), false);
				    
	        DefendHelpers.Log("Player died", "Player " + player.GetName() + " (" + player.GetID().ToString() + ") died (total deaths: " + player.GetDeaths() + ", lives remaining: " + livesLeft + ")");
		}
	}
	
	//! Registers a player's kill, updates their kill count.
	//! -------------------------------------
	//! Parameters:
	//! killerID - The ID of the player who made the kill.
	//! kills - The number of kills the player achieved.
	void RegisterKill(int killerID, int kills)
	{
	    if (DefendHelpers.IsHost())
	    {
	        DefendPlayer player = GetPlayer(killerID);
	        player.AddKills(kills);
	    
	        DefendHelpers.Log("Player got a kill", "Player " + player.GetName() + " (" + player.GetID().ToString() + ") killed someone (total kills: " + player.GetKills() + " )");
	    }
	}
	
	//! Registers an AI's death, but currently does not perform any actions for AI death in this method.
	//! -------------------------------------
	void RegisterAIDeath(IEntity deadAI)
	{
	    if (DefendHelpers.IsHost())
	    {
	        // Empty for now
	    }
	}
	
	//! Updates a player's alive state and logs the change.
	//! -------------------------------------
	//! Parameters:
	//! playerID - The ID of the player whose state is being updated.
	//! alive - The new alive state of the player (true if alive, false if dead).
	void SetPlayerState(int playerID, bool alive)
	{
	    if (DefendHelpers.IsHost())
	    {
	        DefendPlayer player = GetPlayer(playerID);
	        DefendHelpers.Log("Player State Change", "Player " + player.GetName() + " is alive: " + alive + " (oldState: " + player.IsAlive() + ")");
	
	        player.SetAlive(alive);
	    }
	}
	
	//! Updates a AI's alive state and logs the change.
	//! -------------------------------------
	//! Parameters:
	//! spawnedGroup - The spawned Group of the player whose state is being updated.
	//! alive - The new alive state of the player (true if alive, false if dead).
	void SetAIState(SpawnedEnemyGroup spawnedGroup, SpawnedGroupState newStateToSet)
	{
	    if (DefendHelpers.IsHost())
	    {
			int spawnedGroupInd = activeAIGroups.Find(spawnedGroup);
			SpawnedEnemyGroup group = activeAIGroups[spawnedGroupInd];
			group.SetState(newStateToSet);
	    }
	}
	
	void AddPlayerXP(int playerId, int xpAmountToAdd)
	{
		/*
		DefendPlayer player = GetPlayer(playerId);
		player.AddXP(xpAmountToAdd);
		*/
	}
	
	//! Returns an array of all players that are currently alive.
	//! -------------------------------------
	//! Returns:
	//! A reference to an array of DefendPlayer objects representing all alive players.
	ref array<DefendPlayer> GetAlivePlayers()
	{
	    ref array<DefendPlayer> alivePlayers = {};
	    for (int i; i < players.Count(); i++)
	    {
	        DefendPlayer _currPlayer = players[i];
	        if (_currPlayer.IsAlive()) alivePlayers.Insert(players[i]);
	    }
	    return alivePlayers;
	}
	
	//! Adds a player to the game and logs the event.
	//! -------------------------------------
	//! Parameters:
	//! player - The DefendPlayer object representing the player to be added.
	void AddPlayer(DefendPlayer player)
	{
	    DefendHelpers.Log("Player joined", "Player " + player.GetName() + " joined (" + player.GetID() + ")");
	    players.Insert(player);
	}
	
	//! Removes a player from the game and logs the event.
	//! -------------------------------------
	//! Parameters:
	//! playerID - The ID of the player to be removed.
	void RemovePlayer(int playerID)
	{
	    for (int i; i < players.Count(); i++)
	    {
	        DefendPlayer _currPlayer = players[i];
	        if (_currPlayer.GetID() == playerID) 
	        {
	            players.Remove(i);
	            activePlayerIds.Remove(i);
	            
	            DefendHelpers.Log("Player left", "Player " + _currPlayer.GetName() + " left (" + _currPlayer.GetID() + ")");
	            DefendHelpers.Log("PlayerCount", "Players: " + players.Count() + " | Registered: " + activePlayerIds.Count());
	        }
	    }
	}
	
	//! Retrieves a player based on their unique ID.
	//! -------------------------------------
	//! Parameters:
	//! id - The ID of the player to retrieve.
	//! Returns:
	//! The DefendPlayer object corresponding to the given ID, or null if no such player exists.
	ref DefendPlayer GetPlayer(int id)
	{
	    DefendPlayer playerToReturn = null;
	    for (int i; i < players.Count(); i++)
	    {
	        DefendPlayer _currPlayer = players[i];
	        if (_currPlayer.GetID() == id) playerToReturn = players[i];
	    }
	    return playerToReturn;
	}
	
	//! Ends the game, logging the reason and notifying the game mode.
	//! -------------------------------------
	//! Parameters:
	//! type - The reason for ending the game.
	//! winnerId - The ID of the winning player or team.
	void EndGame(EGameOverTypes type, int winnerId = 1)
	{
	    currentWave = 0;
	    DefendHelpers.Log("End Game", "The game was ended due to: " + type);
	    gameStarted = false;
	    
	    SCR_BaseGameMode gameMode = SCR_BaseGameMode.Cast(GetGame().GetGameMode());
	    Faction faction = GetGame().GetFactionManager().GetFactionByKey("US");
	    int usIndex = GetGame().GetFactionManager().GetFactionIndex(faction);
	    
	    if (!gameMode.IsRunning())
	        return;
	
	    gameMode.EndGameMode(SCR_GameModeEndData.CreateSimple(type, winnerId, usIndex));
	}
	
	//! Activates the mission and initializes the game.
	//! -------------------------------------
	override void EOnActivate(IEntity owner)
	{
	    InitMission();
	}
	
	//! Cleans up resources when the game is destroyed.
	//! -------------------------------------
	void OnGameDestroyed()
	{   
	    hud.Destroy();
	    
	    currentWave = 0;
	    livesLeft = 0;
	    localPlayerId = 0;
	    isHost = false;
	    localPlayerInitComplete = false;
	    serverPlayerInitComplete = false;
	    activePlayerIds.Clear();
	    activeAIGroups.Clear();
	    SetCanBuild(true);    
	    
	    GetGame().GetCallqueue().Clear();
	    DefendHelpers.Log("Stopping Mission", "Mission has been destroyed.");
	}
	
	void PickLayout()
	{
		string layoutToUse;

		if (forcedLayoutName != "" && debugMode)
			layoutToUse = forcedLayoutName;
		else
		{
			int maxOptions = layoutLayerNames.Count();
			int chosen = DefendHelpers.GenerateRandom(0, maxOptions);
			DefendHelpers.Log("Picking Layout", "Options: " + (maxOptions+1) + " | Picked: " + chosen);
			layoutToUse = layoutLayerNames[chosen];
		}
			
		currentLayout = layoutToUse;


		foreach (string layoutName : layoutLayerNames)
		{
			if (layoutName != layoutToUse)
			{
				IEntity spawnEntity = GetGame().GetWorld().FindEntityByName(layoutName + "_us_spawnpoint");
				SCR_SpawnPoint spawnpoint = SCR_SpawnPoint.Cast(spawnEntity);
				spawnpoint.SetFactionKey("USSR");
			}
			else
			{
				IEntity spawnEntity = GetGame().GetWorld().FindEntityByName(layoutName + "_us_spawnpoint");
				SCR_SpawnPoint spawnpoint = SCR_SpawnPoint.Cast(spawnEntity);
				primaryLayoutSpawn = spawnpoint;
			}
		}
	}
	
	IEntity FindLayoutEntity(string name)
	{
		IEntity entity = GetGame().GetWorld().FindEntityByName(currentLayout + "_" + name);
		if (entity != null) return entity;
		else
		{
			DefendHelpers.Log("ERROR FINDING ITEM", "Couldn't find layout entity!");
			return null;
		}
	}
	
	void CheckMissionFlags()
	{
		EGameFlags flags = GetGame().GetGameFlags();
		if (flags >= 99)
		{
			debugMode = true;
		}
		
		if (flags >= 70)
		{
			zombiesMode = true;
			numberOfEnemiesPerWave = 8;
			allowBuildingTimeBetweenWaves = false;
			numberOfWaves = 5;
		}
		else if (flags >= 60)
		{
			endlessMode = true;
			numberOfLives = 10;
		}
		else
		{
			if (flags >= 55) numberOfWaves = 10;
			else if (flags >= 50) numberOfWaves = 5;
		}
		
		if (debugMode && debugQuickWaveTest)
		{
			secondsBeforeFirstWaveToPrepare = 5;
			allowPlayerDamage = false;
		}
		else if (debugMode && debugVehicleFeatureTest)
		{
			oddsOfVehicleSpawn = 1;
			secondsBeforeFirstWaveToPrepare = 5;
			allowPlayerDamage = false;
		}
		else if (debugMode && debugExtractionFeatureTest)
		{
			if (!endlessMode) debugStartingWave = numberOfWaves;
			secondsBeforeFirstWaveToPrepare = 5;
			allowPlayerDamage = false;
		}
	}
	
	//! Initializes the mission when the gamemode starts.
	//! -------------------------------------
	void InitMission()
	{
	    DefendHelpers.Log("Init Mission", "Init called for mission");
	            
	    if (DefendHelpers.IsHost())
	    {

	        DefendHelpers.Log("Is Host", "Is currently the game host.");
	        isHost = true;
	        localPlayerId = -1;
			CheckMissionFlags();

			
			PickLayout();
			DefendHelpers.Log("Picked Layout", "Will use: " + currentLayout);
	        
	        hud.Init(uiHUDLayout, timerLoopLogging);
	        
	        GetGame().GetCallqueue().Call(WaitForPlayers);
	        GetGame().GetCallqueue().CallLater(ServerUpdateLoop, serverLoopIntervalSeconds * 1000, true);
	        serverPlayerInitComplete = true;
	    }
	    else
	    {
			hud.Init(uiHUDLayout, timerLoopLogging);
	        DefendHelpers.Log("Not Host", "Not currently the game host.");
	    }
	    
	    GetGame().GetCallqueue().CallLater(PlayerUpdateLoop, playerLoopIntervalSeconds * 1000, true);
	}
	
	//! Initializes a player's data when they join the game.
	//! -------------------------------------
	//! Parameters:
	//! playerId - The ID of the player to initialize.
	void InitPlayerLocal(int playerId)
	{
	    DefendHelpers.Log("Init Player", "Init called for player id: " + playerId);
	    localPlayerId = playerId;
		hud.ShowHUD(0,0,0, customText:"STARTING GAME...");

	            
	    localPlayerInitComplete = true;
	}
	
	//! Handles the server-side game loop, managing game state transitions, AI behavior, and extraction process.
	//! This function is called on every server update to perform key game logic checks and actions.
	//! --------------------------------------------------------------
	protected void ServerUpdateLoop()
	{
	    // Only run the loop if the game has started and we are not waiting for players
	    if (!waitingForPlayers && gameStarted)
	    {
	        // Log if server loop debugging is enabled
	        if (serverLoopDebugLogging) DefendHelpers.Log("Running Server Loop", "Server loop running.");
	        
	        // Check if player lives have been exhausted, trigger the game over condition if so
	        if (livesLeft < 1)
	        {
				if (GetAlivePlayers().Count() < 1)
				{
					EndGame(EGameOverTypes.LOOSE, 1);
				}
				else if (!lastLife)
				{
					lastLife = true;
					SendHint("No More respawns remaining", "We've ran out of team respawns, if all remaining players die we will fail the mission!");
					primaryLayoutSpawn.SetSpawnPointEnabled_S(false);
				}
	        }
	
	        // If AI groups are active, check their status periodically
	        if (activeAIGroups.Count() > 0)
	        {
	            // Check if AI behavior needs to be checked and if not already queued, schedule it
	            if (!checkingAI && !checkAIQueued)
	            {
	                checkAIQueued = true;
	                GetGame().GetCallqueue().CallLater(CheckAI, aiLoopIntervalSeconds * 1000, false);        
	            }
	        }
	        // If no AI groups are alive, log the information
	        else if (serverLoopDebugLogging) DefendHelpers.Log("No AI Alive", "No AI are alive right now.");
	        
	        // If extraction has started but is not yet available, monitor distance to extraction point
	        if (extractionStarted && !extractionAvailable)
	        {
	            // Get the current AI group and calculate its distance to the extraction point
	            ChimeraAIGroup group = extractionGroup.GetGroup();
	            float distance = vector.Distance(group.GetOrigin(), extractionGroup.targetExtractionPoint);
	            DefendHelpers.Log("Extraction Inbound", "Extraction is " + distance.ToString() + " meters away.");
	            
	            // If the extraction is within 150 meters and not yet nearby, notify players
	            if (distance < 150 && !extractionNearby)
	            {
	                SendHint("Extraction Nearby!", "The extraction truck is in your area, you should see it any minute now...");
	                extractionNearby = true;
	            }
	            // If the extraction point is within 20 meters, mark it as available
	            else if (distance < 20)
	            {
	                SendHint("Extraction Available!", "The extraction truck just pulled up to the west of the airfield, get on it now or get left behind!");
	                array<PlayerManager> currPlayers = {};
	                foreach (DefendPlayer player : players)
	                {
	                    // Player waypoints or other extraction-related logic could be handled here
	                }
	                extractionAvailable = true;
	            }
	        }
	        // If extraction is available, handle waiting and extraction processes
	        else if (extractionAvailable && !extractionEnded)
	        {
	            // If extraction waiting has not been triggered, schedule warnings and final extraction logic
	            if (!extractionWaiting)
	            {
	                extractionWaiting = true;
	                GetGame().GetCallqueue().CallLater(ExtractionWaitWarning, (secondsUntilExtractionLeaves / 2) * 1000, false, secondsUntilExtractionLeaves);
	                GetGame().GetCallqueue().CallLater(ExtractionWaitDone, secondsUntilExtractionLeaves * 1000, false);
	            }
	            else
	            {
	                // Check if all players have boarded the extraction vehicle
	                int playersExtracted = extractionGroup.GetVehicle().CountPlayerOccupants();
	                if (playersExtracted == GetAlivePlayers().Count())
	                {
	                    DefendHelpers.Log("All In", "Everyone is in the extraction vehicle, time to go!");
	                    hint.ShowHint("All In!", "Everyone is in the Extraction Truck, it's now leaving!");
	                    // Remove any scheduled extraction wait calls and initiate extraction
	                    GetGame().GetCallqueue().Remove(ExtractionWaitWarning);
	                    GetGame().GetCallqueue().Remove(ExtractionWaitDone);
	                    GetGame().GetCallqueue().Call(ExtractionLeave);
	                    extractionEnded = true;
	                }
	                
	                // Process each occupant in the extraction vehicle and mark them for extraction
	                Vehicle extVeh = extractionGroup.GetVehicle();
	                foreach(IEntity entity : extVeh.GetAllOccupants())
	                {
	                    int id = GetGame().GetPlayerManager().GetPlayerIdFromControlledEntity(entity);
	                    
	                    // Skip processing if the player ID is invalid (e.g., null or non-player entity)
	                    if (id == 0)
	                        continue;
	                    
	                    // Retrieve player and mark them as having extracted
	                    DefendPlayer player = GetPlayer(id);
	                    player.Extract();
	                }
	            }
	        }
	        
	        // If deserters should be punished and extraction is not available, check for deserters
	        if (punishDeserters && !extractionAvailable) CheckForDeserters();
	    }
	}

	//! Warns players that the extraction vehicle is about to leave, providing the time remaining.
	//! This function is called when it's time to send a warning about the imminent departure of the extraction truck.
	//! --------------------------------------------------------------
	protected void ExtractionWaitWarning(int totalTimeWaiting)
	{
	    // Calculate the time left to wait (half of the total waiting time) and prepare a warning message
	    int timeLeftToWait = (totalTimeWaiting / 2);
	    hint.ShowHint("Extraction is nearly leaving!", "The extraction truck will leave in " + timeLeftToWait.ToString() + " seconds...");
	}
	
	
	//! Notifies players that the extraction vehicle is leaving, as the waiting time has expired.
	//! This function is called when the extraction wait time has ended and the truck is leaving.
	//! --------------------------------------------------------------
	protected void ExtractionWaitDone()
	{
	    // Log that the wait period is over and the extraction is leaving
	    DefendHelpers.Log("Done Waiting For Extraction", "Times up, extraction is leaving.");
	    
	    // Inform players that the extraction vehicle can't wait any longer
	    hint.ShowHint("Time is up!", "The extraction truck can't wait any longer and needs to leave!");
	    
	    // Trigger the function to initiate the extraction process
	    GetGame().GetCallqueue().Call(ExtractionLeave);
	}
	
	
	//! Handles the logic for when the extraction vehicle is leaving, including spawning waypoints for AI and triggering the game over.
	//! This function is called to manage the final steps of the extraction, including moving AI and finishing the game.
	//! --------------------------------------------------------------
	protected void ExtractionLeave()
	{
	    // Define the path for the AI's forced movement waypoint (where the AI should go)
	    string forcedMoveWaypointName = "{06E1B6EBD480C6E0}Prefabs/AI/Waypoints/AIWaypoint_ForcedMove.et";
	    
	    // Get the entity (e.g., a position or destination) that the extraction vehicle will move to
	    IEntity extractTo = FindLayoutEntity("us_extractTo");
	
	    // Generate and validate the resource necessary for spawning the forced movement waypoint
	    Resource resource = DefendHelpers.GenerateAndValidateResource(forcedMoveWaypointName);
		
		int playersExtracted = extractionGroup.GetVehicle().CountPlayerOccupants();
	    
	    // Log an error if the resource couldn't be loaded
	    if (resource == null)
	    {
	        DefendHelpers.Log("ERROR!", "Unable to load resource for extraction to", LogLevel.ERROR);
	        return;
	    }
	    
	    // Spawn the AI waypoint at the extraction destination, if the resource is valid
	    AIWaypoint waypoint = AIWaypoint.Cast(GetGame().SpawnEntityPrefab(resource, null, DefendHelpers.GenerateSpawnParamaters(extractTo.GetOrigin())));
	    
	    // Log an error if the waypoint couldn't be spawned
	    if (waypoint == null)
	    {
	        DefendHelpers.Log("ERROR!", "Unable to spawn waypoint!", LogLevel.ERROR);
	        return;
	    }
	    
	    // Add the spawned waypoint to the AI group's waypoints (to direct them to the extraction point)
	    extractionGroup.GetGroup().AddWaypoint(waypoint);
		
		DefendHelpers.Log("Extracting", "Extracting with " + playersExtracted + " players onboard");
		
		if (playersExtracted > 0)
		{
			// After initiating the extraction, trigger the end game process with a victory condition
	    	GetGame().GetCallqueue().CallLater(EndGame, 5 * 1000, false, EGameOverTypes.VICTORY, 1);
		}
		else
		{
			// After initiating the extraction, trigger the end game process with a victory condition
	    	GetGame().GetCallqueue().CallLater(EndGame, 5 * 1000, false, EGameOverTypes.NEUTRAL, 1);
		}
	    
	}

	
	//! Handles the player-side game loop, performing player-specific tasks.
	//! -------------------------------------
	protected void PlayerUpdateLoop()
	{
	    if (playerLoopDebugLogging) DefendHelpers.Log("Running Player Loop", "Player loop running.");
	}
	
	//! Checks for players who have left the designated area (deserters) and applies consequences, such as warnings or execution.
	//! This method is invoked periodically to ensure players are staying within the bounds of the operation area.
	//! --------------------------------------------------------------
	protected void CheckForDeserters()
	{
	    // Find the entity representing the designated operation area (waypoint) in the game world
	    IEntity waypointPosition = FindLayoutEntity(waypointPositionName);
	    vector waypointPos = waypointPosition.GetOrigin();
	    
	    // Iterate through all alive players to check their positions
	    foreach (DefendPlayer player : GetAlivePlayers())
	    {
	        // Get the controlled entity of the player (i.e., the player character)
	        IEntity playerEntity = GetGame().GetPlayerManager().GetPlayerControlledEntity(player.GetID());
	        
	        // Continue if the player entity exists
	        if (playerEntity != null)
	        {
	            // Get the player's current position in the game world
	            vector playerPosition = playerEntity.GetOrigin();
	                
	            // Calculate the distance from the player to the base (waypoint) and to the extraction point
	            float myDistanceToBase = vector.Distance(playerPosition, waypointPos);
	            float myDistanceToExtraction = 999;  // Default high value
	            if (extractionStarted) 
	                myDistanceToExtraction = vector.Distance(playerPosition, extractionGroup.GetGroup().GetOrigin());
	            
	            // If the player is too far from both the waypoint and extraction point, they are considered deserters
	            if (myDistanceToBase > distanceToPunishDeserters && myDistanceToExtraction > distanceToPunishDeserters)
	            {
	                // Set the player as a deserter
	                player.SetDeserter(true);
	                DefendHelpers.Log("Player Deserting", "Player " + player.GetName() + " #" + player.GetID() + " is deserting!");
	                
	                // Get the player character's object for further manipulation
	                SCR_ChimeraCharacter scrChar = SCR_ChimeraCharacter.Cast(playerEntity);
	                if (scrChar != null)
	                {
	                    // Get the character's controller to check the player's life state
	                    CharacterControllerComponent charController = scrChar.GetCharacterController();
	        
	                    // If the character is alive, apply consequences for desertion
	                    if (charController.GetLifeState() == ECharacterLifeState.ALIVE)
	                    {
	                        // If the player has exceeded the warning threshold, execute the player for desertion
	                        if (player.GetDeserterWarnings() > numberOfWarningsForDeserters)
	                        {
	                            SendHint("Deserter!", "You have left the area of operation, this will not be tolerated and you have been executed.", serverLoopIntervalSeconds, playerId:player.GetID());
	                            charController.ForceDeath();
	                        }
	                        // Otherwise, warn the player and inflict health loss for desertion
	                        else
	                        {
	                            SendHint("Deserting! (warning " + player.GetDeserterWarnings().ToString() + "/" + numberOfWarningsForDeserters.ToString() + ")", 
	                                      "You have left the area of operation, this will not be tolerated and you're currently losing health!", 
	                                      serverLoopIntervalSeconds, player.GetID());
	                            player.AddDeserterWarning();
	                        }
	                    }
	                }
	                else 
	                    DefendHelpers.Log("ERROR", "Couldn't find player character!");  // Log error if player character is not found
	                
	                // Set the player as a deserter (again, to ensure they are marked as such)
	                player.SetDeserter(true);
	            }
	            // If the player is near the boundary but not fully outside, give them a warning
	            else if (myDistanceToBase > (distanceToPunishDeserters - 10) && myDistanceToExtraction > (distanceToPunishDeserters - 10))
	            {
	                SendHint("Where are you going?", "You're leaving the area of operation turn back now or be labeled a deserter!", serverLoopIntervalSeconds, player.GetID());
	                player.SetDeserter(false);  // Ensure the player is not marked as a deserter if they are close to the boundary
	            }
	            else
	            {
	                // If the player is within acceptable range, ensure they are not marked as a deserter
	                player.SetDeserter(false);
	            }    
	        }
	        else DefendHelpers.Log("No Player Entity", "The player currently has no entity attached to it.");  // Log if the player has no entity attached
	    }
	}

	
	//! Checks and updates the status of all AI groups, including removing dead AI groups.
	//! -------------------------------------
	protected bool checkAIQueued = false;
	protected bool checkingAI = false;
	protected void CheckAI()
	{
	    checkingAI = true;
	    checkAIQueued = false;
	    if (serverLoopDebugLogging) DefendHelpers.Log("Running Server AI Loop", "Server AI loop running to check AI.");
	    
	    CheckAIAlive();
	    RemindAI();
	    
	    checkingAI = false;
	}
	
	//! Checks if any AI groups have lost all members, and removes those groups from the game.
	//! -------------------------------------
	protected void CheckAIAlive()
	{
		array<SpawnedEnemyGroup> deadGroups = {};
		foreach (int aiGroupInd, SpawnedEnemyGroup spawnedGroup : activeAIGroups)
		{
			if (!spawnedGroup.HasGroup())
				continue;
			
			if (spawnedGroup.GetState() == SpawnedGroupState.RETREATING)
				continue;
			
			if (serverLoopDebugLogging) DefendHelpers.Log("Checking Alive AI", "Checking AI #" + (aiGroupInd+1).ToString() + "/" + activeAIGroups.Count().ToString());
			
			ChimeraAIGroup group = spawnedGroup.GetGroup();
			if (group == null)
			{
				if (serverLoopDebugLogging) DefendHelpers.Log("No AI Group?", "This AI has no group attached?!");
				deadGroups.Insert(spawnedGroup);
				continue;
			}
			
	        AIGroup currGroup = AIGroup.Cast(group);
	        if (currGroup != null)
	        {
	            int agentCount = currGroup.GetAgentsCount();
	    
	            array<AIAgent> agents = {};
	            currGroup.GetAgents(agents);
	            
	            array<SCR_ChimeraCharacter> characters = {};
	            
	            foreach (AIAgent agent : agents)
	            {
	                SCR_ChimeraCharacter char = SCR_ChimeraCharacter.Cast(agent.GetControlledEntity());
	                if (char != null)
	                    characters.Insert(char);
	            }
	    
	            
	            int countDeadMembers = 0;
	            
	            if (serverLoopDebugLogging) DefendHelpers.Log("Checking group", "Group " + (aiGroupInd+1) + "/" + activeAIGroups.Count() + " and has " + characters.Count() + " characters");
	            
	            for (int charInd; charInd < characters.Count(); charInd++)
	            {    
	                SCR_ChimeraCharacter currChar = characters[charInd];
	                ChimeraCharacter character = currChar;
	                SCR_DamageManagerComponent dmgComp = character.GetDamageManager();
	                float health = dmgComp.GetHealthScaled();
	                if (health <= 0) countDeadMembers++; 
	            }
	            
	            if (countDeadMembers == characters.Count()) deadGroups.Insert(spawnedGroup);
	        }            
		}
		
		RemoveAIGroups(deadGroups);
	    
	    if (serverLoopDebugLogging) DefendHelpers.Log("Total AI Alive: ", CountAliveAI().ToString());
	}
	
	//! Reminds AI of their tasks and reassigns waypoints if necessary.
	//! -------------------------------------
	protected void RemindAI()
	{		
	    foreach(int i, SpawnedEnemyGroup spawnedGroup : activeAIGroups)
	    {
			if (!spawnedGroup.HasGroup())
				continue;
			
			if (!spawnedGroup.GetState() == SpawnedGroupState.RETREATING)
				continue;

	        if (serverLoopDebugLogging) DefendHelpers.Log("Reminding AI (" + (i+1).ToString() + "/" + activeAIGroups.Count() + ")", "Remind AI that they're supposed to be doing something...");
				
	        array<AIWaypoint> waypoints = {};
	        spawnedGroup.GetGroup().GetWaypoints(waypoints);
	        IEntity waypointPosition = FindLayoutEntity(waypointPositionName);
	        bool inVeh = false;
	        
	        array<AIAgent> groupAgents = {};
	        spawnedGroup.GetGroup().GetAgents(groupAgents);
	
	        foreach (AIAgent agent : groupAgents)
	        {
				float distance = vector.Distance(agent.GetOrigin(), waypointPosition.GetOrigin());
				
				

	            SCR_ChimeraCharacter char = SCR_ChimeraCharacter.Cast(agent.GetControlledEntity());
	            if (char != null)
	            {
	                if (char.IsInVehicle()) 
	                    inVeh = true;
	            }
				
				if (distance > 50 && !inVeh)
					agent.ClearDangerEvents(agent.GetDangerEventsCount());
	        }
	        
	        
	        if (!inVeh)
	        {
	            if (waypoints.Count() == 0)
	            {
	                float distance = vector.Distance(spawnedGroup.GetGroup().GetLeaderEntity().GetOrigin(), waypointPosition.GetOrigin());
	                if (serverLoopDebugLogging) DefendHelpers.Log("AI isn't busy", "AI #" + (i+1).ToString() + " didn't have anything to do and is " + distance + " away from obj!");
	    
	                
	                if (distance < 50 && !inVeh)
	                {
	                    if (serverLoopDebugLogging) DefendHelpers.Log("AI is within their waypoint", "AI is within their waypoint radius.");
	                    SetWaypoint(spawnedGroup, patrolWaypointType, waypointPosition.GetOrigin());
	                }
	                else
	                {
						AIAgent leaderAgent = spawnedGroup.GetGroup().GetLeaderAgent();
						leaderAgent.ClearDangerEvents(leaderAgent.GetDangerEventsCount());
	                    SetWaypoint(spawnedGroup, moveWaypointType, waypointPosition.GetOrigin());
	                }
	            }
	            else 
	            {
	                AIWaypoint currWaypoint = waypoints[0];
	                if (currWaypoint.IsWithinCompletionRadius(spawnedGroup.GetGroup()))
	                {
	                    if (serverLoopDebugLogging) DefendHelpers.Log("AI is within their waypoint", "AI #" + (i+1).ToString() + " is busy but within their waypoint location");
	                
	                    SetWaypoint(spawnedGroup, patrolWaypointType, waypointPosition.GetOrigin());
	                }
	                else
	                {
	                    if (serverLoopDebugLogging) DefendHelpers.Log("AI is busy", "AI #" + (i+1).ToString() + " is busy going to a waypoint");
	                    GetGame().GetCallqueue().CallLater(CheckRemindAI, 20000, false, spawnedGroup, currWaypoint);
	                }
	            }
	        }
	    }
	}
	
	//! Resets an AI group's waypoint if they are taking too long to reach it.
	//! -------------------------------------
	protected void CheckRemindAI(SpawnedEnemyGroup spawnedGroup, AIWaypoint waypoint)
	{
		if (activeAIGroups.Count() == 0)
			return;
		
	    array<AIWaypoint> waypoints = {};
	    if (spawnedGroup.GetGroup() == null)
	        return;
	    
	    spawnedGroup.GetGroup().GetWaypoints(waypoints);
	    
	    if (waypoints.Count() > 0)
	    {
	        AIWaypoint currWaypoint = waypoints[0];
	        if ((currWaypoint == waypoint) && (spawnedGroup.GetState() != SpawnedGroupState.RETREATING))
	        {
	            if (serverLoopDebugLogging) DefendHelpers.Log("AI Took too long", "AI took too long so resetting its waypoint.");
	            foreach (AIWaypoint waypointToDelete : waypoints)
	            {
	                spawnedGroup.GetGroup().RemoveWaypoint(waypointToDelete);
	            }
	            
	            if (currWaypoint.IsWithinCompletionRadius(spawnedGroup.GetGroup()))
	            {
	                if (serverLoopDebugLogging) DefendHelpers.Log("AI is within their waypoint", "AI is within their waypoint radius.");
	                IEntity waypointPosition = FindLayoutEntity(waypointPositionName);
	                SetWaypoint(spawnedGroup, patrolWaypointType, waypointPosition.GetOrigin());
	            }
	        }
	    }
	}
	
	//! Counts the number of AI characters that are still alive.
	//! -------------------------------------
	//! Returns:
	//! The total number of alive AI agents in the game.
	int CountAliveAI()
	{
	    int totalAIAlive = 0;
	    array<AIAgent> agents = {};
	    array<SCR_ChimeraCharacter> characters = {};
	
	    GetGame().GetAIWorld().GetAIAgents(agents);
	    
	    foreach (int i, AIAgent agent : agents)
	    {
			if (agent == null)
				continue;
			
	        SCR_ChimeraCharacter char = SCR_ChimeraCharacter.Cast(agent.GetControlledEntity());
	        if (char != null)
	        {
	            characters.Insert(char);
	        }
	    }
	                
	    for (int charInd; charInd < characters.Count(); charInd++)
	    {    
	        SCR_ChimeraCharacter currChar = characters[charInd];
	        if (currChar.GetFactionKey() != "USSR")
	        {
	            continue;
	        }    
	        
	        ChimeraCharacter character = currChar;
	        if (character != null)
	        {
	            SCR_DamageManagerComponent dmgComp = character.GetDamageManager();
	            float health = dmgComp.GetHealthScaled();
	            if (health > 0) totalAIAlive++; 
	        }
	        else DefendHelpers.Log("AI Not Ready", "The AI doesn't have a controller character yet.");
	    }
	    return totalAIAlive;
	}
	
	//! Checks whether building is allowed.
	//! -------------------------------------
	//! Returns:
	//! True if building is allowed, false otherwise.
	bool CanBuild()
	{
	    if (debugMode) return true;
		else if (allowBuilding) return _canBuild;
		else return false;
	}
	
	//! Sets whether building is allowed.
	//! -------------------------------------
	//! Parameters:
	//! allowed - A boolean indicating whether building is allowed.
	void SetCanBuild(bool allowed)
	{
	   if (allowBuilding) _canBuild = allowed;
	}
	
	
	//! Sends a request to allow or disallow building in the game
	//! -------------------------------------
	//! Parameters:
	//! allowed - A boolean indicating if building should be allowed (true) or disallowed (false).
	void SendAllowBuilding(bool allowed, int playerId = 0)
	{
	    if (rpcLogging) DefendHelpers.Log("Sending Allow Building", "Sending Allow Building to: " + allowed);
	    if (DefendHelpers.IsHost()) AllowBuilding(allowed);
	
	    Rpc(RpcDo_AllowBuilding, allowed, playerId);
	}
	
	//! RPC function that handles the reception of the "Allow Building" request
	//! -------------------------------------
	//! Parameters:
	//! allowed - A boolean indicating if building should be allowed (true) or disallowed (false).
	[RplRpc(RplChannel.Reliable, RplRcver.Broadcast)]	
	protected void RpcDo_AllowBuilding(bool allowed, int playerId) 
	{
	    if (rpcLogging) DefendHelpers.Log("RpcDo: Allow Building", "Set allow building to " + allowed);
	    if (playerId == 0 || playerId == localPlayerId) AllowBuilding(allowed);
	}
	
	//! Allows or disallows building and handles related game UI updates
	//! -------------------------------------
	//! Parameters:
	//! allowed - A boolean indicating if building should be allowed (true) or disallowed (false).
	protected void AllowBuilding(bool allowed)
	{
	    IEntity gm = GetGame().GetWorld().FindEntityByName("GameMode_Defend");
	    SetCanBuild(allowed);
	    if (gm != null)
	    {
	        SCR_EditorManagerEntity editor = SCR_EditorManagerEntity.GetInstance();
	
	        if (editor != null)
	        {
	            if (editor.IsOpened() && !allowed)
	            {
	                if (!CanBuild()) 
	                {
	                    editor.Close();
	                }
	            }
	        }
	        
	        if (allowed)
	            hint.ShowHint("Building Allowed", "You can now use the table at the Command Post to build defenses.", isDebug:hintLogging);
	        else
	            hint.ShowHint("Building Restricted", "You can no longer use the table at the Command Post to build defenses.", isDebug:hintLogging);
	        
	        DefendHelpers.Log("Allow player building", "Building allowed: " + CanBuild());
	    }
	    else DefendHelpers.Log("Not Defend Gamemode?", "Couldn't find GameModeDefend entity!");
	}
	
	//! Sends a HUD update with wave information and additional details
	//! -------------------------------------
	//! Parameters:
	//! wave - The current wave number.
	//! maxWave - The total number of waves.
	//! secondsUntilNext - The number of seconds until the next wave starts (optional, default 0).
	//! customText - Custom text to display on the HUD (optional, default "").
	//! playerId - The ID of the player to update (optional, default 0).
	void SendHUDUpdate(int wave, int maxWave, int secondsUntilNext = 0, string customText = "", int playerId = 0)
	{
	    if (rpcLogging) DefendHelpers.Log("Sending HUD Update", "Sending HUD Update to: " + playerId + ", custom text: " + customText + " | wave: " + wave + " | " + secondsUntilNext + " secs");
	    if (DefendHelpers.IsHost()) hud.ShowHUD(wave, maxWave, livesLeft, secondsUntilNext, CountAliveAI(), customText);
	    
	    Rpc(RpcDo_ShowHUDUpdate, wave, maxWave, livesLeft, secondsUntilNext, CountAliveAI(), customText, playerId);
	}
	
	//! RPC function that handles the reception of the HUD update
	//! -------------------------------------
	//! Parameters:
	//! wave - The current wave number.
	//! maxWave - The total number of waves.
	//! lives - The number of lives left for the player.
	//! secondsUntilNext - The number of seconds until the next wave starts.
	//! enemies - The number of remaining enemies.
	//! customText - Custom text to display on the HUD.
	//! playerId - The ID of the player receiving the update.
	[RplRpc(RplChannel.Unreliable, RplRcver.Broadcast)]	
	protected void RpcDo_ShowHUDUpdate(int wave, int maxWave, int lives, int secondsUntilNext, int enemies, string customText, int playerId) 
	{
	    if (rpcLogging) DefendHelpers.Log("RpcDo: Show HUD Update", wave.ToString() + "/" + maxWave + " | " + secondsUntilNext + " secs | custom text: " + customText + " | Enemies: " + enemies + " | " + playerId);
	    if (localPlayerId == playerId || playerId == 0)
	    {
	        hud.ShowHUD(wave, maxWave, lives, secondsUntilNext, enemies, customText);
	    }
	    else DefendHelpers.Log("Not me, ignoring hud update", "PlayerID: " + playerId);
	}
	
	//! Sends a hint message to a player
	//! -------------------------------------
	//! Parameters:
	//! header - The header of the hint message.
	//! message - The body of the hint message.
	//! secondsToShow - The number of seconds to display the hint (optional, default 10).
	//! showTimer - Whether to show a countdown timer (optional, default true).
	//! playerId - The ID of the player to send the hint to (optional, default 0).
	void SendHint(string header, string message, int secondsToShow = 10, int playerId = 0)
	{
		bool showTimer = true;
	    if (rpcLogging) DefendHelpers.Log("Sending Hint", "Sending hint to: " + playerId + " | " + header + " | " + message + " | " + secondsToShow + " secs");
	    if (DefendHelpers.IsHost()) hint.ShowHint(header, message, secondsToShow, showTimer, isDebug:hintLogging);
	    
	    Rpc(RpcDo_ShowHint, header, message, secondsToShow, showTimer, playerId);
	}
	
	//! RPC function that handles the reception of the hint message
	//! -------------------------------------
	//! Parameters:
	//! header - The header of the hint message.
	//! message - The body of the hint message.
	//! secondsToShow - The number of seconds to display the hint.
	//! showTimer - Whether to show a countdown timer.
	//! playerId - The ID of the player receiving the hint.
	[RplRpc(RplChannel.Unreliable, RplRcver.Broadcast)]	
	protected void RpcDo_ShowHint(string header, string message, int secondsToShow, bool showTimer, int playerId) 
	{
	    if (rpcLogging) DefendHelpers.Log("RpcDo: Show Hint", header + " | " + message);
	    if (localPlayerId == playerId || playerId == 0)
	    {
	        hint.ShowHint(header, message, secondsToShow, showTimer, isDebug:hintLogging);
	    }
	}
	
	//! Waits for players to join before starting the game
	protected void WaitForPlayers()
	{
	    gameStarted = false;
		if (!waitingForPlayers)
		{
			waitingForPlayers = true;
	    	DefendHelpers.Log("Waiting for players", "There are currently no players, waiting for some...");
		}
	    
	    if (activePlayerIds.Count() < 1)
	    {
	        if (currentWave != 0)
	        {
	            GetGame().GetCallqueue().Call(EndGame, EGameOverTypes.SERVER_RESTART, 0);
	        }
	        else
	        {
	            GetGame().GetCallqueue().CallLater(WaitForPlayers, 10000);
	        }
	    }
	    else
	    {
	        waitingForPlayers = false;
	        GetGame().GetCallqueue().Call(Start);
	    }
	}
	
	//! Starts the game and initiates the first wave countdown
	void Start()
	{
	    DefendHelpers.Log("Manager started!", "The Defend Manager has been started.");
	    livesLeft = numberOfLives;
	    gameStarted = true;
	    
	    GetGame().GetCallqueue().CallLater(StartWaveTimer, secondsBeforeStarting * 1000);
	}
	
	//! Starts the wave timer countdown
	protected void StartWaveTimer()
	{
		DefendHelpers.Log("Waiting Until First Wave", "The game has started, currently waiting until the first wave.");
		
		if (debugMode && debugStartingWave != 0) currentWave = (debugStartingWave-1);
		SendHUDUpdate(0, GetNumberOfWaves(), secondsBeforeFirstWaveToPrepare, "PREPARE YOUR DEFENSES");
	
		GetGame().GetCallqueue().CallLater(WaveTimerTick, secondsBeforeFirstWaveToPrepare * 1000);
	}
	
	//! Handles the wave timer tick, incrementing the wave and triggering other actions
	protected void WaveTimerTick()
	{
	    currentWave++;
		DefendHelpers.Log("Starting Wave " + currentWave, "Currently starting wave " + currentWave);
		if (restrictBuildingDuringWaves) SendAllowBuilding(false);
	    
	    if (players.Count() < 1) 
	    {
	        GetGame().GetCallqueue().Call(WaitForPlayers);
	    }
	    else
	    {
	        GetGame().GetCallqueue().CallLater(EnemyTimerTick, secondsBeforeSpawnEnemy * 1000);
	    }		
	}
	
	//! Handles the spawning of enemies based on wave progression
	protected void EnemyTimerTick()
	{
		DefendHelpers.Log("Spawning Wave " + currentWave, "Currently spawning the enemy for wave " + currentWave);

	    int maxEnemies = numberOfEnemiesPerWave;
	    if (balenceEnemies && GetAlivePlayers().Count() > 1)
	    {
	        maxEnemies = maxEnemies * GetAlivePlayers().Count();
	    }
		maxEnemies = maxEnemies * currentWave;
		if (maxEnemies > maxAIGroupsPerWave) maxEnemies = maxAIGroupsPerWave;
	    
	    for (int enemy; enemy < maxEnemies; enemy++)
	    {
	        DefendHelpers.Log("Enemy " + (enemy+1) + "/" + maxEnemies, "Spawned enemy " + (enemy+1) + " of "  + maxEnemies + ".");
	        bool isVeh = ((DefendHelpers.GenerateRandom(1,(oddsOfVehicleSpawn+1))) == oddsOfVehicleSpawn);
	        if (allowVehicles)
	        {
	            DefendHelpers.Log("Enemy Is Vehicle: ", isVeh.ToString());
	            DefendHelpers.Log("Vehicle Count: ", vehicleCount.ToString() + "/" + maxVehicles.ToString());
	        }
	        
	        if ((allowVehicles && isVeh && (currentWave != GetNumberOfWaves())) && (vehicleCount < maxVehicles) && !zombiesMode)
	        {
	            vehicleCount = vehicleCount + 1;
	            int randomSpawn = DefendHelpers.GenerateRandom(0, (vehicleSpawnPositions.Count()-1));
	            GetGame().GetCallqueue().CallLater(SendWave, ((enemy+1) * 1000), false, randomSpawn, true);
	        }
	        else
	        {
	            int randomSpawn = DefendHelpers.GenerateRandom(0, (infantrySpawnPositions.Count()-1));
	            GetGame().GetCallqueue().CallLater(SendWave, ((enemy+1) * 1000), false, randomSpawn, false);
	        }
	    }
		GetGame().GetCallqueue().CallLater(PostSpawn, 10000, false);
	}
	
	//! Sends a wave of enemies to spawn at a specific location
	//! -------------------------------------
	//! Parameters:
	//! spawnID - The ID of the spawn location.
	//! vehicle - A boolean indicating whether to spawn a vehicle (true) or infantry (false).
	protected void SendWave(int spawnInd, bool vehicle)
	{
	    if (GetGame().GetWorld() != null)
	    {
	        DefendHelpers.Log("Sending wave...", "Sending the wave to the waypoint.");
	    
	        IEntity waypointPosition = FindLayoutEntity(waypointPositionName);
			IEntity vehWaypointPosition = FindLayoutEntity(vehWaypointPositionName);
	        
	        if (vehicle) 
	        {

	            IEntity spawnPosition = FindLayoutEntity(vehicleSpawnPositions[spawnInd]);
	            GetGame().GetCallqueue().Call(AISpawner, spawnGroupsVeh[DefendHelpers.GenerateRandom(0, spawnGroupsVeh.Count())], spawnPosition, vehWaypointPosition.GetOrigin(), vehicle);
	        }	
	        else
	        {
				IEntity spawnPosition = FindLayoutEntity(infantrySpawnPositions[spawnInd]);
				if (zombiesMode)
				{
					GetGame().GetCallqueue().Call(AISpawner, spawnGroupsZom[DefendHelpers.GenerateRandom(0, spawnGroupsZom.Count())], spawnPosition, waypointPosition.GetOrigin(), vehicle);
				}
				else
				{
	            	GetGame().GetCallqueue().Call(AISpawner, spawnGroupsInf[DefendHelpers.GenerateRandom(0, spawnGroupsInf.Count())], spawnPosition, waypointPosition.GetOrigin(), vehicle);
				}
	        }
	    }
	}
	
	//! Spawns an AI group (vehicle or infantry) at the specified position
	//! -------------------------------------
	//! Parameters:
	//! groupToSpawn - The name of the group to spawn (vehicle or infantry).
	//! spawnEntity - The entity that defines the spawn position.
	//! waypointPosition - The position where the AI group should move.
	//! isVehicle - A boolean indicating if the group is a vehicle (true) or infantry (false).
	protected void AISpawner(string groupToSpawn, IEntity spawnEntity, vector waypointPosition, bool isVehicle)
	{
	    DefendHelpers.Log("Spawning AI...", "AI Spawner started.");
	    
	    SpawnedEnemyGroup spawnedGroup = null;
	    
	    if (isVehicle)
	        spawnedGroup = SpawnVehicle(groupToSpawn, spawnEntity);
	    else
	        spawnedGroup = SpawnInfantry(groupToSpawn, spawnEntity);
		
	    GetGame().GetCallqueue().Call(SetWaypoint, spawnedGroup, moveWaypointType, waypointPosition);
	}
	
	//! Handles the logic after spawning a new wave of AI enemies.
	//! -------------------------------------
	//! This method is responsible for preparing the game for the next wave of enemies, 
	//! sending the player a hint about the upcoming wave, and determining the next action 
	//! based on whether the wave is completed or if reinforcement is needed.
	//! It manages different scenarios such as AI death, waiting for reinforcements, or 
	//! starting the extraction process when the maximum number of waves is reached.
	protected void PostSpawn()
	{
		DefendHelpers.Log("Post Spawn", "AI Units should have spawned by now.");
		
		string waveTitle = "Wave " + currentWave.ToString() + "/" + GetNumberOfWaves();
	    if (endlessMode) waveTitle = "Wave " + currentWave.ToString();

	    SendHint(waveTitle, "Prepare yourself, the next wave should be arriving now...");
		
		if (currentWave != GetNumberOfWaves() || endlessMode == true) 
	    {
			if (allowBuildingTimeBetweenWaves)
			{
				DefendHelpers.Log("Waiting on AI death", "Will wait until AI are dead before next wave.");
				SendHUDUpdate(currentWave, GetNumberOfWaves(), 0);
				GetGame().GetCallqueue().CallLater(WaitUntilAllAIAreDead, aiLoopIntervalSeconds * 1000);
			}
			else
			{
				DefendHelpers.Log("Waiting for reinforcements", "Waiting until until reinforcements arrive in " + secondsBeforeReinforcements + " seconds");
	       		SendHUDUpdate(currentWave, GetNumberOfWaves(), secondsBeforeReinforcements);
				GetGame().GetCallqueue().CallLater(WaveTimerTick, secondsBeforeReinforcements * 1000);
			}
		}
		else
		{
			DefendHelpers.Log("Starting Extraction", "Max waves reached, calling extraction.");
			SendHUDUpdate(currentWave, GetNumberOfWaves(), 0);
			GetGame().GetCallqueue().CallLater(StartExtraction, 10000);
		}
	}
	
	//! Initiates the extraction process and spawns the required entities.
	//! -------------------------------------
	//! This method handles the start of the extraction process by selecting a random 
	//! extraction vehicle and position, then spawning the extraction vehicle and its associated AI group.
	//! It also schedules the extraction to proceed after a set time delay.
	protected void StartExtraction()
	{
		DefendHelpers.Log("Starting Extraction", "Began spawning extraction...");
		SendHint("Extraction Nearly Available", "We've called for your extraction, we just need to survive a little while longer...");
		string forcedMoveWaypointName = "{06E1B6EBD480C6E0}Prefabs/AI/Waypoints/AIWaypoint_ForcedMove.et";
		
		string randomExtractionVeh = extractionVehicleTypes[DefendHelpers.GenerateRandom(0, extractionVehicleTypes.Count()-1)];		
		string randomExtractionPos = extractionMarkerPositions[DefendHelpers.GenerateRandom(0, extractionMarkerPositions.Count()-1)];
		
		DefendHelpers.Log("Extraction Location Confirmed", "Will extract to: " + randomExtractionPos + " using " + randomExtractionVeh);
		
		IEntity sourcePos = FindLayoutEntity("us_extraction_source");
		IEntity extractionPoint = FindLayoutEntity(randomExtractionPos);

		Vehicle vehicle = null;
		SCR_AIGroup group = SpawnExtraction(randomExtractionVeh, sourcePos, vehicle);

		DefendHelpers.Log("Extraction Has Started", "Extraction is ready and waiting for seconds until it arrives.");
		GetGame().GetCallqueue().CallLater(SendExtraction, secondsUntilExtractionArrives * 1000, false, group, vehicle, forcedMoveWaypointName, extractionPoint.GetOrigin());
	}
	
	//! Spawns the extraction vehicle and associated AI group at a specified position.
	//! -------------------------------------
	//! Parameters:
	//! unitToSpawn - The type of extraction vehicle to spawn.
	//! spawnAt - The entity representing the spawn position.
	//! -------------------------------------
	//! Returns:
	//! SCR_AIGroup - The spawned AI group that will accompany the extraction vehicle.
	protected SCR_AIGroup SpawnExtraction(string unitToSpawn, IEntity spawnAt, out Vehicle vehicle)
	{
	    Resource resource = DefendHelpers.GenerateAndValidateResource(unitToSpawn);
	    
	    if (resource == null)
	    {
	        DefendHelpers.Log("ERROR!", "Unable to load resource for spawn group: " + unitToSpawn, LogLevel.ERROR);	
	        return null;
	    }
	    
	    vehicle = Vehicle.Cast(GetGame().SpawnEntityPrefab(resource, null, DefendHelpers.GenerateSpawnParamaters(spawnAt.GetOrigin())));
	    IEntity vehEntity = vehicle;
	    vehEntity.SetYawPitchRoll(spawnAt.GetYawPitchRoll());
		
	    SCR_AIGroup group = SpawnExtractionInfantry("{958039B857396B7B}Prefabs/Groups/BLUFOR/Group_US_MachineGunTeam.et", spawnAt);
	    string getInWaypointName = "{B049D4C74FBC0C4D}Prefabs/AI/Waypoints/AIWaypoint_GetInNearest.et";
	    Resource pilotResource = DefendHelpers.GenerateAndValidateResource(getInWaypointName);
			
	    AIWaypoint waypoint = AIWaypoint.Cast(GetGame().SpawnEntityPrefab(pilotResource, null, DefendHelpers.GenerateSpawnParamaters(spawnAt.GetOrigin())));
				
	    group.AddWaypoint(waypoint);
	    return group;
	}
	
	//! Spawns the infantry group associated with the extraction process at a specified position.
	//! -------------------------------------
	//! Parameters:
	//! unitToSpawn - The type of infantry group to spawn.
	//! spawnAt - The entity representing the spawn position.
	//! -------------------------------------
	//! Returns:
	//! SCR_AIGroup - The spawned infantry AI group that will accompany the extraction vehicle.
	protected SCR_AIGroup SpawnExtractionInfantry(string unitToSpawn, IEntity spawnAt)
	{
	    Resource resource = DefendHelpers.GenerateAndValidateResource(unitToSpawn);
	    
	    if (resource == null)
	    {
	        DefendHelpers.Log("ERROR!", "Unable to load resource for spawn group: " + unitToSpawn, LogLevel.ERROR);	
	        return null;
	    }
		
		vector spawnPos = spawnAt.GetOrigin();
		spawnPos = spawnPos+ vector.One;
	    
	    SCR_AIGroup group = SCR_AIGroup.Cast(GetGame().SpawnEntityPrefab(resource, null, DefendHelpers.GenerateSpawnParamaters(spawnPos)));		
	    
	    IEntity groupEntity = group;
	    groupEntity.SetYawPitchRoll(spawnAt.GetYawPitchRoll());
				
	    if (group == null)
	    {
	        DefendHelpers.Log("ERROR!", "Unable to spawn group!", LogLevel.ERROR);
	        return null;
	    }
	    else
		{						
			DefendHelpers.Log("Group Spawned", "Group spawned...");
			return group;
		}
	}
	
	//! Sends the extraction group and vehicle to the designated target location.
	//! -------------------------------------
	//! This method handles the logic for sending the extraction group to the player’s location, 
	//! including setting up waypoints for the extraction vehicle, disabling damage handling 
	//! for the AI agents and vehicle during the extraction process, and updating the game state.
	//! -------------------------------------
	//! Parameters:
	//! group - The AI group associated with the extraction.
	//! vehicle - The vehicle used for the extraction.
	//! type - The resource type for the waypoint used in the extraction.
	//! target - The target position for the extraction vehicle.
	protected void SendExtraction(SCR_AIGroup group, Vehicle vehicle, string type, vector target)
	{	
		DefendHelpers.Log("Sending Extraction", "Began sending extraction...");
		SendHint("Extraction Inbound", "The extraction truch has just left, it will be with you shortly...");

	    Resource resource = DefendHelpers.GenerateAndValidateResource(type);
	    
	    if (resource == null)
	    {
	        DefendHelpers.Log("ERROR!", "Unable to load resource for spawn group: " + group, LogLevel.ERROR);
	        return;
	    }
	    
	    AIWaypoint waypoint = AIWaypoint.Cast(GetGame().SpawnEntityPrefab(resource, null, DefendHelpers.GenerateSpawnParamaters(target)));
	    if (waypoint == null)
	    {
	        DefendHelpers.Log("ERROR!", "Unable to spawn waypoint!", LogLevel.ERROR);
	        return;
	    }
		
		array<AIAgent> agents = {};
		group.GetAgents(agents);
		foreach (AIAgent agent : agents)
		{
			IEntity entity = agent.GetControlledEntity();

			SCR_DamageManagerComponent dmgManager = SCR_DamageManagerComponent.Cast(entity.FindComponent(SCR_DamageManagerComponent));
			dmgManager.EnableDamageHandling(false);
		}
		
		SCR_VehicleDamageManagerComponent dmgManager = SCR_VehicleDamageManagerComponent.Cast(IEntity.Cast(vehicle).FindComponent(SCR_VehicleDamageManagerComponent));
		dmgManager.EnableDamageHandling(false);
	            
		ExtractionGroup extractGroup = new ExtractionGroup();
		extractGroup.SetGroup(group);
		extractGroup.SetVehicle(vehicle);
		extractGroup.targetExtractionPoint = target;
		
		extractionGroup = extractGroup;
		extractionStarted = true;
		
	    group.AddWaypoint(waypoint);
	}
	
	//! Waits for all AI enemies to be defeated before proceeding with the next phase.
	//! -------------------------------------
	//! This method checks if there are enough AI units dead to proceed with retreating 
	//! or initiating the next wave. It will continue to check periodically until the condition 
	//! is met, after which it triggers the retreat or next wave actions.
	protected void WaitUntilAllAIAreDead()
	{
		DefendHelpers.Log("Waiting", "Waiting for AI to die...");
		
		if (activePlayerIds.Count() < 1) 
		{
			GetGame().GetCallqueue().Call(WaitForPlayers);
			return;
		}
		
		int aliveAI = CountAliveAI();
		if ((aliveAI <= enemiesLeftToFinishWave) && (activeAIVehicles.Count() <= 0))
		{
			DefendHelpers.Log("Enough AI are dead", "Enough AI have been killed for them to retreat.");
			SendHint("Enemy is retreating", "They seem to be retreating for now, be careful as they're still dangerous!");
			
			GetGame().GetCallqueue().Call(RetreatWave);
			GetGame().GetCallqueue().CallLater(MidWaveTimerTick, 10000);
		}
		else
		{
			DefendHelpers.Log("Waiting for AI to die", "Waiting until there are less than or equal to " + enemiesLeftToFinishWave + " enemies remaining (currently " + aliveAI + " AI alive)");
			GetGame().GetCallqueue().CallLater(WaitUntilAllAIAreDead, aiLoopIntervalSeconds * 1000);
		} 
		
		
	}
	
	//! Handles the retreat of the current enemy wave once certain conditions are met.
	//! -------------------------------------
	//! This method checks if the AI enemies should retreat, either because they have been 
	//! defeated or because they have reached the retreat condition. It forces retreat behavior 
	//! for AI groups, updates their waypoints, and adjusts the game state accordingly.
	//! It also manages a timer to force AI death if necessary and tracks retreat progress.
	protected void RetreatWave()
	{
		array<SpawnedEnemyGroup> retreatedGroups = {};
		DefendHelpers.Log("Retreating Wave", "Retreating this wave...");
		IEntity waypointPosition = FindLayoutEntity(waypointPositionName);
		foreach(ref SpawnedEnemyGroup spawnedGroup : activeAIGroups)
		{	
			if (!spawnedGroup.HasGroup())
			{
				retreatedGroups.Insert(spawnedGroup);
				continue;
			}
			
			DefendHelpers.Log("Retreating Group", "Retreating this group...");
			
			array<AIAgent> members = {};
			spawnedGroup.GetGroup().GetAgents(members);
			
			if (members.Count() > 0)
			{
				foreach (AIAgent agent : members)
				{
					DefendHelpers.Log("Retreating Member", "Retreating a member of this group...");
					agent.ClearDangerEvents(agent.GetDangerEventsCount());

					IEntity entity = agent.GetControlledEntity();
					CharacterControllerComponent char = CharacterControllerComponent.Cast(entity.FindComponent(CharacterControllerComponent));
					float aiDistance = vector.Distance(entity.GetOrigin(), waypointPosition.GetOrigin());
					
					if ((aiDistance > metersUntilForceAIDeathWhenRetreating) || (retreatEnemyTimer >= secondsUntilForceAIDeathWhenRetreating))
					{
						char.ForceDeath();
					}
				}
				
				if (spawnedGroup.GetState() != SpawnedGroupState.RETREATING)
				{
					array<IEntity> retreatPositions = {};
					foreach (string waypointName : infantrySpawnPositions)
					{
						IEntity posEntity = FindLayoutEntity(waypointName);
						retreatPositions.Insert(posEntity);
					}
					float distance = 0;
					IEntity retreatPos = DefendHelpers.FindNearestEntity(retreatPositions, spawnedGroup.GetGroup().GetOrigin(), distance);
	
			    	string forcedMoveWaypointName = "{06E1B6EBD480C6E0}Prefabs/AI/Waypoints/AIWaypoint_ForcedMove.et";
			    	Resource moveResource = DefendHelpers.GenerateAndValidateResource(forcedMoveWaypointName);
			
			    	AIWaypoint waypoint = AIWaypoint.Cast(GetGame().SpawnEntityPrefab(moveResource, null, DefendHelpers.GenerateSpawnParamaters(retreatPos.GetOrigin())));
					
					array<AIWaypoint> waypoints = {};
					spawnedGroup.GetGroup().GetWaypoints(waypoints);
					
					foreach (AIWaypoint currWaypoint : waypoints) spawnedGroup.GetGroup().RemoveWaypoint(currWaypoint);
					
			    	spawnedGroup.GetGroup().AddWaypoint(waypoint);
					SetAIState(spawnedGroup, SpawnedGroupState.RETREATING);
				}
			}
			else retreatedGroups.Insert(spawnedGroup);
		}
		
		RemoveAIGroups(retreatedGroups);
				
		if (activeAIGroups.Count() > 0)
		{
			retreatEnemyTimer++;
			GetGame().GetCallqueue().CallLater(RetreatWave, 2000, false, null);
		}
		else
		{
			GetGame().GetCallqueue().Remove(RetreatWave);
			retreatEnemyTimer = 0;
			retreatedGroups.Clear();
		}
	}
	
	//! Executes during the mid-wave period to prepare for the next wave of enemies.
	//! -------------------------------------
	//! This method updates the player’s HUD with information about the wave's progress, 
	//! such as the time remaining before the next wave or the time allowed for building defenses.
	//! It sends hints to the player to prepare for the upcoming wave and allows building during this phase.
	protected void MidWaveTimerTick()
	{
		DefendHelpers.Log("Mid-Wave Tick", "The wave has now nearly ended.");
		SendHUDUpdate(currentWave, GetNumberOfWaves(), secondsToBuildBetweenWaves, "PREPARE YOUR DEFENSES");
		SendAllowBuilding(true);
		
		GetGame().GetCallqueue().CallLater(WaveTimerTick, secondsToBuildBetweenWaves * 1000);
	}
	
	//! Spawns a vehicle AI group at the specified position
	//! -------------------------------------
	//! Parameters:
	//! unitToSpawn - The name of the vehicle unit to spawn.
	//! spawnAt - The entity that defines the spawn position.
	//! -------------------------------------
	//! Returns:
	//! SCR_AIGroup - The spawned AI group.
	protected SpawnedEnemyGroup SpawnVehicle(string unitToSpawn, IEntity spawnAt)
	{
	    Resource resource = DefendHelpers.GenerateAndValidateResource(unitToSpawn);
	    
	    if (resource == null)
	    {
	        DefendHelpers.Log("ERROR!", "Unable to load resource for spawn group: " + unitToSpawn, LogLevel.ERROR);	
	        return null;
	    }
	    
		vector spawnPos = spawnAt.GetOrigin();
		spawnPos = spawnPos+ vector.One;
		
	    Vehicle vehicle = Vehicle.Cast(GetGame().SpawnEntityPrefab(resource, null, DefendHelpers.GenerateSpawnParamaters(spawnPos)));
	    
	    IEntity vehEntity = vehicle;
	    vehEntity.SetYawPitchRoll(spawnAt.GetYawPitchRoll());
	            
	    SpawnedEnemyGroup spawnedGroup = SpawnInfantry("{1A5F0D93609DA5DA}Prefabs/Groups/OPFOR/Group_USSR_ManeuverGroup.et", spawnAt);
	    string getInWaypointName = "{B049D4C74FBC0C4D}Prefabs/AI/Waypoints/AIWaypoint_GetInNearest.et";
	    Resource pilotResource = DefendHelpers.GenerateAndValidateResource(getInWaypointName);
	
		SetAIState(spawnedGroup, SpawnedGroupState.VEHICLE);
		
	    AIWaypoint waypoint = AIWaypoint.Cast(GetGame().SpawnEntityPrefab(pilotResource, null, DefendHelpers.GenerateSpawnParamaters(spawnAt.GetOrigin())));
		
		GetGame().GetCallqueue().Call(AddAIVehicle, vehicle);
		
	    spawnedGroup.GetGroup().AddWaypoint(waypoint);
	    return spawnedGroup;
	}
	
	//! Spawns an infantry AI group at the specified position
	//! -------------------------------------
	//! Parameters:
	//! unitToSpawn - The name of the infantry unit to spawn.
	//! spawnAt - The entity that defines the spawn position.
	//! -------------------------------------
	//! Returns:
	//! SpawnedEnemyGroup - The spawned infantry AI group.
	protected SpawnedEnemyGroup SpawnInfantry(string unitToSpawn, IEntity spawnAt)
	{
	    Resource resource = DefendHelpers.GenerateAndValidateResource(unitToSpawn);
	    
	    if (resource == null)
	    {
	        DefendHelpers.Log("ERROR!", "Unable to load resource for spawn group: " + unitToSpawn, LogLevel.ERROR);	
	        return null;
	    }
	    
	    SCR_AIGroup group = SCR_AIGroup.Cast(GetGame().SpawnEntityPrefab(resource, null, DefendHelpers.GenerateSpawnParamaters(spawnAt.GetOrigin())));		
	    
	    IEntity groupEntity = group;
	    groupEntity.SetYawPitchRoll(spawnAt.GetYawPitchRoll());
		
		SpawnedEnemyGroup spawnedGroup = new SpawnedEnemyGroup();
		
	    if (group == null)
	    {
	        DefendHelpers.Log("ERROR!", "Unable to spawn group!", LogLevel.ERROR);
	        return null;
	    }
	    else
		{
			ChimeraAIGroup groupToSave = ChimeraAIGroup.Cast(group);
			spawnedGroup.SetGroup(groupToSave);
			spawnedGroup.SetState(SpawnedGroupState.SPAWNED);
			
			AddAIGroup(spawnedGroup);
			
			DefendHelpers.Log("Group Spawned", "Group spawned...");
			return activeAIGroups.Get(activeAIGroups.Find(spawnedGroup));	
		}
	}
	
	//! Sets a waypoint for the given AI group
	//! -------------------------------------
	//! Parameters:
	//! group - The AI group to set the waypoint for.
	//! type - The type of waypoint to create (e.g., "Move" or "GetIn").
	//! target - The target position where the AI should move.
	protected void SetWaypoint(SpawnedEnemyGroup spawnedGroup, string type, vector target)
	{	
	    Resource resource = DefendHelpers.GenerateAndValidateResource(type);
	    
	    if (resource == null)
	    {
	        DefendHelpers.Log("ERROR!", "Unable to load resource for spawn group: " + spawnedGroup.GetGroup(), LogLevel.ERROR);
	        return;
	    }
	    
	    AIWaypoint waypoint = AIWaypoint.Cast(GetGame().SpawnEntityPrefab(resource, null, DefendHelpers.GenerateSpawnParamaters(target)));
	    if (waypoint == null)
	    {
	        DefendHelpers.Log("ERROR!", "Unable to spawn waypoint!", LogLevel.ERROR);
	        return;
	    }
	            
		if (spawnedGroup.GetState() == SpawnedGroupState.SPAWNED) SetAIState(spawnedGroup, SpawnedGroupState.ATTACKING);
	    spawnedGroup.GetGroup().AddWaypoint(waypoint);
	}

	
	
	
	//
	// ---\/ Config \/---
	//
	
	// HintHelpers instance, used to handle hint-related functionality in the game.
	ref HintHelpers hint = new HintHelpers(); 
	
	// HUDHelpers instance, used for managing and displaying HUD elements in the game.
	ref HUDHelpers hud = new HUDHelpers(); 
	
	// Game state flags
	protected bool gameStarted = false; // Indicates whether the game has started.
	protected bool victoryAcheivable = false; // Tracks whether a victory is currently achievable.
	protected bool extractionStarted = false; // Flag indicating if the extraction process has started.
	protected bool extractionNearby = false; // Flag that marks if the extraction point is near the players.
	protected bool extractionAvailable = false; // Flag indicating whether the extraction vehicle is available.
	protected bool extractionWaiting = false; // Flag to track if players are waiting for extraction to leave.
	protected bool extractionEnded = false; // Indicates if the extraction process has ended.
	
	
	// Methods to get the state of extraction
	bool HasExtractionStarted() return extractionStarted; // Returns true if extraction has started.
	bool IsExtractionAvailable() return extractionAvailable; // Returns true if extraction is available.
	
	
	// Reference to the extraction group, responsible for managing the extraction-related logic.
	protected ref ExtractionGroup extractionGroup = null;
	
	// Game mechanics-related variables
	protected bool _canBuild = true; // Indicates whether building (or construction) is allowed in the game.
	string currentLayout = "";
	SCR_SpawnPoint primaryLayoutSpawn;
	int currentWave = 0;  // The current wave of enemies or events.
	int livesLeft = 0;  // Tracks how many player lives are remaining.
	bool lastLife = false;
	protected bool waitingForPlayers = true; // Indicates whether the game is waiting for players to join.
	int localPlayerId = 0; // The Player ID of the local player.
	bool localPlayerAlive = false;
	protected bool isHost = false; // Indicates whether the local player is the host of the game.
	bool localPlayerInitComplete = false; // Flag indicating whether the local player has completed initialization.
	bool serverPlayerInitComplete = false; // Flag indicating whether the server has completed initialization for the player.
	ref array<ref int> activePlayerIds = {}; // An array of active player IDs in the game.
	
	
	// Managing AI groups
	ref array<ref SpawnedEnemyGroup> activeAIGroups = new array<ref SpawnedEnemyGroup>(); // An array that holds all the active AI groups in the game.
	
	void AddAIGroup(SpawnedEnemyGroup group)
	{
	    activeAIGroups.Insert(group); // Adds a new AI group to the list of active groups.
	}
	
	//! Adds an AI vehicles to the list of active AI vehicles
	//! -------------------------------------
	//! Parameters:
	//! vehicle - The AI vehicle to add to the active list.
	protected void AddAIVehicle(Vehicle vehicle)
	{
	    activeAIVehicles.Insert(vehicle);
	}
	
	array<ref SpawnedEnemyGroup> GetAIGroups()
	{
	    return activeAIGroups; // Returns the array of active AI groups.
	}
	
	void RemoveAIGroup(SpawnedEnemyGroup group)
	{
	    activeAIGroups.RemoveItem(group); // Removes a specified AI group from the list of active groups.
	}
	
	void RemoveAIGroups(array<SpawnedEnemyGroup> groups)
	{
	    foreach (SpawnedEnemyGroup groupToRemove : groups) RemoveAIGroup(groupToRemove); // Removes a list of AI groups from the active groups.
	}
	
	// Timer to track retreating enemies
	protected int retreatEnemyTimer = 0; // Timer to track how long enemies should retreat for.
	
	
	// Managing AI vehicles
	ref array<IEntity> activeAIVehicles = {}; // An array of the currently active AI vehicles in the game.
	protected int vehicleCount = 0; // Keeps track of the total number of active AI vehicles.
	
	// Jammed doors
	ref array<DoorComponent> jammedDoors = {}; // An array of doors that are currently jammed and cannot be opened or closed.
	
	const static string WB_DEFEND_CATEGORY = "Defend Manager";
	protected ref array<ref DefendPlayer> players = {};

	const static string WB_DEFEND_DEBUG = "Defend | Debug";
	[Attribute(defvalue: "0", category: WB_DEFEND_DEBUG, desc: "This is the debug mode toggle, this is used to prevent game master removal. It is also required for used debug starting wave.")]
	bool debugMode;
	[Attribute(defvalue: "0", category: WB_DEFEND_DEBUG, desc: "This is the debug feature test toggle, use this to increase time before starting the first wave.")]
	bool debugQuickWaveTest;
	[Attribute(defvalue: "0", category: WB_DEFEND_DEBUG, desc: "This is the debug feature test toggle, use this to increase time before starting and set max wave.")]
	bool debugExtractionFeatureTest;
	[Attribute(defvalue: "0", category: WB_DEFEND_DEBUG, desc: "This is the debug feature test toggle, use this to increase time before starting and force a vehicle to spawn.")]
	bool debugVehicleFeatureTest;
	[Attribute(defvalue: "1", category: WB_DEFEND_DEBUG, desc: "This will prevent the local player from taking any damage. (requires debugMode = true)")]
	bool allowPlayerDamage;
	[Attribute(defvalue: "", UIWidgets.EditBox, category: WB_DEFEND_DEBUG, desc: "Force a layout to use.")]
	string forcedLayoutName;
	[Attribute(defvalue: "0", UIWidgets.Slider, category: WB_DEFEND_DEBUG, "0 999 1", desc: "This is will start the wave to start in. (requires debugMode = true | default = 0)")]
	int debugStartingWave;
	[Attribute(defvalue: "0", category: WB_DEFEND_DEBUG, desc: "Should log the server loop?")]
	bool serverLoopDebugLogging;
	[Attribute(defvalue: "0", category: WB_DEFEND_DEBUG, desc: "Should log the player loop?")]
	bool playerLoopDebugLogging;
	[Attribute(defvalue: "0", category: WB_DEFEND_DEBUG, desc: "Should log the timer loop?")]
	bool timerLoopLogging;
	[Attribute(defvalue: "0", category: WB_DEFEND_DEBUG, desc: "Should log hints?")]
	bool hintLogging;
	[Attribute(defvalue: "0", category: WB_DEFEND_DEBUG, desc: "Should log RPC?")]
	bool rpcLogging;

	const static string WB_DEFEND_LOOPS = "Defend | Loops";
	[Attribute(defvalue: "1", UIWidgets.Slider, category: WB_DEFEND_LOOPS, "1 100 1", desc: "Interval in seconds when the server loop will run.")]
	int serverLoopIntervalSeconds;
	[Attribute(defvalue: "60", UIWidgets.Slider, category: WB_DEFEND_LOOPS, "1 100 1", desc: "Interval in seconds when the AI loop will run.")]
	int aiLoopIntervalSeconds;
	[Attribute(defvalue: "5", UIWidgets.Slider, category: WB_DEFEND_LOOPS, "1 100 1", desc: "Interval in seconds when the player loop will run.")]
	int playerLoopIntervalSeconds;
	
	const static string WB_DEFEND_GAMEPLAY = "Defend | Gameplay";
	[Attribute(defvalue: "3", UIWidgets.Slider, category: WB_DEFEND_GAMEPLAY, "1 999 1", desc: "Total number of waves until extraction arrives (overridden in endless mode)")]
	protected int numberOfWaves;
	int GetNumberOfWaves()
	{
		if (endlessMode) return 0;
		else return numberOfWaves;
	}
	[Attribute(defvalue: "5", UIWidgets.Slider, category: WB_DEFEND_GAMEPLAY, "0 100 1", desc: "Total number of team lives.")]
	int numberOfLives;
	[Attribute(defvalue: "0", category: WB_DEFEND_GAMEPLAY, desc: "Is endless waves mode?")]
	bool endlessMode;
	[Attribute(defvalue: "0", category: WB_DEFEND_GAMEPLAY, desc: "Should we multiply the number of enemies spawned by the number of players alive?")]
	bool balenceEnemies;
	[Attribute(defvalue: "3", UIWidgets.Slider, category: WB_DEFEND_GAMEPLAY, "0 100 1", desc: "How many enemies are required before enemies retreat.")]
	int enemiesLeftToFinishWave;
	[Attribute(defvalue: "100", UIWidgets.Slider, category: WB_DEFEND_GAMEPLAY, "10 200 1", desc: "How many meters do the enemies need to be before being cleaned up.")]
	int metersUntilForceAIDeathWhenRetreating;
	[Attribute(uiwidget: UIWidgets.Auto, category: WB_DEFEND_GAMEPLAY, desc: "Names of the folders of each layout.")]
	ref array<ref string> layoutLayerNames;
	[Attribute(defvalue: "0", category: WB_DEFEND_GAMEPLAY, desc: "Zombies?")]
	bool zombiesMode;
	
	const static string WB_DEFEND_BUILDING = "Defend | Building";
	[Attribute(defvalue: "1", category: WB_DEFEND_BUILDING, desc: "Allow player building")]
	bool allowBuilding;
	[Attribute(defvalue: "1", category: WB_DEFEND_BUILDING, desc: "If toggled players will be able during waves. (when enemies are approaching)")]
	bool restrictBuildingDuringWaves;
	[Attribute(defvalue: "1", category: WB_DEFEND_BUILDING, desc: "If toggled players will not recieved time between waves, instead waves will be timed.")]
	bool allowBuildingTimeBetweenWaves;
	
	const static string WB_DEFEND_PLAYERS = "Defend | Players";
	[Attribute(defvalue: "1", UIWidgets.Slider, category: WB_DEFEND_PLAYERS, "1 10 1", desc: "How many points per kill does a player recieve.")]
	int pointsPerKill;
	[Attribute(defvalue: "1", category: WB_DEFEND_PLAYERS, desc: "Should warn then kill deserters?")]
	bool punishDeserters;
	[Attribute(defvalue: "5", UIWidgets.Slider, category: WB_DEFEND_PLAYERS, "1 50 1", desc: "How many warnings to give deserters before killing them.")]
	int numberOfWarningsForDeserters;
	[Attribute(defvalue: "100", UIWidgets.Slider, category: WB_DEFEND_PLAYERS, "50 200 1", desc: "How far from the base should player be allowed to go before becoming deserters.")]
	int distanceToPunishDeserters;
	
	const static string WB_DEFEND_VEHICLES = "Defend | Vehicles";
	[Attribute(defvalue: "0", category: WB_DEFEND_VEHICLES, desc: "Allow the spawning of vehicles")]
	bool allowVehicles;
	[Attribute(defvalue: "1", UIWidgets.Slider, category: WB_DEFEND_VEHICLES, "1 10 1", desc: "The maximum number of vehicles allowed to spawn.")]
	int maxVehicles;
	[Attribute(defvalue: "4", UIWidgets.Slider, category: WB_DEFEND_VEHICLES, "1 100 1", desc: "The 1/X odds of spawning a vehicle per wave.")]
	int oddsOfVehicleSpawn;
	
	const static string WB_DEFEND_INFANTRY = "Defend | Infantry";
	[Attribute(defvalue: "3", UIWidgets.Slider, category: WB_DEFEND_INFANTRY, "1 30 1", desc: "The maximum number of enemy groups to spawn per wave.")]
	int numberOfEnemiesPerWave;
	[Attribute(defvalue: "50", UIWidgets.Slider, category: WB_DEFEND_INFANTRY, "10 150 1", desc: "The absolute maximum total number of GROUPS allowed to be spawned per wave.")]
	int maxAIGroupsPerWave;
	
	const static string WB_DEFEND_TIMING = "Defend | Timing";
	[Attribute(defvalue: "60", UIWidgets.Slider, category: WB_DEFEND_TIMING, "10 20 1", desc: "Seconds before starting the Defend Manager.")]
	int secondsBeforeStarting;
	[Attribute(defvalue: "30", UIWidgets.Slider, category: WB_DEFEND_TIMING, "10 300 1", desc: "Seconds before the first wave is spawned to prepare.")]
	int secondsBeforeFirstWaveToPrepare;
	[Attribute(defvalue: "1", UIWidgets.Slider, category: WB_DEFEND_TIMING, "1 5 1", desc: "Seconds between/before enemie groups are spawned per wave.")]
	int secondsBeforeSpawnEnemy;
	[Attribute(defvalue: "120", UIWidgets.Slider, category: WB_DEFEND_TIMING, "30 300 1", desc: "Seconds before enemy reinforcements are spawned. (overridden by allowBuildingTimeBetweenWaves = true)")]
	int secondsBeforeReinforcements;
	[Attribute(defvalue: "120", UIWidgets.Slider, category: WB_DEFEND_TIMING, "30 300 1", desc: "Seconds to prepare/build between waves. (overridden by allowBuildingTimeBetweenWaves = false)")]
	int secondsToBuildBetweenWaves;
	[Attribute(defvalue: "60", UIWidgets.Slider, category: WB_DEFEND_TIMING, "1 999 1", desc: "Seconds before forcing AI to die after they begin retreating.")]
	int secondsUntilForceAIDeathWhenRetreating;
	[Attribute(defvalue: "30", UIWidgets.Slider, category: WB_DEFEND_TIMING, "1 300 1", desc: "Seconds until extraction arrives in the AO.")]
	int secondsUntilExtractionArrives;
	[Attribute(defvalue: "30", UIWidgets.Slider, category: WB_DEFEND_TIMING, "1 300 1", desc: "Seconds the extraction will wait once it arrives at the base.")]
	int secondsUntilExtractionLeaves;
	
	const static string WB_DEFEND_SPAWNGROUPS = "Defend | Spawn Groups";
	[Attribute(uiwidget: UIWidgets.Auto, category: WB_DEFEND_SPAWNGROUPS, desc: "Infantry resource names for the OPFOR infantry to randomly spawn for each wave.")]
	ref array<ref string> spawnGroupsInf;

	[Attribute(uiwidget: UIWidgets.Auto, category: WB_DEFEND_SPAWNGROUPS, desc: "Infantry resource names for the OPFOR infantry to randomly spawn for each wave.")]
	ref array<ref string> spawnGroupsZom;
	
	[Attribute(uiwidget: UIWidgets.Auto, category: WB_DEFEND_SPAWNGROUPS, desc: "Vehicle resource names for the OPFOR vehicles to randomly spawn for each wave.")]
	ref array<ref string> spawnGroupsVeh;
	
	const static string WB_DEFEND_RESOURCES = "Defend | Resources";
	[Attribute(defvalue: "{DF63ECA1E31B61B1}UI/Layouts/Modded/HUD.layout", UIWidgets.EditBox, category: WB_DEFEND_RESOURCES, desc: "The HUD UI Layout")]
	string uiHUDLayout;
	[Attribute(defvalue: "{750A8D1695BD6998}Prefabs/AI/Waypoints/AIWaypoint_Move.et", UIWidgets.EditBox, category: WB_DEFEND_RESOURCES, desc: "The move waypoint for AI.")]
	string moveWaypointType;
	[Attribute(defvalue: "{06E1B6EBD480C6E0}Prefabs/AI/Waypoints/AIWaypoint_ForcedMove.et", UIWidgets.EditBox, category: WB_DEFEND_RESOURCES, desc: "The move waypoint for AI.")]
	string forceMoveWaypointType;
	[Attribute(defvalue: "{22A875E30470BD4F}Prefabs/AI/Waypoints/AIWaypoint_Patrol.et", UIWidgets.EditBox, category: WB_DEFEND_RESOURCES, desc: "The defend/patrol waypoint for AI.")]
	string patrolWaypointType;
	[Attribute(uiwidget: UIWidgets.Auto, category: WB_DEFEND_RESOURCES, desc: "Vehicle resources names to use when picking the extraction vehicle.")]
	ref array<ref string> extractionVehicleTypes;
	
	const static string WB_DEFEND_MARKERNAMES = "Defend | Marker Names";
	[Attribute(uiwidget: UIWidgets.Auto, category: WB_DEFEND_MARKERNAMES, desc: "The names of the extraction markers")]
	ref array<ref string> extractionMarkerPositions;
	[Attribute(uiwidget: UIWidgets.Auto, category: WB_DEFEND_MARKERNAMES, desc: "The names of the enemy infantry spawn markers")]
	ref array<ref string> infantrySpawnPositions;
	[Attribute(uiwidget: UIWidgets.Auto, category: WB_DEFEND_MARKERNAMES, desc: "The names of the enemy vehicle spawn markers")]
	ref array<ref string> vehicleSpawnPositions;
	[Attribute(defvalue: "ussr_waypoint", UIWidgets.EditBox, category: WB_DEFEND_MARKERNAMES, desc: "The name of the AI waypoint marker")]
	string waypointPositionName;
	[Attribute(defvalue: "ussr_veh_waypoint", UIWidgets.EditBox, category: WB_DEFEND_MARKERNAMES, desc: "The name of the AI vehicle waypoint marker")]
	string vehWaypointPositionName;
}