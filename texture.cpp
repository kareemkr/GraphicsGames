#include <stdio.h>
#include <cmath>
#include <cstdlib>
#include <ctime>
#include <glut.h>


bool gameOver = false;
bool gameLost = false;
float playerY = 100.0f;
bool isPlayerJumping = false;
bool isPlayerDucking = false;
float jumpDistance = 70.0f;
float jumpPosition = 0.0f;
int playerHealth = 5;
int playerScore = 0;
float scoreMultiplier = 1.0f;
int pickedUpItemsCount = 0;
int totalTime = 40;
int remainingTime = totalTime;
int frame = 0;
int noDamageDuration = 3;
int doubleScoreDuration = 3;
int noDamageTimer = 0;
int doubleScoreTimer = 0;
bool isNoDamageActive = false; 
float speed = 0.75f;
float obstacleX = 300.0f;
float itemX = 350.0f;
float obstacleHeight = 90.0f;
float backgroundOffset = 0.0f;
float backgroundSpeed = 0.02f;
bool isCollidingWithObstacle = false;


enum PickupType { EXTRA_SCORE, NO_DAMAGE, DOUBLE_SCORE };
PickupType currentItemType;

void respwanObstacle() {
    obstacleX = 300.0f;
    obstacleHeight = 90.0f + rand() % 65;
}
void drawPlayer() {
    float bodyHeight = isPlayerDucking ? 10.0f : 25.0f;
    float bodyWidth = 15.0f;
    float headRadius = isPlayerDucking ? 5.0f : 7.0f;
    float headY = playerY + bodyHeight + headRadius;

    // Draw body (rectangle)
    glColor3f(0.1f, 0.1f, 0.1f);  // Dark color for the body
    glBegin(GL_QUADS);
    glVertex3f(55.0f - bodyWidth / 2, playerY, 0.0f);
    glVertex3f(55.0f + bodyWidth / 2, playerY, 0.0f);
    glVertex3f(55.0f + bodyWidth / 2, playerY + bodyHeight, 0.0f);
    glVertex3f(55.0f - bodyWidth / 2, playerY + bodyHeight, 0.0f);
    glEnd();

    // Draw head (circle)
    glColor3f(0.0f, 0.0f, 0.0f);  // Black head
    glBegin(GL_TRIANGLE_FAN);
    glVertex3f(55.0f, headY, 0.0f);
    for (int i = 0; i <= 360; i += 10) {
        float theta = i * 3.14159f / 180.0f;
        float x = headRadius * cos(theta);
        float y = headRadius * sin(theta);
        glVertex3f(55.0f + x, headY + y, 0.0f);
    }
    glEnd();

    // Draw eyes (small white circles)
    glColor3f(1.0f, 1.0f, 1.0f);  // White for eyes
    glBegin(GL_QUADS);
    glVertex3f(52.5f, headY + 2.0f, 0.0f);  // Left eye
    glVertex3f(53.5f, headY + 2.0f, 0.0f);
    glVertex3f(53.5f, headY + 3.5f, 0.0f);
    glVertex3f(52.5f, headY + 3.5f, 0.0f);

    glVertex3f(56.5f, headY + 2.0f, 0.0f);  // Right eye
    glVertex3f(57.5f, headY + 2.0f, 0.0f);
    glVertex3f(57.5f, headY + 3.5f, 0.0f);
    glVertex3f(56.5f, headY + 3.5f, 0.0f);
    glEnd();

    // Draw legs (rectangles)
    glColor3f(0.1f, 0.1f, 0.1f);  // Same color as the body
    glBegin(GL_QUADS);
    glVertex3f(52.0f, playerY - 10.0f, 0.0f);  // Left leg
    glVertex3f(54.0f, playerY - 10.0f, 0.0f);
    glVertex3f(54.0f, playerY, 0.0f);
    glVertex3f(52.0f, playerY, 0.0f);

    glVertex3f(56.0f, playerY - 10.0f, 0.0f);  // Right leg
    glVertex3f(58.0f, playerY - 10.0f, 0.0f);
    glVertex3f(58.0f, playerY, 0.0f);
    glVertex3f(56.0f, playerY, 0.0f);
    glEnd();
}
void drawUpperBoundary() {
    // Draw the gradient background of the upper boundary
    glBegin(GL_QUADS);
    for (int i = 0; i <= 300; i += 30) {
        // A gradient effect by adjusting the color incrementally
        glColor3f(0.0f, 0.7f - i * 0.002f, 0.7f - i * 0.002f);
        glVertex3f(i, 300.0f, 0.0f);
        glColor3f(0.0f, 0.5f - i * 0.002f, 0.5f - i * 0.002f);
        glVertex3f(i + 30, 300.0f, 0.0f);
        glColor3f(0.0f, 0.5f - i * 0.002f, 0.5f - i * 0.002f);
        glVertex3f(i + 30, 285.0f, 0.0f);
        glColor3f(0.0f, 0.7f - i * 0.002f, 0.7f - i * 0.002f);
        glVertex3f(i, 285.0f, 0.0f);
    }
    glEnd();

    // Draw a wavy pattern along the upper boundary
    glColor3f(1.0f, 0.9f, 0.0f);
    glBegin(GL_LINE_STRIP);
    for (int i = 0; i <= 300; i += 10) {
        float waveHeight = 5.0f * sin(i * 0.2f);
        glVertex3f(i, 295.0f + waveHeight, 0.0f);
    }
    glEnd();

    // Add a dashed line below the wavy pattern for more detail
    glColor3f(1.0f, 1.0f, 1.0f);
    glLineWidth(2.0f);
    glBegin(GL_LINES);
    for (int i = 0; i <= 300; i += 20) {
        glVertex3f(i, 285.0f, 0.0f);
        glVertex3f(i + 10, 285.0f, 0.0f);
    }
    glEnd();

    // Add small dots for extra visual texture
    glColor3f(0.9f, 0.9f, 0.0f);
    glPointSize(3.0f);
    glBegin(GL_POINTS);
    for (int i = 0; i <= 300; i += 15) {
        glVertex3f(i, 295.0f, 0.0f);
    }
    glEnd();
}

