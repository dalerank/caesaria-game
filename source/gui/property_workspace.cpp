#include "property_workspace.hpp"
#include "core/hash.hpp"
#include "treeview_item.hpp"
#include "property_attribute.hpp"
#include "core/utils.hpp"
#include "core/event.hpp"
#include "gfx/camera.hpp"
#include "gfx/tile.hpp"
#include "city/city.hpp"
#include "objects/overlay.hpp"
#include "scene/level.hpp"
#include "environment.hpp"
#include "core/timer.hpp"
#include "core/logger.hpp"

using namespace gfx;

namespace gui
{

const unsigned int EGUIEDCE_ENV_EDITOR = Hash( "EGUIEDCE_ENV_EDITOR" );
const unsigned int EGUIEDCE_ATTRIB_EDITOR = Hash( "EGUIEDCE_ATTRIB_EDITOR" );

class WorkspaceEventHandler : public scene::EventHandler
{
public:
  PropertyWorkspace& _parent;
  scene::Level* _scene;
  bool _finished;
  WorkspaceEventHandler( PropertyWorkspace& parent, scene::Base* scene ) : _parent( parent )
  {
    _finished = false;
    _scene = safety_cast<scene::Level*>( scene );
  }

  void handleEvent(NEvent& event)
  {
    if( !_scene )
      return;   

    if( event.EventType == sEventMouse && event.mouse.type == mouseLbtDblClick )
    {
      Camera* camera = _scene->camera();
      Tile* tile = camera->at( event.mouse.pos(), false );

      if( _parent.ui()->hovered() != _parent.ui()->rootWidget() )
        return;

      if( tile != 0 && tile->overlay() != 0 )
      {
        _parent.setSelectedElement( tile->overlay() );
      }
    }
  }

