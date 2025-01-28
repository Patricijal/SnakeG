// SnakeG.cpp

#include <iostream>
#include <vector>
#include <conio.h>
#include <thread>
#include <chrono>
using namespace std;

class SnakeGame
{
private:
    bool isGameOver, playerLost, computerLost, playerBombPaused, computerBombPaused;
    int width, height, maxPoints;
    int snakeHeadX, snakeHeadY, computerSnakeHeadX, computerSnakeHeadY;
    int foodX, foodY;
    int score, computerScore;
    vector<int> snakeTailX, snakeTailY, computerSnakeTailX, computerSnakeTailY;
    int snakeTailLength, computerSnakeTailLength;
    enum eDirection {
        STOP = 0,
        LEFT,
        RIGHT,
        UP,
        DOWN
    };
    eDirection dir;
    int bombX, bombY;
    char** gameBoard;

public:
    SnakeGame() { Setup(); }
    ~SnakeGame() {}
    bool getIsGameOver() { return isGameOver;  }
    bool playerPaused() { return playerBombPaused;  }
    bool computerPaused() { return computerBombPaused;  }
    void Setup()
    {
        isGameOver = false;
        playerLost = false;
        computerLost = false;
        playerBombPaused = false;
        computerBombPaused = false;

        score = 0;
        computerScore = 0;
        maxPoints = 10;
        
        width = 20;
        height = 10;

        dir = STOP;

        snakeHeadX = width / 3;
        snakeHeadY = height / 2;

        computerSnakeHeadX = width / 3 * 2;
        computerSnakeHeadY = height / 2;

        //srand(time(NULL));
        srand(static_cast<unsigned int>(time(NULL)));
        foodX = (rand() % (width - 1)) + 1;
        foodY = (rand() % (height - 1)) + 1;

        do {
            bombX = foodX + rand() % 3 - 1; // -1, 0, 1 offset from foodX
            bombY = foodY + rand() % 3 - 1; // -1, 0, 1 offset from foodY
        } while ((bombX == foodX && bombY == foodY) ||  // Ensure bomb doesn't overlap with food
            bombX < 0 || bombX >= width ||        // Ensure bomb is within game boundaries
            bombY < 0 || bombY >= height);

        snakeTailLength = 0;
        computerSnakeTailLength = 0;

        snakeTailX.resize(maxPoints);
        snakeTailY.resize(maxPoints);

        computerSnakeTailX.resize(maxPoints);
        computerSnakeTailY.resize(maxPoints);

        gameBoard = new char* [height + 2];
        for (int i = 0; i < height + 2; ++i) {
            gameBoard[i] = new char[width + 3];
        }
    }
    void Update()
    {
        // top boundary
        for (int i = 0; i < width + 2; i++)
            gameBoard[0][i] = '-';
        gameBoard[0][width + 2] = '\0'; // Null-terminate the string

        // middle part of the board
        for (int i = 1; i < height + 1; i++) {
            for (int j = 0; j < width + 2; j++) {
                if (i == snakeHeadY + 1 && j == snakeHeadX)
                    gameBoard[i][j] = 'O';
                else if (i == computerSnakeHeadY + 1 && j == computerSnakeHeadX)
                    gameBoard[i][j] = 'C';
                else if (i == foodY + 1 && j == foodX)
                    gameBoard[i][j] = '@';
                else if (i == bombY + 1 && j == bombX)
                    gameBoard[i][j] = 'B';
                else
                {
                    bool isSnakeTailCoord = false;
                    for (int k = 0; k < snakeTailLength; k++)
                    {
                        if (snakeTailX[k] == j && snakeTailY[k] + 1 == i)
                        {
                            gameBoard[i][j] = 'o';
                            isSnakeTailCoord = true;
                        }
                    }

                    bool isComputerSnakeTailCoord = false;
                    for (int k = 0; k < computerSnakeTailLength; k++)
                    {
                        if (computerSnakeTailX[k] == j && computerSnakeTailY[k] + 1 == i)
                        {
                            gameBoard[i][j] = 'c';
                            isComputerSnakeTailCoord = true;
                        }
                    }

                    if (!isSnakeTailCoord && !isComputerSnakeTailCoord)
                        gameBoard[i][j] = ' ';
                }
                if (j == 0 || j == width + 1)
                    gameBoard[i][j] = '|';
            }
            gameBoard[i][width + 2] = '\0'; // Null-terminate the string
        }

        // bottom boundary
        for (int i = 0; i < width + 2; i++)
            gameBoard[height + 1][i] = '-';
        gameBoard[height + 1][width + 2] = '\0'; // Null-terminate the string
    }
    void Draw()
    {
        std::cout << "\033[" << 0 + 1 << ";" << 0 + 1 << "H"; // ANSI escape sequence to set cursor position

        for (int i = 0; i < height + 2; ++i) {
            cout << gameBoard[i] << "                                                    " << endl;
        }

        cout << "Player score: " << score << endl;
        cout << "Computer score: " << computerScore << endl;
        cout << endl;

        if (!isGameOver)
        {
            cout << "Max points: " << maxPoints << "." << endl;
            cout << endl;
            cout << "Controls: W, A, S, D. Esc - quit." << endl;
            cout << endl;
        }
        else
        {
            if (playerLost) cout << "PLAYER HAS LOST.                                                       " << endl;
            if (computerLost) cout << "COMPUTER HAS LOST.                                                                     " << endl;
            if (score == maxPoints) cout << "PLAYER HAS WON WITH " << score << " POINTS.                                                     " << endl;
            if (computerScore == maxPoints) cout << "COMPUTER HAS WON WITH " << computerScore << " POINTS                                                  " << endl;
        }
    }
    void Input()
    {
        if (_kbhit())
        {
            switch (_getch())
            {
            case 'w':
            case 'W':
            case 72: // Up arrow
                if (dir != DOWN || snakeTailLength == 0) dir = UP;
                break;
            case 'd':
            case 'D':
            case 77: // Right arrow
                if (dir != LEFT || snakeTailLength == 0) dir = RIGHT;
                break;
            case 'a':
            case 'A':
            case 75: // Left arrow
                if (dir != RIGHT || snakeTailLength == 0) dir = LEFT;
                break;
            case 's':
            case 'S':
            case 80: // Down arrow
                if (dir != UP || snakeTailLength == 0) dir = DOWN;
                break;
            case 27: // ESC button
                isGameOver = true;
                break;
             }
        }
    }
    void Logic()
    {
        int prevX = snakeTailX[0];
        int prevY = snakeTailY[0];
        int prev2X, prev2Y;

        snakeTailX[0] = snakeHeadX;
        snakeTailY[0] = snakeHeadY;

        for (int i = 1; i < snakeTailLength; i++)
        {
            prev2X = snakeTailX[i];
            prev2Y = snakeTailY[i];
            snakeTailX[i] = prevX;
            snakeTailY[i] = prevY;
            prevX = prev2X;
            prevY = prev2Y;
        }

        switch (dir)
        {
        case LEFT:
            snakeHeadX--;
            break;
        case RIGHT:
            snakeHeadX++;
            break;
        case UP:
            snakeHeadY--;
            break;
        case DOWN:
            snakeHeadY++;
            break;
        }

        if (snakeHeadX < 0 || snakeHeadX == width + 1 || snakeHeadY < 0 || snakeHeadY == height)
        {
            isGameOver = true;
            playerLost = true;
        }

        for (int i = 0; i < snakeTailLength; i++) // collision with itself
        {
            if (snakeTailX[i] == snakeHeadX && snakeTailY[i] == snakeHeadY)
            {
                isGameOver = true;
                playerLost = true;
                break;
            }
        }
        for (int i = 0; i < computerSnakeTailLength; i++) // collision with another snake
        {
            if (computerSnakeTailX[i] == snakeHeadX && computerSnakeTailY[i] == snakeHeadY)
            {
                isGameOver = true;
                playerLost = true;
                break;
            }
        }

        if (snakeHeadX == bombX && snakeHeadY == bombY)
        {
            playerBombPaused = true;
        }

        if (snakeHeadX == foodX && snakeHeadY == foodY)
        {
            snakeTailLength++;
            score++;
            if (score == maxPoints)
                isGameOver = true;

            do {
                foodX = (rand() % (width - 1)) + 1;
                foodY = (rand() % (height - 1)) + 1;
            } while ((foodX == computerSnakeHeadX || foodY == computerSnakeHeadY) ||
                (std::find(computerSnakeTailX.begin(), computerSnakeTailX.end(), foodX) != computerSnakeTailX.end() &&
                    std::find(computerSnakeTailY.begin(), computerSnakeTailY.end(), foodY) != computerSnakeTailY.end()));
            // Spawn bomb next to the food
            do {
                int direction = rand() % 4; // Randomly choose one of the four directions (0: right, 1: left, 2: above, 3: under)
                switch (direction) {
                case 0: // right
                    bombX = foodX + 1;
                    bombY = foodY;
                    break;
                case 1: // left
                    bombX = foodX - 1;
                    bombY = foodY;
                    break;
                case 2: // above
                    bombX = foodX;
                    bombY = foodY - 1;
                    break;
                case 3: // under
                    bombX = foodX;
                    bombY = foodY + 1;
                    break;
                }
            } while ((bombX == foodX && bombY == foodY) ||  // Ensure bomb doesn't overlap with food
                bombX < 0 || bombX >= width ||        // Ensure bomb is within game boundaries
                bombY < 0 || bombY >= height);
        }
    }
    void SnakeAI()
    {
        int prevX = computerSnakeTailX[0];
        int prevY = computerSnakeTailY[0];
        int prev2X, prev2Y;

        computerSnakeTailX[0] = computerSnakeHeadX;
        computerSnakeTailY[0] = computerSnakeHeadY;

        for (int i = 1; i < computerSnakeTailLength; i++)
        {
            prev2X = computerSnakeTailX[i];
            prev2Y = computerSnakeTailY[i];
            computerSnakeTailX[i] = prevX;
            computerSnakeTailY[i] = prevY;
            prevX = prev2X;
            prevY = prev2Y;
        }

        // Find direction towards the food
        int dx = foodX - computerSnakeHeadX;
        int dy = foodY - computerSnakeHeadY;

        // Choose the next move based on the direction towards the food
        if (dx > 0 && computerSnakeHeadX + 1 != computerSnakeTailX[1]) // Move right if food is to the right and no tail segment is in the way
            computerSnakeHeadX++;
        else if (dx < 0 && computerSnakeHeadX - 1 != computerSnakeTailX[1]) // Move left if food is to the left and no tail segment is in the way
            computerSnakeHeadX--;
        else if (dy > 0 && computerSnakeHeadY + 1 != computerSnakeTailY[1]) // Move down if food is below and no tail segment is in the way
            computerSnakeHeadY++;
        else if (dy < 0 && computerSnakeHeadY - 1 != computerSnakeTailY[1]) // Move up if food is above and no tail segment is in the way
            computerSnakeHeadY--;

        for (int i = 0; i < computerSnakeTailLength; i++) // collision with itself
        {
            if (computerSnakeTailX[i] == computerSnakeHeadX && computerSnakeTailY[i] == computerSnakeHeadY)
            {
                isGameOver = true;
                computerLost = true;
                break;
            }
        }
        for (int i = 0; i < snakeTailLength; i++) // collision with another snake
        {
            if (snakeTailX[i] == computerSnakeHeadX && snakeTailY[i] == computerSnakeHeadY)
            {
                isGameOver = true;
                computerLost = true;
                break;
            }
        }

        if (computerSnakeHeadX == bombX && computerSnakeHeadY == bombY)
        {
            computerBombPaused = true;
        }

        if (computerSnakeHeadX == foodX && computerSnakeHeadY == foodY)
        {
            computerSnakeTailLength++;
            computerScore++;
            if (computerScore == maxPoints)
                isGameOver = true;

            do {
                foodX = (rand() % (width - 1)) + 1;
                foodY = (rand() % (height - 1)) + 1;
            } while ((foodX == computerSnakeHeadX || foodY == computerSnakeHeadY) ||
                (std::find(computerSnakeTailX.begin(), computerSnakeTailX.end(), foodX) != computerSnakeTailX.end() &&
                    std::find(computerSnakeTailY.begin(), computerSnakeTailY.end(), foodY) != computerSnakeTailY.end()));
            // Spawn bomb next to the food
            do {
                int direction = rand() % 4; // Randomly choose one of the four directions (0: right, 1: left, 2: above, 3: under)
                switch (direction) {
                case 0: // right
                    bombX = foodX + 1;
                    bombY = foodY;
                    break;
                case 1: // left
                    bombX = foodX - 1;
                    bombY = foodY;
                    break;
                case 2: // above
                    bombX = foodX;
                    bombY = foodY - 1;
                    break;
                case 3: // under
                    bombX = foodX;
                    bombY = foodY + 1;
                    break;
                }
            } while ((bombX == foodX && bombY == foodY) ||  // Ensure bomb doesn't overlap with food
                bombX < 0 || bombX >= width ||        // Ensure bomb is within game boundaries
                bombY < 0 || bombY >= height);
        }
    }

