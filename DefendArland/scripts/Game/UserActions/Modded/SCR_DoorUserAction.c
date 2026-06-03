modded class SCR_DoorUserAction
{
	override void PerformAction(IEntity pOwnerEntity, IEntity pUserEntity)
	{
		DefendManager dm = DefendHelpers.Get();
		int playerID = GetGame().GetPlayerManager().GetPlayerIdFromControlledEntity(pUserEntity);		
		if (dm != null)
		{
			DoorComponent comp = DoorComponent.Cast(pOwnerEntity.FindComponent(DoorComponent));
			bool isDoorJammed = (pOwnerEntity.GetName() == "JammedDoor");

			if (isDoorJammed) 
			{
				if (playerID == dm.localPlayerId) 
					dm.hint.ShowHint("Can't interact with this door", "This door seems to be magically jammed open.", playerId:dm.localPlayerId, isDebug:dm.hintLogging);
			}
			else super.PerformAction(pOwnerEntity, pUserEntity);
		}
		else super.PerformAction(pOwnerEntity, pUserEntity); 
	}
}