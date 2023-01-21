injection "Make the time piece count include mod time pieces" {
	into = Function'HatinTimeGameContent.Hat_SaveGame.UpdateCurrentCollectedTimePieces',
	select = Opcode.Return{-1}.Start,
	generate = StaticFinalFunctionCall(Function'Shenanigans.Shenanigans_Injections.SumUpTotalTimePieceCount', {
		captureSelf = true,
	}),
}
