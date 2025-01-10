class HUDHelpers
{
	protected Widget hudRoot;
	protected RichTextWidget waveTextWidget;
	protected RichTextWidget timerTextWidget;
	protected RichTextWidget enemiesTextWidget;
	protected ImageWidget separatorWidget;

	protected bool initComplete = false;
	protected int widgetId = 0;
	protected DefendManager dm = null;
	
	bool hasInitComplete()
	{
		return initComplete;
	}

	
 	void Init(string uiHUDLayout, bool isDebug) 
	{ 		
		DefendHelpers.Log("Init HUD", "Starting HUD");
		hudRoot = GetGame().GetWorkspace().CreateWidgets(uiHUDLayout);
		timerDebug = isDebug;
		dm = DefendHelpers.Get();
      
		waveTextWidget = RichTextWidget.Cast(hudRoot.FindAnyWidget("Wave"));
		timerTextWidget = RichTextWidget.Cast(hudRoot.FindAnyWidget("Timer"));
		enemiesTextWidget = RichTextWidget.Cast(hudRoot.FindAnyWidget("Enemies"));
		separatorWidget = ImageWidget.Cast(hudRoot.FindAnyWidget("Separator"));

		if (waveTextWidget == null) DefendHelpers.Log("Error", "Failed to find the HUD wave Text!");
		if (timerTextWidget == null) DefendHelpers.Log("Error", "Failed to find the HUD timer Text!");
		if (enemiesTextWidget == null) DefendHelpers.Log("Error", "Failed to find the HUD enemies Text!");
		if (separatorWidget == null) DefendHelpers.Log("Error", "Failed to find the HUD separator Widget!");

		initComplete = true;
	} 
	
	protected string FormatWaves(int currWave, int maxWave)
	{
		string maxWaveText = "";
		if (maxWave != 0) maxWaveText = "/" + maxWave.ToString();
		string textToShow = "WAVE " + currWave.ToString() + maxWaveText;
		return textToShow;
	}
	
	protected string FormatTimer(int timerSeconds)
	{
		if (timerSeconds > 0)
		{
			string textToShow = "Time until next wave: " + timerSeconds + " seconds...";
			return textToShow;	
		}
		else
		{
			return "";
		}
	}
	
	protected string FormatEnemies(int totalEnemies)
	{
		if (totalEnemies != 0)
		{
			string textToShow = "Enemies remaining: " + totalEnemies;
			return textToShow;
		}
		else
		{
			return "";
		}
	}
	
	void Destroy()
	{ 	
		if (!initComplete)
			return;
		
		timerActive = false;
		timerTimeLeft = -1;
		shouldStartTimer = false;
		initComplete = false;
		GetGame().GetCallqueue().Clear();
		hudRoot.RemoveFromHierarchy();
	}
	
	void ShowHUD(int currWave, int maxWave, int timerSeconds = 0, int totalEnemies = 0, string customText = "")
	{
		if (!initComplete)
			return;
		
		if (DefendHelpers.IsHost()) totalEnemies = dm.CountAliveAI();
		DefendHelpers.Log("Showing HUD", "Wave: " + currWave + "/" + maxWave + " | Timer: " + timerSeconds + " | Custom Text: " + customText + " | Total Enemies: " + totalEnemies);
		
		if (timerActive)
		{
			timerTimeLeft = timerSeconds;
			timerActive = false;
			GetGame().GetCallqueue().RemoveByName(this, "StartTimer");
			GetGame().GetCallqueue().RemoveByName(this, "TimerTick");
		}
		
		if (currWave != 0)
		{
			if (customText == "") 
				waveTextWidget.SetText(FormatWaves(currWave, maxWave));
			else
				waveTextWidget.SetText(customText);

			timerTextWidget.SetText(FormatTimer(timerSeconds));
			enemiesTextWidget.SetText(FormatEnemies(totalEnemies));
			
			DefendHelpers.Log("HUD Text Set", "Wave text set.");
			
			if (timerSeconds != 0 || totalEnemies != 0 && DefendHelpers.IsHost())
			{
				if (timerTimeLeft == 0) timerTimeLeft = -1;
				
				shouldStartTimer = true;
				GetGame().GetCallqueue().Call(StartTimer, timerSeconds, totalEnemies, currWave, maxWave);
				DefendHelpers.Log("HUD Started timer", "Wave timer text set.");
			}
		}
		else
		{
			if (customText == "")
				waveTextWidget.SetText("PREPARE YOUR DEFENSES!");
			else
				waveTextWidget.SetText(customText);
			
			GetGame().GetCallqueue().RemoveByName(this, "StartTimer");
			GetGame().GetCallqueue().RemoveByName(this, "TimerTick");
			
			timerTimeLeft = -1;
			timerActive = false;
			shouldStartTimer = false;
			
			DefendHelpers.Log("HUD Text Set", "Prepare defense text set.");
		}
		
		DefendHelpers.Log("HUD Set Visible", "The HUD should now be visible.");
	}
	
	int timerTimeLeft = -1;
	bool timerActive = false;
	bool shouldStartTimer = false;
	bool timerDebug = false;
	
	void StartTimer(int time, int enemies, int currWave, int maxWave)
	{
		if (timerDebug) DefendHelpers.Log("Timer active: " + timerActive, "HUD timer is " + timerActive + " with " + timerTimeLeft + " seconds left & " + enemies + " enemies remaining");
		if (timerTimeLeft == -1 && !timerActive && shouldStartTimer)
		{
			if (timerDebug) DefendHelpers.Log("Started HUD timer", "HUD timer will start ticking.");
			GetGame().GetCallqueue().RemoveByName(this, "StartTimer");
			GetGame().GetCallqueue().RemoveByName(this, "TimerTick");
			
			shouldStartTimer = false;
			timerActive = true;
			timerTimeLeft = time;
		}
		else if (timerTimeLeft <= 0 && timerActive && enemies <= 0)
		{
			if (timerDebug) DefendHelpers.Log("Stopped HUD timer", "HUD timer will no longer tick.");
			GetGame().GetCallqueue().RemoveByName(this, "TimerTick");
			timerActive = false;
			timerTimeLeft = -1;
			
			waveTextWidget.SetText("");
			timerTextWidget.SetText("");
			enemiesTextWidget.SetText("");
		}
		
		if (timerActive && timerTimeLeft > 0 || enemies > 0)
		{
			if (timerDebug) DefendHelpers.Log("HUD Timer pre-tick", "Waiting for 1 second tick...");
			GetGame().GetCallqueue().CallLater(TimerTick, 1000, false, time, enemies, currWave, maxWave);
			return;
		}
	}
	
	void TimerTick(int initialTime, int initalEnemiesLeft, int currWave, int maxWave)
	{
		if (timerDebug) DefendHelpers.Log("HUD Timer Tick", "HUD timer ticked.");
		if (timerTimeLeft > 0) timerTimeLeft = timerTimeLeft - 1;
		
		waveTextWidget.SetText(FormatWaves(currWave, maxWave));
		timerTextWidget.SetText(FormatTimer(timerTimeLeft));
		if (timerTextWidget.GetText() == "")
		{
			timerTextWidget.SetText(FormatEnemies(dm.CountAliveAI()));
			enemiesTextWidget.SetText("");
		}
		else enemiesTextWidget.SetText(FormatEnemies(dm.CountAliveAI()));
		
		if (DefendHelpers.IsHost()) dm.SendHUDUpdate(currWave, maxWave, timerTimeLeft, "", 0);
		
		GetGame().GetCallqueue().Call(StartTimer, initialTime, initalEnemiesLeft, currWave, maxWave);
	}
}

