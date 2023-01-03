class YbTest_PnTPatch extends Hat_HUDMenu_HatKidDance;

const DanceOnlyPrettyPlease = true;

function bool DisablesCameraMovement(HUD h)
{
	return false;
}

// DISCLAIMER:
// GfB folks I'm terribly sorry for copy-pasting your code in here.
// I just needed a quick proof of concept and this was one of the easiest things to verify working.

simulated function OnOpenHUD(HUD H, optional string command)
{
	local Hat_player ply;
	`if(`isdefined(WITH_DLC1))
	local Hat_StatusEffect_RiftCollapse se;
	`endif
	local Vector CameraLocation;
	local Rotator CameraRotation;
	local AnimNodeBlend BlendNode;
	Super(Hat_HUDMenu).OnOpenHUD(H, command);

	ApplyLocalization();

	H.PlayerOwner.PlayerCamera.GetCameraViewPoint(CameraLocation, CameraRotation);
	CameraRotation.Pitch = 0;
	CameraRotation.Roll = 0;

	CameraActors[0] = H.PlayerOwner.Spawn(class'DynamicCameraActor',,, CameraLocation, CameraRotation,,true);
	CameraActors[0].FovAngle = 140;
	H.PlayerOwner.SetViewTarget(CameraActors[0]);

	CameraActors[1] = H.PlayerOwner.Spawn(class'DynamicCameraActor',,, CameraLocation, CameraRotation,,true);
	CameraActors[1].FovAngle = 50;
	H.PlayerOwner.SetViewTargetWithBlend(CameraActors[1], 0.6);

	DynamicMusicNode = new class'Hat_MusicNodeBlend_Dynamic';
	DynamicMusicNode.Music = Music;
	DynamicMusicNode.Priority = 500;
	DynamicMusicNode.BPM = 202;
	DynamicMusicNode.BlendTimes[0] = 0.6; // 0.6 fade out
	DynamicMusicNode.BlendTimes[1] = 0.05; // 0.05s fade in
	`PushMusicNode(DynamicMusicNode);

	ply = Hat_Player(H.PlayerOwner.Pawn);
	OriginalPlayerLocation = ply.Location;
	ply.SetPhysics(Phys_None);
	ply.SetLocation(CameraLocation + Vector(CameraRotation)*300 + vect(0,0,0));
	ply.DepthPriorityGroup = SDPG_Foreground;
	ply.UpdateDepthPriorityGroup();

	`if(`isdefined(WITH_DLC1))
	se = Hat_StatusEffect_RiftCollapse(ply.GiveStatusEffect(class'Hat_StatusEffect_RiftCollapse'));
	if (se != None)
	{
		se.OnlyActiveWhileMoving = false;
		se.TrailClass = class'Hat_NPC_Player_RiftCollapse_Trail_Dance';
	}
	`endif

	ply.RiftCollapseAmbienceParticle.SetActive(false);
	BlendNode = AnimNodeBlend(ply.Mesh.FindAnimNode('Dance'));
	BlendNode.SetBlendTarget(1, 0.0f);
	ply.SetRotationTowardsCamera();
	ply.SetExpression(EExpressionType_Smug);
	// ply.SetExpression(EExpressionType_Dead);
	if (ply.ExpressionComponent != None)
	{
		ply.ExpressionComponent.ForcedViewTarget = CameraActors[1];
		ply.ExpressionComponent.Eyes.NextSeek = 0;
	}

	BackgroundMeshComponent = new class'StaticMeshComponent';
	BackgroundMeshComponent.SetStaticMesh(BackgroundMesh);
	BackgroundMeshComponent.SetMaterial(0, BackgroundMaterial);
	BackgroundMeshComponent.SetActorCollision(false,false);
	BackgroundMeshComponent.SetAbsolute(true,true,true);
	BackgroundMeshComponent.SetTranslation(CameraLocation + Vector(CameraRotation)*1000);
	BackgroundMeshComponent.SetRotation(Rotator(Vector(CameraRotation)*-1));
	BackgroundMeshComponent.SetDepthPriorityGroup(SDPG_Foreground);
	BackgroundMeshComponent.SetScale(30);
	ply.AttachComponent(BackgroundMeshComponent);

	TextMeshComponent = new class'StaticMeshComponent';
	TextMeshComponent.SetStaticMesh(BackgroundMesh);
	TextMeshComponent.SetActorCollision(false,false);
	TextMeshComponent.SetAbsolute(true,true,true);
	TextMeshComponent.SetTranslation(CameraLocation + Vector(CameraRotation)*30 + vect(0,0,-5.7));
	TextMeshComponent.SetRotation(Rotator(Vector(CameraRotation)*-1));
	TextMeshComponent.SetDepthPriorityGroup(SDPG_Foreground);
	TextMeshComponent.SetScale(0.072);
	TextMeshComponent.SetHidden(true);
	TextMeshComponent.SetScale3D(vect(1,1,0.25));
	ply.AttachComponent(TextMeshComponent);

	TitleMeshComponent = new class'StaticMeshComponent';
	TitleMeshComponent.SetStaticMesh(BackgroundMesh);
	TitleMeshComponent.SetActorCollision(false,false);
	TitleMeshComponent.SetAbsolute(true,true,true);
	TitleMeshComponent.SetTranslation(CameraLocation + Vector(CameraRotation)*600 + vect(0,0,120));
	TitleMeshComponent.SetRotation(Rotator(Vector(CameraRotation)*-1));
	TitleMeshComponent.SetDepthPriorityGroup(SDPG_Foreground);
	TitleMeshComponent.SetScale(2.5);
	TitleMeshComponent.SetHidden(true);
	TitleMeshComponent.SetScale3D(vect(1,1,0.25));
	ply.AttachComponent(TitleMeshComponent);

	Hat_HUD(H).SetVHSRewindPostProcess(true, 'VHS_Dance');

	CreateScriptedCanvas_Messages();

	CurrentMessageTime = -0.5f;
	OptionSelected = INDEX_NONE;
	SelectedButton = INDEX_NONE;
	ShouldShowOptions = true;

	//PlayerCurrency = Hat_SaveGame(`SaveManager.SaveData).MyBadgePoints;
	PlayerCurrency = `GameManager.GetEnergyBits();

	Hat_HUD(H).bShowDefaultHUD = false;
	class'Engine'.static.GetEngine().bMirrorMode = false;

	if (class'Hat_SaveBitHelper'.static.HasLevelBit("DeathWish_EasyMode_LongVersion", 1, `GameManager.HubMapName) && !DanceOnlyPrettyPlease)
		TextMessages = TextMessages_Short;
}

