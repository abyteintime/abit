class YbTest_Mod extends GameMod
	config(Mods);

var transient name packageName;

function YarnboxTest_GetPackageName()
{
	local Object testObject;
	testObject = new (self) class'Object';
	packageName = testObject.GetPackageName();
}

event OnModLoaded()
{
	YarnboxTest_GetPackageName();
}

event OnModUnloaded()
{
}
