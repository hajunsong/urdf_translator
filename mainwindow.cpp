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

		try {
			file<> xmlFile(loadFileName.toStdString().c_str());
			doc_input.parse<0>(xmlFile.data());

//			std::string load_urdf_string;
//			rapidxml::print(std::back_inserter(load_urdf_string), doc);

			make_urdf();

		}  catch (rapidxml::parse_error e) {
			ui->txtPreview->append(e.what());
		}


//		if(xmlFile.size() <= 0){
//			std::string result_desc = result.description();
//			std::cout << result_desc << std::endl;
//			ui->txtPreview->append(result_desc.c_str());

//		}
//		else{
//			load_urdf();
//			make_urdf();
//			prev_urdf();
//		}
	}
}

void MainWindow::btnSaveClicked(){
	qDebug() << loadFileName;
	QStringList fileNameSplit = loadFileName.split("/");
	int indx1 = loadFileName.indexOf(".");
	int indx2 = loadFileName.lastIndexOf("/");
	QString saveFileName = loadFileName.remove(indx2 + 1, indx1-1-indx2 + 4) + ui->txtName->text() + ".urdf";
	qDebug() << saveFileName;

//	doc_urdf.save_file("/home/keti/ros_ws/src/keti_workcell_simulator/keti_workcell_description/urdf/base_ETC_1.urdf");
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
	xml_node<> *cell = doc_input.first_node("cell");

	xml_node<> *dec = doc_urdf.allocate_node(node_declaration);
	dec->append_attribute(doc_urdf.allocate_attribute("version", "1.0"));
	dec->append_attribute(doc_urdf.allocate_attribute("encoding", "UTF-8"));
	doc_urdf.append_node(dec);

	xml_node<> *robot = doc_urdf.allocate_node(node_element, "robot");
	robot->append_attribute(doc_urdf.allocate_attribute("name", cell->first_node("name")->value()));

	robot->append_node(doc_urdf.allocate_node(node_element, "link"));
	robot->first_node()->append_attribute(doc_urdf.allocate_attribute("name", cell->first_node("root")->value()));

	xml_node<> *base = cell->first_node("base");
	make_link_node(robot, base);

	xml_node<> *object = cell->first_node("object");
	std::cout << object->document()->name() << std::endl;
	for (xml_node<> *link = object->first_node("link"); link; link = object->next_sibling("link"))
	{
		std::cout << "=============================" << std::endl;
		make_link_node(robot, link);
	}

	doc_urdf.append_node(robot);

	std::cout << doc_urdf << std::endl;
}

//void MainWindow::make_link_node(xml_node robot, std::string name, std::string filename, std::string color){
void MainWindow::make_link_node(xml_node<> *robot, xml_node<> *input){
	xml_document<> doc;

	xml_node<> *link = doc.allocate_node(node_element, "link");
	link->append_attribute(doc.allocate_attribute("name", input->first_node("name")->value()));
	xml_node<> *visual = doc.allocate_node(node_element, "visual");
	visual->append_node(doc.allocate_node(node_element, "geometry"));
	visual->append_node(doc.allocate_node(node_element, "material"));

	xml_node<> *mesh = doc.allocate_node(node_element, "mesh");
	char filename[255];
	sprintf(filename, "%s%s%s", "package://keti_workcell_description/meshes/", input->first_node("stl")->value(), ".STL");
	mesh->append_attribute(doc.allocate_attribute("filename", filename));
	visual->first_node()->append_node(mesh);

	xml_node<> *color = doc.allocate_node(node_element, "color");
	color->append_attribute(doc.allocate_attribute("rgba", input->first_node("color")->value()));
	visual->last_node()->append_attribute(doc.allocate_attribute("name", ""));
	visual->last_node()->append_node(color);

	link->append_node(visual);

	robot->append_node(link);
}

//void MainWindow::make_urdf(){
//	xml_node decl = doc_urdf.prepend_child(pugi::node_declaration);
//	decl.append_attribute("version") = "1.0";
//	decl.append_attribute("encoding") = "UTF-8";

