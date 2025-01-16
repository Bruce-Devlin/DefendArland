modded class SCR_GetInUserAction
{
	override void PerformAction(IEntity pOwnerEntity, IEntity pUserEntity)
	{
		DefendManager dm = DefendHelpers.Get();
		if (dm != null)
		{
			DefendHelpers.Log("User Getting In", "A user is getting into a vehicle");
			IEntity vehEntity = SCR_EntityHelper.GetMainParent(pOwnerEntity, true);
			if (vehEntity.GetName() == "Extraction")
			{
				DamageManagerComponent dmgManager = DamageManagerComponent.Cast(pUserEntity.FindComponent(DamageManagerComponent));
				dmgManager.EnableDamageHandling(false);
				DefendHelpers.Log("User Extracted", "A user has been extracted.");
			}
		}
		super.PerformAction(pOwnerEntity, pUserEntity);
	}
}