#include "shop.hpp"
#include "TurretStats.hpp"
#include "run_context.hpp"
#include "player.hpp"

#include <random>
#include <algorithm>
#include <iostream>

Shop::Shop() = default;

// Name helper for turrets
std::string Shop::turretName(TurretType type) {
    switch (type) {
    case TurretType::Basic:      return "Basic";
    case TurretType::SMG:        return "SMG";
    case TurretType::Sniper:     return "Sniper";
    case TurretType::Bomb:       return "Bomb";
    case TurretType::Fire:       return "Fire";
    case TurretType::Lightening: return "Lightning";
    case TurretType::Freeze:     return "Freeze";
    case TurretType::Buff:       return "Buff";
    case TurretType::Scatter:    return "Scatter";
    case TurretType::AOE:        return "AOE";
    case TurretType::BananaFarm: return "Banana Farm";
    default:                     return "Unknown";
    }
}

void Shop::init(const sf::Font& font, const sf::Vector2f& basePos) {
    _font = &font;
    _basePos = basePos;
    regenerateItems();
}

void Shop::regenerateItems() {
    _items.clear();
    if (!_font) return;

    // All turrets that can appear in the shop
    std::vector<TurretType> all = {
        TurretType::Basic,
        TurretType::SMG,
        TurretType::Sniper,
        TurretType::Bomb,
        TurretType::Fire,
        TurretType::Lightening,
        TurretType::Freeze,
        TurretType::Buff,
        TurretType::Scatter,
        TurretType::AOE,
        TurretType::BananaFarm
    };

    std::random_device rd;
    std::mt19937 rng(rd());
    std::shuffle(all.begin(), all.end(), rng);

    const float spacing = 150.f;

    // --- Turret items ---
    for (int i = 0; i < 3 && i < static_cast<int>(all.size()); ++i) {
        TurretType t = all[i];
        TurretStats stats = get_turret_stats(t);

        Item item;
        item.isHeal = false;
        item.type = t;
        item.healAmount = 0;
        item.cost = stats.cost;

        // Box
        item.box.setSize({ 120.f, 120.f });
        item.box.setFillColor(sf::Color(50, 50, 100));
        item.box.setOutlineThickness(2.f);
        item.box.setOutlineColor(sf::Color::White);
        item.box.setPosition(_basePos.x + i * spacing, _basePos.y);

        // Name text
        item.nameText.setFont(*_font);
        item.nameText.setCharacterSize(16);
        item.nameText.setFillColor(sf::Color::White);
        item.nameText.setString(turretName(t));
        item.nameText.setPosition(
            item.box.getPosition().x + 8.f,
            item.box.getPosition().y + 8.f
        );

        // Cost text
        item.costText.setFont(*_font);
        item.costText.setCharacterSize(14);
        item.costText.setFillColor(sf::Color::Yellow);
        item.costText.setString("Cost: " + std::to_string(stats.cost));
        item.costText.setPosition(
            item.box.getPosition().x + 8.f,
            item.box.getPosition().y + 40.f
        );

        _items.push_back(item);
    }

    // --- Health item (extra slot on the right) ---
    const int healAmount = 2;   // how much HP to restore per buy
    const int healCost = 10;  // currency cost

    Item healItem;
    healItem.isHeal = true;
    healItem.type = TurretType::Basic; // unused when isHeal == true
    healItem.healAmount = healAmount;
    healItem.cost = healCost;

    healItem.box.setSize({ 120.f, 120.f });
    healItem.box.setFillColor(sf::Color(50, 100, 50)); // green-ish
    healItem.box.setOutlineThickness(2.f);
    healItem.box.setOutlineColor(sf::Color::White);

    // place after current items
    healItem.box.setPosition(
        _basePos.x + static_cast<float>(_items.size()) * spacing,
        _basePos.y
    );

    healItem.nameText.setFont(*_font);
    healItem.nameText.setCharacterSize(16);
    healItem.nameText.setFillColor(sf::Color::White);
    healItem.nameText.setString("Heal +" + std::to_string(healAmount));
    healItem.nameText.setPosition(
        healItem.box.getPosition().x + 8.f,
        healItem.box.getPosition().y + 8.f
    );

    healItem.costText.setFont(*_font);
    healItem.costText.setCharacterSize(14);
    healItem.costText.setFillColor(sf::Color::Yellow);
    healItem.costText.setString("Cost: " + std::to_string(healCost));
    healItem.costText.setPosition(
        healItem.box.getPosition().x + 8.f,
        healItem.box.getPosition().y + 40.f
    );

    _items.push_back(healItem);
}

void Shop::render(sf::RenderWindow& window) const {
    for (const auto& item : _items) {
        if (!item.active) continue;

        window.draw(item.box);
        window.draw(item.nameText);
        window.draw(item.costText);
    }
}

bool Shop::tryPurchaseAt(const sf::Vector2f& playerPos, RunContext& ctx, Player& player)
{
    const float interactRadius = 48.f;
    const float interactRadiusSq = interactRadius * interactRadius;

    for (auto& slot : _items) {
        if (!slot.active) continue; // skip items already bought

        // Centre of the slot box
        sf::Vector2f boxPos = slot.box.getPosition();
        sf::Vector2f boxSize = slot.box.getSize();
        sf::Vector2f centre = boxPos + 0.5f * boxSize;

        sf::Vector2f d = playerPos - centre;
        float        distSq = d.x * d.x + d.y * d.y;

        if (distSq > interactRadiusSq) {
            continue; // player too far from this slot
        }

        // --- If it's a heal item and player is already full HP, do nothing ---
        if (slot.isHeal) {
            int hp = player.get_health();
            int maxHp = player.get_max_health();
            if (hp >= maxHp) {
                std::cout << "Already at full HP.\n";
                return false;
            }
        }

        // Check money
        if (ctx.currency < slot.cost) {
            if (slot.isHeal) {
                std::cout << "Not enough money for Heal (cost "
                    << slot.cost << ", have " << ctx.currency << ")\n";
            }
            else {
                std::cout << "Not enough money for " << turretName(slot.type)
                    << " (cost " << slot.cost << ", have " << ctx.currency << ")\n";
            }
            return false;
        }

        // Spend money
        ctx.currency -= slot.cost;

        if (slot.isHeal) {
            // Heal the player
            player.heal(slot.healAmount);
            slot.active = false;

            std::cout << "Bought heal (+" << slot.healAmount
                << " HP). Remaining money: " << ctx.currency << "\n";
        }
        else {
            // Turret purchase: add to inventory
            ctx.turretInventory.push_back(slot.type);
            slot.active = false;

            std::cout << "Bought " << turretName(slot.type)
                << " for " << slot.cost
                << ". Remaining money: " << ctx.currency << "\n";
        }

        return true; // purchased one item
    }

    return false; // no slot in range
}

bool Shop::hasItemNear(const sf::Vector2f& playerPos) const
{
    const float interactRadius = 48.f;
    const float interactRadiusSq = interactRadius * interactRadius;

    for (const auto& slot : _items) {
        if (!slot.active) continue; // ignore items that were already bought

        sf::Vector2f boxPos = slot.box.getPosition();
        sf::Vector2f boxSize = slot.box.getSize();
        sf::Vector2f centre = boxPos + 0.5f * boxSize;

        sf::Vector2f d = playerPos - centre;
        float        distSq = d.x * d.x + d.y * d.y;

        if (distSq <= interactRadiusSq) {
            return true; // there is at least one active slot near the player
        }
    }

    return false;
}