    void unpausePlayer()
    {
        playerBombPaused = false;
    }
    void unpauseComputer()
    {
        computerBombPaused = false;
    }
};

class counter
{
    protected:
        int count;
    public:
    counter() : count(0) {}
    counter(int a) : count(a) {}
    ~counter() {}
    void input_count()
    {
        count = 0;
    }
    int get_count()
    {
        return count;
    }
};
class playerCounter :public counter
{
    public:
    void operator +=(int x)
    {
        count += x+1;
    }
};
class computerCounter :public counter
{
public:
    void operator +=(int x)
    {
        count += x;
    }
};

class Intro
{
private:
    bool validResponse, resume;
    int gameSpeed;
public:
    Intro() : validResponse(false), resume(true), gameSpeed(0) {}
    ~Intro() {}
    bool Resume() { return resume;  }
    void Draw()
    {
        cout << "+-------------------------------------------------------------+" << std::endl;
        cout << "|   _____             _           _____                       |" << std::endl;
        cout << "|  / ____|           | |         / ____|                      |" << std::endl;
        cout << "| | (___  _ __   __ _| | _____  | |  __  __ _ _ __ ___   ___  |" << std::endl;
        cout << "|  \\___ \\| '_ \\ / _` | |/ / _ \\ | | |_ |/ _` | '_ ` _ \\ / _ \\ |" << std::endl;
        cout << "|  ____) | | | | (_| |   <  __/ | |__| | (_| | | | | | |  __/ |" << std::endl;
        cout << "| |_____/|_| |_|\\__,_|_|\\_\\___|  \\_____|\\__,_|_| |_| |_|\\___| |" << std::endl;
        cout << "|                                                             |" << std::endl;
        cout << "+-------------------------------------------------------------+" << std::endl;
        cout << "|   Reach max points. Do not collide with your tail,          |" << std::endl;
        cout << "|   another snake or walls. Avoid Bombs.                      |" << std::endl;
        cout << "|                                                             |" << std::endl;
        cout << "|   Control the snake using the following keys:               |" << std::endl;
        cout << "|                                                             |" << std::endl;
        cout << "|   W / Up Arrow    : Move Up                                 |" << std::endl;
        cout << "|   S / Down Arrow  : Move Down                               |" << std::endl;
        cout << "|   A / Left Arrow  : Move Left                               |" << std::endl;
        cout << "|   D / Right Arrow : Move Right                              |" << std::endl;
        cout << "|   ESC             : Quit Game                               |" << std::endl;
        cout << "|                                                             |" << std::endl;
        cout << "|   CHOOSE GAME DIFFICULTY : 1 - EASY, 2 - MEDIUM, 3 - HARD   |" << std::endl;
        cout << "|                                                             |" << std::endl;
        cout << "+-------------------------------------------------------------+" << std::endl;
    }
    void Input()
    {
        bool validResponse = false;
        while (!validResponse) {
            if (_kbhit()) {
                switch (_getch()) {
                case '1':
                    gameSpeed = 180; // EASY
                    validResponse = true;
                    break;
                case '2':
                    gameSpeed = 130; // MEDIUM
                    validResponse = true;
                    break;
                case '3':
                    gameSpeed = 100; // HARD
                    validResponse = true;
                    break;
                case 27:
                    resume = false;
                    validResponse = true;
                    break;
                }
            }
            //Sleep(1);
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
        }
    }
    int get_gameSpeed()
    {
        return gameSpeed;
    }
};


