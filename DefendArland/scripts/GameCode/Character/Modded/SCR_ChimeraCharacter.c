modded class SCR_ChimeraCharacter
{
	protected ZombieController _zombieController = null;
	bool IsZombie()
	{
		if (_zombieController == null)
		{
			IEntity unitEntity = IEntity.Cast(this);
			_zombieController = ZombieController.Cast(unitEntity.FindComponent(ZombieController));
		}
		
		if (_zombieController != null) return true;
		else return false;
	}
	
	ZombieController GetZombieController()
	{
		if (IsZombie()) return _zombieController;
		else return null;
	}
}