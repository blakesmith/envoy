#include "source/extensions/filters/http/fb_graph_api_signer/fb_graph_api_signer_filter.h"

#include "source/common/common/hex.h"
#include "source/common/crypto/utility.h"
#include "source/common/http/headers.h"
#include "source/common/http/utility.h"

namespace Envoy {
namespace Extensions {
namespace HttpFilters {
namespace FbGraphApiSigner {

const std::string ACCESS_TOKEN_QUERY_PARAM = "access_token";
const std::string APPSECRET_PROOF_QUERY_PARAM = "appsecret_proof";
const std::string AUTHORIZATION_BEARER_PREFIX = "Bearer ";
const std::string SIGNER_STAT_PREFIX = "fb_graph_api_signer.";

FilterConfig::FilterConfig(const std::string& app_secret,
                           const std::string& stats_prefix,
                           Stats::Scope& scope) :
    app_secret_(app_secret),
    stats_({
        FB_GRAPH_API_SIGNER_FILTER_STATS(POOL_COUNTER_PREFIX(scope, stats_prefix + SIGNER_STAT_PREFIX))
    })
{ }

Filter::Filter(const std::shared_ptr<FilterConfig>& config) : config_(config) { }

const std::string& FilterConfig::app_secret() {
    return app_secret_;
}

FilterStats& FilterConfig::stats() {
    return stats_;
}

Http::FilterHeadersStatus Filter::decodeHeaders(Http::RequestHeaderMap& headers, __attribute__((unused)) bool end_stream) {
    ENVOY_LOG(debug, "fb graph api signer, decode headers");

    if (headers.Path() == nullptr) {
        ENVOY_LOG(debug, "no HTTP URL path found. skip signing.");
        return Http::FilterHeadersStatus::Continue;
    }

    auto params = Http::Utility::parseAndDecodeQueryString(headers.getPathValue());
    if (params.count(APPSECRET_PROOF_QUERY_PARAM) > 0) {
        ENVOY_LOG(debug, "existing appsecret_proof found. skip signing.");
        config_->stats().existing_appsecret_proof_.inc();
        return Http::FilterHeadersStatus::Continue;
    }

    auto access_token = Filter::extractAccessToken(headers, params);
    if (access_token.has_value()) {
        ENVOY_LOG(debug, "found access token: {}", *access_token);

        auto& hashing_util = Envoy::Common::Crypto::UtilitySingleton::get();
        const std::vector<uint8_t> signing_key(config_->app_secret().begin(), config_->app_secret().end());
        const std::string appsecret_proof = Hex::encode(hashing_util.getSha256Hmac(signing_key, *access_token));
        ENVOY_LOG(debug, "computed appsecret_proof: {}", appsecret_proof);

        params[APPSECRET_PROOF_QUERY_PARAM] = appsecret_proof;
        auto stripped_path = Http::Utility::stripQueryString(headers.Path()->value());
        auto new_path = stripped_path + Http::Utility::queryParamsToString(params);
        headers.setPath(new_path);
        config_->stats().signing_added_.inc();
    }

    ENVOY_LOG(debug, "no access token found. skip signing");
    return Http::FilterHeadersStatus::Continue;
}

absl::optional<absl::string_view> Filter::extractAccessToken(const Http::RequestHeaderMap& headers,
                                                             const Http::Utility::QueryParams& queryParams) {
    // First, look for an access token in query params
    const auto& access_token_it = queryParams.find(ACCESS_TOKEN_QUERY_PARAM);
    if (access_token_it != queryParams.end()) {
        config_->stats().query_param_access_token_.inc();
        return absl::make_optional(access_token_it->second);
    }

    // Then, look in the Authorization header.
    const auto authorization_header = headers.get(Http::CustomHeaders::get().Authorization);
    if (!authorization_header.empty()) {
        const auto header_value = authorization_header[0]->value().getStringView();
        const auto token_pos = header_value.find(AUTHORIZATION_BEARER_PREFIX);
        if (token_pos == absl::string_view::npos) {
            ENVOY_LOG(debug, "authorization header found, but no bearer prefix");
            config_->stats().missing_bearer_prefix_.inc();
            return absl::nullopt;
        } else {
            config_->stats().auth_header_access_token_.inc();
            return absl::make_optional(header_value.substr(token_pos + AUTHORIZATION_BEARER_PREFIX.size()));
        }
    }

    // Didn't find an access token anywhere.
    return absl::nullopt;
}

} // namespace FbGraphApiSigner
} // namespace HttpFilters
} // namespace Extensions
} // namespace Envoy


