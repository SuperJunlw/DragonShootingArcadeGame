#pragma once

#include "ofMain.h"
#include "ofxGui.h"
#include "Emitter.h"
#include "Shape.h"
#include "Particle.h"
#include "ParticleEmitter.h"
#include "TransformObject.h"

//Player class
class Player : public TriangleShape {
public:

	Player() {}

	void draw();

	//header function
	glm::vec3 header()
	{
		glm::vec3 d = glm::vec3(-cos(ofDegToRad(rotation + 90)), -sin(ofDegToRad(rotation + 90)), 0);
		return glm::normalize(d);
	}

	float getRadius() {
		if (showImage)
			return ((image.getWidth() / 2) - 5) * scale;
		else
			return 10 * scale;
	}

	ofImage image;
	bool showImage = false;
	float speed = 0;
	int nEnergy = 0;
};

class Agent : public Sprite {
public:
	Agent() : Sprite() {}
	
	void updateAgentDir(Player *player) {
		glm::vec3 diff = glm::normalize(player->pos - pos);
		float deg = glm::orientedAngle(glm::vec2(header()), glm::vec2(diff));
		rotation += deg;
	}

	glm::vec3 header() {
		glm::vec3 d = glm::vec3(-cos(ofDegToRad(rotation + 90)), -sin(ofDegToRad(rotation + 90)), 0);
		return glm::normalize(d);
	}

	float getRadius() {
		if(bShowImage)
			return ((spriteImage.getWidth() / 2) - 5) * scale;
		else
			return 10 * scale;
	}

	void integrade(glm::vec3 pPos) {
		float dt = 1.0 / ofGetFrameRate();
		pos += (velocity * dt);

		glm::vec3 diff = pPos - pos;
		ofVec3f attractForce = glm::normalize(diff) * 500;
		ofVec3f accel = attractForce;
		accel += (forces * 1.0 / mass);
		velocity += accel * dt;
		velocity *= damping;
	}
	
	bool isCollide = false;
	float damping = .99;
	int mass = 1;
	ofVec3f forces;
};

class AgentList {
public:
	void add(Agent);
	void remove(int);
	void update();
	void draw();
	bool rayCollide(Sprite* ray, ParticleEmitter* explosionEmitter, ofSoundPlayer sound);
	bool playerCollide(Player* player, ParticleEmitter* emitter, ofSoundPlayer sound, ofSoundPlayer sound2);
	vector<Agent> agents;
};


class AgentEmitter : public Emitter {
public:
	AgentEmitter() : Emitter() {
		agentSys = new AgentList;
	}
	void moveSprite(Agent* agent, Player *player) {
		agent->integrade(player->pos);
	}

	void spawnSprite() {
		Agent agent;
		if (haveChildImage) agent.setImage(childImage);
		agent.lifespan = lifespan;
		agent.pos = pos;
		agent.rotation = ofRandom(0, 360);
		agent.birthtime = ofGetElapsedTimeMillis();
		agent.forces.set(0, 0, 0);
		agentSys->add(agent);
	}

	void draw() {
		Emitter::draw();
		agentSys->draw();
	}

	void setChildImage(ofImage img, bool showImage) {
		childImage = img;
		if (showImage)
			haveChildImage = true;
		else
			haveChildImage = false;
	}

	void setGroupSize(int s) {
		groupSize = s;
	}

	void update(int nAgent, Player *player, Emitter *ray);

	int groupSize;

	AgentList* agentSys;
};



class ofApp : public ofBaseApp {

public:
	void setup();
	void update();
	void draw();

	void keyPressed(int key);
	void keyReleased(int key);
	void mouseMoved(int x, int y);
	void mouseDragged(int x, int y, int button);
	void mousePressed(int x, int y, int button);
	void mouseReleased(int x, int y, int button);
	void mouseEntered(int x, int y);
	void mouseExited(int x, int y);
	void windowResized(int w, int h);
	void dragEvent(ofDragInfo dragInfo);
	void gotMessage(ofMessage msg);
	void ofApp::checkCollisions();
	void ofApp::checkPlayerCollisions();

	Player *player = NULL;
	AgentEmitter *emitter = NULL;
	Emitter* rayEmitter = NULL;

	float rotDeg = 6.0;
	map<int, bool> keymap;

	bool bHide;
	ofxFloatSlider sp;
	ofxFloatSlider sc;
	ofxIntSlider rate;
	ofxIntSlider life;
	ofxVec3Slider velocity;
	ofxIntSlider numAgent;
	ofxIntSlider numEng;
	ofxIntSlider agentEng;
	ofxIntSlider groupNum;
	ofxIntSlider rayRate;
	ofxToggle shImage;

	ofxPanel gui;
	ofImage background;
	ofImage agentImage;
	ofImage fireBallImage;

	ofTrueTypeFont font;
	string startText = "\t\t\tDragon Pursuit\n\nPress 'space' to start the game!";
	string overText = "\t\t\tYou dead... \nPress 'b' to return to start screen.";

	bool startScreen = true;
	bool overScreen = false;
	bool gameScreen = false;
	float elapedTime = 0;
	float gameStartTime = 0;

	ParticleEmitter *explosionEmitter;
	GravityForce* gravityForce;
	ImpulseRadialForce* radialForce;

	ParticleEmitter* collideExplosionEmitter;
	GravityForce* collideGravityForce;
	ImpulseRadialForce* collidRadialForce;

	ofSoundPlayer dragonFlying;
	ofSoundPlayer dragonPain;
	ofSoundPlayer fireball;
	ofSoundPlayer explosion;
};

