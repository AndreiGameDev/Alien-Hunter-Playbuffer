#define PLAY_IMPLEMENTATION
#define PLAY_USING_GAMEOBJECT_MANAGER
#include "Play.h"

int DISPLAY_WIDTH = 800;
int DISPLAY_HEIGHT = 800;
int DISPLAY_SCALE = 1;
int score = 0;
float speed = 100;

// The entry point for a PlayBuffer program
void MainGameEntry( PLAY_IGNORE_COMMAND_LINE )
{
	Play::CreateManager( DISPLAY_WIDTH, DISPLAY_HEIGHT, DISPLAY_SCALE );
	Play::CreateGameObject(1, { DISPLAY_WIDTH / 2, DISPLAY_HEIGHT - 100 }, 5, "testsprite"); // Player sprite
//	Play::CreateGameObject(2, { 16, 16 }, 2.5, "testSprite"); // Player laser 
//	Play::CreateGameObject(3, { 16,16 }, 5, "testSprite"); // Enemy sprite
	Play::CentreAllSpriteOrigins();
	
}

void UpdatePlayer() {
	GameObject& player = Play::GetGameObjectByType(1);
	Vector2D newVelocity = player.velocity;

	
	if (Play::KeyDown(0x44)) {
		newVelocity.x = 1 * speed;
		Play::DrawDebugText({ DISPLAY_WIDTH / 2, DISPLAY_HEIGHT / 2 }, "Right");
	}
	else if(Play::KeyDown(0x41)){
		newVelocity.x = -1 * speed;
		Play::DrawDebugText({ DISPLAY_WIDTH / 2, DISPLAY_HEIGHT / 2 }, "Left");
	}

	// Update position of the player
	Play::UpdateGameObject(player);
	
	// Draw the update position of the player;
	Play::DrawObject(player);
}

// Called by PlayBuffer every frame (60 times a second!)
bool MainGameUpdate( float elapsedTime )
{
	Play::ClearDrawingBuffer( Play::cBlack );

	UpdatePlayer();

	Play::PresentDrawingBuffer();
	return Play::KeyDown( VK_ESCAPE );
}

// Gets called once when the player quits the game 
int MainGameExit( void )
{
	Play::DestroyManager();
	return PLAY_OK;
}

