#include <array>
#include <chrono>
#include <cstdint>
#include <iostream>
#include <span>
#include <stdexcept>
#include <vector>

#include "features/authentication/password_credentials.h"
#include "game/session_ping_tracker.h"
#include "protocol/lambda_packet_codec.h"
#include "protocol/nx_packet_codec.h"
#include "protocol/packet_buffer.h"

namespace {

void Require(bool condition, const char* message) {
  if (!condition) {
    throw std::runtime_error(message);
  }
}

template <typename Operation>
void RequireInvalid(Operation operation) {
  try {
    operation();
  } catch (const std::invalid_argument&) {
    return;
  }
  throw std::runtime_error("Expected invalid input to be rejected");
}

void CheckPacketFields() {
  bubblefighter::protocol::PacketWriter writer;
  writer.WriteByte(0xab);
  writer.WriteShort(0x1234);
  writer.WriteUnsigned(0x89abcdef);
  writer.WriteString(u"A中");
  const std::vector<std::uint8_t> expected = {0xab, 0x34, 0x12, 0xef, 0xcd,
                                              0xab, 0x89, 2,    0,    0,
                                              0,    0x41, 0,    0x2d, 0x4e};
  Require(writer.GetBytes() == expected,
          "Little endian / UTF-16 wire fixture changed");
  bubblefighter::protocol::PacketReader reader(expected);
  Require(reader.ReadByte() == 0xab && reader.ReadShort() == 0x1234 &&
              reader.ReadUnsigned() == 0x89abcdef &&
              reader.ReadString() == u"A中",
          "Fixed packet could not be decoded");
  reader.RequireEnd();
  RequireInvalid([&reader] { static_cast<void>(reader.ReadByte()); });
  const std::array<std::uint8_t, 4> invalid_boolean{2, 0, 0, 0};
  bubblefighter::protocol::PacketReader boolean_reader(invalid_boolean);
  RequireInvalid(
      [&boolean_reader] { static_cast<void>(boolean_reader.ReadBoolean()); });
}

void CheckLambdaWire() {
  using bubblefighter::protocol::LambdaPacketCodec;
  using bubblefighter::protocol::PacketEndpoint;
  auto greeting = LambdaPacketCodec::CreateGreeting();
  std::vector<std::uint8_t> expected_greeting(258, 0);
  expected_greeting[0] = 2;
  Require(greeting == expected_greeting, "CN0261 fixed greeting changed");
  LambdaPacketCodec server(PacketEndpoint::kServer);
  const std::vector<std::uint8_t> first = {0x1b, 0x6b, 0xbb, 0xe2,
                                           0x2e, 0x9e, 0x6e, 0x0a};
  const std::vector<std::uint8_t> second = {0x79, 0xfa, 0x6d, 0xe8, 0x8d};
  Require(server.Encode({0x1234, {0x10, 0x20, 0x30}}) == first,
          "Lambda wire fixture changed");
  Require(server.Encode({1, {}}) == second,
          "Lambda cipher did not continue between packets");
  LambdaPacketCodec client(PacketEndpoint::kClient);
  Require(client.Feed(std::span(first).first(3)).empty(),
          "Partial frame was delivered early");
  auto packets = client.Feed(std::span(first).subspan(3));
  Require(
      packets.size() == 1 && packets[0].message_id == 0x1234 &&
          packets[0].payload == std::vector<std::uint8_t>({0x10, 0x20, 0x30}),
      "Fragmented Lambda wire fixture failed");
  auto corrupted = first;
  corrupted.back() ^= 1;
  LambdaPacketCodec invalid_client(PacketEndpoint::kClient);
  RequireInvalid([&invalid_client, &corrupted] {
    static_cast<void>(invalid_client.Feed(corrupted));
  });
}

void CheckNxWire() {
  bubblefighter::protocol::NxPacketCodec codec(false);
  const std::vector<std::uint8_t> expected = {3,    0,    0x34, 0x12,
                                              0x10, 0x20, 0x30};
  Require(codec.Encode({0x1234, {0x10, 0x20, 0x30}}) == expected,
          "Nx wire fixture changed");
  Require(codec.Feed(std::span(expected).first(2)).empty(),
          "Partial Nx header was delivered");
  const auto packets = codec.Feed(std::span(expected).subspan(2));
  Require(packets.size() == 1 && packets[0].message_id == 0x1234 &&
              packets[0].payload.size() == 3,
          "Nx fixed packet could not be decoded");
  const std::array<std::uint8_t, 4> overflowing_length{0xff, 0xff, 1, 0};
  RequireInvalid([&codec, &overflowing_length] {
    static_cast<void>(codec.Feed(overflowing_length));
  });
}

void CheckPingAndCredentials() {
  bubblefighter::game::SessionPingTracker tracker;
  const std::array<std::uint64_t, 2> members{10, 20};
  const auto now = bubblefighter::game::SessionPingTracker::Clock::time_point{};
  Require(tracker.Begin(10, 0, members, now), "Valid ping request rejected");
  Require(!tracker.Begin(10, 0, members, now), "Ping rate limit changed");
  Require(tracker.Complete(20, 1, 0, members, now) == 10,
          "Ping response routing changed");
  Require(!tracker.Complete(20, 1, 0, members, now),
          "Duplicate ping response accepted");
  Require(tracker.Begin(10, 0, members, now + std::chrono::seconds(1)),
          "Ping retry rejected");
  Require(!tracker.Complete(20, 1, 0, members, now + std::chrono::seconds(7)),
          "Expired ping accepted");
  using bubblefighter::authentication::ValidateAccountName;
  using bubblefighter::authentication::ValidatePassword;
  ValidateAccountName("player_01");
  ValidatePassword("password123");
  RequireInvalid([] { ValidateAccountName("Player"); });
  RequireInvalid([] { ValidatePassword("short"); });
}

}  // namespace

int main() {
  try {
    CheckPacketFields();
    CheckLambdaWire();
    CheckNxWire();
    CheckPingAndCredentials();
    std::cout
        << "CN0261 fixed packet, framing, ping and credential regressions passed.\n";
    return 0;
  } catch (const std::exception& error) {
    std::cerr << error.what() << '\n';
    return 1;
  }
}
