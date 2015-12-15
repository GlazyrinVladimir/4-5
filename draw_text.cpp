#include <SFML/Graphics.hpp>
#include "init_struct.h"

void draw_text_bg(unsigned int width, unsigned int height, Texture *sandboxT, Texture *text, unsigned int opacity) 
{
	
	Sprite spriteRadius;
	Texture radius;
	radius.loadFromFile("./img/radius.png");
	spriteRadius.setTexture(radius);
	spriteRadius.setColor(Color(255, 255, 255, opacity));
	
	Vector2u radiusSize = radius.getSize();
	width = width + radiusSize.x;
	height = height + radiusSize.y;

	RenderTexture sandbox;
	sandbox.create(width, height);
	sandboxT->create(width, height);
	
	if (height < radiusSize.y * 2 || width < radiusSize.x * 2)
	{
		float scale;
		if (radiusSize.y - height > radiusSize.x - width) 
		{
			scale = height / radiusSize.y / 2;
		}
		else 
		{
			scale = height / radiusSize.x / 2;
		}
		spriteRadius.setScale(Vector2f(scale, scale));
	}

	RectangleShape bg(Vector2f(width - radiusSize.x * 3, height - radiusSize.y * 3));
	bg.setFillColor(Color(0, 0, 0, opacity));

	Sprite newText;
	newText.setTexture(*text);
	newText.setPosition(radiusSize.x + 2, radiusSize.y + 2);
	sandbox.draw(newText);

	sandboxT->create(width, height);
	*sandboxT = sandbox.getTexture();
}

void draw_text(string strText, Font & font, Texture *sandboxT, Vector2u & windowSize) 
{

	const unsigned int fontSize = 14;

	RenderTexture *result = new RenderTexture;

	Text text = Text("", font, fontSize);
	text.setString(strText);
	result->create(strText.length() * 10 + 2, fontSize * 10 + 2);
	result->draw(text);
	Vector2u boxSize = result->getSize();

	Texture *buf = new Texture;
	*buf = result->getTexture();
	delete(result);

	Texture *buf2 = new Texture;
	*sandboxT = *buf;

	draw_text_bg(boxSize.x, boxSize.y, buf2, buf, 80);
	delete(buf);
	*sandboxT = *buf2;
	delete(buf2);
}
