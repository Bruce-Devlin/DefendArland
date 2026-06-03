//! Create a new instance of me to manage Hints within the Defend Gamemode.
class HintHelpers
{
    //! Protected reference to the SCR_HintManagerComponent, which is responsible for managing hints
    protected SCR_HintManagerComponent hintComponent = null;
    
    //! Method to display a hint with a specified header, message, duration, and optional timer display.
    //! -------------------------------------
    //! Parameters:
    //! header - The title of the hint.
    //! message - The body text of the hint.
    //! secondsToShow - Duration for which the hint should be displayed (default is 10 seconds).
    //! showTimer - Whether to show a countdown timer for the hint (default is true).
    void ShowHint(string header, string message, int secondsToShow = 10, bool showTimer = true, int playerId = 0, bool isDebug = false)
    {
		DefendManager dm = DefendHelpers.Get();
		
		if ((playerId == dm.localPlayerId) || playerId == 0)
		{
			if (isDebug) DefendHelpers.Log("Presenting Hint", "Presenting hint to: " + dm.localPlayerId + " | " + header + " | " + message + " | " + secondsToShow + " secs");

	        hintComponent = SCR_HintManagerComponent.GetInstance();
	        
	        EHint type = EHint.UNDEFINED;
	
	        SCR_HintUIInfo hintInfo = SCR_HintUIInfo.CreateInfo(message, header, secondsToShow, type, EFieldManualEntryId.NONE, showTimer);
	        
	        if (hintComponent != null)
	        {
	            if (hintComponent.GetCurrentHint() != null)
	            {
	                bool canHide = hintComponent.Hide();
	                if (!canHide)
	                {
	                    DefendHelpers.Log("ERROR", "Couldn't hide current Hint for new one.");
	                }
	            }
	            
	            bool hintShown = hintComponent.Show(hintInfo);
	            if (hintShown) 
	            {
					if (isDebug) DefendHelpers.Log("Hint Shown", "You should have seen the hint by now.");
					GetGame().GetCallqueue().CallLater(ClearHint, secondsToShow * 1000, false, hintInfo);
				}
	            else 
	                DefendHelpers.Log("ERROR", "Couldn't show hint!");
	        }
	        else 
	        {
	            DefendHelpers.Log("ERROR", "No Hint Component to show?");
	        }
		}
    }
	
	void ClearHint(SCR_HintUIInfo oldHint)
	{
		hintComponent = SCR_HintManagerComponent.GetInstance();
	   		        
	    if (hintComponent != null)
		{
			SCR_HintUIInfo currentHint = hintComponent.GetCurrentHint();
	    	if (currentHint == oldHint)
	       	{
	        	hintComponent.Hide();
	        }
		}
	}
}