//! A Class to represent a Defend gamemode Player.
class DefendPlayer
{    
    protected string name = ""; //! Name of the player
    protected int id = 0; //! Unique identifier for the player
	protected string uid = "";
    protected int kills = 0; //! Total kills made by the player
	protected int xp = 0;
    protected int deaths = 0; //! Total deaths of the player
    protected int deserterWarnings = 0; //! Number of deserter warnings issued
    protected bool isDeserter = false; //! Flag indicating if the player is considered a deserter
	protected bool hasExtracted = false;
	
	void Create(int playerID)
	{		
		this.name = GetGame().GetPlayerManager().GetPlayerName(playerID);
		this.id = playerID;
	}
	
    protected bool alive = false; //! Flag indicating if the player is alive
	
	bool IsAlive()
	{
		return this.alive;
	}
	
	void SetAlive(bool value)
	{
		this.alive = value;
	}
	protected ref array<SCR_Waypoint> currentWaypoints;

	IEntity GetEntity()
	{
		return GetController().GetControlledEntity();
	}
	
	PlayerController GetController()
	{
		return GetGame().GetPlayerManager().GetPlayerController(id);
	}
	
	HUDManagerComponent GetHUDManager()
	{
		return GetController().GetHUDManagerComponent();
	}
	
	string GetName()
	{
		return this.name;
	}
	
	int GetID()
	{
		return this.id;
	}
	
	int GetUserUID()
	{
		return this.uid;
	}
	
	bool HasExtracted()
	{
		return this.hasExtracted;
	}
	
	void Extract()
	{
		this.hasExtracted = true;
	}
	

    //! Increments the deserter warning count by 1
    //! -------------------------------------
    //! This method increases the deserter warning count when called, indicating the player may be in violation of certain game rules (such as deserting).
    void AddDeserterWarning()
    {
        this.deserterWarnings++;
    }
    
    //! Gets the current number of deserter warnings for the player
    //! -------------------------------------
    //! Returns:
    //! The number of deserter warnings that have been issued to the player.
    int GetDeserterWarnings()
    {
        return this.deserterWarnings;
    }
    
    //! Checks if the player is considered a deserter
    //! -------------------------------------
    //! Returns:
    //! A boolean value indicating whether the player is a deserter (true) or not (false).
    bool IsDeserter()
    {
        return this.isDeserter;
    }

    //! Sets the deserter status for the player
    //! -------------------------------------
    //! Parameters:
    //! value - The value to set the deserter status to (true for deserter, false for non-deserter).
    //! -------------------------------------
    //! This method sets the player's deserter status and resets the deserter warnings to 0 if the status is set to false.
    void SetDeserter(bool value)
    {
        if (!value) this.deserterWarnings = 0;
        this.isDeserter = value;
    }
	
	void AddXP(int amountToAdd)
	{
		DefendHelpers.Log("Adding Player XP", "Adding " + amountToAdd + " XP to player " + GetName());
		this.xp = xp + amountToAdd;
	}

    //! Increments the player's kill count by a specified number
    //! -------------------------------------
    //! Parameters:
    //! killsToAdd - The number of kills to add to the player's total kill count.
    //! -------------------------------------
    //! This method increases the player's kill count by the given amount.
    void AddKills(int killsToAdd)
    {
        this.kills += killsToAdd;
    }
	
	int GetKills()
	{
		return this.kills;
	}

    //! Increments the player's death count by a specified number
    //! -------------------------------------
    //! Parameters:
    //! deathsToAdd - The number of deaths to add to the player's total death count.
    //! -------------------------------------
    //! This method increases the player's death count by the given amount.
    void AddDeaths(int deathsToAdd)
    {
        this.deaths += deathsToAdd;
    }
	
	int GetDeaths()
	{
		return this.deaths;
	}
}