void drawLowerBoundary() {
    // Draw a gradient background for the lower boundary
    glBegin(GL_QUADS);
    for (int i = 0; i <= 300; i += 30) {
        glColor3f(0.0f, 0.7f - i * 0.002f, 0.7f - i * 0.002f);
        glVertex3f(i, 10.0f, 0.0f);
        glColor3f(0.0f, 0.5f - i * 0.002f, 0.5f - i * 0.002f);
        glVertex3f(i + 30, 10.0f, 0.0f);
        glColor3f(0.0f, 0.5f - i * 0.002f, 0.5f - i * 0.002f);
        glVertex3f(i + 30, 0.0f, 0.0f);
        glColor3f(0.0f, 0.7f - i * 0.002f, 0.7f - i * 0.002f);
        glVertex3f(i, 0.0f, 0.0f);
    }
    glEnd();

    // Draw a zig-zag pattern in the middle of the lower boundary
    glColor3f(1.0f, 0.8f, 0.0f);
    glBegin(GL_LINE_STRIP);
    for (int i = 0; i <= 300; i += 10) {
        float zigzagHeight = (i % 20 == 0) ? 5.0f : 0.0f;  // Alternate up and down
        glVertex3f(i, 5.0f + zigzagHeight, 0.0f);
    }
    glEnd();

    // Add dotted lines at the bottom for more texture
    glColor3f(1.0f, 1.0f, 1.0f);
    glPointSize(3.0f);
    glBegin(GL_POINTS);
    for (int i = 0; i <= 300; i += 15) {
        glVertex3f(i, 1.0f, 0.0f);
    }
    glEnd();

    // Draw a solid line to mark the bottom boundary
    glColor3f(1.0f, 1.0f, 1.0f);
    glLineWidth(2.0f);
    glBegin(GL_LINES);
    glVertex3f(0.0f, 10.0f, 0.0f);
    glVertex3f(300.0f, 10.0f, 0.0f);
    glEnd();
}

