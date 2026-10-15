#include "features/authentication/password_credentials.h"

#include <openssl/crypto.h>
#include <openssl/evp.h>
#include <openssl/rand.h>

#include <algorithm>
#include <array>
#include <span>
#include <stdexcept>
#include <vector>

namespace bubblefighter::authentication {
namespace {
std::string Hex(std::span<const unsigned char> bytes) {
  constexpr std::string_view digits = "0123456789abcdef";
  std::string result;
  result.reserve(bytes.size() * 2);
  for (const auto byte : bytes) {
    result.push_back(digits[byte >> 4]);
    result.push_back(digits[byte & 15]);
  }
  return result;
}

std::vector<unsigned char> DecodeHex(std::string_view value) {
  constexpr std::string_view digits = "0123456789abcdef";
  std::vector<unsigned char> result;
  if (value.size() % 2 != 0) {
    throw std::invalid_argument("Invalid credential encoding");
  }
  for (std::size_t index = 0; index < value.size(); index += 2) {
    const auto high = digits.find(value[index]);
    const auto low = digits.find(value[index + 1]);
    if (high == std::string_view::npos || low == std::string_view::npos) {
      throw std::invalid_argument("Invalid credential encoding");
    }
    result.push_back(static_cast<unsigned char>((high << 4) | low));
  }
  return result;
}

std::string DerivePassword(std::string_view password, std::string_view salt) {
  const auto salt_bytes = DecodeHex(salt);
  if (salt_bytes.size() != 16) {
    throw std::invalid_argument("Invalid password salt size");
  }
  std::array<unsigned char, 32> result{};
  if (EVP_PBE_scrypt(password.data(), password.size(), salt_bytes.data(),
                     salt_bytes.size(), 32768, 8, 3, 64 * 1024 * 1024,
                     result.data(), result.size()) != 1) {
    throw std::runtime_error("Password derivation failed");
  }
  const auto encoded = Hex(result);
  OPENSSL_cleanse(result.data(), result.size());
  return encoded;
}

std::string RandomHex(int byte_count) {
  std::vector<unsigned char> bytes(static_cast<std::size_t>(byte_count));
  if (RAND_bytes(bytes.data(), byte_count) != 1) {
    throw std::runtime_error("Secure random generation failed");
  }
  return Hex(bytes);
}
}  // namespace

void ValidateAccountName(std::string_view account_name) {
  if (account_name.size() < 3 || account_name.size() > 32 ||
      !std::all_of(account_name.begin(), account_name.end(), [](char value) {
        return (value >= 'a' && value <= 'z') ||
               (value >= '0' && value <= '9') || value == '_';
      })) {
    throw std::invalid_argument(
        "Account name must contain 3 to 32 lowercase letters, digits or underscores");
  }
}

void ValidatePassword(std::string_view password) {
  if (password.size() < 8 || password.size() > 128 ||
      password.find('\0') != std::string_view::npos) {
    throw std::invalid_argument(
        "Password must contain 8 to 128 UTF-8 bytes and no null characters");
  }
}

PasswordCredentials CreatePasswordCredentials(std::string_view password) {
  ValidatePassword(password);
  auto salt = RandomHex(16);
  return {salt, DerivePassword(password, salt)};
}

bool VerifyPassword(std::string_view password,
                    const PasswordCredentials& credentials) {
  ValidatePassword(password);
  if (credentials.digest.size() != 64) {
    throw std::invalid_argument("Invalid password digest size");
  }
  const auto derived = DerivePassword(password, credentials.salt);
  return CRYPTO_memcmp(derived.data(), credentials.digest.data(),
                       derived.size()) == 0;
}

std::string CreateLoginToken() { return RandomHex(32); }

bool IsLoginToken(std::string_view token) {
  return token.size() == 64 &&
         std::all_of(token.begin(), token.end(), [](char value) {
           return (value >= '0' && value <= '9') ||
                  (value >= 'a' && value <= 'f');
         });
}

std::string HashLoginToken(std::string_view token) {
  std::array<unsigned char, 32> digest{};
  unsigned int length = 0;
  if (EVP_Digest(token.data(), token.size(), digest.data(), &length,
                 EVP_sha256(), nullptr) != 1 ||
      length != digest.size()) {
    throw std::runtime_error("Login token digest failed");
  }
  return Hex(digest);
}
}  // namespace bubblefighter::authentication
