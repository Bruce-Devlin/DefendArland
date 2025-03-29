modded class SCR_CharacterDamageManagerComponent
{
 	override void OnPostInit(IEntity owner)
	{
		DefendManager dm = DefendHelpers.Get();
		if (dm != null)
		{
			if (owner.FindComponent(ZombieController) != null)
			{
				SoundKnockout();
				DefendHelpers.Log("DamageManager for Zombie", "Init the Damage Manager for a zombie.");
				array<HitZone> hitZones = {};
				GetAllHitZones(hitZones);
				foreach (HitZone hitZone: hitZones)
				{
					
					SCR_CharacterHitZone characterHitZone = SCR_CharacterHitZone.Cast(hitZone);
					if (characterHitZone != null)
					{
						DefendHelpers.Log("Making zombie clothes dirty", "Make a zombies clothes dirty.");
						AddBloodToClothes(characterHitZone, 100);
					}
				}
			}
		}
		super.OnPostInit(owner);
	}
}