function bool Render(HUD H)
{
	local int NewButtonIndex;
	local float FadeIn, FadeOut, Alpha, maxscale;
	if (!Super(Hat_HUDMenu).Render(H)) return false;

	maxscale = FMax(H.Canvas.ClipX, H.Canvas.ClipY);

	NewButtonIndex = INDEX_NONE;
	if (CurrentMessageIndex == TextMessages.Length-1 && ShouldShowOptions && !DanceOnlyPrettyPlease)
	{
		FadeIn = FClamp(CurrentMessageTime/TextFadeInDuration,0,1);
		FadeIn = 1-((1-FadeIn)**2);
		FadeOut = 1-FClamp((CurrentMessageTime-(1.f-TextFadeInDuration))/TextFadeInDuration,0,1);
		FadeOut = 1-((1-FadeOut)**2);
		Alpha = FadeIn*FadeOut;

		class'Hat_HUDMenuActSelect'.static.RenderCurrencyCounter(H, self, maxscale*0.01, H.Canvas.ClipY - maxscale*0.01, PlayerCurrency, FeeIcon, Alpha, BadgePointsErrorAnimation);

		H.Canvas.SetDrawColor(128,255,128,255*Alpha*Alpha);
		if (DrawButton(H, H.Canvas.ClipX*Lerp(0, 0.2, Alpha), H.Canvas.ClipY*0.5, H.Canvas.ClipX*0.2, ButtonMessage[0], 0, FeeIcon, EasyModeFee)) NewButtonIndex = 0;
		H.Canvas.SetDrawColor(255,128,128,255*Alpha*Alpha);
		if (DrawButton(H, H.Canvas.ClipX*Lerp(1.0, 0.8, Alpha), H.Canvas.ClipY*0.5, H.Canvas.ClipX*0.2, ButtonMessage[1], 1)) NewButtonIndex = 1;
	}

	if (NewButtonIndex == INDEX_NONE && !Hat_HUD(H).IsGamepad() && MouseActivated)
		SelectedButton = INDEX_NONE;

	return true;
}

function AdvanceMessage(HUD H)
{
	CurrentMessageIndex++;

	if (DanceOnlyPrettyPlease && CurrentMessageIndex >= TextMessages.Length-1)
	{
		CurrentMessageIndex = 0;
	}
	else if (CurrentMessageIndex >= TextMessages.Length && ShouldShowOptions && OptionSelected != INDEX_NONE)
	{
		if (TitleMeshComponent != None)	TitleMeshComponent.SetHidden(true);
		TextMessages = OptionSelected == 0 ? AcceptMessage : ExitMessage;
		CurrentMessageIndex = 0;
		ShouldShowOptions = false;

		// Mark that we've sat through the whole thing
		class'Hat_SaveBitHelper'.static.AddLevelBit("DeathWish_EasyMode_LongVersion", 1, `GameManager.HubMapName);
	}
	else if (CurrentMessageIndex >= TextMessages.Length && !ShouldShowOptions && OptionSelected != INDEX_NONE)
	{
		ExitTime = 0.6;
		H.PlayerOwner.SetViewTargetWithBlend(CameraActors[0], 0.6);

		if (OnHUDClosedDelegate != None)
			OnHUDClosedDelegate(H);
		OnHUDClosedDelegate = None;
		DynamicMusicNode.Stop();
		DynamicMusicNode = None;
	}

	if (CurrentMessageIndex < TextMessages.Length)
	{
		CurrentMessageTime = 0;
		UpdateTextCanvas();
	}
	if (CurrentMessageIndex == TextMessages.Length-1 && OptionSelected == INDEX_NONE)
	{
		RequiresMouse = TRUE;
		if (TitleMeshComponent != None)	TitleMeshComponent.SetHidden(false);
		if (!IsGamepad(H))
		{
			SetMouseHidden(H, false);
		}
	}
}
