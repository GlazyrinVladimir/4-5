#pragma once
using namespace sf;
using namespace std;

struct Files 
{
	string *files = new string[0];
	unsigned int nameSize = 0;
	unsigned int arrSize = 0;
	string path;
};

struct Sys 
{
	bool error = false;
	string errorLetter = "unknown error";
	string windowTitle = "loading...";
	bool isLoading = false;
	Vector2u windowSize = Vector2u(800,600);
	string dirPath = "";
};



struct Picture 
{
	string title = "Error";
	string name;
	float left;
	float top;
	Texture *texture = new Texture;
	Texture *prevTexture = new Texture;
	Texture *nextTexture = new Texture;
	bool previousPic = false;
	bool nextPic = false;
	bool errorPath = true;
	bool drag = false;
	Sprite *sprite = new Sprite;
	int pictureNumber = 0;
	float zoom = 0;
	Vector2f shift;
	float size;
};

struct Load_pic 
{
	Vector2u windowSize;
	Files files; 
	Picture *pic; 
	char diraction;
};

void resize_picture(Vector2u windowSize, Picture & picture);