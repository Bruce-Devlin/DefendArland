class ExtractionGroup
{
	protected ChimeraAIGroup _group = null;
	bool HasGroup()
	{
		return (_group != null);
	}
	void SetGroup(ChimeraAIGroup newGroup)
	{
		this._group = newGroup;
	}
	
	
 	ChimeraAIGroup GetGroup()
	{
		if (!HasGroup())
		{
			return null;
		}
		return this._group;
	}
	
	protected Vehicle _vehicle = null;
	bool HasVehicle()
	{
		return (_vehicle != null);
	}
	void SetVehicle(Vehicle newVehicle)
	{
		this._vehicle = newVehicle;
	}
	
	
 	Vehicle GetVehicle()
	{
		if (!HasVehicle())
		{
			return null;
		}
		return this._vehicle;
	}
	
	vector targetExtractionPoint = vector.Zero;
}