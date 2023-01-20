class Shenanigans_Injections extends Object;

function SumUpTotalTimePieceCount(Hat_SaveGame SaveGame)
{
	SaveGame.CurrentCollectedTimePieces += SaveGame.CurrentCollectedTimePieces_Mods;
}
