modded class SCR_DoorUserAction
{
	override void PerformAction(IEntity pOwnerEntity, IEntity pUserEntity)
	{
		DefendManager dm = DefendHelpers.Get();
		if (dm != null)
		{
			DoorComponent comp = DoorComponent.Cast(pOwnerEntity.FindComponent(DoorComponent));
			bool isDoorJammed = (pOwnerEntity.GetName() == "JammedDoor");

			if (isDoorJammed) dm.hint.ShowHint("Can't interact with this door", "This door seems to be magically jammed open.");
			else super.PerformAction(pOwnerEntity, pUserEntity);
		}
	}
}