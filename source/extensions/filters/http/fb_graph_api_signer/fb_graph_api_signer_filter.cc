#include "source/extensions/filters/http/fb_graph_api_signer/fb_graph_api_signer_filter.h"

#include "source/common/common/hex.h"
#include "source/common/crypto/utility.h"
#include "source/common/http/utility.h"

namespace Envoy {
namespace Extensions {
namespace HttpFilters {
namespace FbGraphApiSigner {

const std::string ACCESS_TOKEN_QUERY_PARAM = "access_token";
const std::string APPSECRET_PROOF_QUERY_PARAM = "appsecret_proof";

Filter::Filter(const std::shared_ptr<std::string>& app_secret) : app_secret_(app_secret) { }

Http::FilterHeadersStatus Filter::decodeHeaders(Http::RequestHeaderMap& headers, __attribute__((unused)) bool end_stream) {
    ENVOY_LOG(debug, "fb graph api signer, decode headers");

    if (headers.Path() == nullptr) {
        ENVOY_LOG(debug, "no HTTP URL path found. Skip signing.");
        return Http::FilterHeadersStatus::Continue;
    }

    auto params = Http::Utility::parseAndDecodeQueryString(headers.getPathValue());
    auto access_token = Filter::extractAccessToken(headers, params);

    if (!access_token.has_value()) {
        ENVOY_LOG(debug, "no access token found. skip signing");
    } else {
        ENVOY_LOG(debug, "Found access token: {}", *access_token);

        auto& hashing_util = Envoy::Common::Crypto::UtilitySingleton::get();
        const std::vector<uint8_t> signing_key(app_secret_->begin(), app_secret_->end());
        const std::string appsecret_proof = Hex::encode(hashing_util.getSha256Hmac(signing_key, *access_token));
        ENVOY_LOG(debug, "Computed appsecret_proof: {}", appsecret_proof);

        params[APPSECRET_PROOF_QUERY_PARAM] = appsecret_proof;
        auto stripped_path = Http::Utility::stripQueryString(headers.Path()->value());
        auto new_path = stripped_path + Http::Utility::queryParamsToString(params);
        headers.setPath(new_path);
    }

    return Http::FilterHeadersStatus::Continue;
}

absl::optional<std::string> Filter::extractAccessToken(__attribute__ ((unused)) const Http::RequestHeaderMap& headers,
                                                       const Http::Utility::QueryParams& queryParams) {
    const auto& access_token_it = queryParams.find(ACCESS_TOKEN_QUERY_PARAM);

    // First, look for an access token in query params
    if (access_token_it != queryParams.end()) {
        return absl::make_optional(access_token_it->second);
    } else {
        return absl::nullopt;
    }
}

} // namespace FbGraphApiSigner
} // namespace HttpFilters
} // namespace Extensions
} // namespace Envoy


