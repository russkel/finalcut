/***********************************************************************
* windows.cpp - Shows window handling                                  *
*                                                                      *
* This file is part of the Final Cut widget toolkit                    *
*                                                                      *
* Copyright 2016-2018 Markus Gans                                      *
*                                                                      *
* The Final Cut is free software; you can redistribute it and/or       *
* modify it under the terms of the GNU Lesser General Public License   *
* as published by the Free Software Foundation; either version 3 of    *
* the License, or (at your option) any later version.                  *
*                                                                      *
* The Final Cut is distributed in the hope that it will be useful,     *
* but WITHOUT ANY WARRANTY; without even the implied warranty of       *
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the        *
* GNU Lesser General Public License for more details.                  *
*                                                                      *
* You should have received a copy of the GNU Lesser General Public     *
* License along with this program.  If not, see                        *
* <http://www.gnu.org/licenses/>.                                      *
***********************************************************************/

#include <vector>
#include <final/final.h>


//----------------------------------------------------------------------
// class SmallWindow
//----------------------------------------------------------------------

#pragma pack(push)
#pragma pack(1)

class SmallWindow : public finalcut::FDialog
{
  public:
    // Constructor
    explicit SmallWindow (finalcut::FWidget* = 0);

    // Destructor
    ~SmallWindow();

  private:
    // Disable copy constructor
    SmallWindow (const SmallWindow&);

    // Disable assignment operator (=)
    SmallWindow& operator = (const SmallWindow&);

    // Method
    void adjustSize();

    // Event handlers
    void onShow (finalcut::FShowEvent*);
    void onTimer (finalcut::FTimerEvent*);

    // Data Members
    finalcut::FLabel* left_arrow;
    finalcut::FLabel* right_arrow;
    finalcut::FLabel* top_left_label;
    finalcut::FLabel* top_right_label;
    finalcut::FLabel* bottom_label;
};
#pragma pack(pop)

//----------------------------------------------------------------------
SmallWindow::SmallWindow (finalcut::FWidget* parent)
  : finalcut::FDialog(parent)
  , left_arrow()
  , right_arrow()
  , top_left_label()
  , top_right_label()
  , bottom_label()
{
  wchar_t arrow_up, arrow_down;

  arrow_up = finalcut::fc::BlackUpPointingTriangle;
  arrow_down = finalcut::fc::BlackDownPointingTriangle;

  left_arrow = new finalcut::FLabel (arrow_up, this);
  left_arrow->setForegroundColor (wc.label_inactive_fg);
  left_arrow->setEmphasis();
  left_arrow->ignorePadding();
  left_arrow->setGeometry (2, 2, 1, 1);

  right_arrow = new finalcut::FLabel (arrow_up, this);
  right_arrow->setForegroundColor (wc.label_inactive_fg);
  right_arrow->setEmphasis();
  right_arrow->ignorePadding();
  right_arrow->setGeometry (getWidth() - 1, 2, 1, 1);

  const finalcut::FString& top_left_label_text = "menu";
  top_left_label = new finalcut::FLabel (top_left_label_text, this);
  top_left_label->setForegroundColor (wc.label_inactive_fg);
  top_left_label->setEmphasis();
  top_left_label->setGeometry (1, 1, 6, 1);

  const finalcut::FString& top_right_label_text = "zoom";
  top_right_label = new finalcut::FLabel (top_right_label_text, this);
  top_right_label->setAlignment (finalcut::fc::alignRight);
  top_right_label->setForegroundColor (wc.label_inactive_fg);
  top_right_label->setEmphasis();
  top_right_label->setGeometry (getClientWidth() - 5, 1, 6, 1);

  finalcut::FString bottom_label_text = "resize\n"
                                        "corner\n";
  bottom_label_text += arrow_down;
  bottom_label = new finalcut::FLabel (bottom_label_text, this);
  bottom_label->setAlignment (finalcut::fc::alignRight);
  bottom_label->setForegroundColor (wc.label_inactive_fg);
  bottom_label->setEmphasis();
  bottom_label->setGeometry (13, 3, 6, 3);
}

//----------------------------------------------------------------------
SmallWindow::~SmallWindow()
{
  // Remove own timer
  delOwnTimer();
}

//----------------------------------------------------------------------
void SmallWindow::adjustSize()
{
  if ( isZoomed() )
  {
    *top_right_label = "unzoom";
    bottom_label->hide();
  }
  else
  {
    *top_right_label = "zoom";
    bottom_label->setVisible();
  }

  finalcut::FDialog::adjustSize();
  right_arrow->setGeometry (getWidth() - 1, 2, 1, 1);
  top_right_label->setGeometry (getClientWidth() - 5, 1, 6, 1);
  bottom_label->setGeometry (1, getClientHeight() - 2, getClientWidth(), 3);
}

