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

    // destructor
    ~Window();


private slots:
    void timer_tick();

private:
    // Main widget for drawing
    Renderer *renderer;

    // Menu items and actions
    QMenu * mFileMenu;
    QAction * mQuitAction;

    // Timer
    QTimer *m_pGameTimer;

    // helper function for creating actions
    void createActions();

    Game *game;

};

#endif // WINDOW_H
