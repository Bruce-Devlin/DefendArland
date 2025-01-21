modded class SCR_OrbitingManualCameraComponent
{
	override void EOnCameraFrame(SCR_ManualCameraParam param)
	{
		m_fMinPivotHeight = 999;
		if (!param.isManualInputEnabled)
			return;
		
		//--- ToDo: Better flag handling in his component
		if (
			(param.flag & EManualCameraFlag.ROTATE)
			&&
			(
				GetInputManager().GetActionValue("ManualCameraModifier")
				//--- On gamepad, these editor actions block the orbit action because they use it as a modifer (e.g, lshoulder + lstick horizontal)
				//--- ToDo: Cleaner?
				|| GetInputManager().GetActionValue("EditorTransformRotateYaw")
				|| GetInputManager().GetActionValue("EditorTransformMoveVertical")
			)
		)
		{
			//--- Input movement
			if (m_vTarget == vector.Zero || param.isDirtyExternal)// || param.isManualInput)
			{
				//--- Update orbit target (on orbiting start or upon input from other components)
				if (GetInputManager().IsUsingMouseAndKeyboard())
				{
					if (!GetCenterPos())
						return;
				}
				else
				{
					if (!param.GetCursorWorldPos(m_vTarget))
						return;
				}
				m_vTargetHeightATL = m_vTarget[1] - m_World.GetSurfaceY(m_vTarget[0], m_vTarget[2]);
				m_vTarget = CoordToCamera(m_vTarget);
				m_vDistance = vector.Distance(param.transform[3], m_vTarget);
				m_vDirection = -param.transform[2];
			}

			m_vAngleVelocity = Vector
			(
				GetInputManager().GetActionValue("ManualCameraRotateYaw"),
				-GetInputManager().GetActionValue("ManualCameraRotatePitch"),
				0
			);
			m_vAngleVelocity *= m_fSpeed * param.timeSlice;
			if (m_vAngleVelocity != vector.Zero)
			{
				m_bIsInertia = true;
				param.isManualInput = true;
			}
		}
		else if (m_bIsInertia)
		{		
			//--- Inertia movement	
			m_vAngleVelocity = vector.Lerp(m_vAngleVelocity, vector.Zero, Math.Min(param.timeSlice / m_fInertiaStrength, 1));
			if (m_vAngleVelocity.Length() < INERTIA_THRESHOLD)
				m_bIsInertia = false;
		}
		else
		{
			//--- No movement, terminate
			m_vTarget = vector.Zero;
			return;
		}
		
		//--- Mark camera as orbiting, not moving
		param.flag = param.flag | EManualCameraFlag.ORBIT;
		param.flag = param.flag &~ EManualCameraFlag.MOVE;
		
		//--- No change, terminate
		if (!m_bIsInertia && m_vAngleVelocity == vector.Zero)
			return;
		
		//--- Add rotation
		vector angles = m_vDirection.VectorToAngles().MapAngles();
		vector anglesModified = angles + m_vAngleVelocity;
		anglesModified[1] = Math.Clamp(anglesModified[1], -89, 89);
		m_vDirection = anglesModified.AnglesToVector();
		
		//--- Don't go below terrain level
		vector pos;
		if (param.flag & EManualCameraFlag.ABOVE_TERRAIN)
		{
			//--- Set target height
			vector target = CoordFromCamera(m_vTarget);
			target[1] = m_World.GetSurfaceY(target[0], target[2]) + m_vTargetHeightATL;
			target = CoordToCamera(target);
				
#ifdef DEBUG_ORBIT
			Shape.CreateSphere(ARGBF(10, 0, 1, 1), ShapeFlags.ONCE, CoordFromCamera(target), 0.6);
#endif
			
			//--- Adjust only if camera position is expected to end up below terrain
			pos = target + m_vDirection * m_vDistance;
			vector posWorld = CoordFromCamera(pos - m_vPivotOffset);
			if (posWorld[1] < m_World.GetSurfaceY(posWorld[0], posWorld[2]))
			{
				//--- Move camera to terrain intersection leading from target to expected camera position
				TraceParam trace = new TraceParam();
				trace.Start = CoordFromCamera(target);
				trace.End = CoordFromCamera(target + m_vDirection * m_vDistance - m_vPivotOffset);
				trace.Flags = TraceFlags.WORLD;
				trace.LayerMask = TRACE_LAYER_CAMERA;

				pos = target + m_vDirection * m_vDistance * m_World.TraceMove(trace, null);
			}
		}
		else
		{
			pos = m_vTarget + m_vDirection * m_vDistance;
		}
		//--- Apply
		param.transform[2] = -m_vDirection;
		param.transform[3] = pos;
		param.rotDelta = vector.Zero;
		param.isDirty = true;
		
#ifdef DEBUG_ORBIT
		Shape.CreateSphere(ARGBF(1, 1, 0, 1), ShapeFlags.ONCE, CoordFromCamera(m_vTarget), 0.5);
#endif
	}
}