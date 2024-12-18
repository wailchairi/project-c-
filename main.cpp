#include <iostream>
#include "raylib.h"
#include <vector>
#include <stack>
#include <ctime>
#include <cstdlib>
#include <array>
using namespace std;

// Classe Labyrinthe (représente le labyrinthe)
class Labyrinthe {
private:
    int rows, cols;       // Dimensions du labyrinthe (lignes et colonnes)
    int cellSize;         // Taille de chaque cellule (en pixels)
    int maze[40][40];     // Tableau 2D pour représenter la grille du labyrinthe
    array<int, 4> dx = {1, -1, 0, 0}; // Directions de déplacement (droite, gauche)
    array<int, 4> dy = {0, 0, 1, -1}; // Directions de déplacement (bas, haut)

public:
    // Constructeur pour initialiser les dimensions du labyrinthe
    Labyrinthe(int rows, int cols, int cellSize)
        : rows(rows), cols(cols), cellSize(cellSize) {
        resetMaze(); // Réinitialiser le labyrinthe au départ
    }

    // Fonction pour imprimer les valeurs de dx (utile pour le debug)
    void printDx() const {
        for (const auto& i : dx) {
            cout << i << " ";
        }
        cout << endl;
    }

    // Réinitialise la grille du labyrinthe
    void resetMaze() {
        for (int row = 0; row < rows; row++) {
            for (int col = 0; col < cols; col++) {
                maze[row][col] = 1;  // Définir toutes les cellules comme des murs
            }
        }
    }

    // Génère le labyrinthe de manière aléatoire
    void generateMaze() {
        resetMaze(); // Commencer avec une grille réinitialisée
        stack<pair<int, int>> path; // Pile pour suivre le chemin pendant la génération
        srand(static_cast<unsigned int>(time(0))); // Initialiser une graine aléatoire

        // Commencer la génération depuis (1,1)
        maze[1][1] = 0;  // Marquer cette cellule comme chemin
        path.push({1, 1}); // Ajouter cette position à la pile

        while (!path.empty()) {
            int x = path.top().first; // Récupérer la position actuelle (x)
            int y = path.top().second; // Récupérer la position actuelle (y)

            // Trouver des voisins non visités
            vector<int> neighbors;
            for (int i = 0; i < 4; i++) {
                int nx = x + dx[i] * 2; // Calculer la position du voisin x
                int ny = y + dy[i] * 2; // Calculer la position du voisin y
                if (nx > 0 && nx < rows && ny > 0 && ny < cols && maze[nx][ny] == 1) {
                    neighbors.push_back(i); // Ajouter le voisin s'il n'est pas visité
                }
            }

            if (!neighbors.empty()) {
                // Choisir un voisin aléatoire
                int dir = neighbors[rand() % neighbors.size()];
                int nx = x + dx[dir] * 2;
                int ny = y + dy[dir] * 2;
                maze[x + dx[dir]][y + dy[dir]] = 0; // Supprimer le mur entre les cellules
                maze[nx][ny] = 0;                  // Marquer le voisin comme chemin
                path.push({nx, ny});               // Ajouter cette cellule à la pile
            } else {
                path.pop(); // Reculer si aucun voisin n'est disponible
            }
        }

        // Définir la sortie du labyrinthe (coin inférieur droit)
        maze[rows - 2][cols - 2] = 2;
    }

    // Dessiner le labyrinthe à l'écran
    void drawMaze() const {
        for (int row = 0; row < rows; row++) {
            for (int col = 0; col < cols; col++) {
                int x = col * cellSize;  // Position x du coin supérieur gauche
                int y = row * cellSize;  // Position y du coin supérieur gauche
                // Déterminer la couleur de la cellule
                Color cellColor = (maze[row][col] == 1) ? BLACK : (maze[row][col] == 2) ? GREEN : LIGHTGRAY;
                DrawRectangle(x, y, cellSize, cellSize, cellColor); // Dessiner le rectangle
            }
        }
    }

    // Vérifier si le joueur peut se déplacer dans une position spécifique
    bool canMoveTo(int x, int y) const {
        return x > 0 && x < cols && y > 0 && y < rows && maze[y][x] != 1;
    }

    // Récupérer la valeur de la cellule dans le labyrinthe
    int getCell(int x, int y) const {
        return maze[y][x];
    }
};

// Classe Joueur (représente le joueur)
class Joueur {
private:
    int x, y; // Position actuelle du joueur (en coordonnées de grille)

public:
    // Constructeur pour initialiser la position du joueur
    Joueur(int startX, int startY) : x(startX), y(startY) {}

