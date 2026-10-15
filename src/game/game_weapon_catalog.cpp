#include "game/game_weapon_catalog.h"

#include <algorithm>
#include <cmath>
#include <fstream>
#include <iterator>
#include <nlohmann/json.hpp>
#include <stdexcept>

namespace bubblefighter::game {
namespace {
using Json = nlohmann::json;

void CheckKeys(const Json& object,
               std::initializer_list<std::string_view> allowed) {
  if (!object.is_object() || object.size() != allowed.size()) {
    throw std::invalid_argument(
        "Game weapon catalog has missing or unexpected fields");
  }
  for (const auto& entry : object.items()) {
    if (std::find(allowed.begin(), allowed.end(), entry.key()) ==
        allowed.end()) {
      throw std::invalid_argument("Game weapon catalog has an unknown field");
    }
  }
}

std::uint32_t Number(const Json& object, const char* key,
                     std::uint32_t maximum) {
  const auto& value = object.at(key);
  if (!value.is_number_integer() ||
      (!value.is_number_unsigned() && value.get<std::int64_t>() < 0) ||
      value.get<std::uint64_t>() > maximum) {
    throw std::invalid_argument(std::string("Game weapon ") + key +
                                " must be an integer in the supported range");
  }
  return value.get<std::uint32_t>();
}

float DamageNumber(const Json& object, const char* key, float maximum) {
  const auto& value = object.at(key);
  if (!value.is_number() || !std::isfinite(value.get<double>()) ||
      value.get<double>() < 0.0 || value.get<double>() > maximum) {
    throw std::invalid_argument(std::string("Game weapon damage ") + key +
                                " is outside the supported range");
  }

  return value.get<float>();
}

WeaponDamageProfile ReadDamage(const Json& value) {
  CheckKeys(value, {"baseDamage", "criticalMultiplier", "backMultiplier",
                    "unscopedMultiplier", "wallAttackMultiplier",
                    "wallDefenseMultiplier", "usesScope", "canDamageStunned"});
  WeaponDamageProfile damage;
  damage.base_damage = DamageNumber(value, "baseDamage", 1000000.0f);
  damage.critical_multiplier =
      DamageNumber(value, "criticalMultiplier", 1000.0f);
  damage.back_multiplier = DamageNumber(value, "backMultiplier", 1000.0f);
  damage.unscoped_multiplier =
      DamageNumber(value, "unscopedMultiplier", 1000.0f);
  damage.wall_attack_multiplier =
      DamageNumber(value, "wallAttackMultiplier", 1000.0f);
  damage.wall_defense_multiplier =
      DamageNumber(value, "wallDefenseMultiplier", 1000.0f);
  if (!value.at("usesScope").is_boolean() ||
      !value.at("canDamageStunned").is_boolean() ||
      damage.base_damage == 0.0f) {
    throw std::invalid_argument(
        "Game weapon requires positive base damage and boolean damage flags");
  }

  damage.uses_scope = value.at("usesScope").get<bool>();
  damage.can_damage_stunned = value.at("canDamageStunned").get<bool>();
  return damage;
}
}  // namespace

GameWeaponCatalog GameWeaponCatalog::Load(const std::filesystem::path& path) {
  std::ifstream input(path, std::ios::binary);
  if (!input) {
    throw std::runtime_error("Cannot open gameWeaponsFile");
  }
  const std::string content{std::istreambuf_iterator<char>(input),
                            std::istreambuf_iterator<char>()};
  return Parse(content);
}

GameWeaponCatalog GameWeaponCatalog::Parse(std::string_view text) {
  try {
    const auto root = Json::parse(text);
    CheckKeys(root, {"weapons"});
    const auto& weapons = root.at("weapons");
    if (!weapons.is_array() || weapons.empty() || weapons.size() > 4096) {
      throw std::invalid_argument(
          "Game weapon catalog must contain between 1 and 4096 weapons");
    }

    GameWeaponCatalog catalog;
    for (const auto& entry : weapons) {
      CheckKeys(entry, {"definitionId", "weaponSlot", "weaponType",
                        "projectileCount", "damage"});
      GameWeapon weapon;
      weapon.definition_id = Number(entry, "definitionId", 65535);
      weapon.weapon_slot =
          static_cast<std::uint8_t>(Number(entry, "weaponSlot", 3));
      weapon.projectile_count =
          static_cast<std::uint8_t>(Number(entry, "projectileCount", 64));
      weapon.damage = ReadDamage(entry.at("damage"));
      const auto& type = entry.at("weaponType");
      if (!type.is_string() || weapon.definition_id == 0 ||
          !((type == "Gun" && weapon.weapon_slot < 2 &&
             weapon.projectile_count > 0) ||
            (type == "Near" && weapon.weapon_slot == 2 &&
             weapon.projectile_count == 0) ||
            (type == "Grenade" && weapon.definition_id == 5003 &&
             weapon.weapon_slot == 3 && weapon.projectile_count == 1))) {
        throw std::invalid_argument(
            "Game weapon type, slot and projectile count are inconsistent");
      }
      weapon.grenade = type == "Grenade";
      if (!catalog.weapons_.emplace(weapon.definition_id, weapon).second) {
        throw std::invalid_argument("Game weapon definitionId is duplicated");
      }
    }
    for (const auto& [definition, slot] :
         {std::pair{2004u, 0u}, {1004u, 1u}, {7001u, 2u}}) {
      const auto* weapon = catalog.Find(definition);
      if (!weapon || weapon->weapon_slot != slot) {
        throw std::invalid_argument(
            "Game weapon catalog is missing an empty-slot default");
      }
    }
    return catalog;
  } catch (const Json::exception&) {
    throw std::invalid_argument(
        "Game weapon catalog is not valid JSON with the required fields");
  }
}

const GameWeapon* GameWeaponCatalog::Find(std::uint32_t definition_id) const {
  const auto found = weapons_.find(definition_id);
  return found == weapons_.end() ? nullptr : &found->second;
}
}  // namespace bubblefighter::game
