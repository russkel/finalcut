/***********************************************************************
* flistview.cpp - Widget FListView and FListViewItem                   *
*                                                                      *
* This file is part of the Final Cut widget toolkit                    *
*                                                                      *
* Copyright 2017 Markus Gans                                           *
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

#include "final/fapplication.h"
#include "final/flistview.h"
#include "final/fscrollbar.h"
#include "final/fstatusbar.h"
#include "final/ftermbuffer.h"

// Global null FObject iterator
static FObject::FObjectIterator null_iter;

//----------------------------------------------------------------------
// class FListViewItem
//----------------------------------------------------------------------

// constructor and destructor
//----------------------------------------------------------------------
FListViewItem::FListViewItem (const FListViewItem& item)
  : FObject(item.getParent())
  , column_list(item.column_list)
  , data_pointer(item.data_pointer)
  , visible_lines(1)
  , expandable(false)
  , is_expand(false)
{
  FObject* parent = getParent();

  if ( ! parent )
    return;

  if ( parent->isInstanceOf("FListView") )
  {
    static_cast<FListView*>(parent)->insert (this);
  }
  else if ( parent->isInstanceOf("FListViewItem") )
  {
    static_cast<FListViewItem*>(parent)->insert (this);
  }
}

//----------------------------------------------------------------------
FListViewItem::FListViewItem (FObjectIterator parent_iter)
  : FObject((*parent_iter)->getParent())
  , column_list()
  , data_pointer(0)
  , visible_lines(1)
  , expandable(false)
  , is_expand(false)
{
  insert (this, parent_iter);
}

//----------------------------------------------------------------------
FListViewItem::FListViewItem ( const FStringList& cols
                             , FWidget::data_ptr data
                             , FObjectIterator parent_iter )
  : FObject(0)
  , column_list(cols)
  , data_pointer(data)
  , visible_lines(1)
  , expandable(false)
  , is_expand(false)
{
  if ( cols.empty() )
    return;

  replaceControlCodes();
  insert (this, parent_iter);
}

//----------------------------------------------------------------------
FListViewItem::~FListViewItem()  // destructor
{ }


// public methods of FListViewItem
//----------------------------------------------------------------------
FString FListViewItem::getText (int column) const
{
  if ( column < 1
    || column_list.empty()
    || column > int(column_list.size()) )
    return fc::emptyFString::get();

  column--;  // Convert column position to address offset (index)
  return column_list[uInt(column)];
}

//----------------------------------------------------------------------
uInt FListViewItem::getDepth() const
{
  FObject* parent = getParent();

  if ( parent && parent->isInstanceOf("FListViewItem") )
  {
    FListViewItem* parent_item = static_cast<FListViewItem*>(parent);
    return parent_item->getDepth() + 1;
  }

  return 0;
}

//----------------------------------------------------------------------
void FListViewItem::setText (int column, const FString& text)
{
  if ( column < 1
    || column_list.empty()
    || column > int(column_list.size()) )
    return;

  FObject* parent = getParent();
  column--;  // Convert column position to address offset (index)

  if ( parent && parent->isInstanceOf("FListView") )
  {
    FListView* listview = static_cast<FListView*>(parent);

    if ( ! listview->header[uInt(column)].fixed_width )
    {
      int length = int(text.getLength());

      if ( length > listview->header[uInt(column)].width )
        listview->header[uInt(column)].width = length;
    }
  }

  column_list[uInt(column)] = text;
}

//----------------------------------------------------------------------
FObject::FObjectIterator FListViewItem::insert (FListViewItem* child)
{
  // Add a FListViewItem as child element
  if ( ! child )
    return null_iter;

  return appendItem(child);
}

//----------------------------------------------------------------------
FObject::FObjectIterator FListViewItem::insert ( FListViewItem* child
                                               , FObjectIterator parent_iter )
{
  if ( parent_iter == null_iter )
    return null_iter;

  if ( *parent_iter )
  {
    if ( (*parent_iter)->isInstanceOf("FListView") )
    {
      // Add FListViewItem to a FListView parent
      FListView* parent = static_cast<FListView*>(*parent_iter);
      return parent->insert (child);
    }
    else if ( (*parent_iter)->isInstanceOf("FListViewItem") )
    {
      // Add FListViewItem to a FListViewItem parent
      FListViewItem* parent = static_cast<FListViewItem*>(*parent_iter);
      return parent->insert (child);
    }
  }

  return null_iter;
}

//----------------------------------------------------------------------
void FListViewItem::expand()
{
  if ( is_expand || ! hasChildren() )
    return;

  is_expand = true;
}

//----------------------------------------------------------------------
void FListViewItem::collapse()
{
  if ( ! is_expand )
    return;

  is_expand = false;
  visible_lines = 1;
}

// private methods of FListView
//----------------------------------------------------------------------
FObject::FObjectIterator FListViewItem::appendItem (FListViewItem* child)
{
  expandable = true;
  resetVisibleLineCounter();
  addChild (child);
  // Return iterator to child/last element
  return --FObject::end();
}

//----------------------------------------------------------------------
void FListViewItem::replaceControlCodes()
{
  // Replace the control codes characters
  FStringList::iterator iter = column_list.begin();

  while ( iter != column_list.end() )
  {
    *iter = iter->replaceControlCodes();
    ++iter;
  }
}

//----------------------------------------------------------------------
int FListViewItem::getVisibleLines()
{
  if ( visible_lines > 1 )
    return visible_lines;

  if ( ! isExpand() || ! hasChildren() )
  {
    visible_lines = 1;
    return visible_lines;
  }

  constFObjectIterator iter = FObject::begin();

  while ( iter != FObject::end() )
  {
    FListViewItem* child = static_cast<FListViewItem*>(*iter);
    visible_lines += child->getVisibleLines();
    ++iter;
  }

  return visible_lines;
}

//----------------------------------------------------------------------
void FListViewItem::resetVisibleLineCounter()
{
  visible_lines = 0;
  FObject* parent = getParent();

  if ( parent && parent->isInstanceOf("FListViewItem") )
  {
    FListViewItem* parent_item = static_cast<FListViewItem*>(parent);
    return parent_item->resetVisibleLineCounter();
  }
}


//----------------------------------------------------------------------
// class FListViewIterator
//----------------------------------------------------------------------

// constructor and destructor
//----------------------------------------------------------------------
FListViewIterator::FListViewIterator ()
  : iter_path()
  , node()
  , position(0)
{ }

//----------------------------------------------------------------------
FListViewIterator::FListViewIterator (FObjectIterator iter)
  : iter_path()
  , node(iter)
  , position(0)
{ }

//----------------------------------------------------------------------
FListViewIterator::~FListViewIterator()  // destructor
{ }

// FListViewIterator operators
//----------------------------------------------------------------------
FListViewIterator& FListViewIterator::operator ++ ()  // prefix
{
  nextElement(node);
  return *this;
}

//----------------------------------------------------------------------
FListViewIterator FListViewIterator::operator ++ (int)  // postfix
{
  FListViewIterator tmp = *this;
  ++(*this);
  return tmp;
}

//----------------------------------------------------------------------
FListViewIterator& FListViewIterator::operator -- ()  // prefix
{
  prevElement(node);
  return *this;
}

//----------------------------------------------------------------------
FListViewIterator FListViewIterator::operator -- (int)  // postfix
{
  FListViewIterator tmp = *this;
  --(*this);
  return tmp;
}

//----------------------------------------------------------------------
FListViewIterator& FListViewIterator::operator += (int n)
{
  while ( n > 0 )
  {
    nextElement(node);
    n--;
  }

  return *this;
}

//----------------------------------------------------------------------
FListViewIterator& FListViewIterator::operator -= (int n)
{
  while ( n > 0 )
  {
    prevElement(node);
    n--;
  }

  return *this;
}

// private methods of FListViewIterator
//----------------------------------------------------------------------
void FListViewIterator::nextElement (FObjectIterator& iter)
{
  FListViewItem* item = static_cast<FListViewItem*>(*iter);

  if ( item->isExpandable() && item->isExpand() )
  {
    iter_path.push(iter);
    iter = item->begin();
    position++;
  }
  else
  {
    ++iter;
    position++;

    if ( ! iter_path.empty() )
    {
      FObjectIterator& parent_iter = iter_path.top();

      if ( iter == (*parent_iter)->end() )
      {
        iter = parent_iter;
        iter_path.pop();
        ++iter;
      }
    }
  }
}

//----------------------------------------------------------------------
void FListViewIterator::prevElement (FObjectIterator& iter)
{
  FListViewItem* item;
  FObjectIterator start_iter = iter;

  if ( ! iter_path.empty() )
  {
    FObjectIterator& parent_iter = iter_path.top();

    if ( start_iter == (*parent_iter)->begin() )
    {
      iter = parent_iter;
      position--;
      iter_path.pop();
      return;
    }
  }

  --iter;
  item = static_cast<FListViewItem*>(*iter);

  if ( iter == start_iter )
    return;
  else
    position--;

  while ( item->isExpandable() && item->isExpand() )
  {
    iter_path.push(iter);
    iter = item->end();
    --iter;
    item = static_cast<FListViewItem*>(*iter);
  }
}

//----------------------------------------------------------------------
void FListViewIterator::parentElement()
{
  if ( iter_path.empty() )
    return;

  FObjectIterator& parent_iter = iter_path.top();

  while ( node != parent_iter )
    prevElement(node);
}


//----------------------------------------------------------------------
// class FListView
//----------------------------------------------------------------------

// constructor and destructor
//----------------------------------------------------------------------
FListView::FListView (FWidget* parent)
  : FWidget(parent)
  , root()
  , selflist()
  , itemlist()
  , current_iter()
  , first_visible_line()
  , last_visible_line()
  , header()
  , headerline()
  , vbar(0)
  , hbar(0)
  , drag_scroll(fc::noScroll)
  , scroll_repeat(100)
  , scroll_distance(1)
  , scroll_timer(false)
  , tree_view(false)
  , clicked_expander_pos(-1, -1)
  , xoffset(0)
  , nf_offset(0)
  , max_line_width(1)
{
  init();
}

//----------------------------------------------------------------------
FListView::~FListView()  // destructor
{
  delOwnTimer();
  delete vbar;
  delete hbar;
}

// public methods of FListView
//----------------------------------------------------------------------
uInt FListView::getCount()
{
  int n = 0;
  FObjectIterator iter = itemlist.begin();

  while ( iter != itemlist.end() )
  {
    FListViewItem* item = static_cast<FListViewItem*>(*iter);
    n += item->getVisibleLines();
    ++iter;
  }

  return uInt(n);
}

//----------------------------------------------------------------------
fc::text_alignment FListView::getColumnAlignment (int column) const
{
  // Get the alignment for a column

  if ( column < 1 || header.empty() || column > int(header.size()) )
    return fc::alignLeft;

  column--;  // Convert column position to address offset (index)
  return header[uInt(column)].alignment;
}

//----------------------------------------------------------------------
FString FListView::getColumnText (int column) const
{
  // Get the text of column

  if ( column < 1 || header.empty() || column > int(header.size()) )
    return fc::emptyFString::get();

  column--;  // Convert column position to address offset (index)
  return header[uInt(column)].name;
}

//----------------------------------------------------------------------
void FListView::setGeometry (int x, int y, int w, int h, bool adjust)
{
  // Set the widget geometry

  FWidget::setGeometry(x, y, w, h, adjust);

  if ( isNewFont() )
  {
    vbar->setGeometry (getWidth(), 2, 2, getHeight() - 2);
    hbar->setGeometry (1, getHeight(), getWidth() - 2, 1);
  }
  else
  {
    vbar->setGeometry (getWidth(), 2, 1, getHeight() - 2);
    hbar->setGeometry (2, getHeight(), getWidth() - 2, 1);
  }
}

//----------------------------------------------------------------------
void FListView::setColumnAlignment (int column, fc::text_alignment align)
{
  // Set the alignment for a column

  if ( column < 1 || header.empty() || column > int(header.size()) )
    return;

  column--;  // Convert column position to address offset (index)
  header[uInt(column)].alignment = align;
}

//----------------------------------------------------------------------
void FListView::setColumnText (int column, const FString& label)
{
  // Set the text for a column

  if ( column < 1 || header.empty() || column > int(header.size()) )
    return;

  column--;  // Convert column position to address offset (index)

  if ( ! header[uInt(column)].fixed_width )
  {
    int length = int(label.getLength());

    if ( length > header[uInt(column)].width )
      header[uInt(column)].width = length;
  }

  header[uInt(column)].name = label;
}

//----------------------------------------------------------------------
int FListView::addColumn (const FString& label, int width)
{
  Header new_column;
  new_column.name = label;
  new_column.width = width;

  if ( new_column.width == USE_MAX_SIZE )
  {
    new_column.fixed_width = false;
    new_column.width = int(label.getLength());
  }
  else
    new_column.fixed_width = true;

  header.push_back (new_column);
  return int(std::distance(header.begin(), header.end()));
}

//----------------------------------------------------------------------
FObject::FObjectIterator FListView::insert ( FListViewItem* item
                                           , FObjectIterator parent_iter )
{
  static const int padding_space = 1;
  int  line_width = padding_space;  // leading space
  uInt column_idx = 0;
  uInt entries = uInt(item->column_list.size());
  FObjectIterator item_iter;
  headerItems::iterator header_iter;

  if ( parent_iter == null_iter )
    return null_iter;

  // Determine the line width
  header_iter = header.begin();

  while ( header_iter != header.end() )
  {
    int width = (*header_iter).width;
    bool fixed_width = (*header_iter).fixed_width;

    if ( ! fixed_width )
    {
      int len;

      if ( column_idx < entries )
        len = int(item->column_list[column_idx].getLength());
      else
        len = 0;

      if ( len > width )
        (*header_iter).width = len;
    }

    line_width += (*header_iter).width + padding_space;  // width + trailing space
    column_idx++;
    ++header_iter;
  }

  recalculateHorizontalBar (line_width);

  if  ( parent_iter == root )
  {
    item_iter = appendItem (item);
  }
  else if ( *parent_iter )
  {
    if ( (*parent_iter)->isInstanceOf("FListView") )
    {
      // Add FListViewItem to a FListView parent
      FListView* parent = static_cast<FListView*>(*parent_iter);
      item_iter = parent->appendItem (item);
    }
    else if ( (*parent_iter)->isInstanceOf("FListViewItem") )
    {
      // Add FListViewItem to a FListViewItem parent
      FListViewItem* parent = static_cast<FListViewItem*>(*parent_iter);
      item_iter = parent->appendItem (item);
    }
    else
      item_iter = null_iter;
  }
  else
    item_iter = null_iter;

  if ( itemlist.size() == 1 )
  {
    // Select first item on insert
    current_iter = itemlist.begin();
    // The visible area of the list begins with the first element
    first_visible_line = itemlist.begin();
  }

  int element_count = int(getCount());
  recalculateVerticalBar (element_count);
  return item_iter;
}

//----------------------------------------------------------------------
FObject::FObjectIterator FListView::insert ( const FStringList& cols
                                           , data_ptr d
                                           , FObjectIterator parent_iter )
{
  FListViewItem* item;

  if ( cols.empty() || parent_iter == null_iter )
    return null_iter;

  if ( ! *parent_iter )
    parent_iter = root;

  try
  {
    item = new FListViewItem (cols, d, null_iter);
  }
  catch (const std::bad_alloc& ex)
  {
    std::cerr << "not enough memory to alloc " << ex.what() << std::endl;
    return null_iter;
  }

  item->replaceControlCodes();
  return insert(item, parent_iter);
}

//----------------------------------------------------------------------
FObject::FObjectIterator FListView::insert ( const std::vector<long>& cols
                                           , data_ptr d
                                           , FObjectIterator parent_iter )
{
  FObjectIterator item_iter;
  FStringList str_cols;

  if ( ! cols.empty() )
  {
    for (uInt i = 0; i < cols.size(); i++)
      str_cols.push_back (FString().setNumber(cols[i]));
  }

  item_iter = insert (str_cols, d, parent_iter);

  return item_iter;
}

//----------------------------------------------------------------------
void FListView::onKeyPress (FKeyEvent* ev)
{
  int position_before = current_iter.getPosition()
    , xoffset_before = xoffset
    , first_line_position_before = first_visible_line.getPosition()
    , pagesize = getClientHeight() - 1
    , key = ev->key();
  clicked_expander_pos.setPoint(-1, -1);

  switch ( key )
  {
    case fc::Fkey_return:
    case fc::Fkey_enter:
      processClick();
      ev->accept();
      break;

    case fc::Fkey_up:
      stepBackward();
      ev->accept();
      break;

    case fc::Fkey_down:
      stepForward();
      ev->accept();
      break;

    case fc::Fkey_left:
      keyLeft (first_line_position_before);
      ev->accept();
      break;

    case fc::Fkey_right:
      keyRight(first_line_position_before);
      ev->accept();
      break;

    case fc::Fkey_ppage:
      stepBackward(pagesize);
      ev->accept();
      break;

    case fc::Fkey_npage:
      stepForward(pagesize);
      ev->accept();
      break;

    case fc::Fkey_home:
      keyHome();
      ev->accept();
      break;

    case fc::Fkey_end:
      keyEnd();
      ev->accept();
      break;

    case int('+'):
      if ( keyPlus() )
        ev->accept();
      break;

    case int('-'):
      if ( keyMinus() )
        ev->accept();
      break;

    default:
      ev->ignore();
  }

  if ( position_before != current_iter.getPosition() )
    processChanged();

  if ( ev->isAccepted() )
  {
    bool draw_vbar = first_line_position_before
                  != first_visible_line.getPosition();
    bool draw_hbar = xoffset_before != xoffset;
    updateDrawing (draw_vbar, draw_hbar);
  }
}

//----------------------------------------------------------------------
void FListView::onMouseDown (FMouseEvent* ev)
{
  if ( ev->getButton() != fc::LeftButton )
  {
    clicked_expander_pos.setPoint(-1, -1);
    return;
  }

  if ( ! hasFocus() )
  {
    FWidget* focused_widget = getFocusWidget();
    FFocusEvent out (fc::FocusOut_Event);
    FApplication::queueEvent(focused_widget, &out);
    setFocus();

    if ( focused_widget )
      focused_widget->redraw();

    if ( getStatusBar() )
      getStatusBar()->drawMessage();
  }

  int first_line_position_before = first_visible_line.getPosition()
    , mouse_x = ev->getX()
    , mouse_y = ev->getY();

  if ( mouse_x > 1 && mouse_x < getWidth()
    && mouse_y > 1 && mouse_y < getHeight() )
  {
    int new_pos = first_visible_line.getPosition() + mouse_y - 2;

    if ( new_pos < int(getCount()) )
      setRelativePosition (mouse_y - 2);

    if ( tree_view )
    {
      const FListViewItem* item = getCurrentItem();
      int indent = int(item->getDepth() << 1);  // indent = 2 * depth

      if ( item->isExpandable() && mouse_x - 2 == indent - xoffset )
        clicked_expander_pos = ev->getPos();
    }

    if ( isVisible() )
      drawList();

    vbar->setValue (first_visible_line.getPosition());

    if ( vbar->isVisible()
      && first_line_position_before != first_visible_line.getPosition() )
      vbar->drawBar();

    updateTerminal();
    flush_out();
  }
}

//----------------------------------------------------------------------
void FListView::onMouseUp (FMouseEvent* ev)
{
  if ( drag_scroll != fc::noScroll )
  {
    delOwnTimer();
    drag_scroll = fc::noScroll;
    scroll_distance = 1;
    scroll_timer = false;
  }

  if ( ev->getButton() == fc::LeftButton )
  {
    int mouse_x = ev->getX();
    int mouse_y = ev->getY();

    if ( mouse_x > 1 && mouse_x < getWidth()
      && mouse_y > 1 && mouse_y < getHeight() )
    {
      if ( tree_view )
      {
        FListViewItem* item = getCurrentItem();

        if ( item->isExpandable() && clicked_expander_pos == ev->getPos() )
        {
          if ( item->isExpand() )
            item->collapse();
          else
            item->expand();

          adjustSize();

          if ( isVisible() )
            draw();
        }
      }

      processChanged();
    }
  }

  clicked_expander_pos.setPoint(-1, -1);
}

//----------------------------------------------------------------------
void FListView::onMouseMove (FMouseEvent* ev)
{
  if ( ev->getButton() != fc::LeftButton )
  {
    clicked_expander_pos.setPoint(-1, -1);
    return;
  }

  int first_line_position_before = first_visible_line.getPosition()
    , mouse_x = ev->getX()
    , mouse_y = ev->getY();

  if ( mouse_x > 1 && mouse_x < getWidth()
    && mouse_y > 1 && mouse_y < getHeight() )
  {
    int new_pos = first_visible_line.getPosition() + mouse_y - 2;

    if ( new_pos < int(getCount()) )
      setRelativePosition (mouse_y - 2);

    if ( isVisible() )
      drawList();

    vbar->setValue (first_visible_line.getPosition());

    if ( vbar->isVisible()
      && first_line_position_before != first_visible_line.getPosition() )
      vbar->drawBar();

    updateTerminal();
    flush_out();
  }

  // auto-scrolling when dragging mouse outside the widget
  if ( mouse_y < 2 )
  {
    // drag up
    if ( drag_scroll != fc::noScroll
      && scroll_distance < getClientHeight() )
      scroll_distance++;

    if ( ! scroll_timer && current_iter.getPosition() > 0 )
    {
      scroll_timer = true;
      addTimer(scroll_repeat);

      if ( ev->getButton() == fc::RightButton )
        drag_scroll = fc::scrollUpSelect;
      else
        drag_scroll = fc::scrollUp;
    }

    if ( current_iter.getPosition() == 0 )
    {
      delOwnTimer();
      drag_scroll = fc::noScroll;
    }
  }
  else if ( mouse_y >= getHeight() )
  {
    // drag down
    if ( drag_scroll != fc::noScroll
      && scroll_distance < getClientHeight() )
      scroll_distance++;

    if ( ! scroll_timer && current_iter.getPosition() <= int(getCount()) )
    {
      scroll_timer = true;
      addTimer(scroll_repeat);

      if ( ev->getButton() == fc::RightButton )
        drag_scroll = fc::scrollDownSelect;
      else
        drag_scroll = fc::scrollDown;
    }

    if ( current_iter.getPosition() - 1 == int(getCount()) )
    {
      delOwnTimer();
      drag_scroll = fc::noScroll;
    }
  }
  else
  {
    // no dragging
    delOwnTimer();
    scroll_timer = false;
    scroll_distance = 1;
    drag_scroll = fc::noScroll;
  }
}

//----------------------------------------------------------------------
void FListView::onMouseDoubleClick (FMouseEvent* ev)
{
  int mouse_x, mouse_y;

  if ( ev->getButton() != fc::LeftButton )
    return;

  mouse_x = ev->getX();
  mouse_y = ev->getY();

  if ( mouse_x > 1 && mouse_x < getWidth()
    && mouse_y > 1 && mouse_y < getHeight() )
  {
    if ( first_visible_line.getPosition() + mouse_y - 1 > int(getCount()) )
      return;

    FListViewItem* item = getCurrentItem();

    if ( tree_view && item->isExpandable() )
    {
      if ( item->isExpand() )
        item->collapse();
      else
        item->expand();

      adjustSize();

      if ( isVisible() )
        draw();
    }

    processClick();
  }

  clicked_expander_pos.setPoint(-1, -1);
}

//----------------------------------------------------------------------
void FListView::onTimer (FTimerEvent*)
{
  int element_count = int(getCount())
    , position_before = current_iter.getPosition()
    , first_line_position_before = first_visible_line.getPosition();

  switch ( int(drag_scroll) )
  {
    case fc::noScroll:
      return;

    case fc::scrollUp:
    case fc::scrollUpSelect:
      if ( position_before == 0 )
      {
        drag_scroll = fc::noScroll;
        return;
      }

      stepBackward(scroll_distance);
      break;

    case fc::scrollDown:
    case fc::scrollDownSelect:
      if ( position_before + 1 == element_count )
      {
        drag_scroll = fc::noScroll;
        return;
      }

      stepForward(scroll_distance);
      break;

    default:
      break;
  }

  if ( isVisible() )
    drawList();

  vbar->setValue (first_visible_line.getPosition());

  if ( vbar->isVisible()
    && first_line_position_before != first_visible_line.getPosition() )
    vbar->drawBar();

  updateTerminal();
  flush_out();
}

//----------------------------------------------------------------------
void FListView::onWheel (FWheelEvent* ev)
{
  int wheel
    , element_count = int(getCount())
    , position_before = current_iter.getPosition()
    , first_line_position_before = first_visible_line.getPosition()
    , pagesize = 4;

  wheel = ev->getWheel();

  if ( drag_scroll != fc::noScroll )
  {
    delOwnTimer();
    scroll_timer = false;
    scroll_distance = 1;
    drag_scroll = fc::noScroll;
  }

  switch ( wheel )
  {
    case fc::WheelUp:
      if ( current_iter.getPosition() == 0 )
        break;

      if ( first_visible_line.getPosition() - pagesize >= 0 )
      {
        current_iter -= pagesize;
        first_visible_line -= pagesize;
        last_visible_line -= pagesize;
      }
      else
      {
        // Save relative position from the first line
        int ry = current_iter.getPosition() - first_visible_line.getPosition();
        // Save difference from top
        int difference = first_visible_line.getPosition();
        first_visible_line -= difference;
        last_visible_line -= difference;
        setRelativePosition(ry);
      }

      break;

    case fc::WheelDown:
      if ( current_iter.getPosition() + 1 == element_count )
        break;

      if ( last_visible_line.getPosition() + pagesize < element_count )
      {
        current_iter += pagesize;
        first_visible_line += pagesize;
        last_visible_line += pagesize;
      }
      else
      {
        // Save relative position from the first line
        int ry = current_iter.getPosition() - first_visible_line.getPosition();
        // Save difference from bottom
        int differenz = element_count - last_visible_line.getPosition() - 1;
        first_visible_line += differenz;
        last_visible_line += differenz;
        setRelativePosition(ry);
      }

      break;

    default:
      break;
  }

  if ( position_before != current_iter.getPosition() )
    processChanged();

  if ( isVisible() )
    drawList();

  vbar->setValue (first_visible_line.getPosition());

  if ( vbar->isVisible()
    && first_line_position_before != first_visible_line.getPosition() )
    vbar->drawBar();

  updateTerminal();
  flush_out();
}

//----------------------------------------------------------------------
void FListView::onFocusIn (FFocusEvent*)
{
  if ( getStatusBar() )
    getStatusBar()->drawMessage();
}

//----------------------------------------------------------------------
void FListView::onFocusOut (FFocusEvent*)
{
  if ( getStatusBar() )
  {
    getStatusBar()->clearMessage();
    getStatusBar()->drawMessage();
  }

  delOwnTimer();
}


// protected methods of FListView
//----------------------------------------------------------------------
void FListView::adjustViewport()
{
  int element_count = int(getCount());
  int height = getClientHeight();

  if ( element_count == 0 || height <= 0 )
    return;

  if ( element_count < height )
  {
    first_visible_line = itemlist.begin();
    last_visible_line = first_visible_line;
    last_visible_line += element_count - 1;
  }

  if ( first_visible_line.getPosition() > element_count - height  )
  {
    int difference = first_visible_line.getPosition()
                   - (element_count - height);

    if ( first_visible_line.getPosition() - difference + 1 > 0 )
    {
      first_visible_line -= difference;
      last_visible_line -= difference;
    }
  }

  int max_last_visible_line = first_visible_line.getPosition()
                            + height - 1;

  if ( last_visible_line.getPosition() > max_last_visible_line )
  {
    last_visible_line = first_visible_line;
    last_visible_line += height - 1;
  }

  if ( current_iter.getPosition() > last_visible_line.getPosition() )
    current_iter = last_visible_line;
}

//----------------------------------------------------------------------
void FListView::adjustSize()
{
  int element_count;
  FWidget::adjustSize();
  adjustViewport();

  element_count = int(getCount());
  vbar->setMaximum (element_count - getClientHeight());
  vbar->setPageSize (element_count, getClientHeight());
  vbar->setX (getWidth());
  vbar->setHeight (getClientHeight(), false);
  vbar->resize();

  hbar->setMaximum (max_line_width - getClientWidth());
  hbar->setPageSize (max_line_width, getClientWidth());
  hbar->setY (getHeight() );
  hbar->setWidth (getClientWidth(), false);
  hbar->resize();

  if ( element_count <= getClientHeight() )
    vbar->hide();
  else
    vbar->setVisible();

  if ( max_line_width <= getClientWidth() )
    hbar->hide();
  else
    hbar->setVisible();
}


// private methods of FListView
//----------------------------------------------------------------------
void FListView::init()
{
  selflist.push_back(this);
  root = selflist.begin();
  null_iter = selflist.end();

  setForegroundColor (wc.dialog_fg);
  setBackgroundColor (wc.dialog_bg);

  try
  {
    vbar = new FScrollbar(fc::vertical, this);
    vbar->setMinimum(0);
    vbar->setValue(0);
    vbar->hide();

    hbar = new FScrollbar(fc::horizontal, this);
    hbar->setMinimum(0);
    hbar->setValue(0);
    hbar->hide();
  }
  catch (const std::bad_alloc& ex)
  {
    std::cerr << "not enough memory to alloc " << ex.what() << std::endl;
    return;
  }

  setGeometry (1, 1, 5, 4, false);  // initialize geometry values

  vbar->addCallback
  (
    "change-value",
    F_METHOD_CALLBACK (this, &FListView::cb_VBarChange)
  );

  hbar->addCallback
  (
    "change-value",
    F_METHOD_CALLBACK (this, &FListView::cb_HBarChange)
  );

  nf_offset = isNewFont() ? 1 : 0;
  setTopPadding(1);
  setLeftPadding(1);
  setBottomPadding(1);
  setRightPadding(1 + nf_offset);
}

//----------------------------------------------------------------------
uInt FListView::getAlignOffset ( fc::text_alignment align
                               , uInt txt_length
                               , uInt width )
{
  switch ( align )
  {
    case fc::alignLeft:
      return 0;

    case fc::alignCenter:
      if ( txt_length < width )
        return uInt((width - txt_length) / 2);
      else
        return 0;

    case fc::alignRight:
      if ( txt_length < width )
        return width - txt_length;
      else
        return 0;
  }

  return 0;
}

//----------------------------------------------------------------------
void FListView::draw()
{
  bool isFocus;

  if ( current_iter.getPosition() < 1 )
    current_iter = itemlist.begin();

  setColor();

  if ( isMonochron() )
    setReverse(true);

  if ( isNewFont() )
    drawBorder (1, 1, getWidth() - 1, getHeight());
  else
    drawBorder();

  if ( isNewFont() && ! vbar->isVisible() )
  {
    setColor();

    for (int y = 2; y < getHeight(); y++)
    {
      setPrintPos (getWidth(), y);
      print (' ');  // clear right side of the scrollbar
    }
  }

  drawColumnLabels();

  if ( isMonochron() )
    setReverse(false);

  if ( vbar->isVisible() )
    vbar->redraw();

  if ( hbar->isVisible() )
    hbar->redraw();

  drawList();
  isFocus = ((flags & fc::focus) != 0);

  if ( isFocus && getStatusBar() )
  {
    const FString& msg = getStatusbarMessage();
    const FString& curMsg = getStatusBar()->getMessage();

    if ( curMsg != msg )
    {
      getStatusBar()->setMessage(msg);
      getStatusBar()->drawMessage();
    }
  }
}

//----------------------------------------------------------------------
void FListView::drawColumnLabels()
{
  static const int leading_space = 1;
  static const int trailing_space = 1;
  static const int ellipsis_length = 2;
  std::vector<char_data>::const_iterator first, last;
  headerItems::const_iterator iter;
  FString txt;
  uInt txt_length;

  if ( header.empty()
    || getHeight() <= 2
    || getWidth() <= 4
    || max_line_width < 1 )
    return;

  iter = header.begin();
  headerline.clear();

  while ( iter != header.end() )
  {
    const FString& text = (*iter).name;
    int width = (*iter).width;
    int column_width;

    if ( text.isNull() || text.isEmpty() )
    {
      ++iter;
      continue;
    }

    txt = " " + text;
    txt_length = txt.getLength();
    column_width = leading_space + width;

    if ( isEnabled() )
      setColor (wc.label_emphasis_fg, wc.label_bg);
    else
      setColor (wc.label_inactive_fg, wc.label_inactive_bg);

    if ( txt_length <= uInt(column_width) )
    {
      headerline << txt;

      if ( txt_length < uInt(column_width) )
        headerline << ' ';  // trailing space

      if ( txt_length + trailing_space < uInt(column_width) )
      {
        setColor();
        const FString line ( uInt(column_width) - trailing_space - txt_length
                           , wchar_t(fc::BoxDrawingsHorizontal) );
        headerline << line;  // horizontal line
      }
    }
    else
    {
      // Print ellipsis
      headerline << ' ';
      headerline << text.left(uInt(width - ellipsis_length));
      setColor (wc.label_ellipsis_fg, wc.label_bg);
      headerline << "..";

      if ( iter == header.end() - 1 )  // Last element
        headerline << ' ';
    }

    ++iter;
  }

  std::vector<char_data> h;
  h << headerline;
  first = h.begin() + xoffset;

  if ( int(h.size()) <= getClientWidth() )
    last = h.end();
  else
  {
    int len = getClientWidth() + xoffset - 1;

    if ( len > int(h.size()) )
      len = int(h.size());

    last = h.begin() + len;
  }

  setPrintPos (2, 1);
  print() << std::vector<char_data>(first, last);
}

//----------------------------------------------------------------------
void FListView::drawList()
{
  uInt page_height, y;
  bool is_focus;
  FListViewIterator iter;

  if ( itemlist.empty() || getHeight() <= 2 || getWidth() <= 4 )
    return;

  y           = 0;
  page_height = uInt(getHeight() - 2);
  is_focus    = ((flags & fc::focus) != 0);
  iter = first_visible_line;

  while ( iter != itemlist.end() && y < page_height )
  {
    bool is_current_line = bool( iter == current_iter );
    const FListViewItem* item = static_cast<FListViewItem*>(*iter);
    setPrintPos (2, 2 + int(y));

    // Draw one FListViewItem
    drawListLine (item, is_focus, is_current_line);

    if ( is_focus && is_current_line )
      setCursorPos (3, 2 + int(y));  // first character

    last_visible_line = iter;
    y++;
    ++iter;
  }

  // Reset color
  setColor();

  // Clean empty space after last element
  while ( y < uInt(getClientHeight()) )
  {
    setPrintPos (2, 2 + int(y));
    print (FString(getClientWidth(), ' '));
    y++;
  }
}

//----------------------------------------------------------------------
void FListView::drawListLine ( const FListViewItem* item
                             , bool is_focus
                             , bool is_current )
{
  uInt indent = item->getDepth() << 1;  // indent = 2 * depth

  setColor (wc.list_fg, wc.list_bg);

  if ( is_current )
  {
    if ( is_focus && getMaxColor() < 16 )
      setBold();

    if ( isMonochron() )
      unsetBold();

    if ( is_focus )
    {
      setColor ( wc.current_element_focus_fg
               , wc.current_element_focus_bg );
    }
    else
      setColor ( wc.current_element_fg
               , wc.current_element_bg );

    if ( isMonochron() )
      setReverse(false);
  }
  else
  {
    if ( isMonochron() )
      setReverse(true);
    else if ( is_focus && getMaxColor() < 16 )
      unsetBold();
  }

  // print the entry
  FString line;

  if ( tree_view )
  {
    if ( indent > 0 )
      line = FString(indent, L' ');

    if ( item->expandable  )
    {
      if ( item->is_expand )
      {
        line += wchar_t(fc::BlackDownPointingTriangle);  // ▼
        line += L' ';
      }
      else
      {
        line += wchar_t(fc::BlackRightPointingPointer);  // ►
        line += L' ';
      }
    }
    else
      line += L"  ";
  }
  else
    line = L" ";

  // print columns
  if ( ! item->column_list.empty() )
  {
    for (uInt i = 0; i < item->column_list.size(); )
    {
      static const int leading_space = 1;
      static const int ellipsis_length = 2;

      const FString& text = item->column_list[i];
      int width = header[i].width;
      uInt txt_length = text.getLength();
      // Increment the value of i for the column position
      // and the next iteration
      i++;
      fc::text_alignment align = getColumnAlignment(int(i));
      uInt align_offset = getAlignOffset (align, txt_length, uInt(width));

      if ( tree_view && i == 1 )
      {
        width -= indent;
        width--;
      }

      // Insert alignment spaces
      if ( align_offset > 0 )
        line += FString(align_offset, L' ');

      if ( align_offset + txt_length <= uInt(width) )
      {
        // Insert text and trailing space
        line += text.left(width);
        line += FString ( leading_space + width
                        - int(align_offset + txt_length), L' ');
      }
      else if ( align == fc::alignRight )
      {
        // Ellipse right align text
        line += FString (L"..");
        line += text.right(width - ellipsis_length);
        line += L' ';
      }
      else
      {
        // Ellipse left align text and center text
        line += text.left(width - ellipsis_length);
        line += FString (L".. ");
      }
    }
  }

  line = line.mid ( uInt(1 + xoffset)
                  , uInt(getWidth() - nf_offset - 2) );
  const wchar_t* const& element_str = line.wc_str();
  uInt len = line.getLength();
  uInt i;

  for (i = 0; i < len; i++)
    *this << element_str[i];

  for (; i < uInt(getWidth() - nf_offset - 2); i++)
    print (' ');
}

//----------------------------------------------------------------------
void FListView::updateDrawing (bool draw_vbar, bool draw_hbar)
{
  if ( isVisible() )
    draw();

  vbar->setValue (first_visible_line.getPosition());

  if ( vbar->isVisible() && draw_vbar )
    vbar->drawBar();

  hbar->setValue (xoffset);

  if ( hbar->isVisible() && draw_hbar )
    hbar->drawBar();

  updateTerminal();
  flush_out();
}

//----------------------------------------------------------------------
void FListView::recalculateHorizontalBar (int len)
{
  if ( len <= max_line_width )
    return;

  max_line_width = len;

  if ( len >= getWidth() - nf_offset - 3 )
  {
    hbar->setMaximum (max_line_width - getWidth() + nf_offset + 4);
    hbar->setPageSize (max_line_width, getWidth() - nf_offset - 4);
    hbar->calculateSliderValues();

    if ( ! hbar->isVisible() )
      hbar->setVisible();
  }
}

//----------------------------------------------------------------------
void FListView::recalculateVerticalBar (int element_count)
{
  vbar->setMaximum (element_count - getHeight() + 2);
  vbar->setPageSize (element_count, getHeight() - 2);
  vbar->calculateSliderValues();

  if ( ! vbar->isVisible() && element_count >= getHeight() - 1 )
    vbar->setVisible();
}

//----------------------------------------------------------------------
FObject::FObjectIterator FListView::appendItem (FListViewItem* item)
{
  addChild (item);
  itemlist.push_back (item);
  return --itemlist.end();
}

//----------------------------------------------------------------------
void FListView::processClick()
{
  emitCallback("clicked");
}

//----------------------------------------------------------------------
void FListView::processChanged()
{
  emitCallback("row-changed");
}

//----------------------------------------------------------------------
inline void FListView::keyLeft (int& first_line_position_before)
{
  int element_count = int(getCount());
  int position_before = current_iter.getPosition();
  FListViewItem* item = getCurrentItem();

  if ( xoffset == 0 )
  {
    if ( tree_view && item->isExpandable() && item->isExpand() )
    {
      // Collapse element
      item->collapse();
      adjustSize();
      element_count = int(getCount());
      recalculateVerticalBar (element_count);
      // Force vertical scrollbar redraw
      first_line_position_before = -1;
    }
    else if ( item->hasParent() )
    {
      // Jump to parent element
      FObject* parent = item->getParent();

      if ( parent->isInstanceOf("FListViewItem") )
      {
        current_iter.parentElement();

        if ( current_iter.getPosition() < first_line_position_before )
        {
          int difference = position_before - current_iter.getPosition();

          if ( first_visible_line.getPosition() - difference >= 0 )
          {
            first_visible_line -= difference;
            last_visible_line -= difference;
          }
          else
          {
            int d = first_visible_line.getPosition();
            first_visible_line -= d;
            last_visible_line -= d;
          }
        }
      }
    }
  }
  else
  {
    // Scroll left
    xoffset--;

    if ( xoffset < 0 )
      xoffset = 0;
  }
}

//----------------------------------------------------------------------
inline void FListView::keyRight (int& first_line_position_before)
{
  int xoffset_end = max_line_width - getClientWidth();
  FListViewItem* item = getCurrentItem();

  if ( tree_view && item->isExpandable() && ! item->isExpand() )
  {
    // Expand element
    item->expand();
    adjustSize();
    // Force vertical scrollbar redraw
    first_line_position_before = -1;
  }
  else
  {
    // Scroll right
    xoffset++;

    if ( xoffset > xoffset_end )
      xoffset = xoffset_end;

    if ( xoffset < 0 )
      xoffset = 0;
  }
}

//----------------------------------------------------------------------
inline void FListView::keyHome()
{
  current_iter -= current_iter.getPosition();
  int difference = first_visible_line.getPosition();
  first_visible_line -= difference;
  last_visible_line -= difference;
}

//----------------------------------------------------------------------
inline void FListView::keyEnd()
{
  int element_count = int(getCount());
  current_iter += element_count - current_iter.getPosition() - 1;
  int difference = element_count - last_visible_line.getPosition() - 1;
  first_visible_line += difference;
  last_visible_line += difference;
}

//----------------------------------------------------------------------
inline bool FListView::keyPlus()
{
  FListViewItem* item = getCurrentItem();

  if ( tree_view && item->isExpandable() && ! item->isExpand() )
  {
    item->expand();
    adjustSize();
    return true;
  }

  return false;
}

//----------------------------------------------------------------------
inline bool FListView::keyMinus()
{
  FListViewItem* item = getCurrentItem();

  if ( tree_view && item->isExpandable() && item->isExpand() )
  {
    item->collapse();
    adjustSize();
    return true;
  }

  return false;
}

//----------------------------------------------------------------------
void FListView::setRelativePosition (int ry)
{
  current_iter = first_visible_line;
  current_iter += ry;
}

//----------------------------------------------------------------------
void FListView::stepForward()
{
  if ( current_iter == last_visible_line )
  {
    ++last_visible_line;

    if ( last_visible_line == itemlist.end() )
      --last_visible_line;
    else
      ++first_visible_line;
  }

  ++current_iter;

  if ( current_iter == itemlist.end() )
    --current_iter;
}

//----------------------------------------------------------------------
void FListView::stepBackward()
{
  if ( current_iter == first_visible_line
    && current_iter != itemlist.begin() )
  {
    --first_visible_line;
    --last_visible_line;
  }

  if ( current_iter != itemlist.begin() )
    --current_iter;
}

//----------------------------------------------------------------------
void FListView::stepForward (int distance)
{
  int element_count = int(getCount());

  if ( current_iter.getPosition() + 1 == element_count )
    return;

  if ( current_iter.getPosition() + distance < element_count )
  {
    current_iter += distance;
  }
  else
  {
    current_iter += element_count - current_iter.getPosition() - 1;
  }

  if ( current_iter.getPosition() > last_visible_line.getPosition() )
  {
    if ( last_visible_line.getPosition() + distance < element_count )
    {
      first_visible_line += distance;
      last_visible_line += distance;
    }
    else
    {
      int differenz = element_count - last_visible_line.getPosition() - 1;
      first_visible_line += differenz;
      last_visible_line += differenz;
    }
  }
}

//----------------------------------------------------------------------
void FListView::stepBackward (int distance)
{
  if ( current_iter.getPosition() == 0 )
   return;

  if ( current_iter.getPosition() - distance >= 0 )
  {
    current_iter -= distance;
  }
  else
  {
    current_iter -= current_iter.getPosition();
  }

  if ( current_iter.getPosition() < first_visible_line.getPosition() )
  {
    if ( first_visible_line.getPosition() - distance >= 0 )
    {
      first_visible_line -= distance;
      last_visible_line -= distance;
    }
    else
    {
      int difference = first_visible_line.getPosition();
      first_visible_line -= difference;
      last_visible_line -= difference;
    }
  }
}

//----------------------------------------------------------------------
void FListView::scrollToX (int x)
{
  int xoffset_end = max_line_width - getClientWidth();

  if ( xoffset == x )
    return;

  xoffset = x;

  if ( xoffset > xoffset_end )
    xoffset = xoffset_end;

  if ( xoffset < 0 )
    xoffset = 0;
}

//----------------------------------------------------------------------
void FListView::scrollToY (int y)
{
  int pagesize = getClientHeight() - 1;
  int element_count = int(getCount());

  if ( first_visible_line.getPosition() == y )
    return;

  // Save relative position from the top line
  int ry = current_iter.getPosition() - first_visible_line.getPosition();

  if ( y + pagesize <= element_count )
  {
    first_visible_line = itemlist.begin();
    first_visible_line += y;
    setRelativePosition (ry);
    last_visible_line = first_visible_line;
    last_visible_line += pagesize;
  }
  else
  {
    int differenz = element_count - last_visible_line.getPosition() - 1;
    current_iter += differenz;
    first_visible_line += differenz;
    last_visible_line += differenz;
  }
}

//----------------------------------------------------------------------
void FListView::scrollTo (int x, int y)
{
  scrollToX(x);
  scrollToY(y);
}

//----------------------------------------------------------------------
void FListView::cb_VBarChange (FWidget*, data_ptr)
{
  FScrollbar::sType scrollType;
  int distance = 1;
  int first_line_position_before = first_visible_line.getPosition();

  scrollType = vbar->getScrollType();

  switch ( scrollType )
  {
    case FScrollbar::noScroll:
      break;

    case FScrollbar::scrollPageBackward:
      distance = getClientHeight();
      // fall through
    case FScrollbar::scrollStepBackward:
      stepBackward(distance);
      break;

    case FScrollbar::scrollPageForward:
      distance = getClientHeight();
      // fall through
    case FScrollbar::scrollStepForward:
      stepForward(distance);
      break;

    case FScrollbar::scrollJump:
    {
      int value = vbar->getValue();
      scrollToY (value);

      break;
    }

    case FScrollbar::scrollWheelUp:
    {
      FWheelEvent wheel_ev (fc::MouseWheel_Event, FPoint(2,2), fc::WheelUp);
      onWheel(&wheel_ev);
    }
    break;

    case FScrollbar::scrollWheelDown:
    {
      FWheelEvent wheel_ev (fc::MouseWheel_Event, FPoint(2,2), fc::WheelDown);
      onWheel(&wheel_ev);
    }
    break;
  }

  if ( isVisible() )
    drawList();

  if ( scrollType >= FScrollbar::scrollStepBackward
    && scrollType <= FScrollbar::scrollPageForward )
  {
    vbar->setValue (first_visible_line.getPosition());

    if ( vbar->isVisible()
      && first_line_position_before != first_visible_line.getPosition() )
      vbar->drawBar();

    updateTerminal();
    flush_out();
  }
}

//----------------------------------------------------------------------
void FListView::cb_HBarChange (FWidget*, data_ptr)
{
  FScrollbar::sType scrollType;
  int distance = 1
    , pagesize = 4
    , xoffset_before = xoffset
    , xoffset_end = max_line_width - getClientWidth();
  scrollType = hbar->getScrollType();

  switch ( scrollType )
  {
    case FScrollbar::noScroll:
      break;

    case FScrollbar::scrollPageBackward:
      distance = getClientWidth();
      // fall through
    case FScrollbar::scrollStepBackward:
      xoffset -= distance;

      if ( xoffset < 0 )
        xoffset = 0;
      break;

    case FScrollbar::scrollPageForward:
      distance = getClientWidth();
      // fall through
    case FScrollbar::scrollStepForward:
      xoffset += distance;

      if ( xoffset > xoffset_end )
        xoffset = xoffset_end;

      if ( xoffset < 0 )
        xoffset = 0;

      break;

    case FScrollbar::scrollJump:
    {
      int value = hbar->getValue();
      scrollToX(value);
      break;
    }

    case FScrollbar::scrollWheelUp:
      if ( xoffset == 0 )
        break;

      xoffset -= pagesize;

      if ( xoffset < 0 )
        xoffset = 0;

      break;

    case FScrollbar::scrollWheelDown:
      if ( xoffset == xoffset_end )
        break;

      xoffset += pagesize;

      if ( xoffset > xoffset_end )
        xoffset = xoffset_end;

      break;
  }

  if ( isVisible() )
  {
    drawColumnLabels();
    drawList();
    updateTerminal();
    flush_out();
  }

  if ( scrollType >= FScrollbar::scrollStepBackward
    && scrollType <= FScrollbar::scrollWheelDown )
  {
    hbar->setValue (xoffset);

    if ( hbar->isVisible() && xoffset_before != xoffset )
      hbar->drawBar();

    updateTerminal();
    flush_out();
  }
}
