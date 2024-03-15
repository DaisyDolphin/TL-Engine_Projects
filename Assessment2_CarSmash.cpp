// Assessment2_CarSmash.cpp: A program using the TL-Engine

#include <TL-Engine.h>	// TL-Engine include file and namespace
#include <sstream>
using namespace tle;

enum Direction // Direction is used for each of the moving cars
{
	Right,
	Left,
};
struct Vector3D // can be used for collision later and then eventually for drifting, drag, momentum vectors
{
	float x;
	float y;
	float z;
};
struct Vector2D // unlike Vector3D whihc will be used for the balls that move vertically above the cars, vetcor2d will be used for evertything else- as everything rests on the horizontal plane
{
	float x;
	float z;
};
struct EnemyDetails
{
	IModel* Enemy; // model of enemy
	IModel* Ball; // ball of enemy
	bool Hit; // hit status
	bool CollidedBefore; // So when the ball is red, the playe cant continue to collide for points
	Direction MoveDirection; // move direction of moving enemies
	Vector2D RectangleMin; // rectangleMin used in collision detection
	Vector2D RectangleMax;  // rectangleMin used in collision detection
	

};
struct PlayerDetails
{
	IModel* Player; // player model
	float SphereRadius; // sphere radius for collision detection
	Vector2D PreviousPosition; // previous position for collision detection
	int Health; // health of player
};
enum gameStates
{
	Playing, Paused, GameWon, GameOver
};


void CubeSphereCollision(EnemyDetails& Enemy, PlayerDetails& Player, float& currentScore); // function for collision of enemies vs player

// so function score increases depending on corner of rectangle hit? IF hit

void ScoreIncrementer(EnemyDetails& Enemy, float& currentScore); // Score Incrementor increments score based on collisions with enemies

float DotProduct(Vector2D v1, Vector2D v2); // dotproduct used for identifying side or head on collisions with enemies

Vector2D Normalise(Vector2D v); // normalises a vector

float Length(Vector2D v); // finds length of vector

float AngleRads(Vector2D v1, Vector2D v2); // converts degrees to rads

void Calculate045Vector(IModel* model, Vector2D& facingVector) // Calculates the Facing Vector for the model but for a 45 degree angle clockwise or 045 bearing
{
	// compares the two posistions, we need to move the local z to have 
	// a difference between the posistions for calcualtion purposes.
	Vector2D initialPos = { model->GetX(), model->GetZ() };
	model->MoveLocalZ(1.0f);
	model->MoveLocalX(1.0f);
	Vector2D finalPos = { model->GetX(), model->GetZ() };
	facingVector = { finalPos.x - initialPos.x, finalPos.z - initialPos.z };
	model->MoveLocalZ(-1.0f);
	model->MoveLocalX(-1.0f);
}
void Calculate135Vector(IModel* model, Vector2D& facingVector) // Calculates the Facing Vector for the model but for a 135 degree angle clockwise or 135 bearing
{
	// compares the two posistions, we need to move the local z to have 
	// a difference between the posistions for calcualtion purposes.
	Vector2D initialPos = { model->GetX(), model->GetZ() };
	model->MoveLocalZ(-1.0f);
	model->MoveLocalX(1.0f);
	Vector2D finalPos = { model->GetX(), model->GetZ() };
	facingVector = { finalPos.x - initialPos.x, finalPos.z - initialPos.z };
	model->MoveLocalZ(1.0f);
	model->MoveLocalX(-1.0f);
}

//void CalculateAngle(IModel* model, Vector3D& facingVector) // passed by reference so everytime its called, it changes values
//{
//	double opposite = facingVector.x;
//	double adjacent = facingVector.z;
//	double alphaDegrees = atan(opposite / adjacent); // do i need radians even? no, but i do for the circle of trees
//}

double GetCalculateAngle(IModel* model, Vector2D facingVector);

// 2.1
// array of struct enemies moving or static
// circle of trees
// moving vehicles respawn
// 1.1,
// acceleration
// car bouncing collision
// restart
// enums
// ball on top of mving enemy bounces up n down
// front wheels turn on the car
// dust trail