//	xml_node cell = doc_input.child("cell");
//	xml_node robot = doc_urdf.append_child("robot");
//	robot.append_attribute("name").set_value(cell.child("name").text().as_string());

//	robot.append_child("link").append_attribute("name").set_value(cell.child("root").text().as_string());

//	xml_node base = cell.child("base");
//	make_link_node(robot, base.child("name").text().as_string(), base.child("stl").text().as_string(), base.child("color").text().as_string());

//	xml_node obj = cell.child("object");
//	std::string name, stl, color;
//	for(xml_node link : obj.children("link")){
//		name = link.child("name").text().as_string();
//		stl = link.child("stl").text().as_string();
//		color = link.child("color").text().as_string();
//		make_link_node(robot, name, stl, color);
//	}

//	xml_node tf = cell.child("TF");
//	std::string type, xyz, rpy, par, chi, axis;
////		std::string effort, velocity;
//	double lower, upper;
//	xml_node limit;
//	for(xml_node joint : tf.children("joint")){
//		name = joint.child("name").text().as_string();
//		type = joint.child("type").text().as_string();
//		xyz = joint.child("origin").child("xyz").text().as_string();
//		rpy = joint.child("origin").child("rpy").text().as_string();
//		par = joint.child("parent").text().as_string();
//		chi = joint.child("child").text().as_string();

//		if(type == "fixed"){
//			make_joint_node(robot, name, par, chi, type, xyz, rpy);
//		}
//		else{
//			axis = joint.child("axis").text().as_string();
//			limit = joint.child("limit");
//			lower = limit.child("lower").text().as_double();
//			upper = limit.child("upper").text().as_double();
////				effort = limit.child("effort").text().as_string();
////				velocity = limit.child("velocity").text().as_string();

//			make_joint_node(robot, name, par, chi, type, xyz, rpy, axis, lower, upper);
//		}
//	}
//}

//void MainWindow::make_link_node(xml_node robot, std::string name, std::string filename, std::string color){
//	xml_node link = robot.append_copy(robot.child("link"));
//	link.attribute("name").set_value(name.c_str());
//	xml_node visual = link.append_child("visual");
//	visual.append_child("geometry").append_child("mesh").append_attribute("filename").set_value(("package://keti_workcell_description/meshes/" + filename + ".STL").c_str());
//	visual.append_child("material").append_child("color").append_attribute("rgba").set_value(color.c_str());
//	visual.child("material").append_attribute("name").set_value("");
//}

//void MainWindow::make_joint_node(xml_node robot, std::string name, std::string link1, std::string link2, std::string type, std::string xyz, std::string rpy, std::string axis, double lower, double upper){
//	xml_node joint = robot.append_child("joint");
//	joint.append_attribute("name") = name.c_str();
//	joint.append_attribute("type") = type.c_str();
//	joint.append_child("origin").append_attribute("xyz").set_value(xyz.c_str());
//	joint.child("origin").append_attribute("rpy").set_value(rpy.c_str());
//	joint.append_child("parent").append_attribute("link").set_value(link1.c_str());
//	joint.append_child("child").append_attribute("link").set_value(link2.c_str());
//	joint.append_child("axis").append_attribute("xyz").set_value(axis.c_str());
//	joint.append_child("limit").append_attribute("lower").set_value(std::to_string(lower).c_str());
//	joint.child("limit").append_attribute("upper").set_value(std::to_string(upper).c_str());
//	joint.child("limit").append_attribute("effort").set_value("100");
//	joint.child("limit").append_attribute("velocity").set_value("10");
//}
//void MainWindow::make_joint_node(xml_node robot, std::string name, std::string link1, std::string link2, std::string type, std::string xyz, std::string rpy){
//	xml_node joint = robot.append_child("joint");
//	joint.append_attribute("name") = name.c_str();
//	joint.append_attribute("type") = type.c_str();
//	joint.append_child("origin").append_attribute("xyz").set_value(xyz.c_str());
//	joint.child("origin").append_attribute("rpy").set_value(rpy.c_str());
//	joint.append_child("parent").append_attribute("link").set_value(link1.c_str());
//	joint.append_child("child").append_attribute("link").set_value(link2.c_str());
//}
