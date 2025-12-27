# Pong Game

A two-player Pong game built with C++ and SFML, featuring custom physics, collision detection, and game state management.

## About This Project

I created Pong in C++ to learn about game development and practice C++ programming. This project gave me a better understanding of game physics, collision detection, manage game states, and user input handling.

## Features

- **Two-player local gameplay**: Player 1 uses W/S keys, Player 2 uses arrow keys
- **Custom collision detection**: Ball bounces realistically off paddles and walls using overlap calculations
- **Score tracking system**: First player to reach 10 points wins
- **Game state management**: Handles pauses, respawns, and end-game conditions
- **Smooth physics**: Delta-time based movement for consistent gameplay across different frame rates
- **Randomized ball direction**: Ball launches at random angles initially to keep gameplay unpredictable

## What I Learned

Through this project, I gained more experience with:
- **Object-oriented programming**: Designing custom Paddle and PongBall classes that inherit from SFML shapes
- **Real-time collision detection**: Implementing edge detection and overlap calculations to handle ball-paddle collisions from multiple angles
- **Game state management**: Creating a state machine to handle CENTER_PAUSE, WAITING, PLAYING, and POINT_SCORED states
- **Physics calculations**: Using trigonometry to calculate ball velocity vectors and handle realistic bouncing
- **Delta-time rendering**: Ensuring smooth, frame-rate-independent movement
- **C++ best practices**: Using const correctness, optional types, and modern C++ features

# Challenges

The most difficult part of this project was implementing accurate collision detection for the paddles, as the ball goes through and inside the paddle between frames instead of just making contact with the rectangle's border, which is how I intiuitively thought of it. After several hours of debugging and testing, and a revisit the next morning, I figured out the minimum overlap calculation, making collisions against the paddles feel realistic.

## Technical Highlights

The collision detection system calculates the overlap between the ball and paddles on all four sides, then determines which wall is closest to resolve the collision accurately. The game also uses a state management system to manage pauses between rounds and includes a 0.5-second delay after each point is scored.

## How to Run

### Prerequisites
- C++ compiler (GCC, Clang, or MSVC)
- SFML 2.5 or later
- Arial font file in `assets/arial.ttf`

### Building
```bash
git clone https://github.com/David41413/pong_game.git
cd pong_game
g++ -c main.cpp
g++ main.o -o pong -lsfml-graphics -lsfml-window -lsfml-system
./pong
```

## Controls

**Player 1:**
- W - Move paddle up
- S - Move paddle down

**Player 2:**
- Up Arrow - Move paddle up
- Down Arrow - Move paddle down

**General:**
- Spacebar - Restarts game after someone wins

## Technologies Used

- C++17
- SFML (Simple and Fast Multimedia Library)
- Custom Random number generation for ball direction

## Game Rules

- First player to 10 points wins
- Ball resets to center after each point with a brief pause
- Ball launches at a random angle at the start of each round
- Paddles cannot move beyond the window boundaries
