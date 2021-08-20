#include <gtkmm.h>
#include <iostream>
#include <filesystem>
#include <random>
#include <cstdio>

//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//                                                                                           Window class
//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
class MyWindow : public Gtk::Window {
public:
  MyWindow();
  virtual ~MyWindow();

protected:
  //function prototypes
  void resetApp();
  void on_button_remove_clicked();
  void on_button_randomize_clicked();
  void on_button_browse_clicked();
  void on_button_openSelDir_clicked();
  void on_button_removeRusSongs_clicked();
  void on_button_refresh_clicked();
  void on_folder_dialog_response(int response_id, Glib::RefPtr<Gtk::FileChooserNative>& dialog);
  void refreshFileList(std::string dialog); 

  //List of window elements
  Gtk::Grid m_window_grid;
  Gtk::Label m_path_label, m_filesList_label, m_randomize_label;
  Gtk::Button m_button_browse, m_button_openSelectedDirectory;
  Gtk::TreeView m_filesList;
  Glib::RefPtr<Gtk::ListStore> m_refTreeModel;
  Gtk::Button m_button_refresh, m_button_removeRusSongs, m_button_randomize;
  Gtk::SpinButton m_spinbutton_randomize;
  Gtk::Separator m_Separator;
  Gtk::Button m_button_remove;
  Gtk::ScrolledWindow m_ScrollableWindow;

  //Class for treeview
  class ModelColumns : public Gtk::TreeModel::ColumnRecord {
  public:
  ModelColumns() {add(m_fileNameColumn);}

  Gtk::TreeModelColumn<Glib::ustring> m_fileNameColumn;};

  //Variables
  std::string rememberPath;
  ModelColumns m_Columns;

};

//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//                                                                                        Window constructor
//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
MyWindow::MyWindow() : 
                       m_path_label(),
                       m_filesList_label ("Files in selected folder:"),
                       m_randomize_label ("Digits in random number:"),
                       m_button_browse("Browse"), 
                       m_button_openSelectedDirectory("Open selected folder"), 
                       m_filesList (),
                       m_button_refresh("Refresh list"),
                       m_button_removeRusSongs("Remove RU songs"),
                       m_button_randomize("Add"),
                       m_spinbutton_randomize (Gtk::Adjustment::create(4, 1, 7, 1, 1, 0)),  
                       m_button_remove("Remove")


{
  set_title("File Order Randomizer");

// Elements margins:
  m_window_grid.set_margin(10);
  m_path_label.set_margin_top(10);    //left
  m_path_label.set_margin_start(10);
  m_path_label.set_margin_end(10);    //right

  m_button_browse.set_margin(10);
  m_button_openSelectedDirectory.set_margin(10);

  m_filesList_label.set_margin_start(10);
  m_filesList_label.set_margin_top(10);
  m_filesList_label.set_margin_end(10);

  m_ScrollableWindow.set_margin(10);

  m_button_refresh.set_margin(10);
  m_button_removeRusSongs.set_margin(10);

  m_randomize_label.set_margin(10);
  m_spinbutton_randomize.set_margin(10);

  m_button_randomize.set_margin(10);
  m_button_remove.set_margin(10);

//list of files names forming
  m_ScrollableWindow.set_child(m_filesList);
  m_ScrollableWindow.set_policy(Gtk::PolicyType::AUTOMATIC, Gtk::PolicyType::AUTOMATIC); //show scroll bar only when scrolling
  m_ScrollableWindow.set_expand(); //make element expandable -> makes all elements in grid expandable
  m_ScrollableWindow.set_min_content_height (205);
  m_refTreeModel = Gtk::ListStore::create(m_Columns);
  m_filesList.set_model(m_refTreeModel);
  m_filesList.set_headers_visible(false); //disables columns titles
  m_filesList.append_column("Files names:", m_Columns.m_fileNameColumn);

// connecting functions to buttons
  m_button_browse.signal_clicked().connect(sigc::mem_fun(*this, &MyWindow::on_button_browse_clicked));
  m_button_openSelectedDirectory.signal_clicked().connect(sigc::mem_fun(*this, &MyWindow::on_button_openSelDir_clicked));
  m_button_refresh.signal_clicked().connect(sigc::mem_fun(*this, &MyWindow::on_button_refresh_clicked));
  m_button_removeRusSongs.signal_clicked().connect(sigc::mem_fun(*this, &MyWindow::on_button_removeRusSongs_clicked));
  m_button_randomize.signal_clicked().connect(sigc::mem_fun(*this, &MyWindow::on_button_randomize_clicked));
  m_button_remove.signal_clicked().connect(sigc::mem_fun(*this, &MyWindow::on_button_remove_clicked));

// Adding elements into window main grid
  m_window_grid.attach(m_path_label, 0, 0, 2);

  m_window_grid.attach(m_button_browse, 0, 1);
  m_window_grid.attach(m_button_openSelectedDirectory, 1, 1);
  
  m_window_grid.attach(m_filesList_label, 0, 2);

  m_window_grid.attach(m_ScrollableWindow, 0, 3, 2);

  m_window_grid.attach(m_button_refresh, 0, 4);
  m_window_grid.attach(m_button_removeRusSongs, 1, 4);

  m_window_grid.attach(m_randomize_label, 0, 5);
  m_window_grid.attach(m_spinbutton_randomize, 1, 5);

  m_window_grid.attach(m_button_randomize, 0, 6);
  m_window_grid.attach(m_button_remove, 1, 6);

// Connecting grid to the window
  set_child(m_window_grid);

  resetApp();
  on_folderNotExists_error();
}

