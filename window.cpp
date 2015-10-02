#include "window.h"
#include "renderer.h"
#include <iostream>

using namespace std;

Window::Window(QWidget *parent) :
    QMainWindow(parent)
{
    setWindowTitle("CPSC453: Tetromino Apocalypse");

    // Create the main drawing object
    renderer = new Renderer();

    // Create the actions to be used by the menus
    createFileActions();
    createDrawActions();
    createGameActions();

    // Create the menus
    // Setup the file menu
    mFileMenu = menuBar()->addMenu(tr("&File"));
    mFileMenu->addAction(mNewGameAction);
    mFileMenu->addAction(mResetAction);  // add resetting the scales/rotation
    mFileMenu->addAction(mQuitAction);  // add quitting

    // Setup the Draw menu
    mDrawMenu = menuBar()->addMenu(tr("&Draw"));
    mDrawMenu->addAction(mWireframeAction);
    mDrawMenu->addAction(mFillAction);
    mDrawMenu->addAction(mMultiColourAction);

    // Setup the Game menu
    mGameMenu = menuBar()->addMenu(tr("&Game"));
    mGameMenu->addAction(mPauseAction);
    mGameMenu->addAction(mSpeedUpAction);
    mGameMenu->addAction(mSpeedDownAction);
    mGameMenu->addAction(mSpeedAutoAction);

    // Setup the application's widget collection
    QVBoxLayout * layout = new QVBoxLayout();

    // Add renderer
    layout->addWidget(renderer);
    renderer->setMinimumSize(300, 600);

    QWidget * mainWidget = new QWidget();
    mainWidget->setLayout(layout);
    setCentralWidget(mainWidget);

    // game timer
    this->m_pGameTimer = new QTimer(this);
    connect(this->m_pGameTimer, SIGNAL(timeout()), this, SLOT(timer_tick()));
    gameSpeed = 300;
    this->m_pGameTimer->start(gameSpeed);// ms

    // drawing timer
    this->m_pDrawTimer = new QTimer(this);
    connect(this->m_pDrawTimer, SIGNAL(timeout()), this, SLOT(draw_tick()));
    this->m_pDrawTimer->start(33); // 30fps

    gameHeight = 24;
    gameWidth = 10;

    game = new Game(gameWidth, gameHeight);

}

// helper function for creating actions
void Window::createFileActions()
{
    // New Game
    mNewGameAction = new QAction(tr("&New Game"), this);
    mNewGameAction->setShortcut(QKeySequence(Qt::Key_N));
    connect(mNewGameAction, SIGNAL(triggered()), this, SLOT(newGame()));

    // Reset the view
    mResetAction = new QAction(tr("&Reset"), this);
    mResetAction->setShortcut(QKeySequence(Qt::Key_R));
    connect(mResetAction, SIGNAL(triggered()), this, SLOT(resetView()));

    // Quits the application
    mQuitAction = new QAction(tr("&Quit"), this);
    mQuitAction->setShortcut(QKeySequence(Qt::Key_Q));
    mQuitAction->setStatusTip(tr("Quits the application"));
    connect(mQuitAction, SIGNAL(triggered()), qApp, SLOT(quit()));
}

void Window::createDrawActions()
{
    // WireFrame
    mWireframeAction = new QAction(tr("&Wire Frame"), this);
    mWireframeAction->setShortcut(QKeySequence(Qt::Key_W));
    connect(mWireframeAction, SIGNAL(triggered()), this, SLOT(wireframe()));
    // WireFrame
    mFillAction = new QAction(tr("&Face"), this);
    mFillAction->setShortcut(QKeySequence(Qt::Key_F));
    connect(mFillAction, SIGNAL(triggered()), this, SLOT(face()));
    // WireFrame
    mMultiColourAction = new QAction(tr("&Multicoloured"), this);
    mMultiColourAction->setShortcut(QKeySequence(Qt::Key_M));
    connect(mMultiColourAction, SIGNAL(triggered()), this, SLOT(multicoloured()));
}

// helper function for creating actions
void Window::createGameActions()
{
    // Pause
    mPauseAction = new QAction(tr("&Pause"), this);
    mPauseAction->setShortcut(QKeySequence(Qt::Key_P));
    connect(mPauseAction, SIGNAL(triggered()), this, SLOT(pause()));

    // Speed up
    mSpeedUpAction = new QAction(tr("&Speed Up"), this);
    mSpeedUpAction->setShortcut(QKeySequence(Qt::Key_PageUp));
    connect(mSpeedUpAction, SIGNAL(triggered()), this, SLOT(speedUp()));

    // Slow down
    mSpeedDownAction = new QAction(tr("&Slow Down"), this);
    mSpeedDownAction->setShortcut(QKeySequence(Qt::Key_PageDown));
    connect(mSpeedDownAction, SIGNAL(triggered()), this, SLOT(speedDown()));

    // Auto increase
    mSpeedAutoAction = new QAction(tr("&Auto-Increase"), this);
    mSpeedAutoAction->setShortcut(QKeySequence(Qt::Key_A));
    connect(mSpeedAutoAction, SIGNAL(triggered()), this, SLOT(speedAuto()));
}

void Window::wireframe()
{
    renderer->setDisplayWireFrame();
}
void Window::face()
{
    renderer->setDisplayFace();
}
void Window::multicoloured()
{
    renderer->setDisplayMultiColored();
}

void Window::pause()
{
    gameSpeed = 30000;
    m_pGameTimer->start(gameSpeed);
}

void Window::speedUp()
{
    gameSpeed /= 1.5;
    m_pGameTimer->start(gameSpeed);
}

void Window::speedDown()
{
    gameSpeed *= 1.5;
    m_pGameTimer->start(gameSpeed);
}

void Window::speedAuto()
{
    gameSpeed = 0;
    m_pGameTimer->start(gameSpeed);
}

void Window::newGame()
{
    game->reset();
}

void Window::resetView()
{
    renderer->resetView();
}

void Window::draw_tick()
{
    renderer->update();
}

void Window::timer_tick()
{
    cout << "Tick" << endl;

    game->tick();

    int board[gameHeight][10];

    for (int r=0; r<gameHeight; r++)
    {
        for (int c=0; c<gameWidth; c++)
        {
            board[r][c] = game->get(r,c);
        }
    }
    renderer->setupGameBoard(board);
}

void Window::keyPressEvent(QKeyEvent *event)
{
    switch(event->key()){
    case Qt::Key_Left :
        game->moveLeft();
        break;
    case Qt::Key_Right :
        game->moveRight();
        break;
    case Qt::Key_Up :
        game->rotateCCW();
        break;
    case Qt::Key_Down :
        game->rotateCW();
        break;
    case Qt::Key_Space :
        game->drop();
        break;
    case Qt::Key_Shift :
        renderer->setShiftStatus(true);
        break;
    }
}

void Window::keyReleaseEvent(QKeyEvent *event)
{
    cout << event->key() << " Released" << endl;
    switch(event->key()){
    case Qt::Key_Shift :
        renderer->setShiftStatus(false);
        break;
    }
}

// destructor
Window::~Window()
{
    delete renderer;
}
