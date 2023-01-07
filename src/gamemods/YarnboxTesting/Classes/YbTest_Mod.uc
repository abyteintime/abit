class YbTest_Mod extends GameMod
	config(Mods);

var transient name PackageName;

var transient bool CondA;
var transient bool CondB;

function YarnboxTest_GetPackageName()
{
	local Object testObject;
	testObject = new (self) class'Object';
	PackageName = testObject.GetPackageName();
}

function bool GetCondB()
{
	return CondB;
}

function int YarnboxTest_AndAnd()
{
	if (!CondA && GetCondB())
	{
		return 200;
	}
	return 0;
}

event OnModLoaded()
{
	YarnboxTest_GetPackageName();
}

event OnModUnloaded()
{
}
