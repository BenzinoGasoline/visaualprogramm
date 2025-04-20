#include "mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), currentFile("")
{
    setupEditor();
    createActions();
    createToolBars();
    createNavigation();

    setWindowTitle(tr("Редактор электронных книг"));
    resize(800, 600);
}

MainWindow::~MainWindow()
{
}

void MainWindow::setupEditor()
{
    textEdit = new QTextEdit(this);
    textEdit->setAcceptRichText(true);

    navTree = new QTreeWidget(this);
    navTree->setHeaderLabel(tr("Структура книги"));

    splitter = new QSplitter(Qt::Horizontal, this);
    splitter->addWidget(navTree);
    splitter->addWidget(textEdit);
    splitter->setStretchFactor(1, 1);

    setCentralWidget(splitter);

    connect(navTree, &QTreeWidget::currentItemChanged,
            this, &MainWindow::currentItemChanged);
}

void MainWindow::createActions()
{

    newAct = new QAction(tr("&Новый"), this);
    connect(newAct, &QAction::triggered, this, &MainWindow::newDocument);

    openAct = new QAction(tr("&Открыть"), this);
    connect(openAct, &QAction::triggered, this, &MainWindow::openDocument);

    saveAct = new QAction(tr("&Сохранить"), this);
    connect(saveAct, &QAction::triggered, this, &MainWindow::saveDocument);

    saveAsAct = new QAction(tr("Сохранить &как"), this);
    connect(saveAsAct, &QAction::triggered, this, &MainWindow::saveAsDocument);


    boldAct = new QAction(tr("&Жирный"), this);
    boldAct->setCheckable(true);
    connect(boldAct, &QAction::triggered, this, &MainWindow::textBold);

    italicAct = new QAction(tr("&Курсив"), this);
    italicAct->setCheckable(true);
    connect(italicAct, &QAction::triggered, this, &MainWindow::textItalic);

    underlineAct = new QAction(tr("&Подчеркнутый"), this);
    underlineAct->setCheckable(true);
    connect(underlineAct, &QAction::triggered, this, &MainWindow::textUnderline);

    searchAct = new QAction(tr("&Найти"), this);
    connect(searchAct, &QAction::triggered, this, &MainWindow::searchText);

    findNextAct = new QAction(tr("Найти &следующее"), this);
    connect(findNextAct, &QAction::triggered, this, &MainWindow::findNext);

    findPrevAct = new QAction(tr("Найти &предыдущее"), this);
    connect(findPrevAct, &QAction::triggered, this, &MainWindow::findPrevious);


    addChapterAct = new QAction(tr("Добавить &главу"), this);
    connect(addChapterAct, &QAction::triggered, this, &MainWindow::addChapter);

    addPageAct = new QAction(tr("Добавить &страницу"), this);
    connect(addPageAct, &QAction::triggered, this, &MainWindow::addPage);

    removeItemAct = new QAction(tr("&Удалить"), this);
    connect(removeItemAct, &QAction::triggered, this, &MainWindow::removeItem);
}

void MainWindow::createToolBars()
{

    QToolBar *fileToolBar = addToolBar(tr("Файл"));
    fileToolBar->addAction(newAct);
    fileToolBar->addAction(openAct);
    fileToolBar->addAction(saveAct);
    fileToolBar->addAction(saveAsAct);


    formatToolBar = addToolBar(tr("Формат"));
    formatToolBar->addAction(boldAct);
    formatToolBar->addAction(italicAct);
    formatToolBar->addAction(underlineAct);


    searchToolBar = addToolBar(tr("Поиск"));
    searchLineEdit = new QLineEdit(this);
    searchToolBar->addWidget(searchLineEdit);
    searchToolBar->addAction(searchAct);
    searchToolBar->addAction(findNextAct);
    searchToolBar->addAction(findPrevAct);


    QToolBar *structToolBar = addToolBar(tr("Структура"));
    structToolBar->addAction(addChapterAct);
    structToolBar->addAction(addPageAct);
    structToolBar->addAction(removeItemAct);
}

