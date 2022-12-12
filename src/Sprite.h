#pragma once

#include "Shape.h"



class TriangleShape : public Shape {
public:
	TriangleShape() {
		// default data
		set(glm::vec3(20, 20, 0), glm::vec3(0, -20, 0), glm::vec3(-20, 20, 0));
	}
	virtual void draw();
	virtual bool inside(const glm::vec3 p);

	void set(glm::vec3 v1, glm::vec3 v2, glm::vec3 v3) {
		verts.clear();
		verts.push_back(v1);
		verts.push_back(v2);
		verts.push_back(v3);
	}
};

class Sprite : public TriangleShape {
public:
	// some functions for highlighting when selected
	//
	void draw() {
		if (bShowImage) {
			ofPushMatrix();
			ofSetColor(ofColor::white);
			ofMultMatrix(getMatrix());
			spriteImage.draw(-spriteImage.getWidth() / 2, -spriteImage.getHeight() / 2.0);
			ofPopMatrix();
		}
		else
		{
			if (bHighlight) ofSetColor(ofColor::white);
			else ofSetColor(ofColor::yellow);
			TriangleShape::draw();
		}
	}

	float age() {
		return (ofGetElapsedTimeMillis() - birthtime);
	}

	void setImage(ofImage img) {
		spriteImage = img;
		bShowImage = true;
		width = img.getWidth();
		height = img.getHeight();
	}

	float getRadius() {
		if (bShowImage)
			return ((spriteImage.getWidth() / 2) - 5) * scale;
		else
			return 10 * scale;
	}

	bool inside(const glm::vec3 p);

	void setSelected(bool state) { bSelected = state; }
	void setHighlight(bool state) { bHighlight = state; }
	bool isSelected() { return bSelected; }
	bool isHighlight() { return bHighlight; }


	glm::vec3 heading() {
		return (glm::rotate(glm::mat4(1.0), glm::radians(rotation), glm::vec3(0, 0, 1)) * glm::vec4(0, 1, 0, 1));
	}

	bool bHighlight = false;
	bool bSelected = false;
	bool bShowImage = false;

	glm::vec3 velocity = glm::vec3(0, 0, 0);
	float birthtime = 0; // elapsed time in ms
	float lifespan = -1;  //  time in ms
	string name = "UnammedSprite";
	float width = 40;
	float height = 40;

	ofImage spriteImage;
};

