#include <SFML/Graphics.hpp>
#include <iostream>

class Samurai : public sf::Texture {

};

int main() {
    sf::RenderWindow window{sf::VideoMode({800u, 600u}), "Test"};
    sf::Clock clock;
    sf::Texture samuraiSpriteSheet;

    if (!samuraiSpriteSheet.loadFromFile("assets/Samurai_Spritelist.psd")) {
        std::cerr << "Error: Could not load samurai from assets/Samurai_Spritelist.psd\n";
        return 1;
    }
    
    sf::Sprite samurai{ samuraiSpriteSheet };
    samurai.setTextureRect(sf::IntRect({0, 0},
                                       {static_cast<int>(1536 / 10), static_cast<int>(1280 / 12)}));
    samurai.setOrigin({samurai.getLocalBounds().size.x / 2, samurai.getLocalBounds().size.y / 2});

    while(window.isOpen()) {
        float dt{ clock.restart().asSeconds() };

        while(const auto event = window.pollEvent()) {
            if(event->is<sf::Event::Closed>()) {
                window.close();
            }
        }

        sf::Angle newRotation{ samurai.getRotation() };
        if(sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Q)) {
            samurai.rotate(sf::Angle(sf::degrees(-0.1)));
        }
        if(sf::Keyboard::isKeyPressed(sf::Keyboard::Key::E)) {
            samurai.rotate(sf::Angle(sf::degrees(0.1)));
        }
        if(sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Hyphen)) {
            samurai.scale({1.01f, 1.01f});
        }
        if(sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Equal)) {
            samurai.scale({0.99f, 0.99f});
        }
        
        float velocity{ 300.f };
        if(sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Up)) {
            samurai.move({0, -velocity * dt});
        }
        if(sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Down)) {
            samurai.move({0.f, velocity * dt});
        }
        if(sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Left)) {
            samurai.move({-velocity * dt, 0.f});
        }
        if(sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Right)) {
            samurai.move({velocity * dt, 0.f});
        }

        window.clear(sf::Color::Blue);
        window.draw(samurai);
        window.display();
    }

    return 0;
}