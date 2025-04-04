class DefendHelpers
{	
	static string GenerateUID(string prefix)
	{
		string newUID = prefix + "_" + DefendHelpers.GenerateRandom(100000, 999999);
		return newUID;
	}
	
	static bool IsPlayerSaved(string playerUID)
	{
	
	}
	
	static void SaveLocalPlayerUID(string playerUID)
	{
		
	}
	
	static string LoadLocalPlayerUID()
	{
		
	}
	
	static void SavePlayer(DefendPlayer player)
	{
	
	}
	
	static DefendPlayer LoadPlayer(string playerUID)
	{
	
	}
	
	static void AllowCharacterDamage(IEntity entity)
	{
		DamageManagerComponent damageManager = DamageManagerComponent.Cast(entity.FindComponent(DamageManagerComponent));
		if (damageManager != null)
		{
			GetGame().GetCallqueue().Remove(PreventCharacterDamage);
			if (!damageManager.IsDamageHandlingEnabled())
				damageManager.EnableDamageHandling(true);		
		}
		else Log("ERROR", "Couldn't find Damage Manager Component in entity: " + entity.GetName());
	}
	
	static void PreventCharacterDamage(IEntity entity, int seconds = -1, int initialSeconds = 0)
	{
		GetGame().GetCallqueue().Remove(PreventCharacterDamage);

		if (initialSeconds == 0 && seconds != -1)
		{
			initialSeconds = seconds;
			seconds = 0;
		}
				
		DamageManagerComponent damageManager = DamageManagerComponent.Cast(entity.FindComponent(DamageManagerComponent));
		if (damageManager != null)
		{
			damageManager.EnableDamageHandling(false);
			
			if (seconds == -1) GetGame().GetCallqueue().CallLater(PreventCharacterDamage, 1 * 1000, false, entity, seconds, initialSeconds);
			else
			{
				if (seconds < initialSeconds)
					GetGame().GetCallqueue().CallLater(PreventCharacterDamage, 1 * 1000, false, entity, seconds+1, initialSeconds);
				else damageManager.EnableDamageHandling(true);
			}
		}
		else Log("ERROR", "Couldn't find Damage Manager Component in entity: " + entity.GetName());
	}
	
	static IEntity FindNearestEntity(array<IEntity> entities, vector point, out float distance)
	{
		IEntity closestObj = null;
		float closestDistance = -1;
		foreach (IEntity entity : entities)
		{
			if (closestObj == null)
			{
				closestObj = entity;
				closestDistance = vector.Distance(entity.GetOrigin(), point);
			}
			else if (vector.Distance(entity.GetOrigin(), point) < closestDistance)
			{
				closestObj = entity;
				closestDistance = vector.Distance(entity.GetOrigin(), point);
			}
		}
		return closestObj;
	}
	
    //! Checks if the current game instance is the host (master)
    //! -------------------------------------
    //! Returns:
    //! A boolean value indicating if the game is running in host (master) mode (true) or not (false).
    static bool IsHost()
    {
        SCR_BaseGameMode gm = SCR_BaseGameMode.Cast(GetGame().GetGameMode());
        if (gm != null)
        {
            return gm.IsMaster();
        }
        else return false;
    }
    
    //! Retrieves the DefendManager entity from the game world
    //! -------------------------------------
    //! Returns:
    //! The DefendManager instance if found and cast successfully, or null if there was an error.
    static DefendManager Get()
    {
		if (GetGame() == null)
			return null;
		
		if (!GetGame().InPlayMode())
			return null;
		
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
    
    //! Logs a message with a timestamp in the console
    //! -------------------------------------
    //! Parameters:
    //! header - The header or title for the log message.
    //! message - The body of the log message.
    //! level - The log level (default is LogLevel.NORMAL).
    //! -------------------------------------
    //! This method generates a log entry with a timestamp and the given message.
    static void Log(string header, string message, LogLevel level = LogLevel.NORMAL)
    {
        string timeStamp = SCR_DateTimeHelper.GetTimeUTC();
        Print("[" + timeStamp + "] Defend Arland: " + header + " - "+ message, level);
    }
    
    //! Generates a random integer between a specified minimum and maximum value
    //! -------------------------------------
    //! Parameters:
    //! min - The minimum value (inclusive).
    //! max - The maximum value (exclusive).
    //! -------------------------------------
    //! Returns:
    //! A random integer between min and max.
    static int GenerateRandom(int min, int max)
    {
        int value = Math.RandomInt(min, (max+1));
		Log("Rolling dice", "Rolling dice starting at " + min.ToString() + " to " + max.ToString() + " | Rolled: " + value.ToString());
        return value;
    }
    
    //! Creates and returns EntitySpawnParams with the specified position
    //! -------------------------------------
    //! Parameters:
    //! position - The world position at which the entity should be spawned.
    //! -------------------------------------
    //! Returns:
    //! An EntitySpawnParams object initialized with the provided position.
    static EntitySpawnParams GenerateSpawnParamaters(vector position)
    {
        EntitySpawnParams params = EntitySpawnParams();
        params.TransformMode = ETransformMode.WORLD;
        
        params.Transform[3] = position;
        return params;
    }
    
    //! Loads and validates a resource, returning it if valid or logging an error
    //! -------------------------------------
    //! Parameters:
    //! resourceToLoad - The name of the resource to load.
    //! -------------------------------------
    //! Returns:
    //! The loaded resource if valid, or null if the resource could not be loaded.
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