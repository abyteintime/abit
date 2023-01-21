#include "yarnbox/vm/tracing.hpp"

#include "abit/loader/logging.hpp"

#include "abit/procs/3d.hpp"

#include "abit/ue/FFrame.hpp"
#include "abit/ue/UObject.hpp"

#include "yarnbox/bytecode/opcode.hpp"

using namespace yarn;
using namespace ue;

using NativeFunc = void (*)(UObject* self, FFrame& frame, void* returnValue);
static NativeFunc O_GNatives[opcodeCount];

static bool nativeTracingEnabled = false;
static bool nativeTracingEnabledInThisScope = false;

template<uint32_t Base>
static void
TracingNative(UObject* self, FFrame& frame, void* returnValue)
{
	uint8_t opcodeByte = frame.ip[Base == 0 ? -1 : 0];
	Opcode opcode = static_cast<Opcode>(Base * 256 + static_cast<size_t>(opcodeByte));
	if (nativeTracingEnabled && nativeTracingEnabledInThisScope) {
		size_t ip = static_cast<size_t>(frame.ip - frame.function->bytecode.dataPtr);
		spdlog::trace(
			"exec {}.{} @ {:04x} | {}",
			frame.function->outer->GetName().ToString(),
			frame.function->GetName().ToString(),
			ip,
			OpcodeToString(opcode)
		);
	}
	O_GNatives[static_cast<size_t>(opcode)](self, frame, returnValue);
}

template<size_t Base>
static inline void
InjectTracingNativesInner()
{
	NativeFunc* natives = abit::procs::G_3de4cdbe920d9e3dfbcfcca2b19b3ab7_0.As<NativeFunc>();

	for (size_t i = 0; i < 256; ++i) {
		size_t op = Base * 256 + i;
		O_GNatives[op] = natives[op];
		natives[op] = TracingNative<Base>;
	}
}

void
yarn::SetupNativeTracing()
{
	InjectTracingNativesInner<0>();

	// TODO: Figure out what's causing these to crash, and fix them.
	// InjectTracingNativesInner<1>();
	// InjectTracingNativesInner<2>();
	// InjectTracingNativesInner<3>();
	// InjectTracingNativesInner<4>();
	// InjectTracingNativesInner<5>();
	// InjectTracingNativesInner<6>();
	// InjectTracingNativesInner<7>();
	// InjectTracingNativesInner<8>();
	// InjectTracingNativesInner<9>();
	// InjectTracingNativesInner<10>();
	// InjectTracingNativesInner<11>();
	// InjectTracingNativesInner<12>();
	// InjectTracingNativesInner<13>();
	// InjectTracingNativesInner<14>();
	// InjectTracingNativesInner<15>();
}

void
yarn::SetNativeTracingEnabled(bool enabled)
{
	nativeTracingEnabled = enabled;
}

EnableNativeTracingInThisScope::EnableNativeTracingInThisScope(bool enable)
	: wasEnabled(nativeTracingEnabledInThisScope)
{
	nativeTracingEnabledInThisScope = enable;
}

EnableNativeTracingInThisScope::~EnableNativeTracingInThisScope()
{
	nativeTracingEnabledInThisScope = wasEnabled;
}
