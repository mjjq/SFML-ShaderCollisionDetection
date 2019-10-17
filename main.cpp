#include <iostream>
#include <vector>
#include <SFML/Graphics.hpp>
#include <SFML/OpenGL.hpp>
#include <stdlib.h>

class Math
{
public:
    static float dot(sf::Vector2f const & v1,
              sf::Vector2f const & v2)
    {
        return v1.x*v2.x + v1.y*v2.y;
    }

    static float square(sf::Vector2f const & v)
    {
        return Math::dot(v,v);
    }
};

class CollisionDetector
{
    sf::Vector2f worldSize;
    sf::RenderTexture collisionPairTexture;
    sf::RenderTexture resolvingTexture;
    sf::Shader collisionShader;
    sf::Shader resolutionShader;
    sf::Sprite emptySprite;
    sf::Sprite emptyResSprite;

    std::vector<sf::Glsl::Vec2 > spherePositions;
    std::vector<sf::Vector2f > sphereVelocities;
    std::vector<sf::Color > sphereColors;
    std::vector<std::pair<int, int> > collidingPairs;

    int numSpheres = 0;

    float radius = 2.0f;

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

        resolvingTexture.create(noSpheres, 1);
        emptyResSprite.setTexture(resolvingTexture.getTexture());


        if (!collisionShader.loadFromFile("collision.frag", sf::Shader::Fragment))
        {
            std::cout << "error loading shader\n";
        }
        if (!resolutionShader.loadFromFile("resolving.frag", sf::Shader::Fragment))
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

    void detectCollisionsGPU()
    {

        collisionShader.setUniformArray("positions", spherePositions.data(), spherePositions.size());
        collisionShader.setUniform("nSpheres", (int)spherePositions.size());
        collisionShader.setUniform("radius", radius);


        collisionPairTexture.draw(emptySprite, &collisionShader);

       // getCollidingPairsFromTexture(collisionPairTexture.getTexture());
    }

    void detectCollisionsCPU()
    {
        collidingPairs.clear();
        for(int i=0; i<spherePositions.size(); ++i)
            for(int j=i+1; j<spherePositions.size(); ++j)
            {
                if(Math::square(spherePositions[i] - spherePositions[j]) <= 4.0f*radius*radius )
                    collidingPairs.push_back({i,j});
            }
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

    void resolveCollisionsPureShader()
    {
        resolutionShader.setUniform("collisionTexture", &collisionPairTexture);

        resolvingTexture.draw(emptyResSprite, &resolutionShader);

        auto image = resolvingTexture.getTexture().copyToImage();

        for(int i=0; i<image.getSize().x; ++i)
            sphereColors[i] = image.getPixel(i,0);
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

        emptySprite.setScale({500.0f/numSpheres, 500.0f/numSpheres});
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

    CollisionDetector test(4, {100.0f, 100.0f});



    srand(420000);

    bool isPaused = false;
    bool useGPUColl = true;

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
                    else if(sf::Keyboard::isKeyPressed(sf::Keyboard::C))
                        useGPUColl = !useGPUColl;
                    break;


                default:
                    break;
            }
        }



        window.clear(sf::Color::Black);

        if(!isPaused)
        {
            if(!useGPUColl)
                test.detectCollisionsCPU();
            else
                test.detectCollisionsGPU();
            test.resolveCollisionsPureShader();
            test.integratePositions(0.1f);

        }
            test.drawSpheres(window);

        window.display();

        sf::sleep(sf::milliseconds(30));
    }
}
