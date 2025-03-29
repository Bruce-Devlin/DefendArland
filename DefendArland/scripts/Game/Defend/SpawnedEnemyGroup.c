class SpawnedEnemyGroup
{
	protected ChimeraAIGroup _group = null;
	bool HasGroup()
	{
		return (_group != null);
	}
	void SetGroup(ChimeraAIGroup newGroup)
	{
		this._group = newGroup;
	}
	
	
 	ChimeraAIGroup GetGroup()
	{
		if (!HasGroup())
		{
			return null;
		}
		return this._group;
	}
	
	
	protected SpawnedGroupState _state = SpawnedGroupState.SPAWNED;
	void SetState(SpawnedGroupState newState)
	{
		this._state = newState;
	}
	
	SpawnedGroupState GetState()
	{
		return this._state;
	}
}

enum SpawnedGroupState
{
	SPAWNING,
	SPAWNED,
	ATTACKING,
	RETREATING,
	VEHICLE
}