//----------------------------------------------------------------------
void SmallWindow::onShow (finalcut::FShowEvent*)
{
  addTimer(1000);
}

//----------------------------------------------------------------------
void SmallWindow::onTimer (finalcut::FTimerEvent*)
{
  left_arrow->unsetEmphasis();
  left_arrow->redraw();
  right_arrow->unsetEmphasis();
  right_arrow->redraw();
  top_left_label->unsetEmphasis();
  top_left_label->redraw();
  top_right_label->unsetEmphasis();
  top_right_label->redraw();
  bottom_label->unsetEmphasis();
  bottom_label->redraw();
  updateTerminal();
  delOwnTimer();
}


//----------------------------------------------------------------------
// class Window
//----------------------------------------------------------------------

#pragma pack(push)
#pragma pack(1)

class Window : public finalcut::FDialog
{
  public:
    // Constructor
    explicit Window (finalcut::FWidget* = 0);

    // Destructor
    ~Window();

  private:
    // Typedefs
    typedef void (Window::*WindowCallback)(finalcut::FWidget*, data_ptr);
    typedef void (finalcut::FApplication::*FAppCallback)(finalcut::FWidget*, data_ptr);
    typedef struct
    {
      bool is_open;
      finalcut::FString* title;
      SmallWindow* dgl;
    }
    win_data;

    // Disable copy constructor
    Window (const Window&);

    // Disable assignment operator (=)
    Window& operator = (const Window&);

    // Method
    void createFileMenuItems (finalcut::FMenu*);
    void createDialogButtons();
    void activateWindow (finalcut::FDialog*);
    void adjustSize();
    void addClickedCallback (finalcut::FWidget*, WindowCallback);
    void addClickedCallback (finalcut::FWidget*, FAppCallback);

    // Event handlers
    void onClose (finalcut::FCloseEvent*);

    // Callback methods
    void cb_createWindows (finalcut::FWidget*, data_ptr);
    void cb_closeWindows (finalcut::FWidget*, data_ptr);
    void cb_next (finalcut::FWidget*, data_ptr);
    void cb_previous (finalcut::FWidget*, data_ptr);
    void cb_destroyWindow (finalcut::FWidget*, data_ptr);

    // Data Members
    std::vector<win_data*> windows;
};
#pragma pack(pop)

//----------------------------------------------------------------------
Window::Window (finalcut::FWidget* parent)
  : finalcut::FDialog(parent)
  , windows()
{
  finalcut::FMenu* File;
  finalcut::FDialogListMenu* DglList;
  finalcut::FString drop_down_symbol;
  finalcut::FMenuBar* Menubar;
  finalcut::FStatusBar* Statusbar;

  // Menu bar
  Menubar = new finalcut::FMenuBar (this);

  // Menu bar item
  File = new finalcut::FMenu ("&File", Menubar);
  File->setStatusbarMessage ("File management commands");

  // Dialog list menu item
  drop_down_symbol = wchar_t(finalcut::fc::BlackDownPointingTriangle);
  DglList = new finalcut::FDialogListMenu (drop_down_symbol, Menubar);
  DglList->setStatusbarMessage ("List of all the active dialogs");

  // File menu items
  createFileMenuItems (File);

  // Dialog buttons
  createDialogButtons();

  // Statusbar at the bottom
  Statusbar = new finalcut::FStatusBar (this);
  Statusbar->setMessage("Status bar message");

  // Generate data vector for the windows
  for (int n = 1; n <= 6; n++)
  {
    win_data* win_dat = new win_data;
    win_dat->is_open = false;
    win_dat->title = new finalcut::FString();
    win_dat->title->sprintf("Window %d", n);
    windows.push_back(win_dat);
  }
}

//----------------------------------------------------------------------
Window::~Window()
{
  std::vector<win_data*>::iterator iter;
  iter = windows.begin();

  while ( iter != windows.end() )
  {
    win_data* win_dat = *iter;

    // Remove all callbacks before Window::cb_destroyWindow() will be called
    if ( win_dat->is_open && win_dat->dgl )
      win_dat->dgl->delCallbacks();

    delete win_dat->title;
    delete win_dat;
    iter = windows.erase(iter);
  }
}

