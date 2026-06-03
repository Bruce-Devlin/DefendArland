modded class SCR_AICombatMoveLogic_HideFromUnknownFire
{
	override ENodeResult EOnTaskSimulate(AIAgent owner, float dt)
	{
		IEntity myEntity = owner.GetControlledEntity();
		SCR_ChimeraCharacter char = SCR_ChimeraCharacter.Cast(myEntity);
		bool isZombie = false;
		if (char.IsZombie())
		{
			isZombie = true;
		}
		
		if (!m_State || !myEntity)
			return ENodeResult.FAIL;
		
		
		vector threatPos;
		GetVariableIn(PORT_POSITION, threatPos);
		if (threatPos == vector.Zero)
			return ENodeResult.FAIL;
		
		float distToThreat = vector.Distance(myEntity.GetOrigin(), threatPos);
		CombatMoveLogic(threatPos, distToThreat);
			
		//SetVariableOut(PORT_COMPLETE_ACTION);
		
		return ENodeResult.SUCCESS;
	}
}