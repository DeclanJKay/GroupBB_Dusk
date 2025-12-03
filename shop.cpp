#include "shop.hpp"
#include "TurretStats.hpp"
#include "run_context.hpp"

#include <random>
#include <algorithm> // std::find

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

    // Shuffle and take first 3
    std::random_device rd;
    std::mt19937 rng(rd());
    std::shuffle(all.begin(), all.end(), rng);

    const float spacing = 150.f;

    for (int i = 0; i < 3 && i < static_cast<int>(all.size()); ++i) {
        TurretType t = all[i];
        TurretStats stats = get_turret_stats(t);

        Item item;
        item.type = t;
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
}

void Shop::render(sf::RenderWindow& window) const {
    for (const auto& item : _items) {
        window.draw(item.box);
        window.draw(item.nameText);
        window.draw(item.costText);
    }
}

bool Shop::tryPurchaseAt(const sf::Vector2f& playerPos, RunContext& ctx) {
    for (const auto& item : _items) {
        if (item.box.getGlobalBounds().contains(playerPos)) {

            // Already own this turret type? Treat as an unlock, not stackable.
            auto& inv = ctx.turretInventory;
            bool alreadyOwned =
                std::find(inv.begin(), inv.end(), item.type) != inv.end();

            if (alreadyOwned) {
                // Could print a message if you want
                return false;
            }

            if (ctx.currency < item.cost) {
                // Not enough money
                return false;
            }

            ctx.currency -= item.cost;
            inv.push_back(item.type);
            return true;
        }
    }
    return false;
}