    // Obtenir la position actuelle du joueur
    int getX() const { return x; }
    int getY() const { return y; }

    // Déplacer le joueur dans une direction donnée (si possible)
    void move(int dx, int dy, const Labyrinthe& maze) {
        int newX = x + dx; // Nouvelle position x
        int newY = y + dy; // Nouvelle position y
        if (maze.canMoveTo(newX, newY)) { // Vérifier si le déplacement est valide
            x = newX;
            y = newY;
        }
    }

    // Réinitialiser la position du joueur
    void reset(int startX, int startY) {
        x = startX;
        y = startY;
    }
};

// Classe Niveau (gère la difficulté)
class Niveau {
private:
    int rows, cols, cellSize;
    int difficulty; // 1: Facile, 2: Moyen, 3: Difficile

public:
    Niveau() : difficulty(1), rows(11), cols(11), cellSize(40) {}

    void setDifficulty(int level) {
        difficulty = level;
        if (difficulty == 1) {
            rows = 11; cols = 11; cellSize = 40;
        } else if (difficulty == 2) {
            rows = 19; cols = 19; cellSize = 30;
        } else if (difficulty == 3) {
            rows = 31; cols = 31; cellSize = 20;
        }
    }

    int getRows() const { return rows; }
    int getCols() const { return cols; }
    int getCellSize() const { return cellSize; }
};

// Classe Jeu (orchestration du jeu)
class Jeu {
private:
    Labyrinthe maze;
    Joueur player;
    Niveau niveau;
    bool gameWon, gameStarted;
    float startTime, elapsedTime;

public:
    Jeu() : maze(11, 11, 40), player(1, 1), niveau(), gameWon(false), gameStarted(false), startTime(0), elapsedTime(0) {}

    void run() {
        InitWindow(700, 650, "Maze Game");
        SetTargetFPS(60);

        while (!WindowShouldClose()) {
            if (!gameStarted) showDifficultySelection();
            else playGame();
        }

        CloseWindow();
    }

private:
    void showDifficultySelection() {
        BeginDrawing();
        ClearBackground(RAYWHITE);

        DrawText("Selectionnez la difficulté", 250, 200, 30, DARKGRAY);
        DrawText("1: Facile", 280, 250, 20, DARKGRAY);
        DrawText("2: Moyen", 280, 290, 20, DARKGRAY);
        DrawText("3: Difficile", 280, 330, 20, DARKGRAY);

        if (IsKeyPressed(KEY_ONE)) { niveau.setDifficulty(1); initializeGame(); }
        if (IsKeyPressed(KEY_TWO)) { niveau.setDifficulty(2); initializeGame(); }
        if (IsKeyPressed(KEY_THREE)) { niveau.setDifficulty(3); initializeGame(); }

        EndDrawing();
    }

    void initializeGame() {
        maze = Labyrinthe(niveau.getRows(), niveau.getCols(), niveau.getCellSize());
        maze.generateMaze();
        player.reset(1, 1);
        gameStarted = true;
        gameWon = false;
        startTime = GetTime();
    }

    void playGame() {
        if (IsKeyPressed(KEY_RIGHT)) player.move(1, 0, maze);
        if (IsKeyPressed(KEY_LEFT)) player.move(-1, 0, maze);
        if (IsKeyPressed(KEY_UP)) player.move(0, -1, maze);
        if (IsKeyPressed(KEY_DOWN)) player.move(0, 1, maze);

        if (maze.getCell(player.getX(), player.getY()) == 2) gameWon = true;

        if (!gameWon) elapsedTime = GetTime() - startTime;

        BeginDrawing();
        ClearBackground(DARKBLUE);

        maze.drawMaze();
        DrawRectangle(player.getX() * niveau.getCellSize(), player.getY() * niveau.getCellSize(), niveau.getCellSize(), niveau.getCellSize(), RED);

        DrawText(TextFormat("Temps: %.2f secondes", elapsedTime), 10, 10, 20, WHITE);

        if (gameWon) {
            DrawText("Vous avez gagné!", 250, 300, 40, GREEN);
            DrawText("Appuyez sur R pour recommencer", 250, 350, 20, WHITE);
            if (IsKeyPressed(KEY_R)) gameStarted = false;
        }

        EndDrawing();
    }
};

// Fonction principale
int main() {
    Jeu jeu;
    jeu.run();
    return 0;
}
