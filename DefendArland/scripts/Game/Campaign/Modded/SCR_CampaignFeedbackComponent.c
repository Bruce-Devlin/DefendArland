modded class SCR_CampaignFeedbackComponent
{
	override void ShowHint(EHint hintID, bool showImmediately = false, bool showMultipleTimes = false)
	{
		DefendManager dm = DefendHelpers.Get();
		if (dm != null)
		{
			return;
		}
		else super.ShowHint(hintID, showImmediately, showMultipleTimes)
	}

}