void drawObstacle() {
    // Base of the obstacle (a cube with gradient colors)
    glBegin(GL_QUADS);
    glColor3f(0.2f, 0.6f, 0.6f); // Gradient color
    glVertex3f(obstacleX, obstacleHeight, 0.0f);
    glColor3f(0.1f, 0.4f, 0.4f);
    glVertex3f(obstacleX + 20.0f, obstacleHeight, 0.0f);
    glColor3f(0.0f, 0.3f, 0.3f);
    glVertex3f(obstacleX + 20.0f, obstacleHeight + 20.0f, 0.0f);
    glColor3f(0.1f, 0.4f, 0.4f);
    glVertex3f(obstacleX, obstacleHeight + 20.0f, 0.0f);
    glEnd();

    // Adding diagonal lines for texture
    glColor3f(1.0f, 1.0f, 0.0f); // Yellow diagonal lines
    glLineWidth(2.0f);
    glBegin(GL_LINES);
    glVertex3f(obstacleX, obstacleHeight, 0.0f);
    glVertex3f(obstacleX + 20.0f, obstacleHeight + 20.0f, 0.0f);

    glVertex3f(obstacleX, obstacleHeight + 20.0f, 0.0f);
    glVertex3f(obstacleX + 20.0f, obstacleHeight, 0.0f);
    glEnd();

    // Adding a rotating pentagon on top for visual effect
    glPushMatrix();
    glTranslatef(obstacleX + 10.0f, obstacleHeight + 25.0f, 0.0f); // Move to center above the cube
    glRotatef(45.0f, 0.0f, 0.0f, 1.0f); // Rotate the pentagon for effect
    glColor3f(0.8f, 0.2f, 0.2f); // Red pentagon
    glBegin(GL_POLYGON);
    for (int i = 0; i < 5; ++i) {
        float angle = i * (2.0f * 3.14159f / 5.0f) - (3.14159f / 2.0f);
        float radius = 7.0f;
        glVertex3f(radius * cos(angle), radius * sin(angle), 0.0f);
    }
    glEnd();
    glPopMatrix();

    // Draw a small circle at the center of the cube
    glColor3f(1.0f, 0.0f, 0.0f); // Red circle
    glBegin(GL_TRIANGLE_FAN);
    glVertex3f(obstacleX + 10.0f, obstacleHeight + 10.0f, 0.0f); // Center of the circle
    for (int i = 0; i <= 360; i += 30) {
        float theta = i * 3.14159f / 180.0f;
        float radius = 3.0f;
        glVertex3f(obstacleX + 10.0f + radius * cos(theta), obstacleHeight + 10.0f + radius * sin(theta), 0.0f);
    }
    glEnd();

    // Add small dotted points for texture
    glPointSize(5.0f);
    glColor3f(0.9f, 0.9f, 0.9f); // Light grey points
    glBegin(GL_POINTS);
    glVertex3f(obstacleX + 5.0f, obstacleHeight + 5.0f, 0.0f);
    glVertex3f(obstacleX + 15.0f, obstacleHeight + 5.0f, 0.0f);
    glVertex3f(obstacleX + 5.0f, obstacleHeight + 15.0f, 0.0f);
    glVertex3f(obstacleX + 15.0f, obstacleHeight + 15.0f, 0.0f);
    glEnd();
}

void randomizePickUpType() {

    if (pickedUpItemsCount >= 5) {
        currentItemType = (rand() % 2 == 0) ? NO_DAMAGE : DOUBLE_SCORE;
        pickedUpItemsCount = 0;
    }
    else {
        int randomValue = rand() % 100;

        if (randomValue < 60) {
            currentItemType = EXTRA_SCORE;
            pickedUpItemsCount++;
        }
        else if (randomValue < 80 && currentItemType != NO_DAMAGE) {
            currentItemType = NO_DAMAGE;
        }
        else if (randomValue < 100 && currentItemType != DOUBLE_SCORE) {
            currentItemType = DOUBLE_SCORE;
        }
        else {
            currentItemType = EXTRA_SCORE;
            pickedUpItemsCount++;
        }
    }
}

