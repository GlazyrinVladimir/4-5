#include <SFML/Graphics.hpp>
#include <string>
#include <windows.h>
#include <iostream>
#include <sstream>
#include "init_struct.h"
#include "draw_viewer.h"

float get_file_size(string path) 
{
	LPCTSTR file = path.c_str();

	DWORD fileSize = 0;

	HANDLE hFile = CreateFile(file,
		GENERIC_READ,
		0,
		NULL,
		OPEN_EXISTING,
		FILE_FLAG_SEQUENTIAL_SCAN,
		NULL);
	if (hFile == INVALID_HANDLE_VALUE)
		cout << "Не удалось открыть файл";

	fileSize = GetFileSize(hFile, NULL);
	if (fileSize == INVALID_FILE_SIZE)
		cout << "Не удалось определить размер файла";

	CloseHandle(hFile);

	return fileSize / 1024.0 / 1024.0;
}

bool is_image(string fileName)
{
	int i;

	if (!strrchr(fileName.c_str(), '.')) 
	{
		return false;
	}
	string expansionArray[] = { "jpg","jpeg","png","gif","bmp" };
	
	for (i = 0;i < 5;i++)
		if (fileName.substr(fileName.find_last_of(".") + 1) == expansionArray[i])
			return true;
	return false;
}

bool directory_exists(const char* FileName)
{
	int Code = GetFileAttributes(FileName);
	return (Code != -1) && (FILE_ATTRIBUTE_DIRECTORY & Code);
}

Files get_file_list(string & oldPath) 
{
	string path = oldPath + string("*");
	Files files;
	files.path = oldPath;
	unsigned long i = 0;

	WIN32_FIND_DATA fileData;
	HANDLE hFile = FindFirstFile(path.c_str(), &fileData);
	if (hFile != INVALID_HANDLE_VALUE)
	{
		do 
		{
			if (!(fileData.dwFileAttributes&FILE_ATTRIBUTE_DIRECTORY)) 
			{
				files.arrSize++;
				if (strlen(fileData.cFileName) > files.nameSize)
					files.nameSize = strlen(fileData.cFileName);
			}

		} while (FindNextFile(hFile, &fileData));

		files.files = new string[files.arrSize];
		i = 0;
		hFile = FindFirstFile(path.c_str(), &fileData);
		do
		{
			if (!(fileData.dwFileAttributes&FILE_ATTRIBUTE_DIRECTORY))
			{
				if (is_image(fileData.cFileName)) {
					files.files[i] = fileData.cFileName;
					i++;
				}
			}

		}
		while (FindNextFile(hFile, &fileData));
		FindClose(hFile);
	}
	files.arrSize = i;
	return files;
}

string init_picture(RenderWindow & window, Files files, Picture *pic, char direction, Sys & sys)
{
	Font font;
	font.loadFromFile("./img/monoid.ttf");
	Text loadText("Loading...", font, 14);
	float textHeight = loadText.getLocalBounds().height;
	float textWidth = loadText.getLocalBounds().width;
	loadText.setOrigin(Vector2f(textWidth * 0.5, textHeight * 0.5));
	loadText.setPosition(sys.windowSize.x * 0.5, sys.windowSize.y * 0.5);
	loadText.setColor(Color(0, 0, 0));
	window.draw(loadText);
	window.display();
	if (!(pic->pictureNumber < 0))
	{
		if (!pic->errorPath)
		{
			delete(pic->sprite);
		}
		Image *image = new Image;
		string path = files.path + files.files[pic->pictureNumber];
		pic->size = get_file_size(path);
		if (pic->size > 10)
		{
			cout << "errorPath (too big size) with: \n" << files.path + files.files[pic->pictureNumber] << '\n';
			pic->errorPath = true;
			sys.error = true;
			sys.errorLetter = "errorPath:too large size";
		}
		if (!(image->loadFromFile(files.path + files.files[pic->pictureNumber])))
		{
			std::cout << "errorPath with: \n" << files.path + files.files[pic->pictureNumber] << '\n';
			pic->errorPath = true;
			sys.error = true;
			sys.errorLetter = "error with loading image in texture";
		}
		else 
		{
			pic->errorPath = false;
		}
		direction = 0;
		if (!pic->errorPath)
		{
			if (direction == 1) 
			{
				pic->nextTexture = pic->texture;
				pic->nextPic = true;
			}
			else if (direction == 2) 
			{
				pic->prevTexture = pic->texture;
				pic->previousPic = true;
			}
		}
		if (direction == 1 && pic->previousPic)
		{
			pic->texture = pic->prevTexture;
			pic->previousPic = false;
		}
		else if (direction == 2 && pic->nextPic)
		{
			pic->texture = pic->nextTexture;
			pic->nextPic = false;
		}
		else
		{
			delete(pic->texture);
			pic->texture = new Texture;
			pic->texture->loadFromImage(*image);
			delete(image);
		}
		pic->zoom = 0;
		pic->sprite = new Sprite; 
		pic->sprite->setPosition(0, 0);
		pic->sprite->setTexture(*(pic->texture));
		pic->sprite->setOrigin(pic->texture->getSize().x * 0.5, pic->texture->getSize().y / 2.0);
		pic->title = string(files.files[pic->pictureNumber]);
		resize_picture(window.getSize(), *pic);
	}
	return pic->title;
}