const float kSphereRadius = 1.6f; //also serves as the boundary volume's radius and the sphere is the players collision
const float kCubeRadius = 1.6f; //also serves as the boundary volume's radius
const int kmyFontFontSize = 36; // avoids magic number, font size of the myFont Font
const EKeyCode k1KeyHit = Key_1; // Avoids magic numbers for 1 key presses 
const EKeyCode k2KeyHit = Key_2; // Avoids magic numbers for 2 key presses 
const EKeyCode k3KeyHit = Key_3; // Avoids magic numbers for 3 key presses 
const EKeyCode kPKeyHit = Key_P; // Avoids magic numbers for P key presses pausing screen
const EKeyCode kWKeyHit = Key_W; // Avoids magic numbers for W key presses 
const EKeyCode kAKeyHit = Key_A; // Avoids magic numbers for A key presses 
const EKeyCode kSKeyHit = Key_S; // Avoids magic numbers for S key presses 
const EKeyCode kDKeyHit = Key_D; // Avoids magic numbers for D key presses 
const EKeyCode kEscapeKeyHit = Key_Escape; // avoids magic numbers for Esc key presses
const float kPlayerMovementSpeed = 35.f; // movement speed of player
const float kPlayerRotationSpeed = 70.f; // rotation speed of player
const float kScorePerHit = 10.0f; // score per hit on enemy
const float kScorePerSideHitBonus = 5.0f; // bonus score added if it was side on collision
bool SideCollision = false; // side collision true or false
const float kCurrentScoreCoordModifier = 15.f / 16.f; // multiplier used for currentscores coordinate on screen
const float kBackdropYCoordModifier = 60.f; // to make the bakcdrop placement accurate it needed to be modifeied slightly
const float kKey3HitCamModifier = 2.f; // to make camera look more accurate i adjusted the camera placement
const float kKey2HitCamYModifier = 5.f; // to make camera look more accurate i adjusted the camera placement
const float kKey2HitCamZModifier = 15.f; // to make camera look more accurate i adjusted the camera placement
const int kNumEnemiesPerType = 4; // there is 4 enemies of each type
const Vector2D kenemy1spawn = { -20, 20 }; 
const Vector2D kenemy2spawn = { 20, 20 };
const Vector2D kenemy3spawn = { -20, 0 };
const Vector2D kenemy4spawn = { 20, 0 };
const Vector2D kenemy5spawn = { -30, 15 };
const Vector2D kenemy5destination = { 30, 15 };
const Vector2D kenemy6spawn = { 30, -15 };
const Vector2D kenemy6destination = { -30, -15 };
const Vector2D kenemy7spawn = { 30, 30 };
const Vector2D kenemy7destination = { -30, 30 };
const Vector2D kenemy8spawn = { -30, -30 };
const Vector2D kenemy8destination = { 30, -30 };
const float kMovingEnemyMinX = -30; // for movement of enemy once reaches this coordinate it turns around.
const float kMovingEnemyMaxX = 30; // for movement of enemy once reaches this coordinate it turns around.

const float kBallMinHeight = 3.0f; // height of ball of the ground 
const float kMovingBallMaxHeight = 5.0f; // maximum height of ball as it gently bounces up and down
const float kEnemyYCoordinateModifier = 0.0f; // coordinates of enemies Y coordinate
const Vector3D kGroundCoords = { 0,0,0 }; // groundMesh and model coordinates
const Vector3D kPlayerCoords = { 0,0,0 }; // PlayerMesh and model coordinates
const Vector3D kSkyboxCoords = { 0,-960.f,0 }; // skybox mesh and model coordinates
const double kPI = 3.14159265359; // a close value to pi
const int kEnemy1 = 0; // avoids magic numbers
const int kEnemy2 = 1; // avoids magic numbers
const int kEnemy3 = 2;// avoids magic numbers
const int kEnemy4 = 3;// avoids magic numbers
const float kSideCollisionEnemyScale = 0.990; // upon side collision the enemys scale decreases by this proportion
const float kNonSideCollisionEnemyScale = 0.995;// upon non-side collision the enemys scale decreases by this proportion
const int kNumOfTrees = 160;
const float kRadiusOfTrees = 50.0f; // radius of perimiter for all these trees
const int kNumDirections = 2; // number of directions the moving cars can go in
const float kMovingEnemySpeed = kPlayerMovementSpeed/2;// speed of moving enemies
const float kMovingBallSpeed = 10.0f; // speed of moving balls as they bounce




