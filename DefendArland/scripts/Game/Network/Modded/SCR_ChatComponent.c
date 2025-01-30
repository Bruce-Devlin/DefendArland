modded class SCR_ChatComponent
{
	override void OnNewMessage(string msg, int channelId, int senderId)
	{
		DefendManager dm = DefendHelpers.Get();
		if (dm != null)
		{
			DefendHelpers.Log("New Message", "Msg: " + msg + " | senderId: " + senderId.ToString());
			if (DefendHelpers.IsHost())
			{
				if (msg.StartsWith("@"))
				{
					array<string> commandParts = {};
					msg.Split(" ", commandParts, true);
					GetGame().GetCallqueue().Call(DefendChat.HandleCommand, commandParts, senderId);
				}
			}
		}
		super.OnNewMessage(msg, channelId, senderId);
	}

}