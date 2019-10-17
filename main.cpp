#include <iostream>
#include <vector>
#include <SFML/Graphics.hpp>
#include <SFML/OpenGL.hpp>
#include <stdlib.h>

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

    sf::RectangleShape debugShape;

    std::vector<sf::Glsl::Vec2 > spherePositions;
    std::vector<sf::Vector2f > sphereVelocities;
    std::vector<sf::Color > sphereColors;
    std::vector<std::pair<int, int> > collidingPairs;

    int numSpheres = 0;

    float radius = 5.0f;

public:
    CollisionDetector(int noSpheres, sf::Vector2f _worldSize) : worldSize{_worldSize}, numSpheres{noSpheres}
    {
        for(int i=0; i<noSpheres; ++i)
        {
            float scale = 10.0f;
            float random = (float)rand()/(float)RAND_MAX;
            spherePositions.push_back(worldSize/2.0f);
            sphereVelocities.push_back(scale*sf::Vector2f{random, random*random});
            sphereColors.push_back(sf::Color::Red);
        }
        collisionPairTexture.create(noSpheres, noSpheres);
        emptySprite.setTexture(collisionPairTexture.getTexture());
        emptySprite.setScale({10.0f, 10.0f});

        debugShape.setSize({100.0f, 100.0f});
        debugShape.setPosition({100.0f, 100.0f});

        if (!collisionShader.loadFromFile("collision.frag", sf::Shader::Fragment))
        {
            std::cout << "error loading shader\n";
        }
                //collisionShader.setUniform("numberOfSpheres", 5);
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
        collidingPairs.clear();
        auto image = collidingTexture.copyToImage();

        for(int i=0; i<collidingTexture.getSize().x; ++i)
            for(int j=i+1; j<collidingTexture.getSize().y; ++j)
            {
                if(image.getPixel(j,i) == sf::Color::White)
                {
                    //std::cout << "i,j: " << i << ", " << j << "\n";
                    collidingPairs.push_back({i,j});
                }
            }

        //std::cout << collidingPairs.size() << " collpairs\n";
    }

    void detectCollisions()
    {

        collisionShader.setUniformArray("positions", spherePositions.data(), spherePositions.size());
        collisionShader.setUniform("nSpheres", (int)spherePositions.size());
        collisionShader.setUniform("radius", radius);


        collisionPairTexture.draw(emptySprite, &collisionShader);

        getCollidingPairsFromTexture(collisionPairTexture.getTexture());
    }

    void resolveCollisions()
    {
        for(int i=0; i<sphereColors.size(); ++i)
        {
            sphereColors[i] = sf::Color::Red;
        }

        for(int i=0; i<collidingPairs.size(); ++i)
        {
            int collider1 = collidingPairs[i].first;
            int collider2 = collidingPairs[i].second;

            if(collider1 != collider2)
            {
                sphereColors[collider1] = sf::Color::Green;
                sphereColors[collider2] = sf::Color::Green;

            }
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

        emptySprite.setScale({200.0f/numSpheres, 200.0f/numSpheres});
        emptySprite.setPosition({200.0f, 200.0f});
        window.draw(emptySprite);
        emptySprite.setPosition({00.0f, 00.0f});
        emptySprite.setScale({1.0f, 1.0f});
    }

};

int main()
{
    sf::RenderWindow window;
    sf::VideoMode mode(800, 800);
    window.create(mode, "test");

    CollisionDetector test(150, {100.0f, 100.0f});

    sf::Shader shader;
    shader.loadFromFile("collision.frag", sf::Shader::Fragment);
    sf::RenderTexture texture;
    texture.create(20,20);

    sf::Sprite emptySprite;
    emptySprite.setTexture(texture.getTexture());
    texture.draw(emptySprite, &shader);
    emptySprite.setScale({20.f, 20.0f});

    sf::RectangleShape shape;
    shape.setPosition(100.0f, 100.0f);
    shape.setSize({10.0f, 10.0f});
    shape.setScale({10.0f, 10.0f});

    shape.setTexture(&texture.getTexture());

    srand(420000);

    bool isPaused = false;

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
                case(sf::Event::KeyPressed):

                    if(sf::Keyboard::isKeyPressed(sf::Keyboard::Space))
                        isPaused = !isPaused;
                    break;

                default:
                    break;
            }
        }



        window.clear(sf::Color::Black);

        if(!isPaused)
        {
            test.detectCollisions();
            test.resolveCollisions();
            test.integratePositions(0.1f);

        }
        //window.draw(emptySprite);
            test.drawSpheres(window);

        window.display();

        sf::sleep(sf::milliseconds(30));
    }
}
