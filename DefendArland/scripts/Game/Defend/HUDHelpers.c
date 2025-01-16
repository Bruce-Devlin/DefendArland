//! Create a new instance of me to manage the HUD within the Defend Gamemode.
class HUDHelpers
{
    protected Widget hudRoot;
    protected RichTextWidget waveTextWidget;
    protected RichTextWidget livesTextWidget;
    protected RichTextWidget timerTextWidget;
    protected RichTextWidget enemiesTextWidget;
    protected ImageWidget separatorWidget;

    protected bool initComplete = false; 
    protected int widgetId = 0;
    protected DefendManager dm = null;

    //! Checks if the HUD initialization is complete
    bool hasInitComplete()
    {
        return initComplete;
    }

    //! Initializes the HUD with the specified layout and debug flag
    //! -------------------------------------
    //! Parameters:
    //! uiHUDLayout - The layout of the HUD (XML/Widget definition).
    //! isDebug - Flag indicating if the debug mode should be activated.
    void Init(string uiHUDLayout, bool isDebug) 
    { 
        DefendHelpers.Log("Init HUD", "Starting HUD");
        hudRoot = GetGame().GetWorkspace().CreateWidgets(uiHUDLayout);
        hudHelpersDebug = isDebug;
        dm = DefendHelpers.Get();

        //! Find widgets by their names and cast to appropriate types
        waveTextWidget = RichTextWidget.Cast(hudRoot.FindAnyWidget("Wave"));
        livesTextWidget = RichTextWidget.Cast(hudRoot.FindAnyWidget("Lives"));
        timerTextWidget = RichTextWidget.Cast(hudRoot.FindAnyWidget("Timer"));
        enemiesTextWidget = RichTextWidget.Cast(hudRoot.FindAnyWidget("Enemies"));
        separatorWidget = ImageWidget.Cast(hudRoot.FindAnyWidget("Separator"));

        //! Error checking: ensure widgets are found
        if (waveTextWidget == null) DefendHelpers.Log("Error", "Failed to find the HUD wave Text!");
        if (livesTextWidget == null) DefendHelpers.Log("Error", "Failed to find the HUD lives Text!");
        if (timerTextWidget == null) DefendHelpers.Log("Error", "Failed to find the HUD timer Text!");
        if (enemiesTextWidget == null) DefendHelpers.Log("Error", "Failed to find the HUD enemies Text!");
        if (separatorWidget == null) DefendHelpers.Log("Error", "Failed to find the HUD separator Widget!");

        initComplete = true;
    }

    //! Formats the wave information to display
    //! -------------------------------------
    //! Parameters:
    //! currWave - The current wave number.
    //! maxWave - The maximum wave number in the game.
    //! -------------------------------------
    //! Returns:
    //! A formatted string displaying the current wave and the maximum wave.
    protected string FormatWaves(int currWave, int maxWave)
    {
        string maxWaveText = "";
        if (maxWave != 0) maxWaveText = "/" + maxWave.ToString();
        string textToShow = "WAVE " + currWave.ToString() + maxWaveText;
        return textToShow;
    }

    //! Formats the lives remaining information to display
    //! -------------------------------------
    //! Parameters:
    //! livesLeft - The number of lives remaining.
    //! -------------------------------------
    //! Returns:
    //! A string displaying the number of team respawns left, or an empty string if no lives remain.
    protected string FormatLives(int livesLeft)
    {
        if (livesLeft != 0)
        {
            string textToShow = "Team respawns remaining: " + livesLeft;
            return textToShow;
        }
        else
        {
            return "";
        }
    }

    //! Formats the timer information to display
    //! -------------------------------------
    //! Parameters:
    //! timerSeconds - The number of seconds left on the timer.
    //! -------------------------------------
    //! Returns:
    //! A string displaying the remaining time until the next wave, or an empty string if no time remains.
    protected string FormatTimer(int timerSeconds)
    {
        if (timerSeconds > 0)
        {
            string textToShow = "Time until next wave: " + timerSeconds + " seconds...";
            return textToShow;
        }
        else
        {
            return "";
        }
    }

    //! Formats the enemies remaining information to display
    //! -------------------------------------
    //! Parameters:
    //! totalEnemies - The total number of enemies remaining.
    //! -------------------------------------
    //! Returns:
    //! A string displaying the number of enemies remaining, or an empty string if no enemies remain.
    protected string FormatEnemies(int totalEnemies)
    {
        if (totalEnemies != 0)
        {
            string textToShow = "Enemies remaining: " + totalEnemies;
            return textToShow;
        }
        else
        {
            return ""; //! Return empty if no enemies are left
        }
    }

    //! Destroys the HUD and removes all widgets
    //! -------------------------------------
    //! This method will reset the HUD components, stop the timer, and clear any queued function calls.
    void Destroy()
    { 
        if (!initComplete) return;
        
        timerActive = false;
        timerTimeLeft = -1; 
        shouldStartTimer = false;
        initComplete = false;
        GetGame().GetCallqueue().Clear();
        hudRoot.RemoveFromHierarchy();
    }

