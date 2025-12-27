#include <SFML/Graphics.hpp>
#include <optional>

int main() {
    sf::Vector2u windowSize{800u, 600u};
    sf::RenderWindow window{sf::VideoMode(windowSize), "Test"};
    window.setVerticalSyncEnabled(true);

    sf::Clock clock;

    float radius{ 20.f };
    sf::CircleShape circle{ radius };
    circle.setFillColor(sf::Color::Magenta);
    circle.setOrigin({radius, radius});
    circle.setPosition({window.getSize().x / 2.f, window.getSize().y / 2.f});

    sf::Vector2f velocity{600.f, 600.f};

    while(window.isOpen()) {
        float dt = clock.restart().asSeconds();

        while(const auto event = window.pollEvent()) {
            if(event->is<sf::Event::Closed>()) {
                window.close();
            }
        }

        sf::Vector2f currentPos{ circle.getPosition() };

        // Create resolveEdge again here
        auto resolveEdge{
            [](float pos, float radius, float limit) -> std::optional<float> {
                if(pos - radius < 0) { return radius; }
                if(pos + radius > limit) { return limit - radius; }
                return std::nullopt;
            }
        };

        if(auto x = resolveEdge(currentPos.x, radius, windowSize.x)) {
            currentPos.x = *x;
            velocity.x *= -1.f;
        }
        if(auto y = resolveEdge(currentPos.y, radius, windowSize.y)) {
            currentPos.y = *y;
            velocity.y *= -1.f;
        }

        currentPos += velocity * dt;

        circle.setPosition(currentPos);

        window.clear();
        window.draw(circle);
        window.display();
    }

    return 0;
}