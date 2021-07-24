#pragma once

#include "envoy/http/filter.h"
#include "envoy/http/query_params.h"
#include "envoy/stats/scope.h"
#include "source/extensions/filters/http/common/pass_through_filter.h"

namespace Envoy {
namespace Extensions {
namespace HttpFilters {
namespace FbGraphApiSigner {

class Filter: public Http::PassThroughDecoderFilter, Logger::Loggable<Logger::Id::filter> {
public:
    Filter(const std::shared_ptr<std::string>& app_secret_);

    Http::FilterHeadersStatus decodeHeaders(Http::RequestHeaderMap& headers,
                                            bool end_stream) override;
private:
    const std::shared_ptr<std::string> app_secret_;

    static absl::optional<std::string> extractAccessToken(const Http::RequestHeaderMap& headers,
                                                          const Http::Utility::QueryParams& query_params);
};

} // namespace FbGraphApiSigner
} // namespace HttpFilters
} // namespace Extensions
} // namespace Envoy