  bool finished() const { return _finished; }
};

//! constructor
PropertyWorkspace::PropertyWorkspace( Widget* _parent, scene::Base* scene, const Rect& rectangle )
    : Window( _parent, rectangle, "", -1 ),
      _resizing(false), _selectedElement(0),
      _attribEditor(0)
{
	#ifdef _DEBUG
      setDebugName( "PropertyWorkspace");
	#endif

	// we can't tab out of this window
  setTabgroup(true);

  setWindowFlag( fdraggable, true );
	// we can ctrl+tab to it
	setTabStop(true);
	// the tab order number is auto-assigned
	setTabOrder(-1);

	// set window text
  setText("");
  button( buttonClose )->setVisible( false );

  _eventHandler = new WorkspaceEventHandler( *this, scene );

  _createElementsTreeView();
  _createTabControl();
  _lastUpdateTime = 0;

  PushButton* btnClose = new PushButton( this, Rect( width() - 20, 0, width(), 20 ), "X" );
  CONNECT( btnClose, onClicked(), this, PropertyWorkspace::deleteLater )
}

void PropertyWorkspace::_createElementsTreeView()
{
  _treeView = new TreeView( this, Rect(0,0,100,100), -1, true, true, true );
  _treeView->setGeometry( Rect(12, 12, width()/2, height()-12));
}

void PropertyWorkspace::_createTabControl()
{
  _attribEditor = new PropertyBrowser( this );
  _attribEditor->setID( EGUIEDCE_ATTRIB_EDITOR );  
  _attribEditor->setGeometry( Rect(width()/2, 12, width()-12, height()-12) );
}

//! destructor
PropertyWorkspace::~PropertyWorkspace()
{
	// drop everything
  if( _eventHandler )
    _eventHandler->_finished = true;
}

TreeView* PropertyWorkspace::getTreeView() const{	return _treeView;}
PropertyBrowser* PropertyWorkspace::getAttributeEditor() const{  return _attribEditor;}
scene::EventHandlerPtr PropertyWorkspace::handler() const{  return _eventHandler;}
void PropertyWorkspace::setCity(PlayerCityPtr city){  _city = city;}

TreeViewItem* PropertyWorkspace::_getTreeNode(OverlayPtr element, TreeViewItem* searchnode)
{
	TreeViewItem* child = searchnode->getFirstChild();
	while (child)
	{
    if (((Overlay*)child->getData()) == element.object() )
			return child;

		if (child->hasChildren())
		{
      TreeViewItem* foundnode = _getTreeNode(element, child);
			if (foundnode)
				return foundnode;
		}
		child = child->getNextSibling();
	}
	return 0;
}

void PropertyWorkspace::_addChildrenToTree( Widget* _parentElement, TreeViewItem* treenode)
{
  std::string name = utils::format( 0xff, "[id=%d,%s]", _parentElement->ID(), _parentElement->internalName().c_str() );

	TreeViewItem* newnode = treenode->addChildBack( name );
	newnode->setData((void*)_parentElement);
  const Widget::Widgets& children = _parentElement->children();

  foreach( i, children )
	{
		if( !(*i)->isSubElement())
      _addChildrenToTree(*i, newnode);
	}
}

void PropertyWorkspace::updateTree( Widget* elm )
{
	_treeView->getRoot()->clearChildren();
  _addChildrenToTree(elm, _treeView->getRoot());
  _treeView->getRoot()->getFirstChild()->setExpanded(true);
}

void PropertyWorkspace::setSelectedElement(OverlayPtr sel)
{
	// save changes
	_attribEditor->updateAttribs();
  TreeViewItem* elementTreeNode = _getTreeNode(sel, _treeView->getRoot());

	if (elementTreeNode)
	{
		elementTreeNode->setSelected(true);
		while (elementTreeNode)
		{
			elementTreeNode->setExpanded(true);
			elementTreeNode = elementTreeNode->getParentItem();
		}
	}

  VariantMap attribs = _attribEditor->getAttribs();

  /*if (_selectedElement.isValid() && sel != _selectedElement)
	{
		// deserialize attributes
		_selectedElement->load(Attribs);
  }*/
	// clear the attributes list
  attribs.clear();
	_selectedElement = sel;

	// get the new attributes
  if (_selectedElement.isValid())
    _selectedElement->save(attribs);

  _attribEditor->setAttribs( attribs );
  _treeView->getRoot()->addChildBack( sel->name(), 0, -1, -1, (void*)sel.object() );
}

//! draws the element and its children.
//! same code as for a window
void PropertyWorkspace::draw( gfx::Engine& painter )
{
  if (!visible())
		return;

  /*if( DebugTimer::ticks() - _lastUpdateTime > 10000 )
  {
    const OverlayList& ovs = _city->
  }
  Rect rect = absoluteRect();

  if( text().length() )
	{
    Font font = Font::create( FONT_2 );
    font.Draw( text(), rect, 0xff000000, false, true, &absoluteClippingRectRef() );
  }*/

  Window::draw( painter );
}


//! called if an event happened.
bool PropertyWorkspace::onEvent(const NEvent &event)
{
    if( event.EventType == AbstractAttribute::ATTRIBEDIT_ATTRIB_CHANGED
        && event.user.data2 == EGUIEDCE_ATTRIB_EDITOR )
    {
        // update selected items attributes
        /*if( _editWorkspace && _editWorkspace->GetSelectedElement() )
        {
            const VariantMap& attr = getAttributeEditor()->getAttribs();

            int index = attr->Find( L"InternalName" );
            if( index >= 0 )
            {
                core::String elmName = attr->getAttributeAsString( L"InternalName" );
                _editWorkspace->setElementName( _editWorkspace->GetSelectedElement(), false, elmName );
                attr->setAttribute( index, elmName );
            }

            _editWorkspace->GetSelectedElement()->load( attr );
            _editWorkspace->UpdateTree();
            _editWorkspace->Update();
        }*/
        return true;
     }

    switch(event.EventType)
	{    
  case sEventGui:
    switch(event.gui.type)
		{
    case guiTreeviewNodeSelect:
    {
        /*TreeViewItem* eventnode = ((TreeView*)event.gui.caller)->getLastEventNode();
        if(!eventnode->isRoot())
        {
            Overlay* elm = (Overlay*)eventnode->getData();
            setSelectedElement( elm );
        }
        break;*/
    }
    break;

		default:
			break;
		}

  break;

      default:
  break;
	}

  return Window::onEvent(event);
}

}
