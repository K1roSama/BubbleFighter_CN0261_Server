#include "game/damage_attributes.h"

#include <stdexcept>
#include <utility>

namespace bubblefighter::game {
namespace {
constexpr std::uint32_t kClockMask = 0xFFFFFF;
constexpr std::uint32_t kClockHalfRange = 0x800000;
// C11 kinds to the state bits built by 0x5928C0. Back direction (bit 3) is a
// separate calculation.
constexpr std::array<std::uint16_t, 12> kDamageFlags{
    1, 2, 4, 0x10, 0x20, 0x40, 0x80, 0x100, 0x200, 0x400, 0x800, 0x1000};
}  // namespace

DamageAttributes::DamageAttributes(NowHandler now) : now_(std::move(now)) {
  if (!now_) {
    throw std::invalid_argument("Damage attribute clock is required");
  }
}

void DamageAttributes::Bind(std::uint32_t character_object) {
  if (character_object < 1000000 || character_object >= 1800000 ||
      (character_object_ && *character_object_ != character_object)) {
    throw std::invalid_argument(
        "Damage attribute reports do not match the registered character");
  }

  character_object_ = character_object;
  bound_ = true;
}

bool DamageAttributes::Apply(const protocol::DamageAttributeReport& report) {
  if (report.character_object < 1000000 || report.character_object >= 1800000 ||
      report.kind >= entries_.size() ||
      (protocol::IsDamageAttributeSwitch(report.kind) && report.value > 1) ||
      (character_object_ && *character_object_ != report.character_object)) {
    throw std::invalid_argument(
        "Damage attribute report is invalid or belongs to another character");
  }

  const auto tick = report.tick & kClockMask;
  auto& entry = entries_[report.kind];
  if (entry && entry->has_tick) {
    const auto elapsed = (tick - entry->tick) & kClockMask;
    // Same-tick transitions keep TCP order. Exact duplicates cannot refresh an
    // expired timer.
    if (elapsed >= kClockHalfRange ||
        (elapsed == 0 && entry->value == report.value)) {
      return false;
    }
  }

  character_object_ = report.character_object;
  entry = Entry{tick, report.value,
                now_() + std::chrono::milliseconds(report.value), true, true};
  return true;
}

void DamageAttributes::BeginPlay() {
  for (auto& entry : entries_) {
    if (entry) {
      // S704 restarts GameContext +1744. Keep remaining durations, but start a
      // new clock domain.
      entry->has_tick = false;
    }
  }
}

void DamageAttributes::ClearForRespawn() {
  for (auto& entry : entries_) {
    if (entry) {
      // Keep the last timestamp and value so a delayed duplicate cannot restore
      // the old state.
      entry->active = false;
    }
  }
}

std::uint16_t DamageAttributes::GetFlags() const {
  if (!bound_) {
    throw std::logic_error(
        "Damage attributes require the confirmed character identity");
  }

  std::uint16_t flags = 0;
  const auto now = now_();
  for (std::size_t kind = 0; kind < entries_.size(); ++kind) {
    const auto& entry = entries_[kind];
    if (!entry || !entry->active || entry->value == 0) {
      continue;
    }
    if (protocol::IsDamageAttributeSwitch(static_cast<std::uint8_t>(kind)) ||
        now < entry->expires_at) {
      flags |= kDamageFlags[kind];
    }
  }
  return flags;
}
}  // namespace bubblefighter::game
