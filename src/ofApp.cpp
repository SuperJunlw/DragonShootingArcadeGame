#include "ofApp.h"

void AgentList::add(Agent s) {
	agents.push_back(s);
}

void AgentList::remove(int i) {
	agents.erase(agents.begin() + i);
}

void AgentList::update() {

	if (agents.size() == 0) return;
	vector<Agent>::iterator a = agents.begin();
	vector<Agent>::iterator tmp;

	// check which agents have exceed their lifespan and delete
	// from list.  When deleting multiple objects from a vector while
	// traversing at the same time, use an iterator.
	//
	while (a != agents.end()) {
		if ((a->lifespan != -1 && a->age() > a->lifespan) || a->isCollide) {
			tmp = agents.erase(a);
			a = tmp;
		}
		else a++;
	}
}

void AgentList::draw() {
	for (int i = 0; i < agents.size(); i++) {
		agents[i].draw();
	}
}

//check a ray particle hits any of the agents
bool AgentList::rayCollide(Sprite* ray, ParticleEmitter* explosionEmitter, ofSoundPlayer sound) {
	vector<Agent>::iterator a = agents.begin();
	vector<Agent>::iterator tmp;
	bool hits = false;
	while (a != agents.end()) {
		float d = glm::distance(ray->pos, a->pos);
		if (d < (ray->getRadius() + a->getRadius())) {
			sound.setVolume(0.1f);
			sound.play();
			explosionEmitter->setPosition(ofVec3f(a->pos.x, a->pos.y, 0));
			hits = true;
			tmp = agents.erase(a);
			a = tmp;
		}
		else
			a++;
	}
	return hits;
}

//check an agent hits the player
bool AgentList::playerCollide(Player* player, ParticleEmitter* emitter, ofSoundPlayer sound, ofSoundPlayer sound2) {
	vector<Agent>::iterator a = agents.begin();
	vector<Agent>::iterator tmp;
	bool hit = false;
	while (a != agents.end()) {
		float d = glm::distance(player->pos, a->pos);
		if (d < (player->getRadius() + a->getRadius())) {
			sound.setVolume(0.05f);
			sound.play();
			sound2.play();
			emitter->setPosition(ofVec3f(a->pos.x, a->pos.y, 0));
			player->nEnergy--;
			tmp = agents.erase(a);
			a = tmp;
			hit = true;
		}
		else
			a++;
	}
	return hit;
}

void Player::draw() {

	ofSetColor(ofColor::white);
	ofPushMatrix();
	ofMultMatrix(getMatrix());
	image.draw(-image.getWidth() / 2.0, -image.getHeight() / 2.0);
	ofPopMatrix();
}

