#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTextEdit>
#include <QTreeWidget>
#include <QToolBar>
#include <QAction>
#include <QLineEdit>
#include <QSplitter>
#include <QFile>
#include <QFileDialog>
#include <QTextStream>
#include <QMessageBox>
#include <QDataStream>
#include <QCloseEvent>
#include <QMap>

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

protected:
    void closeEvent(QCloseEvent *event) override;

private slots:
    void newDocument();
    void openDocument();
    void saveDocument();
    void saveAsDocument();

    void textBold();
    void textItalic();
    void textUnderline();

    void searchText();
    void findNext();
    void findPrevious();

    void addChapter();
    void addPage();
    void removeItem();

    void currentItemChanged(QTreeWidgetItem *current, QTreeWidgetItem *previous);

private:
    void createActions();
    void createToolBars();
    void createNavigation();
    void setupEditor();
    void saveTreeItem(QTreeWidgetItem *item, QDataStream &stream);
    void loadTreeItem(QTreeWidgetItem *parent, QDataStream &stream);

    QTextEdit *textEdit;
    QTreeWidget *navTree;
    QToolBar *formatToolBar;
    QToolBar *searchToolBar;
    QLineEdit *searchLineEdit;
    QSplitter *splitter;

    QString currentFile;
    QMap<QTreeWidgetItem*, QString> pageContents;

    QAction *newAct;
    QAction *openAct;
    QAction *saveAct;
    QAction *saveAsAct;

    QAction *boldAct;
    QAction *italicAct;
    QAction *underlineAct;

    QAction *searchAct;
    QAction *findNextAct;
    QAction *findPrevAct;

    QAction *addChapterAct;
    QAction *addPageAct;
    QAction *removeItemAct;
};

#endif // MAINWINDOW_H
