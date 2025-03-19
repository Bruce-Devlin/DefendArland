modded class SCR_EditorManagerEntity
{
	override void Open(bool showErrorNotification = true)
	{
		DefendManager dm = DefendHelpers.Get();
		if (dm != null)
		{
			IEntity aliveEntity = IEntity.Cast(GetGame().GetPlayerController().GetControlledEntity());
			if (dm.CanBuild())
			{
				super.Open(showErrorNotification);
			}
			else
			{
				if (showErrorNotification)
					SCR_NotificationsComponent.SendToPlayer(GetPlayerID(), ENotification.EDITOR_CANNOT_OPEN);
				return;
			}
		}
		else super.Open(showErrorNotification);
		
	}
}