void AgentEmitter::update(int nAgent, Player *player, Emitter *ray) {
	if (!started) return;

	float time = ofGetElapsedTimeMillis();

	if ((time - lastSpawned) > (1000.0 / rate)) {

		// call virtual to spawn a new sprite
		//
		for (int i = 0; i < groupSize; i++)
			if (agentSys->agents.size() < nAgent) {
				spawnSprite();
			}

		lastSpawned = time;
	}
	
	for (int i = 0; i < agentSys->agents.size(); i++) {
		agentSys->agents[i].updateAgentDir(player);
		moveSprite(&agentSys->agents[i], player);
	}

	// update sprite list
//
	if (agentSys->agents.size() == 0) return;
	vector<Agent>::iterator a = agentSys->agents.begin();
	vector<Agent>::iterator tmp;

	// check which sprites have exceed their lifespan and delete
	// from list.  When deleting multiple objects from a vector while
	// traversing at the same time, use an iterator.
	//
	while (a != agentSys->agents.end()) {
		if ((a->lifespan != -1 && a->age() > a->lifespan) || a->isCollide) {
			tmp = agentSys->agents.erase(a);
			a = tmp;
		}
		else a++;
	}
}
//--------------------------------------------------------------
void ofApp::setup() {
	ofSetVerticalSync(true);
	player = new Player();
	player->pos = glm::vec3(ofGetWindowWidth() / 2.0 + 300, ofGetWindowHeight() / 2.0, 0);

	//load the images
	if (!background.load("images/background2.png")) {
		cout << "Can't load image" << endl;
		ofExit();
	}
	if (!player->image.load("images/dragon2.png")) {
		cout << "Can't load image" << endl;
		ofExit();
	}
	if (!agentImage.load("images/Agent.png")) {
		cout << "Can't load image" << endl;
		ofExit();
	}
	if (!fireBallImage.load("images/FireBallImage.png")) {
		cout << "Can't load image" << endl;
		ofExit();
	}

	player->speed = 500;
	player->scale = 1;

	font.load("arial.ttf", 32);

	//load and setting up sounds
	dragonFlying.load("sounds/dragonFlying.mp3");
	dragonFlying.setLoop(true);
	fireball.load("sounds/fireball2.mp3");
	fireball.setLoop(true);
	fireball.setVolume(0.2f);
	explosion.load("sounds/explosion2.mp3");
	explosion.setVolume(0.1f);
	dragonPain.load("sounds/dragonPain.mp3");
	dragonPain.setVolume(0.2f);
	
	gui.setup();
	gui.add(sp.setup("Speed", player->speed, 1, 2000));
	gui.add(sc.setup("Scale", player->scale, 0.5, 10));
	gui.add(numEng.setup("Energy Level", 10, 1, 20));
	gui.add(numAgent.setup("Number of Agent", 5 , 1, 40));
	gui.add(rate.setup("rate", 1, 1, 10));
	gui.add(groupNum.setup("Agent spawn group size", 1, 1, 5));
	gui.add(life.setup("Agent life time", 5, 1, 20));
	gui.add(rayRate.setup("Fire ball rate", 1, 1, 5));
	gui.add(shImage.setup("Show images", true));
	bHide = false;

	player->nEnergy = numEng;

	rayEmitter = new Emitter();
	rayEmitter->pos = player->pos;
	rayEmitter->drawable = false;
	rayEmitter->setRate(rayRate);
	rayEmitter->setLifespan(3000);
	rayEmitter->setVelocity(player->header() * 500);

	emitter = new AgentEmitter();
	explosionEmitter = new ParticleEmitter();
	collideExplosionEmitter = new ParticleEmitter();

	emitter->pos = glm::vec3(ofRandom(0, ofGetWindowWidth()), ofRandom(0, ofGetWindowHeight()), 0);
	emitter->drawable = false;
	emitter->setChildImage(agentImage, shImage);
	emitter->setGroupSize(groupNum);

	rayEmitter->setChildImage(fireBallImage);

	//explosion when fireball hits an agent
	gravityForce = new GravityForce(ofVec3f(0, 60, 0));
	radialForce = new ImpulseRadialForce(100);
	explosionEmitter->sys->addForce(gravityForce);
	explosionEmitter->sys->addForce(radialForce);
	explosionEmitter->setVelocity(ofVec3f(40, 5, 10));
	explosionEmitter->setOneShot(true);
	explosionEmitter->setEmitterType(RadialEmitter);
	explosionEmitter->setGroupSize(200);
	explosionEmitter->setLifespan(1.5);
	explosionEmitter->setRate(1);
	explosionEmitter->setParticleRadius(1.5);

	//explosion when agent collide with player
	collideGravityForce = new GravityForce(ofVec3f(0, 30, 0));
	collidRadialForce = new ImpulseRadialForce(100);
	collideExplosionEmitter->sys->addForce(gravityForce);
	collideExplosionEmitter->sys->addForce(radialForce);
	collideExplosionEmitter->setVelocity(ofVec3f(20, 5, 10));
	collideExplosionEmitter->setOneShot(true);
	collideExplosionEmitter->setEmitterType(RadialEmitter);
	collideExplosionEmitter->setGroupSize(150);
	collideExplosionEmitter->setLifespan(1.0);
	collideExplosionEmitter->setRate(1);
	collideExplosionEmitter->setParticleRadius(1.5);
}

//--------------------------------------------------------------
void ofApp::update() {
	ofSeedRandom();
	player->speed = sp;
	player->scale = sc;
	player->showImage = shImage;
	emitter->setRate(rate);
	emitter->setLifespan(life * 1000);    // convert to milliseconds 
	emitter->setChildImage(agentImage, shImage);
	emitter->setGroupSize(groupNum);
	rayEmitter->setVelocity(player->header() * 1200);
	rayEmitter->setRate(rayRate);
	rayEmitter->pos = player->pos;
	rayEmitter->rotation = player->rotation;
	if (shImage)
		rayEmitter->setChildImage(fireBallImage);
	else
		rayEmitter->haveChildImage = false;

	if (startScreen) { //when player in start screen
		player->nEnergy = numEng;
		elapedTime = 0;
	}
	if (gameScreen) { //when player started the game and in game screen
		emitter->pos = glm::vec3(ofRandom(0, ofGetWindowWidth()), ofRandom(0, ofGetWindowHeight()), 0);
		emitter->update(numAgent, player, rayEmitter);
		rayEmitter->update();
		
		checkCollisions();
		explosionEmitter->update();
		checkPlayerCollisions();
		collideExplosionEmitter->update();
		elapedTime = ofGetElapsedTimef() - gameStartTime;

		//check for arrow keys press
		if (keymap[OF_KEY_UP]) {
			//check if player move across the screen boundraies
			if (player->pos.x >= ofGetWindowWidth())
				player->pos.x -= 1;
			else if (player->pos.x <= 0)
				player->pos.x += 1;
			else if (player->pos.y >= ofGetWindowHeight())
				player->pos.y -= 1;
			else if (player->pos.y <= 0)
				player->pos.y += 1;
			else 
				player->pos += player->header() * (player->speed / ofGetFrameRate());

			if(!dragonFlying.isPlaying())
				dragonFlying.play();
		}

		if (keymap[OF_KEY_DOWN]) {
			//check if player move across the screen boundraies
			if (player->pos.x >= ofGetWindowWidth())
				player->pos.x -= 1;
			else if (player->pos.x <= 0)
				player->pos.x += 1;
			else if (player->pos.y >= ofGetWindowHeight())
				player->pos.y -= 1;
			else if (player->pos.y <= 0)
				player->pos.y += 1;
			else
				player->pos -= player->header() * (player->speed / ofGetFrameRate());

			if (!dragonFlying.isPlaying())
				dragonFlying.play();
		}

		if (keymap[OF_KEY_LEFT])
			player->rotation -= rotDeg;

		if (keymap[OF_KEY_RIGHT])
			player->rotation += rotDeg;
	}
	if (player->nEnergy < 1) { //when player die and in game over screen.
		overScreen = true;
		gameScreen = false;
		emitter->agentSys->agents.clear();
		emitter->stop();
		rayEmitter->stop();
		rayEmitter->sys->sprites.clear();
		fireball.stop();
		player->pos = glm::vec3(ofGetWindowWidth() / 2.0, ofGetWindowHeight() / 2.0, 0);
		player->rotation = 0;
	}
}

