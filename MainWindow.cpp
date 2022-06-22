#include "MainWindow.h"
#include "ui_MainWindow.h"

#include <QFileDialog>
#include <QMessageBox>
#include <QFile>
#include <QTextStream>

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

	ui->label_7->setText(QString::fromStdString(ut::toString(
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
	readFields();
	m_qResult = "";
	m_qResult = QString::fromStdString(m_conv(m_qInputNumber.toStdString()
								   , m_base1
								   , m_base2
								   , m_precision));
	ui->plainTextEditOutput->setPlainText(m_qResult);
}
catch(const nsNumberConverter::ParserException &ex)
{
	QMessageBox::critical(this, "Error", ex.what());
	auto cursor = ui->plainTextEditInput->textCursor();
	cursor.setPosition(ex.pos());
	cursor.setPosition(ex.pos() + 1, QTextCursor::KeepAnchor);
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
	ui->lineEditPrecision->setText("20");
	ui->plainTextEditOutput->setPlainText("");
	ui->plainTextEditInput->setFocus();
	m_qResult = "";
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

void MainWindow::readFields()
{
	m_qInputNumber = ui->plainTextEditInput->toPlainText();
	m_base1 = std::clamp<std::size_t>(ui->lineEditBase1->text().toUInt(), m_conv.minBase(), m_conv.maxBase());
	ui->lineEditBase1->setText(QString::number(m_base1));
	m_base2 = std::clamp<std::size_t>(ui->lineEditBase2->text().toUInt(), m_conv.minBase(), m_conv.maxBase());
	ui->lineEditBase2->setText(QString::number(m_base2));
	m_precision = ui->lineEditPrecision->text().toUInt();
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

void MainWindow::on_actionSaveResult_triggered() try
{
	// Save result to a file.
	if(m_qResult.isEmpty())
		throw std::runtime_error("No computed result.");

	static QString oldName = "NumberConverter";
	const QString fileName = QFileDialog::getSaveFileName(this,
													"Save result",
													oldName,
													"Text files (*.txt);;All files (*.*)");

	if(fileName.isEmpty()) return;
	oldName = fileName;

	QFile file(fileName);
	bool f = file.open(QIODevice::WriteOnly | QIODevice::Text);
	if(!f)
		throw std::runtime_error("Cannot open " + fileName.toStdString());

	QTextStream streamOut(&file);
	streamOut << m_qInputNumber << "_{" << m_base1 << "} = "
			  << m_qResult << "_{" << m_base2 << "}";
}
catch(const std::exception &ex)
{
	QMessageBox::critical(this, "Error", ex.what());
}

