#pragma once

#include "ofMain.h"
#include "Shape.h"
#include "Sprite.h"
#include "ofApp.h"

//
//  Manages all Sprites in a system.  You can create multiple systems
//
class SpriteList {
public:
	void add(Sprite);
	void remove(int);
	void update();
	void draw();
	vector<Sprite> sprites;
};


//  General purpose Emitter class for emitting sprites
//  This works similar to a Particle emitter
//
class Emitter : public Shape {
public:
	Emitter();
	void init();
	void draw();
	void start();
	void stop();
	void setLifespan(float);
	void setVelocity(const glm::vec3 v);
	void setChildImage(ofImage);
	void setImage(ofImage);
	void setRate(float);
	void update();

	// virtuals - can overloaded
	virtual void moveSprite(Sprite*);
	virtual void spawnSprite();

	SpriteList* sys;
	float rate;
	glm::vec3 velocity;
	float lifespan;
	bool started;
	float lastSpawned;
	ofImage childImage;
	ofImage image;
	bool drawable;
	bool haveChildImage;
	bool haveImage;
	float width, height;
};


