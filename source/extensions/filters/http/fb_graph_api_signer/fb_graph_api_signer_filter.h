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

    // Extract an access token from the request, so it can be used to
    // generate the appsecret_proof signature. The precedent is:
    // 1. Look for an access token from the 'access_token' query parameter.
    // 2. Look for an access token in the 'Authorization: Bearer <MY_ACCESS_TOKEN>' header
    //
    // If no access tokens are found, skip signing and pass the request through
    static absl::optional<absl::string_view> extractAccessToken(const Http::RequestHeaderMap& headers,
                                                                const Http::Utility::QueryParams& query_params);
};

} // namespace FbGraphApiSigner
} // namespace HttpFilters
} // namespace Extensions
} // namespace Envoy

