#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)	: QMainWindow(parent), ui(new Ui::MainWindow)
{
	ui->setupUi(this);

	connect(ui->btnFile, SIGNAL(clicked()), this, SLOT(btnFileClicked()));
	connect(ui->btnSave, SIGNAL(clicked()), this, SLOT(btnSaveClicked()));

	ui->btnSave->setEnabled(false);

	connect(ui->txtName, SIGNAL(textChanged(QString)), this, SLOT(textChanged(QString)));
}

MainWindow::~MainWindow()
{
	delete ui;
}

void MainWindow::btnFileClicked(){
	QString pwd("");
	char *PWD;
	PWD = getenv("PWD");
	pwd.append(PWD);
	loadFileName = QFileDialog::getOpenFileName(this,QString::fromLocal8Bit("open file"), (pwd + "../"), "File (*.xml)");

	if(loadFileName.size() > 0){
		ui->txtFilePath->setText(loadFileName);

		xml_parse_result result = doc_input.load_file(loadFileName.toStdString().c_str());

		ui->txtPreview->clear();
		if(result.status > 0){
			std::string result_desc = result.description();
			std::cout << result_desc << std::endl;
			ui->txtPreview->append(result_desc.c_str());
		}
		else{
			doc_urdf.reset();
			make_urdf();

			xml_string_writer writer;
			doc_urdf.print(writer);
			ui->txtPreview->append(writer.result.c_str());
		}
	}
}

void MainWindow::btnSaveClicked(){
//	qDebug() << loadFileName;
	QStringList fileNameSplit = loadFileName.split("/");
	int indx1 = loadFileName.indexOf(".");
	int indx2 = loadFileName.lastIndexOf("/");
	QString saveFileName = loadFileName.remove(indx2 + 1, indx1-1-indx2 + 4) + ui->txtName->text() + ".urdf";
//	qDebug() << saveFileName;

	doc_urdf.save_file(saveFileName.toStdString().c_str());
}

void MainWindow::textChanged(QString txt){
	if(txt.size() > 0){
		ui->btnSave->setEnabled(true);
	}
	else{
		ui->btnSave->setEnabled(false);
	}
}

void MainWindow::make_urdf(){
	xml_node cell = doc_input.child("cell");

	xml_node dec = doc_urdf.append_child(node_declaration);
	dec.append_attribute("version") = "1.0";
	dec.append_attribute("encoding") = "UTF-8";

	xml_node robot = doc_urdf.append_child("robot");
	robot.append_attribute("name").set_value(cell.child_value("name"));

	robot.append_child("link").append_attribute("name").set_value(cell.child_value("root"));
	xml_node base = cell.child("base");
	make_link_node(robot.append_child("link"), base);

	xml_node object = cell.child("object");
	for(xml_node link : object.children("link")){
		make_link_node(robot.append_child("link"), link);
	}

	xml_node tf = cell.child("TF");
	for(xml_node joint : tf.children("joint")){
		make_joint_node(robot.append_child("joint"), joint);
	}
}

void MainWindow::make_link_node(xml_node link, xml_node input){
//	xml_node link = robot.append_child("link");
	link.append_attribute("name").set_value(input.child_value("name"));
	if(!input.child("stl").empty()){
		xml_node visual = link.append_child("visual");
		std::string filename = "package://keti_workcell_description/meshes/";
		filename.append(input.child("stl").text().as_string());
		filename.append(".STL");
		visual.append_child("geometry").append_child("mesh").append_attribute("filename").set_value(filename.c_str());
		visual.append_child("material").append_child("color").append_attribute("rgba").set_value(input.child_value("color"));
		visual.last_child().append_attribute("name").set_value("");
	}
}

void MainWindow::make_joint_node(xml_node joint, xml_node input){
//	xml_node joint = robot.append_child("joint");
	joint.append_attribute("name").set_value(input.child_value("name"));
	joint.append_attribute("type").set_value(input.child_value("type"));
	xml_node origin = joint.append_child("origin");
	xml_node origin_input = input.child("origin");
	origin.append_attribute("xyz").set_value(origin_input.child_value("xyz"));
	origin.append_attribute("rpy").set_value(origin_input.child_value("rpy"));
	joint.append_child("parent").append_attribute("link").set_value(input.child_value("parent"));
	joint.append_child("child").append_attribute("link").set_value(input.child_value("child"));

	if(input.child("axis")){
		joint.append_child("axis").append_attribute("xyz").set_value(input.child_value("axis"));
		xml_node limit = joint.append_child("limit");
		xml_node limit_input = input.child("limit");
		limit.append_attribute("lower").set_value(limit_input.child_value("lower"));
		limit.append_attribute("upper").set_value(limit_input.child_value("upper"));
		limit.append_attribute("effort").set_value("100");
		limit.append_attribute("velocity").set_value("10");
	}
}
