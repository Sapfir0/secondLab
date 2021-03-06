﻿#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "fotobasetablewidgetitem.h"
#include <QCloseEvent>
#include <fotobase.h>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    createWindow();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::setCheckRes() {
    qDebug() << stopIt;
    if (stopIt) {
        if (ui->analogOrNot->isChecked()) {
            ui->matrixResolution->setEnabled(true);
            qDebug() << "Включение рез";
        }

        else {
            ui->matrixResolution->setEnabled(false);
            qDebug() << "Отключение рез";
        }
    }
}

void MainWindow::setCheckPolProf() {
    if (stopIt) {
        if (ui->category->currentIndex() == 2) {
            ui->changeLens->setEnabled(true);
            qDebug() << "Включение линзы";
        }
        else {
            ui->changeLens->setEnabled(false);
            qDebug() << "Отключение линзы";
        }
    }
}

fotobase MainWindow::createRecord() //из ui в экземпляр класса
{
    fotobase write;

    write.setNameOfModel(ui->nameOfModel->text());
    write.setCategory(ui->category->currentText());
    write.setAnalogOrNot(ui->analogOrNot->isChecked());
    write.setProducer(ui->producer->currentText());
    write.setMatrRes(ui->matrixResolution->value());
    write.setChangeLense(ui->changeLens->isChecked());
    write.setSize(ui->size->text());
    write.setWeight(ui->weight->value());
    write.setCost(ui->cost->value());
    write.setmyDate(ui->date->date());

    return write;
}

void MainWindow::on_filling_clicked()
{
    //initializationTable(db.count()+10);
    for (int i=0; i<10; i++) {
        fotobase temp = fotobase::randomix();
        addRecordToDatabase(temp);
        //setToUi(temp.id, db.count());

    }
    //sorting();

}

void MainWindow::loadRecord(fotobase value) //выводит на ui данные из экземпляра класса
{
    //set ui from value;
    ui->nameOfModel->setText(value.getNameOfModel());
    ui->category->setCurrentText(value.getGategory());
    ui->analogOrNot->setChecked(value.getAnalogOrNot());
    setCheckRes();
    ui->producer->setCurrentText(value.getProducer());
    ui->matrixResolution->setValue(value.getMatrRes());
    setCheckPolProf();
    ui->changeLens->setChecked(value.getChangeLense());
    ui->size->setText(value.getSize());
    ui->weight->setValue(value.getWeight());
    ui->cost->setValue(value.getCost());
    ui->date->setDate(value.getmyDate());
}


void MainWindow::initializationTable (int rows) {

    ui->spisok->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->spisok->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->spisok->setEditTriggers(QAbstractItemView::NoEditTriggers);

    ui->spisok->setRowCount(rows);
}


void MainWindow::on_saveBtn_clicked() //нажатие на кнопку Сохранить
{
    if (ui->nameOfModel->text() == nullptr ) {//пустая строка
        QMessageBox::warning(nullptr,"Alert", "Название не задано");
        return;
    }

    callEnableDisable=0;
    //db.database.insert(numberOfRecords, createRecord());
    if (createClicked) {
        //initializationTable(db.count()+1);
        fotobase temp = createRecord();
        addRecordToDatabase(temp);
        qDebug() << temp.id << " ПОЧЕМУ ТЫ ВЫЛЕТАЕШЬ " << db.count();
        //setToUi(temp.id, db.count());/////////////////////////////////////////
        //addRecordToUi(temp.id);

    } else {
        auto t0 = static_cast<fotobaseTableWidgetItem*>(ui->spisok->item(indexOfRecord, 0));
        auto t1 = static_cast<fotobaseTableWidgetItem*>(ui->spisok->item(indexOfRecord, 1));
        db.update(t0->get_id(), createRecord());
        t0->update_text();
        t1->update_text();
    }

    if (edit != 1) {
        indexOfRecord++;
    }
    editMode(false);

    sorting();
}



void MainWindow::sorting() {
    //  Записи упорядочиваются по следующим полям: категория, разрешение матрицы, цена, производитель, модель
    ui->spisok->sortItems(0);
}

void MainWindow::on_denied_clicked()//нажатие на кнопку Отменить
{
    edit =0;
    callEnableDisable=0;
    editMode(false);

    if (!createClicked && db.count() != 0)
        on_spisok_currentCellChanged(indexOfRecord);
}

void MainWindow::enableDisableEdit(bool arg) {

    ui->analogOrNot->setEnabled(arg);
    ui->category->setEnabled(arg);
    ui->cost->setEnabled(arg);
    ui->date->setEnabled(arg);
    ui->nameOfModel->setEnabled(arg);
    ui->producer->setEnabled(arg);
    ui->size->setEnabled(arg);
    ui->weight->setEnabled(arg);

    ui->saveBtn->setVisible(arg);
    ui->denied->setVisible(arg);

    ui->filling->setEnabled(!arg);
    ui->deleteBtn->setEnabled(!arg);
    ui->createBtn->setEnabled(!arg);
    ui->editBtn->setEnabled(!arg);
}

void MainWindow::on_editBtn_clicked()
{

    if ( callEnableDisable == 0)    {
        editMode(true);
        //db.database.replace(indexOfRecord, createRecord() );
        setCheckPolProf();
        setCheckRes();
        qDebug() << "callEnableDisable == 0";
        //не уверен что нужно то что ниже
        callEnableDisable=1;
    }
    else if(callEnableDisable ==1) {
        editMode(false);
        qDebug() << "callEnableDisable == 1";
        callEnableDisable=0;
    }
    ui->matrixResolution->setEnabled(false);
    ui->changeLens->setEnabled(false);
    edit=1;

    createClicked=false;
}


