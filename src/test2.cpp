#include <SFML/Graphics.hpp>

#include "Random.h"

#include <algorithm>
#include <cmath>
#include <iostream>
#include <optional>

class Paddle : public sf::RectangleShape {
public:
    Paddle(const sf::Vector2f& paddleSize, const sf::Vector2f& initPos)
        : sf::RectangleShape{ paddleSize }, m_paddleSize{ paddleSize }
    {
        setPosition(initPos);
    }

    sf::Vector2f updatePos(const sf::Keyboard::Scan& upButton,
                           const sf::Keyboard::Scan& downButton,
                           const sf::Vector2u& windowSize, const float dt) {
        sf::Vector2f paddlePos{ getPosition() };
        if(paddlePos.y > 0) {
            if(sf::Keyboard::isKeyPressed(upButton)) {
                paddlePos.y -= 400.f * dt;
            }
        }
        else {
            paddlePos.y = 0;
        }
        if(paddlePos.y + m_paddleSize.y < windowSize.y) {
            if(sf::Keyboard::isKeyPressed(downButton)) {
                paddlePos.y += 400.f * dt;
            }
        }
        else {
            paddlePos.y = windowSize.y - m_paddleSize.y;
        }

        return paddlePos;
    }

    void pointWon() { ++m_score; }
    void setScore(int score) { m_score = score; }
    int getScoreNum() const { return m_score; }

private:
    const sf::Vector2f m_paddleSize;
    int m_score{ 0 };
};

class PongBall : public sf::CircleShape {
public:
    PongBall(float radius, const float speed, const sf::Vector2u& windowSize,
             const sf::Color& color = sf::Color::Magenta)
        : sf::CircleShape{ radius }, m_radius{ radius }, m_speed{ speed },
          m_velocity{ 0.f, 0.f }, m_windowSize{ windowSize }
    {
        setFillColor(color);
        setOrigin({radius, radius});
        setPosition({windowSize.x / 2.f, windowSize.y / 2.f});
    }

    std::optional<sf::Vector2f> updatePos(Paddle& leftPaddle, Paddle& rightPaddle, const float dt) {
        auto ballPos{ getPosition() };

        ballPos += m_velocity * dt;

        initPause();

        if(pointHasBeenWon(ballPos, leftPaddle, rightPaddle)) {
            return resetPause(ballPos);
        }

        windowWallsCheck(ballPos, leftPaddle, rightPaddle);
        paddleWallsCheck(ballPos, leftPaddle);
        paddleWallsCheck(ballPos, rightPaddle);

        return ballPos;
    }

    void resetState() {
        setPosition({m_windowSize.x / 2.f, m_windowSize.y / 2.f});
        m_velocity = {0.f, 0.f};
        m_gameState = GameState::CENTER_PAUSE;
    }

    sf::Vector2f getVelocity() const { return m_velocity; }
    void setVelocity(sf::Vector2f velocity) { m_velocity = velocity; }

private:
    const float m_radius;
    const float m_speed;
    sf::Vector2f m_velocity;
    sf::Vector2u m_windowSize;
    sf::Clock m_respawnClock;
    enum class GameState {
        PLAYING,
        CENTER_PAUSE,
        WAITING,
        POINT_SCORED,
    };
    GameState m_gameState { GameState::CENTER_PAUSE };

    void initPause() {
        if(m_gameState == GameState::CENTER_PAUSE) {
            m_respawnClock.restart();
            m_gameState = GameState::WAITING;
        }
        if(m_gameState == GameState::WAITING &&
           m_respawnClock.getElapsedTime().asSeconds() >= 0.5f) {
            m_velocity = randomizeDirection(m_speed);
            m_gameState = GameState::PLAYING;
        }
    }

    bool pointHasBeenWon(const sf::Vector2f& ballPos, Paddle& paddle1, Paddle& paddle2) const {
        if(ballPos.x + m_radius >= m_windowSize.x) {
            if(m_gameState == GameState::PLAYING) { paddle1.pointWon(); }
            return true;
        }
        else if(ballPos.x - m_radius <= 0) {
            if(m_gameState == GameState::PLAYING) { paddle2.pointWon(); }
            return true;
        }

        return false;
    }

    std::optional<sf::Vector2f> resetPause(sf::Vector2f& ballPos) {
        if(m_gameState == GameState::PLAYING) {
            m_respawnClock.restart();
            m_gameState = GameState::POINT_SCORED;
        }
        if(m_respawnClock.getElapsedTime().asSeconds() >= 0.5f) {
            ballPos = {m_windowSize.x / 2.f, m_windowSize.y / 2.f};
            m_velocity = {0.f, 0.f};
            m_gameState = GameState::CENTER_PAUSE;
        
            return ballPos;
        }

        return std::nullopt;
    }

    void windowWallsCheck(sf::Vector2f& ballPos, Paddle& paddle1, Paddle& paddle2) {
        auto resolveEdge{
            [](float pos, float radius, float limit) -> std::optional<float> {
                if(pos - radius < 0) { return radius; }
                if(pos + radius > limit) { return limit - radius; }
                return std::nullopt;
            }
        };

        if(auto x{ resolveEdge(ballPos.x, m_radius, m_windowSize.x) }) {
            ballPos.x = *x;
            m_velocity.x *= -1.f;
        }
        if(auto y{ resolveEdge(ballPos.y, m_radius, m_windowSize.y) }) {
            ballPos.y = *y;
            m_velocity.y *= -1.f;
        }
    }

