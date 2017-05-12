#include <QAction>
#include <QMenuBar>
#include <QTextEdit>
#include <iostream>

#include "mainwindow.h"
#include "opencldevicedialog.h"
#include "abstractanalyticfactory.h"



using namespace std;
Ace::MainWindow* Ace::MainWindow::_instance {nullptr};






Ace::MainWindow& Ace::MainWindow::getInstance()
{
   if ( !_instance )
   {
      _instance = new MainWindow;
   }
   return *_instance;
}






void Ace::MainWindow::open()
{

}






void Ace::MainWindow::exit()
{

}






void Ace::MainWindow::runAnalytic()
{
   QAction* from = qobject_cast<QAction*>(sender());
   cout << "Analytic #" << from->data().toInt() << "\n";
}






void Ace::MainWindow::setOpenCL()
{
   OpenCLDeviceDialog::getInstance().exec();
}






Ace::MainWindow::MainWindow(QWidget *parent):
   QMainWindow(parent)
{
   createActions();
   createMenus();
   setCentralWidget(new QTextEdit(this));
}






void Ace::MainWindow::createActions()
{
   // create open action
   _openAction = new QAction(tr("&Open"),this);
   _openAction->setShortcut(QKeySequence::Open);
   _openAction->setStatusTip(tr("Open an existing data object file."));
   connect(_openAction,SIGNAL(triggered(bool)),this,SLOT(open()));

   // create exit action
   _exitAction = new QAction(tr("&Exit"),this);
   _exitAction->setShortcut(QKeySequence::Close);
   _exitAction->setStatusTip(tr("Exit the application."));
   connect(_exitAction,SIGNAL(triggered(bool)),this,SLOT(exit()));

   // create set opencl device action
   _setOpenCLAction = new QAction(tr("&Set Device"),this);
   _setOpenCLAction->setStatusTip(tr("Set the active OpenCL device to a new one."));
   connect(_setOpenCLAction,SIGNAL(triggered(bool)),this,SLOT(setOpenCL()));

   // create analytic actions
   _analyticActions.reserve(EAbstractAnalyticFactory::getInstance().getCount());
   for (quint16 i = 0; i < EAbstractAnalyticFactory::getInstance().getCount() ;++i)
   {
      _analyticActions.append(new QAction(EAbstractAnalyticFactory::getInstance().getName(i),this));
      _analyticActions.back()->setData(QVariant(i));
      connect(_analyticActions.back(),SIGNAL(triggered(bool)),this,SLOT(runAnalytic()));
   }
}






void Ace::MainWindow::createMenus()
{
   // create file menu
   _fileMenu = menuBar()->addMenu(tr("&File"));
   _fileMenu->addAction(_openAction);
   _fileMenu->addAction(_exitAction);

   // create analytic menu
   _analyticMenu = menuBar()->addMenu(tr("&Analytics"));
   for (int i = 0; i < EAbstractAnalyticFactory::getInstance().getCount() ;++i)
   {
      _analyticMenu->addAction(_analyticActions[i]);
   }

   // create OpenCL menu
   _settingsMenu = menuBar()->addMenu(tr("&OpenCL"));
   _settingsMenu->addAction(_setOpenCLAction);
}