void resize_picture(Vector2u windowSize, Picture & picture) 
{
	float scale;
	float oldScale = picture.sprite->getScale().x;
	if (picture.texture->getSize().x * oldScale <= windowSize.x || picture.texture->getSize().y * oldScale) 
	{
		if (picture.texture->getSize().x * oldScale - windowSize.x < picture.texture->getSize().y * oldScale - windowSize.y)
		{
			scale = float(windowSize.y) / picture.texture->getSize().y;
		}
		else 
		{
			scale = float(windowSize.x) / picture.texture->getSize().x;
		}
		if (scale > 1)
			scale = 1;
		scale = scale + picture.zoom;
		picture.sprite->setScale(Vector2f(scale, scale));
	}
	else 
	{
		scale = 1 + picture.zoom;
		picture.sprite->setScale(Vector2f(scale, scale));
	}

	picture.left = windowSize.x *0.5f;
	picture.top = windowSize.y *0.5f;
	picture.sprite->setPosition(Vector2f(picture.left, picture.top));

	picture.texture->setSmooth(true);
	picture.texture->setRepeated(false);
}

void zoom(Picture & picture, bool zoom, Vector2u & windowSize) 
{
	float zoomStep = 0.1;

	if (zoom && picture.zoom + zoomStep < 1) 
	{
		picture.zoom = picture.zoom + zoomStep;
	}
	else if (!zoom && picture.zoom > 0) 
	{
		picture.zoom = picture.zoom - zoomStep;
	}
	resize_picture(windowSize, picture);
}

Vector2f get_cursor_position(RenderWindow & window)
{
	Vector2i pixelPos = Mouse::getPosition(window);
	return window.mapPixelToCoords(pixelPos);
}

void drag(Picture & picture, Vector2f & cursor,Sys & sys) 
{
	Vector2f picturePos = picture.sprite->getPosition();
	Vector2f pictureScale = picture.sprite->getScale();
	Vector2u pictureSize = picture.texture->getSize();

	Vector2f pictureShift = Vector2f(cursor.x + picture.shift.x - pictureSize.x * pictureScale.x * 0.5, cursor.y + picture.shift.y - pictureSize.y * pictureScale.y * 0.5);
	bool ex = (pictureShift.x + pictureSize.x * pictureScale.x >= sys.windowSize.x && pictureShift.y + pictureSize.y * pictureScale.y >= sys.windowSize.y);
	if (pictureShift.x <= 0 && pictureShift.y < 0 && ex) 
	{
		picture.sprite->setPosition(Vector2f(cursor.x + picture.shift.x, cursor.y + picture.shift.y));
	}
}