    void paddleWallsCheck(sf::Vector2f& ballPos, const Paddle& paddle) {
        const auto pPos{ paddle.getPosition() };
        const auto pSize{ paddle.getSize() };

        auto posCheck{
            [](float pos, float radius, float paddlePos, float paddleSize) -> bool {
                return pos + radius > paddlePos &&
                       pos - radius < paddlePos + paddleSize;
            }
        };
        const bool XYcheck{ posCheck(ballPos.x, m_radius, pPos.x, pSize.x) &&
                            posCheck(ballPos.y, m_radius, pPos.y, pSize.y) };
        if(!XYcheck){ return; }
        
        float overlapLeft{(ballPos.x + m_radius) - pPos.x};
        float overlapRight{(pPos.x + pSize.x) - (ballPos.x - m_radius)};
        float overlapTop{(ballPos.y + m_radius) - pPos.y};
        float overlapBottom{(pPos.y + pSize.y) - (ballPos.y - m_radius)};

        float closestWall{ std::min({overlapLeft, overlapRight,
                                     overlapTop, overlapBottom}) };

        if(closestWall == overlapLeft) {
            ballPos.x = pPos.x - m_radius;
            m_velocity.x *= -1.f;
        }
        else if(closestWall == overlapRight) {
            ballPos.x = pPos.x + pSize.x + m_radius;
            m_velocity.x *= -1.f;
        }
        else if(closestWall == overlapTop) {
            ballPos.y = pPos.y - m_radius;
            m_velocity.y *= -1.f;
        }
        else if(closestWall == overlapBottom) {
            ballPos.y = pPos.y + pSize.y + m_radius;
            m_velocity.y *= -1.f;
        }
    }

    sf::Vector2f randomizeDirection(float speed) {
        // Choose one of two angle ranges: [-45,45] or [135,225]
        float angleDegrees;
        if (Random::get(0, 1) == 0) {
            angleDegrees = Random::get(-45.f, 45.f);
        } else {
            angleDegrees = Random::get(135.f, 225.f);
        }

        return {speed * std::cos(sf::degrees(angleDegrees).asRadians()),
                speed * std::sin(sf::degrees(angleDegrees).asRadians())};
    }
};

void endGameSetup(sf::RenderWindow& window, sf::Text& endText,
                  PongBall& ball, Paddle& paddle1, Paddle& paddle2) {
    ball.setVelocity({0.f, 0.f});
    endText.setString("Player " +
                      std::to_string((paddle1.getScoreNum() > paddle2.getScoreNum()) ? 1 : 2) +
                      " won! Press the spacebar to play again.");
    endText.setPosition({(window.getSize().x / 2.f) - (endText.getLocalBounds().size.x / 2.f),
                         (window.getSize().y / 2.f) - (endText.getLocalBounds().size.y / 2.f)});

    window.draw(endText);
}

int main() {
    sf::RenderWindow window{sf::VideoMode({800u, 600u}), "Test"};
    window.setVerticalSyncEnabled(true);
    sf::Clock clock;

    auto verticalCenter{ (window.getSize().y / 2.f) - 25.f };
    Paddle paddle1{{ 15.f, 50.f }, {50.f, verticalCenter}};
    Paddle paddle2{{ 15.f, 50.f }, {window.getSize().x - 65.f, verticalCenter}};

    sf::Font font;
    if (!font.openFromFile("assets/arial.ttf")) {
        std::cerr << "Error: Could not load font from assets/arial.ttf\n";
        return 1;
    }

    sf::Text endText{ font };
    endText.setCharacterSize(24);
    endText.setFillColor(sf::Color::White);

    sf::Text score{ font };
    score.setCharacterSize(24);
    score.setFillColor(sf::Color::White);

    const float ballRadius { 20.f };
    const float speed{ 600.f };
    PongBall ball{ballRadius, speed, window.getSize()};

    while(window.isOpen()) {
        float dt{ clock.restart().asSeconds() };

        while(const auto event = window.pollEvent()) {
            if(event->is<sf::Event::Closed>()) {
                window.close();
            }
            if (const auto* keyPressed = event->getIf<sf::Event::KeyPressed>()) {
                if (keyPressed->scancode == sf::Keyboard::Scan::Space &&
                    (paddle1.getScoreNum() == 10 || paddle2.getScoreNum() == 10)) {
                    ball.resetState();
                    paddle1.setScore(0);
                    paddle2.setScore(0);
                }
            }
        }

        sf::Vector2f paddle1Pos{
            paddle1.updatePos(sf::Keyboard::Scan::W, sf::Keyboard::Scan::S,
                              window.getSize(), dt)
        };
        paddle1.setPosition(paddle1Pos);

        sf::Vector2f paddle2Pos{
            paddle2.updatePos(sf::Keyboard::Scan::Up, sf::Keyboard::Scan::Down,
                              window.getSize(), dt)
        };
        paddle2.setPosition(paddle2Pos);

        if(auto ballPos{ ball.updatePos(paddle1, paddle2, dt) }) {
            ball.setPosition(ballPos.value());
        }

        score.setString("Score - P1: " + std::to_string(paddle1.getScoreNum()) +
                               " P2: " + std::to_string(paddle2.getScoreNum()));
        score.setPosition({(window.getSize().x / 2.f) - (score.getLocalBounds().size.x / 2.f), 10.f});

        window.clear();
        window.draw(paddle1);
        window.draw(paddle2);
        window.draw(score);
        window.draw(ball);

        if(paddle1.getScoreNum() == 10 || paddle2.getScoreNum() == 10) {
            endGameSetup(window, endText, ball, paddle1, paddle2);
        }

        window.display();
    }

    return 0;
}