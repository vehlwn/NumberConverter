#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMessageBox>


#include "NumberConverter.h"


namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
	Q_OBJECT

public:
	explicit MainWindow(QWidget *parent = nullptr);
	virtual ~MainWindow() override;

private slots:
	void on_action_triggered();

	void on_pushButton_clicked();

	void on_pushButton_2_clicked();

	void on_plainTextEditInput_textChanged();

	void on_plainTextEditOutput_textChanged();

	void on_action_3_triggered();

private:
	Ui::MainWindow *ui;

	nsNumberConverter::NumberConverter m_conv;

	void keyPressEvent(QKeyEvent *event) override;
	bool eventFilter(QObject *target, QEvent *event) override;
	void clearFields();
};

#endif // MAINWINDOW_H
