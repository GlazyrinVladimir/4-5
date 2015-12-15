#pragma once
using namespace sf;
using namespace std;

void draw_file_size_line(RenderWindow & window, RectangleShape & fileSizeBg, Text & fileSizeText, Sys & sys, float & pictureSize)
{
	fileSizeBg.setSize(Vector2f(sys.windowSize.x, 15));
	fileSizeBg.setPosition(Vector2f(0, sys.windowSize.y - 15));
	fileSizeText.setPosition(Vector2f(5, sys.windowSize.y - 15));
	fileSizeText.setString(to_string(pictureSize) + " mb");
	window.draw(fileSizeBg);
	window.draw(fileSizeText);
}

void draw_error(RenderWindow & window, Text & error, Sys & sys)
{
	error.setString(sys.errorLetter);
	float textHeight = error.getLocalBounds().height;
	float textWidth = error.getLocalBounds().width;
	error.setOrigin(Vector2f(textWidth * 0.5, textHeight * 0.5));
	error.setPosition(sys.windowSize.x * 0.5, sys.windowSize.y * 0.5);
	window.draw(error);
}

void draw_buttons(RenderWindow & window, Sys & sys, Sprite & left, Sprite & right)
{
	left.setPosition(Vector2f(0, sys.windowSize.y * 0.5));
	right.setPosition(Vector2f(sys.windowSize.x, sys.windowSize.y * 0.5));
	window.draw(left);
	window.draw(right);
}

void draw_elements(RenderWindow & window, Sys & sys, Picture & picture, Sprite & left, Sprite & right, Sprite & minus, Sprite & plus, RectangleShape & fileSizeBg, Text & fileSizeText, Text & error) {
	if (!sys.error) {
		picture.sprite->setTexture((*picture.texture));
		window.draw((*picture.sprite));

		draw_file_size_line(window, fileSizeBg, fileSizeText, sys, picture.size);
	}

	draw_buttons(window, sys, left, right);

	if (sys.error) {
		draw_error(window, error, sys);
	}
	else {
		window.draw(minus);
		window.draw(plus);
	}
}
