#define GLM_ENABLE_EXPERIMENTAL
#include "gameLayer.h"
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include "platformInput.h"
#include "imgui.h"
#include <iostream>
#include <sstream>
#include "imfilebrowser.h"
#include <gl2d/gl2d.h>
#include <platformTools.h>
#include <vector>

// Game objects
#include "objects.h"
#include <random>
#include <memory>

struct GameData
{
    glm::vec2 rectPos;

    GameData() : rectPos(500, 500){}
}gameData;

gl2d::Renderer2D renderer;

template<class T>
struct ObjectPool {
	std::vector<std::shared_ptr<T>> objects;
};

ObjectPool<GameObjects::Object> frames;
ObjectPool<GameObjects::Block> blocks;
ObjectPool<GameObjects::Block> blockchain;
std::shared_ptr<GameObjects::Block> currentBlock;
GameObjects::Block nextBlock;
int blocksize = 10;

/**
 * @brief 	1 (single)	40
			2 (double)	100
			3 (triple)	300
			4 (tetris)	1200
 * 
 */
int score = 0;

#include <unordered_set>

std::vector<int> randomZahlenArray(int min, int max, int size) {
    std::vector<int> verteilung {};
    std::unordered_set<int> generated_numbers;

    std::random_device rd; // benutzt hardwarebasierte Zufallsquelle, falls vorhanden
    std::mt19937 gen(rd()); // initialisieren des Mersenne Twister Zufallszahlengenerators
    std::uniform_int_distribution<> distr(min, max);

    while (verteilung.size() < size) {
        int random_number = distr(gen);
        if (generated_numbers.find(random_number) == generated_numbers.end()) {
            verteilung.emplace_back(random_number);
            generated_numbers.insert(random_number);
        }
    }
    return verteilung;
}

gl2d::Color4f HTMLToColor(const std::string& html) {
    std::string color = html.substr(1); // Überspringe das erste Zeichen (#)
    int r, g, b;
    std::istringstream(color.substr(0, 2)) >> std::hex >> r;
    std::istringstream(color.substr(2, 2)) >> std::hex >> g;
    std::istringstream(color.substr(4, 2)) >> std::hex >> b;
    return gl2d::Color4f(r / 255.0f, g / 255.0f, b / 255.0f, 1.0f);
}

struct ColorPair {
	gl2d::Color4f inner;
	gl2d::Color4f outer;
	ColorPair(gl2d::Color4f i, gl2d::Color4f o ) : inner(i), outer(o) {}
};

gl2d::Color4f randomColor() {
	 // Initialisieren eines Zufallszahlengenerators mit einem seed
    std::random_device rd; // benutzt hardwarebasierte Zufallsquelle, falls vorhanden
    std::mt19937 gen(rd()); // initialisieren des Mersenne Twister Zufallszahlengenerators

    // Definieren des Bereichs der Zufallszahlen (z.B. 1 bis 100)
    std::uniform_int_distribution<> distr(0, 12);

    // Generieren einer Zufallszahl
    int random_number = distr(gen);
	switch( random_number ) {
		case 0: return HTMLToColor("#ff5733"); // Leuchtendes Orange
		case 1: return HTMLToColor("#ffbd33"); // Sonnengelb
		case 2: return HTMLToColor("#75ff33"); // Limettengrün
		case 3: return HTMLToColor("#33ff57"); // Neon Grün
		case 4: return HTMLToColor("#33ffbd"); // Türkis
		case 5: return HTMLToColor("#33d1ff"); // Helles Blau
		case 6: return HTMLToColor("#3375ff"); // Kobaltblau
		case 7: return HTMLToColor("#7533ff"); // Violett
		case 8: return HTMLToColor("#bd33ff"); // Magenta
		case 9: return HTMLToColor("#ff33d1"); // Fuchsia
		case 10: return HTMLToColor("#ff3375"); // Helles Rot
		case 11: return HTMLToColor("#ff3347"); // Karminrot
		case 12: return HTMLToColor("#ff7633"); // Mandarinenorange
		default: return HTMLToColor("#ffffff"); // Weiß (Neutral)
	}
}
void createNewBlockChain() {
	// Initialisieren eines Zufallszahlengenerators mit einem seed
	for ( int i = 0; i < 500; i++ ) {
		std::vector<int> verteilung = randomZahlenArray(0,6, 7);
		int random = randomZahlenArray(0,12, 13)[0];
		for ( auto zahl : verteilung ) {
			glm::vec2 blockpos = {600, 270};
			gl2d::Color4f col = randomColor();
			blockchain.objects.emplace_back(std::make_shared<GameObjects::Block>( blockpos, blocksize, col, zahl));
		}
		
	}
	currentBlock = blockchain.objects[0];
	renderer.clearScreen();
	if (blockchain.objects[1]->id == 0)
		nextBlock = GameObjects::Block( {790, 600}, blocksize, blockchain.objects[1]->color_, blockchain.objects[1]->id);
	else
		nextBlock = GameObjects::Block( {810, 600}, blocksize, blockchain.objects[1]->color_, blockchain.objects[1]->id);
}

