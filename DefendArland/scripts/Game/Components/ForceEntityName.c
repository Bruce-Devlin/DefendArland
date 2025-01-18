class ForceEntityNameClass : ScriptComponentClass
{
	
}

class ForceEntityName : ScriptComponent
{
	[Attribute(defvalue: "", UIWidgets.EditBox, category: "Force Entity Name", desc: "The name you would like to force this entity to have.")]
	string entityName;
	
	override void OnPostInit(IEntity owner)
	{
		GetGame().GetCallqueue().CallLater(SetEntityName, 1 * 1000, false, owner);
	}
	
	void SetEntityName(IEntity entity)
	{
		if (entityName != "" && entity != null)
			entity.SetName(entityName);
	}
}