void main()
{
	// Create a 3D engine (using TLX engine here) and open a window for it
	I3DEngine* myEngine = New3DEngine( kTLX );
	myEngine->StartWindowed();

	// Add default folder for meshes and other media
	myEngine->AddMediaFolder( ".\\Media" );

	/**** Set up your scene here ****/

	ICamera* myCamera;
	const Vector3D kCameraCoords = { 0.f,15.f,-60.f };
	float cameraDegree = 15.f;
	myCamera = myEngine->CreateCamera(kManual, kCameraCoords.x, kCameraCoords.y, kCameraCoords.z);
	myCamera->RotateX(cameraDegree);

	IModel* groundModel;
	IMesh* groundMesh;
	IModel* playerModel;
	IMesh* playerMesh;
	IModel* skyboxModel;
	IMesh* skyboxMesh;

	int const kNumOfStates = 3;

	gameStates states[kNumOfStates]
	{
		gameStates::Playing,
		gameStates::Paused,
		gameStates::GameWon
	};
	gameStates currentState = Playing;


	/*Direction directions[kNumDirections]
	{
		Direction::Right,
		Direction::Left,
	};*/

	groundMesh = myEngine->LoadMesh("ground.x");
	const Vector3D kGroundCoords = { 0,0,0 };
	playerMesh = myEngine->LoadMesh("4x4jeep.x");
	const Vector3D kPlayerCoords = { 0,0,0 };
	skyboxMesh = myEngine->LoadMesh("skybox01.x");
	const Vector3D kSkyboxCoords = { 0,-960.f,0 };
	skyboxModel = skyboxMesh->CreateModel(kSkyboxCoords.x, kSkyboxCoords.y, kSkyboxCoords.z);
	playerModel = playerMesh->CreateModel(kPlayerCoords.x, kPlayerCoords.y, kPlayerCoords.z);
	groundModel = groundMesh->CreateModel(kGroundCoords.x, kGroundCoords.y, kGroundCoords.z);	

	IMesh* enemyAudiCarMesh;
	enemyAudiCarMesh = myEngine->LoadMesh("audi.x"); // array of static enemies
	IModel* staticEnemies[kNumEnemiesPerType] =
	{
		enemyAudiCarMesh->CreateModel(kenemy1spawn.x, kEnemyYCoordinateModifier, kenemy1spawn.z),
		enemyAudiCarMesh->CreateModel(kenemy2spawn.x, kEnemyYCoordinateModifier, kenemy2spawn.z),
		enemyAudiCarMesh->CreateModel(kenemy3spawn.x, kEnemyYCoordinateModifier, kenemy3spawn.z),
		enemyAudiCarMesh->CreateModel(kenemy4spawn.x, kEnemyYCoordinateModifier, kenemy4spawn.z)
	};	
	IMesh* staticBallMesh;
	staticBallMesh = myEngine->LoadMesh("ball.x"); // array of static balls
	IModel* staticballs[kNumEnemiesPerType] =
	{
		staticBallMesh->CreateModel(kenemy1spawn.x, kBallMinHeight, kenemy1spawn.z),
		staticBallMesh->CreateModel(kenemy2spawn.x, kBallMinHeight, kenemy2spawn.z),
		staticBallMesh->CreateModel(kenemy3spawn.x, kBallMinHeight, kenemy3spawn.z),
		staticBallMesh->CreateModel(kenemy4spawn.x, kBallMinHeight, kenemy4spawn.z)
	};
	for (int i = 0; i < kNumEnemiesPerType; i++) // setting the skin on balls
	{
		staticballs[i]->SetSkin("white.png");
	}
	//EnemyDetails* sStaticEnemyDetails[kNumEnemies];
	EnemyDetails sStaticEnemyDetails[kNumEnemiesPerType]; // putting all the enemy details into a struct for easy access
	for (int i = 0; i < kNumEnemiesPerType; i++)
	{
		EnemyDetails StaticEnemies = { staticEnemies[i], staticballs[i], false, false };
		sStaticEnemyDetails[i] = StaticEnemies;
	}

	

	IMesh* enemyEstateCarMesh;
	enemyEstateCarMesh = myEngine->LoadMesh("estate.x"); // array of moving enemies
	IModel* movingEnemies[kNumEnemiesPerType] =
	{
		enemyEstateCarMesh->CreateModel(kenemy5spawn.x, kEnemyYCoordinateModifier, kenemy5spawn.z),
		enemyEstateCarMesh->CreateModel(kenemy6spawn.x, kEnemyYCoordinateModifier, kenemy6spawn.z),
		enemyEstateCarMesh->CreateModel(kenemy7spawn.x, kEnemyYCoordinateModifier, kenemy7spawn.z),
		enemyEstateCarMesh->CreateModel(kenemy8spawn.x, kEnemyYCoordinateModifier, kenemy8spawn.z)
	};
	IMesh* movingBallMesh;
	movingBallMesh = myEngine->LoadMesh("ball.x"); // array of moving balls
	IModel* movingballs[kNumEnemiesPerType] =
	{
		movingBallMesh->CreateModel(kenemy5spawn.x, kBallMinHeight, kenemy5spawn.z),
		movingBallMesh->CreateModel(kenemy6spawn.x, kBallMinHeight, kenemy6spawn.z),
		movingBallMesh->CreateModel(kenemy7spawn.x, kBallMinHeight, kenemy7spawn.z),
		movingBallMesh->CreateModel(kenemy8spawn.x, kBallMinHeight, kenemy8spawn.z)
	};
	for (int i = 0; i < kNumEnemiesPerType; i++)
	{
		movingballs[i]->SetSkin("white.png"); // setting skins on balls
	}
	//EnemyDetails* sStaticEnemyDetails[kNumEnemies];
	EnemyDetails sMovingEnemyDetails[kNumEnemiesPerType]; // putting all the enemy details into a struct for easy access
	for (int i = 0; i < kNumEnemiesPerType; i++)
	{
		EnemyDetails MovingEnemies = { movingEnemies[i], movingballs[i], false, false };
		sMovingEnemyDetails[i] = MovingEnemies;
	}
	float NinetyDegrees = 90;
	sMovingEnemyDetails[kEnemy1].MoveDirection = Direction::Right; // manually assigning the moving enemies rotations at the start and their starting direction of travel
	sMovingEnemyDetails[kEnemy1].Enemy->RotateY(NinetyDegrees);
	sMovingEnemyDetails[kEnemy2].MoveDirection = Direction::Left;
	sMovingEnemyDetails[kEnemy2].Enemy->RotateY(-NinetyDegrees);
	sMovingEnemyDetails[kEnemy3].MoveDirection = Direction::Left;
	sMovingEnemyDetails[kEnemy3].Enemy->RotateY(-NinetyDegrees);
	sMovingEnemyDetails[kEnemy4].MoveDirection = Direction::Right;
	sMovingEnemyDetails[kEnemy4].Enemy->RotateY(NinetyDegrees);


	IMesh* treeMesh;
	treeMesh = myEngine->LoadMesh("tree.x");
	IModel* treeModels[kNumOfTrees];

	float degrees = 0;
	float radians = kPI / 180 * degrees;
	float x = kRadiusOfTrees * cos(radians);
	float z = kRadiusOfTrees * sin(radians);
	for (int i = 0; i < kNumOfTrees; i++) // for loop used to create each tree in the giant circle
	{
		degrees += 360.0f/kNumOfTrees;
		radians = kPI / 180 * degrees;
		x = cos(radians) * kRadiusOfTrees;
		z = sin(radians) * kRadiusOfTrees;

		treeModels[i] = treeMesh->CreateModel(x, 0, z); 
	}






	
	IFont* myFont; 
	myFont = myEngine->LoadFont("Roboto", kmyFontFontSize); // the font im using is Roboto, because it looks nice, and 36 is a readable size of font
	const float kwidthOfScreen = myEngine->GetWidth(); // width of the TL engine screen
	const float kheightOfScreen = myEngine->GetHeight(); // height of the TL engine screen
	float currentScore = 0;
	myEngine->Timer();
	float frameTime = 0.f; // frameTime used for movement of models
	float elapsedTime = 0.f;
	ISprite* backdrop = myEngine->CreateSprite("backdrop.jpg", kwidthOfScreen / 4, kheightOfScreen - kBackdropYCoordModifier); // backdrop for stringstream
	Vector2D PlayerVector = { 0, 0};
	Vector2D RectangleMin = { 0, 0}; 
	Vector2D RectangleMax = { 0, 0}; 
	Vector2D prevPos = { 0, 0};
	int playerHealth = 100;
	PlayerDetails sPlayerDetails = {playerModel, kSphereRadius, prevPos, playerHealth};
	bool isPlayerOnCooldown = false;
	int playerCooldownFrames = 120; // frames cooldown before player can be collided into again
	int framesSinceCollision = 0; // variable to count frames since last collision	

	// The main game loop, repeat until engine is stopped
	while (myEngine->IsRunning())
	{
		// Draw the scene
		myEngine->DrawScene();
		/**** Update your scene each frame here ****/

		PlayerVector.x = playerModel->GetX();
		PlayerVector.z = playerModel->GetZ();		

		switch (currentState)
		{
		case gameStates::Playing:
		{	
			frameTime = myEngine->Timer();
			elapsedTime += myEngine->Timer();		
			stringstream ssplayerHealth;
			ssplayerHealth << "Health: " << sPlayerDetails.Health;
			myFont->Draw(ssplayerHealth.str(), kwidthOfScreen / 3, kheightOfScreen * kCurrentScoreCoordModifier, kBlack, kCentre); // kheight * currentscorecoordmulti = 15/16

			stringstream sscurrentScore;
			sscurrentScore << "Current Score: " << currentScore;
			myFont->Draw(sscurrentScore.str(), 2* kwidthOfScreen/ 3, kheightOfScreen * kCurrentScoreCoordModifier, kBlack, kCentre); // kheight * currentscorecoordmulti = 15/16

			prevPos = { playerModel->GetX(), playerModel->GetZ() };
			sPlayerDetails.PreviousPosition = { playerModel->GetX(), playerModel->GetZ() }; // prevPos is the Player previosu position // should be added to playerdetails

			if (myEngine->KeyHit(k1KeyHit)) // default camera
			{
				myCamera->DetachFromParent();
				myCamera = myEngine->CreateCamera(kManual, kCameraCoords.x, kCameraCoords.y, kCameraCoords.z);
				myCamera->RotateX(cameraDegree);
			}
			if (myEngine->KeyHit(k2KeyHit)) // chase camera
			{			
				myCamera->DetachFromParent();
				float camX = PlayerVector.x;
				float camY = kKey2HitCamYModifier;
				float camZ = PlayerVector.z - kKey2HitCamZModifier;
				myCamera = myEngine->CreateCamera(kManual, camX, camY, camZ);
				myCamera->AttachToParent(playerModel);


				//float cameraDegreeChase = GetCalculateAngle(sPlayerDetails.Player, PlayerVector); 	
			}
			if (myEngine->KeyHit(k3KeyHit)) // first person camera
			{
				myCamera->DetachFromParent();
				//myCamera = myEngine->CreateCamera(kManual, playerModel->GetX(), kKey3HitCamModifier, PlayerVector.z);

				myCamera->AttachToParent(playerModel);
			}
			if (myEngine->KeyHeld(kWKeyHit))
			{
				playerModel->MoveLocalZ(kPlayerMovementSpeed * frameTime);
				// eventually this will change, 
				// to calculating thrust vector
				// // get the facing vector (where th car is facing, make Vector2D)
				// calculate drag vector
				// move car according to momentum
			}
			if (myEngine->KeyHeld(kAKeyHit))
			{
				playerModel->RotateY(-kPlayerRotationSpeed * frameTime);
			}
			if (myEngine->KeyHeld(kSKeyHit))
			{
				playerModel->MoveLocalZ(-kPlayerMovementSpeed * frameTime);
			}
			if (myEngine->KeyHeld(kDKeyHit))
			{
				playerModel->RotateY(kPlayerRotationSpeed * frameTime);
			}
			if (myEngine->KeyHit(kPKeyHit))
			{
				currentState = Paused;
			}

			



			if (Length(PlayerVector) >= 50) // if the player has exceeded 50 units away from (0, 0) position
			{
				if (!isPlayerOnCooldown)
				{
					// player isn't in cooldown and can lose health

					// collision with the trees is true;
					sPlayerDetails.Health -= 1; // magic number
					sPlayerDetails.Player->SetPosition(prevPos.x, 0, prevPos.z);  // teleport the player back into the ring

					isPlayerOnCooldown = true;
					framesSinceCollision = 0;
				}
			}
			framesSinceCollision++;
			if (isPlayerOnCooldown && framesSinceCollision >= playerCooldownFrames) // checks if playe is still on cooldown, if they arem't it'll reset it back to false
			{
				isPlayerOnCooldown = false;
			}

			
			for (int i = 0; i < kNumEnemiesPerType; i++)				
			{
				// Moving Cars Directions:

				switch (sMovingEnemyDetails[i].MoveDirection)
				{
				case Direction::Left:
				{
					sMovingEnemyDetails->Enemy->MoveX(-kMovingEnemySpeed * frameTime); 
					if (sMovingEnemyDetails[i].Enemy->GetX() <= kMovingEnemyMinX)
					{
						sMovingEnemyDetails[i].MoveDirection = Direction::Right;
						sMovingEnemyDetails[i].Enemy->RotateY(180); // flip the car around
					}	
					break;
				}
				case Direction::Right:
				{
					sMovingEnemyDetails->Enemy->MoveX(kMovingEnemySpeed * frameTime);
					if (sMovingEnemyDetails[i].Enemy->GetX() >= kMovingEnemyMaxX)
					{
						sMovingEnemyDetails[i].MoveDirection = Direction::Left;
						sMovingEnemyDetails[i].Enemy->RotateY(180); // flips the car around
					}
					break;
				}
				}




				//CalculateFacingVector for the 45 degree angle and 135 angle
				// i specifically hcose these angles because i can separate the front, back and each side collisions by doing this...

				Vector2D Static045Direction = { 0, 0 };
				Static045Direction.x = sStaticEnemyDetails[i].Enemy->GetX();
				Static045Direction.z = sStaticEnemyDetails[i].Enemy->GetZ();
				Vector2D Static135Direction = { 0, 0 };
				Static135Direction.x = sStaticEnemyDetails[i].Enemy->GetX();
				Static135Direction.z = sStaticEnemyDetails[i].Enemy->GetZ();
				Vector2D playerToStaticEnemy = { 0, 0 };
				playerToStaticEnemy.x = sStaticEnemyDetails[i].Enemy->GetX() - playerModel->GetX();
				playerToStaticEnemy.z = sStaticEnemyDetails[i].Enemy->GetZ() - playerModel->GetZ();


				Vector2D Moving045Direction = { 0, 0 };
				Moving045Direction.x = sMovingEnemyDetails[i].Enemy->GetX();
				Moving045Direction.z = sMovingEnemyDetails[i].Enemy->GetZ();
				Vector2D Moving135Direction = { 0, 0 };
				Moving135Direction.x = sMovingEnemyDetails[i].Enemy->GetX();
				Moving135Direction.z = sMovingEnemyDetails[i].Enemy->GetZ();
				Vector2D playerToMovingEnemy = { 0, 0 };
				playerToMovingEnemy.x = sMovingEnemyDetails[i].Enemy->GetX() - playerModel->GetX();
				playerToMovingEnemy.z = sMovingEnemyDetails[i].Enemy->GetZ() - playerModel->GetZ();
					
				Calculate045Vector(sStaticEnemyDetails[i].Enemy, Static045Direction);
				Calculate135Vector(sStaticEnemyDetails[i].Enemy, Static135Direction);
				Calculate045Vector(sMovingEnemyDetails[i].Enemy, Moving045Direction);
				Calculate135Vector(sMovingEnemyDetails[i].Enemy, Moving135Direction);
		
				// Normalize

				Normalise(Static045Direction);
				Normalise(Static135Direction);
				Normalise(Moving045Direction);
				Normalise(Moving135Direction);

				// Dotproduct right and front

				float dot045Static = DotProduct(Static045Direction, playerToStaticEnemy);
				float dot135Static = DotProduct(Static135Direction, playerToStaticEnemy); // dotRight's forward facing vector faces 90 degrees right
				float dot045Moving = DotProduct(Moving045Direction, playerToMovingEnemy);
				float dot135Moving = DotProduct(Moving135Direction, playerToMovingEnemy); // dotRight's forward facing vector faces 90 degrees right

				// static enemy collisions
				if (dot045Static < 0 && dot135Static > 0) 
				{
					// Front collision
					SideCollision = false;
				}
				else if (dot045Static > 0 && dot135Static > 0)
				{
					// Left collision
					SideCollision = true;
				}
				else if (dot045Static < 0 && dot135Static < 0)
				{
					// Right collision
					SideCollision = true;
				}
				else if (dot045Static > 0 && dot135Static < 0)
				{
					// Back collision
					SideCollision = false;
				}

				// moving enemy collisions
				if (dot045Moving < 0 && dot135Moving > 0)
				{
					// Front collision
					SideCollision = false;
				}
				else if (dot045Moving > 0 && dot135Moving > 0)
				{
					// Left collision
					SideCollision = true;
				}
				else if (dot045Moving < 0 && dot135Moving < 0)
				{
					// Right collision
					SideCollision = true;
				}
				else if (dot045Moving > 0 && dot135Moving < 0)
				{
					// Back collision
					SideCollision = false;
				}

				CubeSphereCollision(sStaticEnemyDetails[i], sPlayerDetails, currentScore);
				CubeSphereCollision(sMovingEnemyDetails[i], sPlayerDetails, currentScore);			

				// if all of the enemies ball's are red (All Collided Before is true) - then become GameWon state				
				if (sStaticEnemyDetails[kEnemy1].CollidedBefore && 
					sStaticEnemyDetails[kEnemy2].CollidedBefore && 
					sStaticEnemyDetails[kEnemy3].CollidedBefore && 
					sStaticEnemyDetails[kEnemy4].CollidedBefore && 
					sMovingEnemyDetails[kEnemy1].CollidedBefore &&
					sMovingEnemyDetails[kEnemy2].CollidedBefore &&
					sMovingEnemyDetails[kEnemy3].CollidedBefore &&
					sMovingEnemyDetails[kEnemy4].CollidedBefore)
				{
					currentState = GameWon;// I'm not sure if this code can be simplified
				}
				if (sPlayerDetails.Health <= 0)
				{
					currentState = GameOver;
				}
			}		
			break;
		}
		case gameStates::Paused:
		{
			stringstream sspausedScreen;
			sspausedScreen << "Game is paused"; // displays the paused string on screen
			myFont->Draw(sspausedScreen.str(), kwidthOfScreen / 2, kheightOfScreen / 2, kWhite, kCentre); // draws the paused screen string, in the middle of the screen cemtralized

			if (myEngine->KeyHit(kPKeyHit))
			{
				currentState = Playing;
			}
			break;
		}
		case gameStates::GameWon:
		{
			stringstream ssYouWin; // Gmae win, so display score and cant do anything else
			ssYouWin << "You win, you're current score is " << currentScore;
			myFont->Draw(ssYouWin.str(), kwidthOfScreen / 2, kheightOfScreen / 2, kWhite, kCentre);			
			break;
		}
		case gameStates::GameOver:
		{
			stringstream ssYouLose;
			ssYouLose << "You lose, you're current score is " << currentScore;
			myFont->Draw(ssYouLose.str(), kwidthOfScreen / 2, kheightOfScreen / 2, kWhite, kCentre);
			break;
		}
		}
		if (myEngine->KeyHit(kEscapeKeyHit)) //if the key Escape key is hit
		{
			//myEngine->Stop(); //exit the program
			int fred = 0;
		}
	}

	// Delete the 3D engine now we are finished with it
	myEngine->Delete();
}