//--------------------------------------------------------------
void ofApp::draw() {
	ofSetColor(ofColor::white);
	background.resize(ofGetWindowWidth(), ofGetWindowHeight());
	background.draw(0,0);

	if (startScreen) {
		ofSetColor(ofColor::white);
		font.drawString(startText, ofGetWindowWidth() / 2 - font.stringWidth(startText) / 2,
			ofGetWindowHeight() / 2 - font.stringHeight(startText) / 2);
	}
	else if (gameScreen) {
		if (player->showImage)
			player->draw();
		else
		{
			ofSetColor(ofColor::red);
			player->TriangleShape::draw();
		}
		if (rayEmitter->started)
			rayEmitter->draw();
		
		emitter->draw();
		ofSetColor(ofColor::orange);
		explosionEmitter->draw();

		ofSetColor(ofColor::orangeRed);
		collideExplosionEmitter->draw();
	}
	else if (overScreen) {
		ofSetColor(ofColor::white);
		font.drawString("\t\t\t\tYou dead... \nGame Time: " + std::to_string(elapedTime) + " seconds\nPress 'b' to return to start screen.",
			ofGetWindowWidth() / 2 - font.stringWidth(startText) / 2 - 20,
			ofGetWindowHeight() / 2 - font.stringHeight(startText) / 2);
	}

	string str;
	str += "Player Energy: " + std::to_string(player->nEnergy) + "/" + std::to_string(numEng) +
		"   Elapsed Time: " + std::to_string(elapedTime) +
		"   Frame Rate: " + std::to_string(ofGetFrameRate());
	ofSetColor(ofColor::white);
	ofDrawBitmapString(str, ofGetWindowWidth() - 550, 15);

	if (!bHide) gui.draw();
}

void ofApp::checkCollisions() {
	for (int i = 0; i < rayEmitter->sys->sprites.size(); i++) {
		//float d = glm::distance(ray->sys->sprites[i].pos, pos);
		if (emitter->agentSys->rayCollide(&rayEmitter->sys->sprites[i], explosionEmitter, explosion)) {
			if (player->nEnergy < numEng)
				player->nEnergy++;

			//remove the ray particle if it hits an agent;
			rayEmitter->sys->remove(i);

			//if an agent was hit, start the explosion
			explosionEmitter->sys->reset();
			explosionEmitter->start();
		}
	}
}

//check if player collisions with agents
void ofApp::checkPlayerCollisions() {
	if (emitter->agentSys->playerCollide(player, collideExplosionEmitter, explosion, dragonPain)) {
		collideExplosionEmitter->sys->reset();
		collideExplosionEmitter->start();
	}
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key) {
	keymap[key] = true;

	if(keymap['h'])
		bHide = !bHide;

	if (keymap['b']) {
		if (overScreen) {
			overScreen = false;
			startScreen = true;
		}
	}	
	if (keymap[' ']) {
		if (startScreen) {
			emitter->start();
			gameStartTime = ofGetElapsedTimef();
			startScreen = false;
			gameScreen = true;
		}
	}

	if (keymap['s']) {//'s' key to shoot
		if (gameScreen) {
			if (!rayEmitter->started) {
				rayEmitter->start();
				fireball.play();
			}
			else {
				rayEmitter->stop();
				rayEmitter->sys->sprites.clear();
				fireball.stop();
			}
		}
	}
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key) {
	keymap[key] = false;
	dragonFlying.stop();
}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y) {
}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button) {
}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button) {
}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button) {
}

//--------------------------------------------------------------
void ofApp::mouseEntered(int x, int y) {

}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y) {

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h) {

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg) {

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo) {

}
