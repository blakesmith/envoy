#pragma once

#include "envoy/http/filter.h"
#include "envoy/stats/scope.h"
#include "source/extensions/filters/http/common/pass_through_filter.h"

namespace Envoy {
namespace Extensions {
namespace HttpFilters {
namespace FbGraphApiSigner {

class Filter: public Http::PassThroughDecoderFilter, Logger::Loggable<Logger::Id::filter> {
public:
    Http::FilterHeadersStatus decodeHeaders(Http::RequestHeaderMap& headers,
                                            bool end_stream) override;

    Http::FilterDataStatus decodeData(Buffer::Instance& data, bool end_stream) override;
};

} // namespace FbGraphApiSigner
} // namespace HttpFilters
} // namespace Extensions
} // namespace Envoy