class HintHelpers
{
	SCR_HintManagerComponent hintComponent = null;
	
	void ShowHint(string header, string message, int secondsToShow = 10, bool showTimer = true)
	{
		DefendHelpers.Log("Presenting Hint", "Presenting hint: " + header + " | " + message + " | " + secondsToShow + " secs");
		hintComponent = SCR_HintManagerComponent.GetInstance();
		
		EHint type = EHint.UNDEFINED;

		SCR_HintUIInfo hintInfo = SCR_HintUIInfo.CreateInfo(message, header, secondsToShow, type, EFieldManualEntryId.NONE, showTimer);
		
		if (hintComponent != null)
		{
			if (hintComponent.GetCurrentHint() != null)
			{
				bool canHide = hintComponent.Hide();
				if (!canHide)
				{
					DefendHelpers.Log("ERROR", "Couldn't hide current Hint for new one.");
				}
			}
			
			bool hintShown = hintComponent.ShowHint(hintInfo);
			if (hintShown) DefendHelpers.Log("Hint Shown", "You should have seen the hint by now.");
			else DefendHelpers.Log("ERROR", "Couldn't show hint!");
		}
		else 
		{
			DefendHelpers.Log("ERROR", "No Hint Component to show?");
		}
	}
}

class DefendPlayer
{	
	string name = "";
	int id = "";
	int kills = 0;
	int deaths = 0;
	bool alive = false;
	
	void AddKills(int killsToAdd)
	{
		this.kills += killsToAdd;
	}
	
	void AddDeaths(int deathsToAdd)
	{
		this.deaths += deathsToAdd;
	}
}


class DefendHelpers
{
	static bool IsHost()
	{
		SCR_BaseGameMode gm = SCR_BaseGameMode.Cast(GetGame().GetGameMode());
		if (gm != null)
		{
			return gm.IsMaster();	
		}
		else return false;
	}
	
	static DefendManager Get()
	{
		IEntity defendManagerEntity = GetGame().GetWorld().FindEntityByName("DefendManager");
		if (defendManagerEntity != null)
		{
			DefendManager defendManager = DefendManager.Cast(defendManagerEntity);
			if (defendManager == null)
			{
				Print("Error casting Defend Manager!", LogLevel.ERROR);
				return null;
			}
			else
				return defendManager;
		}
		else
		{
			Print("Error finding Defend Manager!", LogLevel.ERROR);
			return null;
		}
	}
	
	static void Log(string header, string message, LogLevel level = LogLevel.NORMAL)
	{
		string timeStamp = SCR_DateTimeHelper.GetTimeUTC();
		Print("[" + timeStamp + "] Defend Arland: " + header + " - "+ message, level);
	}
	
