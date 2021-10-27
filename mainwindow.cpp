#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    com = new Communicator();
    //Связь сигналов и слотов
    connect(com,&Communicator::resp_countries,this,&MainWindow::resp_countries);
    connect(com,&Communicator::resp_summary,this,&MainWindow::resp_summary);
    connect(com,&Communicator::resp_country_info,this,&MainWindow::resp_country);
    connect(com,&Communicator::resp_city_info,this,&MainWindow::resp_city_info);
    //Инициализация таймера
    timer = new QTimer();
    connect(timer,SIGNAL(timeout()),this,SLOT(t_tick()));
    QChart *chart = new QChart();
    chartView = new QChartView(chart);
    chartView->setRenderHint(QPainter::Antialiasing);
    ui->plot_Layout->addWidget(chartView);
    //Инициализация вектора
    city_case = new QVector<int>;
    country_case = new QVector<int>;
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_pb_request_clicked()
{
    //com->getCountries();
    com->getSummary();
    com->getCountryInfo("Russia",ui->dateEdit->text());
    com->getCityInfo("Moscow",ui->dateEdit->text());
}

//Слот получения списка стран
void MainWindow::resp_countries(QByteArray resp)
{
    // То создаём объект Json Document, считав в него все данные из ответа
      QJsonDocument document = QJsonDocument::fromJson(resp);

      // Забираем из документа корневой объект
      QJsonObject root = document.object();
      /* Находим объект "departament", который располагается самым первым в корневом объекте.
       * С помощью метода keys() получаем список всех объектов и по первому индексу
       * забираем название объекта, по которому получим его значение
       * */
      ui->textEdit->clear();
      //ui->textEdit->append(root.keys().at(0) + ": " + root.value(root.keys().at(0)).toString());
      ui->textEdit->setText(document.toJson(QJsonDocument::Indented));
}

//Слот получения сводки
void MainWindow::resp_summary(QByteArray resp)
{
    // То создаём объект Json Document, считав в него все данные из ответа
      QJsonDocument document = QJsonDocument::fromJson(resp);
      // Забираем из документа корневой объект
      QJsonObject root = document.object();
      ui->textEdit->clear();
      ui->textEdit->setText(document.toJson(QJsonDocument::Indented));
       //Проверка входных данных
      if(root.keys().size()>0 && root["data"]!=NULL){
          //Сводка по миру
          QJsonObject global = root["data"].toObject();
          ui->textEdit->setText(global.value("confirmed").toString());
          //Данные о заболеваемости по миру
          int world_issues = global.value("confirmed").toInt();
          ui->lcdNumber_world->display(world_issues);
      }else{
          ui->textEdit->setText("Ошибка получения данных");
      }
}
