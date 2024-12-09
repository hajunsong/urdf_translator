#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QFileDialog>
#include <QtDebug>

#include "pugixml.hpp"
using namespace pugi;

#include <iostream>

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
	xml_document doc_input, doc_urdf;
	xml_parse_result result;

	void make_urdf();
	void make_link_node(xml_node robot, std::string name, std::string filename, std::string color="0 0 0 1");
	void make_joint_node(xml_node robot, std::string name, std::string link1, std::string link2, std::string type, std::string xyz, std::string rpy, std::string axis, double lower, double upper);
	void make_joint_node(xml_node robot, std::string name, std::string link1, std::string link2, std::string type, std::string xyz, std::string rpy);
	void prev_urdf();

public slots:
	// button event
	void btnFileClicked();
	void btnSaveClicked();

	// lineedit event
	void textChanged(QString txt);
};
#endif // MAINWINDOW_H