void drawPickupItem() {
    static float rotationAngle = 0.0f;

    // Coin color (golden)
    glColor3f(1.0f, 0.84f, 0.0f);  // Gold color

    // Push the matrix to apply transformations
    glPushMatrix();

    // Translate to collectable position
    glTranslatef(itemX, 150.0f, 0.0f);

    // Apply rotation for the spinning effect
    glRotatef(rotationAngle, 0.0f, 1.0f, 0.0f);

    // Draw the circular coin (filled)
    glBegin(GL_TRIANGLE_FAN);
    glVertex3f(0.0f, 0.0f, 0.0f);  // Center of the coin
    for (int i = 0; i <= 360; i += 10) {
        float theta = i * 3.14159f / 180.0f;
        float x = 10.0f * cos(theta);  // Radius of 10 units
        float y = 10.0f * sin(theta);
        glVertex3f(x, y, 0.0f);
    }
    glEnd();

    // Draw the outer edge of the coin
    glColor3f(0.9f, 0.75f, 0.0f);  // Slightly darker gold for the edge
    glBegin(GL_LINE_LOOP);
    for (int i = 0; i <= 360; i += 10) {
        float theta = i * 3.14159f / 180.0f;
        float x = 10.0f * cos(theta);  // Same radius
        float y = 10.0f * sin(theta);
        glVertex3f(x, y, 0.0f);
    }
    glEnd();

    // Pop the matrix to undo transformations
    glPopMatrix();

    // Update the rotation angle for continuous spinning
    rotationAngle += 2.0f;
    if (rotationAngle > 360.0f) {
        rotationAngle -= 360.0f;
    }
}


void drawNoDamageItem() {
    static float scaleFactor = 1.0f;
    static bool growing = true;

    glPushMatrix();
    glTranslatef(itemX, 150.0f, 0.0f);
    glScalef(scaleFactor, scaleFactor, 1.0f);
    glTranslatef(-itemX, -150.0f, 0.0f);

    // Draw the outer circle (invincibility power-up)
    glColor3f(0.0f, 0.7f, 1.0f);  // Light blue color
    glBegin(GL_TRIANGLE_FAN);
    glVertex3f(itemX, 150.0f, 0.0f);  // Center of the circle
    for (int i = 0; i <= 360; i += 10) {
        float theta = i * 3.14159f / 180;
        glVertex3f(itemX + 12.0f * cos(theta), 150.0f + 12.0f * sin(theta), 0.0f);
    }
    glEnd();

    // Draw the inner star shape
    glColor3f(1.0f, 1.0f, 0.0f);  // Yellow color
    glBegin(GL_TRIANGLE_FAN);
    glVertex3f(itemX, 150.0f, 0.0f);  // Center of the star
    for (int i = 0; i < 8; i++) {
        float outerAngle = i * (2.0f * 3.14159f / 8);
        float innerAngle = outerAngle + (3.14159f / 8);
        glVertex3f(itemX + 6.0f * cos(outerAngle), 150.0f + 6.0f * sin(outerAngle), 0.0f);
        glVertex3f(itemX + 12.0f * cos(innerAngle), 150.0f + 12.0f * sin(innerAngle), 0.0f);
    }
    glEnd();

    // Draw glowing effect (light blue halo)
    glColor4f(0.0f, 0.7f, 1.0f, 0.3f);  // Semi-transparent light blue
    glBegin(GL_TRIANGLE_FAN);
    glVertex3f(itemX, 150.0f, 0.0f);  // Center for halo
    for (int i = 0; i <= 360; i += 10) {
        float theta = i * 3.14159f / 180;
        glVertex3f(itemX + 16.0f * cos(theta), 150.0f + 16.0f * sin(theta), 0.0f);
    }
    glEnd();

    glPopMatrix();

    // Scale effect for growing/shrinking
    if (growing) {
        scaleFactor += 0.02f;
        if (scaleFactor >= 1.3f) {
            growing = false;
        }
    }
    else {
        scaleFactor -= 0.02f;
        if (scaleFactor <= 1.0f) {
            growing = true;
        }
    }
}

