#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "algo.h"

#include <algorithm>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow) {
    ui->setupUi(this);
    ui->txt_size->setValidator(new QIntValidator(0, 1000, this));

    ApplyModel();
}

MainWindow::~MainWindow() {
    delete ui;
}

void MainWindow::on_btn_edit_clicked() {
    *deque_model_.iterator = ui->txt_elem_content->text().toStdString();

    ApplyModel();
}

void MainWindow::ApplyModel() {
    QStringList elements;
    for(size_t i = 0; i < deque_model_.items.size(); ++i) {
        elements << QString::fromStdString(std::to_string(i) + ": " + deque_model_.items[i]);
    }
    elements << "end";
    auto preserve_iter = deque_model_.iterator;

    ui->list_widget->clear();
    ui->list_widget->addItems(elements);

    ui->txt_size->setText(QString::number(deque_model_.items.size()));
    bool is_empty = deque_model_.items.empty();
    ui->btn_pop_back->setDisabled(is_empty);
    ui->btn_pop_front->setDisabled(is_empty);
    ui->btn_erase->setDisabled(is_empty);

    deque_model_.iterator = preserve_iter;
    ApplyIterator();
}

void MainWindow::ApplyIterator() {
    bool disabled = deque_model_.iterator == deque_model_.items.end();
    auto position = deque_model_.iterator - deque_model_.items.begin();
    ui->list_widget->setCurrentRow(position);
    ui->btn_edit->setDisabled(disabled);
    ui->btn_erase->setDisabled(disabled);
    if (!disabled) {
        ui->txt_elem_content->setText(QString::fromStdString(*deque_model_.iterator));
    } else {
        ui->txt_elem_content->setText("");
    }
    ui->btn_dec_iterator->setDisabled(deque_model_.iterator == deque_model_.items.begin());
    ui->btn_inc_iterator->setDisabled(disabled);
}

void MainWindow::on_btn_push_front_clicked() {
    deque_model_.items.push_front(ui->txt_elem_content->text().toStdString());
    deque_model_.iterator = deque_model_.items.begin();
    ApplyModel();
}

void MainWindow::on_btn_push_back_clicked() {
    deque_model_.items.push_back(ui->txt_elem_content->text().toStdString());
    deque_model_.iterator = deque_model_.items.begin();
    ApplyModel();
}

void MainWindow::on_btn_pop_front_clicked() {
    deque_model_.items.pop_front();
    deque_model_.iterator = deque_model_.items.begin();
    ApplyModel();
}

void MainWindow::on_btn_pop_back_clicked() {
    deque_model_.items.pop_back();
    deque_model_.iterator = deque_model_.items.begin();
    ApplyModel();
}

void MainWindow::on_btn_erase_clicked() {
    deque_model_.items.erase(deque_model_.iterator);
    deque_model_.iterator = deque_model_.items.begin();
    ApplyModel();
}

void MainWindow::on_btn_resize_clicked() {
    bool conversion_ok = false;
    int new_size = ui->txt_size->text().toInt(&conversion_ok);
    if (!conversion_ok) {
        return;
    }
    if (new_size < 0 || new_size > 1000) {
        return;
    }
    deque_model_.items.resize(new_size);
    deque_model_.iterator = deque_model_.items.begin();
    ApplyModel();
}

void MainWindow::on_btn_insert_clicked() {
    deque_model_.items.insert(deque_model_.iterator, ui->txt_elem_content->text().toStdString());
    deque_model_.iterator = deque_model_.items.begin();
    ApplyModel();
}

void MainWindow::on_btn_clear_clicked() {
    deque_model_.items.clear();
    deque_model_.iterator = deque_model_.items.begin();
    ApplyModel();
}

void MainWindow::on_btn_dec_iterator_clicked() {
    --deque_model_.iterator;
    ApplyIterator();
}

void MainWindow::on_btn_inc_iterator_clicked() {
    ++deque_model_.iterator;
    ApplyIterator();
}

void MainWindow::on_btn_begin_clicked() {
    deque_model_.iterator = deque_model_.items.begin();
    ApplyIterator();
}

void MainWindow::on_btn_end_clicked() {
    deque_model_.iterator = deque_model_.items.end();
    ApplyIterator();
}

