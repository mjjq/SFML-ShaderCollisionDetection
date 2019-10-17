#include <iostream>
#include <SFML/Graphics.hpp>
#include <SFML/OpenGL.hpp>

struct Sphere
{
    sf::Vector2f position;
    sf::Vector2f velocity;

private:
    float radius = 5.0f;
};

class CollisionDetector
{
    sf::Vector2f worldSize;
    sf::RenderTexture collisionPairTexture;
    sf::Shader collisionShader;
    sf::Sprite emptySprite;

    std::vector<sf::Glsl::Vec2 > spherePositions;
    std::vector<sf::Vector2f > sphereVelocities;
    std::vector<sf::Color > sphereColors;
    std::vector<std::pair<int, int> > collidingPairs;

    float radius = 5.0f;

public:
    CollisionDetector(int noSpheres, sf::Vector2f _worldSize) : worldSize{_worldSize}
    {
        for(int i=0; i<noSpheres; ++i)
        {
            spherePositions.push_back({0.0f, 0.0f});
            sphereVelocities.push_back({0.0f, 0.0f});
            sphereColors.push_back(sf::Color::Red);
        }
        collisionPairTexture.create(noSpheres, noSpheres);

        if (!collisionShader.loadFromFile("collision.frag", sf::Shader::Fragment))
        {
            std::cout << "error loading shader\n";
        }
    }

    void integratePositions(float dt)
    {
        for(int i=0; i<spherePositions.size(); ++i)
        {
            spherePositions[i] += sphereVelocities[i]*dt;

            if(spherePositions[i].x > worldSize.x ||
               spherePositions[i].x < 0.0f)
                sphereVelocities[i].x *= -1.0f;
            if(spherePositions[i].y > worldSize.y ||
               spherePositions[i].y < 0.0f)
                sphereVelocities[i].y *= -1.0f;
        }

    }

    void getCollidingPairsFromTexture(const sf::Texture & collidingTexture)
    {
        auto image = collidingTexture.copyToImage();

        for(int i=0; i<collidingTexture.getSize().x; ++i)
            for(int j=i+1; j<collidingTexture.getSize().x; ++j)
            {
                if(image.getPixel(i,j) == sf::Color::White)
                    collidingPairs.push_back({i,j});
            }
    }

    void detectCollisions()
    {
        for(int i=0; i<sphereColors.size(); ++i)
        {
            sphereColors[i] = sf::Color::Red;
        }

        //collisionShader.setUniformArray("positions", spherePositions.data(), spherePositions.size());
        //collisionShader.setUniform("result", radius);

        collisionPairTexture.draw(emptySprite, &collisionShader);

        getCollidingPairsFromTexture(collisionPairTexture.getTexture());
    }

    void resolveCollisions()
    {
        for(int i=0; i<collidingPairs.size(); ++i)
        {
            int collider1 = collidingPairs[i].first;
            int collider2 = collidingPairs[i].second;

            sphereColors[collider1] = sf::Color::Green;
            sphereColors[collider2] = sf::Color::Green;
        }
    }

    void drawSpheres(sf::RenderWindow &window)
    {
        sf::CircleShape circle(radius);
        circle.setOrigin(radius, radius);
        //circle.setFillColor(sf::Color::Red);
        for(int i=0; i<spherePositions.size(); ++i)
        {
            circle.setPosition(spherePositions[i]);
            circle.setFillColor(sphereColors[i]);

            window.draw(circle);
        }

        window.draw(emptySprite, &collisionShader);
    }

};

int main()
{
    sf::RenderWindow window;
    sf::VideoMode mode(800, 800);
    window.create(mode, "test");

    //CollisionDetector test(1000, {100.0f, 100.0f});

    sf::Shader shader;
    shader.loadFromFile("collision.frag", sf::Shader::Fragment);
    sf::RenderTexture texture;
    texture.create(1000, 1000);

    sf::Sprite emptySprite;
    texture.draw(emptySprite, &shader);

    sf::RectangleShape shape;
    shape.setPosition(100.0f, 100.0f);
    shape.setSize({100.0f, 100.0f});
    shape.setTexture(&texture.getTexture());

    while(window.isOpen())
    {
        sf::Event currEvent;

        while(window.pollEvent(currEvent))
        {
            switch(currEvent.type)
            {
                case(sf::Event::Closed):
                    window.close();
                    break;
                default:
                    break;
            }
        }


        window.clear(sf::Color::Black);

        /*test.detectCollisions();
        test.resolveCollisions();
        test.drawSpheres(window);
        test.integratePositions(0.1f);*/
        window.draw(shape);

        window.display();
    }
}