void drawDoubleScoreItem() {
    static float bounceOffset = 0.0f;
    static float bounceSpeed = 0.05f;
    static bool movingUp = true;

    // Bouncing effect
    if (movingUp) {
        bounceOffset += bounceSpeed;
        if (bounceOffset > 5.0f) {
            movingUp = false;
        }
    }
    else {
        bounceOffset -= bounceSpeed;
        if (bounceOffset < -5.0f) {
            movingUp = true;
        }
    }

    glPushMatrix();
    glTranslatef(itemX, 150.0f + bounceOffset, 0.0f);

    // Draw outer circle (base of the collectible)
    glColor3f(1.0f, 0.85f, 0.0f);  // Gold color
    glBegin(GL_TRIANGLE_FAN);
    glVertex3f(0.0f, 0.0f, 0.0f);  // Center
    for (int i = 0; i <= 360; i += 10) {
        float theta = i * 3.14159f / 180;
        glVertex3f(15.0f * cos(theta), 15.0f * sin(theta), 0.0f);
    }
    glEnd();

    // Draw inner circle (playerScore indicator)
    glColor3f(1.0f, 0.7f, 0.0f);  // Darker gold
    glBegin(GL_TRIANGLE_FAN);
    glVertex3f(0.0f, 0.0f, 0.0f);  // Center
    for (int i = 0; i <= 360; i += 10) {
        float theta = i * 3.14159f / 180;
        glVertex3f(10.0f * cos(theta), 10.0f * sin(theta), 0.0f);
    }
    glEnd();

    // Draw "SCORE" text using quads
    glColor3f(1.0f, 1.0f, 1.0f);  // White color for text
    glBegin(GL_QUADS);
    glVertex3f(-5.0f, 2.0f, 0.0f);
    glVertex3f(5.0f, 2.0f, 0.0f);
    glVertex3f(5.0f, 4.0f, 0.0f);
    glVertex3f(-5.0f, 4.0f, 0.0f);
    glEnd();

    // Draw playerScore lines (dashes) on either side of "SCORE"
    glBegin(GL_LINES);
    glVertex3f(-8.0f, 3.0f, 0.0f);
    glVertex3f(-4.0f, 3.0f, 0.0f);
    glVertex3f(4.0f, 3.0f, 0.0f);
    glVertex3f(8.0f, 3.0f, 0.0f);
    glEnd();

    glPopMatrix();
}

