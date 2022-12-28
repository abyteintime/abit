#include "abit/loader/logging_init.hpp"

#include <chrono>
#include <memory>

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

#include "fmt/chrono.h"
#include "spdlog/pattern_formatter.h"
#include "spdlog/sinks/rotating_file_sink.h"
#include "spdlog/sinks/wincolor_sink.h"
#include "spdlog/spdlog.h"

static spdlog::sink_ptr consoleSink;
static spdlog::sink_ptr fileSink;
static spdlog::sink_ptr mainSink;
static std::shared_ptr<spdlog::logger> logger;

using Clock = std::chrono::steady_clock;
using Instant = std::chrono::time_point<Clock>;
static Instant initTime;

namespace abit {

struct LogProcessTime : spdlog::custom_flag_formatter
{
	virtual void format(const spdlog::details::log_msg&, const std::tm&, spdlog::memory_buf_t& dest)
		override
	{
		Instant now = Clock::now();
		std::chrono::duration<double> timeDuration = now - initTime;
		double time = timeDuration.count();
		fmt::format_to(fmt::appender(dest), FMT_STRING("[{:6.3f}]"), time);
	}

	virtual std::unique_ptr<spdlog::custom_flag_formatter> clone() const override
	{
		return spdlog::details::make_unique<LogProcessTime>();
	}
};

struct LogCategory : spdlog::custom_flag_formatter
{
	virtual void format(
		const spdlog::details::log_msg& message,
		const std::tm&,
		spdlog::memory_buf_t& dest
	) override
	{
		if (message.logger_name.size() > 0) {
			dest.append(message.logger_name);
			dest.append(std::string_view(": "));
		}
	}

	virtual std::unique_ptr<spdlog::custom_flag_formatter> clone() const override
	{
		return spdlog::details::make_unique<LogCategory>();
	}
};

static std::unique_ptr<spdlog::pattern_formatter>
LogFormatter()
{
	auto formatter = std::make_unique<spdlog::pattern_formatter>();
	formatter->add_flag<LogProcessTime>('T');
	formatter->add_flag<LogCategory>('C');
	return formatter;
}

}

void
abit::InitializeLogging() noexcept
{
	AllocConsole();

	initTime = Clock::now();

	auto consoleFormatter = LogFormatter();
	consoleFormatter->set_pattern("%T %^%L%L%$ | %C%v");
	consoleSink = std::make_shared<spdlog::sinks::wincolor_stdout_sink_mt>();
	consoleSink->set_formatter(std::move(consoleFormatter));

	fileSink = std::make_shared<spdlog::sinks::rotating_file_sink_mt>(
		"ByteinTime/Logs/Log.txt", 1024 * 1024 * 64, 10, true
	);

	spdlog::set_level(spdlog::level::debug);
	spdlog::default_logger()->sinks() = { consoleSink, fileSink };

	spdlog::flush_every(std::chrono::seconds(5));
}

void
abit::DeinitializeLogging() noexcept
{
	spdlog::shutdown();
}

spdlog::sink_ptr&
abit::GetConsoleSink()
{
	return consoleSink;
}

spdlog::sink_ptr&
abit::GetFileSink()
{
	return fileSink;
}