void MainWindow::createNavigation()
{

    QTreeWidgetItem *root = new QTreeWidgetItem(navTree);
    root->setText(0, tr("Моя книга"));
    root->setData(0, Qt::UserRole, "book");

    QTreeWidgetItem *chapter1 = new QTreeWidgetItem(root);
    chapter1->setText(0, tr("Глава 1"));
    chapter1->setData(0, Qt::UserRole, "chapter");

    QTreeWidgetItem *page1 = new QTreeWidgetItem(chapter1);
    page1->setText(0, tr("Страница 1.1"));
    page1->setData(0, Qt::UserRole, "page");
    pageContents[page1] = "";

    QTreeWidgetItem *page2 = new QTreeWidgetItem(chapter1);
    page2->setText(0, tr("Страница 1.2"));
    page2->setData(0, Qt::UserRole, "page");
    pageContents[page2] = "";

    navTree->expandAll();
}

void MainWindow::newDocument()
{

    QTreeWidgetItem *current = navTree->currentItem();
    if (current && current->data(0, Qt::UserRole) == "page") {
        pageContents[current] = textEdit->toHtml();
    }

    textEdit->clear();
    navTree->clear();
    pageContents.clear();
    currentFile = "";


    QTreeWidgetItem *root = new QTreeWidgetItem(navTree);
    root->setText(0, tr("Моя книга"));
    root->setData(0, Qt::UserRole, "book");

    setWindowTitle(tr("Безымянный - Редактор электронных книг"));
}

void MainWindow::openDocument()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("Открыть файл"), "",
                                                   tr("Электронные книги (*.ebk);;Все файлы (*.*)"));

    if (fileName.isEmpty())
        return;

    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly)) {
        QMessageBox::warning(this, tr("Ошибка"), tr("Не удалось открыть файл"));
        return;
    }


    textEdit->clear();
    navTree->clear();
    pageContents.clear();

    QDataStream in(&file);


    int topLevelCount;
    in >> topLevelCount;

    for (int i = 0; i < topLevelCount; ++i) {
        loadTreeItem(nullptr, in);
    }

    file.close();
    currentFile = fileName;
    setWindowTitle(tr("%1 - Редактор электронных книг").arg(fileName));
}

void MainWindow::saveDocument()
{
    if (currentFile.isEmpty()) {
        saveAsDocument();
        return;
    }


    QTreeWidgetItem *current = navTree->currentItem();
    if (current && current->data(0, Qt::UserRole) == "page") {
        pageContents[current] = textEdit->toHtml();
    }

    QFile file(currentFile);
    if (!file.open(QIODevice::WriteOnly)) {
        QMessageBox::warning(this, tr("Ошибка"), tr("Не удалось сохранить файл"));
        return;
    }

    QDataStream out(&file);


    out << navTree->topLevelItemCount();
    for (int i = 0; i < navTree->topLevelItemCount(); ++i) {
        saveTreeItem(navTree->topLevelItem(i), out);
    }

    file.close();
}

void MainWindow::saveAsDocument()
{
    QString fileName = QFileDialog::getSaveFileName(this, tr("Сохранить как"), "",
                                                  tr("Электронные книги (*.ebk);;Все файлы (*.*)"));

    if (fileName.isEmpty())
        return;

    currentFile = fileName;
    saveDocument();
}

void MainWindow::saveTreeItem(QTreeWidgetItem *item, QDataStream &stream)
{
    stream << item->text(0) << item->data(0, Qt::UserRole).toString();


    if (item->data(0, Qt::UserRole) == "page") {
        stream << pageContents.value(item, "");
    }


    stream << item->childCount();
    for (int i = 0; i < item->childCount(); ++i) {
        saveTreeItem(item->child(i), stream);
    }
}

void MainWindow::loadTreeItem(QTreeWidgetItem *parent, QDataStream &stream)
{
    QString text, type;
    stream >> text >> type;

    QTreeWidgetItem *item = new QTreeWidgetItem(parent);
    item->setText(0, text);
    item->setData(0, Qt::UserRole, type);

    if (type == "page") {
        QString content;
        stream >> content;
        pageContents[item] = content;
    }

    int childCount;
    stream >> childCount;
    for (int i = 0; i < childCount; ++i) {
        loadTreeItem(item, stream);
    }

    if (parent) {
        parent->setExpanded(true);
    } else {
        navTree->addTopLevelItem(item);
    }
}

