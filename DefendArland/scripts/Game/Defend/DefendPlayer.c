//! A Class to represent a Defend gamemode Player.
class DefendPlayer
{    
    string name = ""; //! Name of the player
    int id = ""; //! Unique identifier for the player
    int kills = 0; //! Total kills made by the player
    int deaths = 0; //! Total deaths of the player
    protected int deserterWarnings = 0; //! Number of deserter warnings issued
    protected bool isDeserter = false; //! Flag indicating if the player is considered a deserter
	protected bool hasExtracted = false;
    bool alive = false; //! Flag indicating if the player is alive
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
	
	bool HasExtracted()
	{
		return hasExtracted;
	}
	
	void Extract()
	{
		hasExtracted = true;
	}
	

    //! Increments the deserter warning count by 1
    //! -------------------------------------
    //! This method increases the deserter warning count when called, indicating the player may be in violation of certain game rules (such as deserting).
    void AddDeserterWarning()
    {
        deserterWarnings++;
    }
    
    //! Gets the current number of deserter warnings for the player
    //! -------------------------------------
    //! Returns:
    //! The number of deserter warnings that have been issued to the player.
    int GetDeserterWarnings()
    {
        return deserterWarnings;
    }
    
    //! Checks if the player is considered a deserter
    //! -------------------------------------
    //! Returns:
    //! A boolean value indicating whether the player is a deserter (true) or not (false).
    bool IsDeserter()
    {
        return isDeserter;
    }

    //! Sets the deserter status for the player
    //! -------------------------------------
    //! Parameters:
    //! value - The value to set the deserter status to (true for deserter, false for non-deserter).
    //! -------------------------------------
    //! This method sets the player's deserter status and resets the deserter warnings to 0 if the status is set to false.
    void SetDeserter(bool value)
    {
        if (!value) deserterWarnings = 0;
        isDeserter = value;
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
}