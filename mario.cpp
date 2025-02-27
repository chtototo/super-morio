#include <SFML/Graphics.hpp>
#include <vector>
#include <iostream>
using namespace std;

const float GRAVITY = 5000.0f;
const float JUMP_STRENGTH = -1500.0f;
const float MOVE_SPEED = 300.0f;
class Player
{
public:
    sf::Sprite shape;
    sf::Vector2f velocity;
    bool isOnGround;

    std::vector<sf::Texture> texturesIdle;
    std::vector<sf::Texture> texturesWalkLeft;
    std::vector<sf::Texture> texturesWalkRight;
    std::vector<sf::Texture> texturesJump;

    int currentFrame;
    sf::Clock animationClock;
    const float frameTime = 100.0f;

    enum PlayerState
    {
        Idle,
        WalkingLeft,
        WalkingRight,
        Jumping
    };
    PlayerState currentState;

    bool isUpKeyPressed;

    Player()
    {
        loadTextures();

        shape.setTexture(texturesIdle[0]);
        shape.setPosition(100.0f, 100.0f);
        velocity = sf::Vector2f(0.0f, 0.0f);
        isOnGround = false;
        currentFrame = 0;
        currentState = Idle;
        isUpKeyPressed = false;
    }

    void loadTextures()
    {
        texturesIdle.resize(3);
        if (!texturesIdle[0].loadFromFile("./images/cat0.png") ||
            !texturesIdle[1].loadFromFile("./images/cat1.png") ||
            !texturesIdle[2].loadFromFile("./images/cat2.png"))
        {
            cout << "Failed to load idle texture!" << endl;
        }

        texturesWalkLeft.resize(3);
        if (!texturesWalkLeft[0].loadFromFile("./images/walk_left0.png") ||
            !texturesWalkLeft[1].loadFromFile("./images/walk_left1.png") ||
            !texturesWalkLeft[2].loadFromFile("./images/walk_left2.png"))
        {
            cout << "Failed to load walk left textures!" << endl;
        }

        texturesWalkRight.resize(3);
        if (!texturesWalkRight[0].loadFromFile("./images/walk_right0.png") ||
            !texturesWalkRight[1].loadFromFile("./images/walk_right1.png") ||
            !texturesWalkRight[2].loadFromFile("./images/walk_right2.png"))
        {
            cout << "Failed to load walk right textures!" << endl;
        }

        texturesJump.resize(1);
        if (!texturesJump[0].loadFromFile("./images/walk_right0.png"))
        {
            cout << "Failed to load jump texture!" << endl;
        }
    }

    void update(std::vector<sf::FloatRect> &platforms, const sf::Vector2u &windowSize, float deltaTime)
    {
        if (!isOnGround)
        {
            velocity.y += GRAVITY * deltaTime;
        }

        shape.move(velocity.x * deltaTime, 0.0f);
        if (checkCollisions(platforms) || shape.getPosition().x < 0 || shape.getPosition().x + shape.getGlobalBounds().width > windowSize.x)
        {
            shape.move(-velocity.x * deltaTime, 0.0f);
        }

        shape.move(0.0f, velocity.y * deltaTime);
        if (checkCollisions(platforms) || shape.getPosition().y < 0 || shape.getPosition().y + shape.getGlobalBounds().height > windowSize.y)
        {
            shape.move(0.0f, -velocity.y * deltaTime);
            velocity.y = 0.0f;
            isOnGround = true;
        }
        else
        {
            isOnGround = false;
        }

        updateAnimation();
    }

    void updateAnimation()
    {
        if (animationClock.getElapsedTime().asMilliseconds() > frameTime)
        {
            currentFrame = (currentFrame + 1) % getCurrentTextureSet().size();
            shape.setTexture(getCurrentTextureSet()[currentFrame]);
            animationClock.restart();
        }
    }

    const std::vector<sf::Texture> &getCurrentTextureSet() const
    {
        switch (currentState)
        {
        case WalkingLeft:
            return texturesWalkLeft;
        case WalkingRight:
            return texturesWalkRight;
        case Jumping:
            return texturesJump;
        default:
            return texturesIdle;
        }
    }

    bool checkCollisions(std::vector<sf::FloatRect> &platforms)
    {
        for (auto &platform : platforms)
        {
            if (shape.getGlobalBounds().intersects(platform))
            {
                return true;
            }
        }
        return false;
    }

    void jump()
    {
        if (isOnGround)
        {
            velocity.y = JUMP_STRENGTH;
            isOnGround = false;
            currentState = Jumping;
        }
    }

    void moveLeft()
    {
        velocity.x = -MOVE_SPEED;
        currentState = WalkingLeft;
    }

    void moveRight()
    {
        velocity.x = MOVE_SPEED;
        currentState = WalkingRight;
    }

    void stop()
    {
        velocity.x = 0.0f;
        if (isOnGround)
        {
            currentState = Idle;
        }
    }
};

int main()
{
    sf::RenderWindow window(sf::VideoMode(800, 600), "Super Mario Clone");

    Player player;
    std::vector<sf::FloatRect> platforms;
    platforms.push_back(sf::FloatRect(0.0f, 560.0f, 800.0f, 40.0f));
    platforms.push_back(sf::FloatRect(200.0f, 450.0f, 200.0f, 20.0f));

    sf::Texture skyTexture;
    if (!skyTexture.loadFromFile("./images/background.png"))
    {
        cout << "Failed to load sky texture!" << endl;
        return -1;
    }

    sf::Sprite skySprite;
    skySprite.setTexture(skyTexture);
    skySprite.setPosition(-140.f, 0);
    skySprite.setScale(1.f, 0.6f);
    sf::Clock clock;
    while (window.isOpen())
    {
        float deltaTime = clock.restart().asSeconds();

        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up))
        {
            if (!player.isUpKeyPressed)
            {
                player.jump();
                player.isUpKeyPressed = true;
            }

            else
            {
                player.isUpKeyPressed = false;
            }
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left))
        {
            player.moveLeft();
        }
        else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right))
        {
            player.moveRight();
        }
        else
        {
            player.stop();
        }

        player.update(platforms, window.getSize(), deltaTime);

        window.clear();
        window.draw(skySprite);
        window.draw(player.shape);
        int number = 0;
        for (auto &platform : platforms)
        {
            sf::RectangleShape rect(sf::Vector2f(platform.width, platform.height));
            rect.setPosition(platform.left, platform.top);
            if (number == 0)
            {
                rect.setFillColor(sf::Color::Transparent);
            }
            else
            {
                rect.setFillColor(sf::Color(27, 158, 0));
            }
            number++;
            window.draw(rect);
        }
        window.display();
    }

    return 0;
}