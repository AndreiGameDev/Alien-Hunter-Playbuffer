#define PLAY_IMPLEMENTATION
#define PLAY_USING_GAMEOBJECT_MANAGER
#include "Play.h"

enum GameObjectTypes {
	TYPE_Null = -1,
	TYPE_Player,
	Type_PlayerProjectile,
	Type_Destroyed,
	Enemy_Speed,
	Enemy_Heavy,
	Enemy_Normal,
};

enum PlayState {
	State_MainMenu,
	State_Play,
	State_Pause
};

PlayState currentPlayState;

struct EnemyManager {
	float spawnTimer = 0;
	float spawnRate = .4f;
	int enemiesSpawned = 0;
	int enemiesToSpawn = 10;
};
EnemyManager enemyManager;

int _displayWidth = 240;
int _displayHeight = 240;
int _displayScale = 3;
int _clampScreen = 15;
int score = 0;
float playerSpeed = 1;
float projectileSpeed = 2;
float animatorSpeed = 0.1f;

float enemySpeed = 2;

// The entry point for a PlayBuffer program
void MainGameEntry(PLAY_IGNORE_COMMAND_LINE)
{
	Play::CreateManager(_displayWidth, _displayHeight, _displayScale);
	Play::CreateGameObject(TYPE_Player, { _displayWidth / 2, _displayHeight - 10 }, 16, "Player_Spaceship_Idle"); // Player sprite
	Play::CentreAllSpriteOrigins();
}


void UpdateScore() {
	Play::DrawDebugText({ 5, 14 }, std::string("Score:" + std::to_string(score)).c_str(), Play::cWhite, false);
}

void UpdatePlayer() {
	GameObject& player = Play::GetGameObjectByType(TYPE_Player);
	bool canShoot = true;
	player.pos += player.velocity;
	if (Play::IsLeavingDisplayArea(player, Play::HORIZONTAL)) {
		player.pos.x = player.oldPos.x;
	}

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
	if (Play::KeyPressed(0x01) && canShoot) { // Mouse 1
		Vector2D RightPos = Vector2D(player.pos.x + 6, player.pos.y - 6);
		Vector2D LeftPos = Vector2D(player.pos.x - 6, player.pos.y - 6);
		int idLeftProjectile = Play::CreateGameObject(Type_PlayerProjectile, RightPos, 2, "PlayerLaser");
		int idRightProjectile = Play::CreateGameObject(Type_PlayerProjectile, LeftPos, 2, "PlayerLaser");
		Play::GetGameObject(idLeftProjectile).velocity.y = -projectileSpeed;
		Play::GetGameObject(idRightProjectile).velocity.y = -projectileSpeed;
		canShoot = false;
	}
	// Update position of the player
	Play::UpdateGameObject(player);

	Play::DrawCircle({ _displayWidth / 2, 40 }, 3, Play::cWhite); // Simulating where the point of spawn would be 
	// Draw the update position of the player;
	Play::DrawObject(player);
}

void EnemySpawner() {
	Vector2D rightSpawnPos = { _displayWidth, 40 };
	
	if (enemyManager.spawnTimer > enemyManager.spawnRate && enemyManager.enemiesToSpawn > 0) {
		enemyManager.spawnTimer = 0;
		enemyManager.enemiesToSpawn--;
		enemyManager.enemiesSpawned++;
		int randomEnemy = Play::RandomRollRange(1, 10);
		if (randomEnemy < 6) {
			int idEnemy = Play::CreateGameObject(Enemy_Normal, rightSpawnPos, 16, "Enemy1");
			Play::GetGameObject(idEnemy).velocity.x = -enemySpeed;
		}
		else if (randomEnemy > 6 && randomEnemy < 8) {
			int idEnemy = Play::CreateGameObject(Enemy_Speed, rightSpawnPos, 16, "Enemy2");
			Play::GetGameObject(idEnemy).velocity.x = -enemySpeed;
		}
		else {
			int idEnemy = Play::CreateGameObject(Enemy_Heavy, rightSpawnPos, 16, "Enemy3");
			Play::GetGameObject(idEnemy).velocity.x = -enemySpeed;
		}
		
	}
	
}

