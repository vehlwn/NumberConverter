#include "MainWindow.h"

#include "ui_MainWindow.h"

#include <algorithm>
#include <memory>

#include <QFile>
#include <QFileDialog>
#include <QMessageBox>
#include <QTextStream>

MainWindow::MainWindow(QWidget* parent)
	: QMainWindow{parent}
	, ui{new Ui::MainWindow}
{
	ui->setupUi(this);

	ui->plainTextEditInput->installEventFilter(this);
	ui->plainTextEditOutput->installEventFilter(this);

	clearFields();

	ui->label_7->setText(
		QString{"(%1..%2)"}.arg(m_converter.minBase()).arg(m_converter.maxBase()));
}

MainWindow::~MainWindow()
{
	delete ui;
}

void MainWindow::on_pushButton_clicked()
try
{
	readFields();
	m_qResult = "";
	m_qResult = QString::fromStdString(
		m_converter(m_qInputNumber.toStdString(), m_base1, m_base2, m_precision));
	ui->plainTextEditOutput->setPlainText(m_qResult);
}
catch(const nsNumberConverter::ParserException& ex)
{
	QMessageBox::critical(this, "Error", ex.what());
	auto cursor = ui->plainTextEditInput->textCursor();
	cursor.setPosition(static_cast<int>(ex.pos()));
	cursor.setPosition(static_cast<int>(ex.pos() + 1), QTextCursor::KeepAnchor);
	ui->plainTextEditInput->setTextCursor(cursor);
	ui->plainTextEditInput->setFocus();
}
catch(const std::exception& ex)
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

void MainWindow::keyPressEvent(QKeyEvent* event)
{
	switch(event->key())
	{
	case Qt::Key_Return:
	case Qt::Key_Enter:
		on_pushButton_clicked();
		break;

	case Qt::Key_Escape:
		on_pushButton_2_clicked();
		break;
	}
}

bool MainWindow::eventFilter(QObject* target, QEvent* event)
{
	if(target == ui->plainTextEditInput || target == ui->plainTextEditOutput)
	{
		if(event->type() == QEvent::KeyPress)
		{
			const auto keyEvent = static_cast<QKeyEvent*>(event);
			switch(keyEvent->key())
			{
			case Qt::Key_Return:
			case Qt::Key_Enter:
				on_pushButton_clicked();
				return true;

			case Qt::Key_Escape:
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

	m_base1 = std::clamp<std::size_t>(
		ui->lineEditBase1->text().toUInt(), m_converter.minBase(), m_converter.maxBase());
	ui->lineEditBase1->setText(QString::number(m_base1));

	m_base2 = std::clamp<std::size_t>(
		ui->lineEditBase2->text().toUInt(), m_converter.minBase(), m_converter.maxBase());
	ui->lineEditBase2->setText(QString::number(m_base2));

	m_precision = ui->lineEditPrecision->text().toUInt();
	ui->lineEditPrecision->setText(QString::number(m_precision));
}

void MainWindow::on_plainTextEditInput_textChanged()
{
	ui->label_5->setText(QString::number(ui->plainTextEditInput->toPlainText().size()));
}

void MainWindow::on_plainTextEditOutput_textChanged()
{
	ui->label_6->setText(QString::number(ui->plainTextEditOutput->toPlainText().size()));
}

void MainWindow::on_actionSaveResult_triggered()
try
{
	// Save result to a file.
	if(m_qResult.isEmpty())
		throw std::runtime_error("No computed result.");

	static QString oldName = "NumberConverter";
	const QString fileName = QFileDialog::getSaveFileName(
		this, "Save result", oldName, "Text files (*.txt);;All files (*.*)");
	if(fileName.isEmpty())
		return;
	oldName = fileName;

	QFile file{fileName};
	bool f = file.open(QIODevice::WriteOnly | QIODevice::Text);
	if(!f)
		throw std::runtime_error("Cannot open " + fileName.toStdString());

	QTextStream streamOut{&file};
	streamOut << m_qInputNumber << "_{" << m_base1 << "} = " << m_qResult << "_{"
			  << m_base2 << "}";
}
catch(const std::exception& ex)
{
	QMessageBox::critical(this, "Error", ex.what());
}

void MainWindow::on_actionExit_triggered()
{
	// Exit.
	this->close();
}

void MainWindow::on_actionHelpGrammar_triggered()
{
	QMessageBox::information(this, "Grammar",
		QString{"Start <- [ \\t\\r\\n]* ("
				"\n    IntPart '%1' FractPart"
				"\n    / IntPart '%1'?"
				"\n    / '%1' FractPart"
				"\n)"
				"\nIntPart <- Digit+"
				"\nFractPart <- Digit+"
				"\nDigit <- [0-9a-zA-Z]"}
			.arg(nsNumberConverter::NumberConverter::decimal_point()));
}