void initScene( int width, int height ) {
	glm::vec2 boardpos = {600, 600};
	// 10 blocks wide, 20 blocks high
	int w_game = 300;
	GameObjects::Frame* board = new GameObjects::Frame( boardpos,w_game, w_game*2 );
	glm::vec2 framepos = {850, 600};
	GameObjects::Frame* nextBlockFrame = new GameObjects::Frame( framepos, 150, 150 );
	blocksize = w_game * 0.1;
	//renderer.renderText({850, 400}, "Next Block",,gl2d::Font Colors_White, 2.f, 4.0);

	frames.objects.emplace_back(board);
	frames.objects.emplace_back(nextBlockFrame);
	createNewBlockChain();
}

#include <chrono>
#include <thread>


// ebenen sind 0, 1, 2, 3 usw.
void letBlocksFall(std::array<int, 20> ebenenMove )  {
	int startEbene = 870;
	for ( auto& bloecke : blocks.objects ) {
		for( auto& oneblocks : bloecke->shape) {
			int one_ebene = (startEbene - oneblocks.y)/30;
			if ( one_ebene >= 0 && one_ebene <= 19 ) {
				if ( ebenenMove[one_ebene] != 0) 
					oneblocks.move(0, blocksize * ebenenMove[one_ebene]);
			}
		}
	}
	/**
	 * @brief (single)	40
			2 (double)	100
			3 (triple)	300
			4 (tetris)	1200

	 * 
	 */
	int gap = 0;
	for ( auto elem : ebenenMove) {
		if ( elem > gap )
			gap = elem;
	}
	switch(gap) {
		case 0: break;
		case 1: score += 40; break;
		case 2: score += 100; break;
		case 3: score += 300; break;
		case 4: score += 1200; break;
	}
}

#include <memory>
#include <algorithm>
#include <vector>
#include <array>
using shape_ptr = std::shared_ptr<std::vector<GameObjects::OneBlock>>;

void checkTenBlocks() {
    int startEbene = 870;
    std::array< std::vector<std::shared_ptr<GameObjects::Block> > , 20> ebenen;
    // Populate the ebenen array
    for (auto& elem : blocks.objects) { // Iterate through each game object
        for (auto& oneblock : elem->shape) { // Iterate through each block in the shape
            int index = (startEbene - oneblock.y) / 30; // Calculate the level index
            if (index >= 0 && index < ebenen.size()) { // Check if the index is within bounds
                ebenen[index].emplace_back(elem); // Add the block to the corresponding level
            }
        }
    }
	std::array<int, 20> downmove_perebene = {0};
    // Check and modify levels with 10 or more blocks
    for (int i = 0; i < ebenen.size(); i++) {
        if (ebenen[i].size() >= 10) { // Check if the level has 10 or more blocks
			for ( int j = 0; j < downmove_perebene.size(); j++ ) {
				if ( j > i ) 
					downmove_perebene[j] += 1;
			} 
            for (auto& block : ebenen[i]) { // Iterate through blocks at this level
                for (auto it = block->shape.begin(); it != block->shape.end(); ) {
                    if (it->y == startEbene - 30 * i) { // If the block's y-coordinate matches the level
                        it = block->shape.erase(it); // Remove the block from the shape
                    } else {
                        ++it; // Move to the next block
                    }
                }
            }
        }
    }
	letBlocksFall(downmove_perebene);
}






