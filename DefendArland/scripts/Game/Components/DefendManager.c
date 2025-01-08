class HUDHelpers
{
	protected Widget hudRoot;
	protected RichTextWidget waveTextWidget;
	protected RichTextWidget timerTextWidget;
	protected RichTextWidget enemiesTextWidget;
	protected ImageWidget separatorWidget;

	protected bool initComplete = false;

	
 	void Init(string uiHUDLayout, bool isDebug) 
	{ 		
		DefendHelpers.Log("Init HUD", "Starting HUD");
		hudRoot = GetGame().GetWorkspace().CreateWidgets(uiHUDLayout);
		timerDebug = isDebug;
      
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
	
	protected string FormatWaves(int wave)
	{
		DefendManager dm = DefendHelpers.Get();
		string textToShow = "WAVE " + wave + "/" + dm.numberOfWaves;
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
	
	void ShowHUD(int currWave, int timerSeconds = 0, int totalEnemies = 0)
	{
		if (!initComplete)
			return;
		
		if (DefendHelpers.IsHost()) totalEnemies = DefendManager.CountAliveAI();
		DefendHelpers.Log("Showing HUD", "Wave: " + currWave + " | Timer: " + timerSeconds + " | Total Enemies: " + totalEnemies);
		
		if (timerActive)
		{
			timerTimeLeft = -1;
			timerActive = false;
			GetGame().GetCallqueue().RemoveByName(this, "StartTimer");
			GetGame().GetCallqueue().RemoveByName(this, "TimerTick");
		}
		
		if (currWave != 0)
		{
			waveTextWidget.SetText(FormatWaves(currWave));
			timerTextWidget.SetText(FormatTimer(timerSeconds));
			enemiesTextWidget.SetText(FormatEnemies(totalEnemies));
			
			DefendHelpers.Log("HUD Text Set", "Wave text set.");
			
			if (timerSeconds != 0 || totalEnemies != 0 && DefendHelpers.IsHost())
			{
				if (timerTimeLeft == 0) timerTimeLeft = -1;
				
				shouldStartTimer = true;
				GetGame().GetCallqueue().Call(StartTimer, timerSeconds, totalEnemies, currWave);
				DefendHelpers.Log("HUD Started timer", "Wave timer text set.");
			}
		}
		else
		{
			waveTextWidget.SetText("PREPARE YOUR DEFENSES!");
			GetGame().GetCallqueue().RemoveByName(this, "StartTimer");
			GetGame().GetCallqueue().RemoveByName(this, "TimerTick");
			
			timerActive = false;
			shouldStartTimer = false;
			
			DefendHelpers.Log("HUD Text Set", "Prepare defense text set.");
		}
		
		DefendHelpers.Log("HUD Set Visible", "The HUD should now be visible.");
	}
	
	static int timerTimeLeft = -1;
	static bool timerActive = false;
	static bool shouldStartTimer = false;
	static bool timerDebug = false;
	
	void StartTimer(int time, int enemies, int wave)
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
			GetGame().GetCallqueue().CallLater(TimerTick, 1000, false, time, enemies, wave);
			return;
		}
	}
	
	void TimerTick(int initialTime, int initalEnemiesLeft, int wave)
	{
		if (timerDebug) DefendHelpers.Log("HUD Timer Tick", "HUD timer ticked.");
		if (timerTimeLeft > 0) timerTimeLeft = timerTimeLeft - 1;
		
		waveTextWidget.SetText(FormatWaves(wave));
		timerTextWidget.SetText(FormatTimer(timerTimeLeft));
		if (timerTextWidget.GetText() == "")
		{
			timerTextWidget.SetText(FormatEnemies(DefendManager.CountAliveAI()));
			enemiesTextWidget.SetText("");
		}
		else enemiesTextWidget.SetText(FormatEnemies(DefendManager.CountAliveAI()));
		
		DefendManager dm = DefendHelpers.Get();
		dm.SendHUDUpdate(wave, timerTimeLeft, 0);
		
		GetGame().GetCallqueue().Call(StartTimer, initialTime, initalEnemiesLeft, wave);
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
	static void RegisterDeath(int deadID)
	{
		if (DefendHelpers.IsHost())
		{
			DefendPlayer player = GetPlayer(deadID);
			player.AddDeaths(1);
			livesLeft = (livesLeft - 1);
		
			DefendHelpers.Log("Player died", "Player " + player.name + " (" + player.id.ToString() + ") died (total deaths: " + player.deaths + ", lives remaining: " + livesLeft + ")");
		}
	}
	
	static void RegisterKill(int killerID, int kills)
	{
		if (DefendHelpers.IsHost())
		{
			DefendPlayer player = GetPlayer(killerID);
			player.AddKills(kills);
		
			DefendHelpers.Log("Player got a kill", "Player " + player.name + " (" + player.id.ToString() + ") killed someone (total kills: " + player.kills + " )");
		}
	}
	
	static void RegisterAIDeath(IEntity deadAI)
	{
		if (DefendHelpers.IsHost())
		{
			
		}
	}
	
	static void SetPlayerState(int playerID, bool alive)
	{
		if (DefendHelpers.IsHost())
		{
			DefendPlayer player = GetPlayer(playerID);
			DefendHelpers.Log("Player State Change", "Player " + player.name + " is alive: " + alive + " (oldState: " + player.alive + ")");

			player.alive = alive;
		}
	}
	
	static ref array<DefendPlayer> GetAlivePlayers()
	{
		ref array<DefendPlayer> alivePlayers = {};
		for (int i; i < players.Count(); i++)
		{
			DefendPlayer _currPlayer = players[i];
			if (_currPlayer.alive) alivePlayers.Insert(players[i]);
		}
		return alivePlayers;
	} 
	
	static void AddPlayer(DefendPlayer player)
	{
		DefendHelpers.Log("Player joined", "Player " + player.name + " joined (" + player.id + ")");
		players.Insert(player);
	}
	
	static void RemovePlayer(int playerID)
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
	
	static ref DefendPlayer GetPlayer(int id)
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
		
		if (DefendHelpers.IsHost())
		{
			SendHUDUpdate(0);
		}
		
		hud.timerActive = false;
		hud.timerTimeLeft = 0;
		hud.shouldStartTimer = false;
		
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
		
		gameMode.EndGameMode(SCR_GameModeEndData.CreateSimple(type, -1, usIndex));
	}
	
	override void EOnActivate(IEntity owner)
	{
		InitMission();
	}
	
	void OnGameDestroyed()
	{	
		hud.timerActive = false;
		hud.timerTimeLeft = 0;
		hud.shouldStartTimer = false;
		
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
			hud.Init(uiHUDLayout, debugMode);
			isHost = true;
			localPlayerId = -1;
			
			GetGame().GetCallqueue().Call(WaitForPlayers);
			GetGame().GetCallqueue().CallLater(ServerUpdateLoop, 1000, true);
			serverPlayerInitComplete = true;
		}
		else
		{
			DefendHelpers.Log("Not Host", "Not currently the game host.");
		}

	}
	
	void InitPlayerLocal(int playerId)
	{
		DefendHelpers.Log("Init Player", "Init called for player id: " + playerId);
		localPlayerId = playerId;
		
		hud.Init(uiHUDLayout, debugMode);
				
		localPlayerInitComplete = true;
	}
	
	protected void ServerUpdateLoop()
	{
		if (!waitingForPlayers && gameStarted)
		{
			DefendHelpers.Log("Running Server Loop", "Server loop running.");
			if (livesLeft < 1)
			{
				EndGame(EGameOverTypes.LOOSE, 0);
			}
			
			if (CountAliveAI() > 0)
			{
				DefendHelpers.Log("Running Server AI Loop", "Server AI loop running.");

				CheckAI();
				
				if (GetGame().GetCallqueue().GetRemainingTime(RemindAI) != 0)
				{
					GetGame().GetCallqueue().CallLater(RemindAI, 10000, false);
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
	
	void RemindAI()
	{
		foreach(int i, SCR_AIGroup group : activeAIGroups)
		{
			DefendHelpers.Log("Reminding AI (" + i + "/" + activeAIGroups.Count() + ")", "Remind AI that they're supposed to be doing something...");

			array<AIWaypoint> waypoints = {};
			group.GetWaypoints(waypoints);
			IEntity waypointPosition = GetGame().GetWorld().FindEntityByName(waypointPositionName);

			if (waypoints.Count() == 0)
			{
				DefendHelpers.Log("AI isn't busy", "AI #" + i + " didn't have anything to do!");
				float distance = vector.Distance(group.GetLeaderEntity().GetOrigin(), waypointPosition.GetOrigin());
				DefendHelpers.Log("AI isn't busy", "AI #" + i + " didn't have anything to do and is " + distance + " away from obj!");

				if (distance < 10)
				{
					DefendHelpers.Log("AI is within their waypoint", "AI is within their waypoint radius.");
					bool patrolResult = SetWaypoint(group, patrolWaypointType, waypointPosition.GetOrigin());
				}
				else
				{
					bool patrolResult = SetWaypoint(group, moveWaypointType, waypointPosition.GetOrigin());
				}
			}
			else 
			{
				AIWaypoint currWaypoint = waypoints[0];
				if (currWaypoint.IsWithinCompletionRadius(group))
				{
					DefendHelpers.Log("AI is within their waypoint", "AI #" + i + " is busy but within their waypoint location");
				
					bool patrolResult = SetWaypoint(group, patrolWaypointType, waypointPosition.GetOrigin());
				}
				else
				{
					DefendHelpers.Log("AI is busy", "AI #" + i + " is busy going to a waypoint");
					GetGame().GetCallqueue().CallLater(CheckRemindAI, 20000, false, group, currWaypoint);
				}
			}
		}
	}
	
	void CheckRemindAI(SCR_AIGroup group, AIWaypoint waypoint)
	{
		array<AIWaypoint> waypoints = {};
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
					bool patrolResult = SetWaypoint(group, patrolWaypointType, waypointPosition.GetOrigin());
				}
			}
		}
	}
	
	static int CountAliveAI()
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
	
	
	
	protected void CheckAI()
	{
		DefendHelpers.Log("Checking AI", "Checking health of alive AI Groups");

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

	protected bool gameStarted = false;
	protected bool victoryAcheivable = false;
	static int currentWave = 0;
	static int livesLeft = 0;
	protected bool waitingForPlayers = true;
	static int localPlayerId = 0;
	static bool isHost = false;
	static bool localPlayerInitComplete = false;
	static bool serverPlayerInitComplete = false;
	static ref array<int> activePlayerIds = {};
	static ref array<SCR_AIGroup> activeAIGroups = {};
	
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
	
	static ref HintHelpers hint = new HintHelpers();
	static ref HUDHelpers hud = new HUDHelpers();
	const static string WB_DEFEND_CATEGORY = "Defend Manager";
	protected static ref array<ref DefendPlayer> players = {};

	[Attribute(defvalue: "0", category: WB_DEFEND_CATEGORY)]
	bool debugMode;
	[Attribute(defvalue: "3", UIWidgets.Slider, category: WB_DEFEND_CATEGORY, "1 10 1")]
	int numberOfWaves;
	[Attribute(defvalue: "5", UIWidgets.Slider, category: WB_DEFEND_CATEGORY, "0 100 0")]
	int numberOfLives;
	[Attribute(defvalue: "0", category: WB_DEFEND_CATEGORY)]
	bool endlessMode;
	[Attribute(defvalue: "0", category: WB_DEFEND_CATEGORY)]
	bool balenceEnemies;
	[Attribute(defvalue: "3", UIWidgets.Slider, category: WB_DEFEND_CATEGORY, "1 30 1")]
	int numberOfEnemiesPerWave;
	[Attribute(defvalue: "9", UIWidgets.Slider, category: WB_DEFEND_CATEGORY, "1 30 1")]
	int maxNumberOfEnemySpawns;
	[Attribute(defvalue: "1", UIWidgets.Slider, category: WB_DEFEND_CATEGORY, "1 10 1")]
	int pointsPerKill;
	
	[Attribute(defvalue: "60", UIWidgets.Slider, category: WB_DEFEND_CATEGORY, "10 20 10")]
	int secondsBeforeStarting;
	[Attribute(defvalue: "30", UIWidgets.Slider, category: WB_DEFEND_CATEGORY, "10 300 01")]
	int secondsBeforeFirstWaveToPrepare;
	[Attribute(defvalue: "1", UIWidgets.Slider, category: WB_DEFEND_CATEGORY, "1 5 1")]
	int secondsBeforeSpawnEnemy;
	[Attribute(defvalue: "120", UIWidgets.Slider, category: WB_DEFEND_CATEGORY, "30 300 1")]
	int secondsBeforeReinforcements;
	
	
	[Attribute(uiwidget: UIWidgets.Auto, category: WB_DEFEND_CATEGORY)]
	ref array<ref string> spawnGroupsInf;
	
	[Attribute(uiwidget: UIWidgets.Auto, category: WB_DEFEND_CATEGORY)]
	ref array<ref string> spawnGroupsVeh;
	
	[Attribute(defvalue: "{DF63ECA1E31B61B1}UI/Layouts/Modded/HUD.layout", UIWidgets.EditBox, category: WB_DEFEND_CATEGORY)]
	string uiHUDLayout;
	[Attribute(defvalue: "{750A8D1695BD6998}Prefabs/AI/Waypoints/AIWaypoint_Move.et", UIWidgets.EditBox, category: WB_DEFEND_CATEGORY)]
	string moveWaypointType;
	[Attribute(defvalue: "{22A875E30470BD4F}Prefabs/AI/Waypoints/AIWaypoint_Patrol.et", UIWidgets.EditBox, category: WB_DEFEND_CATEGORY)]
	string patrolWaypointType;
	[Attribute(defvalue: "ussr_spawn_", UIWidgets.EditBox, category: WB_DEFEND_CATEGORY)]
	string spawnPositionPrefix;
	[Attribute(defvalue: "ussr_waypoint", UIWidgets.EditBox, category: WB_DEFEND_CATEGORY)]
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
		if (gm != null)
		{
			SCR_EditorManagerEntity editor = SCR_EditorManagerEntity.GetInstance();

			if (editor != null)
			{
				if (!allowed) editor.Close();
			}
			
			SetCanBuild(allowed);
			DefendHelpers.Log("Allow player building", "Building allowed: " + CanBuild());
		}
		else DefendHelpers.Log("Not Defend Gamemode?", "Couldn't find GameModeDefend entity!");
		
	}
	
	void SendHUDUpdate(int wave, int secondsUntilNext = 0, int playerId = 0)
	{
		DefendHelpers.Log("Sending HUD Update", "Sending HUD Update to: " + playerId + ", wave: " + wave + " | " + secondsUntilNext + " secs");
		if (DefendHelpers.IsHost()) hud.ShowHUD(wave, secondsUntilNext);
		
		Rpc(RpcDo_ShowHUDUpdate, wave, secondsUntilNext, CountAliveAI(), playerId);
	}
	
	[RplRpc(RplChannel.Unreliable, RplRcver.Broadcast)]	
	protected void RpcDo_ShowHUDUpdate(int wave, int secondsUntilNext, int enemies, int playerId) 
	{
		DefendHelpers.Log("RpcDo: Show HUD Update", wave.ToString() + " | " + secondsUntilNext + " | " + enemies + " | " + playerId);
		if (localPlayerId == playerId || playerId == 0)
		{
			hud.ShowHUD(wave, secondsUntilNext, enemies);
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
		
		SendHint("Wave " + (currentWave) + "/" + numberOfWaves, "Prepare yourself, the next wave should be arriving now...");
		
		if (currentWave != numberOfWaves)
		{
			int timeBeforeNextWave = secondsBeforeSpawnEnemy + secondsBeforeReinforcements;
			SendHUDUpdate(currentWave, secondsBeforeReinforcements);
		}
		else
		{
			SendHUDUpdate(currentWave, 0);
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
			int randomSpawn = DefendHelpers.GenerateRandom(1, maxNumberOfEnemySpawns);
			GetGame().GetCallqueue().CallLater(SendWave, ((enemy+1) * 1000), false, randomSpawn);
		}
	}
	
	protected void SendWave(int spawnID)
	{
		if (GetGame().GetWorld() != null)
		{
			DefendHelpers.Log("Sending wave...", "Sending the wave to the waypoint.");
		
			IEntity spawnPosition = GetGame().GetWorld().FindEntityByName(spawnPositionPrefix + spawnID);
			IEntity waypointPosition = GetGame().GetWorld().FindEntityByName(waypointPositionName);
			
			GetGame().GetCallqueue().Call(AISpawner, spawnGroupsInf[DefendHelpers.GenerateRandom(0, spawnGroupsInf.Count())], spawnPosition.GetOrigin(), waypointPosition.GetOrigin());
		}
		
	}
	
	protected void AISpawner(string groupToSpawn, vector spawnAt, vector waypointPosition)
	{
		DefendHelpers.Log("Spawning AI...", "AI Spawner started.");
		Resource resource = DefendHelpers.GenerateAndValidateResource(groupToSpawn);
		
		if (resource == null)
		{
			DefendHelpers.Log("ERROR!", "Unable to load resource for spawn group: " + groupToSpawn, LogLevel.ERROR);	
			return;
		}
		
		
		SCR_AIGroup group = SCR_AIGroup.Cast(GetGame().SpawnEntityPrefab(resource, null, DefendHelpers.GenerateSpawnParamaters(spawnAt)));		
		
		if (group == null)
		{
			DefendHelpers.Log("ERROR!", "Unable to spawn group!", LogLevel.ERROR);
			return;
		}
		else DefendHelpers.Log("Group Spawned", "Group spawned...");
						
		bool moveResult = SetWaypoint(group, moveWaypointType, waypointPosition);
		bool patrolResult = SetWaypoint(group, patrolWaypointType, waypointPosition);

		if (!moveResult && !patrolResult) 
		{
			DefendHelpers.Log("ERROR!", "Unable to set wave waypoints!", LogLevel.ERROR);
			return;
		}
		
		activeAIGroups.Insert(group);
	}
	
	protected bool SetWaypoint(SCR_AIGroup group, string type, vector target)
	{	
		Resource resource = DefendHelpers.GenerateAndValidateResource(type);
		
		if (resource == null)
		{
			DefendHelpers.Log("ERROR!", "Unable to load resource for spawn group: " + group, LogLevel.ERROR);
			return false;
		}
		
		AIWaypoint waypoint = AIWaypoint.Cast(GetGame().SpawnEntityPrefab(resource, null, DefendHelpers.GenerateSpawnParamaters(target)));
		if (waypoint == null)
		{
			DefendHelpers.Log("ERROR!", "Unable to spawn waypoint!", LogLevel.ERROR);
			return false;
		}
				
		group.AddWaypoint(waypoint);
		return true;
	}
}