	static int GenerateRandom(int min, int max)
    {
		int value = Math.RandomInt(min, max);
		return value;
    }
	
	static EntitySpawnParams GenerateSpawnParamaters(vector position)
	{
		EntitySpawnParams params = EntitySpawnParams();
		params.TransformMode = ETransformMode.WORLD;
		
		params.Transform[3] = position;
		return params;
	}
	
	
	static Resource GenerateAndValidateResource(string resourceToLoad)
	{
		Resource resource = Resource.Load(resourceToLoad);
		if (!resource.IsValid())
		{
			Log("ERROR!", "Unable to load resource: " + resourceToLoad, LogLevel.ERROR);
			return null;
		}
		return resource;
	}
}

class DefendManagerClass: GenericEntityClass
{
	
}

class DefendManager: GenericEntity
{
	void RegisterDeath(int deadID)
	{
		if (DefendHelpers.IsHost())
		{
			DefendPlayer player = GetPlayer(deadID);
			player.AddDeaths(1);
			livesLeft = (livesLeft - 1);
		
			DefendHelpers.Log("Player died", "Player " + player.name + " (" + player.id.ToString() + ") died (total deaths: " + player.deaths + ", lives remaining: " + livesLeft + ")");
		}
	}
	
	void RegisterKill(int killerID, int kills)
	{
		if (DefendHelpers.IsHost())
		{
			DefendPlayer player = GetPlayer(killerID);
			player.AddKills(kills);
		
			DefendHelpers.Log("Player got a kill", "Player " + player.name + " (" + player.id.ToString() + ") killed someone (total kills: " + player.kills + " )");
		}
	}
	
	void RegisterAIDeath(IEntity deadAI)
	{
		if (DefendHelpers.IsHost())
		{
			
		}
	}
	
	void SetPlayerState(int playerID, bool alive)
	{
		if (DefendHelpers.IsHost())
		{
			DefendPlayer player = GetPlayer(playerID);
			DefendHelpers.Log("Player State Change", "Player " + player.name + " is alive: " + alive + " (oldState: " + player.alive + ")");

			player.alive = alive;
		}
	}
	
	ref array<DefendPlayer> GetAlivePlayers()
	{
		ref array<DefendPlayer> alivePlayers = {};
		for (int i; i < players.Count(); i++)
		{
			DefendPlayer _currPlayer = players[i];
			if (_currPlayer.alive) alivePlayers.Insert(players[i]);
		}
		return alivePlayers;
	} 
	
	void AddPlayer(DefendPlayer player)
	{
		DefendHelpers.Log("Player joined", "Player " + player.name + " joined (" + player.id + ")");
		players.Insert(player);
	}
	
	void RemovePlayer(int playerID)
	{
		for (int i; i < players.Count(); i++)
		{
			DefendPlayer _currPlayer = players[i];
			if (_currPlayer.id == playerID) 
			{
				players.Remove(i);
				activePlayerIds.Remove(i);
				
				DefendHelpers.Log("Player left", "Player " + _currPlayer.name + " left (" + _currPlayer.id + ")");
				DefendHelpers.Log("PlayerCount", "Players: " + players.Count() + " | Registered: " + activePlayerIds.Count());
			}
		}
	}
	
	ref DefendPlayer GetPlayer(int id)
	{
		DefendPlayer playerToReturn = null;
		for (int i; i < players.Count(); i++)
		{
			DefendPlayer _currPlayer = players[i];
			if (_currPlayer.id == id) playerToReturn = players[i];
		}
		return playerToReturn;
	}
	
	protected void EndGame(EGameOverTypes type, int winnerId = 1)
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
	
	override void EOnActivate(IEntity owner)
	{
		InitMission();
	}
	
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
	
	void InitMission()
	{
		DefendHelpers.Log("Init Mission", "Init called for mission");
				
		if (DefendHelpers.IsHost())
		{
			DefendHelpers.Log("Is Host", "Is currently the game host.");
			isHost = true;
			localPlayerId = -1;
			
			hud.Init(uiHUDLayout, debugMode);
			
			GetGame().GetCallqueue().Call(WaitForPlayers);
			GetGame().GetCallqueue().CallLater(ServerUpdateLoop, serverLoopIntervalSeconds * 1000, true);
			serverPlayerInitComplete = true;
		}
		else
		{
			DefendHelpers.Log("Not Host", "Not currently the game host.");
		}
		
		GetGame().GetCallqueue().CallLater(PlayerUpdateLoop, playerLoopIntervalSeconds * 1000, true);
	}
	
	void InitPlayerLocal(int playerId)
	{
		DefendHelpers.Log("Init Player", "Init called for player id: " + playerId);
		localPlayerId = playerId;
				
		localPlayerInitComplete = true;
	}
	
	protected void ServerUpdateLoop()
	{
		if (!waitingForPlayers && gameStarted)
		{
			if (serverLoopDebugLogging) DefendHelpers.Log("Running Server Loop", "Server loop running.");
			if (livesLeft < 1)
			{
				EndGame(EGameOverTypes.LOOSE, 1);
			}
			
			if (CountAliveAI() > 0)
			{
				if (!checkingAI && !checkAIQueued)
				{
					checkAIQueued = true;
					GetGame().GetCallqueue().CallLater(CheckAI, aiLoopIntervalSeconds * 1000, false);		
				}
			}
			else DefendHelpers.Log("No AI Alive", "No AI are alive right now.");
			
			if (currentWave == numberOfWaves && !victoryAcheivable)
			{
				victoryAcheivable = true;
				//Do victory
			}
		}
	}
	
