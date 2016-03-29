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
#include "core/saveadapter.hpp"

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

    if( event.EventType == sEventMouse && event.mouse.type == NEvent::Mouse::btnLeftPressed )
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
      _resizing(false), _selectedElement(0)
{
  #ifdef _DEBUG
      setDebugName( "PropertyWorkspace");
  #endif

  // we can't tab out of this window
  setTabgroup(true);

  setWindowFlag( fdraggable, true );
  // we can ctrl+tab to it
  setTabstop(true);
  // the tab order number is auto-assigned
  setTaborder(-1);

  // set window text
  setText("");
  button( buttonClose )->setVisible( false );

  _eventHandler = new WorkspaceEventHandler( *this, scene );

  //_createElementsTreeView();
  _createTabControl();
  _lastUpdateTime = 0;

  PushButton* btnClose = new PushButton( this, Rect( width() - 24, 0, width(), 24 ), "X" );
  CONNECT( btnClose, onClicked(), this, PropertyWorkspace::deleteLater )
}

void PropertyWorkspace::_createElementsTreeView()
{
 // _treeView = new TreeView( this, Rect(0,0,100,100), -1, true, true, true );
 // _treeView->setGeometry( Rect(12, 12, width()/2, height()-12));
}

void PropertyWorkspace::_createTabControl()
{
  /*_attribEditor = new PropertyBrowser( this );
  _attribEditor->setID( EGUIEDCE_ATTRIB_EDITOR );
  _attribEditor->setGeometry( Rect(13, 13, width()-13, height()-13) );
  */
  _editor = new Label( this, Rect(13, 13, width()-13, height()-13), "" );
  _editor->setFont( Font::create( "FONT_1" ) );
  _editor->setWordwrap( true );
}

void PropertyWorkspace::_update()
{
  /*_treeView->getRoot()->clearChildren();
  const OverlayList& overlays = _city->overlays();
  OverlayList forest;
  foreach( it, overlays )
  {
    if( (*it)->type() == object::tree )
      forest.push_back( *it );
  }

  TreeViewItem* item = _treeView->getRoot()->addChildBack( "Trees" );
  foreach( it, forest )
  {
    std::string text = utils::format( 0xff, "[%d,%d]%s", (*it)->pos().i(), (*it)->pos().j(), (*it)->name().c_str() );
    item->addChildBack( text, 0, -1, -1, (*it).object() );
  }*/
}

//! destructor
PropertyWorkspace::~PropertyWorkspace()
{
  // drop everything
  if( _eventHandler )
    _eventHandler->_finished = true;
}

scene::EventHandlerPtr PropertyWorkspace::handler() const{  return _eventHandler;}

void PropertyWorkspace::setCity(PlayerCityPtr city)
{
  _city = city;
  _update();
}

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

  for( auto i : children )
  {
    if( !i->isSubElement())
      _addChildrenToTree(i, newnode);
  }
}

void PropertyWorkspace::updateTree( Widget* elm )
{
  //_treeView->getRoot()->clearChildren();
  //_addChildrenToTree(elm, _treeView->getRoot());
  //_treeView->getRoot()->getFirstChild()->setExpanded(true);
}

void PropertyWorkspace::setSelectedElement(OverlayPtr sel)
{
  // save changes
  //_attribEditor->updateAttribs();
  VariantMap attribs;// = _attribEditor->getAttribs();

  //attribs.clear();
  _selectedElement = sel;

  // get the new attributes
  if (_selectedElement.isValid())
    _selectedElement->save(attribs);

  //_attribEditor->setAttribs( attribs );
  std::string text = config::save(attribs);
  _editor->setText( text );
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