    //! Shows the HUD with updated wave, lives, timer, and enemy information
    //! -------------------------------------
    //! Parameters:
    //! currWave - The current wave number.
    //! maxWave - The maximum wave number in the game.
    //! livesRemaining - The number of lives remaining for the team.
    //! timerSeconds - The number of seconds remaining on the timer (default is 0).
    //! totalEnemies - The total number of enemies remaining (default is 0).
    //! customText - Custom text to display in place of the wave information (default is an empty string).
    void ShowHUD(int currWave, int maxWave, int livesRemaining, int timerSeconds = 0, int totalEnemies = 0, string customText = "")
    {
        if (!initComplete) return;
        
        if (hudHelpersDebug) DefendHelpers.Log("Showing HUD", "Wave: " + currWave + "/" + maxWave + " | Timer: " + timerSeconds + " | Lives Remaining: " + livesRemaining + " | Custom Text: " + customText + " | Total Enemies: " + totalEnemies);
        
        if (timerActive)
        {
            timerTimeLeft = -1;
            timerActive = false;
            GetGame().GetCallqueue().RemoveByName(this, "StartTimer");
            GetGame().GetCallqueue().RemoveByName(this, "TimerTick");
        }

        UpdateHUDDisplayValues(currWave, maxWave, timerSeconds, totalEnemies, livesRemaining, customText);
        if (hudHelpersDebug) DefendHelpers.Log("HUD Text Set", "Wave text set.");

        if ((timerSeconds > 0 || totalEnemies > 0) && DefendHelpers.IsHost())
        {
            shouldStartTimer = true;
            GetGame().GetCallqueue().Call(StartTimer, timerSeconds, totalEnemies, livesRemaining, currWave, maxWave, customText);
            if (hudHelpersDebug) DefendHelpers.Log("HUD Started timer", "Wave timer text set.");
        }

        if (hudHelpersDebug) DefendHelpers.Log("HUD Set Visible", "The HUD should now be visible.");
    }

    int timerTimeLeft = -1; 
    bool timerActive = false;
    bool shouldStartTimer = false;
    bool hudHelpersDebug = false;

    //! Starts the timer countdown
	//! -------------------------------------
    //! Parameters:
    //! time - The initial time in seconds for the countdown.
    //! enemies - The number of enemies remaining.
    //! livesLeft - The number of lives remaining.
    //! currWave - The current wave number.
    //! maxWave - The maximum wave number in the game.
    void StartTimer(int time, int enemies, int livesLeft, int currWave, int maxWave, string customText = "")
    {
        if (hudHelpersDebug) DefendHelpers.Log("Timer active: " + timerActive, "HUD timer is " + timerActive + " with " + time + " seconds left & " + enemies + " enemies remaining");

        if (timerTimeLeft == -1 && !timerActive && shouldStartTimer)
        {
            if (hudHelpersDebug) DefendHelpers.Log("Started HUD timer", "HUD timer will start ticking.");
            GetGame().GetCallqueue().RemoveByName(this, "StartTimer");
            GetGame().GetCallqueue().RemoveByName(this, "TimerTick");
            
            shouldStartTimer = false;
            timerActive = true;
            timerTimeLeft = time;
        }
        else if ((timerTimeLeft <= 0) && timerActive && (enemies <= 0))
        {
            if (hudHelpersDebug) DefendHelpers.Log("Stopped HUD timer", "HUD timer will no longer tick.");
            GetGame().GetCallqueue().RemoveByName(this, "TimerTick");
            timerActive = false;
            timerTimeLeft = -1;
            
            waveTextWidget.SetText("");
            timerTextWidget.SetText("");
            enemiesTextWidget.SetText("");
        }

        if (timerActive && (timerTimeLeft > 0 || enemies > 0))
        {
            if (hudHelpersDebug) DefendHelpers.Log("HUD Timer pre-tick", "Waiting for 1 second tick...");
            GetGame().GetCallqueue().CallLater(TimerTick, 1000, false, time, enemies, livesLeft, currWave, maxWave, customText);
            return;
        }
    }

    //! Handles each tick of the timer, updating the HUD and sending updates
    //! -------------------------------------
    //! Parameters:
    //! timeLeft - The time left on the timer.
    //! initalEnemiesLeft - The initial number of enemies left.
    //! initialLivesLeft - The initial number of lives left.
    //! currWave - The current wave number.
    //! maxWave - The maximum wave number.
    void TimerTick(int timeLeft, int initalEnemiesLeft, int initialLivesLeft, int currWave, int maxWave, string customText = "")
    {
        if (timerTimeLeft > 0) timerTimeLeft = timerTimeLeft - 1;
        if (hudHelpersDebug) DefendHelpers.Log("HUD Timer Tick", "HUD timer ticked with " + timerTimeLeft + " seconds left.");
        
        UpdateHUDDisplayValues(currWave, maxWave, timerTimeLeft, initalEnemiesLeft, initialLivesLeft, customText);
        
        if (DefendHelpers.IsHost()) dm.SendHUDUpdate(currWave, maxWave, timerTimeLeft, customText, 0);
        
        GetGame().GetCallqueue().Call(StartTimer, timerTimeLeft, initalEnemiesLeft, initialLivesLeft, currWave, maxWave, customText);
    }

    //! Updates the HUD display values with current wave, timer, lives, and enemies information
    //! -------------------------------------
    //! Parameters:
    //! currWave - The current wave number.
    //! maxWave - The maximum wave number.
    //! timeLeft - The number of seconds left on the timer.
    //! enemiesLeft - The number of remaining enemies.
    //! livesRemaining - The number of remaining lives.
    //! customText - Custom text for the wave display (default is an empty string).
    protected void UpdateHUDDisplayValues(int currWave, int maxWave, int timeLeft, int enemiesLeft, int livesRemaining, string customText = "")
    {
        if (customText == "")
            waveTextWidget.SetText(FormatWaves(currWave, maxWave));
        else
            waveTextWidget.SetText(customText);
		
        livesTextWidget.SetText(FormatLives(livesRemaining));

        if (timeLeft <= 0)
        {
            timerTextWidget.SetText(FormatEnemies(enemiesLeft));
            enemiesTextWidget.SetText("");
        }
        else
        {
            timerTextWidget.SetText(FormatTimer(timeLeft));
            enemiesTextWidget.SetText(FormatEnemies(enemiesLeft));
        }
    }
}