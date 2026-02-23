#pragma once

#include <deque>

#include "visualization/telemetry_model.h"

void RenderTelemetryDashboard(const std::deque<TelemetrySample>& samples);
