// Colorful Render - Copyright(c) 2025 Kie - e
// SFML (Simple and Fast Multimedia Library) - Copyright (c) Laurent Gomila
//
// All the files of Colorful Render, including this example, and SFML are licensed under zlib license.

#include "ColorfulRender.h"
#include <iostream>

const sf::Vector2u windowSize = { 1000, 500 };
const unsigned int FPS = 60;

int main() {
	ColorfulWindow window;

	if (window.init(windowSize, "ColorfulRender application example")) { // Creating window as we usually do with SFML
		std::cout << "ColorfulWindow created successifully." << std::endl;
	}
	else {
		std::cout << "Some error occured during initialization of ColorfulWindow..." << std::endl;
		std::cout << "Probably, your graphics card does not support shaders." << std::endl;
		return 1;
	}
	window.setFramerateLimit(FPS);

	ColorfulTexture CRtexture;
	sf::Texture texture;

	if (CRtexture.loadFromFile("assets/cr") && texture.loadFromFile("assets/no_cr.png")) { // Loading cr.png and cr_.png for ColorfulTexture & no-cr for sf::Texture from assets/ folder
		std::cout << "ColorfulTexture & sf::Texture created successifully." << std::endl;

		if (texture.generateMipmap())
			std::cout << "Mipmap of sf::Texture was generated correctly." << std::endl;
		else
			std::cout << "Warning: Could not generate mipmap." << std::endl;
	}
	else {
		std::cout << "Some error occured during textures loading..." << std::endl;
		std::cout << "Check if the application can find and open all the required files." << std::endl;
		return 1;
	}

	ColorfulSprite CRsprite(CRtexture); // Creating sprites to render
	sf::Sprite sprite(texture);

	sprite.scale({ 0.25, 0.25 });
	sprite.move({ 760, 365 });

	bool doColorfulRender = false;
	bool reducedFramerate = false;

	bool isRunning = true;
	while (isRunning) {
		while (const std::optional event = window.pollEvent())
		{
			if (event->is<sf::Event::Closed>())
			{
				isRunning = false;
			}

			if (auto* key = event->getIf<sf::Event::KeyPressed>()) {
				switch (key->scancode) {

				// Turning on / off 10-bit emulation
				case sf::Keyboard::Scan::Space:
					doColorfulRender = !doColorfulRender;
					std::cout << "Colorful render is " << (doColorfulRender ? "ON" : "OFF") << '.' << std::endl;
					break;

				// Reducing framerate to demonstrate how 10-bit emulation works / Returning framerate to normal to see the effect of emulation
				case sf::Keyboard::Scan::F:
					reducedFramerate = !reducedFramerate;
					std::cout << "Framerate is " << (reducedFramerate ? "REDUCED" : "NORMAL") << '.' << std::endl;

					if (reducedFramerate)
						window.setFramerateLimit(4);
					else
						window.setFramerateLimit(FPS);
					break;

				// Moving sprite
				// sprite.update(); is necessary after any chain of transformations on sprite.main 

				case sf::Keyboard::Scan::Up:
					CRsprite.main->move({ 0, 100 });
					CRsprite.update();
					break;

				case sf::Keyboard::Scan::Down:
					CRsprite.main->move({ 0, -100 });
					CRsprite.update();
					break;

				case sf::Keyboard::Scan::Left:
					CRsprite.main->move({ 100, 0 });
					CRsprite.update();
					break;

				case sf::Keyboard::Scan::Right:
					CRsprite.main->move({ -100, 0 });
					CRsprite.update();
					break;
				}
			}
		}

		window.clear();
		window.draw(CRsprite);
		window.draw(sprite);
		window.display(doColorfulRender);
	}

	return 0;
}