void CubeSphereCollision(EnemyDetails& Enemy, PlayerDetails& Player, float& currentScore)
{
	Vector3D PlayerXYZ = { Player.Player->GetX(), Player.Player->GetY(), Player.Player->GetZ() };
	Vector3D CubeSphereVector = { Enemy.Enemy->GetX() - Player.Player->GetX(), Enemy.Enemy->GetY() - Player.Player->GetY(), Enemy.Enemy->GetZ() - Player.Player->GetZ() };
	Enemy.RectangleMin.x = Enemy.Enemy->GetX() - kCubeRadius - kSphereRadius;
	Enemy.RectangleMin.z = Enemy.Enemy->GetZ() - kCubeRadius - kSphereRadius;
	Enemy.RectangleMax.x = Enemy.Enemy->GetX() + kCubeRadius + kSphereRadius;
	Enemy.RectangleMax.z = Enemy.Enemy->GetZ() + kCubeRadius + kSphereRadius;

	if (Enemy.RectangleMin.x < PlayerXYZ.x && Enemy.RectangleMax.x > PlayerXYZ.x && Enemy.RectangleMin.z < PlayerXYZ.z && Enemy.RectangleMax.z > PlayerXYZ.z)
	{
		// collision has occurred
		Player.Player->SetPosition(Player.PreviousPosition.x, 0, Player.PreviousPosition.z);// if the sphere and cube collides then set the player back to its orginal position.
		Enemy.Hit = true;// and the enemy's details hit becomes true
		if (SideCollision)
		{
			Enemy.Enemy->ScaleX(kSideCollisionEnemyScale);
		}
		else
		{
			Enemy.Enemy->ScaleZ(kNonSideCollisionEnemyScale);
		}
		ScoreIncrementer(Enemy, currentScore);
	}
}

