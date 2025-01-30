modded class SCR_AICombatMoveLogic_Attack
{
	protected override ENodeResult EOnTaskSimulate(AIAgent owner, float dt)
	{
		IEntity entity = IEntity.Cast(m_MyEntity);
		SCR_ChimeraCharacter char = SCR_ChimeraCharacter.Cast(entity);
		bool isZombie = false;
		if (char.IsZombie())
		{
			DefendHelpers.Log("Zombie Simulate", "Doing Zombie Attack logic");
			isZombie = true;
		}
		
		float currentTime_ms = GetGame().GetWorld().GetWorldTime();
		if (currentTime_ms < m_fNextUpdate_ms)
			return ENodeResult.RUNNING;
		m_fNextUpdate_ms = currentTime_ms + m_fUpdateInterval_ms;
		
		if (!OnUpdate(owner, dt))
			return ENodeResult.FAIL;
		
		if (!m_State || !m_MyEntity || !m_Utility || !m_CombatComp || !m_CharacterController)
			return ENodeResult.FAIL;
		
		// Don't run combat movement logic if CombatMove BT is not used now (like in turret)
		SCR_AIBehaviorBase executedBehavior = SCR_AIBehaviorBase.Cast(m_Utility.GetExecutedAction());
		if (executedBehavior && !executedBehavior.m_bUseCombatMove)
			return ENodeResult.RUNNING;
		
		// Update cached variables
		m_fTargetDist = GetTargetDistance();
		m_bCloseRangeCombat = m_fTargetDist < SCR_AICombatMoveUtils.CLOSE_RANGE_COMBAT_DIST;
		m_eThreatState = m_Utility.m_ThreatSystem.GetState();
		m_eStance = m_CharacterController.GetStance();
		m_fWeaponMinDist = m_CombatComp.GetSelectedWeaponMinDist();
		m_eWeaponType = m_CombatComp.GetSelectedWeaponType();
		
		
		/*		
		//------------------------------------------------------------------------------------
		Combat movement logic
		
		Conditions represent states inside which we want to remain.
		
		Conditions are organized based on their priority, highest first.
		
		Within each state there can be extra logic which decides if it's worth to
		send a new request, because even though we have selected a state, we should avoid
		spamming same request over and over.
		
		Conditions for states mostly depend on Combat Move State and its timers.
		
		It is important to write logic in such a way that it doesn't depend on state
		of this node. In this case the state flow also doesn't depend on it, and AI
		does movement is more fluent when switching to a new behavior which also utilizes
		combat movement, including attacking a different target.
		*/
		
		if (isZombie)
		{
			return ENodeResult.RUNNING;
		}
		else if (SuppressedInCoverCondition())
		{
			SuppressedInCoverLogic();
		}
		else if (MoveFromTargetCondition())
		{
			// Too close to target
			// Step away
			if (MoveFromTargetNewRequestCondition())
				PushRequestMoveFromTarget();
		}
		else if (CurrentCoverUselessCondition())
		{
			// Current cover has been compromised, it's not directed at enemy any more
			// Find a new cover nearby
			PushRequestLeaveUselessCover();
		}
		else if (m_State.m_bInCover && m_CharacterController.IsReloading())
		{
			// We're reloading and can't do much else now
			// Hide in cover
			if (m_State.m_bExposedInCover)
				m_State.ApplyRequestChangeStanceInCover(false);
		}
		else if (m_State.m_bInCover && !m_State.m_bExposedInCover)
		{
			// We're in cover but we are still hiding in it, unhide
			m_State.ApplyRequestChangeStanceInCover(true);
		}
		else if (FFAvoidanceCondition())
		{
			if (FFAvoidanceNewRequestCondition())
				PushRequestFFAvoidance();
		}
		else if (MoveToNextPosCondition())
		{
			// We've waited here too long, move to next place
			PushRequestMove();
		}
		else if (!m_State.IsExecutingRequest() && !m_State.m_bInCover)
		{
			// We are stopped and not in cover, manage our stance
			ECharacterStance newStance = ResolveStanceOutsideCover(m_bCloseRangeCombat, m_eThreatState);
			if (newStance > m_eStance)
			{
				// Only let stance go down, no need to get back up
				m_State.ApplyRequestChangeStanceOutsideCover(newStance);
			}
		}
		
		return ENodeResult.RUNNING;
	}
}