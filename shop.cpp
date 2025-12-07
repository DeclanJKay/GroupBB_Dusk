#include "shop.hpp"
#include "TurretStats.hpp"
#include "run_context.hpp"
#include "player.hpp"

#include <random>
#include <algorithm>
#include <iostream>

Shop::Shop() = default;

// Forward declaration so we can use it in regenerateItems
static void wrapText(sf::Text& text, float maxWidth);


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
    case TurretType::Slow:       return "Slow";
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

    // Box for the items
    const float boxWidth = 130.f;
    const float boxHeight = 100.f;    
    const float boxSpacing = 150.f;  

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
        item.box.setSize({ boxWidth, boxHeight });
        item.box.setFillColor(sf::Color(50, 50, 100));
        item.box.setOutlineThickness(2.f);
        item.box.setOutlineColor(sf::Color::White);
        item.box.setPosition(_basePos.x + i * boxSpacing, _basePos.y);

        // Name text
        item.nameText.setFont(*_font);
        item.nameText.setCharacterSize(20); 
        item.nameText.setFillColor(sf::Color::White);
        item.nameText.setString(turretName(t));
        item.nameText.setPosition(
            item.box.getPosition().x + 8.f,
            item.box.getPosition().y + 6.f
        );

        // Description text (wrapped to fit in box)
        item.descText.setFont(*_font);
        item.descText.setCharacterSize(13);
        item.descText.setFillColor(sf::Color(200, 200, 200));
        item.descText.setString(turretDescription(t));
        item.descText.setPosition(
            item.box.getPosition().x + 8.f,
            item.box.getPosition().y + 30.f
        );
        wrapText(item.descText, boxWidth - 16.f); // leave 8px padding each side

        // Cost text near the bottom of the box
        item.costText.setFont(*_font);
        item.costText.setCharacterSize(15);
        item.costText.setFillColor(sf::Color::Yellow);
        item.costText.setString("Cost: " + std::to_string(stats.cost));
        item.costText.setPosition(
            item.box.getPosition().x + 8.f,
            item.box.getPosition().y + boxHeight - 22.f
        );

        _items.push_back(item);
    }

    // --- Health item (extra slot on the right) ---
    const int healAmount = 2;
    const int healCost = 10;

    Item healItem;
    healItem.isHeal = true;
    healItem.type = TurretType::Basic; // unused when isHeal == true
    healItem.healAmount = healAmount;
    healItem.cost = healCost;

    healItem.box.setSize({ boxWidth, boxHeight });
    healItem.box.setFillColor(sf::Color(50, 100, 50));
    healItem.box.setOutlineThickness(2.f);
    healItem.box.setOutlineColor(sf::Color::White);

    healItem.box.setPosition(
        _basePos.x + static_cast<float>(_items.size()) * boxSpacing,
        _basePos.y
    );

    healItem.nameText.setFont(*_font);
    healItem.nameText.setCharacterSize(20);
    healItem.nameText.setFillColor(sf::Color::White);
    healItem.nameText.setString("Heal +" + std::to_string(healAmount));
    healItem.nameText.setPosition(
        healItem.box.getPosition().x + 8.f,
        healItem.box.getPosition().y + 6.f
    );

    healItem.costText.setFont(*_font);
    healItem.costText.setCharacterSize(15);
    healItem.costText.setFillColor(sf::Color::Yellow);
    healItem.costText.setString("Cost: " + std::to_string(healCost));
    healItem.costText.setPosition(
        healItem.box.getPosition().x + 8.f,
        healItem.box.getPosition().y + boxHeight - 22.f
    );

    _items.push_back(healItem);
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

        // --------------------------------------------------------------------
        // Decide how we pay:
        //   - If we have freeShopItemsPending, consume 1 and pay 0 gold.
        //   - Otherwise, pay gold (turret cost scaled by turretCostMult).
        // --------------------------------------------------------------------
        bool usedFreeCharge = false;
        int  effectiveCost = slot.cost; // default: base cost

        if (ctx.freeShopItemsPending > 0) {
            // Use a free shop charge instead of gold
            ctx.freeShopItemsPending -= 1;
            usedFreeCharge = true;

            std::cout << "Used free shop charge. Remaining: "
                << ctx.freeShopItemsPending << "\n";
        }
        else {
            // No free charges: must pay gold.

            // For turret items, apply turretCostMult discount.
            if (!slot.isHeal) {
                float scaled = static_cast<float>(slot.cost) * ctx.turretCostMult;
                effectiveCost = static_cast<int>(scaled + 0.5f); // round to nearest

                // Never let a positive-cost turret become free
                if (effectiveCost < 1 && slot.cost > 0) {
                    effectiveCost = 1;
                }
            }

            // Check money
            if (ctx.currency < effectiveCost) {
                if (slot.isHeal) {
                    std::cout << "Not enough money for Heal (cost "
                        << effectiveCost << ", have " << ctx.currency << ")\n";
                }
                else {
                    std::cout << "Not enough money for " << turretName(slot.type)
                        << " (cost " << effectiveCost << ", have "
                        << ctx.currency << ")\n";
                }
                return false;
            }

            // Spend money
            ctx.currency -= effectiveCost;
        }

        // --------------------------------------------------------------------
        // Apply the purchase effect
        // --------------------------------------------------------------------
        if (slot.isHeal) {
            // Heal the player
            player.heal(slot.healAmount);
            slot.active = false;

            if (usedFreeCharge) {
                std::cout << "Used free shop charge for Heal (+"
                    << slot.healAmount << " HP).\n";
            }
            else {
                std::cout << "Bought heal (+" << slot.healAmount
                    << " HP). Remaining money: " << ctx.currency << "\n";
            }
        }
        else {
            // Turret purchase: add to inventory
            ctx.turretInventory.push_back(slot.type);
            slot.active = false;

            if (usedFreeCharge) {
                std::cout << "Used free shop charge to get "
                    << turretName(slot.type)
                    << ". Remaining free charges: "
                    << ctx.freeShopItemsPending << "\n";
            }
            else {
                std::cout << "Bought " << turretName(slot.type)
                    << " for " << effectiveCost
                    << ". Remaining money: " << ctx.currency << "\n";
            }
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

std::string Shop::turretDescription(TurretType t)
{
    switch (t) {
    case TurretType::Basic:       return "Cheap all-round turret.";
    case TurretType::SMG:         return "Fast firing, short range.";
    case TurretType::Sniper:      return "Slow but high damage, long range.";
    case TurretType::Bomb:        return "Fires shells that explode in an area.";
    case TurretType::Fire:        return "Applies burning damage over time.";
    case TurretType::Lightening:  return "Stuns nearby enemies.";
    case TurretType::Freeze:      return "Greatly slows enemies.";
    case TurretType::Buff:        return "Doesn't shoot; buffs nearby turrets.";
    case TurretType::Scatter:     return "Shotgun cone of pellets.";
    case TurretType::BananaFarm:  return "Generates extra money during waves.";
    case TurretType::AOE:         return "Periodically blasts all nearby enemies.";
    case TurretType::Slow:        return "Large slow exploding shells.";
    default:                      return "Turret.";
    }
}

// Word-wrap for  text.
// Simple word-wrap helper: wraps text to maxWidth in pixels
static void wrapText(sf::Text& text, float maxWidth)
{
    const std::string original = text.getString();
    std::string wrapped;
    std::string line;
    std::string word;

    auto appendLine = [&](const std::string& l)
        {
            if (!wrapped.empty())
                wrapped += '\n';
            wrapped += l;
        };

    auto commitWord = [&]()
        {
            if (word.empty())
                return;

            // Try adding this word to the current line
            std::string candidate = line;
            if (!candidate.empty())
                candidate += ' ';
            candidate += word;

            // Measure candidate width
            text.setString(candidate);
            if (text.getLocalBounds().width <= maxWidth || line.empty())
            {
                // Fits on this line (or this is the first word on the line)
                line = candidate;
            }
            else
            {
                // Current line is full: push it and start a new line with this word
                appendLine(line);
                line = word;
            }

            word.clear();
        };

    for (char c : original)
    {
        if (c == ' ')
        {
            commitWord();
        }
        else if (c == '\n')
        {
            commitWord();
            if (!line.empty())
            {
                appendLine(line);
                line.clear();
            }
        }
        else
        {
            word += c;
        }
    }

    // Last word + last line
    commitWord();
    if (!line.empty())
        appendLine(line);

    text.setString(wrapped);
}


void Shop::refreshDisplayCosts(const RunContext& ctx)
{
    if (!_font) return;

    for (auto& slot : _items) {
        if (!slot.active) continue;

        // If we have at least one free charge, every active slot is FREE
        if (ctx.freeShopItemsPending > 0) {
            slot.costText.setFillColor(sf::Color::Green);
            slot.costText.setString("Cost: FREE");
            continue;
        }

        int effectiveCost = slot.cost;

        // Turrets get discounted by turretCostMult
        if (!slot.isHeal) {
            float scaled = static_cast<float>(slot.cost) * ctx.turretCostMult;
            effectiveCost = static_cast<int>(scaled + 0.5f);

            if (effectiveCost < 1 && slot.cost > 0) {
                effectiveCost = 1;
            }
        }

        slot.costText.setFillColor(sf::Color::Yellow);
        slot.costText.setString("Cost: " + std::to_string(effectiveCost));
    }
}

void Shop::render(sf::RenderWindow& window) const
{
    for (const auto& item : _items) {
        if (!item.active) continue;

        window.draw(item.box);
        window.draw(item.nameText);
        window.draw(item.descText);
        window.draw(item.costText);
    }
}