void MainWindow::textBold()
{
    QTextCharFormat fmt;
    fmt.setFontWeight(boldAct->isChecked() ? QFont::Bold : QFont::Normal);
    textEdit->mergeCurrentCharFormat(fmt);
}

void MainWindow::textItalic()
{
    QTextCharFormat fmt;
    fmt.setFontItalic(italicAct->isChecked());
    textEdit->mergeCurrentCharFormat(fmt);
}

void MainWindow::textUnderline()
{
    QTextCharFormat fmt;
    fmt.setFontUnderline(underlineAct->isChecked());
    textEdit->mergeCurrentCharFormat(fmt);
}

void MainWindow::searchText()
{
    QString searchString = searchLineEdit->text();
    if (searchString.isEmpty())
        return;

    bool found = textEdit->find(searchString);

    if (!found) {
        QMessageBox::information(this, tr("Поиск"),
                                tr("Достигнут конец документа. Поиск начнется с начала."));
        textEdit->moveCursor(QTextCursor::Start);
        textEdit->find(searchString);
    }
}

void MainWindow::findNext()
{
    QString searchString = searchLineEdit->text();
    if (searchString.isEmpty())
        return;

    bool found = textEdit->find(searchString);

    if (!found) {
        QMessageBox::information(this, tr("Поиск"),
                                tr("Достигнут конец документа. Поиск начнется с начала."));
        textEdit->moveCursor(QTextCursor::Start);
        textEdit->find(searchString);
    }
}

void MainWindow::findPrevious()
{
    QString searchString = searchLineEdit->text();
    if (searchString.isEmpty())
        return;

    bool found = textEdit->find(searchString, QTextDocument::FindBackward);

    if (!found) {
        QMessageBox::information(this, tr("Поиск"),
                                tr("Достигнуто начало документа. Поиск начнется с конца."));
        textEdit->moveCursor(QTextCursor::End);
        textEdit->find(searchString, QTextDocument::FindBackward);
    }
}

void MainWindow::addChapter()
{
    QTreeWidgetItem *current = navTree->currentItem();
    QTreeWidgetItem *parent = current ? current : navTree->invisibleRootItem();


    if (current && current->data(0, Qt::UserRole) == "page") {
        parent = current->parent();
    }

    QTreeWidgetItem *chapter = new QTreeWidgetItem(parent);
    chapter->setText(0, tr("Новая глава"));
    chapter->setData(0, Qt::UserRole, "chapter");
    chapter->setFlags(chapter->flags() | Qt::ItemIsEditable);

    if (parent)
        parent->setExpanded(true);
}

void MainWindow::addPage()
{
    QTreeWidgetItem *current = navTree->currentItem();
    QTreeWidgetItem *parent = current ? current : navTree->invisibleRootItem();


    if (current && current->data(0, Qt::UserRole) == "page") {
        parent = current->parent();
    }

    QTreeWidgetItem *page = new QTreeWidgetItem(parent);
    page->setText(0, tr("Новая страница"));
    page->setData(0, Qt::UserRole, "page");
    page->setFlags(page->flags() | Qt::ItemIsEditable);
    pageContents[page] = "";

    if (parent)
        parent->setExpanded(true);
}

void MainWindow::removeItem()
{
    QTreeWidgetItem *current = navTree->currentItem();
    if (!current || current == navTree->topLevelItem(0))
        return;

    if (current->data(0, Qt::UserRole) == "page") {
        pageContents.remove(current);
    }

    delete current;
}

void MainWindow::currentItemChanged(QTreeWidgetItem *current, QTreeWidgetItem *previous)
{

    if (previous && previous->data(0, Qt::UserRole) == "page") {
        pageContents[previous] = textEdit->toHtml();
    }


    if (current && current->data(0, Qt::UserRole) == "page") {
        textEdit->setHtml(pageContents.value(current, ""));
    } else {
        textEdit->clear();
    }
}

void MainWindow::closeEvent(QCloseEvent *event)
{

    QTreeWidgetItem *current = navTree->currentItem();
    if (current && current->data(0, Qt::UserRole) == "page") {
        pageContents[current] = textEdit->toHtml();
    }


    event->accept();
}