MyWindow::~MyWindow()
{
}

//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//                                                                                              Functions
//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//                                                                                        <<<Refresh file list>>>

void MyWindow::refreshFileList(std::string dialog){
  rememberPath = dialog;
  m_refTreeModel->clear();
  for (const auto & entry : std::filesystem::directory_iterator(dialog)){
  std::string fileName = entry.path();
  int index = fileName.find_last_of("/\\");
  fileName = fileName.substr(index+1);

  if (fileName != ".DS_Store" && fileName != ".localized" && fileName != "$RECYCLE.BIN" && 
      fileName != ".fseventsd" && fileName != ".Spotlight-V100" && fileName != "System Volume Information"){ //ignoring macOS junk
  auto row = *(m_refTreeModel->append());
  row[m_Columns.m_fileNameColumn] = static_cast<Glib::ustring>(fileName);}}

  m_refTreeModel->set_sort_column( m_Columns.m_fileNameColumn, Gtk::SortType::ASCENDING);
}

//                                                                                        <<<Reset app window>>>
void MyWindow::resetApp()
{
  m_refTreeModel->clear();
  m_path_label.set_text("Press browse button to select a folder");
  m_button_openSelectedDirectory.set_sensitive(false);
  m_button_refresh.set_sensitive(false);
  m_button_removeRusSongs.set_sensitive(false);
  m_button_randomize.set_sensitive(false);
  m_button_remove.set_sensitive(false);
}

//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//                                                                                          Buttons functions
//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//                                                                                        <<<Button - Browse>>>
void MyWindow::on_button_browse_clicked(){
    auto dialog = Gtk::FileChooserNative::create("Please choose a folder", *this, Gtk::FileChooser::Action::SELECT_FOLDER, "Choose", "Cancel");
    dialog->set_transient_for(*this);
    dialog->set_modal(true);
    dialog->signal_response().connect(sigc::bind(sigc::mem_fun(*this, &MyWindow::on_folder_dialog_response), dialog));

    dialog->show();
}

  void MyWindow::on_folder_dialog_response(int response_id, Glib::RefPtr<Gtk::FileChooserNative>& dialog){
  switch (response_id) {
    case Gtk::ResponseType::ACCEPT: {
      m_path_label.set_label(dialog->get_file()->get_path());
      refreshFileList(dialog->get_file()->get_path());
      m_button_openSelectedDirectory.set_sensitive(true);
      m_button_refresh.set_sensitive(true);
      m_button_removeRusSongs.set_sensitive(true);
      m_button_randomize.set_sensitive(true);
      m_button_remove.set_sensitive(true);
      break;}
    case Gtk::ResponseType::CANCEL: {break;}
    default: {std::cout << "Unexpected button clicked: " << response_id << std::endl; break;}}
}

//                                                                              <<<Button - Open delected folder>>>
void MyWindow::on_button_openSelDir_clicked(){
  if(!std::filesystem::exists(rememberPath.c_str())){resetApp(); m_path_label.set_text("ERROR: Folder doesn't exist anymore (╯°□°)╯┻━┻");} //checking if path still exists
  std::string command = "open \"" + rememberPath + "\"";
  //std::string command = "nemo \"" + rememberPath + "\""; //Command for Linux mint.
  const char * char_command = command.c_str();
  std::system(char_command);
}

//                                                                                   <<<Button - Refresh list>>>
void MyWindow::on_button_refresh_clicked(){
  if(!std::filesystem::exists(rememberPath.c_str())){resetApp(); m_path_label.set_text("ERROR: Folder doesn't exist anymore (╯°□°)╯┻━┻");}
  refreshFileList(rememberPath);
}