	protected void PlayerUpdateLoop()
	{
		if (playerLoopDebugLogging) DefendHelpers.Log("Running Player Loop", "Player loop running.");
		PlayerController myController = GetGame().GetPlayerController();

		if (myController != null)
		{
			CheckIfDeserting(myController);
		}
		else if (playerLoopDebugLogging) DefendHelpers.Log("No Player Controller", "The player doesnt currently have a player controller.");
	}
	
	int deserterHurtMultipler = 1;
	void CheckIfDeserting(PlayerController myController)
	{
		IEntity waypointPosition = GetGame().GetWorld().FindEntityByName(waypointPositionName);
		vector waypointPos = waypointPosition.GetOrigin();
		
		IEntity myEntity = myController.GetControlledEntity();
		if (myEntity != null)
		{
			vector myPosition = myEntity.GetOrigin();
				
			float myDistance = vector.Distance(myPosition, waypointPos);
			ChimeraCharacter myChar = ChimeraCharacter.Cast(myEntity);

			
			if (myDistance > 90)
			{
				DefendHelpers.Log("Player Deserting", "A player is deserting!");
				if (myChar != null)
				{
					DamageManagerComponent dmgManager = myChar.GetDamageManager();
					
					if (dmgManager.IsDamageHandlingEnabled())
					{
						myChar.GetCharacterController().SetMovement(myChar.GetCharacterController().GetMovementSpeed() - (0.05 * deserterHurtMultipler), vector.Zero);
						dmgManager.SetHealthScaled(dmgManager.GetHealthScaled() - (0.1 * deserterHurtMultipler));
						deserterHurtMultipler++;
						hint.ShowHint("Deserting!", "You have left the area of operation, this will not be tolorated and you're currently losing health!", playerLoopIntervalSeconds);
					}
				}
			}
			else if (myDistance > 75)
			{
				hint.ShowHint("Where are you going?", "You're leaving the area of operation turn back now or be labeled a deserter!", playerLoopIntervalSeconds);
			}
			else
			{
				if (myChar != null)
					myChar.GetCharacterController().SetMovement(1, vector.Zero);
				deserterHurtMultipler = 1;
			}	
		}
		else DefendHelpers.Log("No Player Entity", "The player currently has no entity attached to it.");
	}
	
	
	bool checkAIQueued = false;
	bool checkingAI = false;
	protected void CheckAI()
	{
		checkingAI = true;
		checkAIQueued = false;
		DefendHelpers.Log("Running Server AI Loop", "Server AI loop running to check AI.");
		
		CheckAIAlive();
		RemindAI();
		
		checkingAI = false;
	}
	
	void CheckAIAlive()
	{
		for (int aiGroupInd; aiGroupInd < activeAIGroups.Count(); aiGroupInd++)
		{
			IEntity groupEntity = activeAIGroups[aiGroupInd];
			SCR_AIGroup currGroup = activeAIGroups[aiGroupInd];
			
			int agentCount = currGroup.GetAgentsCount();
			int maxAgents = currGroup.GetMaxMembers();	
			
			if (currGroup.IsSlave())
			{
				DefendHelpers.Log("AI Group Is Slave", "AI Group Is a Slave");
				return;
			}

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
			
			DefendHelpers.Log("Checking group", "Group " + (aiGroupInd+1) + "/" + activeAIGroups.Count() + " and has " + characters.Count() + " characters");
			
			for (int charInd; charInd < characters.Count(); charInd++)
			{	
				SCR_ChimeraCharacter currChar = characters[charInd];
				ChimeraCharacter character = currChar;
				SCR_DamageManagerComponent dmgComp = character.GetDamageManager();
				float health = dmgComp.GetHealthScaled();
				if (health <= 0) countDeadMembers++; 
			}
			
			if (countDeadMembers == characters.Count()) activeAIGroups.RemoveItem(currGroup);
		}
		
		DefendHelpers.Log("Total AI Alive: ", CountAliveAI().ToString());
	}
	
