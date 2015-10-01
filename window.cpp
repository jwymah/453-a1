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
    createActions();

    // Create the menus
    // Setup the file menu
    mFileMenu = menuBar()->addMenu(tr("&File"));
    mFileMenu->addAction(mQuitAction);  // add quitting

    // Setup the application's widget collection
    QVBoxLayout * layout = new QVBoxLayout();

    // Add renderer
    layout->addWidget(renderer);
    renderer->setMinimumSize(300, 600);

    QWidget * mainWidget = new QWidget();
    mainWidget->setLayout(layout);
    setCentralWidget(mainWidget);

    // Timer stuff - tutorial 5
    this->m_pGameTimer = new QTimer(this);
    connect(this->m_pGameTimer, SIGNAL(timeout()), this, SLOT(timer_tick()));
    this->m_pGameTimer->start(300); // 500 ms

    gameHeight = 24;
    gameWidth = 10;

    game = new Game(gameWidth, gameHeight);

}

// helper function for creating actions
void Window::createActions()
{
    // Quits the application
    mQuitAction = new QAction(tr("&Quit"), this);
    mQuitAction->setShortcut(QKeySequence(Qt::Key_Q));
    mQuitAction->setStatusTip(tr("Quits the application"));
    connect(mQuitAction, SIGNAL(triggered()), qApp, SLOT(quit()));

}

void Window::timer_tick()
{
    cout << "Tick" << endl;
    // Here's where you'd increment the game state
    // then draw a new frame

//    renderer->rotate10();
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
    renderer->update();
}

// destructor
Window::~Window()
{
    delete renderer;
}
