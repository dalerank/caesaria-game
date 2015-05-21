#include "property_workspace.hpp"
#include "core/hash.hpp"
#include "treeview_item.hpp"
#include "core/utils.hpp"
#include "core/event.hpp"

namespace gui
{

const unsigned int EGUIEDCE_ENV_EDITOR = Hash( "EGUIEDCE_ENV_EDITOR" );
const unsigned int EGUIEDCE_ATTRIB_EDITOR = Hash( "EGUIEDCE_ATTRIB_EDITOR" );

//! constructor
PropertyWorkspace::PropertyWorkspace( Widget* _parent, const Rect& rectangle )
    : Window( _parent, rectangle, "", -1 ),
		  Dragging(false), IsDraggable(true), Resizing(false), _selectedElement(0),
		  _attribEditor(0)
{
	#ifdef _DEBUG
      setDebugName( "EditorWindow");
	#endif

	// we can't tab out of this window
  setTabgroup(true);

    setDraggable( false );
    setSubElement( true );
	// we can ctrl+tab to it
	setTabStop(true);
	// the tab order number is auto-assigned
	setTabOrder(-1);

	// set window text
  setText("");
  button( buttonClose )->setVisible( false );

  setMinSize( Size(200,200));

  _windowLayout = new VLayout( this, Rect( 0, 0, width(), height()), -1 );
  //_windowLayout->setDrawBorder( false );
  _windowLayout->setAlignment( align::upperLeft, align::lowerRight, align::upperLeft, align::lowerRight );

  createElementsTreeView_();
  createTabControl_();
}

void PropertyWorkspace::createElementsTreeView_()
{
    _treeView = new TreeView( _windowLayout, Rect(0,0,100,100), -1, true, true, true );
    _treeView->setSubElement( true );
    _treeView->setAlignment( align::upperLeft, align::lowerRight, align::upperLeft, align::lowerRight);
}

void PropertyWorkspace::createTabControl_()
{
    _attribEditor = new PropertyBrowser( _windowLayout );
    _attribEditor->setID( EGUIEDCE_ATTRIB_EDITOR );
    _attribEditor->setGeometry(RectF(0.0f, 0.0f, 1.0f, 1.0f));
    _attribEditor->setAlignment(align::upperLeft, align::lowerRight, align::upperLeft, align::lowerRight);
}


//! destructor
PropertyWorkspace::~PropertyWorkspace()
{
	// drop everything
}

TreeView* PropertyWorkspace::getTreeView() const
{
	return _treeView;
}

PropertyBrowser* PropertyWorkspace::getAttributeEditor() const
{
	return _attribEditor;
}

TreeViewItem* PropertyWorkspace::_GetTreeNode(Overlay* element, TreeViewItem* searchnode)
{
	TreeViewItem* child = searchnode->getFirstChild();
	while (child)
	{
    if (((Overlay*) child->getData()) == element)
			return child;

		if (child->hasChildren())
		{
			TreeViewItem* foundnode = _GetTreeNode(element, child);
			if (foundnode)
				return foundnode;
		}
		child = child->getNextSibling();
	}
	return 0;
}

void PropertyWorkspace::addChildrenToTree_( Widget* _parentElement, TreeViewItem* treenode)
{
  std::string name = utils::format( 0xff, "[id=%d,%s]", _parentElement->ID(), _parentElement->internalName().c_str() );

	TreeViewItem* newnode = treenode->addChildBack( name );
	newnode->setData((void*)_parentElement);
  const Widget::Widgets& children = _parentElement->children();

  for (ConstChildIterator i = children.begin(); i != children.end(); i++ )
	{
		if( !(*i)->isSubElement())
			addChildrenToTree_(*i, newnode);
	}
}

void PropertyWorkspace::updateTree( Widget* elm )
{
	_treeView->getRoot()->clearChildren();
	addChildrenToTree_(elm, _treeView->getRoot());
	_treeView->getRoot()->getFirstChild()->setExpanded(true);
}

void PropertyWorkspace::setSelectedElement(Overlay *sel)
{
	// save changes
	_attribEditor->updateAttribs();
	TreeViewItem* elementTreeNode = _GetTreeNode(sel, _treeView->getRoot());

	if (elementTreeNode)
	{
		elementTreeNode->setSelected(true);
		while (elementTreeNode)
		{
			elementTreeNode->setExpanded(true);
			elementTreeNode = elementTreeNode->getParentItem();
		}
	}

  VariantMap Attribs = _attribEditor->getAttribs();

	if (_selectedElement && sel != _selectedElement)
	{
		// deserialize attributes
		_selectedElement->load(Attribs);
	}
	// clear the attributes list
  Attribs.clear();
	_selectedElement = sel;

	// get the new attributes
	if (_selectedElement)
		_selectedElement->save(Attribs);

	_attribEditor->refreshAttribs();
}

//! draws the element and its children.
//! same code as for a window
void PropertyWorkspace::draw( gfx::Engine& painter )
{
  if (!visible())
		return;

  /*Rect rect = absoluteRect();

  if( text().length() )
	{
    Font font = Font::create( FONT_2 );
    font.Draw( text(), rect, 0xff000000, false, true, &absoluteClippingRectRef() );
  }*/

  Widget::draw( painter );
}


//! called if an event happened.
bool PropertyWorkspace::onEvent(const NEvent &event)
{
    if( event.EventType == AbstractAttribute::ATTRIBEDIT_ATTRIB_CHANGED
        && event.UserEvent.UserData1 == EGUIEDCE_ATTRIB_EDITOR )
    {
        // update selected items attributes
        if( _editWorkspace && _editWorkspace->GetSelectedElement() )
        {
            core::VariantArray* attr = getAttributeEditor()->getAttribs();

            s32 index = attr->Find( L"InternalName" );
            if( index >= 0 )
            {
                core::String elmName = attr->getAttributeAsString( L"InternalName" );
                _editWorkspace->setElementName( _editWorkspace->GetSelectedElement(), false, elmName );
                attr->setAttribute( index, elmName );
            }

            _editWorkspace->GetSelectedElement()->load( attr );
            _editWorkspace->UpdateTree();
            _editWorkspace->Update();
        }
        return true;
     }

    switch(event.EventType)
	{    
	case NRP_GUI_EVENT:
		switch(event.GuiEvent.EventType)
		{
		case NRP_ELEMENT_FOCUS_LOST:
		    if (event.GuiEvent.Caller == this)
		    {
			    Dragging = false;
			    Resizing = false;
		    }
		break;

        case NRP_TREEVIEW_NODE_SELECT:
        {
            TreeViewItem* eventnode = ((TreeView*)event.GuiEvent.Caller)->getLastEventNode();
            if(!eventnode->isRoot())
            {   
                Widget* elm = (Widget*)eventnode->getData();
                //setSelectedElement( elm );
                if( elm != _editWorkspace )
                    _editWorkspace->setSelectedElement( elm );
            }
            break;
        }
        break;

		default:
			break;
		}

		break;
	case NRP_MOUSE_EVENT:
		switch(event.MouseEvent.Event)
		{
		case NRP_LMOUSE_PRESSED_DOWN:
		{
			DragStart.X = event.MouseEvent.X;
			DragStart.Y = event.MouseEvent.Y;

			Widget* clickedElement = getElementFromPoint(DragStart);

			if (clickedElement == this)
			{
				Dragging = IsDraggable;
				//Environment->setFocus(this);
				bringToFront();
				return true;
			}
			break;
		}
		case NRP_LMOUSE_LEFT_UP:
			if (Dragging || Resizing)
			{
				Dragging = false;
				Resizing = false;
				return true;
			}
			break;
		case NRP_MOUSE_MOVED:
			if (Dragging || Resizing)
			{
				// gui window should not be dragged outside of its _parent
				RectI absRect = getParent()->getAbsoluteRect();

				if( event.MouseEvent.X < absRect.getLeft() + 1 
					|| event.MouseEvent.Y < absRect.getTop() + 1 
					|| event.MouseEvent.X > absRect.getRight() - 1 
					|| event.MouseEvent.Y > absRect.getBottom() - 1 )
						return true;

				Point diff( event.MouseEvent.getPosition() - DragStart );
				if (Dragging)
				{
					setPosition( diff );
					DragStart = event.MouseEvent.getPosition();
				}
				else if (Resizing)
				{
					core::Point dp = getUpperLeftPointA() + diff;
					setGeometry( core::RectI( getUpperLeftPointA(), dp));
					DragStart += dp - getRelativeRect().LowerRightCorner + diff;
				}

				return true;
			}
			break;
		default:
			break;
		}
	default:
		break;
	}

	return getParent()->onEvent(event);
}

bool PropertyWorkspace::isDraggable() const
{
	return IsDraggable;
}

void PropertyWorkspace::setDraggable(bool draggable)
{
	IsDraggable = draggable;

	if (Dragging && !IsDraggable)
		Dragging = false;
}

void PropertyWorkspace::SetEditWorkspace( EditWorkspace* editWorkspace )
{
    _editWorkspace = editWorkspace;
}

}