void switch_prev(RenderWindow & window,Picture & picture, Files & files, Sys & sys)
{
	if (picture.pictureNumber == 0)
	{
		picture.pictureNumber = files.arrSize;
	}
	picture.pictureNumber--;
	window.setTitle(init_picture(window, files, &picture, 1, sys));
}

void switch_next(RenderWindow & window, Picture & picture, Files & files, Sys & sys) 
{
	if (picture.pictureNumber + 1 == files.arrSize)
	{
		picture.pictureNumber = 0;
	}
	else 
	{
		picture.pictureNumber++;
	}
	window.setTitle(init_picture(window, files, &picture, 2, sys));
}

void hover(Vector2f & cursor,RectangleShape & fileSizeBg, Sprite & left, Sprite & right) 
{
	if (fileSizeBg.getGlobalBounds().contains(cursor.x, cursor.y)) 
	{
		fileSizeBg.setFillColor(Color(33, 150, 243, 100));
	}
	else 
	{
		fileSizeBg.setFillColor(Color(255, 255, 255, 0));
	}

	if (left.getGlobalBounds().contains(cursor.x, cursor.y)) 
	{
		left.setColor(Color(255, 255, 255, 100));
	}
	else 
	{
		left.setColor(Color(255, 255, 255, 255));
	}

	if (right.getGlobalBounds().contains(cursor.x, cursor.y)) 
	{
		right.setColor(Color(255, 255, 255, 100));
	}
	else 
	{
		right.setColor(Color(255, 255, 255, 255));
	}
}

void events(RenderWindow & window, Sys & sys,Picture & picture, Vector2f & cursor, Files & files, RectangleShape & fileSizeBg, Sprite & left, Sprite & right,Sprite & plus, Sprite & minus) 
{
	Event event;
	while (window.pollEvent(event))
	{
		if (event.type == sf::Event::Closed) 
		{
			delete[] files.files;
			window.close();
		}

		if (event.type == Event::MouseButtonPressed && !picture.drag) 
		{
			if (picture.sprite->getGlobalBounds().contains(cursor.x, cursor.y)) 
			{
				if (event.key.code == Mouse::Left) 
				{
					Vector2f picturePos = picture.sprite->getPosition();
					picture.shift.x = picturePos.x - cursor.x;
					picture.shift.y = picturePos.y - cursor.y;
					picture.drag = true;
				}
			}
		}

		if (event.type == Event::MouseMoved) 
		{
			if (picture.drag) 
			{
				drag(picture,cursor,sys);
			}

			hover(cursor,fileSizeBg, left, right);
		}

		if (event.type == Event::MouseButtonReleased && event.key.code == Mouse::Left) {
			if (picture.drag) {
				picture.drag = false;
			}

			if (left.getGlobalBounds().contains(cursor.x, cursor.y)) {
				switch_prev(window,picture,files, sys);
			}
			else if (right.getGlobalBounds().contains(cursor.x, cursor.y)) {
				switch_next(window, picture, files, sys);
			}
		}

		if (Keyboard::isKeyPressed(Keyboard::Left)) {
			switch_prev(window, picture, files, sys);
		}
		else if (Keyboard::isKeyPressed(Keyboard::Right)) {
			switch_next(window, picture, files, sys);
		}


		if (!sys.error) {
			if (event.type == Event::MouseButtonReleased && event.key.code == Mouse::Left) {
				if (plus.getGlobalBounds().contains(cursor.x, cursor.y)) {
					zoom(picture, true, sys.windowSize);
				}
				else if (minus.getGlobalBounds().contains(cursor.x, cursor.y)) {
					zoom(picture, false, sys.windowSize);
				}
			}

			if (event.type == Event::MouseWheelMoved || Keyboard::isKeyPressed) 
			{
				if (event.mouseWheel.delta == 1 || Keyboard::isKeyPressed(Keyboard::Up) || Keyboard::isKeyPressed(Keyboard::LControl) && Keyboard::isKeyPressed(Keyboard::Equal)) 
				{
					zoom(picture, true, sys.windowSize);
				}
				else if (event.mouseWheel.delta == -1 || Keyboard::isKeyPressed(Keyboard::Down) || Keyboard::isKeyPressed(Keyboard::LControl) && Keyboard::isKeyPressed(Keyboard::Dash)) 
				{
					zoom(picture, false, sys.windowSize);
				}
			}
		}

		if (event.type == Event::Resized) 
		{
			sys.windowSize = window.getSize();
			resize_picture(sys.windowSize, picture);
			minus.setPosition(Vector2f(sys.windowSize.x - 47, sys.windowSize.y - 52));
			plus.setPosition(Vector2f(sys.windowSize.x - 80, sys.windowSize.y - 52));
		}

	} 
}