//                                                                                   <<<Button - Remove RU songs>>
void MyWindow::on_button_removeRusSongs_clicked(){
  if(!std::filesystem::exists(rememberPath.c_str())){resetApp(); m_path_label.set_text("ERROR: Folder doesn't exist anymore (╯°□°)╯┻━┻");}
  std::string triggers[] = {"а", "А", "е", "Е", "ё", "Ё", "и", "И", "о", "О", "у", "У", "э", "Э", "ю", "Ю", "я", "Я", "MORGENSHTERN", "V $ X V Prince", "GAYAZOV$ BROTHER$", "ONUKA", 
  "Slava Marlow", "Niletto", "Zivert", "Johnyboy", "Nebezao", "ALEXEMELYA", "Jah Khalib", "BOOSIN", "SHLAKOBLOCHINA"};
  int triggersLenght = sizeof(triggers)/sizeof(triggers[0]);
  std::string username = getlogin();

  for (auto row: m_refTreeModel->children()){
    for (int i = 0; i<triggersLenght; ++i){
    Glib::ustring fileName = row[m_Columns.m_fileNameColumn];
    if (static_cast<std::string>(fileName).find(triggers[i]) != std::string::npos){
      /*std::string oldName = rememberPath + "/" + static_cast<std::string>(fileName); //Deletes files without moving them to trash
      const char * char_oldName = oldName.c_str();
      if (remove(char_oldName) != 0) {perror("Couldn't delete file (╯°□°)╯┻━┻");}*/

      std::string oldName = rememberPath + "/" + static_cast<std::string>(fileName);
      std::string newName = "/Users/" + username + "/.Trash/" + static_cast<std::string>(fileName);
      //std::string newName = "/home/" + username + ".local/share/Trash/files/" + static_cast<std::string>(fileName); //newName for Linux mint.

      const char * char_oldName = oldName.c_str();
      const char * char_newName = newName.c_str();

      int result = rename(char_oldName, char_newName );
      if (result != 0){perror("Couldn't rename file (╯°□°)╯┻━┻");}

      break;}}}

    refreshFileList(rememberPath);
}

//                                                                                 <<<Button - Randomize>>>
void MyWindow::on_button_randomize_clicked(){
  if(!std::filesystem::exists(rememberPath.c_str())){resetApp(); m_path_label.set_text("ERROR: Folder doesn't exist anymore (╯°□°)╯┻━┻");}
  refreshFileList(rememberPath);
  int i = m_spinbutton_randomize.get_value(); //Value can be from 1 to 7

  int randomNumberLenght = 1;
  switch(i){
  case 1: {randomNumberLenght = 10; break;}
  case 2: {randomNumberLenght = 100; break;}
  case 3: {randomNumberLenght = 1000; break;}
  case 4: {randomNumberLenght = 10000; break;}
  case 5: {randomNumberLenght = 100000; break;}
  case 6: {randomNumberLenght = 1000000; break;}
  case 7: {randomNumberLenght = 10000000; break;}}

  std::random_device rd; 
  std::mt19937 mersenne(rd());
  int randomNumber;
  int digitsInRandomNumber;

  for (auto row: m_refTreeModel->children()){
  do {randomNumber = mersenne()%randomNumberLenght;
  digitsInRandomNumber = std::to_string(randomNumber).size();} //check number of digits. Sometimes it outputs number with 3 digits
  while (digitsInRandomNumber < m_spinbutton_randomize.get_value());

  Glib::ustring oldFileName = row[m_Columns.m_fileNameColumn];
  std::string oldName = rememberPath + "/" + static_cast<std::string>(oldFileName);
  std::string newName = rememberPath + "/" + std::to_string(randomNumber) + "_" + static_cast<std::string>(oldFileName);

  const char * char_oldName = oldName.c_str();
  const char * char_newName = newName.c_str();

  int result = rename(char_oldName, char_newName );
  if (result != 0){
    perror("Couldn't rename file (╯°□°)╯┻━┻");}}

  refreshFileList(rememberPath);
}

//                                                                                    <<<Button - Remove randomization>>
void MyWindow::on_button_remove_clicked(){
  if(!std::filesystem::exists(rememberPath.c_str())){resetApp(); m_path_label.set_text("ERROR: Folder doesn't exist anymore (╯°□°)╯┻━┻");}
  refreshFileList(rememberPath);
  int i = m_spinbutton_randomize.get_value(); //Value can be from 1 to 7

  for (auto row: m_refTreeModel->children()){
  Glib::ustring oldFileName = row[m_Columns.m_fileNameColumn];
  std::string oldName = rememberPath + "/" + static_cast<std::string>(oldFileName);
  std::string newName = rememberPath + "/" + static_cast<std::string>(oldFileName).erase(0,i+1);

  const char * char_oldName = oldName.c_str();
  const char * char_newName = newName.c_str();

  int result = rename(char_oldName, char_newName );
  if (result != 0){
    perror("Couldn't rename file (╯°□°)╯┻━┻");}}

  refreshFileList(rememberPath);
}

int main(int argc, char* argv[]){
  auto app = Gtk::Application::create("org.gtkmm.examples.base");

  return app->make_window_and_run<MyWindow>(argc, argv);
}
