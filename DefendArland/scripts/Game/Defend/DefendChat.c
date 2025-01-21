class DefendChat
{
	static void HandleCommand(array<string> commandParts, int senderId)
	{
		DefendHelpers.Log("Handling Command", "Command was used by user id: " + senderId.ToString());
		switch (commandParts[0])
		{
			case "/admin":
			break;
		}
	}
}