#include <SFML/Graphics.hpp>
#include <SFML/Window/Keyboard.hpp>
#include <algorithm>
#include <cmath>
#include <optional>
#include <vector>
#include <string>

int main()
{
    // Window settings
    const unsigned int WIDTH = 1000;
    const unsigned int HEIGHT = 700;

    sf::RenderWindow window(
        sf::VideoMode({ WIDTH, HEIGHT }),
        "Lab 5 - Variant 15 - Gravity Simulation"
    );

    window.setFramerateLimit(60);

    // Central body
    const sf::Vector2f center(500.f, 350.f);
    const float planetRadius = 30.f;
    const float particleRadius = 7.f;

    sf::CircleShape planet(planetRadius);
    planet.setOrigin({ planetRadius, planetRadius });
    planet.setPosition(center);
    planet.setFillColor(sf::Color(255, 190, 50));

    // Moving particle
    sf::CircleShape particle(particleRadius);
    particle.setOrigin({ particleRadius, particleRadius });
    particle.setFillColor(sf::Color::Cyan);

    // Physical parameters
    const float GM = 2000000.f;
    const float dt = 0.01f;

    float initialRadius = 200.f;
    float initialSpeed = 100.f;

    sf::Vector2f position;
    sf::Vector2f velocity;

    std::vector<sf::Vector2f> trajectory;

    bool paused = false;
    bool collision = false;

    // Reset simulation
    auto reset = [&]()
        {
            position = {
                center.x + initialRadius,
                center.y
            };

            velocity = { 0.f, -initialSpeed };

            trajectory.clear();
            trajectory.push_back(position);

            paused = false;
            collision = false;
        };

    // Update window title with parameters
    auto updateTitle = [&]()
        {
            std::string title =
                "Lab 5 | Variant 15 | Speed: " +
                std::to_string(static_cast<int>(initialSpeed)) +
                " | Radius: " +
                std::to_string(static_cast<int>(initialRadius)) +
                " | SPACE: Pause | R: Reset | Arrows: Change";

            if (paused)
                title += " | PAUSED";

            if (collision)
                title += " | COLLISION";

            window.setTitle(title);
        };

    reset();
    updateTitle();

    // Main simulation loop
    while (window.isOpen())
    {
        while (const std::optional event = window.pollEvent())
        {
            if (event->is<sf::Event::Closed>())
            {
                window.close();
            }

            if (const auto* key =
                event->getIf<sf::Event::KeyPressed>())
            {
                if (key->code == sf::Keyboard::Key::Escape)
                {
                    window.close();
                }

                // Pause / Resume
                if (key->code == sf::Keyboard::Key::Space)
                {
                    if (!collision)
                        paused = !paused;
                }

                // Restart
                if (key->code == sf::Keyboard::Key::R)
                {
                    reset();
                }

                // Increase initial speed
                if (key->code == sf::Keyboard::Key::Up)
                {
                    initialSpeed += 10.f;
                    reset();
                }

                // Decrease initial speed
                if (key->code == sf::Keyboard::Key::Down)
                {
                    initialSpeed =
                        std::max(10.f, initialSpeed - 10.f);

                    reset();
                }

                // Increase initial distance
                if (key->code == sf::Keyboard::Key::Right)
                {
                    initialRadius =
                        std::min(300.f, initialRadius + 20.f);

                    reset();
                }

                // Decrease initial distance
                if (key->code == sf::Keyboard::Key::Left)
                {
                    initialRadius =
                        std::max(60.f, initialRadius - 20.f);

                    reset();
                }

                updateTitle();
            }
        }

        // Physics simulation
        if (!paused && !collision)
        {
            for (int i = 0; i < 5; ++i)
            {
                float dx = center.x - position.x;
                float dy = center.y - position.y;

                float r2 = dx * dx + dy * dy;
                float r = std::sqrt(r2);

                // Collision with central body
                if (r <= planetRadius + particleRadius)
                {
                    collision = true;
                    paused = true;
                    updateTitle();
                    break;
                }

                // Gravitational acceleration
                float acceleration = GM / r2;

                float ax = acceleration * dx / r;
                float ay = acceleration * dy / r;

                // Update velocity
                velocity.x += ax * dt;
                velocity.y += ay * dt;

                // Update position
                position.x += velocity.x * dt;
                position.y += velocity.y * dt;

                trajectory.push_back(position);
            }

            // Limit trajectory size
            if (trajectory.size() > 10000)
            {
                trajectory.erase(
                    trajectory.begin(),
                    trajectory.begin() +
                    (trajectory.size() - 10000)
                );
            }
        }

        particle.setPosition(position);

        // Drawing
        window.clear(sf::Color(10, 10, 30));

        // Draw trajectory
        if (trajectory.size() >= 2)
        {
            sf::VertexArray path(
                sf::PrimitiveType::LineStrip,
                trajectory.size()
            );

            for (std::size_t i = 0;
                i < trajectory.size(); ++i)
            {
                path[i].position = trajectory[i];
                path[i].color =
                    sf::Color(100, 180, 255);
            }

            window.draw(path);
        }

        // Draw central body and particle
        window.draw(planet);
        window.draw(particle);

        window.display();
    }

    return 0;
}