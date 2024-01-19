#define PLAY_IMPLEMENTATION
#define PLAY_USING_GAMEOBJECT_MANAGER
#include "Play.h"

enum GameObjectTypes {
	TYPE_Null = -1,
	TYPE_Player,
	Type_PlayerProjectile,
	Type_Destroyed,
	Enemy_Light,
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
	float spawnRate = .5f;
	float newWaveTimer = 0;
	float newWaveCountdown = 10;
	bool triggerNewWaveCountdown = false;
	int enemiesSpawned = 0;
	int enemiesToSpawn = 100;
	float lightEnemySpeed = 5;
	float normalEnemySpeed = 3;
	float heavyEnemySpeed = 1;
};
EnemyManager enemyManager;

struct PlayerProperties {
	float playerSpeed = 1;
	float projectileSpeed = 2;
	float animatorSpeed = 0.1f;
	
	float projectileFireRate = .5f;
	float projectileTimer = 0;
	bool canShoot = true;
};

PlayerProperties playerProperties;
int _displayWidth = 240;
int _displayHeight = 240;
int _displayScale = 3;
int _clampScreen = 15;
int score = 0;


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
	if (playerProperties.canShoot == false && playerProperties.projectileTimer > playerProperties.projectileFireRate) {
		playerProperties.canShoot = true;
		playerProperties.projectileTimer = 0;
	}
	player.pos += player.velocity;
	if (Play::IsLeavingDisplayArea(player, Play::HORIZONTAL)) {
		player.pos.x = player.oldPos.x;
	}

	if (Play::KeyDown(0x44)) { // Right
		Play::SetSprite(player, "Player_Spaceship_Moving", playerProperties.animatorSpeed);
		player.velocity.x = playerProperties.playerSpeed;
		Play::DrawDebugText({ _displayWidth / 2, _displayHeight / 2 }, "Right");
	}
	else if (Play::KeyDown(0x41)) { // Left
		Play::SetSprite(player, "Player_Spaceship_Moving", playerProperties.animatorSpeed);
		player.velocity.x = -playerProperties.playerSpeed;
		Play::DrawDebugText({ _displayWidth / 2, _displayHeight / 2 }, "Left");
	}
	else { // No input
		Play::SetSprite(player, "Player_Spaceship_Idle", playerProperties.animatorSpeed);
		player.velocity.x = 0;
		Play::DrawDebugText({ _displayWidth / 2, _displayHeight / 2 }, "Idle");
	}
	if (Play::KeyPressed(0x01) && playerProperties.canShoot) { // Mouse 1
		Vector2D RightPos = Vector2D(player.pos.x + 6, player.pos.y - 6);
		Vector2D LeftPos = Vector2D(player.pos.x - 6, player.pos.y - 6);
		int idLeftProjectile = Play::CreateGameObject(Type_PlayerProjectile, RightPos, 2, "PlayerLaser");
		int idRightProjectile = Play::CreateGameObject(Type_PlayerProjectile, LeftPos, 2, "PlayerLaser");
		Play::GetGameObject(idLeftProjectile).velocity.y = -playerProperties.projectileSpeed;
		Play::GetGameObject(idRightProjectile).velocity.y = -playerProperties.projectileSpeed;
		playerProperties.canShoot = false;
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
			int idEnemy = Play::CreateGameObject(Enemy_Normal, rightSpawnPos, 16, "EnemyNormal");
			Play::GetGameObject(idEnemy).velocity.x = -enemyManager.normalEnemySpeed;
		}
		else if (randomEnemy > 6 && randomEnemy < 8) {
			int idEnemy = Play::CreateGameObject(Enemy_Light, rightSpawnPos, 16, "EnemyLight");
			Play::GetGameObject(idEnemy).velocity.x = -enemyManager.lightEnemySpeed;
		}
		else {
			int idEnemy = Play::CreateGameObject(Enemy_Heavy, rightSpawnPos, 16, "EnemyHeavy");
			Play::GetGameObject(idEnemy).velocity.x = -enemyManager.heavyEnemySpeed;
		}
	}
	else {
		enemyManager.triggerNewWaveCountdown = true;
	}
	
}

void NewWaveEvent(float elapsedTime) {
	
	if (enemyManager.newWaveTimer > enemyManager.newWaveCountdown) {
		enemyManager.triggerNewWaveCountdown = false;
		enemyManager.spawnRate = enemyManager.spawnRate / 2;
	}
	else {
		enemyManager.newWaveTimer += elapsedTime;
	}
}

void UpdateEnemies() {
	std::vector<int> vEnemiesNormal = Play::CollectGameObjectIDsByType(Enemy_Normal);
	std::vector<int> vEnemiesSpeed = Play::CollectGameObjectIDsByType(Enemy_Light);
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
			if (obj_enemy.velocity.x > 0) {
				obj_enemy.velocity.x = -obj_enemy.velocity.x;
			}
			else {
				obj_enemy.velocity.x = abs(obj_enemy.velocity.x);
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
	std::vector<int> vEnemiesSpeed = Play::CollectGameObjectIDsByType(Enemy_Light);
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
		else if (Play::IsColliding(obj_laser, Play::GetGameObjectByType(Enemy_Light))) {
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
	Play::ClearDrawingBuffer(Play::cBlack);
	switch (currentPlayState) {
	case State_MainMenu:
		MainMenu();
		break;
	case State_Play:
		TimerRelatedFunctions(elapsedTime);
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
void TimerRelatedFunctions(float deltaTime) {
	enemyManager.spawnTimer += deltaTime;
	playerProperties.projectileTimer += deltaTime;
	if (enemyManager.triggerNewWaveCountdown) {
		NewWaveEvent(deltaTime);
	}
}
// Gets called once when the player quits the game 
int MainGameExit(void)
{
	Play::DestroyManager();
	return PLAY_OK;
}

