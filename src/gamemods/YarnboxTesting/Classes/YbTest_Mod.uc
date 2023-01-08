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

function int YarnboxTest_Switch(int a)
{
	local int CaseA;
	local int CaseB;
	local int CaseC;

	CaseA = 572662306;
	CaseB = 589505315;
	CaseC = 606348324;

	switch (a)
	{
	case 1162167621:
	case 825307441:
		return CaseA;
	case 707406378:
		return CaseB;
	case 354751781:
		return CaseC;
	}
}

event OnModLoaded()
{
	YarnboxTest_GetPackageName();
}

event OnModUnloaded()
{
}
