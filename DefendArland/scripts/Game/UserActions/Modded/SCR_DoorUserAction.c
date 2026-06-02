modded class SCR_DoorUserAction
{
	override void PerformAction(IEntity pOwnerEntity, IEntity pUserEntity)
	{
		DefendManager dm = DefendHelpers.Get();
		if (dm == null || pOwnerEntity == null)
		{
			super.PerformAction(pOwnerEntity, pUserEntity);
			return;
		}

		int playerID = GetGame().GetPlayerManager().GetPlayerIdFromControlledEntity(pUserEntity);
		bool isDoorJammed = (pOwnerEntity.GetName() == "JammedDoor");

		if (isDoorJammed)
		{
			if (playerID == dm.localPlayerId && dm.hint != null)
				dm.hint.ShowHint("Can't interact with this door", "This door seems to be magically jammed open.", playerId:dm.localPlayerId, isDebug:dm.hintLogging);
		}
		else super.PerformAction(pOwnerEntity, pUserEntity);
	}
}