bool initGame()
{
	//initializing stuff for the renderer
	gl2d::init();
	renderer.create();
	//loading the saved data. Loading an entire structure like this makes saving game data very easy.
	platform::readEntireFile(RESOURCES_PATH "gameData.data", &gameData, sizeof(GameData));
	gameData.rectPos = { platform::getFrameBufferSizeX() * 0.5, platform::getFrameBufferSizeY() * 0.5 }; 
	initScene(platform::getFrameBufferSizeX(), platform::getFrameBufferSizeY());
	return true;
}


//IMPORTANT NOTICE, IF YOU WANT TO SHIP THE GAME TO ANOTHER PC READ THE README.MD IN THE GITHUB
//https://github.com/meemknight/cmakeSetup
//OR THE INSTRUCTION IN THE CMAKE FILE.
//YOU HAVE TO CHANGE A FLAG IN THE CMAKE SO THAT RESOURCES_PATH POINTS TO RELATIVE PATHS
//BECAUSE OF SOME CMAKE PROGBLMS, RESOURCES_PATH IS SET TO BE ABSOLUTE DURING PRODUCTION FOR MAKING IT EASIER.

void setNewCurrent() {
	blocks.objects.emplace_back( currentBlock );
	if ( blockchain.objects.size() > 1 ) {
		blockchain.objects.erase(blockchain.objects.begin());
		if ( blockchain.objects.size() >= 2 ) {
			if (blockchain.objects[1]->id == 0)
				nextBlock = GameObjects::Block( {790, 600}, blocksize, blockchain.objects[1]->color_, blockchain.objects[1]->id);
			else
				nextBlock = GameObjects::Block( {810, 600}, blocksize, blockchain.objects[1]->color_, blockchain.objects[1]->id);
		}
		currentBlock = blockchain.objects[0];
		renderer.clearScreen();
	}else {
		createNewBlockChain();
	}
}

bool collidesInNextMove(int x_bew, int y_bew) {
	GameObjects::Block neueBeweung = *currentBlock;
	neueBeweung.move(x_bew, y_bew);
	bool collidesWithBlock = false;
	for ( auto& other : blocks.objects ) {
		if (neueBeweung.collidesOtherBlock(other, renderer))
			collidesWithBlock = true;
	}
	return collidesWithBlock;
}

bool collidesInNextRotation() {
	GameObjects::Block neueBeweung = *currentBlock;
	neueBeweung.rotate();
	bool collidesWithBlock = false;
	for ( auto& other : blocks.objects ) {
		if (neueBeweung.collidesOtherBlock(other, renderer))
			collidesWithBlock = true;
	}
	if (neueBeweung.collidesLeftWall(420) || neueBeweung.collidesRightWall(750) || neueBeweung.collidesBottom(900))
		return true;
	return collidesWithBlock;
}

float down_counter = 0;
float move_counter = 0;
void renderBlocks( float deltaTime ) {
	bool movedown = false;
	if ( down_counter >= 1 ) {
		movedown = true;
		down_counter = 0;
	}
	// render frames
	for (auto& f : frames.objects )
		f->update(renderer,true);
	for ( auto& b : blocks.objects )  
		b->update(renderer, true);
	nextBlock.update(renderer, true);
	// 
	if ( currentBlock != nullptr ) {
		if ( movedown ) {
			if ( !collidesInNextMove(0, blocksize) && !currentBlock->collidesBottom(875)) 
				currentBlock->move(0, blocksize);
			else 
				setNewCurrent();
		}
		currentBlock->update(renderer, true);
	}
}