void ScoreIncrementer(EnemyDetails& Enemy, float& currentScore) // This function increments the score by 10 if the enemy's ball isn't red and theyve not been hit yet.
{
	// if the enemy is hit - and the ball is red - add score

	if (Enemy.Hit && !Enemy.CollidedBefore) // if enemy hit and the player hasn't collided before
	{
		if (SideCollision)
		{
			currentScore += kScorePerSideHitBonus;
		}
		currentScore += kScorePerHit; //kSscorePerHit = 10
		Enemy.CollidedBefore = true; // collided before becomes true to stop further collisions
		Enemy.Ball->SetSkin("red.png"); // change the colour of the ball to red after collision
	}
}

float DotProduct(Vector2D v1, Vector2D v2)
{
	return ((v1.x * v2.x) + (v1.z * v2.z));
}

Vector2D Normalise(Vector2D v)
{
	float vectorx = (v.x / Length(v));
	//float vectory = (v.y / Length(v));
	float vectorz = (v.z / Length(v));

	Vector2D vector
	{
		vectorx, vectorz
	};

	return vector;
}

float Length(Vector2D v)
{
	return sqrt(v.x * v.x + v.z * v.z);
}

float AngleRads(Vector2D v1, Vector2D v2)
{
	return acos(DotProduct(v1, v2) / Length(v1) * Length(v2));
}

double GetCalculateAngle(IModel* model, Vector2D facingVector)
{
	double opposite = facingVector.x; // -9.255...e+61
	double adjacent = facingVector.z; // 0.0
	double alphaRadians = 0;
	if (adjacent != 0) // I cant divide by 0 
	{
		alphaRadians = atan(opposite / adjacent);
	}
	double alphaDegrees = alphaRadians * (180.0 / kPI); // Convert radians to degrees
	return alphaDegrees;
}