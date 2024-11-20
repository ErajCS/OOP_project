#include <SFML/Graphics.hpp>

int main() {
    sf::RenderWindow window(sf::VideoMode(700, 600), "Ending Screen");

    // Load background image
    sf::Texture backgroundTexture;
    if (!backgroundTexture.loadFromFile("ending.jpg")) {
        return -1;
    }
    sf::Sprite background(backgroundTexture);

    // Load font
    sf::Font font;
    if (!font.loadFromFile("arialbd.ttf")) {
        return -1;
    }

    // Create text
    sf::Text titleText("GAME\nOVER", font, 100); // Set font size to 100
    titleText.setFillColor(sf::Color(255, 153, 0)); // Set color similar to orange
    titleText.setOutlineColor(sf::Color::Black);    // Add an outline for contrast
    titleText.setOutlineThickness(5);
    titleText.setPosition(200, 150); // Adjust position based on your screen

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();
        }

        window.clear();
        window.draw(background);    // Draw background
        window.draw(titleText);     // Draw title text
        window.display();
    }

    return 0;
}