	void RemindAI()
	{
		foreach(int i, SCR_AIGroup group : activeAIGroups)
		{
			DefendHelpers.Log("Reminding AI (" + (i+1).ToString() + "/" + activeAIGroups.Count() + ")", "Remind AI that they're supposed to be doing something...");

			array<AIWaypoint> waypoints = {};
			group.GetWaypoints(waypoints);
			IEntity waypointPosition = GetGame().GetWorld().FindEntityByName(waypointPositionName);
			bool inVeh = false;
			
			array<AIAgent> groupAgents = {};
			group.GetAgents(groupAgents);

			foreach (AIAgent agent : groupAgents)
			{
				SCR_ChimeraCharacter char = SCR_ChimeraCharacter.Cast(agent.GetControlledEntity());
				if (char != null)
				{
					if (char.IsInVehicle()) 
						inVeh = true;
				}
			}
			
			
			if (!inVeh)
			{
				if (waypoints.Count() == 0)
				{
					DefendHelpers.Log("AI isn't busy", "AI #" + (i+1).ToString() + " didn't have anything to do!");
					float distance = vector.Distance(group.GetLeaderEntity().GetOrigin(), waypointPosition.GetOrigin());
					DefendHelpers.Log("AI isn't busy", "AI #" + (i+1).ToString() + " didn't have anything to do and is " + distance + " away from obj!");
	
					
					if (distance < 10 && !inVeh)
					{
						DefendHelpers.Log("AI is within their waypoint", "AI is within their waypoint radius.");
						SetWaypoint(group, patrolWaypointType, waypointPosition.GetOrigin());
					}
					else
					{
						SetWaypoint(group, moveWaypointType, waypointPosition.GetOrigin());
					}
				}
				else 
				{
					AIWaypoint currWaypoint = waypoints[0];
					if (currWaypoint.IsWithinCompletionRadius(group))
					{
						DefendHelpers.Log("AI is within their waypoint", "AI #" + (i+1).ToString() + " is busy but within their waypoint location");
					
						SetWaypoint(group, patrolWaypointType, waypointPosition.GetOrigin());
					}
					else
					{
						DefendHelpers.Log("AI is busy", "AI #" + (i+1).ToString() + " is busy going to a waypoint");
						GetGame().GetCallqueue().CallLater(CheckRemindAI, 20000, false, group, currWaypoint);
					}
				}
			}
		}
	}
	
	void CheckRemindAI(SCR_AIGroup group, AIWaypoint waypoint)
	{
		array<AIWaypoint> waypoints = {};
		if (group == null)
			return;
		
		group.GetWaypoints(waypoints);
		
		if (waypoints.Count() > 0)
		{
			AIWaypoint currWaypoint = waypoints[0];
			if (currWaypoint == waypoint)
			{
				DefendHelpers.Log("AI Took too long", "AI took too long so resetting its waypoint.");
				foreach (AIWaypoint waypointToDelete : waypoints)
				{
					group.RemoveWaypoint(waypointToDelete);
				}
				
				if (currWaypoint.IsWithinCompletionRadius(group))
				{
					DefendHelpers.Log("AI is within their waypoint", "AI is within their waypoint radius.");
					IEntity waypointPosition = GetGame().GetWorld().FindEntityByName(waypointPositionName);
					SetWaypoint(group, patrolWaypointType, waypointPosition.GetOrigin());
				}
			}
		}
	}
	
