#ifndef BUBBLEFIGHTER_SERVER_FEATURES_AUTHENTICATION_PASSWORD_CREDENTIALS_H_
#define BUBBLEFIGHTER_SERVER_FEATURES_AUTHENTICATION_PASSWORD_CREDENTIALS_H_

#include <string>
#include <string_view>

namespace bubblefighter::authentication {
struct PasswordCredentials {
  std::string salt;
  std::string digest;
};

inline constexpr std::string_view kPasswordScheme = "scrypt_32768_8_3";

void ValidateAccountName(std::string_view account_name);
void ValidatePassword(std::string_view password);
PasswordCredentials CreatePasswordCredentials(std::string_view password);
bool VerifyPassword(std::string_view password,
                    const PasswordCredentials& credentials);
std::string CreateLoginToken();
bool IsLoginToken(std::string_view token);
std::string HashLoginToken(std::string_view token);
}  // namespace bubblefighter::authentication

#endif  // BUBBLEFIGHTER_SERVER_FEATURES_AUTHENTICATION_PASSWORD_CREDENTIALS_H_
