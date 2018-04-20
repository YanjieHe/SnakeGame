#include "Game.hpp"
#include <QBrush>
#include <QDebug>
#include <QPen>
#include <QSet>
#include <random>
Game::Game()
    : QGraphicsView()
    , scene{new QGraphicsScene()}
    , view(60, 60, scene)
    , snake(SnakeDirection::Up, 20)
{
    setFixedSize(60 * 15, 60 * 15);
    scene->setSceneRect(0, 0, 60 * 15, 60 * 15);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setScene(scene);
    drawSnake();
    spawnApple();
    connect(&timer, SIGNAL(timeout()), this, SLOT(moveStraight()));
    timer.start(50);
}

bool Game::drawSnake()
{
    QSet<int> set;
    for (Position p : snake.positions)
    {
        set.insert(p.x * view.cols + p.y);
    }
    bool ateApple = false;
    for (int i = 0; i < view.rows; i++)
    {
        for (int j = 0; j < view.cols; j++)
        {
            int index = i * view.cols + j;
            if (set.contains(index))
            {
                if (view.at(i, j)->color != RectangleColor::Green)
                {
                    if (view.at(i, j)->color == RectangleColor::Red)
                    {
                        ateApple = true;
                    }
                    view.at(i, j)->setGreen();
                }
            }
            else
            {
                if (view.at(i, j)->color == RectangleColor::Green)
                {
                    view.at(i, j)->setBlack();
                }
            }
        }
    }
    return ateApple;
}

void Game::keyPressEvent(QKeyEvent* event)
{
    switch (event->key())
    {
    case Qt::Key_Up:
        if (snake.direction != SnakeDirection::Down)
        {
            move(SnakeDirection::Up);
        }
        break;
    case Qt::Key_Down:
        if (snake.direction != SnakeDirection::Up)
        {
            move(SnakeDirection::Down);
        }
        break;
    case Qt::Key_Left:
        if (snake.direction != SnakeDirection::Right)
        {
            move(SnakeDirection::Left);
        }
        break;
    case Qt::Key_Right:
        if (snake.direction != SnakeDirection::Left)
        {
            move(SnakeDirection::Right);
        }
        break;
    }
}

void Game::move(SnakeDirection direction)
{
    if (snake.move(direction, view))
    {
        if (drawSnake())
        {
            spawnApple();
        }
    }
}

void Game::spawnApple()
{
    QVector<int> positions;
    static std::default_random_engine generator;
    static std::uniform_int_distribution<int> distribution;
    for (int i = 0; i < view.data.size(); i++)
    {
        if (view.data[i]->color == RectangleColor::Black)
        {
            positions.append(i);
        }
    }
    int choice = distribution(generator) % positions.size();
    view.data[positions[choice]]->setRed();
}

void Game::moveStraight()
{
    move(snake.direction);
}

GridView::GridView(int rows, int cols, QGraphicsScene* scene)
    : rows{rows}
    , cols{cols}
    , data(rows * cols)
{
    const int rectWidth = 15;
    const int rectHeight = 15;
    for (int i = 0; i < rows; i++)
    {
        for (int j = 0; j < cols; j++)
        {
            at(i, j) = new Rectangle();
            scene->addItem(at(i, j));
            at(i, j)->setRect(i * rectWidth, j * rectHeight, rectWidth,
                              rectHeight);
        }
    }
}

Rectangle*& GridView::at(int row, int col)
{
    return data[row * cols + col];
}

Rectangle::Rectangle()
    : QObject()
    , QGraphicsRectItem()
{
    setPen(QPen(QColor::fromRgb(71, 71, 71)));
    setBlack();
}

void Rectangle::setGreen()
{
    setBrush(QBrush(QColor::fromRgb(153, 255, 81)));
    color = RectangleColor::Green;
}

void Rectangle::setRed()
{
    setBrush(QBrush(Qt::GlobalColor::red));
    color = RectangleColor::Red;
}

void Rectangle::setBlack()
{
    setBrush(QBrush(QColor::fromRgb(71, 71, 71)));
    color = RectangleColor::Black;
}

Snake::Snake(SnakeDirection direction, int length)
    : direction{direction}
{
    int x = 40;
    int y = 10;
    positions.append(Position(x, y));
    for (int i = 0; i < length - 1; i++)
    {
        y++;
        positions.append(Position(x, y));
    }
}

bool Snake::move(SnakeDirection direction, GridView& view)
{
    this->direction = direction;
    Position head = positions.front();
    Position tail = positions.last();
    positions.removeLast();
    Position next = nextPosition(direction, head);
    if (next.isVaild(view.rows, view.cols))
    {
        if (view.at(next.x, next.y)->color == RectangleColor::Green)
        {
            positions.push_back(tail);
            return false;
        }
        else
        {
            positions.push_front(next);
            if (view.at(next.x, next.y)->color == RectangleColor::Red)
            {
                positions.push_back(tail);
            }
            return true;
        }
    }
    else
    {
        positions.push_back(tail);
        return false;
    }
}

Position Snake::nextPosition(SnakeDirection direction, Position head)
{
    switch (direction)
    {
    case SnakeDirection::Up:
        return Position(head.x, head.y - 1);
    case SnakeDirection::Down:
        return Position(head.x, head.y + 1);
    case SnakeDirection::Left:
        return Position(head.x - 1, head.y);
    default:
    case SnakeDirection::Right:
        return Position(head.x + 1, head.y);
    }
}

Position::Position(int x, int y)
    : x{x}
    , y{y}
{
}

bool Position::isVaild(int rows, int cols)
{
    return x >= 0 and y >= 0 and x < cols && y < rows;
}
