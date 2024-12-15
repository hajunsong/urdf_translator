#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QFileDialog>
#include <QtDebug>

#include "pugixml.hpp"
using namespace pugi;

#include <iostream>
#include <string>
#include <cstring>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
	Q_OBJECT

public:
	MainWindow(QWidget *parent = nullptr);
	~MainWindow();

private:
	Ui::MainWindow *ui;

	QString loadFileName;
//	xml_document<> doc_input, doc_urdf;
	xml_document doc_input, doc_urdf;

	void make_urdf();
	void make_link_node(xml_node robot, xml_node input);
	void make_joint_node(xml_node robot, xml_node input);

public slots:
	// button event
	void btnFileClicked();
	void btnSaveClicked();

	// lineedit event
	void textChanged(QString txt);
};

struct xml_string_writer: pugi::xml_writer
{
	std::string result;

	virtual void write(const void* data, size_t size)
	{
		result.append(static_cast<const char*>(data), size);
	}
};
#endif // MAINWINDOW_H
