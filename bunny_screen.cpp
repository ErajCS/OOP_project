#include <SFML/Graphics.hpp>
#include <iostream>

int main() {
    // Create the game window
    sf::RenderWindow window(sf::VideoMode(736, 473), "Main Menu");

    // Load the background image
    sf::Texture backgroundTexture;
    if (!backgroundTexture.loadFromFile("jungle1.jpg")) {
        std::cerr << "Error loading background image" << std::endl;
        return -1;
    }

    // Create a sprite for the background
    sf::Sprite backgroundSprite;
    backgroundSprite.setTexture(backgroundTexture);

    // Load font for button text
    sf::Font font;
    if (!font.loadFromFile("arial.ttf")) {
        std::cerr << "Error loading font" << std::endl;
        return -1;
    }

    // Create Start Game button
    sf::RectangleShape startButton(sf::Vector2f(150, 50)); // Button size
    startButton.setFillColor(sf::Color(150, 250, 50));   // Button color
    startButton.setPosition(300, 150);                    // Button position

    sf::Text startText("PLAY", font, 30);
    startText.setFillColor(sf::Color::Black);
    startText.setPosition(startButton.getPosition().x +33, startButton.getPosition().y + 5);

    // Create Exit button
    sf::RectangleShape exitButton(sf::Vector2f(150, 50));
    exitButton.setFillColor(sf::Color(160, 80, 19));
    exitButton.setPosition(300, 250);

    sf::Text exitText("EXIT", font, 30);
    exitText.setFillColor(sf::Color::Black);
    exitText.setPosition(exitButton.getPosition().x + 35, exitButton.getPosition().y + 5);

    // Main loop
    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();

            // Check for mouse clicks
            if (event.type == sf::Event::MouseButtonPressed) {
                sf::Vector2i mousePos = sf::Mouse::getPosition(window);

                // Check if Start Game button is clicked
                if (startButton.getGlobalBounds().contains(static_cast<sf::Vector2f>(mousePos))) {
                    std::cout << "Starting the game..." << std::endl;
                    // Add code here to start the game
                    window.close();
                }

                // Check if Exit button is clicked
                if (exitButton.getGlobalBounds().contains(static_cast<sf::Vector2f>(mousePos))) {
                    std::cout << "Exiting the game..." << std::endl;
                    window.close();
                }
            }
        }

        // Clear the window and draw the background, buttons, and text
        window.clear();
        window.draw(backgroundSprite);
        window.draw(startButton);
        window.draw(startText);
        window.draw(exitButton);
        window.draw(exitText);
        window.display();
    }

    return 0;
}
