#include "MainWindow.h"
#include "ui_MainWindow.h"

#include <memory>
#include <algorithm>

#include "Ut.h"

MainWindow::MainWindow(QWidget *parent) :
	QMainWindow(parent),
	ui(new Ui::MainWindow)
{
	ui->setupUi(this);

	ui->plainTextEditInput->installEventFilter(this);
	ui->plainTextEditOutput->installEventFilter(this);

	clearFields();

	ui->label_7->setText(QString::fromStdString(Ut::toString(
		"(", m_conv.minBase(), "..", m_conv.maxBase(), ")")));
}

MainWindow::~MainWindow()
{
	delete ui;
}

void MainWindow::on_action_triggered()
{
	// Exit.
	this->close();
}

void MainWindow::on_pushButton_clicked() try
{
	const std::string inputNumber = ui->plainTextEditInput->toPlainText().toStdString();

	const auto base1 = std::clamp<std::size_t>(ui->lineEditBase1->text().toUInt(), m_conv.minBase(), m_conv.maxBase());
	ui->lineEditBase1->setText(QString::number(base1));

	const auto base2 = std::clamp<std::size_t>(ui->lineEditBase2->text().toUInt(), m_conv.minBase(), m_conv.maxBase());
	ui->lineEditBase2->setText(QString::number(base2));

	const auto digitsAfterPoint = static_cast<std::size_t>(ui->lineEdit_3->text().toUInt());

	const std::string res = m_conv(inputNumber, base1, base2, digitsAfterPoint);
	ui->plainTextEditOutput->setPlainText(QString::fromStdString(res));
}
catch(const nsNumberConverter::ParserException &ex)
{
	QMessageBox::critical(this, "Error", ex.what());

	auto cursor = ui->plainTextEditInput->textCursor();
	cursor.movePosition(QTextCursor::Start);
	cursor.movePosition(QTextCursor::Right, QTextCursor::MoveAnchor, ex.pos());
	ui->plainTextEditInput->setTextCursor(cursor);
	ui->plainTextEditInput->setFocus();
}
catch(const std::exception &ex)
{
	QMessageBox::critical(this, "Error", ex.what());
}

void MainWindow::clearFields()
{
	ui->plainTextEditInput->setPlainText("");
	ui->lineEditBase1->setText("10");
	ui->lineEditBase2->setText("2");
	ui->lineEdit_3->setText("20");
	ui->plainTextEditOutput->setPlainText("");
	ui->plainTextEditInput->setFocus();

}

void MainWindow::on_pushButton_2_clicked()
{
	clearFields();
}


void MainWindow::keyPressEvent(QKeyEvent *event)
{
	switch(event->key())
	{
	case Qt::Key_Return :
	case Qt::Key_Enter :
		on_pushButton_clicked();
		break;

	case Qt::Key_Escape :
		on_pushButton_2_clicked();
		break;
	}
}

bool MainWindow::eventFilter(QObject *target, QEvent *event)
{
	if(target == ui->plainTextEditInput || target == ui->plainTextEditOutput)
	{
		if(event->type() == QEvent::KeyPress)
		{
			const auto keyEvent = dynamic_cast<QKeyEvent *>(event);
			switch(keyEvent->key())
			{
			case Qt::Key_Return :
			case Qt::Key_Enter :
				on_pushButton_clicked();
				return true;

			case Qt::Key_Escape :
				on_pushButton_2_clicked();
				return true;
			}
		}
	}
	return QMainWindow::eventFilter(target, event);
}


void MainWindow::on_plainTextEditInput_textChanged()
{
	ui->label_5->setText(QString::number(ui->plainTextEditInput->toPlainText().size()));
}

void MainWindow::on_plainTextEditOutput_textChanged()
{
	ui->label_6->setText(QString::number(ui->plainTextEditOutput->toPlainText().size()));
}

void MainWindow::on_action_3_triggered()
{
	QMessageBox::information(this, "Grammar", R"(E -> [0-9a-zA-Z]*(\.[0-9a-zA-Z]*)?)");
}
