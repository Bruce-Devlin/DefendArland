modded class SCR_GetOutAction
{
	override void PerformAction(IEntity pOwnerEntity, IEntity pUserEntity)
	{
		DefendManager dm = DefendHelpers.Get();
		if (dm != null)
		{
			DefendHelpers.Log("User Getting Out", "A user is getting out of a vehicle");
			IEntity vehEntity = SCR_EntityHelper.GetMainParent(pOwnerEntity, true);
			if (vehEntity.GetName() == "Extraction")
			{
				dm.hint.ShowHint("Getting out would be a bad idea", "You've already boarded the extraction truck, leaving it now would be a bad idea, it will leave soon.");
			}
			else super.PerformAction(pOwnerEntity, pUserEntity);

		}
		else super.PerformAction(pOwnerEntity, pUserEntity);
	}
}