void UpdateEnemies() {
	std::vector<int> vEnemiesNormal = Play::CollectGameObjectIDsByType(Enemy_Normal);
	std::vector<int> vEnemiesSpeed = Play::CollectGameObjectIDsByType(Enemy_Speed);
	std::vector<int> vEnemiesHeavy = Play::CollectGameObjectIDsByType(Enemy_Heavy);

	std::vector<int> vEnemies;
	vEnemies.reserve(vEnemiesNormal.size() + vEnemiesSpeed.size() + vEnemiesHeavy.size());

	// Append the vectors using std::copy
	std::copy(vEnemiesNormal.begin(), vEnemiesNormal.end(), std::back_inserter(vEnemies));
	std::copy(vEnemiesSpeed.begin(), vEnemiesSpeed.end(), std::back_inserter(vEnemies));
	std::copy(vEnemiesHeavy.begin(), vEnemiesHeavy.end(), std::back_inserter(vEnemies));

	for (int _idEnemy : vEnemies) {
		GameObject& obj_enemy = Play::GetGameObject(_idEnemy);
		if (Play::IsLeavingDisplayArea(obj_enemy, Play::HORIZONTAL)) {
			obj_enemy.pos.x = obj_enemy.pos.x;
			obj_enemy.pos.y += 20;
			if (obj_enemy.velocity.x == enemySpeed) {
				obj_enemy.velocity.x = -enemySpeed;
			}
			else {
				obj_enemy.velocity.x = enemySpeed;
			}
		}
		Play::UpdateGameObject(obj_enemy);
		Play::DrawObject(obj_enemy);
	}
}

void UpdateProjectiles()
{
	std::vector<int> vPlayerProjectiles = Play::CollectGameObjectIDsByType(Type_PlayerProjectile);
	std::vector<int> vEnemiesNormal = Play::CollectGameObjectIDsByType(Enemy_Normal);
	std::vector<int> vEnemiesSpeed = Play::CollectGameObjectIDsByType(Enemy_Speed);
	std::vector<int> vEnemiesHeavy = Play::CollectGameObjectIDsByType(Enemy_Heavy);

	std::vector<int> vEnemies;
	vEnemies.reserve(vEnemiesNormal.size() + vEnemiesSpeed.size() + vEnemiesHeavy.size());

	// Append the vectors using std::copy
	std::copy(vEnemiesNormal.begin(), vEnemiesNormal.end(), std::back_inserter(vEnemies));
	std::copy(vEnemiesSpeed.begin(), vEnemiesSpeed.end(), std::back_inserter(vEnemies));
	std::copy(vEnemiesHeavy.begin(), vEnemiesHeavy.end(), std::back_inserter(vEnemies));


	for (int id_laser : vPlayerProjectiles) {
		GameObject& obj_laser = Play::GetGameObject(id_laser);
		bool hasCollided = false;
		
		if (Play::IsColliding(obj_laser, Play::GetGameObjectByType(Enemy_Normal))) {
			hasCollided = true;
		}
		else if (Play::IsColliding(obj_laser, Play::GetGameObjectByType(Enemy_Speed))) {
			hasCollided = true;
		}
		else if (Play::IsColliding(obj_laser, Play::GetGameObjectByType(Enemy_Heavy))) {
			hasCollided = true;
		}

		for (int id_enemy : vEnemies) {
			GameObject& obj_enemy = Play::GetGameObject(id_enemy);
			if (Play::IsColliding(obj_laser, obj_enemy)) {
				if (obj_enemy.type != Type_Destroyed) {
					hasCollided = true;
					obj_enemy.type = Type_Destroyed;
					//Play audio
					score += 200;
				}
			}
		}

		Play::UpdateGameObject(obj_laser);
		Play::DrawObject(obj_laser);
		

		if (!Play::IsVisible || hasCollided) {
			Play::DestroyGameObject(id_laser);
		}
	}
}
void UpdateDestroyed()
{
	std::vector<int> vDead = Play::CollectGameObjectIDsByType(Type_Destroyed);
	for (int id_dead : vDead)
	{
		GameObject& obj_dead = Play::GetGameObject(id_dead);
		if (!Play::IsVisible(obj_dead))
			Play::DestroyGameObject(id_dead);
	}
}

void MainMenu() {
	Play::DrawDebugText({ _displayWidth / 2, _displayHeight / 2 }, "PRESS ENTER TO PLAY");
	if (Play::KeyPressed(VK_RETURN)) {
		currentPlayState = State_Play;
	}
}

bool MainGameUpdate(float elapsedTime)
{
	Play::ClearDrawingBuffer(Play::cMagenta);
	switch (currentPlayState) {
	case State_MainMenu:
		MainMenu();
		break;
	case State_Play:
		enemyManager.spawnTimer += elapsedTime;
		PlayFunctions();
		break;
	case State_Pause:
		break;
	default:
		currentPlayState = State_MainMenu;
		break;
	}

	Play::PresentDrawingBuffer();
	return Play::KeyDown(VK_ESCAPE);
}

void PlayFunctions()
{
	EnemySpawner();
	UpdateDestroyed();
	UpdateScore();
	UpdatePlayer();
	UpdateProjectiles();
	UpdateEnemies();
}

// Gets called once when the player quits the game 
int MainGameExit(void)
{
	Play::DestroyManager();
	return PLAY_OK;
}

