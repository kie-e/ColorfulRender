// Colorful Render - Copyright (c) 2025 Kie-e
// SFML (Simple and Fast Multimedia Library) - Copyright (c) Laurent Gomila
//
// All the files of Colorful Render and SFML are licensed under zlib license.

#include <SFML/System.hpp>
#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>

class ColorfulTexture {
public:
	sf::Texture main, additional;

	ColorfulTexture() {}
	ColorfulTexture(sf::Texture& main, sf::Texture& additional) : main(main), additional(additional) {}
	ColorfulTexture(ColorfulTexture&& other) {
		main = other.main;
		additional = other.additional;
	}

	bool loadFromFile(std::string name) {
		return main.loadFromFile(name + ".png") && additional.loadFromFile(name + "_.png");
	}

	bool loadFromFile(std::string nameMain, std::string nameAdditional) {
		return main.loadFromFile(nameMain) && additional.loadFromFile(nameAdditional);
	}
};

class ColorfulSprite {
public:
	sf::Sprite* main;
	sf::Sprite* additional;

	ColorfulSprite(ColorfulTexture& texture) {
		main = new sf::Sprite(texture.main);
		additional = new sf::Sprite(texture.additional);
	}

	void setTexture(ColorfulTexture& texture) {
		main->setTexture(texture.main);
		additional->setTexture(texture.additional);
	}

	// Call after each transformation
	void update() {
		additional->setOrigin(main->getOrigin());
		additional->setPosition(main->getPosition());
		additional->setRotation(main->getRotation());
		additional->setScale(main->getScale());
	}

	~ColorfulSprite() {
		delete main, additional;
	}
};

class ColorfulWindow : public sf::RenderWindow {
	sf::RenderTexture buffer8, buffer2; // for main and additional images

	const static std::string colorfulShaderCode, blackShaderCode;
	sf::Shader colorfulShader, blackShader;

	sf::Sprite* sprite;
	int frame = 0;

public:

	ColorfulWindow() {}

	bool init(sf::Vector2u windowSize, const std::string& title, sf::State state = sf::State::Windowed) {
		bool success = true;

		sf::RenderWindow::create(sf::VideoMode(windowSize), title, state);
		sprite = new sf::Sprite(buffer8.getTexture(), sf::IntRect({ 0, 0 }, { (int)windowSize.x, (int)windowSize.y }));

		success = success && buffer8.resize(windowSize);
		success = success && buffer2.resize(windowSize);

		success = success && colorfulShader.loadFromMemory(colorfulShaderCode, sf::Shader::Type::Fragment);
		colorfulShader.setUniform("base", sf::Shader::CurrentTexture); // or buffer8.getTexture()
		colorfulShader.setUniform("addition", buffer2.getTexture());

		success = success && blackShader.loadFromMemory(blackShaderCode, sf::Shader::Type::Fragment);
		blackShader.setUniform("texture", sf::Shader::CurrentTexture);
		
		return success;
	}

	void clear(const sf::Color color = sf::Color()) {
		sf::RenderWindow::clear(color);
		buffer8.clear(color);
		buffer2.clear();
	}

	// Adapted sf::RenderWindow draw functions vvvvv

	void draw(const sf::Drawable& drawable, const sf::RenderStates& states = sf::RenderStates::Default) {
		buffer8.draw(drawable, states);

		sf::RenderStates states2 = states;
		states2.blendMode = sf::BlendAlpha;
		states2.shader = &blackShader;
		buffer2.draw(drawable, states2); // Clear ColorfulRender info at the place where non-ColorfulRender object will be drawn
	}

	void draw(const sf::Vertex* vertices,
		std::size_t         vertexCount,
		sf::PrimitiveType       type,
		const sf::RenderStates& states = sf::RenderStates::Default) {

		buffer8.draw(vertices, vertexCount, type, states);

		sf::RenderStates states2 = states;
		states2.blendMode = sf::BlendAlpha;
		states2.shader = &blackShader;
		buffer2.draw(vertices, vertexCount, type, states2);
	}

	void draw(const sf::VertexBuffer& vertexBuffer, const sf::RenderStates& states = sf::RenderStates::Default) {
		buffer8.draw(vertexBuffer, states);

		sf::RenderStates states2 = states;
		states2.blendMode = sf::BlendAlpha;
		states2.shader = &blackShader;
		buffer2.draw(vertexBuffer, states2);
	}

	void draw(const sf::VertexBuffer& vertexBuffer,
		std::size_t         firstVertex,
		std::size_t         vertexCount,
		const sf::RenderStates& states = sf::RenderStates::Default) {

		buffer8.draw(vertexBuffer, firstVertex, vertexCount, states);

		sf::RenderStates states2 = states;
		states2.blendMode = sf::BlendAlpha;
		states2.shader = &blackShader;
		buffer2.draw(vertexBuffer, firstVertex, vertexCount, states2);
	}

	// Draw functions that use ColorfulRender vvvvv

	void draw(const sf::Drawable& main, const sf::Drawable& additional, const sf::RenderStates& states = sf::RenderStates::Default) {
		buffer8.draw(main, states);

		sf::RenderStates states2 = states;
		states2.blendMode = sf::BlendNone;
		buffer2.draw(additional, states2);
	}

	void draw(ColorfulSprite& sprite, const sf::RenderStates& states = sf::RenderStates::Default) {
		buffer8.draw(*sprite.main, states);

		sf::RenderStates states2 = states;
		states2.blendMode = sf::BlendNone;
		buffer2.draw(*sprite.additional, states2);
	}

	void display(bool doColorfulRender = true) {
		frame++;

		if (frame == 4)
			frame = 0;

		colorfulShader.setUniform("frame", frame);

		buffer8.display();
		buffer2.display();

		if (doColorfulRender)
			sf::RenderWindow::draw(*sprite, sf::RenderStates(&colorfulShader));
		else
			sf::RenderWindow::draw(*sprite);

		sf::RenderWindow::display();
	}

	~ColorfulWindow() {
		sf::RenderWindow::close();
		delete sprite;
	}
};

const std::string ColorfulWindow::colorfulShaderCode = R"(
uniform sampler2D base;
uniform sampler2D addition;
uniform int frame;

void main()
{
	vec4 inputBaseColor = texture2D(base, gl_TexCoord[0].xy);
	vec4 inputAdditionalColor = texture2D(addition, gl_TexCoord[0].xy);
	vec4 outColor;

	for (int i = 0; i < 3; i++) {
		outColor[i] = inputBaseColor[i] +
			((frame == 0) * (inputAdditionalColor[i] >= 0.25) * (inputAdditionalColor[i] < 0.5) +
			(frame % 2) * (inputAdditionalColor[i] >= 0.5) * (inputAdditionalColor[i] < 0.75) +
			(frame != 0) * (inputAdditionalColor[i] >= 0.75)) / 256.0;
	}

	outColor.a = inputBaseColor.a;

	gl_FragColor = gl_Color * outColor;
}
)";

const std::string ColorfulWindow::blackShaderCode = R"(
uniform sampler2D texture;

void main()
{
	vec4 inputColor = texture2D(texture, gl_TexCoord[0].xy);
	vec4 outColor = vec4(0, 0, 0, inputColor.a);
	gl_FragColor = gl_Color * outColor;
}
)";

// 0.0 - 0.25: mode 0 (no color added)
// 0.25 - 0.5: mode 1 (+1 to color once every four frames)
// 0.5 - 0.75: mode 2 (+1 to color twice every four frames)
// 0.75 - 1.0: mode 3 (+1 to color three times every four frames)