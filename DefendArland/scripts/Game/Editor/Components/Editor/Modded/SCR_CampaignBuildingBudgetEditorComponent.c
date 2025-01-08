modded class SCR_CampaignBuildingBudgetEditorComponent
{
	override int GetCurrentBudgetValue(EEditableEntityBudget type)
	{
		DefendManager dm = DefendHelpers.Get();
		if (dm != null)
		{
			if (dm.CanBuild()) return super.GetCurrentBudgetValue(type);
			else return 0;
		}
		else return super.GetCurrentBudgetValue(type);
	}
}