//----------------------------------------------------------------------
void Window::createFileMenuItems (finalcut::FMenu* File)
{
  // "File" menu item
  finalcut::FMenuItem* New = new finalcut::FMenuItem ("&New", File);
  New->setStatusbarMessage ("Create the windows");

  finalcut::FMenuItem* Close = new finalcut::FMenuItem ("&Close", File);
  Close->setStatusbarMessage ("Close the windows");

  finalcut::FMenuItem* Line1 = new finalcut::FMenuItem (File);
  Line1->setSeparator();

  finalcut::FMenuItem* Next = new finalcut::FMenuItem ("Ne&xt window", File);
  Next->addAccelerator (finalcut::fc::Fmkey_npage);  // Meta/Alt + PgDn
  Next->setStatusbarMessage ("Switch to the next window");

  finalcut::FMenuItem* Previous = new finalcut::FMenuItem ("&Previous window", File);
  Previous->addAccelerator (finalcut::fc::Fmkey_ppage);  // Meta/Alt + PgUp
  Previous->setStatusbarMessage ("Switch to the previous window");

  finalcut::FMenuItem* Line2 = new finalcut::FMenuItem (File);
  Line2->setSeparator();

  finalcut::FMenuItem* Quit = new finalcut::FMenuItem ("&Quit", File);
  Quit->addAccelerator (finalcut::fc::Fmkey_x);  // Meta/Alt + X
  Quit->setStatusbarMessage ("Exit the program");

  // Add menu item callback
  addClickedCallback (New, &Window::cb_createWindows);
  addClickedCallback (Close, &Window::cb_closeWindows);
  addClickedCallback (Next, &Window::cb_next);
  addClickedCallback (Previous, &Window::cb_previous);
  addClickedCallback (Quit, &finalcut::FApplication::cb_exitApp);
}

//----------------------------------------------------------------------
void Window::createDialogButtons()
{
  // Dialog buttons
  finalcut::FButton* CreateButton = new finalcut::FButton (this);
  CreateButton->setGeometry(2, 2, 9, 1);
  CreateButton->setText (L"&Create");

  finalcut::FButton* CloseButton = new finalcut::FButton (this);
  CloseButton->setGeometry(15, 2, 9, 1);
  CloseButton->setText (L"C&lose");

  finalcut::FButton* QuitButton = new finalcut::FButton (this);
  QuitButton->setGeometry(28, 2, 9, 1);
  QuitButton->setText (L"&Quit");

  // Add button callback
  addClickedCallback (CreateButton, &Window::cb_createWindows);
  addClickedCallback (CloseButton, &Window::cb_closeWindows);
  addClickedCallback (QuitButton, &finalcut::FApplication::cb_exitApp);
}

//----------------------------------------------------------------------
void Window::activateWindow (finalcut::FDialog* win)
{
  if ( ! win || win->isWindowHidden() || win->isWindowActive() )
    return;

  bool has_raised = finalcut::FWindow::raiseWindow(win);
  win->activateDialog();

  if ( has_raised )
    win->redraw();

  updateTerminal();
}

//----------------------------------------------------------------------
void Window::adjustSize()
{
  std::vector<win_data*>::const_iterator iter, first;
  int w  = getRootWidget()->getWidth()
    , h  = getRootWidget()->getHeight()
    , X  = int(1 + (w - 40) / 2)
    , Y  = int(1 + (h - 22) / 2)
    , dx = ( w > 80 ) ? (w - 80) / 2 : 0
    , dy = ( h > 24 ) ? (h - 24) / 2 : 0;

  if ( Y < 2 )
    Y = 2;

  setPos (X, Y);
  iter = first = windows.begin();

  while ( iter != windows.end() )
  {
    if ( (*iter)->is_open )
    {
      int n = int(std::distance(first, iter))
        , x = dx + 5 + (n % 3) * 25 + int(n / 3) * 3
        , y = dy + 11 + int(n / 3) * 3;
      (*iter)->dgl->setPos (x, y);
    }

    ++iter;
  }

  finalcut::FDialog::adjustSize();
}

//----------------------------------------------------------------------
void Window::addClickedCallback ( finalcut::FWidget* widget
                                , WindowCallback call )
{
  FMemberCallback callback
      = reinterpret_cast<finalcut::FWidget::FMemberCallback>(call);

  widget->addCallback
  (
    "clicked",
    F_METHOD_CALLBACK (this, callback)
  );
}

//----------------------------------------------------------------------
void Window::addClickedCallback ( finalcut::FWidget* widget
                                , FAppCallback call )
{
  FMemberCallback callback
      = reinterpret_cast<finalcut::FWidget::FMemberCallback>(call);

  widget->addCallback
  (
    "clicked",
    F_METHOD_CALLBACK (this, callback)
  );
}

//----------------------------------------------------------------------
void Window::onClose (finalcut::FCloseEvent* ev)
{
  finalcut::FApplication::closeConfirmationDialog (this, ev);
}