void drawLayout() {
    int windowWidth = 300;
    int windowHeight = 300;
    float healthBarRadius = 15.0f; // Radius for playerHealth points
    int healthBarX = windowWidth / 2;
    int healthBarY = windowHeight - 40; // Base Y position for the playerHealth bar

    // Draw empty playerHealth segments
    glColor3f(1.0f, 1.0f, 1.0f); // White color for outline
    for (int i = 0; i < 5; i++) {
        glBegin(GL_LINE_LOOP);
        for (int angle = 0; angle <= 360; angle += 10) {
            float theta = angle * 3.14159f / 180; // Convert to radians
            glVertex3f(healthBarX + (i - 2) * 30 + healthBarRadius * cos(theta),
                healthBarY + healthBarRadius * sin(theta), 0.0f);
        }
        glEnd();
    }

    // Draw filled playerHealth segments
    glColor3f(1.0f, 0.0f, 0.0f); // Red color for playerHealth
    for (int i = 0; i < playerHealth; i++) {
        glBegin(GL_TRIANGLE_FAN);
        glVertex3f(healthBarX + (i - 2) * 30, healthBarY, 0.0f); // Center point
        for (int angle = 0; angle <= 360; angle += 10) {
            float theta = angle * 3.14159f / 180; // Convert to radians
            glVertex3f(healthBarX + (i - 2) * 30 + healthBarRadius * cos(theta),
                healthBarY + healthBarRadius * sin(theta), 0.0f);
        }
        glEnd();
    }

    // Centered Score Text
    char scoreStr[50];
    snprintf(scoreStr, sizeof(scoreStr), "Score: %d", playerScore);
    glColor3f(0.0f, 0.0f, 0.0f);

    int scoreTextWidth = glutBitmapLength(GLUT_BITMAP_HELVETICA_18, (const unsigned char*)scoreStr);
    int scoreX = (windowWidth - scoreTextWidth) / 2;

    glRasterPos2f(scoreX, windowHeight - 60);
    for (int i = 0; scoreStr[i] != '\0'; i++) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, scoreStr[i]);
    }

    int minutes = remainingTime / 60;
    int seconds = remainingTime % 60;
    char timerStr[50];
    snprintf(timerStr, sizeof(timerStr), "Time: %02d:%02d", minutes, seconds);

    int timerTextWidth = glutBitmapLength(GLUT_BITMAP_HELVETICA_18, (const unsigned char*)timerStr);
    int timerX = (windowWidth - timerTextWidth) / 2;

    glRasterPos2f(timerX, windowHeight - 80);
    for (int i = 0; timerStr[i] != '\0'; i++) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, timerStr[i]);
    }

    // Centered Power-Up Status
    if (isNoDamageActive && scoreMultiplier == 2.0f) {
        snprintf(timerStr, sizeof(timerStr), "Shield Active: %d s, Double Score Active: %d s", noDamageTimer / 60, doubleScoreTimer / 60);
    }
    else if (isNoDamageActive) {
        snprintf(timerStr, sizeof(timerStr), "Shield Active: %d s", noDamageTimer / 60);
    }
    else if (scoreMultiplier == 2.0f) {
        snprintf(timerStr, sizeof(timerStr), "Double Score Active: %d s", doubleScoreTimer / 60);
    }
    else {
        snprintf(timerStr, sizeof(timerStr), "No Active Power-Up");
    }

    int powerUpTextWidth = glutBitmapLength(GLUT_BITMAP_HELVETICA_18, (const unsigned char*)timerStr);
    int powerUpX = (windowWidth - powerUpTextWidth) / 2;

    glRasterPos2f(powerUpX, windowHeight - 100);
    for (int i = 0; timerStr[i] != '\0'; i++) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, timerStr[i]);
    }
}

void drawGameOverScreen() {
    glColor3f(1.0f, 0.84f, 0.0f);

    char endStr[50];
    if (gameLost) {
        snprintf(endStr, sizeof(endStr), "Game Lose - Score: %d", playerScore);
    }
    else {
        snprintf(endStr, sizeof(endStr), "Game End - Score: %d", playerScore);
    }
    glRasterPos2f(100, 150);
    for (int i = 0; endStr[i] != '\0'; i++) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, endStr[i]);
    }
}
void drawBackground() {
    // Simple gradient from top (light blue) to bottom (darker blue)
    for (float y = 0.0f; y <= 300.0f; y += 1.0f) {
        float colorValue = (y / 300.0f);
        // Gradient from light blue at the top to a darker blue at the bottom
        glColor3f(0.0f, 0.5f + 0.5f * colorValue, 1.0f);

        glBegin(GL_LINES);
        glVertex3f(0.0f, y + backgroundOffset, 0.0f);
        glVertex3f(300.0f, y + backgroundOffset, 0.0f);
        glEnd();
    }

    // Increment background offset for scrolling effect
    backgroundOffset += backgroundSpeed;
    if (backgroundOffset > 300.0f) {
        backgroundOffset -= 300.0f;  // Reset once we reach the end of the gradient
    }
}