void playerMove() { 
	
	if (platform::isButtonHeld(platform::Button::Left))
	{
		if ( !collidesInNextMove(-blocksize, 0) && !currentBlock->collidesLeftWall(450) ) {
			currentBlock->move(-blocksize, 0);
		}	
	}
	if (platform::isButtonHeld(platform::Button::Right))
	{ 
		if ( !collidesInNextMove(blocksize, 0) && !currentBlock->collidesRightWall(720)) {
			currentBlock->move(blocksize, 0);
		}	
	}
	if (platform::isButtonHeld(platform::Button::Up))
	{
		// turn
	}
	if (platform::isButtonHeld(platform::Button::Down))
	{
		if ( !collidesInNextMove(0, blocksize) && !currentBlock->collidesBottom(870) ) {
			currentBlock->move(0, blocksize);
		}
	}
}

void playerRotate() {
	if (platform::isButtonReleased(platform::Button::Up)) {
		if ( !collidesInNextRotation() )
			currentBlock->rotate();
	}
}
void nextText() {
	ImGui::Begin("Next");

    // Set text color to red
    ImVec4 white = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
    ImGui::PushStyleColor(ImGuiCol_Text, white);

    // Set font size to big
	ImGui::SetWindowPos({780, 450});
    ImGui::SetWindowFontScale(2.0f);
	ImGui::SetWindowSize({150, 80});

    // Display the score
	/**
     * @brief blockTypes are determined by its index:
     * 0 = I-Block
     * 1 = J-Block
     * 2 = L-Block
     * 3 = O-Black
     * 4 = S-Block
     * 5 = T-Block
     * 6 = Z-Block
     * 
     */
	switch(nextBlock.id ) {
		case 0: ImGui::Text("I-Block"); break;
		case 1: ImGui::Text("J-Block"); break;
		case 2: ImGui::Text("L-Block"); break;
		case 3: ImGui::Text("O-Block"); break;
		case 4: ImGui::Text("S-Block"); break;
		case 5: ImGui::Text("T-Block"); break;
		case 6: ImGui::Text("Z-Block"); break;
	}
    

    // Restore default text color
    ImGui::PopStyleColor();

    ImGui::End();
}

void displayScoreboardText() {
    ImGui::Begin("Score");

    // Set text color to red
    ImVec4 white = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
    ImGui::PushStyleColor(ImGuiCol_Text, white);

    // Set font size to big
	ImGui::SetWindowPos({460, 210});
    ImGui::SetWindowFontScale(3.0f);
	ImGui::SetWindowSize({300, 100});

    // Display the score
    ImGui::Text("%d", score);

    // Restore default text color
    ImGui::PopStyleColor();

    ImGui::End();
}
bool gameLogic(float deltaTime)
{
#pragma region init stuff
	int w = 0; int h = 0;
	w = platform::getFrameBufferSizeX(); //window w
	h = platform::getFrameBufferSizeY(); //window h
	glViewport(0, 0, w, h);
	glClear(GL_COLOR_BUFFER_BIT); //clear screen

	renderer.updateWindowMetrics(w, h);
#pragma endregion
	// Sorgt dafuer dass unsere objekte nicht out of screen gehen
	down_counter += deltaTime;
	playerRotate();
	if ( move_counter >= 0.05 ) {
		playerMove();
		move_counter = 0;
	}else {
		move_counter += deltaTime;
	}
	checkTenBlocks();
	renderBlocks(deltaTime);
	// draws to the scene
	renderer.flush();
	
	displayScoreboardText();
	nextText();

	return true;
#pragma endregion

}

//This function might not be be called if the program is forced closed
void closeGame()
{

	//saved the data.
	platform::writeEntireFile(RESOURCES_PATH "gameData.data", &gameData, sizeof(GameData));

}