int main()
{
    
    Intro intro;
    
    while (true)
    {
        // Intro screen
        intro.Draw();
        intro.Input();
        if (!intro.Resume())
            return 0;
        //system("cls");
        std::cout << "\033[2J\033[1;1H";

        SnakeGame snake;
        playerCounter pc;
        computerCounter cc;
        //snake.Setup();
        while (!snake.getIsGameOver())
        {
            snake.Input();

            if (!snake.playerPaused())
                snake.Logic();
            else
            {
                pc += 1;
                if (pc.get_count() >= 20)
                {
                    snake.unpausePlayer();
                    pc.input_count();
                } 
            }

            if (!snake.computerPaused())
                snake.SnakeAI();
            else
            {
                cc += 1;
                if (cc.get_count() == 20)
                {
                    snake.unpauseComputer();
                    cc.input_count();
                }
            }

            snake.Update();
            snake.Draw();
            //Sleep(intro.get_gameSpeed()); // ADJUST GAME SPEED
            std::this_thread::sleep_for(std::chrono::milliseconds(intro.get_gameSpeed()));
        }
        //Sleep(5000);
        std::this_thread::sleep_for(std::chrono::milliseconds(5000)); // 5000 milliseconds (5 seconds)

        //system("cls");
        std::cout << "\033[2J\033[1;1H";
    }
    return 1;
}
