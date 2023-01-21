injectionset "Take control over DanceOnly flag" {
	{
		into = Function'HatinTimeGameContent.Hat_HUDMenu_HatKidDance.OnOpenHUD',
		select = Opcode.False{-1}.Span,
		generate = StaticFinalFunctionCall(Function'PeacefulAndTranquilForever.PnTF_Injections.DoDanceForever'),
	},
	{
		into = Function'HatinTimeGameContent.Hat_HUDMenu_HatKidDance.Render',
		select = Opcode.False{2}.Span,
		generate = StaticFinalFunctionCall(Function'PeacefulAndTranquilForever.PnTF_Injections.DoDanceForever'),
	},
	{
		into = Function'HatinTimeGameContent.Hat_HUDMenu_HatKidDance.AdvanceMessage',
		select = Opcode.False{1}.Span,
		generate = StaticFinalFunctionCall(Function'PeacefulAndTranquilForever.PnTF_Injections.DoDanceForever'),
	},
}
