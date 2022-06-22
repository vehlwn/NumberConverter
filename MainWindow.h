#pragma once

#include "NumberConverter.h"

#include <QMainWindow>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget* parent = nullptr);
    ~MainWindow() override;

private slots:
    void on_pushButtonConvert_clicked();
    void on_pushButtonClear_clicked();
    void on_plainTextEditInput_textChanged();
    void on_plainTextEditOutput_textChanged();
    void on_actionSaveResult_triggered();
    void on_actionExit_triggered();
    void on_actionHelpGrammar_triggered();
    void on_pushButtonSwap_clicked();

private:
    Ui::MainWindow* ui;
    nsNumberConverter::NumberConverter m_converter;
    QString m_qInputNumber, m_qResult;
    std::size_t m_base1{}, m_base2{}, m_precision{};

    void keyPressEvent(QKeyEvent* event) override;
    bool eventFilter(QObject* target, QEvent* event) override;
    void readFields();
    void clearFields();
};