void Display() {
    glClear(GL_COLOR_BUFFER_BIT);

    drawBackground();

    if (gameOver) {
        drawGameOverScreen();
    }
    else {
        drawUpperBoundary();
        drawLowerBoundary();
        drawPlayer();
        drawObstacle();
        drawLayout();
        switch (currentItemType) {
        case EXTRA_SCORE:
            drawPickupItem();
            break;
        case NO_DAMAGE:
            drawNoDamageItem();
            break;
        case DOUBLE_SCORE:
            drawDoubleScoreItem();
            break;
        }
    }

    glFlush();
}
void Timer(int value) {
    frame++;

    if (gameOver) {
        glutPostRedisplay();
        return;
    }

    if (isPlayerJumping) {
        if (jumpPosition < jumpDistance && playerY + 40.0f <= 280.0f) {
            playerY += 5.0f;
            jumpPosition += 5.0f;
        }
        else {
            isPlayerJumping = false;
        }
    }
    else if (playerY > 100.0f) {
        playerY -= 7.5f;
        if (playerY < 100.0f) {
            playerY = 100;
        }
    }

    if (!isCollidingWithObstacle) {
        obstacleX -= speed;
        itemX -= speed;

        if (obstacleX < -20.0f) {
            respwanObstacle();
            glColor3f(0.8f, 0.3f, 0.3f);
        }

        if (itemX < -20.0f) {
            itemX = 350.0f + rand() % 100;
            randomizePickUpType();
            glColor3f(1.0f, 1.0f, 0.3f);
        }

        if (abs(itemX - obstacleX) < 30.0f) {
            itemX = obstacleX + 50.0f;
        }

        speed += 0.01f;
    }

    if (obstacleX < 70.0f && obstacleX + 15.0f > 50.0f) {
        float playerHeight = isPlayerDucking ? 25.0f : 40.0f;
        if (playerY < obstacleHeight + 25.0f && playerY + playerHeight > obstacleHeight) {
            if (!isNoDamageActive && !isCollidingWithObstacle) {
                playerHealth--;
                isCollidingWithObstacle = true;
                if (playerHealth == 0) {
                    gameLost = true;
                    gameOver = true;
                }
            }
            if (isNoDamageActive && !isCollidingWithObstacle) {
                isCollidingWithObstacle = true;
            }
        }
        else {
            isCollidingWithObstacle = false;
        }
    }

    if (itemX < 70.0f && itemX + 10.0f > 50.0f && playerY + 40.0f > 140.0f && playerY < 160.0f) {
        if (currentItemType == NO_DAMAGE) {
            isNoDamageActive = true;
            noDamageTimer = noDamageDuration * 60;
        }
        else if (currentItemType == DOUBLE_SCORE) {
            scoreMultiplier = 2.0f;
            doubleScoreTimer = doubleScoreDuration * 60;
        }

        playerScore += 1 * scoreMultiplier;
        itemX = 350.0f;
        randomizePickUpType();
    }

    if (noDamageTimer > 0) {
        noDamageTimer--;
        if (noDamageTimer == 0) {
            if (isNoDamageActive) {
                isNoDamageActive = false;
            }
        }
    }

    if (doubleScoreTimer > 0) {
        doubleScoreTimer--;
        if (doubleScoreTimer == 0) {
            if (scoreMultiplier == 2.0f) {
                scoreMultiplier = 1.0f;
            }
        }
    }

    if (playerHealth == 0 || remainingTime == 0) {
        scoreMultiplier = 1.0f;
    }

    if (frame == 60 && remainingTime > 0) {
        remainingTime--;
        frame = 0;
    }

    if (remainingTime == 0) {
        gameOver = true;
    }

    glutPostRedisplay();
    glutTimerFunc(1000 / 60, Timer, 0);
}
void Keyboard(unsigned char key, int x, int y) {
    if (key == ' ' && !isPlayerJumping && playerY == 100.0f) {
        isPlayerJumping = true;
        jumpPosition = 0.0f;
    }
    else if (key == 's') {
        isPlayerDucking = true;
    }
}
void duckInput(unsigned char key, int x, int y) {
    if (key == 's') {
        isPlayerDucking = false;
        playerY = 100.0f;
    }
}
int main(int argc, char** argv) {

    srand(static_cast<unsigned int>(time(0)));

    glutInit(&argc, argv);
    glutInitWindowSize(450, 300);
    glutInitWindowPosition(150, 150);
    glutCreateWindow("2D Infinite Runner");
    glutDisplayFunc(Display);
    glutKeyboardFunc(Keyboard);
    glutKeyboardUpFunc(duckInput);
    glutTimerFunc(0, Timer, 0);
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    gluOrtho2D(0.0, 300, 0.0, 300);

    glutMainLoop();
    return 0;
}