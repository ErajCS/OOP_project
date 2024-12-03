#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <SFML/System.hpp>
#include <vector>
#include <memory>
#include <iostream>
#include <string>

// Constants
const float GRAVITY = 0.5f;
const float JUMP_SPEED = -6.0f;
const float MOVE_SPEED = 0.7f;

// Rabbit Class
class Rabbit {
public:
    sf::Sprite sprite;
    sf::Texture texture;
    float velocityY = 0;
    float groundLevel;
    bool isJumping = false;

    Rabbit(const std::string& texturePath, float groundLevel)
        : groundLevel(groundLevel) {
        if (!texture.loadFromFile(texturePath)) {
            throw std::runtime_error("Failed to load rabbit texture");
        }
        sprite.setTexture(texture);
        sprite.setScale(0.4f, 0.4f);
        sprite.setPosition(30, groundLevel);
    }

    void jump(float jumpSpeed) {
        if (!isJumping) {
            isJumping = true;
            velocityY = jumpSpeed;
        }
    }

    void moveRight(float moveSpeed) {
        sprite.move(moveSpeed, 0);
    }

    void update(float gravity) {
        if (isJumping) {
            velocityY += gravity;
            sprite.move(0, velocityY);

            if (sprite.getPosition().y >= groundLevel) {
                sprite.setPosition(sprite.getPosition().x, groundLevel);
                isJumping = false;
                velocityY = 0;
            }
        }
    }

    void draw(sf::RenderWindow& window) {
        window.draw(sprite);
    }
};

// Obstacle Class
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

    void draw(sf::RenderWindow& window) {
        window.draw(sprite);
    }
};

// Box Class (inherits from Obstacle)
class Box : public Obstacle {
public:
    Box(float x, float y) : Obstacle("box.png", x, y, 0.25f, 0.25f) {}
};

// Collectible Class
class Collectible {
public:
    sf::Sprite sprite;
    sf::Texture texture;

    Collectible(const std::string& texturePath, float x, float y, float scaleX = 1.0f, float scaleY = 1.0f) {
        if (!texture.loadFromFile(texturePath)) {
            throw std::runtime_error("Failed to load collectible texture");
        }
        sprite.setTexture(texture);
        sprite.setPosition(x, y);
        sprite.setScale(scaleX, scaleY);
    }

    void draw(sf::RenderWindow& window) {
        window.draw(sprite);
    }
};

// Carrot Class (inherits from Collectible)
class Carrot : public Collectible {
public:
    Carrot(float x, float y) : Collectible("carrot.png", x, y, 0.2f, 0.2f) {}
};

class Background {
public:
    sf::Sprite sprite1, sprite2;
    sf::Texture texture;
    float width;

    Background(const std::string& texturePath) {
        if (!texture.loadFromFile(texturePath)) {
            std::cout<<("Failed to load background texture");
        }
        sprite1.setTexture(texture);
        sprite2.setTexture(texture);

        width = texture.getSize().x;

        sprite1.setPosition(0, 0);
        sprite2.setPosition(width, 0);
    }

    void update(float viewLeft) {//if the left side of the view has moved past the right edge of sprite1. If it has, sprite1 is repositioned to the right of sprite2.
        if (viewLeft > sprite1.getPosition().x + width) {
            sprite1.setPosition(sprite2.getPosition().x + width, sprite1.getPosition().y);
        }
        //|-------------------------|  <- camera view (view)
//| sprite2 | sprite1 | sprite3 |

        if (viewLeft > sprite2.getPosition().x + width) {
            sprite2.setPosition(sprite1.getPosition().x + width, sprite2.getPosition().y);
        }
    }

    void draw(sf::RenderWindow& window) {
        window.draw(sprite1);
        window.draw(sprite2);
    }
};


// Game Class
class Game {
public:
    sf::RenderWindow window;
    Rabbit rabbit;
    Background background;
    sf::View view;
    //https://stackoverflow.com/questions/26211985/c-shared-ptrvectort-vs-vectorshared-ptrt
    std::vector<std::shared_ptr<Obstacle>> obstacles;//this is vector of shared pointers pointing to the box 
    //the box automatically gets destroyed when the pointer pointing to it gets nothing 
    //this handles the fiunctionality of delete in dynamic array 
    std::vector<std::shared_ptr<Carrot>> carrots;
    int score = 0;
    sf::Font font;
    sf::Text scoreText;
    sf::Text timerText;
    sf::Clock clock; // Timer for the game
    float timeLimit = 60.0f; // Game ends after 60 seconds
    sf::RectangleShape endpoint; // Visual indicator for the finish line
    bool gameEnded = false;

