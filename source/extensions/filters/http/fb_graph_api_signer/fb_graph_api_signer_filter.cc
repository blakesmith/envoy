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

absl::optional<absl::string_view> Filter::extractAccessToken(const Http::RequestHeaderMap& headers,
                                                             const Http::Utility::QueryParams& queryParams) {

    // First, look for an access token in query params
    const auto& access_token_it = queryParams.find(ACCESS_TOKEN_QUERY_PARAM);
    if (access_token_it != queryParams.end()) {
        return absl::make_optional(access_token_it->second);
    }

    // Then, look in the Authorization header.
    const auto authorization_header = headers.get(Http::CustomHeaders::get().Authorization);
    if (!authorization_header.empty()) {
        const auto header_value = authorization_header[0]->value().getStringView();
        const auto token_pos = header_value.find(AUTHORIZATION_BEARER_PREFIX);
        if (token_pos == absl::string_view::npos) {
            ENVOY_LOG(debug, "Authorization header found, but no bearer prefix");
            return absl::nullopt;
        } else {
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