//----------------------------------------------------------------------
void Window::cb_createWindows (finalcut::FWidget*, data_ptr)
{
  std::vector<win_data*>::const_iterator iter, first;
  iter = first = windows.begin();
  int w = getRootWidget()->getWidth()
    , h = getRootWidget()->getHeight()
    , dx = ( w > 80 ) ? (w - 80) / 2 : 0
    , dy = ( h > 24 ) ? (h - 24) / 2 : 0;

  while ( iter != windows.end() )
  {
    if ( ! (*iter)->is_open )
    {
      win_data* win_dat = *iter;
      SmallWindow* win = new SmallWindow(this);
      win_dat->dgl = win;
      win_dat->is_open = true;
      win->setText(*(win_dat)->title);
      int n = int(std::distance(first, iter))
        , x = dx + 5 + (n % 3) * 25 + int(n / 3) * 3
        , y = dy + 11 + int(n / 3) * 3;
      win->setGeometry (x, y, 20, 8);
      win->setMinimumSize (20, 8);
      win->setResizeable();
      win->show();

      win->addCallback
      (
        "destroy",
        F_METHOD_CALLBACK (this, &Window::cb_destroyWindow),
        static_cast<finalcut::FWidget::data_ptr>(win_dat)
      );
    }

    ++iter;
  }

  activateWindow(this);
}

//----------------------------------------------------------------------
void Window::cb_closeWindows (finalcut::FWidget*, data_ptr)
{
  if ( ! dialog_list || dialog_list->empty() )
    return;

  finalcut::FWidget::widgetList::const_iterator iter, first;
  iter = dialog_list->end();
  first = dialog_list->begin();
  activateWindow(this);

  do
  {
    --iter;

    if ( (*iter) != this )
      (*iter)->close();
  }
  while ( iter != first );
}

//----------------------------------------------------------------------
void Window::cb_next (finalcut::FWidget*, data_ptr)
{
  if ( ! dialog_list || dialog_list->empty() )
    return;

  finalcut::FWidget::widgetList::const_iterator iter;
  iter = dialog_list->begin();

  while ( iter != dialog_list->end() )
  {
    if ( static_cast<finalcut::FWindow*>(*iter)->isWindowActive() )
    {
      finalcut::FDialog* next;
      finalcut::FWidget::widgetList::const_iterator next_element;
      next_element = iter;

      do
      {
        ++next_element;

        if ( next_element == dialog_list->end() )
          next_element = dialog_list->begin();

        next = static_cast<finalcut::FDialog*>(*next_element);
      } while ( ! next->isEnabled()
             || ! next->acceptFocus()
             || ! next->isVisible()
             || ! next->isWindowWidget() );

      activateWindow(next);
      break;
    }

    ++iter;
  }
}

//----------------------------------------------------------------------
void Window::cb_previous (finalcut::FWidget*, data_ptr)
{
  if ( ! dialog_list || dialog_list->empty() )
    return;

  finalcut::FWidget::widgetList::const_iterator iter;
  iter = dialog_list->end();

  do
  {
    --iter;

    if ( (*iter)->isDialogWidget()
      && static_cast<finalcut::FWindow*>(*iter)->isWindowActive() )
    {
      finalcut::FDialog* prev;
      finalcut::FWidget::widgetList::const_iterator prev_element;
      prev_element = iter;

      do
      {
        if ( prev_element == dialog_list->begin() )
          prev_element = dialog_list->end();

        --prev_element;
        prev = static_cast<finalcut::FDialog*>(*prev_element);
      } while ( ! prev->isEnabled()
             || ! prev->acceptFocus()
             || ! prev->isVisible()
             || ! prev->isWindowWidget() );

      activateWindow(prev);
      break;
    }
  }
  while ( iter != dialog_list->begin() );
}

//----------------------------------------------------------------------
void Window::cb_destroyWindow (finalcut::FWidget*, data_ptr data)
{
  win_data* win_dat = static_cast<win_data*>(data);

  if ( win_dat )
  {
    win_dat->is_open = false;
    win_dat->dgl = 0;
  }
}


//----------------------------------------------------------------------
//                               main part
//----------------------------------------------------------------------

int main (int argc, char* argv[])
{
  // Create the application object
  finalcut::FApplication app (argc, argv);

  // Create main dialog object
  Window main_dlg (&app);
  main_dlg.setText ("Main window");
  main_dlg.setGeometry (int(1 + (app.getWidth() - 40) / 2), 2, 40, 6);

  // Set dialog main_dlg as main widget
  app.setMainWidget (&main_dlg);

  // Show and start the application
  main_dlg.show();
  return app.exec();
}
