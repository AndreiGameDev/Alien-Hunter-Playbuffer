#define PLAY_IMPLEMENTATION
#define PLAY_USING_GAMEOBJECT_MANAGER
#include "Play.h"


enum GameObjectTypes {
	TYPE_Null = -1,
	TYPE_Player,
	Type_PlayerProjectile,
	Type_Enemy,
	Type_Destroyed
};
struct GameState
{
	float timer = 0;
	int spriteId = 0;
};
GameState gameState;
int _displayWidth = 240;
int _displayHeight = 240;
int _displayScale = 5;
int _clampScreen = 15;
int score = 0;
float playerSpeed = 1;
float projectileSpeed = 2;
float animatorSpeed = 0.1f;

// The entry point for a PlayBuffer program
void MainGameEntry(PLAY_IGNORE_COMMAND_LINE)
{
	Play::CreateManager(_displayWidth, _displayHeight, _displayScale);
	Play::CreateGameObject(TYPE_Player, { _displayWidth / 2, _displayHeight - 10 }, 16, "Player_Spaceship_Idle"); // Player sprite
	//	Play::CreateGameObject(2, { 16, 16 }, 2.5, "testSprite"); // Player laser 
	//	Play::CreateGameObject(3, { 16,16 }, 5, "testSprite"); // Enemy sprite
	Play::CentreAllSpriteOrigins();
}
float clip(float n, float lower, float upper) {
	return std::max(lower, std::min(n, upper));
}
void UpdatePlayer() {
	GameObject& player = Play::GetGameObjectByType(TYPE_Player);
	player.pos += player.velocity;
	player.pos.x = clip(player.pos.x, _clampScreen, _displayWidth - _clampScreen);

	if (Play::KeyDown(0x44)) { // Right
		Play::SetSprite(player, "Player_Spaceship_Moving", animatorSpeed);
		player.velocity.x = playerSpeed;
		Play::DrawDebugText({ _displayWidth / 2, _displayHeight / 2 }, "Right");
	}
	else if (Play::KeyDown(0x41)) { // Left
		Play::SetSprite(player, "Player_Spaceship_Moving", animatorSpeed);
		player.velocity.x = -playerSpeed;
		Play::DrawDebugText({ _displayWidth / 2, _displayHeight / 2 }, "Left");
	}
	else { // No input
		Play::SetSprite(player, "Player_Spaceship_Idle", animatorSpeed);
		player.velocity.x = 0;
		Play::DrawDebugText({ _displayWidth / 2, _displayHeight / 2 }, "Idle");
	}
	if (Play::KeyPressed(0x01)) { // Mouse 1
		Vector2D RightPos = Vector2D(player.pos.x + 6, player.pos.y - 6);
		Vector2D LeftPos = Vector2D(player.pos.x - 6, player.pos.y - 6);
		int idLeftProjectile = Play::CreateGameObject(Type_PlayerProjectile, RightPos, 2, "PlayerLaser");
		int idRightProjectile = Play::CreateGameObject(Type_PlayerProjectile, LeftPos, 2, "PlayerLaser");
		Play::GetGameObject(idLeftProjectile).velocity.y = -projectileSpeed;
		Play::GetGameObject(idRightProjectile).velocity.y = -projectileSpeed;
	}
	// Update position of the player
	Play::UpdateGameObject(player);

	Play::DrawCircle({ _displayWidth / 2, 40 }, 3, Play::cWhite); // Simulating where the point of spawn would be 
	// Draw the update position of the player;
	Play::DrawObject(player);
}

void UpdateProjectiles()
{
	std::vector<int> vPlayerProjectiles = Play::CollectGameObjectIDsByType(Type_PlayerProjectile);
	for (int id_laser : vPlayerProjectiles) {
		GameObject& obj_laser = Play::GetGameObject(id_laser);
		bool hasCollided = false;

		Play::UpdateGameObject(obj_laser);
		Play::DrawObject(obj_laser);

		if (!Play::IsVisible(obj_laser) || hasCollided) {
			Play::DestroyGameObject(id_laser);
		}
	}
}
//void UpdateDestroyed()
//{
//	std::vector<int> vDead = Play::CollectGameObjectIDsByType(Type_Destroyed);
//	for (int id_dead : vDead)
//	{
//		GameObject& obj_dead = Play::GetGameObject(id_dead);
//		obj_dead.animSpeed = 0.2f;
//		Play::UpdateGameObject(obj_dead);
//		if (obj_dead.frame % 2)
//			Play::DrawObjectRotated(obj_dead, (10 - obj_dead.frame) / 10.0f);
//		if (!Play::IsVisible(obj_dead) || obj_dead.frame >= 10)
//			Play::DestroyGameObject(id_dead);
//	}
//}
// Called by PlayBuffer every frame (60 times a second!)
bool MainGameUpdate(float elapsedTime)
{
	Play::ClearDrawingBuffer(Play::cMagenta);

	UpdatePlayer();
	UpdateProjectiles();
	Play::PresentDrawingBuffer();
	return Play::KeyDown(VK_ESCAPE);
}

// Gets called once when the player quits the game 
int MainGameExit(void)
{
	Play::DestroyManager();
	return PLAY_OK;
}

