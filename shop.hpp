#pragma once

#include <vector>
#include <string>
#include <SFML/Graphics.hpp>
#include "TurretType.hpp"

struct RunContext; // forward declaration – defined in run_context.hpp
class Player;      // forward declaration – defined in player.hpp

class Shop {
public:
    Shop();

    // Call once when the scene loads.
    // font: UI font (must outlive the shop)
    // basePos: top-left where the first shop box should be drawn.
    void init(const sf::Font& font, const sf::Vector2f& basePos);

    // Regenerate a new set of random items (3 turrets + 1 heal).
    void regenerateItems();

    // Draw the shop UI (boxes + labels).
    void render(sf::RenderWindow& window) const;

    // Try to buy the item the player is standing on.
    // playerPos: world position of player
    // ctx: shared run context (money + inventory)
    // player: the player (for healing)
    // Returns true if a purchase was made.
    bool tryPurchaseAt(const sf::Vector2f& playerPos, RunContext& ctx, Player& player);

    // Used by SafehouseScene to show "Press E to buy"
    bool hasItemNear(const sf::Vector2f& playerPos) const;

    // Helper for displaying turret names in other places (inventory, etc.)
    static std::string turretName(TurretType type);

private:
    struct Item {
        bool           isHeal = false;              // true = heal box
        int            healAmount = 0;                  // used when isHeal == true
        TurretType     type = TurretType::Basic;  // safe default
        int            cost = 0;
        sf::RectangleShape box;
        sf::Text       nameText;
        sf::Text       costText;
        bool           active = true;               // false once bought
    };

    const sf::Font* _font = nullptr;
    sf::Vector2f      _basePos{ 0.f, 0.f };
    std::vector<Item> _items;
};
