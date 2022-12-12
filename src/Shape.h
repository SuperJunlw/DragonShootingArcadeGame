#pragma once

#include "ofMain.h"

class Shape {
public:
	Shape() {}
	virtual void draw() {

		// draw a box by defaultd if not overridden
		//
		ofPushMatrix();
		ofMultMatrix(getMatrix());
		ofDrawBox(defaultSize);
		ofPopMatrix();
	}
	virtual bool inside(glm::vec3 p) {
		return false;
	}
	glm::mat4 getMatrix() {
		glm::mat4 trans = glm::translate(glm::mat4(1.0), glm::vec3(pos));
		glm::mat4 rot = glm::rotate(glm::mat4(1.0), glm::radians(rotation), glm::vec3(0, 0, 1));
		glm::mat4 scal = glm::scale(glm::vec3(scale, scale, scale));
		return (trans * rot * scal);
	}
	glm::vec3 pos;
	float rotation = 0.0;    // degrees 
	float scale = 1;
	float defaultSize = 20.0;

	vector<glm::vec3> verts;
};
