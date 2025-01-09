modded class SCR_DoorUserAction
{
	override void PerformAction(IEntity pOwnerEntity, IEntity pUserEntity)
	{
		DefendManager dm = DefendHelpers.Get();
		if (dm != null)
		{
			string doorName = pOwnerEntity.GetName();
			bool shouldKeepOpen = dm.namesOfDoorsToForceOpen.Contains(doorName);

			if (!shouldKeepOpen) super.PerformAction(pOwnerEntity, pUserEntity);
			else dm.hint.ShowHint("Can't interact with this door", "This door seems to be magically jammed open.")
		}
	}
}