void MainWindow::on_list_widget_currentRowChanged(int currentRow) {
    currentRow = std::clamp(currentRow, 0, (int)deque_model_.items.size());
    deque_model_.iterator = deque_model_.items.begin() + currentRow;
    ApplyIterator();
}

void MainWindow::on_btn_min_element_clicked()
{
    deque_model_.iterator = std::min_element(deque_model_.items.begin(), deque_model_.items.end());
    ApplyIterator();
}

void MainWindow::on_btn_max_element_clicked()
{
    deque_model_.iterator = std::max_element(deque_model_.items.begin(), deque_model_.items.end());
    ApplyIterator();
}

void MainWindow::on_btn_count_clicked()
{
    QString text = ui->le_count->text();
    auto result = std::count(deque_model_.items.begin(), deque_model_.items.end(), text.toStdString());
    ui->lbl_count->setText(QString::number(result));
}

void MainWindow::on_btn_unique_clicked() {
    bool deque_is_sorted = std::is_sorted(deque_model_.items.begin(), deque_model_.items.end());

    if (!deque_is_sorted) {
        return;
    }

    deque_model_.items.erase(std::unique(deque_model_.items.begin(), deque_model_.items.end()), deque_model_.items.end());
    deque_model_.iterator = deque_model_.items.begin();
    ApplyModel();
}

void MainWindow::on_btn_sort_case_insensitive_clicked() {
    static const auto compare_ci = [](const std::string& l, const std::string& r) {
        return QString::compare(QString::fromStdString(l), QString::fromStdString(r), Qt::CaseInsensitive) < 0;
    };
    deque_model_.items = MergeSort(deque_model_.items, compare_ci);
    deque_model_.iterator = deque_model_.items.begin();
    ApplyModel();
}

void MainWindow::on_btn_shuffle_clicked() {
    std::shuffle(deque_model_.items.begin(), deque_model_.items.end(), random_gen_);
    ApplyModel();
}

void MainWindow::on_btn_sort_clicked() {
    deque_model_.items = MergeSort(deque_model_.items, std::less<std::string>());
    deque_model_.iterator = deque_model_.items.begin();
    ApplyModel();
}

void MainWindow::on_btn_reverse_clicked() {
    std::reverse(deque_model_.items.begin(), deque_model_.items.end());
    ApplyModel();
}

void MainWindow::on_btn_tea_clicked() {
    static std::deque<std::string> tea {
        "Чай Лунцзин",
        "Эрл Грей",
        "Сенча",
        "Пуэр",
        "Дарджилинг",
        "Ассам",
        "Матча",
        "Ганпаудер",
        "Оолонг",
        "Лапсанг Сушонг"
    };

    deque_model_.items = tea;
    deque_model_.iterator = deque_model_.items.begin();
    ApplyModel();
}

void MainWindow::on_btn_cakes_clicked() {
    static std::deque<std::string> cakes {
        "Красный бархат",
        "Наполеон",
        "Медовик",
        "Тирамису",
        "Прага",
        "Чизкейк",
        "Захер",
        "Эстерхази",
        "Морковный торт",
        "Чёрный лес",
    };

    deque_model_.items = cakes;
    deque_model_.iterator = deque_model_.items.begin();
    ApplyModel();
}

void MainWindow::on_btn_find_clicked()
{
    deque_model_.iterator = std::find(deque_model_.items.begin(), deque_model_.items.end(), ui->txt_elem_content->text().toStdString());
    ApplyIterator();
}

void MainWindow::on_btn_lower_bound_clicked()
{
    if (!std::is_sorted(deque_model_.items.begin(), deque_model_.items.end())) {
        return;
    }
    deque_model_.iterator = std::lower_bound(deque_model_.items.begin(), deque_model_.items.end(), ui->txt_elem_content->text().toStdString());
    ApplyIterator();
}

void MainWindow::on_btn_upper_bound_clicked()
{
    if (!std::is_sorted(deque_model_.items.begin(), deque_model_.items.end())) {
        return;
    }
    deque_model_.iterator = std::upper_bound(deque_model_.items.begin(), deque_model_.items.end(), ui->txt_elem_content->text().toStdString());
    ApplyIterator();
}