   Game()
   //makes objects and set their initial positions
    : window(sf::VideoMode(626, 352), "Bunny Run Main Screen"), rabbit("bunny_1.png", 180), background("track_1.jpeg") {
    // Place obstacles (10 boxes)
    for (int i = 0; i < 15; ++i) {
        float x = 300 + i * 200; //the first box position is 300 and then other boxes are placed with 300 units apart
        obstacles.push_back(std::make_shared<Box>(x, 245));
        //here mkae_shared creates an object of box and contructor is called with initial posiiton of box 
        //push back adds the elment at the end of the vector 
        //make shared is used in place of new keyword while handling dynamic memory allocation
    }

    // Place carrots (15 carrots)
    for (int i = 0; i < 15; ++i) {
        float x = 400 + i * 200; 
        carrots.push_back(std::make_shared<Carrot>(x, 245));
    }

    if (!font.loadFromFile("arial.ttf")) {
        throw std::runtime_error("Failed to load font");
    }

    // Initialize score text
    scoreText.setFont(font);
    scoreText.setCharacterSize(24);
    scoreText.setFillColor(sf::Color::White);
    scoreText.setPosition(500, 10);
    updateScoreText();

    // Initialize timer text
    timerText.setFont(font);
    timerText.setCharacterSize(24);
    timerText.setFillColor(sf::Color::White);
    timerText.setPosition(10, 10);

    // Initialize endpoint
    endpoint.setSize(sf::Vector2f(50, 100));//a rectangle with dimensions
    endpoint.setFillColor(sf::Color::Red);
    float endpointX = 300 + (14) * 200; //putting the endpoint after the last box 
    //The x-position (endpointX) places the endpoint after the last obstacle (i.e., 200 * 14 + 300).
endpoint.setPosition(endpointX, 180); 
   // endpoint.setPosition(4000, 180); // Place it far enough on the track for 10 items
//the endpoint is not visible because it is placed outside the dimenions of the screen 
    // Initialize the view


    view.reset(sf::FloatRect(0, 0, 626, 352));
    //reset is memeber of class View used to reset the position of the camera 
    //floatrect is a struct in sfml that takes 4 paramters: x coordinate of top left corner, y coordinate of top left corner, length and width which are dimeniso  of screen 

}


        

    

      void run() {
    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();
        }

//this make sa object of clock 
//jabsy clock start hui hy kitna time guzar chuka hy 
        float elapsed = clock.getElapsedTime().asSeconds();
        if (!gameEnded && elapsed >= timeLimit) {
            endGame(false); // Game over due to timeout
            return;
        }

        updateTimerText(timeLimit - elapsed);

        // Player Controls
        if (!gameEnded) {//if game has not ended 
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) {
                rabbit.moveRight(MOVE_SPEED);
                if (rabbit.sprite.getPosition().x > view.getCenter().x) {
                    view.setCenter(rabbit.sprite.getPosition().x, view.getCenter().y);
                    //y coordinate reamins the same as only x axis is changing 
                }
            }

            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up)) {
                rabbit.jump(JUMP_SPEED);
            }
        }

        rabbit.update(GRAVITY);
        checkCarrotCollision();

        // Update background
        background.update(view.getCenter().x - (view.getSize().x / 2));
        //If the view is centered at (500, 300) in the world, view.getCenter().x will be 500.

        // Update text positions relative to the view
        sf::Vector2f viewCenter = view.getCenter();
        //gives the width and height of the area that is being displayed
        scoreText.setPosition(viewCenter.x - view.getSize().x / 2 + 10, viewCenter.y - view.getSize().y / 2 + 10);
        //
        timerText.setPosition(viewCenter.x - view.getSize().x / 2 + 10, viewCenter.y - view.getSize().y / 2 + 40);

        // Only move the endpoint when the time reaches exactly 60 seconds
        if (elapsed >= 60.0f && !gameEnded) {
            endpoint.setPosition(3600, 180); // Set endpoint position at 60 seconds
        }

        // Rendering
        window.setView(view);
        window.clear();
        background.draw(window);
        drawObstacles(window);
        drawCarrots(window);
        rabbit.draw(window);
        window.draw(scoreText);
        window.draw(timerText);

        // Draw endpoint if it's visible (after 60 seconds)
        if (elapsed >= 60.0f) {
            window.draw(endpoint);
        }

        window.display();

        // Check if rabbit reaches the endpoint
        if (!gameEnded && rabbit.sprite.getGlobalBounds().intersects(endpoint.getGlobalBounds())) {
            endGame(true); // Level completed
            return;
        }
    }
}


private:
    void checkCarrotCollision() {
    // Loop through the carrots and check for collision
    for (auto it = carrots.begin(); it != carrots.end(); ) {
        // Check if the rabbit collides with the current carrot
        if (checkCollisionWithCarrot(*it)) {
            // If collision occurs, increment score and remove the carrot
            updateScoreText();
            it = carrots.erase(it);  // Remove the carrot and get the new iterator
        } else {
            ++it;  // Move to the next carrot if no collision
        }
    }
}

// Helper method to check collision between rabbit and carrot
bool checkCollisionWithCarrot(const std::shared_ptr<Carrot>& carrot) {
    if (rabbit.sprite.getGlobalBounds().intersects(carrot->sprite.getGlobalBounds())) {
        score += 5;  // Increase score
        return true; // Return true to indicate a collision
    }
    return false; // No collision
}

    void updateScoreText() {
        scoreText.setString("Score: " + std::to_string(score));
    }

    void updateTimerText(float timeRemaining) {
        timerText.setString("Time: " + std::to_string(static_cast<int>(timeRemaining)) + "s");
    }

  void drawObstacles(sf::RenderWindow& window) {
    for (auto& obstacle : obstacles) {
        obstacle->draw(window); // Always draw all obstacles
    }
}

void drawCarrots(sf::RenderWindow& window) {
    for (auto& carrot : carrots) {
        carrot->draw(window); // Always draw all carrots
    }
}



    void endGame(bool levelCompleted) {
        gameEnded = true;

        // Pause to show the end screen
        sf::Text endText;
        endText.setFont(font);
        endText.setCharacterSize(36);
        endText.setFillColor(sf::Color::White);
        endText.setString(levelCompleted ? "Level Completed!" : "Game Over!");
        endText.setPosition(view.getCenter().x - 100, view.getCenter().y - 50);

        window.clear();
        window.draw(endText);
        window.display();

        sf::sleep(sf::seconds(3)); // Display for 3 seconds
        window.close();

        // Print final score
        std::cout << (levelCompleted ? "Level Completed!" : "Game Over!") << std::endl;
        std::cout << "Final Score: " << score << std::endl;
    }
};

int main() {
   
        Game game;
        game.run();
   

    return 0;
}