void MainWindow::on_createBtn_clicked()
{
    editMode(true);
    createClicked = true;
    loadRecord( fotobase() );

}


void MainWindow::editMode(bool arg) {
    stopIt = arg; //редактирование пошло
    if (arg)
        ui->statusBar->showMessage("Режим редактирования");
    else
        ui->statusBar->showMessage(" ");

    enableDisableEdit(arg);
    //    setCheckPolProf();
    //    setCheckRes();

    ui->changeLens->setEnabled(false);
    ui->matrixResolution->setEnabled(false);

    //закончено
}

void MainWindow::on_deleteBtn_clicked()
{
    ui->spisok->setCurrentCell(ui->spisok->currentRow(), 0);
    on_spisok_currentCellChanged(ui->spisok->currentRow());
    db.remove(currentId);

    //ui->spisok->removeRow(indexOfRecord);
}


void MainWindow::on_spisok_currentCellChanged(int currentRow)
{
    editMode(false);
    ui->changeLens->setEnabled(false);
    ui->matrixResolution->setEnabled(false);
    //    callEnableDisable=0;

    indexOfRecord = currentRow;

    bool dataBaseIsEmpty = db.count() == 0;

    ui->editBtn->setEnabled(!dataBaseIsEmpty);
    ui->deleteBtn->setEnabled(!dataBaseIsEmpty);

    qDebug() << "current record: " << indexOfRecord;
    if (indexOfRecord != -1) {
        auto item = static_cast<fotobaseTableWidgetItem*>(ui->spisok->item(indexOfRecord, 0));
        if (item) {
            currentId = item->get_id();
            loadRecord(db.record(currentId));
        } else {
            qDebug() << "item is empty";
        }
    }

    if (dataBaseIsEmpty) {
        loadRecord(fotobase());//кидаем пустые поля
    }

}



void MainWindow::closeEvent(QCloseEvent *cEvent){
    //    QMessageBox::StandardButton wquit = QMessageBox::question(this, "Внимание", "Вы действительно хотите выйти?");
    //    if (wquit == QMessageBox::Yes) {

    //        cEvent->accept();
    //        if (db.isModified()) {
    //            //saveChanges();
    //        }
    //    }
    //    else cEvent->ignore();
}

void MainWindow::createWindow() {

    QRegExp AcceptIter ("^[a-zA-zа-яА-Я]\\w{0,29}");
    QValidator *validno = new QRegExpValidator(AcceptIter, this);
    ui->nameOfModel->setValidator(validno);

    connect( ui->analogOrNot, SIGNAL(clicked(bool)), this, SLOT(setCheckRes()) );
    connect( ui->category, SIGNAL(currentIndexChanged(int)), this, SLOT(setCheckPolProf()) );

    enableDisableEdit(false);

    ui->size->setCursorPosition(0);
    ui->size->setInputMask("00-00-00 мм.");  //нужна маска

    ui->nameOfModel->setPlaceholderText("Название модели");

    ui->cost->setSuffix(" руб.");
    ui->weight->setSuffix(" гр.");
    setWindowTitle("Почти курсач");

    ui->saveBtn->hide();
    ui->denied->hide();

    ui->changeLens->setEnabled(false);
    ui->matrixResolution->setEnabled(false);

    ui->spisok->setSortingEnabled(true);
    QVector<fotobase> temp_vector = db.records();
    QVectorIterator<fotobase> it(temp_vector);
    while (it.hasNext()) {
        auto item = it.next();
        auto id = item.id;
        addRecordToUi(id);
    }
    connect(&db, &DataBaseController::update_signal, this, &MainWindow::updateRecordByID);
    connect(&db, &DataBaseController::append_signal, this, &MainWindow::addRecordToUi);
    connect(&db, &DataBaseController::remove_signal, this, &MainWindow::removeRecordFromUiByID);
    connect(&db, &DataBaseController::clear_signal , this, &MainWindow::clearBrowser);
    connect(&db, &DataBaseController::server_stop_signal, [this](){
        QMessageBox::critical(nullptr, "Ошибка подключения к серверу", "К сожалению приложение не может работать");
        this->close();
    });

}

void MainWindow::addRecordToDatabase(const fotobase &data) {
    db.append(data);
}


void MainWindow::addRecordToUi(uint id) {

    auto model = new fotobaseTableWidgetItem(id, &db, 0);
    auto cost = new fotobaseTableWidgetItem(id, &db, 1);
    //cost->update_text();

    auto count = ui->spisok->rowCount();
    ui->spisok->setRowCount(count+1);
    ui->spisok->setItem(count, 0, model);
    ui->spisok->setItem(count, 1, cost);

    browserWidgetItems.insert(std::make_pair(id, model));
}

void MainWindow::updateRecordByID(uint id) {
    removeRecordFromUiByID(id);
    addRecordToUi(id);
}

void MainWindow::removeRecordFromUiByID(uint id) {
    auto it = browserWidgetItems.find(id);
    if (it != browserWidgetItems.end()) {
        auto& item = it->second;
        ui->spisok->removeRow(item->row());
        browserWidgetItems.erase(it);
    }
}

void MainWindow::clearBrowser() {
    browserWidgetItems.clear();
    ui->spisok->clear();
}