Sprite create_sprite(const char* FileName) {
	Sprite sprite;
	Texture *texture = new Texture;
	texture->loadFromFile(FileName);
	sprite.setTexture(*texture);
	return sprite;
}

void get_path(Sys & sys,int & argc, char* argv[]) {
	if (argc > 1 && argv[1] != "") {
		if (directory_exists(argv[1])) {
			sys.dirPath = argv[1];
		}
		else {
			sys.error = true;
			sys.errorLetter = "Error with path to dir.";
		}
	}
	else {
		sys.error = true;
		sys.errorLetter = "Error with path to dir. It is empty.";
	}
}

void checking_dir(Sys &sys, Files &files, RenderWindow &window, Picture &picture, int &argc, char* argv[])
{
	sys.dirPath = "C:/Image/";

	get_path(sys, argc, argv);

	files = get_file_list(sys.dirPath);
	sys.windowSize = window.getSize();
	resize_picture(sys.windowSize, picture);

	picture.texture->loadFromFile("./img/transparent.png");
	if (files.arrSize > 0) {
		window.setTitle(init_picture(window, files, &picture, 0, sys));
	}
	else if (!sys.error) {
		sys.error = true;
		sys.errorLetter = "Dir hasn't images.";
	}
}

void start_program(int argc, char* argv[])
{
	Sys sys;

	RenderWindow window(VideoMode(sys.windowSize.x, sys.windowSize.y), sys.windowTitle);
	window.setVerticalSyncEnabled(true);
	View view = View(FloatRect(0, 0, sys.windowSize.x, sys.windowSize.y));
	window.setView(view);

	Clock clock;

	Font font;
	font.loadFromFile("./img/monoid.ttf");

	Text error("ERROR", font, 14);
	error.setColor(Color(255, 0, 0));

	Picture picture;

	Sprite left = create_sprite("./img/left2.png");
	Sprite right = create_sprite("./img/right2.png");

	left.setOrigin(Vector2f(0, left.getGlobalBounds().height));
	right.setOrigin(Vector2f(right.getGlobalBounds().width, right.getGlobalBounds().height));

	Sprite plus = create_sprite("./img/plus.png");
	Sprite minus = create_sprite("./img/minus.png");

	minus.setPosition(Vector2f(sys.windowSize.x - 47, sys.windowSize.y - 52));
	plus.setPosition(Vector2f(sys.windowSize.x - 80, sys.windowSize.y - 52));

	RectangleShape fileSizeBg(Vector2f(100, 40));
	fileSizeBg.setFillColor(Color(33, 150, 243, 0));

	Text fileSizeText("unknow", font, 10);
	fileSizeText.setColor(Color(0, 0, 0));

	Vector2f cursor;

	Files files;

	checking_dir(sys, files, window, picture, argc, argv);

	while (window.isOpen())
	{
		window.clear(Color(255, 255, 255));
		view = View(FloatRect(0, 0, sys.windowSize.x, sys.windowSize.y));
		window.setView(view);
		if (!picture.errorPath)
			sys.error = false;
		cursor = get_cursor_position(window);

		events(window, sys, picture, cursor, files, fileSizeBg, left, right, plus, minus);

		draw_elements(window, sys, picture, left, right, minus, plus, fileSizeBg, fileSizeText, error);
		window.display();
		sys.windowSize = window.getSize();
	}
}



int main(int argc, char* argv[])
{		
	start_program(argc, argv);	
}