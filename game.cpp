#include "game.h"
#include "ball.h"
#include "paddle.h"
#include "brick.h"
#include <QThread>
#include <QObject>
#include <QDebug>
#include <QTimer>
#include <QApplication>
#include "backgroundmusic.h"

extern Paddle* paddle;

Game::Game(QWidget *parent):QGraphicsView (parent)
{
    scene=new QGraphicsScene(0,0,700,700);
    setScene(scene);
    setFixedSize(700, 700);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    music = new BackgroundMusic();
};

void Game::start()
{   
    music->start();
    paddle = new Paddle();
    scene->addItem(paddle);
    QApplication::setKeyboardInputInterval(5);

    Ball *ball=new Ball();
    scene->addItem(ball);

//    Brick *brick=new Brick();
//    scene->addItem(brick);

    //create a grid of blocks of size m*n
       //brick->setPos(100,100);
    double sx=100;
    double sy=100;
    for(int i=0;i<10;i++)
    {
        for(int j=0;j<6;j++)
        {
            Brick *brick=new Brick();
            brick->setPos(sx,sy);
            scene->addItem(brick);
            sx+=80;
        }
        sy+=20;
        sx=100;
    }

    //Add the grid QObject to the scene

    QThread *thread=new QThread;
    QTimer *timer=new QTimer(nullptr);
    timer->setInterval(5);
    timer->moveToThread(thread);
    connect(timer,SIGNAL(timeout()),ball,SLOT(move()));
    connect(thread,SIGNAL(started()),timer,SLOT(start()));
    thread->start();
    connect(ball,SIGNAL(endgame()),timer,SLOT(stop()));
}
Game::~Game()
{
    music->exit();
    delete music;
}
