/*
 * CPSC 453 - Introduction to Computer Graphics
 * Assignment 1
 *
 * Window - Fundamental GUI for interacting within the scene
 */

#ifndef WINDOW_H
#define WINDOW_H

#include <QMainWindow>
#include <QApplication>
#include <QMenuBar>
#include <QAction>
#include <QMessageBox>
#include <QPushButton>
#include <QVBoxLayout>
#include <QActionGroup>
#include <QTimer>
#include "game.h"

class Renderer;

class Window : public QMainWindow
{
    // informs the qmake that a Qt moc_* file will need to be generated
    Q_OBJECT

public:
    // constructor
    Window(QWidget *parent = 0);

    virtual void keyPressEvent(QKeyEvent *event);
    virtual void keyReleaseEvent(QKeyEvent *event);

    // destructor
    ~Window();

private slots:
    void draw_tick();
    void timer_tick();
    void newGame();
    void resetView();

    void wireframe();
    void face();
    void multicoloured();

    void pause();
    void speedUp();
    void speedDown();
    void speedAuto();

private:
    // Main widget for drawing
    Renderer *renderer;

    // Menu items and actions
    QMenu * mFileMenu;
    QAction * mNewGameAction;
    QAction * mResetAction;
    QAction * mQuitAction;

    QMenu * mDrawMenu;
    QAction * mWireframeAction;
    QAction * mFillAction;
    QAction * mMultiColourAction;

    QMenu * mGameMenu;
    QAction * mPauseAction;
    QAction * mSpeedUpAction;
    QAction * mSpeedDownAction;
    QAction * mSpeedAutoAction;

    // Timer
    QTimer *m_pGameTimer;
    QTimer *m_pDrawTimer;

    // helper function for creating actions
    void createFileActions();
    void createDrawActions();
    void createGameActions();

    int gameHeight;
    int gameWidth;

    float gameSpeed;

    Game *game;

};

#endif // WINDOW_H
