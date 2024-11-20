#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <SFML/System.hpp>
#include <vector>
#include <memory>
#include <iostream>
//heelo
// Constants for reduced speeds
const float GRAVITY = 0.5f;
const float JUMP_SPEED = -6.0f;
const float MOVE_SPEED = 0.15f; // Slower bunny movement speed
const float OBSTACLE_SPEED = 1.5f; // Slower obstacle speed

// Rabbit class to handle bunny movement and jumping
class Rabbit {
public:
    sf::Sprite sprite;
    sf::Texture texture;
    bool isJumping = false;
    float velocityY = 0;
    float groundLevel = 180;

    Rabbit(const std::string& texturePath) {
        if (!texture.loadFromFile(texturePath)) {
            throw std::runtime_error("Failed to load bunny texture");
        }
        sprite.setTexture(texture);
        sprite.setPosition(30, groundLevel);
        sprite.setScale(0.4f, 0.4f);
    }

    void update() {
        if (isJumping) {
            velocityY += GRAVITY;
            sprite.move(0, velocityY);

            if (sprite.getPosition().y >= groundLevel) {
                sprite.setPosition(sprite.getPosition().x, groundLevel);
                isJumping = false;
                velocityY = 0;
            }
        }
    }

    void jump() {
        if (!isJumping) {
            isJumping = true;
            velocityY = JUMP_SPEED;
        }
    }

    void moveRight() {
        sprite.move(MOVE_SPEED, 0);
    }

    void draw(sf::RenderWindow& window) {
        window.draw(sprite);
    }
};

// Obstacle class to handle moving obstacles like the box
class Obstacle {
public:
    sf::Sprite sprite;
    sf::Texture texture;

    Obstacle(const std::string& texturePath, float x, float y, float scaleX = 1.0f, float scaleY = 1.0f) {
        if (!texture.loadFromFile(texturePath)) {
            throw std::runtime_error("Failed to load obstacle texture");
        }
        sprite.setTexture(texture);
        sprite.setPosition(x, y);
        sprite.setScale(scaleX, scaleY);
    }

    void moveObstacle() {
        sprite.move(-OBSTACLE_SPEED, 0);
    }

    bool isOffScreen() const {
        return sprite.getPosition().x + sprite.getGlobalBounds().width < 0;
    }

    void draw(sf::RenderWindow& window) {
        window.draw(sprite);
    }
};

// Background class to handle background scrolling
class Background {
public:
    sf::Sprite background1, background2;
    sf::Texture texture;

    Background(const std::string& texturePath) {
        if (!texture.loadFromFile(texturePath)) {
            throw std::runtime_error("Failed to load background texture");
        }

        background1.setTexture(texture);
        background2.setTexture(texture);
        background2.setPosition(background1.getGlobalBounds().width, 0);
    }

    void update() {
        background1.move(-0.65, 0); // Slower background scrolling speed
        background2.move(-0.65, 0); // Slower background scrolling speed

        if (background1.getPosition().x + background1.getGlobalBounds().width < 0) {
            background1.setPosition(background2.getPosition().x + background2.getGlobalBounds().width, 0);
        }
        if (background2.getPosition().x + background2.getGlobalBounds().width < 0) {
            background2.setPosition(background1.getPosition().x + background1.getGlobalBounds().width, 0);
        }
    }

    void draw(sf::RenderWindow& window) {
        window.draw(background1);
        window.draw(background2);
    }
};

// Main game class that controls the game loop and manages objects
class Game {
public:
    sf::RenderWindow window;
    Rabbit bunny;
    Background background;
    std::vector<std::shared_ptr<Obstacle>> obstacles;

    Game() : window(sf::VideoMode(626, 352), "Bunny Run Main Screen"), bunny("bunny_1.png"), background("track_1.jpeg") {
        obstacles.push_back(std::make_shared<Obstacle>("box.png", 190, 245, 0.25f, 0.25f));
    }

    void run() {
        bool isMovingRight = false; //track if the bunny is moving right

        while (window.isOpen()) {
            sf::Event event;
            while (window.pollEvent(event)) {
                if (event.type == sf::Event::Closed)
                    window.close();
            }

            //check key presses for movement
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) {
                bunny.moveRight();
                background.update();  // Move the background when the bunny moves right
                isMovingRight = true; // Mark that the bunny is moving
            } else {
                isMovingRight = false; // Bunny is not moving right
            }

            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up)) {
                bunny.jump();
            }

            bunny.update();

            // Move obstacles only if the bunny is moving right
            if (isMovingRight) {
                for (auto& obstacle : obstacles) {//this is a for loop which iterates over vector of obstacle and 
                    obstacle->moveObstacle();//move obstacle updates the box movements and make it move
                }
            }

            // Remove obstacles that are off-screen and spawn new ones
            for (auto& obstacle : obstacles) {
                if (obstacle->isOffScreen()) {//this checks that if box is out of screen
                    obstacles.erase(std::remove(obstacles.begin(), obstacles.end(), obstacle), obstacles.end());//this goes in the vector and removes the 
                    //obstacle from the vector which has gone off screen
                    obstacles.push_back(std::make_shared<Obstacle>("box.png", 800, 245, 0.25f, 0.25f));//if obstacle moves out of screen then a new obstacle is bein gintroduced from the postion
                    //where it creates a new obstacle box 
                }
            }

            window.clear();
            background.draw(window);

            for (auto& obstacle : obstacles) {//this automatically knows the type of obstacle 
                obstacle->draw(window);
            }

            bunny.draw(window);

            window.display();
        }
    }
};

int main() {
   
        Game game;
        game.run();
 

    return 0;
}
