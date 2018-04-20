#ifndef GAME_HPP
#define GAME_HPP
#include <QGraphicsRectItem>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QList>
#include <QObject>
#include <QTimer>
#include <QVector>
enum class RectangleColor
{
    Red,
    Green,
    Black
};
class Rectangle : public QObject, public QGraphicsRectItem
{
    Q_OBJECT
public:
    RectangleColor color;
    Rectangle();
    void setGreen();
    void setRed();
    void setBlack();
};
class GridView
{
public:
    int rows;
    int cols;
    QVector<Rectangle*> data;
    GridView(int rows, int cols, QGraphicsScene* scene);
    Rectangle*& at(int row, int col);
};
enum class SnakeDirection
{
    Up,
    Down,
    Left,
    Right
};
class Position
{
public:
    int x;
    int y;
    Position(int x, int y);
    bool isVaild(int rows, int cols);
};
class Snake
{
public:
    SnakeDirection direction;
    QList<Position> positions;
    Snake(SnakeDirection direction, int length);
    bool move(SnakeDirection direction, GridView& view);

private:
    Position nextPosition(SnakeDirection direction, Position head);
};

class Game : public QGraphicsView
{
    Q_OBJECT
public:
    QGraphicsScene* scene;
    GridView view;
    Snake snake;
    QTimer timer;
    Game();
    bool drawSnake();
    void keyPressEvent(QKeyEvent* event) override;
    void move(SnakeDirection direction);
    void spawnApple();
    Position findApple();
public slots:
    void moveStraight();
};

#endif // GAME_HPP