	int CountAliveAI()
	{
		int totalAIAlive = 0;
		array<AIAgent> agents = {};
		array<SCR_ChimeraCharacter> characters = {};

		GetGame().GetAIWorld().GetAIAgents(agents);
		
		foreach (int i, AIAgent agent : agents)
		{
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

	protected bool gameStarted = false;
	protected bool victoryAcheivable = false;
	int currentWave = 0;
	int livesLeft = 0;
	protected bool waitingForPlayers = true;
	int localPlayerId = 0;
	protected bool isHost = false;
	bool localPlayerInitComplete = false;
	bool serverPlayerInitComplete = false;
	ref array<int> activePlayerIds = {};
	ref array<SCR_AIGroup> activeAIGroups = {};
	protected int vehicleCount = 0;
	ref array<DoorComponent> jammedDoors = {};
	
	protected bool _canBuild = true;
	bool CanBuild()
	{
		if (debugMode) return true;
		else return _canBuild;
	}
	
	void SetCanBuild(bool allowed)
	{
		_canBuild = allowed;
	}
	
 	ref HintHelpers hint = new HintHelpers();
	ref HUDHelpers hud = new HUDHelpers();
	const static string WB_DEFEND_CATEGORY = "Defend Manager";
	protected ref array<ref DefendPlayer> players = {};

	const static string WB_DEFEND_DEBUG = "Defend | Debug";
	[Attribute(defvalue: "0", category: WB_DEFEND_DEBUG)]
	bool debugMode;
	[Attribute(defvalue: "0", category: WB_DEFEND_DEBUG)]
	bool serverLoopDebugLogging;
	[Attribute(defvalue: "0", category: WB_DEFEND_DEBUG)]
	bool playerLoopDebugLogging;
	
	const static string WB_DEFEND_LOOPS = "Defend | Loops";
	[Attribute(defvalue: "1", UIWidgets.Slider, category: WB_DEFEND_LOOPS, "1 100 1")]
	int serverLoopIntervalSeconds;
	[Attribute(defvalue: "60", UIWidgets.Slider, category: WB_DEFEND_LOOPS, "1 100 1")]
	int aiLoopIntervalSeconds;
	[Attribute(defvalue: "5", UIWidgets.Slider, category: WB_DEFEND_LOOPS, "1 100 1")]
	int playerLoopIntervalSeconds;
	
	const static string WB_DEFEND_GAMEPLAY = "Defend | Gameplay";
	[Attribute(defvalue: "3", UIWidgets.Slider, category: WB_DEFEND_GAMEPLAY, "1 10 1")]
	int numberOfWaves;
	int GetNumberOfWaves()
	{
		if (endlessMode) return 0;
		else return numberOfWaves;
	}
	[Attribute(defvalue: "5", UIWidgets.Slider, category: WB_DEFEND_GAMEPLAY, "0 100 1")]
	int numberOfLives;
	[Attribute(defvalue: "0", category: WB_DEFEND_GAMEPLAY)]
	bool endlessMode;
	[Attribute(defvalue: "0", category: WB_DEFEND_GAMEPLAY)]
	bool balenceEnemies;
	
	const static string WB_DEFEND_VEHICLES = "Defend | Vehicles";
	[Attribute(defvalue: "0", category: WB_DEFEND_VEHICLES)]
	bool allowVehicles;
	[Attribute(defvalue: "1", UIWidgets.Slider, category: WB_DEFEND_VEHICLES, "1 10 1")]
	int maxVehicles;
	[Attribute(defvalue: "4", UIWidgets.Slider, category: WB_DEFEND_VEHICLES, "1 10 1")]
	int oddsOfVehicleSpawn;
	
	const static string WB_DEFEND_INFANTRY = "Defend | Infantry";
	[Attribute(defvalue: "3", UIWidgets.Slider, category: WB_DEFEND_INFANTRY, "1 30 1")]
	int numberOfEnemiesPerWave;
	[Attribute(defvalue: "9", UIWidgets.Slider, category: WB_DEFEND_INFANTRY, "1 30 1")]
	int maxNumberOfEnemyInfSpawns;
	[Attribute(defvalue: "9", UIWidgets.Slider, category: WB_DEFEND_INFANTRY, "1 30 1")]
	int maxNumberOfEnemyVehSpawns;
	
	const static string WB_DEFEND_SCORE = "Defend | Score";
	[Attribute(defvalue: "1", UIWidgets.Slider, category: WB_DEFEND_SCORE, "1 10 1")]
	int pointsPerKill;
	
	const static string WB_DEFEND_TIMING = "Defend | Timing";
	[Attribute(defvalue: "60", UIWidgets.Slider, category: WB_DEFEND_TIMING, "10 20 1")]
	int secondsBeforeStarting;
	[Attribute(defvalue: "30", UIWidgets.Slider, category: WB_DEFEND_TIMING, "10 300 1")]
	int secondsBeforeFirstWaveToPrepare;
	[Attribute(defvalue: "1", UIWidgets.Slider, category: WB_DEFEND_TIMING, "1 5 1")]
	int secondsBeforeSpawnEnemy;
	[Attribute(defvalue: "120", UIWidgets.Slider, category: WB_DEFEND_TIMING, "30 300 1")]
	int secondsBeforeReinforcements;
	
	
	const static string WB_DEFEND_SPAWNGROUPS = "Defend | Spawn Groups";
	[Attribute(uiwidget: UIWidgets.Auto, category: WB_DEFEND_SPAWNGROUPS)]
	ref array<ref string> spawnGroupsInf;
	
	[Attribute(uiwidget: UIWidgets.Auto, category: WB_DEFEND_SPAWNGROUPS)]
	ref array<ref string> spawnGroupsVeh;
	
	const static string WB_DEFEND_RESOURCES = "Defend | Resources";
	[Attribute(defvalue: "{DF63ECA1E31B61B1}UI/Layouts/Modded/HUD.layout", UIWidgets.EditBox, category: WB_DEFEND_RESOURCES)]
	string uiHUDLayout;
	[Attribute(defvalue: "{750A8D1695BD6998}Prefabs/AI/Waypoints/AIWaypoint_Move.et", UIWidgets.EditBox, category: WB_DEFEND_RESOURCES)]
	string moveWaypointType;
	[Attribute(defvalue: "{22A875E30470BD4F}Prefabs/AI/Waypoints/AIWaypoint_Patrol.et", UIWidgets.EditBox, category: WB_DEFEND_RESOURCES)]
	string patrolWaypointType;
	
	const static string WB_DEFEND_MARKERNAMES = "Defend | Marker Names";
	[Attribute(defvalue: "ussr_spawn_", UIWidgets.EditBox, category: WB_DEFEND_MARKERNAMES)]
	string spawnPositionInfPrefix;
	[Attribute(defvalue: "ussr_veh_spawn_", UIWidgets.EditBox, category: WB_DEFEND_MARKERNAMES)]
	string spawnPositionVehPrefix;
	[Attribute(defvalue: "ussr_waypoint", UIWidgets.EditBox, category: WB_DEFEND_MARKERNAMES)]
	string waypointPositionName;
	
	void SendAllowBuilding(bool allowed)
	{
		DefendHelpers.Log("Sending Allow Building", "Sending Allow Building to: " + allowed);
		if (DefendHelpers.IsHost()) AllowBuilding(allowed);
		
		Rpc(RpcDo_AllowBuilding, allowed);
	}
	
	[RplRpc(RplChannel.Reliable, RplRcver.Broadcast)]	
	protected void RpcDo_AllowBuilding(bool allowed) 
	{
		DefendHelpers.Log("RpcDo: Allow Building", "Set allow building to " + allowed);
		AllowBuilding(allowed);
	}
	
	protected void AllowBuilding(bool allowed)
	{
		IEntity gm = GetGame().GetWorld().FindEntityByName("GameMode_Defend");
		SetCanBuild(allowed);
		if (gm != null)
		{
			SCR_EditorManagerEntity editor = SCR_EditorManagerEntity.GetInstance();

			if (editor != null)
			{
				if (editor.IsOpened())
				{
					if (!CanBuild()) 
					{
						editor.Close();
					}
				}
			}
			
			if (allowed)
				hint.ShowHint("Building Allowed", "You can now use the table at the Command Post to build defenses.");
			else
				hint.ShowHint("Building Restricted", "The first wave has begun and you can no longer build defenses.");
			
			DefendHelpers.Log("Allow player building", "Building allowed: " + CanBuild());
		}
		else DefendHelpers.Log("Not Defend Gamemode?", "Couldn't find GameModeDefend entity!");
	}
	
	void SendHUDUpdate(int wave, int maxWave, int secondsUntilNext = 0, string customText = "", int playerId = 0)
	{
		DefendHelpers.Log("Sending HUD Update", "Sending HUD Update to: " + playerId + ", custom text: " + customText + " | wave: " + wave + " | " + secondsUntilNext + " secs");
		if (DefendHelpers.IsHost()) hud.ShowHUD(wave, maxWave, secondsUntilNext, CountAliveAI(), customText);
		
		Rpc(RpcDo_ShowHUDUpdate, wave, maxWave, secondsUntilNext, CountAliveAI(), customText, playerId);
	}
	
	[RplRpc(RplChannel.Unreliable, RplRcver.Broadcast)]	
	protected void RpcDo_ShowHUDUpdate(int wave, int maxWave, int secondsUntilNext, int enemies, string customText, int playerId) 
	{
		DefendHelpers.Log("RpcDo: Show HUD Update", wave.ToString() + "/" + maxWave + " | " + secondsUntilNext + " secs | custom text: " + customText + " | Enemies: " + enemies + " | " + playerId);
		if (localPlayerId == playerId || playerId == 0)
		{
			hud.ShowHUD(wave, maxWave, secondsUntilNext, enemies, customText);
		}
		else DefendHelpers.Log("Not me, ignoring hud update", "PlayerID: " + playerId);
	}
	
	void SendHint(string header, string message, int secondsToShow = 10, bool showTimer = true)
	{
		DefendHelpers.Log("Sending Hint", "Sending hint: " + header + " | " + message + " | " + secondsToShow + " secs");
		if (DefendHelpers.IsHost()) hint.ShowHint(header, message, secondsToShow, showTimer);
		
		Rpc(RpcDo_ShowHint, header, message, secondsToShow, showTimer);
	}
	
	[RplRpc(RplChannel.Unreliable, RplRcver.Broadcast)]	
	protected void RpcDo_ShowHint(string header, string message, int secondsToShow, bool showTimer) 
	{
		DefendHelpers.Log("RpcDo: Show Hint", header + " | " + message);
		hint.ShowHint(header, message, secondsToShow, showTimer);
	}
	
	protected void WaitForPlayers()
	{
		gameStarted = false;
		waitingForPlayers = true;
		DefendHelpers.Log("Waiting for players", "There are currently no players, waiting for some...");
		
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
	
	void Start()
	{
		DefendHelpers.Log("Manager started!", "The Defend Manager has been started.");
		livesLeft = numberOfLives;
		gameStarted = true;
		
		GetGame().GetCallqueue().CallLater(StartWaveTimer, secondsBeforeStarting * 1000);
	}
	
	protected void StartWaveTimer()
	{			
		GetGame().GetCallqueue().CallLater(WaveTimerTick, secondsBeforeFirstWaveToPrepare * 1000);
	}
	
	protected void WaveTimerTick()
	{
		currentWave++;
		SendAllowBuilding(false);
		
		string waveTitle = "Wave " + currentWave.ToString() + "/" + GetNumberOfWaves();
		if (endlessMode) waveTitle = "Wave " + currentWave.ToString();
		
		SendHint(waveTitle, "Prepare yourself, the next wave should be arriving now...");
		
		if (currentWave != numberOfWaves)
		{
			int timeBeforeNextWave = secondsBeforeSpawnEnemy + secondsBeforeReinforcements;
			SendHUDUpdate(currentWave, GetNumberOfWaves(), secondsBeforeReinforcements);
		}
		else
		{
			SendHUDUpdate(currentWave, GetNumberOfWaves(), 0);
		}
		
		
		if (players.Count() < 1) 
		{
			GetGame().GetCallqueue().Call(WaitForPlayers);
			return;
		}
		else
		{
			GetGame().GetCallqueue().CallLater(EnemyTimerTick, secondsBeforeSpawnEnemy * 1000);

			if (currentWave != numberOfWaves || endlessMode) 
			{
				GetGame().GetCallqueue().CallLater(WaveTimerTick, secondsBeforeReinforcements * 1000);
			}
		}		
	}
	
	protected void EnemyTimerTick()
	{
		int maxEnemies = numberOfEnemiesPerWave;
		if (balenceEnemies)
		{
			maxEnemies = maxEnemies * GetAlivePlayers().Count();
		}
		
		for (int enemy; enemy < maxEnemies; enemy++)
		{
			DefendHelpers.Log("Enemy " + (enemy+1) + "/" + maxEnemies, "Spawned enemy " + (enemy+1) + " of "  + maxEnemies + ".");
			bool isVeh = (DefendHelpers.GenerateRandom(1,oddsOfVehicleSpawn) == 1);
			if (allowVehicles)
			{
				DefendHelpers.Log("Enemy Is Vehicle: ", isVeh.ToString());
				DefendHelpers.Log("Vehicle Count: ", vehicleCount.ToString() + "/" + maxVehicles.ToString());
			}
			
			if ((allowVehicles && isVeh) && (vehicleCount < maxVehicles))
			{
				vehicleCount = vehicleCount + 1;
				int randomSpawn = DefendHelpers.GenerateRandom(1, maxNumberOfEnemyVehSpawns);
				GetGame().GetCallqueue().CallLater(SendWave, ((enemy+1) * 1000), false, randomSpawn, true);
			}
			else
			{
				int randomSpawn = DefendHelpers.GenerateRandom(1, maxNumberOfEnemyInfSpawns);
				GetGame().GetCallqueue().CallLater(SendWave, ((enemy+1) * 1000), false, randomSpawn, false);
			}
		}
	}
	
	
	
	protected void SendWave(int spawnID, bool vehicle)
	{
		if (GetGame().GetWorld() != null)
		{
			DefendHelpers.Log("Sending wave...", "Sending the wave to the waypoint.");
		
			IEntity waypointPosition = GetGame().GetWorld().FindEntityByName(waypointPositionName);
			
			if (vehicle) 
			{
				IEntity spawnPosition = GetGame().GetWorld().FindEntityByName(spawnPositionVehPrefix + spawnID);
				GetGame().GetCallqueue().Call(AISpawner, spawnGroupsVeh[DefendHelpers.GenerateRandom(0, spawnGroupsVeh.Count())], spawnPosition, waypointPosition.GetOrigin(), vehicle);
			}	
			else
			{
				IEntity spawnPosition = GetGame().GetWorld().FindEntityByName(spawnPositionInfPrefix + spawnID);
				GetGame().GetCallqueue().Call(AISpawner, spawnGroupsInf[DefendHelpers.GenerateRandom(0, spawnGroupsInf.Count())], spawnPosition, waypointPosition.GetOrigin(), vehicle);
			}
		}
		
	}
	
	protected void AISpawner(string groupToSpawn, IEntity spawnEntity, vector waypointPosition, bool isVehicle)
	{
		DefendHelpers.Log("Spawning AI...", "AI Spawner started.");
		
		SCR_AIGroup group = null;
		
		if (isVehicle)
			group = SpawnVehicle(groupToSpawn, spawnEntity);
		else
			group = SpawnInfantry(groupToSpawn, spawnEntity);
		
			
		GetGame().GetCallqueue().CallLater(SetWaypoint, 10000, false, group, moveWaypointType, waypointPosition);
		GetGame().GetCallqueue().CallLater(AddAIGroup, 10000, false, group, group);
	}
	
	protected SCR_AIGroup SpawnVehicle(string unitToSpawn, IEntity spawnAt)
	{
		Resource resource = DefendHelpers.GenerateAndValidateResource(unitToSpawn);
		
		if (resource == null)
		{
			DefendHelpers.Log("ERROR!", "Unable to load resource for spawn group: " + unitToSpawn, LogLevel.ERROR);	
			return null;
		}
		
		Vehicle vehicle = Vehicle.Cast(GetGame().SpawnEntityPrefab(resource, null, DefendHelpers.GenerateSpawnParamaters(spawnAt.GetOrigin())));
		
		IEntity vehEntity = vehicle;
		vehEntity.SetYawPitchRoll(spawnAt.GetYawPitchRoll());
				
		SCR_AIGroup group = SpawnInfantry("{1A5F0D93609DA5DA}Prefabs/Groups/OPFOR/Group_USSR_ManeuverGroup.et", spawnAt);
		string getInWaypointName = "{B049D4C74FBC0C4D}Prefabs/AI/Waypoints/AIWaypoint_GetInNearest.et";
		Resource pilotResource = DefendHelpers.GenerateAndValidateResource(getInWaypointName);

		AIWaypoint waypoint = AIWaypoint.Cast(GetGame().SpawnEntityPrefab(pilotResource, null, DefendHelpers.GenerateSpawnParamaters(spawnAt.GetOrigin())));

		group.AddWaypoint(waypoint);
		return group;
	}
	
	protected SCR_AIGroup SpawnInfantry(string unitToSpawn, IEntity spawnAt)
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
		
		if (group == null)
		{
			DefendHelpers.Log("ERROR!", "Unable to spawn group!", LogLevel.ERROR);
			return null;
		}
		else DefendHelpers.Log("Group Spawned", "Group spawned...");
		
		return group;
	}
	
	protected void SetWaypoint(SCR_AIGroup group, string type, vector target)
	{	
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
				
		group.AddWaypoint(waypoint);
	}
	
	protected void AddAIGroup(SCR_AIGroup groupToAdd)
	{
		activeAIGroups.Insert(groupToAdd);
	}
}