// Assessment 1 - Square About.cpp: A program using the TL-Engine
#include <TL-Engine.h>	// TL-Engine include file and namespace
#include <sstream>
using namespace std;
using namespace tle;

enum States //  enum = a data type that has members, states of the game, that will be played
{
	MainMenu,
	Playing,
	Paused,
	GameWon,
	GameOver
};
enum Direction // enum = a data type that has members, in this one, the directions are for the sphere movement that happens in game
{
	Up,
	Right,
	Down,
	Left,
};
void main()
{
	// Create a 3D engine (using TLX engine here) and open a window for it
	I3DEngine* myEngine = New3DEngine(kTLX);
	myEngine->StartWindowed();

	// Add default folder for meshes and other media
	myEngine->AddMediaFolder(".\\Media");
	//myEngine->AddMediaFolder("C:\\ProgramData\\TL-Engine\\Media");

	/**** Set up your scene here ****/

	IMesh* floorMesh;
	IModel* floorModel;
	IMesh* platformMesh;
	IModel* platformModel;
	IMesh* sphereMesh;
	IModel* sphere1;
	IMesh* cubeMesh;
	IModel* cubeModel;
	IMesh* bulletMesh;
	IModel* bulletModel;

	IFont* myFont; // lines 49 - 51 initialising the Font, 
	const int kmyFontFontSize = 36; // avoids magic number, font size of the myFont Font
	myFont = myEngine->LoadFont("Roboto", kmyFontFontSize); // the font im using is Roboto, because it looks nice, and 36 is a readable size of font

	const int kNumofStates = 4;

	States states[kNumofStates]
	{
		States::Playing,
		States::Paused,
		States::GameOver,
		States::GameWon,	
	};
	States currentState = Playing;

	const int kNumSpheres = 4; // size(spheres) / ; // how many spheres do i want to spawn? 4.
	const float kLowestSphereXCoordinate = -85.0f; // furthest left x coordinate
	const float kHighestSphereXCoordinate = 85.0f; // furthest right x coordinate
	const float kLowestSphereZCoordinate = -85.0f; // furthest down z coordinate
	const float kHighestSphereZCoordinate = 85.0f; // furthest top z coordinate
	const float kSphereYCoordinate = 10.0f; // the y coordinate set for all the spheres

	sphereMesh = myEngine->LoadMesh("spheremesh.x"); // initialisation of the spheres to travel around the map 
	IModel* spheres[kNumSpheres] = // an array including all the spheres that will be used in the game, they are inside an array to be more efficient at traveling in each direction.
	{
		sphereMesh->CreateModel(kLowestSphereXCoordinate, kSphereYCoordinate, kLowestSphereZCoordinate), // each sphere will spawn around the edge of the map, and start they're designated paths
		sphereMesh->CreateModel(kHighestSphereXCoordinate, kSphereYCoordinate, kHighestSphereZCoordinate), // each sphere spawns separately at each corner
		sphereMesh->CreateModel(kHighestSphereXCoordinate, kSphereYCoordinate, kLowestSphereZCoordinate),
		sphereMesh->CreateModel(kLowestSphereXCoordinate, kSphereYCoordinate, kHighestSphereZCoordinate)
	};
	Direction directions[kNumSpheres] 
	{
		Direction::Up,
		Direction::Right,
		Direction::Down,
		Direction::Left,
	};

	const float kplatformXCoordinate = 0.0f; //the x-ccordinate for the platform to be placed in the scene
	const float kplatformYCoordinate = -5.0f; //the y-ccordinate for the platform to be placed in the scene
	const float kplatformZCoordinate = 0.0f; //the z-ccordinate for the platform to be placed in the scene
	const float kcubeXCoordinate = 0.0f; //the x-ccordinate for the cube to be placed in the scene
	const float kcubeYCoordinate = 5.0f; //the y-ccordinate for the cube to be placed in the scene
	const float kcubeZCoordinate = 0.0f; //the z-ccordinate for the cube to be placed in the scene
	const float kbulletXCoordinate = 0.0f; //the x-ccordinate for the bullet to be placed in the scene
	const float kbulletYCoordinate = 5.0f; //the y-ccordinate for the bullet to be placed in the scene
	const float kbulletZCoordinate = 0.0f; //the z-ccordinate for the bullet to be placed in the scene

	platformMesh = myEngine->LoadMesh("platform.x"); // initialisation of the platform for the floor that the spheres will travel around
	platformModel = platformMesh->CreateModel(kplatformXCoordinate, kplatformZCoordinate, kplatformZCoordinate); // the platoform is the floor of the whole map, slightly lower on the y-axis coodinate
	cubeMesh = myEngine->LoadMesh("Cubemesh.x");  // initialisation of the cube thats rests in the middle of the square the spheres travel around
	cubeModel = cubeMesh->CreateModel(kcubeXCoordinate, kcubeYCoordinate, kcubeZCoordinate); // cube spawns in the middle of the map and above the floor
	bulletMesh = myEngine->LoadMesh("bullet.x"); // initialisation of the bullet that is located inside of the cube upon spawning
	bulletModel = bulletMesh->CreateModel(kbulletXCoordinate, kbulletYCoordinate, kbulletZCoordinate);	// the bullet spawns inside of the cube and cannot be seen
	spheres[2]->SetSkin("super.jpg");
	spheres[3]->SetSkin("super.jpg");


	ICamera* myCamera; // initialisation of the camera fo rthe player to be looking through
	myCamera = myEngine->CreateCamera(kManual); // creating the camera, its a kManual camera - meaing it will be a constant view of the same location, never changing 

	const float kCameraXCoordinate = 0.0f; //the x-ccordinate for the camera to be placed in the scene
	const float kCameraYCoordinate = 200.0f; //the y-ccordinate for the camera to be placed in the scene
	const float kCameraZCoordinate = -170.0f; //the z-ccordinate for the camera to be placed in the scene

	myCamera->SetPosition(kCameraXCoordinate, kCameraYCoordinate, kCameraZCoordinate); // The position of the Camera the player will look from is (0, 200, -170)
	myCamera->RotateX(45.0f); // the camera is angled downwards at 45 degress so they have an angle of the floor, moving spheres and the bullets.

	const float koriginalsphereMovementSpeed = 0.1f; // original movement speed of the ball will remain constant and is used later in decreasing or increasing the speed of the spheres using A and D
	float sphereMovementSpeed = 0.1f; // Movement speed of the spheres, this changes as when A and D is pressed the speed will change
	float sphereRotationSpeed = 0.2f; // Rotation speed of the ball to mimic more realistic movement of the spheres

	const float kMaxSphereXCoordinate = 85.0f; //the max x-coordinate that the sphere can move to
	const float kMinSphereXCoordinate = -85.0f; //the min x-coordinate that the sphere can move to
	const float kMaxSphereZCoordinate = 85.0f; //the max y-coordinate that the sphere can move to
	const float kMinSphereZCoordinate = -85.0f; //the min y-coordinate that the sphere can move to

	//bool isPaused = false; // a boolean to test whether the game is paused or not (pressing P)
	//bool gameOver = false; // when the player runs out of bullets gameOver becomes true and they must start again or quit the game
	//bool gameWon = false; // when the player shoots all the spheres the gameWon boolean becomes true and then the player can either start again or quit
	bool reversedSphereMovement = false; // a boolean to test if (pressing R) has been pressed and the sphere movements will flip and travel opposite direction
	bool bulletFired = false; // a boolean to test if the bullet has been fired or not (pressing Space)
	bool bulletCollision = false; // a boolean to test if there is a collision between the bullet and the spheres
	bool isSuperSphere02 = true; // a bool to check if sphere 02 is a super version which will have be able to take two bullet collisisons before death
	bool isSuperSphere03 = true; // a bool to check if sphere 03 is a super version which will have be able to take two bullet collisisons before death
	bool restartGame = false;

	const float kbulletMovementSpeed = 5 * koriginalsphereMovementSpeed; // The bullet movement speed travels 5 times faster than the spheres original speeds, so the bullet speed will be constant, but i cant call it const as when i initialize the variable im changing the value. :/
	const float kBulletXCoordinate = 0.0f; // x-coordinate fo rthe original (never changes) spawn location of the bullet, its used when re-creating the bullet after it is de-spawned
	const float kBulletYCoordinate = 5.0f;// y-coordinate fo rthe original (never changes) spawn location of the bullet, its used when re-creating the bullet after it is de-spawned
	const float kBulletZCoordinate = 0.0f; // z-coordinate fo rthe original (never changes) spawn location of the bullet, its used when re-creating the bullet after it is de-spawned
	const float kMaxBulletZCoordinate = 200.0f; // 200 is the desired distance limit for the bullet to travel 

	float vectorX = 0; // the distance of the x coordinate of the sphere minus the bullets x-coordinate. this is used later ot find the collision boundary of the spheres and the bullet
	float vectorY = 0; // the distance of the y coordinate of the sphere minus the bullets y-coordinate. this is used later ot find the collision boundary of the spheres and the bullet
	float vectorZ = 0; // the distance of the z coordinate of the sphere minus the bullets z-coordinate. this is used later ot find the collision boundary of the spheres and the bullet

	//calculate the squares of the components first
	//note that the y-component is ignored since the cube only moves along the x and z axes
	float squaredVectorX = 0; // vectorX multiplied by vectorX used for trigonometry later
	float squaredVectorZ = 0; // vectorZ multiplied by vectorZ used for trigonometry later
	float squaredVectorY = 0; // vectorY multiplied by vectorZ used for trigonometry later
	//then take the square root of the sum of the squares to get the distance
	float distance = 0; // distance of the sphere and bullet

	float playerScore = 0; // The score of the player, they can earn score by succesfully shooting a sphere with a bullet
	const float kmaximumScore = 60; // the maximum score the player can get beofre the GameWin appears
	float remainingBullets = 10; // remaing bullets left that the player can shoot using (Up) key

	const EKeyCode kEscapeKeyHit = Key_Escape; // Avoids magic numbers for escape key presses 
	const EKeyCode kPKeyHit = Key_P; // Avoids magic numbers for P key presses 
	const EKeyCode kDKeyHit = Key_D; // Avoids magic numbers for D key presses 
	const EKeyCode kAKeyHit = Key_A; // Avoids magic numbers for A key presses 
	const EKeyCode kRKeyHit = Key_R; // Avoids magic numbers for R key presses 
	const EKeyCode kUpKeyHit = Key_Up; // Avoids magic numbers for Up key presses 
	const EKeyCode kSpaceKeyHit = Key_Space; // Avoids magic numbers for Space key presses 

	const float ksphereRadius = 15.0f; // radius of the spheres used in collision detection between bullet and sphere
	const int kspheresIndex00 = 0; // index of spheres array, slecting sphere 0, remember index of arrays start at 0
	const int kspheresIndex01 = 1; // index of spheres array, slecting sphere 1, remember index of arrays start at 0
	const int kspheresIndex02 = 2; // index of spheres array, slecting sphere 2, remember index of arrays start at 0
	const int kspheresIndex03 = 3; // index of spheres array, slecting sphere 3, remember index of arrays start at 0
	const float kTPAwaySphereVector = 4000.0f; // when a sphere is collided with, i want to to send the other sphere very far away out of view, so increasing its Y and moving it very far, does the job
	const float kLevelUpSphereSpeed = 1.25; // this is the mulitpliwer used for leveling up the sphere movement speed every time a sphere dies
	const float kscoreIncreaseAmount = 10; // every time a bullet collided with a apshere, the current score shold increase by this amount
	const float kwidthOfScreen = myEngine->GetWidth(); // width of the TL engine screen
	const float kheightOfScreen = myEngine->GetHeight(); // height of the TL engine screen
	const float kheightDifBetweenLines = 40; // just to make the format of the gameOver and gameWon screens look nice, so decreasing the height by this amount makes the page more readable and look nicer
	const float kmaximumSphereIncreaseSpeed = 2.0f; // avoids magic numbers, used in clicking (D) to increase movement speed.
	const float kminimumSphereDecreaseSpeed = 0.51f; // avoids magic numbers, used in clicking (A) to decrease movement speed. It is 0.51 rather than 0.5, as it has rounds downwards when dcereasing the speed for some reason, adding 0.01 to it solves the issue
	const float ksphereMovementSpeedMultiplier = 0.1f; // avoids magic numbers, used in clicking (D) and (A) to increase movement speed.
	const float ksphereRotationSpeedMultiplier = 0.2f; // avoids magic numbers, used in clicking (D) and (A) to increase rotation speed.
	const int kballSpeedYCoordinate = 0; // avoids magic numbers, used as a coordinate in drawing the ballSpeed string stream on the screen
	const int kcurrentScoreXCoordinate = 0; // avoids magic numbers, used as a coordinate in drawing the currentScore string stream on the screen
	const int kcurrentScoreYCoordinate = 0; // avoids magic numbers, used as a coordinate in drawing the currentScore string stream on the screen
	const int kremainingBulletsYCoordinate = 0; // avoids magic numbers, used as a coordinate in drawing the remainingBullets string stream on the screen
	

	myEngine->DrawScene();

	// The main game loop, repeat until engine is stopped
	while (myEngine->IsRunning())
	{
		// Draw the scene
		
		/**** Update your scene each frame here ****/
		myEngine->DrawScene();
		switch (currentState)
		{
		case States::Playing:
		{
			for (int i = 0; i < kNumSpheres; i++) // for loop that counts for each sphere presemt in my spheres array
			{
				switch (directions[i]) // each sphere has a different direction it will be travelling in, so sphere[0] will travel a direction Up 
				{                      // compared to sphere[1] which initially travels Right.
				case Direction::Up:
				{
					spheres[i]->MoveZ(sphereMovementSpeed); // Movement of UP is a positive Z coordiante
					spheres[i]->RotateX(sphereRotationSpeed); // Rotating the X axis makes the animation look smoother.
					if (spheres[i]->GetZ() >= kMaxSphereZCoordinate) // when the sphere reaches the Top-Left coordiante...
					{
						spheres[i]->SetZ(kMaxSphereZCoordinate); // in case the sphere goes past the maximum coordinate, the spheres z coordinate will be brought back to the same coordinate
						if (!reversedSphereMovement) // used when (R) is clicked reverse activates, otheriwse its false
						{
							directions[i] = Direction::Right; // after the upwards direction, the next direction will be Right, 
						}
						else
						{
							directions[i] = Direction::Left; // if the reverse is true, then we want the next direction to be left instead of right to readjust the course of the direction
						}
					}
					break;
				}
				case Direction::Right: // Read the Direction::Up comments 
				{
					spheres[i]->MoveX(sphereMovementSpeed);
					spheres[i]->RotateZ(-sphereRotationSpeed);
					if (spheres[i]->GetX() >= kMaxSphereXCoordinate)
					{
						spheres[i]->SetX(kMaxSphereXCoordinate);
						if (!reversedSphereMovement)
						{
							directions[i] = Direction::Down;
						}
						else
						{
							directions[i] = Direction::Up;
						}
					}
					break;
				}
				case Direction::Down:
				{
					spheres[i]->MoveZ(-sphereMovementSpeed);
					spheres[i]->RotateX(-sphereRotationSpeed);
					if (spheres[i]->GetZ() <= kMinSphereZCoordinate)
					{
						spheres[i]->SetZ(kMinSphereZCoordinate);

						if (!reversedSphereMovement)
						{
							directions[i] = Direction::Left;
						}
						else
						{
							directions[i] = Direction::Right;
						}
					}
					break;
				}
				case Direction::Left:
				{
					spheres[i]->MoveX(-sphereMovementSpeed);
					spheres[i]->RotateZ(sphereRotationSpeed);
					if (spheres[i]->GetX() <= kMinSphereXCoordinate)
					{
						spheres[i]->SetX(kMinSphereXCoordinate);

						if (!reversedSphereMovement)
						{
							directions[i] = Direction::Up;
						}
						else
						{
							directions[i] = Direction::Down;
						}
					}
					break;
				}
				}

				if (bulletFired) // when the bullet is has been shot via pressing the up key
				{
					bulletModel->MoveZ(kbulletMovementSpeed); // I'm shooting the bullet vertially forward, in the Z direction
					vectorX = spheres[i]->GetX() - bulletModel->GetX(); // i need to know the vectors of the bullet to track if its colliding with nay of the spheres
					vectorY = spheres[i]->GetY() - bulletModel->GetY(); // so finding the vector of AB = b - a, in our case its bullet to sphere = sphere coordinates - bullet coordinates
					vectorZ = spheres[i]->GetZ() - bulletModel->GetZ();

					//calculate the squares of the components first
					//note that the y-component is ignored since the cube only moves along the x and z axes
					squaredVectorX = vectorX * vectorX; // Vector^2 is used for pythagorease theorem, a^2 + b^2 = c^2 or vector = (a^2 + b^2 + c^2)^0.5
					squaredVectorZ = vectorZ * vectorZ;
					squaredVectorY = vectorY * vectorY;

					//then take the square root of the sum of the squares to get the distance
					distance = sqrt(squaredVectorX + squaredVectorZ + squaredVectorY); //vector = (a ^ 2 + b ^ 2 + c ^ 2) ^ 0.5, however the y coordinate is irrelevant as it never changes ever and is always zero.

					if (distance <= ksphereRadius) // radius of sphere is 15, so if bullet is 15 or closer than it, then there must be a collision
					{
						bulletCollision = true; // bullet collision is true when the sphere as bullet is within (touching) the radius of the sphere
						if (isSuperSphere02 && spheres[i] == spheres[kspheresIndex02]) // this is to check if the collision with the sphere is one of the super spheres the ones that can take two bullets
						{ // so if the supersphere for sphere02 is true it means the sphere is brown coloured and will not be 'destroyed' instead it will just become orange and downgrade to a regular sphere
							spheres[kspheresIndex02]->SetSkin("regular.jpg"); // skin chnages to orange
							isSuperSphere02 = false; // get rid of the supersphere being true
							bulletMesh->RemoveModel(bulletModel); // bullet is consumed in destroying the sphere, this solves issues i was struggling with before such as multipule collisions happening (as the bullet is closer than the radius of the sphere) per sceond
							bulletModel = bulletMesh->CreateModel(kBulletXCoordinate, kBulletYCoordinate, kBulletZCoordinate); // recreate the model of the bullet inside the cube
							bulletFired = false; //  the bullet is no longer being shot
						}
						else if (isSuperSphere03 && spheres[i] == spheres[kspheresIndex03]) //  all the same code as above, but with differnet varaibles
						{
							spheres[kspheresIndex03]->SetSkin("regular.jpg");
							isSuperSphere03 = false;
							bulletMesh->RemoveModel(bulletModel);
							bulletModel = bulletMesh->CreateModel(kBulletXCoordinate, kBulletYCoordinate, kBulletZCoordinate);
							bulletFired = false;
						}
						else
						{
							spheres[i]->SetLocalY(kTPAwaySphereVector); // in case the sphere is orange, a regular sphere, we will move the sphere very fast away from the current position of the rest of the game, basically moving the spheres out of view
							bulletMesh->RemoveModel(bulletModel); //  remove bullet upon collison...
							bulletModel = bulletMesh->CreateModel(kBulletXCoordinate, kBulletYCoordinate, kBulletZCoordinate); // recreate the model of the bullet inside the cube
							bulletFired = false; //  the bullet is no longer being shot
							sphereMovementSpeed *= kLevelUpSphereSpeed; // a sphere dies, so we want to increase the movement speed of the remaining sphere to be faster.
						}
					}
					if (bulletCollision) // if there is a bullet collision i want to increment player score only once per collision.
					{
						playerScore += kscoreIncreaseAmount; //  increment score amount
						bulletCollision = false; // no longer colliding with the spher, so bullet collision off
					}
					if (bulletModel->GetZ() >= kMaxBulletZCoordinate) // checking incase the bullet goes past the maximum z coordinate, we dont want a bullet to fly away endlessly into the void...
					{
						bulletMesh->RemoveModel(bulletModel); // remove the bullet
						bulletModel = bulletMesh->CreateModel(kBulletXCoordinate, kBulletYCoordinate, kBulletZCoordinate); //  repalce the bullet inside the cube
						bulletFired = false; // bullet is no longer being fired.
					}

				}
			}	
			if (remainingBullets <= 0 && playerScore < kmaximumScore && !bulletFired) // the game will be over if the players score is less than 60 (as thats when all the spheres are gone from the screen) AND the player has remainig bullets 
			{
				currentState = GameOver;
			}
			if (playerScore >= kmaximumScore) // if the player score is more than or equal to 60
			{
				currentState = GameWon;
			}
			if (myEngine->KeyHit(kPKeyHit)) // if hit (P) key, the game goes form paused to unpaused or vice versa
			{
				currentState = Paused;
			}
			if (myEngine->KeyHit(kDKeyHit)) // when you hit (D) key the speed of all the sphere and rotation pseeds increase, but only if the current sphere speed is less than 2 times the original movement speed
			{
				if (sphereMovementSpeed <= kmaximumSphereIncreaseSpeed * koriginalsphereMovementSpeed)
				{
					sphereMovementSpeed = sphereMovementSpeed + koriginalsphereMovementSpeed * ksphereMovementSpeedMultiplier;
					sphereRotationSpeed = sphereRotationSpeed + koriginalsphereMovementSpeed * ksphereRotationSpeedMultiplier;
				}
			}
			if (myEngine->KeyHit(kAKeyHit))// when you hit (A) key the speed of all the sphere and rotation speeds decrease, but only if the current sphere speed is more than 1/2 times the original movement speed
			{
				if (sphereMovementSpeed >= kminimumSphereDecreaseSpeed * koriginalsphereMovementSpeed)
				{
					sphereMovementSpeed = sphereMovementSpeed - koriginalsphereMovementSpeed * ksphereMovementSpeedMultiplier;
					sphereRotationSpeed = sphereRotationSpeed - koriginalsphereMovementSpeed * ksphereRotationSpeedMultiplier;
				}
			}
			if (myEngine->KeyHit(kRKeyHit)) // hitting (R) key reverses the spheres...
			{
				reversedSphereMovement = !reversedSphereMovement; // reverse bool switches from on to ff or vise versa
				for (int i = 0; i < kNumSpheres; i++) // for each sphere in my spheres array
				{
					switch (directions[i])
					{
					case Direction::Up:
					{
						directions[i] = Direction::Down; // the current direction its travelling will reverse Up to Down etc...
						break; //                           this is needed or else the sphere will continue travelling UP until it reaches the up boundary, and then travel downwards
					}
					case Direction::Right:
					{
						directions[i] = Direction::Left;
						break;
					}
					case Direction::Down:
					{
						directions[i] = Direction::Up;
						break;
					}
					case Direction::Left:
					{
						directions[i] = Direction::Right;
						break;
					}
					}
				}
			}
			if (myEngine->KeyHit(kUpKeyHit)) // Clicking the (Up) key 
			{
				if (remainingBullets > 0)
				{
					if (!bulletFired) // as long as there isnt already a bullet fired, then a bullet will be fired, as we dont want mutilpule bullets being shot at once.
					{
						remainingBullets -= 1; // remaining bullets decreases
						bulletFired = true; // bullet fired becomes true
					}

				}
			}
			break;
		}
		case States::Paused:
		{
			stringstream sspausedScreen;
			sspausedScreen << "Game is paused"; // displays the paused string on screen
			myFont->Draw(sspausedScreen.str(), myEngine->GetWidth() / 2, myEngine->GetHeight() / 2, kWhite, kCentre); // draws the paused screen string, in the middl eof the screen cemtralized

			if (myEngine->KeyHit(kPKeyHit)) // if hit (P) key, the game goes form paused to unpaused or vice versa
			{
				currentState = Playing;
			}
			break;
		}
		case States::GameWon:
		{
			stringstream ssgameWonScreen;
			stringstream ssgameWonScreen2;
			ssgameWonScreen << "You Win! Your current score was: " << playerScore; // displaying the player score on screen
			ssgameWonScreen2 << "(Esc to exit game)   (Space to play again) "; // displaying the text on screen (the prompts of what to do next)
			myFont->Draw(ssgameWonScreen.str(), myEngine->GetWidth() / 2, myEngine->GetHeight() / 2, kWhite, kCentre); // drawing the gamewon info on screen, in the middle of the sceen centralized
			myFont->Draw(ssgameWonScreen2.str(), myEngine->GetWidth() / 2, myEngine->GetHeight() / 2 + kheightDifBetweenLines, kWhite, kCentre);
			
			if (myEngine->KeyHit(kSpaceKeyHit)) // when (Space) key is hit for the restarting the game
			{
				restartGame = true;
				currentState = Playing;
				break; // breaks the user out of the running while loop 
			}
			break;
		}
		case States::GameOver:
		{
			stringstream ssgameOverScreen;
			stringstream ssgameOverScreen2;
			ssgameOverScreen << "You Lost. Your current score was: " << playerScore; // displaying the player score on screen
			ssgameOverScreen2 << "(Esc to exit game)   (Space to play again) "; // displaying the text on screen (the prompts of what to do next)
			myFont->Draw(ssgameOverScreen.str(), kwidthOfScreen / 2, kheightOfScreen / 2, kWhite, kCentre); // drawing the gameover info on screen, in the middle of the sceen centralized
			myFont->Draw(ssgameOverScreen2.str(), kwidthOfScreen / 2, kheightOfScreen / 2 + kheightDifBetweenLines, kWhite, kCentre);

			if (myEngine->KeyHit(kSpaceKeyHit)) // when (Space) key is hit for the restarting the game
			{
				restartGame = true;
				currentState = Playing;			
				break; // breaks the user out of the running while loop 
			}
			break;
		}
		}
	
		stringstream ssballSpeed; // string for the ball speed
		ssballSpeed << "Speed of Balls: " << sphereMovementSpeed; // displaying the spped of the balls on screen
		myFont->Draw(ssballSpeed.str(), kwidthOfScreen, kballSpeedYCoordinate, kWhite, kRight); // drawing the speed of the balls as a font on screen, at the top right of the screen

		stringstream sscurrentScore; //  string of the current score 
		sscurrentScore << "Score: " << playerScore; // displaying the score of the player on screen
		myFont->Draw(sscurrentScore.str(), kcurrentScoreXCoordinate, kcurrentScoreYCoordinate, kWhite, kLeft); // drawing the current score as a font on the screen, at the top left of the screen

		stringstream ssremainingBullets; // string of the remaining bullets on screen
		ssremainingBullets << "Remaining bullets: " << remainingBullets; //  displays the remaining bullets on screen
		myFont->Draw(ssremainingBullets.str(), kwidthOfScreen / 2, kremainingBulletsYCoordinate, kWhite, kCentre); // drawing the remaining bullets on the screen, at the top middle of the screen



		if (myEngine->KeyHit(kEscapeKeyHit)) //if the key Escape key is hit
		{
			myEngine->Stop(); //exit the program
		}
		if (restartGame) // restarting requires me to re-initialise all the variables and the coordinate locations of the spheres
		{
			sphereMovementSpeed = 0.1f; // Movement speed of the spheres, this changes as when A and D is pressed the speed will change
			sphereRotationSpeed = 0.2f; // Rotation speed of the ball to mimic more realistic movement of the spheres
			reversedSphereMovement = false; // a boolean to test if (pressing R) has been pressed and the sphere movements will flip and travel opposite direction
			bulletFired = false; // a boolean to test if the bullet has been fired or not (pressing Space)
			bulletCollision = false; // a boolean to test if there is a collision between the bullet and the spheres
			isSuperSphere02 = true; // a bool to check if sphere 02 is a super version which will have be able to take two bullet collisisons before death
			isSuperSphere03 = true; // a bool to check if sphere 03 is a super version which will have be able to take two bullet collisisons before death
			playerScore = 0; // The score of the player, they can earn score by succesfully shooting a sphere with a bullet
			remainingBullets = 10; // remaing bullets left that the player can shoot using (Up) key

			for (int i = 0; i < kNumSpheres; i++)
			{
				switch (directions[i])  // |_ sphere travels left it changes to right >:(
				{                      
				case Direction::Up:
				{
					directions[i] = Direction::Down; 
					break;
				}
				case Direction::Right:
				{
					directions[i] = Direction::Left;
					break;
				}
				case Direction::Down:
				{
					directions[i] = Direction::Up;
					break;
				}
				case Direction::Left:
				{
					directions[i] = Direction::Right;
					break;
				}
				}
			}
			spheres[kspheresIndex00]->SetLocalPosition(kLowestSphereXCoordinate, kSphereYCoordinate, kLowestSphereZCoordinate);
			spheres[kspheresIndex01]->SetLocalPosition(kHighestSphereXCoordinate, kSphereYCoordinate, kHighestSphereZCoordinate);
			spheres[kspheresIndex02]->SetLocalPosition(kHighestSphereXCoordinate, kSphereYCoordinate, kLowestSphereZCoordinate);
			spheres[kspheresIndex03]->SetLocalPosition(kLowestSphereXCoordinate, kSphereYCoordinate, kHighestSphereZCoordinate);
			spheres[kspheresIndex02]->SetSkin("super.jpg");
			spheres[kspheresIndex03]->SetSkin("super.jpg");

			currentState = Playing;
            restartGame = false;
		}
	}
	// Delete the 3D engine now we are finished with it
	myEngine->Delete();
}
