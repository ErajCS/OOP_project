#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <SFML/System.hpp>

const float GRAVITY = 0.8f;
const float JUMP_SPEED = -10.0f; // Adjusted for a better jump arc
const float MOVE_SPEED = 0.09f;

int main() {
    sf::RenderWindow window(sf::VideoMode(626, 352), "Bunny Run Main Screen");

    // Create a view that follows the bunny
    sf::View view(sf::FloatRect(0, 0, 626, 352));

    // Load background texture
    sf::Texture backgroundTexture;
    if (!backgroundTexture.loadFromFile("track_1.jpeg")) {
        return -1;
    }

    // Create two background sprites for looping
    sf::Sprite background1(backgroundTexture);
    sf::Sprite background2(backgroundTexture);
    background2.setPosition(background1.getGlobalBounds().width, 0); // Position the second background next to the first

    // Load bunny texture
    sf::Texture bunnyTexture;
    if (!bunnyTexture.loadFromFile("bunny_1.png")) {
        return -1;
    }
    sf::Sprite bunny(bunnyTexture);
    bunny.setPosition(30, 180); // Initial position of the bunny on the track
    bunny.setScale(0.4f, 0.4f);

    // Load box texture
    sf::Texture boxTexture;
    if (!boxTexture.loadFromFile("box.png")) { // Replace with the actual box image path
        return -1;
    }
    sf::Sprite box(boxTexture);
    box.setPosition(190, 245); // Position the box on the track
    box.setScale(0.25f, 0.25f); // Adjust size if needed

    // Variables for bunny movement
    bool isJumping = false;
    float velocityY = 0;

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();
        }

        // Controls for right movement and jumping
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) {
            bunny.move(MOVE_SPEED, 0);
            background1.move(-2, 0);
            background2.move(-2, 0);
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up) && !isJumping) {
            isJumping = true;
            velocityY = JUMP_SPEED; // Initial jump speed
        }

        // Apply gravity if jumping
        if (isJumping) {
            velocityY += GRAVITY; // Apply gravity to Y velocity
            bunny.move(0, velocityY);

            // Check if bunny lands back on track
            if (bunny.getPosition().y >= 180) { // Adjust to the ground level of your track
                bunny.setPosition(bunny.getPosition().x, 180); // Reset Y position to track level
                isJumping = false; // Stop jumping
                velocityY = 0; // Reset vertical velocity
            }
        }

        // Move background continuously
        if (background1.getPosition().x + background1.getGlobalBounds().width < 0) {
            background1.setPosition(background2.getPosition().x + background2.getGlobalBounds().width, 0);
        }
        if (background2.getPosition().x + background2.getGlobalBounds().width < 0) {
            background2.setPosition(background1.getPosition().x + background1.getGlobalBounds().width, 0);
        }

        // Render everything
        window.clear();
        window.draw(background1); // Draw the first background
        window.draw(background2); // Draw the second background
        window.draw(box);         // Draw the box (does not move)
        window.draw(bunny);       // Draw the bunny
        window.display();
    }

    return 0;
}
