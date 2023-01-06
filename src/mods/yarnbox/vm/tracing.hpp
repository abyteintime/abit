#pragma once

namespace yarn {

void
SetupNativeTracing();

void
SetNativeTracingEnabled(bool enabled);

struct EnableNativeTracingInThisScope
{
	EnableNativeTracingInThisScope(bool enable = true);
	~EnableNativeTracingInThisScope();

private:
	bool wasEnabled;
};

}
