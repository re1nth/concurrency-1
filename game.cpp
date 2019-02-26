#include <QThread>
#include <QObject>
#include <QDebug>
#include <QTimer>
#include <QPixmap>
#include <QBrush>
#include <QColor>
#include <QApplication>
#include <QGraphicsRectItem>
#include "backgroundmusic.h"
#include "game.h"
#include "ball.h"
#include "paddle.h"
#include "start_menu.h"
#include "pause_menu.h"
#include "end_menu.h"
#include "brick.h"
#include "ballworker.h"
#include "powerup.h"
#include <algorithm>
extern Paddle* paddle;
extern start_menu *smenu;
extern Game *game;


Game::Game(QWidget *parent):QGraphicsView (parent)
{
    setup_scene();
    QApplication::setKeyboardInputInterval(5);
    music = new BackgroundMusic();

};

void Game::setup_scene()
{
    scene=new QGraphicsScene(0,0,700,700,this);
    setScene(scene);
    setFixedSize(700, 700);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

}
void Game::build()
{
    qDebug()<<QThread::currentThreadId();
    //deleting the previous menu derived from the QGraphicsScene
    //menu depending . can be start or end menu ??
    music->start();
    paddle = new Paddle();
    scene->addItem(paddle);

    ball = new Ball();
    scene->addItem(ball);


    QThread* thread1 = new QThread();

    ballworker* worker1 = new ballworker(ball,scene);

    worker1->moveToThread(thread1);
    thread1->start();


    ball_list.push_back(ball);
    worker_list.push_back(worker1);
    QObject::connect(paddle,SIGNAL(ballCollision(bool,bool)),worker1,SLOT(PaddleCollisionDetected(bool,bool)));

    connect(worker1,SIGNAL(destroy(Brick*)),this,SLOT(remove_brick(Brick*)));

    //create a grid of blocks of size m*n
    grid = new gridlayout(9,6,scene);

    connect(worker1,SIGNAL(endgame()),this,SLOT(end()));

    timer = new QTimer();
    timer->start(5);
    connect(timer,SIGNAL(timeout()),worker1,SLOT(ball_move()));
    connect(worker1,SIGNAL(ballposupdater(Ball*,double, double)),this,SLOT(ballpositionupdater(Ball*,double, double)));
    connect(paddle,SIGNAL(multiballadd(Powerup*)),this,SLOT(Multiply_ball(Powerup*)));
    connect(paddle,SIGNAL(multiballadd(Powerup*)),this,SLOT(removepowerup(Powerup*)));


    this->show();
}
void Game::ballpositionupdater(Ball* b,double x, double y)
{
    b->setPos(x,y);
    brick_collision();
}
void Game::remove_brick(Brick *brick)
{

    brick->setHits(brick->getHits()-1);

    if(brick->getHits()==1)
    {
        brick->update();
    }

    else {
        scene->removeItem(brick);
        delete brick;
    }
}

void Game::pause()
{
    pause_menu *pmenu=new pause_menu();
    pmenu->show();
}
void Game::restart()
{
    qDebug()<<"Restart";
    scene->clear();
    this->build();
}

void Game::end()
{
    qDebug()<<"Disconnected";
    qApp->exit();
    end_menu *emenu = new end_menu();
    this->hide();
    emenu->show();
    //worker_list[0]->exit();
    worker_list.pop_back();

}

Game::~Game()
{
    music->exit();
    delete music;
}
void Game::brick_collision()
{
    QList<QGraphicsItem*> cItems = ball->collidingItems();

    for (int i = 0, n = cItems.size(); i < n; ++i){
            Brick* brick = dynamic_cast<Brick*>(cItems[i]);
            // collides with brick
            if (brick){
                double ballx = ball->pos().x();
                double bally = ball->pos().y();
                double brickx = brick->pos().x();
                double bricky = brick->pos().y();

                // collides from bottom
                if (bally >= bricky + brick->getHeight() && ball->y_velocity < 0){
                    ball->y_velocity = -1 * ball->y_velocity;
                }

                // collides from top
                if (bricky >= bally + ball->rect().height() && ball->y_velocity > 0 ){
                    ball->y_velocity = -1 * ball->y_velocity;
                }

                // collides from right
                if (ballx >= brickx + brick->getWidth() && ball->x_velocity < 0){
                    ball->x_velocity = -1 * ball->x_velocity;
                }

                // collides from left
                if (brickx >= ballx + ball->rect().width()  && ball->x_velocity > 0){
                    ball->x_velocity = -1 * ball->x_velocity;
                }

          //      emit destroy(brick);

                brick->setHits(brick->getHits()-1);

                if(brick->getHits()==1)
                {
                    brick->update();
                }

                else
                {
                    if(brick->brick_id!=0)
                    {
                           Powerup* power = new Powerup();
                           scene->addItem(power);

                           power->set(brick->brick_id,brick->x(),brick->y());
                           qDebug()<<brick->x()<<" "<<brick->y();
                           power_list.push_back(power);
                           connect(timer,SIGNAL(timeout()),power,SLOT(powerup_move()));
                           connect(power,SIGNAL(remove_connection(Powerup*)),this,SLOT(removepowerup(Powerup*)));
                    }
                    scene->removeItem(brick);
                    delete brick;
                }
            }
        }

}
void Game::Multiply_ball(Powerup* power)
{
    long unsigned number_of_balls = ball_list.size();
    qDebug()<<number_of_balls<<"A'''";
    for(long unsigned i = 0;i< number_of_balls ; i++)
    {
        Ball* new_ball = new Ball();
        scene->addItem(new_ball);

        new_ball->setPos(ball_list[i]->x(),ball_list[i]->y());
        QThread* thread1 = new QThread();

        ballworker* worker1 = new ballworker(new_ball,scene);

        worker1->moveToThread(thread1);
        thread1->start();


        ball_list.push_back(new_ball);
        worker_list.push_back(worker1);
        QObject::connect(paddle,SIGNAL(ballCollision(bool,bool)),worker1,SLOT(PaddleCollisionDetected(bool,bool)));

        connect(worker1,SIGNAL(destroy(Brick*)),this,SLOT(remove_brick(Brick*)));
        connect(worker1,SIGNAL(endgame()),this,SLOT(end()));

        connect(timer,SIGNAL(timeout()),worker1,SLOT(ball_move()));
        connect(worker1,SIGNAL(ballposupdater(Ball*, double, double)),this,SLOT(ballpositionupdater(Ball*,double, double)));
    }
}
void Game::removepowerup(Powerup* power)
{
    disconnect(timer,SIGNAL(timeout()),power,SLOT(powerup_move()));
    power_list.erase(std::remove(power_list.begin(),power_list.end(),power),power_list.end());
    delete power;
}
